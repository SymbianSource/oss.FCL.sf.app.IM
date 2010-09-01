/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Network session handling (login, logout, etc.)
*
*/



// INCLUDE FILES
#include "CCASessionHandlerCmd.h"
#include "ChatDebugPrint.h"
#include "MCAImpsFundClient.h"
#include "CCAEngine.h"
#include "mcagroupmanagerinterface.h"
#include "MCAContactLists.h"
#include "MCAPresence.h"
#include "CAPresenceManager.h"
#include "MCAStoredContact.h"
#include "MCAStoredContacts.h"
#include "CCAStorageManagerFactory.h"
#include "ImpsCSPAllErrors.h"
#include "MCASettings.h"
#include "MCAChatInterface.h"
#include "MCAContactFetchObserver.h"
#include "MCAAccessInterface.h"
#include "mcaservicestateobserver.h"

#include "MCABackgroundTaskObserver.h"
#include "SServerPrefers.h"
#include "impsbuilddefinitions.h"

#include <bautils.h>
#include <ChatNG.rsg>
#include <cimpspresenceconnectionuing.h>
#include <cimpsconnuiclntfilteredpreseventnotifierng.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsList.h>
#include <CWVSettingsUINGDialog.h>
#include <WVSettingsSharedDataNG.h>
#include <bldvariant.hrh>
#include <centralrepository.h>
#include <IMPSServiceSettingsUINGInternalCRKeys.h>
#include <aknenv.h>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <Profile.hrh>
#include <impspresenceconnectionuiconstsng.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlot2.h>
#include <eikenv.h>
#include <e32property.h>
#include <StringLoader.h>

#include "IMUtils.h"
#include "IMDialogUtils.h"
#include "IMNoteMapper.h"
#include "CCACommandManagerFactory.h"
#include "CCACommandManager.h"
#include "MCALoginPC.h"
#include "MCAProcessManager.h"
#include "MCASettingsPC.h"
#include "MCAConversationPC.h"
#include "MCAUiLoginCmdCB.h"

// The Settings have been moved to Cenrep (also retained in the Resource file)
// so the enums for keys and central repository header is added here

#include 	"VariantKeys.h"


