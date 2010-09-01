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
* Description:  Network connection restore UI control.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include "CCnUiReconnectAllUiCntrlStep.h"

#include "MCnUiUiControlContext.h"
#include "MCnUiUiFacade.h"
#include "MCnUiSignaller.h"
#include "MCnUiConnectionHandler.h"
#include "MCnUiClientPlugin.h"
#include "CCnUiConnOpener.h"


#include "CnUiErrors.h"



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiReconnectAllUiCntrlStep* CCnUiReconnectAllUiCntrlStep::NewLC(
    MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLoginSap,
    TIMPSConnectionClient aReconnectedClient,
    CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiReconnectAllUiCntrlStep* self =
        new ( ELeave ) CCnUiReconnectAllUiCntrlStep(
        aCCntxt,
        aLoginSap,
        aReconnectedClient,
        aNWSessionSlotID );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CCnUiReconnectAllUiCntrlStep::~CCnUiReconnectAllUiCntrlStep()
    {
    iClientsToReConn.Reset();
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiReconnectAllUiCntrlStep::CCnUiReconnectAllUiCntrlStep(
    MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLoginSap,
    TIMPSConnectionClient aReconnectedClient,
    CPEngNWSessionSlotID2& aNWSessionSlotID )
        : CCnUiLoginUiCntrlStep( aCCntxt, aLoginSap, aNWSessionSlotID, aReconnectedClient ),
        iRequestedReconnClient( aReconnectedClient ), iNWSessionSlotID( aNWSessionSlotID )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiReconnectAllUiCntrlStep::ConstructL()
    {
    CCnUiLoginUiCntrlStep::ConstructL();
    }


// -----------------------------------------------------------------------------
// CCnUiReconnectAllUiCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiReconnectAllUiCntrlStep::HandleCompleteL()
    {
    //if there has been a client to reconnect, it has been
    //the first in the count array ==> index 0

    if ( iClientsToReConn.Count() > 0 )
        {
        iClientsToReConn.Remove( 0 );
        }


    //If there is more clients to reconnect, do so
    if ( iClientsToReConn.Count() > 0 )
        {
        //there is still clients to reconnect
        return ECnUiRestartStep;
        }
    else
        {
        //all done
        return ECnUiStepContinueTeardown;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiReconnectAllUiCntrlStep::UndoStepL()
// -----------------------------------------------------------------------------
//
void CCnUiReconnectAllUiCntrlStep::UndoStepL()
    {
    //nothing to undo
    }


// -----------------------------------------------------------------------------
// CCnUiReconnectAllUiCntrlStep::DoInitLoginStepL()
// Template method
// -----------------------------------------------------------------------------
//
TInt CCnUiReconnectAllUiCntrlStep::DoInitLoginStepL( TIMPSConnectionClient& aClient )
    {
    if ( !iInitDone )
        {
        //Login and thus also reconnects are connection owning so they have
        //to always check for simultaneous operations
        if ( iCCntxt.Signaller().OperationRunning() )
            {
            ShowSimultaneousLoginOperationNoteL();
            return KCnUiErrorLoginOperationAlreadyInUse;
            }


        //initialize the reconnection array
        InitNWReConnectL();
        iInitDone = ETrue;
        if ( iClientsToReConn.Count() == 0 )
            {
            //there isn't any clients to reconnect
            return KCnUiErrorNoClientsToReconnect;
            }
        }

    //in all other rounds prepare the client again,
    //to cope from possible sub steps.
    if ( iClientsToReConn.Count() > 0 )
        {
        aClient = iClientsToReConn[ 0 ];
        return ECnUiLoginInitContinue;
        }

    else
        {
        //no more clients to logout
        return KErrNone; //breaks away from RunStepL() with KErrNone
        }
    }



// -----------------------------------------------------------------------------
// CCnUiReconnectAllUiCntrlStep::DoShowLoginStateSpecificWaitnoteL()
// -----------------------------------------------------------------------------
//
void CCnUiReconnectAllUiCntrlStep::DoShowLoginStateSpecificWaitnoteL(
    TInt aStateId,
    TIMPSConnectionClient aClient )
    {
    switch ( aStateId )
        {
        case ECnOpenerOpeningSAPConnection:
        case ECnOpenerProcessingPlugin:
        case ECnOpenerClosingInterferingSAPConnection:
            {
            if ( !iWaitNote )
                {
                //the same wait note is used
                //both when opening SAP connection &
                //processing plug-in stuff
                if ( aClient == EIMPSConnClientIM )
                    {
                    iWaitNote = iCCntxt.Ui().WaitNoteL( ECnUiWaitNoteReConnectingChat,
                                                        iLoginSap.SAPName(),
                                                        this );
                    }

                else
                    {
                    //client is considered to be PEC
                    iWaitNote = iCCntxt.Ui().WaitNoteL( ECnUiWaitNoteReConnectingPEC,
                                                        iLoginSap.SAPName(),
                                                        this );
                    }
                }

            break;
            }

        case ECnOpenerIdle:
        case ECnOpenerClosingOwnedSAPConnection:
        default:
            {
            //clear any existing wait note set by the previous
            //progress steps. No wait note used during these steps.
            //own waitnote container used ==> direct deletion ok.
            delete iWaitNote;
            iWaitNote = NULL;

            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCnUiReconnectAllUiCntrlStep::InitNWReConnectL()
// -----------------------------------------------------------------------------
//
void CCnUiReconnectAllUiCntrlStep::InitNWReConnectL()
    {
    //internal count array
    iClientsToReConn.Reset();

    //determine is reconnection needed for given clients
    //UI specification states that only those clients which
    //connection mode settings is automatic are reconnected back

    MCnUiClientPlugin& clientPlugin = iCCntxt.ClientPluginL( iRequestedReconnClient,
                                                             iNWSessionSlotID );
    if ( clientPlugin.CurrentConnectionModeSettingL() == ECnUiCMAutomatic )
        {
        User::LeaveIfError( iClientsToReConn.Append( iRequestedReconnClient ) );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiReconnectAllUiCntrlStep::ShowSimultaneousLoginOperationNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiReconnectAllUiCntrlStep::ShowSimultaneousLoginOperationNoteL()
    {
    HBufC* serverName = NULL;
    iCCntxt.Signaller().OperationDetailsL( serverName );

    //OperationDetailsL() returns the HBufC ownership
    CleanupStack::PushL( serverName );
    iCCntxt.Ui().ShowNoteL( ECnUiConnOperationAllreadyRunning, *serverName );
    CleanupStack::PopAndDestroy( serverName ); //serverName
    }



//  End of File
