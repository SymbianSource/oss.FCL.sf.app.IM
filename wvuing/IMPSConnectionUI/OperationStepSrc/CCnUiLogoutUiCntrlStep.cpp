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
* Description:  Logout UI control.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsStore.h>
#include <CPEngNWSessionSlotID2.h>

#include "CCnUiLogoutUiCntrlStep.h"
#include "CCnUiConnCloser.h"
#include "CnUiSapDataPacker.h"

#include "MCnUiUiControlContext.h"
#include "MCnUiUiFacade.h"
#include "MCnUiSignaller.h"
#include "MCnUiConnectionHandler.h"
#include "MCnUiConnModeHandler.h"
#include "MCnUiClientPlugin.h"
#include "MCnUiConnModeRewaker.h"

#include    <centralrepository.h>
#include "CnUiErrors.h"
#include <e32property.h>


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiLogoutUiCntrlStep* CCnUiLogoutUiCntrlStep::NewLC(
    MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLogoutSap,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiLogoutUiCntrlStep* self = new ( ELeave ) CCnUiLogoutUiCntrlStep( aCCntxt, aLogoutSap );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    return self;
    }


// Destructor
CCnUiLogoutUiCntrlStep::~CCnUiLogoutUiCntrlStep()
    {
    delete iConnCloser;
    delete iWaitNote;
    delete iNWSessionSlotID;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiLogoutUiCntrlStep::CCnUiLogoutUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                                                CIMPSSAPSettings& aLogoutSap )
        : iCCntxt( aCCntxt ),
        iLogoutSap( aLogoutSap )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiLogoutUiCntrlStep::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iConnCloser = CCnUiConnCloser::NewL();
    iNWSessionSlotID = aNWSessionSlotID.CloneL();
    }


// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::RunStepL()
// -----------------------------------------------------------------------------
//
TInt CCnUiLogoutUiCntrlStep::RunStepL()
    {
    TIMPSConnectionClient clientToLogout;
    TInt initErr = DoInitLogoutStepL(  clientToLogout );
    if ( initErr != ECnUiLogoutInitContinue )
        {
        return initErr;
        }

    //Logout is desired for the client
    //==> no automatic rewake for client allowed
    iCCntxt.ConnModeRewaker().RemoveFromAARewakeList( clientToLogout );

    //proceed with logout, notify first observers
    iCCntxt.ProcessObserverProxy().NotifyAccessingSapL( iLogoutSap );

    //close the client connection using the connection closer
    //This control follows closer progress events
    //and sets up proper wait notes according the closer states
    TInt logoutStatus;
    logoutStatus =
        iConnCloser->MakeLogoutForClient(
            clientToLogout,
            iLogoutSap,
            iCCntxt.ClientPluginL( clientToLogout, *iNWSessionSlotID ),
            iCCntxt.ConnHandler(),
            this,
            *iNWSessionSlotID );

    //connection closer finished its processing
    //clear any existing wait note set by the progress step.
    delete iWaitNote;
    iWaitNote = NULL;

    //and flush the possible buffered UI commands
    iCCntxt.Ui().FlushCommandBuffer();

    //show proper note for status code
    if ( logoutStatus == KErrNone )
        {
        //show a note about succesful logout
        DoShowLogoutOkNoteL( clientToLogout );
        /*
         *
         * Check if the connection mode is Automatic (always online), in which case
         * we need to login again.
         */
        MCnUiClientPlugin& cPlugin = iCCntxt.ClientPluginL( clientToLogout, *iNWSessionSlotID );
        if ( cPlugin.CurrentConnectionModeSettingL() == ECnUiCMAutomatic )
            {
            _LIT_SECURITY_POLICY_PASS( KIMPSSharedReadPolicy );
            _LIT_SECURITY_POLICY_PASS( KIMPSSharedWritePolicy );
            // The uid of shared data to notify the Always online plugin.
            const TUid KIMPSServSettNotifyUid =   {0x101f75b6};
            // 154 is EIMPSSharedKeysServSettScheduleChangedIM.
            TUint32 aKey = 154;
            RProperty::Define( KIMPSServSettNotifyUid, aKey, RProperty::EInt,
                               KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
            TInt err = RProperty::Set( KIMPSServSettNotifyUid, aKey, 0 );
            }

        //and notify about manual logout
        iCCntxt.ConnModeHandler().SendUserSelectionL( EUserLevelLogout, clientToLogout );
        }

    else
        {
        //logout failed
        iCCntxt.Ui().ShowLogoutErrorNoteL( logoutStatus );
        }

    return logoutStatus;
    }


// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiLogoutUiCntrlStep::HandleCompleteL()
    {
    if ( !iCCntxt.ConnHandler().NwConnectionActiveL( *iNWSessionSlotID ) &&
         iConnCloser->ConnectionReallyClosed() )
        {
        //there isn't no more active NW connections
        //and the connection was closed by this control.

        //let see is there clients to reconnect
        RArray< TIMPSConnectionClient > aaClientsToReconnect;
        CleanupClosePushL( aaClientsToReconnect );
        TBool gotAAClients = CnUiSapDataPacker::ExtractAAClientsL( iLogoutSap,
                                                                   aaClientsToReconnect );
        CleanupStack::PopAndDestroy(); //aaClientsToReconnect

        if ( gotAAClients )
            {
            //There is some AA clients to reconnect.

            CIMPSSAPSettings* defaultSAPToReconnect = CIMPSSAPSettings::NewLC();
            CIMPSSAPSettingsStore& sapStore = iCCntxt.SapStoreL();

            TIMPSConnectionClient client( ConnectionClient() );
            TIMPSAccessGroup accessGroup;
            if ( client == EIMPSConnClientIM )
                {
                accessGroup = EIMPSIMAccessGroup;
                }
            else
                {
                accessGroup = EIMPSPECAccessGroup;
                }

            TRAPD( err, sapStore.GetDefaultL( defaultSAPToReconnect, accessGroup ) );

            if ( err == KErrNotFound )
                {
                //Ignore not found error.
                //If there isn't the default SAP to reconnect
                //nothing can't be done.
                err = KErrNone;
                }

            else if ( err == KErrNone )
                {
                //remove first any packed SAP from default SAP
                //to avoid restore nesting...
                CnUiSapDataPacker::RemovePackedSapL( *defaultSAPToReconnect );
                iCCntxt.SubOpReconnectAllL( *defaultSAPToReconnect, *iNWSessionSlotID );
                }

            //Handle other default SAP load errors with leaving
            User::LeaveIfError( err );
            CleanupStack::PopAndDestroy( defaultSAPToReconnect ); //defaultSAPToReconnect
            }
        }

    return ECnUiStepContinueTeardown;
    }

// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::UndoStepL()
// -----------------------------------------------------------------------------
//
void CCnUiLogoutUiCntrlStep::UndoStepL()
    {
    //nothing to undo
    }


// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::HandleProgressStateEnterL()
// -----------------------------------------------------------------------------
//
void CCnUiLogoutUiCntrlStep::HandleProgressStateEnter( TInt aStateId,
                                                       TIMPSConnectionClient aClient )
    {
    TRAPD( leaveErr, DoShowLogoutStateSpecificWaitnoteL( aStateId, aClient ) );
    iCCntxt.Ui().HandleIfError( leaveErr );
    }


// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::ShowSimultaneousLoginOperationNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiLogoutUiCntrlStep::ShowSimultaneousLoginOperationNoteL()
    {
    HBufC* serverName = NULL;
    iCCntxt.Signaller().OperationDetailsL( serverName );

    //OperationDetailsL() returns the HBufC ownership
    CleanupStack::PushL( serverName );
    iCCntxt.Ui().ShowNoteL( ECnUiConnOperationAllreadyRunning, *serverName );
    CleanupStack::PopAndDestroy( serverName ); //serverName
    }


// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::DoInitLogoutStepL()
// Template method
// -----------------------------------------------------------------------------
//
TInt CCnUiLogoutUiCntrlStep::DoInitLogoutStepL( TIMPSConnectionClient& aClient )
    {
    const TIMPSConnectionClient clientId = iCCntxt.ControlledClient();

    //is this client logged in?
    //if not, then no need to proceed
    if ( !iCCntxt.ConnHandler().TheClientLoggedInL( clientId ) )
        {
        return KErrNone; //breaks the RunStepL() with KErrNone
        }


    //Is this operation free to proceed?
    //If not, show a note, else proceed.
    //However no signalling about logout is used...
    if ( iCCntxt.Signaller().OperationRunning() )
        {
        ShowSimultaneousLoginOperationNoteL();
        return KCnUiErrorLoginOperationAlreadyInUse;
        }


    //get SAP from where logging out..
    iCCntxt.ConnHandler().GetLoggedInSapL( iLogoutSap, clientId );
    aClient = clientId;

    return ECnUiLogoutInitContinue;
    }


// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::DoShowLogoutStateSpecificWaitnoteL()
// -----------------------------------------------------------------------------
//
void CCnUiLogoutUiCntrlStep::DoShowLogoutStateSpecificWaitnoteL( TInt aStateId,
                                                                 TIMPSConnectionClient /*aClient*/ )
    {
    switch ( aStateId )
        {
        case ECnCloser_ProcessingPlugin:
        case ECnCloser_ClosingSAPConnection:
            {
            if ( !iWaitNote )
                {
                //the same wait note is used
                //both when closing SAP connection &
                //processing plug-in stuff
                iWaitNote = iCCntxt.Ui().WaitNoteL( ECnUiWaitNoteDisconnecting );
                }

            break;
            }

        case ECnCloser_Idle:
        default:
            {
            //clear any existing wait note set by the previous
            //progress steps. No wait note used during these steps.
            delete iWaitNote;
            iWaitNote = NULL;

            break;
            }
        }
    }



// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::DoShowLogoutOkNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiLogoutUiCntrlStep::DoShowLogoutOkNoteL( TIMPSConnectionClient aClient )
    {
    MCnUiClientPlugin& cPlugin = iCCntxt.ClientPluginL( aClient, *iNWSessionSlotID );
    if ( cPlugin.CurrentConnectionModeSettingL() == ECnUiCMAutomatic )
        {
        iCCntxt.Ui().ShowNoteL( ECnUiDisconnectedAutomaticConnection );
        }

    else
        {
        switch ( aClient )
            {
            case EIMPSConnClientPEC:
                {
                iCCntxt.Ui().ShowNoteL( ECnUiDisconnectedPEC );
                break;
                }

            case EIMPSConnClientIM:
                {
                iCCntxt.Ui().ShowNoteL( ECnUiDisconnectedIM );
                break;
                }

            default:
                {
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCnUiLogoutUiCntrlStep::DoShowLogoutOkNoteL()
// -----------------------------------------------------------------------------
//
TIMPSConnectionClient CCnUiLogoutUiCntrlStep::ConnectionClient()
    {
    // rvct does not support casting from TAny* to enumeration value
    TInt returnValue = ( TInt )Dll::Tls();
    return ( TIMPSConnectionClient )returnValue;
    }

//  End of File

