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
* Description:  Connection opener.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include <CIMPSSAPSettings.h>
#include <ApSettingsHandlerUI.h>
#include <ApUtils.h>
#include <PEngWVPresenceErrors2.h>
#include <CPEngNWSessionSlotID2.h>
#include <MPEngTransactionStatus2.h>

#include "CCnUiConnOpener.h"
#include "IMPSPresenceLoginDetailValidator.h"

#include "MCnUiConnectionHandler.h"
#include "MCnUiClientPlugin.h"
#include "MCnUiOpProgressStateObserver.h"

#include "CnUiErrors.h"
#include "CnUiPanics.h"
#include "IMPSCommonUiDebugPrint.h"
#include "impscommonuibuilddefinitions.h"


// ===================== TOpenerStateProxy MEMBER FUNCTIONS ===================
// -----------------------------------------------------------------------------
// TOpenerStateProxy::TOpenerStateProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCnUiConnOpener::TOpenerStateProxy::TOpenerStateProxy( TCnUiConnOpenerState aInitial )
        : iState( aInitial ),
        iObs( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::TOpenerStateProxy::SetProgressObserver()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::TOpenerStateProxy::SetProgressObserver( MCnUiOpProgressStateObserver* aObs )
    {
    iObs = aObs;
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::TOpenerStateProxy::Set()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::TOpenerStateProxy::Set( TCnUiConnOpenerState aNewState,
                                              TIMPSConnectionClient aClient )
    {
    iState = aNewState;
    if ( iObs )
        {
        iObs->HandleProgressStateEnter( iState, aClient );
        }
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::TOpenerStateProxy::operator TCnUiConnOpenerState ()
// -----------------------------------------------------------------------------
//
TCnUiConnOpenerState CCnUiConnOpener::TOpenerStateProxy::operator()()
    {
    return iState;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCnUiConnOpener::CCnUiConnOpener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCnUiConnOpener::CCnUiConnOpener()
        : CActive( EPriorityStandard ),
        iState( ECnOpenerIdle )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiConnOpener* CCnUiConnOpener::NewL()
    {
    CCnUiConnOpener* self = new( ELeave ) CCnUiConnOpener;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCnUiConnOpener::~CCnUiConnOpener()
    {
    CancelLogin();
    delete iNWSessionSlotID;
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::CopyNWSessionSlotL()
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2* CCnUiConnOpener::CopyNWSessionSlotL( CPEngNWSessionSlotID2& aSessionSlot )
    {
    CPEngNWSessionSlotID2* tempSlot = aSessionSlot.CloneL();
    return tempSlot;
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::MakeLoginForClient()
// -----------------------------------------------------------------------------
//
TInt CCnUiConnOpener::MakeLoginForClient( TIMPSConnectionClient aClient,
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

    //issue SAP connection open
    TInt loginStatusCode = KErrNone;
    TRAP( loginStatusCode, HandleSapConnectionOpenRequestL( ETrue ) );
    if ( loginStatusCode == KErrNone )
        {
        //Connection open OK, wait steps to complete if needed
        WaitCompletion( iOpenWait );

        //Login completed.

        //else handle the real open status & possible cancels
        if ( iCanceled )
            {
            //if canceled, force the status be KErrCancel
            loginStatusCode = KErrCancel;
            }
        else
            {
            //else get the real status code
            loginStatusCode = iStatus.Int();
            }

        TBool connectionActive( ETrue );

        TInt ignore;
        TRAP( ignore, connectionActive =
                  iConnHandler->NwConnectionActiveL( aNWSessionSlotID ) );
        if ( ( !connectionActive ) &&
             ( loginStatusCode == KErrNone ) )
            {
            loginStatusCode = KErrCouldNotConnect;
            }

        if ( ( loginStatusCode == KErrNone ) ||
             ( loginStatusCode == KPEngNwErrPartiallySuccessful ) )
            {
            //Ignore ignorable partial errors
            loginStatusCode = KErrNone;

            //whole connection open went fine
            //==>client is now considered to be logged in
            TRAPD( err, iConnHandler->LoginTheClientL( iClient ) );
            loginStatusCode = err;
            }


        //If operation was canceled or some error occured during
        //the opening ==> the real network connection opened by this opener
        //must be closed.
        if ( loginStatusCode != KErrNone )
            {
            //Closes the owned connection & sets the client logged out
            UndoConnectionStateChanges();
            }
        }


    ResetCachedParameters();
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnOpener::MakeLoginForClient() loginStatusCode=%d" ), loginStatusCode );
    return loginStatusCode;
    }



// -----------------------------------------------------------------------------
// CCnUiConnOpener::CancelLogin()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::CancelLogin()
    {
    //cancel the operation step currently running...
    switch ( iState() )
        {
        case ECnOpenerOpeningSAPConnection:
            {
            //opening the SAP connection
            if ( iConnHandler )
                {
                iConnHandler->CancelSapConnectionOpen( *iNWSessionSlotID );
                }
            iCanceled = ETrue;
            break;
            }

        case ECnOpenerProcessingPlugin:
            {
            if ( iPlugin )
                {
                iPlugin->CancelPostLoginProsessing();
                }
            iCanceled = ETrue;
            break;
            }

        //flow through
        case ECnOpenerClosingInterferingSAPConnection:
        case ECnOpenerClosingOwnedSAPConnection:
            {
            //closing SAP connection
            if ( iConnHandler )
                {
                iConnHandler->CancelSapConnectionOpen( *iNWSessionSlotID );
                }

            // Waiting connection to close.
            WaitCompletion( iOpenWait );

            iCanceled = ETrue;
            break;
            }

        case ECnOpenerIdle:
        default:
            {
            //nothing to do
            break;
            }
        }

    //break away from scheduler loop...
    Completed( iOpenWait );
    }




// -----------------------------------------------------------------------------
// CCnUiConnOpener::WaitCompletion()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::WaitCompletion( CActiveSchedulerWait& aWaiter )
    {
    //wait with the scheduler loop
    IMPSCUI_DP_TXT( "CCnUiConnOpener::WaitCompletion() - starting wait" );
    if ( !aWaiter.IsStarted() )
        {
        aWaiter.Start();
        }
    IMPSCUI_DP_TXT( "CCnUiConnOpener::WaitCompletion() - wait done" );
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::Completed()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::Completed( CActiveSchedulerWait& aWaiter )
    {
    //break away from the waiting scheduler loop
    if ( aWaiter.IsStarted() )
        {
        aWaiter.AsyncStop();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::HandleSapConnectionOpenRequestL()
// State handler.
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::HandleSapConnectionOpenRequestL( TBool aCloseOfInterferingConnectionAllowed )
    {
    //New SAP connection needed

    //are compulsory SAP details ok for login?
    TInt compulsoryStatus = CheckConnOpenCompulsorySettingsL( *iSap );
    if ( compulsoryStatus != KErrNone )
        {
        User::Leave( compulsoryStatus );
        return;
        }


    //switch the operation according current SAP connection status
    TCnUiSapCnStatus sapCnStatus = iConnHandler->SapConnectionStatusL( *iSap, iClient );
    switch ( sapCnStatus )
        {
        case ECnUiSCS_NotConnected:
            {
            //no connected sap, notify plug-in
            iPlugin->ConnectionOpenInitL( EFalse, *iSap );

            //and issue real connection open
            IssueConnectionOpenL();
            break;
            }

        case ECnUiSCS_SapConnected:
            {
            //SAP already connected, notify plug-in
            iPlugin->ConnectionOpenInitL( ETrue, *iSap );

            //and progress directly to next state..
            HandleSapConnectionOpenedL();
            break;
            }


        case ECnUiSCS_SapConnected_PwdMissMatch:
            {
            //SAP already connected
            //However, session join password not equal to original session password
            User::Leave( KPEngNwErrInvalidPassword );
            break;
            }


        case ECnUiSCS_AnotherSapConnected:
            {
            //there is a conflicting SAP connection by some known client
            User::Leave( KCnUiErrorAlreadyConnectedToAnotherSap );
            break;
            }


        //Flow trough
        case ECnUiSCS_AnotherSapConnected_ClientsNotKnown:
        default:
            {
            //there is a conflicting SAP connection by unknow client
            //==>close it if allowed
            if ( aCloseOfInterferingConnectionAllowed )
                {
                IssueCloseOfInterferingSapConnectionL();
                }
            else
                {
                User::Leave( KErrAlreadyExists );
                }

            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::IssueConnectionOpenL()
// Transition to next state.
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::IssueConnectionOpenL()
    {
    //progress to next state by issuing the
    //connection open
    iState.Set( ECnOpenerOpeningSAPConnection, iClient );

    iConnHandler->OpenSapConnectionL( *iSap, *iNWSessionSlotID, *this );
    iOwnsSapConnection = ETrue;
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::HandleSapConnectionOpenedL()
// State handler.
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::HandleSapConnectionOpenedL()
    {
    //SAP connection succesfully open
    //let client plug-in to verify NW server capabilities
    if ( iPlugin->VerifySapCapabilitiesL() )
        {
        //Server capabilities OK for client.
        //progress to next state by issuing the client
        //plug-in to do its processing
        IssuePluginPostLoginProcessingL();

        //Client is considered to be logged in just after the plug-in
        //has performed it steps
        }

    else
        {
        //server capabilities NOT OK for client
        iStatus = KCnUiErrorInsufficientNWServerCapabilities;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::IssuePluginPostLoginProcessingL()
// Transition to next state.
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::IssuePluginPostLoginProcessingL()
    {
    IMPSCUI_DP_FUNC_ENTER( "CCnUiConnOpener::IssuePluginPostLoginProcessingL()" );

    //initialize the status
    iStatus = KErrNone;

    //and issue the post login processing
    iPlugin->DoPostLoginProsessingL( iStatus );
    if ( iStatus == KRequestPending )
        {
        //plug-in started a real asynchronous operation
        //==> wait it to complete
        SetActive();
        IMPSCUI_DP_TXT( "CCnUiConnOpener::IssuePluginPostLoginProcessingL() set active" );
        }

    // progress to next state by issuing the client
    // plug-in processing... this is moved here, because it initiates the
    // showing of the Login ok -note.
    iState.Set( ECnOpenerProcessingPlugin, iClient );
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::UndoConnectionStateChanges()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::UndoConnectionStateChanges()
    {
    // inform client plugin, that creating a connection
    // was not completed. If this fails, there's nothing we can do
    TRAPD( ignore, iPlugin->LoginCancelledL() );

    //Undo first the possible owned NW connection
    //If the connection close fails, there isn't much to do.
    //Actually this should be PEC engine service to
    //"emergency close" the network connection when needed.
    TRAP( ignore, IssueSapConnectionCloseIfNeededL() );

    if ( iState() == ECnOpenerClosingOwnedSAPConnection )
        {
        //some processing steps started, wait here with scheduler loop
        WaitCompletion( iOpenWait );
        }
    else
        {
        //no owned NW connection to close ==> no logout event is sent for client
        //==>force sent one
        TBool networkActive( EFalse );
        TInt ignore;
        TRAP( ignore, networkActive =
                  iConnHandler->NwConnectionActiveL( *iNWSessionSlotID ) );
        if ( !networkActive )
            {
            TInt ignore;
            TRAP( ignore, iConnHandler->LogoutTheClientL( iClient ) );
            }
        }
    }



// -----------------------------------------------------------------------------
// CCnUiConnOpener::IssueSapConnectionCloseIfNeededL()
// Transition to next state.
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::IssueSapConnectionCloseIfNeededL()
    {
    //Login operation has failed at some step
    //Close network connection if the connection open
    //was issued by this connection opener & the PEC engine is online...
    if ( iOwnsSapConnection && iConnHandler->NwConnectionActiveL( *iNWSessionSlotID ) )
        {
        //progress to next state by issuing the
        //connection close
        iState.Set( ECnOpenerClosingOwnedSAPConnection, iClient );

        iConnHandler->CloseSapConnectionL( *iNWSessionSlotID, *this );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::IssueCloseOfInterferingSapConnectionL()
// Transition to next state.
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::IssueCloseOfInterferingSapConnectionL()
    {
    //progress to next state by issuing the connection close
    iState.Set( ECnOpenerClosingInterferingSAPConnection, iClient );

    iConnHandler->CloseSapConnectionL( *iNWSessionSlotID, *this );
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::ResetCachedParameters()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::ResetCachedParameters()
    {
    //cache parameters to be later accessible
    iState.SetProgressObserver( NULL );

    iConnHandler = NULL;
    iPlugin = NULL;
    iSap = NULL;
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::ResetStateFlags()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::ResetStateFlags()
    {
    ///set state to idle
    iState.Set( ECnOpenerIdle, iClient );
    iOwnsSapConnection = EFalse;
    iCanceled = EFalse;

    //initially there hasn't happened any errors
    iStatus = KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::CheckConnOpenCompulsorySettingsL()
// -----------------------------------------------------------------------------
//
TInt CCnUiConnOpener::CheckConnOpenCompulsorySettingsL( const CIMPSSAPSettings& aSap )
    {
    //check that SAP has an URL that can used in connection
    if ( aSap.SAPAddress().Length() == 0 )
        {
        return KCnUiErrorSapMissingCompulsoryFields;
        }

    //check that SAP has proper internet access point
    if ( !IMPSPresenceLoginDetailValidator::ValidWVLoginIAPL( aSap.AccessPoint() ) )
        {
        //SAP doesn't have a proper internet access
        //point defined in it.
        return KCnUiErrorSapMissingCompulsoryFields;
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::HandleNWSessionOperationCompleteL()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::HandleNWSessionOperationCompleteL(
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
// CCnUiConnOpener::HandleNWSessionOperationNotifyError()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::HandleNWSessionOperationNotifyError(
    TInt aError,
    CPEngNWSessionSlot2& /* aNWSessionSlot */,
    TInt /* aNWSessionSlotOperation */ )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnOpener::HandleNWSessionOperationCompleteL [%d]" ), aError );

    //HandleNWSessionOperationCompleteL() has left. Stop the processing & store leave code.
    Completed( iOpenWait );
    iStatus = aError;
    }


// -----------------------------------------------------------------------------
// CCnUiConnOpener::RunL()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::RunL()
    {
    HandleOperationCompletionsL();
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::DoCancel()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::DoCancel()
    {
    CancelLogin();
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::RunError()
// -----------------------------------------------------------------------------
//
TInt CCnUiConnOpener::RunError( TInt aError )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnOpener::HandleNWSessionOperationCompleteL [%d]" ), aError );

    //HandleNWSessionOperationCompleteL() has left. Stop the processing & store leave code.
    Completed( iOpenWait );
    iStatus = aError;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::Cancel()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::Cancel()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::IsActive()
// -----------------------------------------------------------------------------
//
TBool CCnUiConnOpener::IsActive() const
    {
    return CActive::IsActive();
    }

// -----------------------------------------------------------------------------
// CCnUiConnOpener::HandleOperationCompletionsL()
// -----------------------------------------------------------------------------
//
void CCnUiConnOpener::HandleOperationCompletionsL()
    {
    IMPSCUI_DP_TXT( "CCnUiConnOpener::HandleOperationCompletionsL()" );

    switch ( iState() )
        {
        case ECnOpenerOpeningSAPConnection:
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnOpener::OperationCompleteL - state=ECnOpenerOpeningSAPConnection, status=%d" ), iStatus );

            if ( iStatus == KErrNone )
                {
                //SAP connection opened successfully
                //handling will issue next step if needed.

                //possible opening errors & errors coming from
                //handling connection open, are handled from
                //outside of the wait loop
                HandleSapConnectionOpenedL();
                }
            break;
            }


        case ECnOpenerProcessingPlugin:
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnOpener::OperationCompleteL -  state=ECnOpenerProcessingPlugin, status=%d" ), iStatus );
            //plug-in processing completed, no more steps to do.
            //however, if plug-in failed, the possible errors
            //are handled from outside of the wait loop
            break;
            }

        case ECnOpenerClosingOwnedSAPConnection:
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnOpener::OperationCompleteL -  state=ECnOpenerClosingOwnedSAPConnection, status=%d" ), iStatus );
            //There has been a error somewhere
            //and now the opened SAP connection
            //is closed, no more to do

            // Login canceled: wait for PEC is complete -> we can continue
            Completed( iOpenWait );

            //
            // must do the transition to prevent uncompleted waits
            iState.Set( ECnOpenerIdle, iClient );
            break;
            }

        case ECnOpenerClosingInterferingSAPConnection:
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnOpener::OperationCompleteL -  state=ECnOpenerClosingInterferingSAPConnection, status=%d" ), iStatusInt );
            //Interfering SAP connection is closed.
            //Connection close is assumed to be succesful,
            //and state is advanced to next by issuing the SAP connection open
            //However, the interfering connection close handling isn't allowed
            //to cause looping...
            HandleSapConnectionOpenRequestL( EFalse );
            break;
            }

        //Flow trough...
        case ECnOpenerIdle:
        default:
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnOpener::OperationCompleteL -  state=ECnOpenerIdle, status=%d" ), iStatus );
            //not allowed state completed
            CnUiPanicOrLeaveL( EIMPSConnOpenerUnknownState, KErrUnknown );
            break;
            }
        }

    if ( !IsActive() )
        {
        //if no subsequent processing steps started,
        //break away from scheduler loop...
        Completed( iOpenWait );
        }
    }

//  End of File


