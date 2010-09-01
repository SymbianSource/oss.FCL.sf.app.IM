/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Connection closer.
*
*/


// INCLUDE FILES
#include "E32Std.h"
#include <CIMPSSAPSettings.h>
#include <ApSettingsHandlerUI.h>
#include <ApUtils.h>
#include <CPEngNWSessionSlotID2.h>
#include <MPEngTransactionStatus2.h>

#include "CCnUiConnCloser.h"

#include "MCnUiConnectionHandler.h"
#include "MCnUiClientPlugin.h"
#include "MCnUiOpProgressStateObserver.h"

#include "CnUiErrors.h"
#include "CnUiPanics.h"
#include "IMPSCommonUiDebugPrint.h"



// ===================== TCloserStateProxy MEMBER FUNCTIONS ===================
// -----------------------------------------------------------------------------
// CCnUiConnCloser::TCloserStateProxy::TCloserStateProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCnUiConnCloser::TCloserStateProxy::TCloserStateProxy( TCnUiConnCloserState aInitial )
        : iState( aInitial ),
        iObs( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CCnUiConnCloser::TCloserStateProxy::SetProgressObserver
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::TCloserStateProxy::SetProgressObserver( MCnUiOpProgressStateObserver* aObs )
    {
    iObs = aObs;
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::TCloserStateProxy::Set()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::TCloserStateProxy::Set( TCnUiConnCloserState aNewState,
                                              TIMPSConnectionClient aClient )
    {
    iState = aNewState;
    if ( iObs )
        {
        iObs->HandleProgressStateEnter( iState, aClient );
        }
    }

// -----------------------------------------------------------------------------
// CCnUiConnCloser::TCloserStateProxy::operator TCnUiConnCloserState ()
// -----------------------------------------------------------------------------
//
TCnUiConnCloserState CCnUiConnCloser::TCloserStateProxy::operator()()
    {
    return iState;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCnUiConnCloser::CCnUiConnCloser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCnUiConnCloser::CCnUiConnCloser()
        : CActive( EPriorityStandard ),
        iState( ECnCloser_Idle )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCnUiConnCloser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiConnCloser* CCnUiConnCloser::NewL()
    {
    CCnUiConnCloser* self = new( ELeave ) CCnUiConnCloser;
    return self;
    }

// Destructor
CCnUiConnCloser::~CCnUiConnCloser()
    {
    CancelLogout();
    delete iNWSessionSlotID;
    }

// -----------------------------------------------------------------------------
// CCnUiConnCloser::CopyNWSessionSlotL()
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2* CCnUiConnCloser::CopyNWSessionSlotL( CPEngNWSessionSlotID2& aSessionSlot )
    {
    CPEngNWSessionSlotID2* tempSlot = aSessionSlot.CloneL();
    return tempSlot;
    }

// -----------------------------------------------------------------------------
// CCnUiConnCloser::MakeLogoutForClient()
// -----------------------------------------------------------------------------
//
TInt CCnUiConnCloser::MakeLogoutForClient( TIMPSConnectionClient aClient,
                                           const CIMPSSAPSettings& aSap,
                                           MCnUiClientPlugin& aPlugin,
                                           MCnUiConnectionHandler& aConnHandler,
                                           MCnUiOpProgressStateObserver* aProgressObserver,
                                           CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngNWSessionSlotID2* tempSlot = NULL;
    TRAPD( error, tempSlot = CopyNWSessionSlotL( aNWSessionSlotID ) );
    if ( error )
        {
        return error;
        }
    delete iNWSessionSlotID;
    iNWSessionSlotID = tempSlot;

    ResetCachedParameters();
    ResetStateFlags();

    //cache parameters to be later accessible
    iState.SetProgressObserver( aProgressObserver );
    iClient = aClient;
    iSap = &aSap;
    iPlugin = &aPlugin;
    iConnHandler = &aConnHandler;


    //issue SAP connection close
    TInt logoutStatusCode = KErrNone;
    TRAP( logoutStatusCode, HandleClientLoggoutRequestL() );
    if ( logoutStatusCode == KErrNone )
        {
        //Connection close request OK, wait steps to complete if needed

        //some processing steps started, wait here with scheduler loop
        WaitCompletion();
        // setting to null, this actually cannot leave so ignoring the leave code
        TInt ignore;
        TRAP( ignore, iConnHandler->SetSessionSlotL( NULL ) );

        //Logout completed.
        TBool clientLoggedIn = ETrue;
        TRAPD( err, clientLoggedIn = aConnHandler.TheClientLoggedInL( aClient ) )
        if ( err != KErrNone )
            {
            //logout status retrieve failed, return it as error
            logoutStatusCode = err;
            }

        else if ( !clientLoggedIn )
            {
            //If the tried client isn't anymore logged in ==> logout was succesful
            //despite what so ever error has been reported
            logoutStatusCode = KErrNone;
            }

        else
            {
            //In other cases report the errors as they appear
            if ( iCanceled )
                {
                //if canceled, force the status be KErrCancel
                logoutStatusCode = KErrCancel;
                }
            else
                {
                logoutStatusCode = iStatus.Int();
                }
            }
        }

    ResetCachedParameters();
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnCloser::MakeLogoutForClient() logoutStatus=%d" ), logoutStatusCode );
    return logoutStatusCode;
    }



// -----------------------------------------------------------------------------
// CCnUiConnCloser::CancelLogout()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::CancelLogout()
    {
    if ( IsActive() )
        {
        CActive::Cancel();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::ConnectionReallyClosed()
// -----------------------------------------------------------------------------
//
TBool CCnUiConnCloser::ConnectionReallyClosed()
    {
    return iConnectionReallyClosed;
    }



// -----------------------------------------------------------------------------
// CCnUiConnCloser::WaitCompletion()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::WaitCompletion()
    {
    //wait with the scheduler loop
    IMPSCUI_DP_TXT( "CCnUiConnCloser::WaitCompletion() - starting wait" );
    if ( !iWait.IsStarted() )
        {
        // Code scanner warning "active object called without checking
        // whether it is active or cancelling it first" ignored because
        // CActiveSchedulerWait is not an active object
        iWait.Start(); // CSI: 10 # See above
        }
    IMPSCUI_DP_TXT( "CCnUiConnCloser::WaitCompletion() - wait done" );
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::Completed()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::Completed()
    {
    //break away from the waiting scheduler loop
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::HandleClientLoggoutRequestL()
// State handler.
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::HandleClientLoggoutRequestL()
    {
    //logout for client is needed
    if ( iConnHandler->TheClientLoggedInL( iClient ) )
        {
        //client is logged in, notify plug-in about conn close
        iPlugin->ConnectionCloseInitL( *iSap );

        //and issue the plug-in to do its stuff
        IssuePluginPreLoggoutProcessingL();
        }

    //if client isn't logged in ==> all ok from logout point of view
    }



// -----------------------------------------------------------------------------
// CCnUiConnCloser::IssuePluginPreLoggoutProcessingL()
// Transition to next state.
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::IssuePluginPreLoggoutProcessingL()
    {
    //progress to next state by issuing the client
    //plug-in processing...

    //initialize the request status & issue plug-in processing
    iStatus = KErrNone;

    // logout has to succeed no matter what
    TRAPD( err, iPlugin->DoPreLogoutProsessingL( iStatus ) );

    if ( ( iStatus == KRequestPending ) && ( err == KErrNone ) )
        {
        //plug-in started a real asynchronous operation
        //==> wait it to complete

        iState.Set( ECnCloser_ProcessingPlugin, iClient );
        SetActive();
        }

    else
        {
        //plug-in didn't start any asynchronous operation
        //progress directly to connection close
        IssueSapConnectionCloseL();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::IssueSapConnectionCloseL()
// Transition to next state.
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::IssueSapConnectionCloseL()
    {
    //progress to next state by issuing the real
    //connection close if needed
    iConnHandler->CloseSapConnectionL( *iNWSessionSlotID, *this );
    iState.Set( ECnCloser_ClosingSAPConnection, iClient );

    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::ResetCachedParameters()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::ResetCachedParameters()
    {
    //cache parameters to be later accessible
    iState.SetProgressObserver( NULL );
    iConnHandler = NULL;
    iPlugin = NULL;
    iSap = NULL;
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::ResetStateFlags()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::ResetStateFlags()
    {
    ///set state to idle
    iState.Set( ECnCloser_Idle, iClient );
    iCanceled = EFalse;
    iConnectionReallyClosed = EFalse;

    //initially there hasn't happened any errors
    iStatus = KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::HandleNWSessionOperationCompleteL()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::HandleNWSessionOperationCompleteL(
    MPEngTransactionStatus2& aStatus,
    CPEngNWSessionSlot2& /* aNWSessionSlot */,
    TInt /* aNWSessionSlotOperation */ )
    {
    IMPSCUI_DP_TXT( "CCnUiConnOpener::HandleNWSessionOperationCompleteL()" );
    iTransactionStatus = &aStatus;
    iStatus = aStatus.Status();
    HandleOperationCompletionsL();
    }

// -----------------------------------------------------------------------------
// CCnUiConnCloser::HandleNWSessionOperationCompleteL()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::HandleNWSessionOperationNotifyError(
    TInt aError,
    CPEngNWSessionSlot2& /* aNWSessionSlot */,
    TInt /* aNWSessionSlotOperation */ )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnCloser::HandleNWSessionOperationNotifyError [%d]" ), aError );

    //RunL() has leaved. Stop the processing & store leave code.
    Completed();
    iStatus = aError;
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::RunL()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::RunL()
    {
    HandleOperationCompletionsL();
    }

// -----------------------------------------------------------------------------
// CCnUiConnCloser::DoCancel()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::DoCancel()
    {
    //cancel the operation step currently running...
    switch ( iState() )
        {
        case ECnCloser_ProcessingPlugin:
            {
            iPlugin->CancelPreLogoutProsessing();
            iCanceled = ETrue;
            break;
            }

        case ECnCloser_ClosingSAPConnection:
            {
            //closing the SAP connection
            iConnHandler->CancelSapConnectionClose( *iNWSessionSlotID );
            iCanceled = ETrue;
            break;
            }

        //flow through
        case ECnCloser_Idle:
        default:
            {
            //nothing to do
            break;
            }
        }

    //break away from scheduler loop...
    Completed();
    }

// -----------------------------------------------------------------------------
// CCnUiConnCloser::RunError()
// -----------------------------------------------------------------------------
//
TInt CCnUiConnCloser::RunError( TInt aError )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnCloser::HandleNWSessionOperationCompleteL [%d]" ), aError );

    //HandleNWSessionOperationCompleteL() has left. Stop the processing & store leave code.
    Completed();
    iStatus = aError;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiConnCloser::HandleOperationCompletionsL()
// -----------------------------------------------------------------------------
//
void CCnUiConnCloser::HandleOperationCompletionsL()
    {
    IMPSCUI_DP_TXT( "CCnUiConnCloser::HandleOperationCompletionsL" );

    switch ( iState() )
        {
        case ECnCloser_ProcessingPlugin:
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "HandleNWSessionOperationCompleteL - state=ECnCloser_ProcessingPlugin, status=%d" ), iStatus );

            //ignore possible errors propagating from plug-in logout processing
            //(logout _can't_ fail to some plug-in stuff)

            iStatus = KErrNone;
            IssueSapConnectionCloseL();
            break;
            }


        case ECnCloser_ClosingSAPConnection:
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "HandleNWSessionOperationCompleteL - state=ECnCloser_ClosingSAPConnection, status=%d" ), iStatus );
            //connection close completed, no more steps to do.

            //if connection close failed, the possible
            //errors are handled from outside of the wait loop

            if ( iStatus == KErrNone )
                {
                //connection was closed really
                //store it so it will be available to external clients
                iConnectionReallyClosed  = ETrue;

                //set also the current client as logged out
                if ( iConnHandler )
                    {
                    iConnHandler->LogoutTheClientL( iClient );
                    }
                Completed();
                }

            break;
            }


        //Flow trough...
        case ECnCloser_Idle:
        default:
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "HandleNWSessionOperationCompleteL - state=ECnCloser_Idle, status=%d" ), iStatus );
            //not allowed state completed
            CnUiPanicOrLeaveL( EIMPSConnCloserUnknownState, KErrUnknown );
            break;
            }
        }
    }
//  End of File