// CONSTANTS
// defines whether first login to server has been done
_LIT( KIMFirstLoginToServer, "IMFirstLoginToServer" );



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::CCASessionHandlerCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCASessionHandlerCmd::CCASessionHandlerCmd(
    MCAProcessManager& aProcessManager,
    MCAUiLoginCmdCB& aUiLoginCmdCB )
        : iLastState( EIMPSPresenceServiceUnknownEvent ),
        iPresenceInitializationSuccesfull( EFalse ),
        iIsContactFetchDone( EFalse ),
        iProcessManager( aProcessManager ),
        iUiLoginCmdCB( aUiLoginCmdCB ),

        iBackgroundTaskRunning( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCASessionHandlerCmd::ConstructL()
    {

    iProfileApi = CreateProfileEngineL();

    iConnUI = CIMPSPresenceConnectionUi::NewL( EIMPSConnClientIM );

    iConnUIEventNotifier = CIMPSConnUiClntFilteredPresEventNotifier::NewL(
                               EIMPSConnClientIM );
    iConnUIEventNotifier->AddObserverL( this );

    iConnUIEventNotifier->StartL();

    iUiLoginCmdCB.SetStatusPaneIconsL();


    MCAPresence* presence = CAPresenceManager::InstanceL();


    if ( IsLoggedIn() )
        // If already logged in, just update engine state and fetch contact lists
        {

        iBackgroundTaskRunning = EFalse;

        // we can ignore the returnvalue here,
        // since the member variable is set inside anyway
        NetworkSessionSlotIDL();
        TRAPD( err, presence->InitializePresenceAPIL( *iSessionSlotID ) );

        // ATM only occasion for this to fail is that PEC has not fully
        // synchronized contact lists. We will get online event when the
        // lists are in sync.
        if ( err == KErrNone )
            {
            iPresenceInitializationSuccesfull = ETrue;

            TBool redraw( GetServicesL() );

            iUiLoginCmdCB.SetIMPSServices( iImpsServices, ETrue, redraw );

            // make sure chat has been constructed before notifying
            // the engine. if we're logged in, IMPS server should be up,
            // so this is fast
            iUiLoginCmdCB.FinalizeChatConstructionL();

            NotifyEngineL( ELoggedIn, iSessionSlotID );


            iUiLoginCmdCB.CaptureEventsL();

            // Idle loop for starting default list selection

            delete iSelectIdle;
            iSelectIdle = NULL;
            iSelectIdle = CIdle::NewL( CActive::EPriorityIdle );
            iSelectIdle->Start( TCallBack( BackgroundSelect, this ) );


            // set variated settings
            TRAPD( err, UpdateDefaultSettingsL() );
            if ( err != KErrNone ) // Signal user about error
                {
                CActiveScheduler::Current()->Error( err );
                }


            // Store current logged in SAP as previous
            CIMPSSAPSettings* temp = CurrentSAPLC();
            iPreviousSAP = temp;    // Take ownership
            CleanupStack::Pop( temp );
            }
        }
    else
        {
#ifndef CHAT_UNIT_TEST
        // Idle loop for starting login sequence login
        iIdle = CIdle::NewL( CActive::EPriorityIdle );
        iIdle->Start( TCallBack( BackgroundTasks, this ) );
#endif
        }
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCASessionHandlerCmd* CCASessionHandlerCmd::NewL(
    MCAProcessManager& aProcessManager,
    MCAUiLoginCmdCB& aUiLoginCmdCB )
    {
    CCASessionHandlerCmd* self = new( ELeave ) CCASessionHandlerCmd(
        aProcessManager,
        aUiLoginCmdCB );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCASessionHandlerCmd::~CCASessionHandlerCmd()
    {
    if ( iProfileApi )
        {
        iProfileApi->Release();
        }

    if ( iConnUIEventNotifier )
        {
        iConnUIEventNotifier->Stop();
        iConnUIEventNotifier->RemoveObserver( this );
        }

    delete iConnUIEventNotifier;
    delete iConnUI;
    delete iIdle;
    delete iSelectIdle;

    delete iPreviousSAP;
    delete iSessionSlotID;

    iFetchObservers.Close();
    iServiceStateObservers.Close();

    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::IsLoggedIn
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASessionHandlerCmd::IsLoggedIn() const
    {
    TBool returnValue( EFalse );

    if ( IsOfflineProfileOn() )
        {
        CHAT_DP( D_CHAT_LIT(
                     "CCASessionHandlerCmd::IsLoggedIn, offline profile is ON, returning %d" ),
                 returnValue );
        return returnValue;
        }


    TRAPD( err, returnValue = iConnUI->LoggedInL( EIMPSConnClientIM ) );

    if ( err )
        {
        returnValue = EFalse;
        }

    CHAT_DP( D_CHAT_LIT( "CCASessionHandlerCmd::IsLoggedIn returning %d" ),
             returnValue );

    // if there are no slots in the system then we get KErrNotFound
    // and we don't show it to the user
    if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
        {
        CActiveScheduler::Current()->Error( err );
        }
    return returnValue;
    }


// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::LoginL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASessionHandlerCmd::LoginL( TBool aAskConfirmation /*= ETrue */,
                                    TBool aIsStartupLogin /*= EFalse */ )
    {
    CHAT_DP_TXT( "CCASessionHandlerCmd::LoginL started" );

    if ( IsOfflineProfileOn() )
        {
        IMDialogUtils::DisplayInformationNoteL( R_QTN_OFFLINE_NOT_POSSIBLE );
        return EFalse;
        }

    if ( IsLoggedIn() )
        {
        return ETrue; // already logged in
        }

    TInt ret( EAknSoftkeyNo );
    if ( aAskConfirmation )
        {
        TPtrC serverNamePtr( KNullDesC );

        CIMPSSAPSettings* sap = NULL;
        if ( iPreviousSAP )
            {

            // get current SAP
            CIMPSSAPSettings* currentSap = CurrentSAPLC();
            delete iPreviousSAP;

            // replace the previous SAP
            iPreviousSAP = currentSap;
            CleanupStack::Pop( currentSap );


            // get the last logged in server name

            serverNamePtr.Set( iPreviousSAP->SAPName() );
            }
        else
            {
            sap = CurrentSAPLC();
            serverNamePtr.Set( sap->SAPName() );


            CleanupStack::PopAndDestroy( sap );
            }
        // load the promp from lOC file + server name
        HBufC* prompt = StringLoader::LoadLC( R_QTN_CHAT_CONNECTION_NEEDED_TO,
                                              serverNamePtr );

        ret = IMDialogUtils::DisplayQueryDialogL( R_GENERIC_YES_NO_CONFIRMATION_QUERY,
                                                  *prompt );

        CleanupStack::PopAndDestroy( prompt );
        }

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes )
         || ( !aAskConfirmation ) )
        {
        // check if we have valid server data
        if ( iUiLoginCmdCB.CheckCurrentServerDataL() )
            {
            CHAT_DP_TXT(
                "PEC is offline, server data is ok -> execute PEC login" );

            TIMPSLoginType loginType = LoginType(); // default to manual login
            if ( ( loginType == EIMPSAAConnectionStart ||
                   loginType == EIMPSApplicationLaunch ) &&
                 !aIsStartupLogin )
                {
                //defaulting to manual if login type is automatic
                // since in this case we want to see the server selection dialog
                loginType = EIMPSManualLogin;
                }

            if ( loginType != EIMPSAAConnectionStart )
                {
                iLogging = ETrue;

                if ( aAskConfirmation )
                    {
                    loginType = EIMPSApplicationLaunch;
                    }

                CPEngNWSessionSlotID2* tempID = CPEngNWSessionSlotID2::NewL();
                delete iSessionSlotID;
                iSessionSlotID = tempID;

                iCurrentLoginIsFirst = EFalse;
                TInt err = KErrNone;

                if ( aAskConfirmation && iPreviousSAP )
                    {
                    //  user want to connect to last logged in server
                    // hence pass the previous sap
                    // aAskConfirmation will allow to load from sapsetting any more
                    err = iConnUI->LoginL( EIMPSConnClientIM, loginType,
                                           *iSessionSlotID, this , iPreviousSAP, aAskConfirmation ) ;
                    }
                else
                    {
                    err = iConnUI->LoginL( EIMPSConnClientIM, loginType,
                                           *iSessionSlotID, this ) ;
                    }

                CHAT_DP( D_CHAT_LIT(
                             "CCASessionHandlerCmd::LoginL, \
                            iConnUI->LoginL( EIMPSConnClientIM, %d ) returned %d" ),
                         loginType, err );
                iLogging = EFalse;

                if ( err == KErrNone )
                    {

                    TBool redraw( GetServicesL() );

                    iUiLoginCmdCB.SetIMPSServices( iImpsServices, ETrue, redraw );

                    NotifyEngineL( EServerPrefers );

                    iUiLoginCmdCB.SetStatusPaneIconsL();

                    CCAStorageManagerFactory::ContactListInterfaceL()->
                    ResetContactLists();

                    CIMPSSAPSettings* sap = CurrentSAPLC();

                    MCASettingsPC* settingsPC = iProcessManager.GetSettingsInterface();
                    settingsPC->SetDefaultSapL( sap );
                    CleanupStack::PopAndDestroy( sap );
                    }

                iUiLoginCmdCB.FinalizeChatConstructionL();

                }
            }
        }

    iUiLoginCmdCB.ReleaseCapturingL();
    return IsLoggedIn(); // Login was perhaps done
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::LogoutL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASessionHandlerCmd::LogoutL()
    {
    CHAT_DP_TXT( "CCASessionHandlerCmd::LogoutL started" );
    //Check for background tasks completion.
    //only then give perform logout

    if ( !IsBackgroundTaskPendingL() )
        {

        iProcessManager.GetEngine()->CancelAllRequests();

        return DoLogoutL();
        }
    CHAT_DP_TXT( "CCASessionHandler::LogoutL done" );

    return IsLoggedIn();
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::LogoutL
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TBool CCASessionHandlerCmd::DoLogoutL()
    {
    CHAT_DP_TXT( "CCASessionHandlerCmd::DoLogoutL started" );

    MCAStoredContacts* contactsInterface =
        CCAStorageManagerFactory::ContactListInterfaceL();

    contactsInterface->SetContactListLock( ETrue );

    iLogging = ETrue;
    // this should not even be called if there is no session slot ID
    if ( iSessionSlotID )
        {

        iProcessManager.GetLogInInterface()->LogoutL();
        iConnUI->LogoutL( *iSessionSlotID, this );
        delete iSessionSlotID;
        iSessionSlotID = NULL;

        }

    iLogging = EFalse;
    iCurrentLoginIsFirst = EFalse;
    NotifyEngineL( ELoggingOut );


    contactsInterface->SetContactListLock( EFalse );


    CHAT_DP_TXT( "CCASessionHandlerCmd::DoLogoutL done" );
    return IsLoggedIn();
    }

// ---------------------------------------------------------
// CCASessionHandler::HandleSapAccessEventEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::HandleSapAccessEventEventL( const TDesC& aServerName,
                                                       const TDesC& aServerURL,
                                                       MIMPSConnProcessUi* aConnProcessUi )
    {
    iUiLoginCmdCB.HandleSapAccessEventEventL( aServerName, aServerURL,
                                              aConnProcessUi );
    }


// ---------------------------------------------------------
// CCASessionHandlerCmd::IsSupported
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASessionHandlerCmd::IsSupported( TSupportedFeature aFeature )
    {
    switch ( aFeature )
        {
        case EGroup :
            {
            return iImpsServices.iGroupFeat.FeatureSupported();
            }
        case ESearch :
            {
            return iImpsServices.iFundamentalFeat.FunctionSupported(
                       KPEngFFSearchFunction );
            }
        case EInvite :
            {
            return iImpsServices.iFundamentalFeat.FunctionSupported(
                       KPEngFFInviteFunction );
            }
        case EBlock :
            {
            return iImpsServices.iIMFeat.FunctionSupported(
                       KPEngIMAuthorFunctions );
            }
        case EAttrList :
            {
            return iImpsServices.iPresenceFeat.FunctionSupported(
                       KPEngWVAttrListFunction );
            }
        case ECLIMod:
            {
            // contact list creation/deletion is possible only if
            // both CCLI and DCLI are supported
            TBool returnValue( ETrue );
            returnValue &=
                iImpsServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncCCLI );
            returnValue &=
                iImpsServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncDCLI );
            return returnValue;
            }
        case EGroupRejectList:
            {
            return iImpsServices.iGroupFeat.SubFunctionSupported(
                       KPEngGRSubFuncREJEC );
            }
        default:
            {
            return EFalse; // Unknown features are not supported
            }
        }
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::HandlePresenceEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASessionHandlerCmd::HandlePresenceEventL(
    CIMPSConnUiPresEventNotifier* /*aNotifier*/,
    const CPEngNWSessionSlotID2& /*aSessionSlotID*/,
    TIMPSConnectionClient /*aClient*/,
    TIMPSPresenceServiceEvent aEvent )
    {
    CHAT_DP( D_CHAT_LIT(
                 "CCASessionHandlerCmd::HandlePresenceEventL, TIMPSPresenceServiceEvent: %d" ),
             aEvent );

    // Same state as previous, no need for action
    if ( aEvent == iLastState )
        {
        return;
        }

    switch ( aEvent )
        {
        case EIMPSPresenceServiceUnknownEvent:
            {
            CHAT_DP_TXT(
                "CCASessionHandlerCmd::HandlePresenceEventL\
            ->EIMPSPresenceServiceUnknownEvent" );
            break;
            }
        case EIMPSPresenceServiceOffline:
            {
            CHAT_DP_TXT(
                "CCASessionHandlerCmd::HandlePresenceEventL\
            ->EIMPSPresenceServiceOffline" );
            iTemporaryNetworkDown = EFalse;

            if ( iLastState != EIMPSPresenceServiceForceLogOut )
                {
                NotifyEngineL( ELoggedOut );
                }

            iAlreadyLoggedOut = ETrue;

            iProcessManager.GetLogInInterface()->LogoutL();

            TRAPD( error, iUiLoginCmdCB.SetStatusPaneIconsL() );

            iAlreadyLoggedOut = EFalse;

            User::LeaveIfError( error );

            // nuke all contacts from IM Storage
            MCAStoredContacts* contacts =
                CCAStorageManagerFactory::ContactListInterfaceL();
            // Update own status item
            contacts->OwnStatus().SetOnlineStatus( TStorageManagerGlobals::EOffline );
            // nuke all contacts from IM Storage
            contacts->RemoveAllContactsL();

            iUiLoginCmdCB.IMUIPIConForcedClear();

            break;
            }
        case EIMPSPresenceServiceOnline:
            {
            CHAT_DP_TXT(
                "CCASessionHandlerCmd::HandlePresenceEventL \
                ->EIMPSPresenceServiceOnline" );

            if ( iPresenceInitializationSuccesfull )
                {
                // Construction is already done, so no need to do it here
                // again. If user have launched IM after contact list
                // synchronization and before this online event, we might
                // initialize IM twice.
                break;
                }

            iUiLoginCmdCB.FinalizeChatConstructionL();


            if ( !iLogging )
                {
                // this login was not initiated by us
                // -> we need to refresh the session slot id
                CPEngNWSessionSlotID2* tempId =
                    iConnUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
                delete iSessionSlotID;
                iSessionSlotID = tempId;

                CIMPSSAPSettings* sap = CurrentSAPLC();
                MCASettingsPC* settingsPC = iProcessManager.GetSettingsInterface();
                settingsPC->SetDefaultSapL( sap );

                CleanupStack::PopAndDestroy( sap );
                }
            if ( !iTemporaryNetworkDown )
                {

                TBool redraw( GetServicesL() );

                CAPresenceManager::InstanceL()->InitializePresenceAPIL(
                    *iSessionSlotID );

                iUiLoginCmdCB.SetIMPSServices( iImpsServices, ETrue, redraw );
                NotifyEngineL( ELoggedIn, iSessionSlotID );

                iUiLoginCmdCB.SetStatusPaneIconsL();

                // Idle loop for starting default list selection
                delete iSelectIdle;
                iSelectIdle = NULL;
                iSelectIdle = CIdle::NewL( CActive::EPriorityIdle );
                iSelectIdle->Start( TCallBack( BackgroundSelect, this ) );

                // set variated settings
                TRAPD( err, UpdateDefaultSettingsL() );
                if ( err != KErrNone ) // Signal user about error
                    {
                    CActiveScheduler::Current()->Error( err );
                    }

                CIMPSSAPSettings* currentSap = CurrentSAPLC();
                // reomoved extra code from here

                // take the ownership
                delete iPreviousSAP;
                iPreviousSAP = currentSap;
                CleanupStack::Pop( currentSap );

                // check if this was first login or not
                UpdateFirstLoginL();

                iUiLoginCmdCB.CaptureEventsL();
                }
            else
                {
                iTemporaryNetworkDown = EFalse;
                }

            break;
            }
        case EIMPSPresenceServiceForceLogOut:
            {
            CHAT_DP_TXT(
                "CCASessionHandlerCmd::HandlePresenceEventL ->\
                EIMPSPresenceServiceForceLogOut" );


            NotifyEngineL( ELoggedOut );
            iUiLoginCmdCB.SetStatusPaneIconsL();

            // nuke all contacts from IM Storage
            MCAStoredContacts* contacts =
                CCAStorageManagerFactory::ContactListInterfaceL();
            // Update own status item
            contacts->OwnStatus().SetOnlineStatus( TStorageManagerGlobals::EOffline );

            contacts->RemoveAllContactsL();

            break;
            }
        default:
            {
            break;
            }
        }

    if ( aEvent == EIMPSPresenceServiceForceLogOut ||
         aEvent == EIMPSPresenceServiceOffline ||
         aEvent == EIMPSPresenceServiceOnline )
        {
        // Reset initialization flag.
        iPresenceInitializationSuccesfull = EFalse;
        iIsContactFetchDone = EFalse;
        }

    // Offline events
    if ( aEvent == EIMPSPresenceServiceForceLogOut ||
         aEvent == EIMPSPresenceServiceOffline )
        {
        // notify observers just in case some post login task failed
        NotifyContactFetchObserversL( KErrCancel );
        }

    iLastState = aEvent;
    
    // Notify Service state change to MCAServiceStateObservers
    NotifyServiceStateObserversL( aEvent );
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::HandlePresenceEventNotifyError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASessionHandlerCmd::HandlePresenceEventNotifyError(
    CIMPSConnUiPresEventNotifier* /*aNotifier*/,
    TInt aError )
    {
    CHAT_DP( D_CHAT_LIT(
                 "CCASessionHandlerCmd::HandlePresenceEventNotifyError, Error: %d" ),
             aError );

    // Show note, if appropriate found
    if ( aError != KErrCancel ) // don't show error note for cancel events
        {
        TRAPD( err, IMNoteMapper::ShowNoteL( aError ) );
        if ( err )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }
    }


// ---------------------------------------------------------
// CCASessionHandlerCmd::LoginType
// Central repository version
// (other items were commented in a header).
// ---------------------------------------------------------
//
TIMPSLoginType CCASessionHandlerCmd::LoginType()
    {
    CHAT_DP( D_CHAT_LIT( "CCASessionHandlerCmd::LoginType" ) );

    TInt readValue( KErrCouldNotConnect ); // initialize to an error value

    TIMPSLoginType loginType( EIMPSManualLogin );

    CRepository* cenrep = NULL;
    TRAPD( err, cenrep = CRepository::NewL( KWVSettingsCenRepUid ) );

    if ( err != KErrNone )
        {
        // creation of cenrep failed -> return
        return loginType;
        }

    TRAP( err,
          CleanupStack::PushL( cenrep );
          err = cenrep->Get( KIMPSCRChatLogin, readValue );
          CleanupStack::PopAndDestroy( cenrep );
        ); // TRAP
    cenrep = NULL;

    if ( err != KErrNone )
        {
        // there was an error
        return loginType;
        }

    if ( ( readValue == EWVSettingsChatLoginAutoAlways ) ||
         ( readValue == EWVSettingsChatLoginAutoInHomeNW ) )
        {
        return EIMPSAAConnectionStart;
        }
    else if ( readValue == EWVSettingsChatLoginApplicationLaunch )
        {
        return EIMPSApplicationLaunch;
        }
    else
        {
        return EIMPSManualLogin;
        }
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::BackgroundTasks
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCASessionHandlerCmd::BackgroundTasks( TAny *aInstance )
    {
    return static_cast<CCASessionHandlerCmd*>( aInstance )->DoBackgroundTasks();
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::DoBackgroundTasks
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCASessionHandlerCmd::DoBackgroundTasks()
    {
    CHAT_DP_TXT( "CCASessionHandlerCmd::DoBackgroundTasks()" );

    TBool dataOk( EFalse );
    TRAPD( err, dataOk = iUiLoginCmdCB.CheckCurrentServerDataL() );

    if ( err != KErrNone ) // Signal user about error
        {
        CActiveScheduler::Current()->Error( err );
        }


    // do not even try to log in if data check failed or data is invalid
    if ( err != KErrNone || !dataOk )
        {


        // update to platform using the new task.

        // If the server data is invalid, then set the flag back
        iBackgroundTaskRunning = EFalse;

        // create the views
        TRAPD( err2, iUiLoginCmdCB.FinalizeChatConstructionL() );
        if ( err2 != KErrNone )
            {
            // show e.g. OOM
            CActiveScheduler::Current()->Error( err2 );
            }
        return EFalse;
        }


    TBool loginDone = EFalse;
    TRAP( err, loginDone = LoginL( EFalse, ETrue ) );

    if ( err != KErrNone || ! loginDone ) // Signal user about error
        {
        TRAPD( err2, iUiLoginCmdCB.FinalizeChatConstructionL() );
        if ( err2 != KErrNone )
            {
            // show e.g. OOM
            CActiveScheduler::Current()->Error( err2 );
            }
        if ( err != KErrNone )
            {
            // some error, the views should exist now.
            // must check err, because loginDone condition could bring us here,
            // in which case errors have been shown by IMPS Common UI.
            CActiveScheduler::Current()->Error( err );
            }
        }


    // Set it back.
    iBackgroundTaskRunning = EFalse;

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::BackgroundSelect
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCASessionHandlerCmd::BackgroundSelect( TAny *aInstance )
    {
    // CodeScanner warning ignored because CS does not regognize TRAP being used
    // after line break

    TRAPD( err, static_cast<CCASessionHandlerCmd*>( aInstance )->DoBackgroundSelectL() );
    // CSI: 42 # See comment above

    if ( err != KErrNone ) // Signal user about error
        {
        CActiveScheduler::Current()->Error( err );
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::DoBackgroundSelectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASessionHandlerCmd::DoBackgroundSelectL()
    {
    CHAT_DP_TXT( "CCASessionHandlerCmd::DoBackgroundSelectL()" );


    iUiLoginCmdCB.ReleaseCapturingL();

    if ( ! IsLoggedIn() )
        {
        // perhaps we couldn't login
        return;
        }

    TBool supportAlias = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVE_ALIAS );

    if ( supportAlias )
        {
        // subscribe also the alias attribute
        MCAPresence* presence = CAPresenceManager::InstanceL();
        presence->AddAttributeL( MCAPresence::EAliasAttr );
        }

    TInt chatLoginType( IMUtils::WVSettingsChatLoginValue() );
    TBool manualLogin = chatLoginType != EWVSettingsChatLoginAutoAlways &&
                        chatLoginType != EWVSettingsChatLoginAutoInHomeNW;

    iUiLoginCmdCB.ShowPostLoginNotesL( iCurrentLoginIsFirst, manualLogin );


    // Check P&S flag for forced login.
    // If forced login is made in Common UI side
    // contact list base sync is needed here although
    // login type value is allways online at this point.
    TBool forcedLogin = EFalse;
    // Ignore errors
    TInt err = RProperty::Get( KBrandingUid, KForcedLoginKey, forcedLogin );

    if ( forcedLogin )
        {
        // Force contact list base sync when starting the fetch
        manualLogin = ETrue;
        }

    // fetch all contact lists
    FetchContactListsL( !manualLogin );

    if ( forcedLogin )
        {
        // Flag used, clear it
        // Ignore errors
        TInt err = RProperty::Set( KBrandingUid, KForcedLoginKey, EFalse );
        }

    GetServicesL();
    iUiLoginCmdCB.SetIMPSServices( iImpsServices );
    NotifyEngineL( ELoggedIn );

    if ( ! iUiLoginCmdCB.CancelLoginValue() )
        {
        // flush RA stuff only if login was not cancelled

        // flush any reactive authorizations now that we are done with the login
        MCAPresence* presence = CAPresenceManager::InstanceL();
        presence->PostLoginRAFlushL();
        }
    else
        {
        // user wanted to cancel login
        if ( ! iLogging )
            {
            // invoked by Always Online, so must do logout here
            LogoutL();
            }
        }
    }



// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::CurrentSAPLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIMPSSAPSettings* CCASessionHandlerCmd::CurrentSAPLC()
    {
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();

    // If session is active, then current user-id should be retrieved from
    // connection UI instead of SAP settings store
    if ( IsLoggedIn() )
        {
        CHAT_DP_TXT(
            "CCASessionHandlerCmd::CurrentSAPLC returning SAP based on ConnUI" );
        if ( !iSessionSlotID )
            {
            iSessionSlotID =
                iConnUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
            }
        iConnUI->GetLoggedInSapL( *iSessionSlotID, *sap );
        }
    else
        {
        CHAT_DP_TXT(
            "CCASessionHandlerCmd:: \
                CurrentSAPLC returning SAP based on SAPSettingsStore" );
        CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
        sapStore->GetDefaultL( sap, EIMPSIMAccessGroup );
        CleanupStack::PopAndDestroy( sapStore );
        }

    CHAT_DP( D_CHAT_LIT( "CCASessionHandlerCmd::CurrentSAPLC sap uid %d" ),
             sap->Uid() );
    return sap;
    }


// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::GetServicesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASessionHandlerCmd::GetServicesL()
    {
    TBool redraw( EFalse );

    TBool groupsBefore( IsSupported( EGroup ) );

    CPEngNWSessionSlot2* slot = CPEngNWSessionSlot2::NewLC( *iSessionSlotID );
    TPckg<TPEngWVCspServicesTree2> cspTreePtr( iImpsServices );
    TInt err = slot->GetOption( KPEngWVCspServicesTree2,
                                KPEngNWSessionSlotTransportQuery,
                                cspTreePtr );

    CleanupStack::PopAndDestroy( slot );
    if ( err <= Imps_ERROR_BASE )
        {
        IMNoteMapper::ShowNoteL( err );
        }
    else
        {
        User::LeaveIfError( err );

        // redraw if group feature has changed
        redraw = ( groupsBefore != IsSupported( EGroup ) );
        }
    CHAT_DP( D_CHAT_LIT( "*getservices, need redraw %d" ), redraw );
    return redraw;
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::NotifyEngineL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASessionHandlerCmd::NotifyEngineL( TNetworkState aState,
                                          CPEngNWSessionSlotID2* aSessionSlotID /* = NULL */ )
    {

    CIMPSSAPSettings* sap = CurrentSAPLC();

    if ( aState == ELoggedOut )
        {
        // clear the contacts refreshed flag also
        iContactsRefreshed = EFalse;
        MCALoginPC* login = iProcessManager.GetLogInInterface();

        if ( login )
            {
            login->NotifyEngineForLogoutL( aState, aSessionSlotID, iImpsServices );
            iUiLoginCmdCB.ShutDialogsL();
            }
        else
            return; // don't anything, becoz login object is null.
        }

    else if ( aState == ELoggedIn )
        {
        CHAT_DP( D_CHAT_LIT( "NotifyEngineL - ELoggedIn" ) );

        if ( aSessionSlotID )
            {

            MCALoginPC* login = iProcessManager.GetLogInInterface();
            login->LoginL( aState, sap, aSessionSlotID, iImpsServices );

            }

        }


    CleanupStack::PopAndDestroy();


    }


// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::IsOfflineProfileOn
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASessionHandlerCmd::IsOfflineProfileOn() const
    {
    return ( EProfileOffLineId == iProfileApi->ActiveProfileId()  );
    }



// ---------------------------------------------------------
// CCASessionHandlerCmd::FetchContactListsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::FetchContactListsL( TBool aAlwaysOnlineActive )
    {
    MCAPresence* presence = CAPresenceManager::InstanceL();
    if ( !presence )
        {
        User::Leave( KErrNotFound );
        }
    MCAContactLists* listInterface = presence->ContactLists();

#ifndef IMPS_CONTACT_FETCH_BACKGROUND

    iUiLoginCmdCB.ShowProcessingNoteLC();
    TInt err( listInterface->FetchContactsL( aAlwaysOnlineActive ) );
    CleanupStack::PopAndDestroy(); // waitdialog

    iIsContactFetchDone = ETrue;
    NotifyContactFetchObserversL( err );

    if ( err != KErrNone )
        {

        HBufC* text = CCoeEnv::Static()->AllocReadResourceLC(
                          R_QTN_CHAT_FETCHING_CONTACT_LISTS_FAILED );
        IMDialogUtils::DisplayErrorNoteL( *text );
        CleanupStack::PopAndDestroy( text );

        }
#else
    // If we have AO active, then base sync should be done already
    listInterface->FetchContactsL( aAlwaysOnlineActive );
#endif //IMPS_CONTACT_FETCH_BACKGROUND
    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::CloseConversationsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::CloseConversations()
    {

    iProcessManager.GetConversationInterface()->CloseConversations();

    }


// ---------------------------------------------------------
// CCASessionHandlerCmd::NetworkSessionSlotID
// (other items were commented in a header).
// ---------------------------------------------------------
//
const CPEngNWSessionSlotID2& CCASessionHandlerCmd::NetworkSessionSlotIDL()
    {
    if ( !iSessionSlotID )
        {
        iSessionSlotID = iConnUI->GetActiveNWSessionSlotIDL(
                             EIMPSConnClientIM );
        }

    return *iSessionSlotID;
    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::CloseConversationsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::DoApplicationExitL()
    {
    if ( IsLoggedIn() )
        {
        // Leave groups when exiting application in AO.

        TRAP_IGNORE( iProcessManager.GetConversationInterface()->LeaveJoinedGroupsL() );

        if ( !iSessionSlotID )
            {
            NetworkSessionSlotIDL();
            }
        iConnUI->HandleApplicationExitL( EIMPSConnClientIM,
                                         EIMPSLeaveSessionOpenExit,
                                         *iSessionSlotID );

        iProcessManager.GetLogInInterface()->LogoutL();

        }
    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::UpdateFirstLoginL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::UpdateFirstLoginL()
    {
    CIMPSSAPSettings* currentSAP = CurrentSAPLC();

    // check if this is first login to server
    TInt firstLogin = 0;
    TInt sapError = -1;
    sapError = currentSAP->GetOpaqueInt( KIMFirstLoginToServer, firstLogin );

    if ( sapError != KErrNotFound )
        {
        User::LeaveIfError( sapError );
        }

    if ( ! firstLogin )
        {
        // first login has not been done. so this is the first login

        iCurrentLoginIsFirst = ETrue;

        CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
        CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
        sapStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

        // update information that this server has now logged in
        // at least once and find our sap.. because logged in SAP has UID of zero, we must
        // find the correct UID manually from list
        TInt index( KErrNotFound );
        sapList->FindNameL( currentSAP->SAPName(), index );
        if ( index == KErrNotFound )
            {
            User::Leave( index );
            }

        if ( index != KErrNotFound )
            {
            // found it, update the correct sap
            CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();

            TUint32 sapUid = sapList->UidForIndex( index );
            sapStore->GetSAPL( sapUid, storedSap );
            storedSap->SetOpaqueInt( KIMFirstLoginToServer, ETrue );
            sapStore->UpdateOldSAPL( storedSap, sapUid );

            CleanupStack::PopAndDestroy( storedSap );
            }

        CleanupStack::PopAndDestroy( 2, sapStore );
        }
    else
        {
        iCurrentLoginIsFirst = EFalse;
        }

    CleanupStack::PopAndDestroy( currentSAP );
    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::UpdateDefaultSettingsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::UpdateDefaultSettingsL()
    {

    MCASettingsPC* settingsPC = iProcessManager.GetSettingsInterface();


    // set variated presence update mode

    TInt presenceUpdate = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_UPDATE_CONTACTS_SETTING );

    CHAT_DP( D_CHAT_LIT(
                 "CCASessionHandlerCmd::UpdateDefaultSettingsL, presenceUpdate %d" ),
             presenceUpdate );

    if ( presenceUpdate == 1 )     // 1: only automatic update supported
        {
        settingsPC->SetBoolValuePCL( TEnumsPC::EAutomaticPresenceUpdate, ETrue );
        }
    else if ( presenceUpdate == 2 ) // 2: only manual update supported
        {
        settingsPC->SetBoolValuePCL( TEnumsPC::EAutomaticPresenceUpdate, EFalse );
        }

    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::NotifyContactFetchObserversL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::NotifyContactFetchObserversL( TInt aError )
    {
    TInt count = iFetchObservers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iFetchObservers[ i ]->HandleFetchCompleteL( aError );
        }

    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::AddContactFetchObserverL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::AddContactFetchObserverL( MCAContactFetchObserver* aObserver )
    {
    if ( iFetchObservers.Find( aObserver ) == KErrNotFound )
        {
        iFetchObservers.AppendL( aObserver );
        }
    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::RemoveContactFetchObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::RemoveContactFetchObserver( MCAContactFetchObserver* aObserver )
    {
    TInt index = iFetchObservers.Find( aObserver );
    if ( index >= 0 )
        {
        iFetchObservers.Remove( index );
        }
    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::NotifyServiceStateObserversL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::NotifyServiceStateObserversL( TIMPSPresenceServiceEvent aEvent )
    {
    MCAServiceStateObserver::TServiceState state =
        MCAServiceStateObserver::EUnknown;

    // Convert TIMPSPresenceServiceEvent to TServiceState
    switch ( aEvent )
        {
        case EIMPSPresenceServiceOffline:
        case EIMPSPresenceServiceForceLogOut: // flowthrough
            {
            state = MCAServiceStateObserver::EOffline;
            break;
            }
        case EIMPSPresenceServiceOnline:
            {
            state = MCAServiceStateObserver::EOnline;
            break;
            }
        default:
            {
            // Not important event.
            return;
            }
        }

    // Notify observers
    TInt count = iServiceStateObservers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        TRAP_IGNORE( iServiceStateObservers[ i ]->
                     HandleServiceStateChangeL( state ) );
        }

    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::AddServiceStateObserversL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::AddServiceStateObserversL( MCAServiceStateObserver* aObserver )
    {
    if ( iServiceStateObservers.Find( aObserver ) == KErrNotFound )
        {
        iServiceStateObservers.AppendL( aObserver );
        }
    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::RemoveServiceStateObservers
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASessionHandlerCmd::RemoveServiceStateObservers( MCAServiceStateObserver* aObserver )
    {
    TInt index = iServiceStateObservers.Find( aObserver );
    if ( index >= 0 )
        {
        iServiceStateObservers.Remove( index );
        }
    }

// ---------------------------------------------------------
// CCASessionHandlerCmd::IsFetchDone
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASessionHandlerCmd::IsFetchDone()
    {
    return iIsContactFetchDone;
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::ContactsRefreshed()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASessionHandlerCmd::ContactsRefreshed() const
    {
    return iContactsRefreshed;
    }
// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::ContactsRefreshed()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASessionHandlerCmd::IsAlreadyLoggedOut() const
    {
    return iAlreadyLoggedOut;
    }
// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::SetContactsRefreshed()
// (other items were commented in a header).
// -----------------------------------------------------------------------------

void CCASessionHandlerCmd::SetContactsRefreshed( TBool aRefreshed )
    {
    iContactsRefreshed = aRefreshed;
    }

// -----------------------------------------------------------------------------
// CCASessionHandlerCmd::SetContactsRefreshed()
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TBool CCASessionHandlerCmd::IsBackgroundTaskRunning() const
    {
    return iBackgroundTaskRunning;
    }

// ---------------------------------------------------------
// CCASessionHandler::IsBackgroundGroupSyncPending
// (other items were commented in a header).
// ---------------------------------------------------------

TBool CCASessionHandlerCmd::IsBackgroundTaskPendingL()
    {
    TBool ret = EFalse;

    CHAT_DP_TXT( "CCASessionHandler::IsBackgroundGroupSyncPending started" );

    MCALoginPC* loginPC = iProcessManager.GetLogInInterface();

    iBackgroundTaskPending = loginPC->IsBackgroundTaskPending();

    // this call minimize the synch time
    // it cancel all runing request
    loginPC->CancelAllRequests();

    // check for background task state
    if ( iBackgroundTaskPending )
        {
        loginPC->RegisterBackGroundTaskObserver( this );

        ret = WaitToCompleteBackgroundTaskL();

        }

    CHAT_DP_TXT( "CCASessionHandler::IsBackgroundGroupSyncPending completed" );

    return ret;

    }

// ---------------------------------------------------------
// CCASessionHandler::HandleGroupSync
// (other items were commented in a header).
// ---------------------------------------------------------

void CCASessionHandlerCmd::HandleBackGroundTaskCompleteL( TBool aCompleted )
    {

    CHAT_DP_TXT( "CCASessionHandler::HandleBackGroundTaskComplete started" );

    MCALoginPC* loginPC = iProcessManager.GetLogInInterface();

    iBackgroundTaskPending = aCompleted;

    if ( ! iBackgroundTaskPending ) //group synchro is complete
        {
        // hide wait note
        if ( iWaitVisible )
            {
            TRAPD( err, iUiLoginCmdCB.DismissProcessingNoteL( KErrNone , ETrue ) );
            if ( err != KErrNone )
                {
                User::Leave( err );
                }
            iWaitVisible = EFalse;
            }

        loginPC->UnRegisterBackGroundTaskObserver( this );

        // every thinh is ok ,now call logout
        DoLogoutL() ;

        }
    CHAT_DP_TXT( "CCASessionHandler::HandleGroupSync done" );
    }
// -----------------------------------------------------------------------------
// CCASessionHandler::GroupSyncL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASessionHandlerCmd::WaitToCompleteBackgroundTaskL()
    {

    CHAT_DP_TXT( "CCASessionHandler::GroupSyncL started" );

    if ( ! iWaitVisible )
        {
        CHAT_DP_TXT(
            "CCASessionHandler::GroupSyncL - ! iWaitVisible && iGroupSyncProgress" );
        // launch the wait note
        iWaitVisible = ETrue;

        iUiLoginCmdCB.ShowProcessingNoteLC();

        CleanupStack::Pop();// waitnote

        CHAT_DP_TXT( "CCASessionHandler::GroupSyncL done" );
        }
    return ETrue;
    }


//  End of File

