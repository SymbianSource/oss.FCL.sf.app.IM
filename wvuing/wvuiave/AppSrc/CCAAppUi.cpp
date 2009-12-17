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
* Description:  AppUi class of application
 *  Since		: 3.2
 *
*/


#include    <WVSettingsSharedDataNG.h>
#include    <cimpsconnectionuingagent.h>
#include    <PEngWVPresenceAttributes2.h>
#include 	<cdlengine.h>
 #include "MCAGroupManagerInterface.h"

/* A single usage of imps error base constant.
 * Hence not moved to the process component.
 */
#include    "ImpsCSPAllErrors.h"

#include    "CCAEngine.h"               // Model
#include    "CIMPSSAPSettings.h"
#include    "CIMPSSAPSettingsStore.h"

#include    "CCAAppUi.h"
#include    "CCAApp.h"
#include    "CAExternalInterface.h"     // UIDs
#include    "ChatDefinitions.h"         // Constant definitions
#include    "chatngclient.hrh"			// resource header file
#include    "ChatDebugPrint.h"          // For debug macros
#include    "IMNoteMapper.h"
#include    "IMDialogUtils.h"
#include    "IMUtils.h"
#include    "CAViewCreator.h"           //View creator
#include    "CAUtils.h"
#include    "CAVersion.h"               //Version info provider
#include    "CCAStatusPaneHandler.h"    // For statuspane application wide handling
#include    "CCAAudioManager.h"
#include    "CCAVariantFactory.h"       // for variation
#include    "CCAInputAbsorber.h"
#include    "CCAGroupUtils.h"
#include    "CCAUISessionManager.h"
#include    "CCAIconLoader.h"
#include    "CCAMessageExtensionsHandler.h"
#include    "CAArrayUtils.h"
#include    "CCAWaitNote.h"
#include    "MCASkinVariant.h"
#include    "MCAOpBrandVariant.h"
#include    "MCAAHVariant.h"
#include    "MCALayoutChangeObserver.h"
#include    "CANotifierFactory.h"
#include    "MCANotifierInterface.h"
#include    "CCAGlobalNoteHandler.h"
#include    "TCAChatListBoxLayout.h"
#include    "CCALayoutInfo.h"
#include    "mcaresourcechangeobserver.h"
#include    "CCAAppSettingsDialog.h"
#include    "CCAAppSettingsSAPExt.h"
#include    "chatdebugassert.h"
#include    "iminternalpskeys.h"
#include    "imnginternalcrkeys.h"
#include    "imprivatecrkeys.h"
#include    "imvariant.hrh"
#include	"mcaserverchangenotify.h"
#include    "CWVSettingsUINGDialog.h"

#include 	"CCASplashScreen.h"
//process component
#include    "CCACommandManagerFactory.h"
#include    "CCACommandManager.h"
#include    "MCALoginPC.h"
#include    "MCAProcessManager.h"
#include    "MCACommand.h"
#include    "MCAMainViewArrayPC.h"
#include	"MCAConversationPC.h"
#include	"MCAGroupPC.h"
#include    "MCASettingsPC.h"
#include    "MCARecordedChatsArrayPC.h"
#include	"CCAPCUtils.h"
#include    "MCAApplicationNGPC.h"
#include	"IMMessageUtilsPC.h"
#include    "MCAGlobalNotificationObserverPC.h"
#include    "MCAConversationMessage.h"
#include    "MCAInvitationPC.h"
#include    "MCASettingsPC.h"
#include    "MCABackgroundTaskObserver.h"


#include    <data_caging_path_literals.hrh>
#include    <hlplch.h>
#include    <aknenv.h>
#include    <ChatNG.rsg>
#include    <ChatNG.mbg>
#include    <CAVariationNG.rsg>
#include    <AknDlgShut.h>
#include    <AknSoftNotifier.h>
#include    <AknsUtils.h>
#include    <bautils.h>
#include    <FeatMgr.h> // feature manager
#include    <sysutil.h>
#include    <aknsconstants.h>

#include    <aknlistquerydialog.h>
#include    <StringLoader.h>

#include    <s32file.h> // File streams
#include    <s32mem.h> //CBufStore
#include    <s32stor.h> // CStreamStore
#include    <s32buf.h> // TStreamPos
#include    <aknnotewrappers.h>
#include    <akniconarray.h>
#include    <bldvariant.hrh>
#include    <akndef.h>

#include 	<aknsconstants.h>
#include 	<aknsconstants.hrh>
#include 	<AknUtils.h>
#include 	<eikdef.h>
#include 	<COEAUI.H>
#include 	<SkinLayout.cdl.h>
#include 	<LayoutMetaData.cdl.h>
#include 	<AknLayoutScalable_Avkon.cdl.h>
#include 	<aknstatuspaneutils.h>
#include 	<aknlayoutscalable_avkon.cdl.common.h>
#include 	<errorui.h>
#include    <e32std.h>
// The Settings have been moved to Cenrep (also retained in the Resource file),
//so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#include    "MCABlocking.h"

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#include <rsfwmountman.h>
#endif
_LIT( KPrivateVersionFile, "IMVersion.txt" );
_LIT( KPublishedVersionFile, "\\System\\data\\IMVersion\\IMVersion.txt" );
_LIT( KFileName, "z:\\resource\\apps\\chatng.mif" );

const TUid KSDUidSysApLocal = {0x100058F3};

// For own status
const TInt KAmountOfOwnStatusStates = 3;
const TInt KOnlineIconIndex = 0;

const TInt KMaxStatusMessageCount = 4;

// This is used in IMPSCommonUi in CCnUiClientPluginIM.cpp
// If this is changed, remember to change also in IMPSCommonUi side.
const TInt KStatusMessageHeaderLength = 3; // length of status message header.
// KStatusMessageHeaderLength must be used as amount of characters for format.
_LIT( KStatusMessageHeaderFormat, "%3d" );
_LIT( KStatusMessageHeaderLengthFormat, "%d" ); // This is length of header.
_LIT( KWVSettingsResFileName, "CWVSETTINGSUING.RSC" );
_LIT( KIMPSConnUIResFileName, "IMPSCONNECTIONUING.RSC" );
// The IM UI Settings are in the Resource file now.
_LIT( KIMPSCUVariationResFileName, "IMConnectionUIVariationNG.RSC" );

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAAppUi::CCAAppUi( CCAEngine& aEngine, MCAProcessManager& aProcessManager )
        : iEngine( aEngine ),
        iApplicationNGPC( *aProcessManager.GetApplicationNGPC()  ),
        iIsAppForeground( ETrue ),
        iLayoutObservers( 1 ) /* usually just one observer */,
        iPlayOnce( EFalse ),
        iPendingDialogCounter( 0 ),
        iProcessManager( aProcessManager )

    {
    CHAT_DP_FUNC_ENTER( "CCAAppUi" );
    CHAT_DP_FUNC_DONE( "CCAAppUi" );
    }

// Destructor
CCAAppUi::~CCAAppUi()
    {

    if ( iInfoPopup )
        {
        iInfoPopup->HideInfoPopupNote();
        delete iInfoPopup;
        }

    delete iPreviewMessageList;

    iDestructingFlag = ETrue;

    // Remove observers if they are already set
    if ( iEngineInitialized )
        {

        MCAGlobalNotificationObserverPC& globalNotificationObserverPC ( *iProcessManager.GetGlobalNotificationInterface() );

        globalNotificationObserverPC.RemoveGlobalObserver( this );

        }

    FeatureManager::UnInitializeLib();
    //delete splash screen
    delete iSplashScreen;
    iSplashScreen = NULL;

    delete iConnUIAgent;
    delete iIconLoader;
    delete iWaitDialog;
    delete iGroupUtils;
    delete iConnUI;
    delete iUISessionManager;
    delete iScreenName;
    delete iStatusPaneHandler;
    delete iAudioManager;
    // delete iInputAbsorber;
    delete iExitIdle;
    delete iForwardMessage;
    delete iMessageHandler;

    delete iNotifierAPI;

    iLayoutObservers.Close();
    iResourceObservers.Close();

    delete iCustomMessage;
    delete iLayoutInfo;

    delete iStatusMessage;

    iSwitchView.Close();
    iSwitchMessage.Close();
    iSwitchMessageContent.ResetAndDestroy();
    iSwitchMessageContent.Close();


    delete iSAPSettingsStore;
    delete iInputAbsorber;

    CCACommandManagerFactory::ReleaseResources();
    // The Central repository instance needs to be deleted, as it would have been instantiated by InitializeLibL () at the beginning of AppUi
    //have been instantiated by InitializeLibL () at the beginning of AppUi
    IMUtils::UnInitializeLib();

    CHAT_DP_TXT( "CCAAppUi::~CCAAppUi done" );
    }


// ---------------------------------------------------------
// CCAAppUi::ConnectionUI
// (other items were commented in a header).
// ---------------------------------------------------------
//
CIMPSPresenceConnectionUi& CCAAppUi::ConnectionUI()
    {
    return *iConnUI;
    }

// ---------------------------------------------------------
// CCAAppUi::UISessionManager
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAUISessionManager& CCAAppUi::UISessionManager()
    {
    return *iUISessionManager;
    }

// ---------------------------------------------------------
// CCAAppUi::CAStatusPane() const
// Fetches a handle to status pane handler
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAStatusPaneHandler* CCAAppUi::CAStatusPane() const
    {
    return iStatusPaneHandler;
    }

// ---------------------------------------------------------
// CCAAppUi::CAModel() const
// Fetches a handle to application's engine
// (other items were commented in a header).
// ---------------------------------------------------------
//
//CCAEngine& CCAAppUi::CAModel() const
//    {
//    return iEngine;
//   }

// ---------------------------------------------------------
// CCAAppUi::GetProcessManager() const
// Returns a reference to application's process manager
// (other items were commented in a header).
// ---------------------------------------------------------
//
MCAProcessManager& CCAAppUi::GetProcessManager() const
    {
    return iProcessManager;
    }


// ---------------------------------------------------------
// CCAAppUi::ScreenNamePtr
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCAAppUi::ScreenNamePtr()
    {
    MCASettingsPC* appSettingsPC = GetProcessManager().GetSettingsInterface();
    TPtr screenNamePtr( iScreenName->Des() );

    // check if there is default screen name specified in settings
    // and override the existing screen name with the settings value
    if ( appSettingsPC->GetBoolValuePC( TEnumsPC::EDefaultScreenNameInUse, NULL ) )
        {
        HBufC* defaultScreenName = NULL;
        TRAPD( err, defaultScreenName =
                   appSettingsPC->GetSapSettingValuePCL( TEnumsPC::EDefaultScreenName, NULL ) );
        if ( err )
            {
            CActiveScheduler::Current()->Error( err );
            }

        if ( !defaultScreenName )
            {
            return iScreenName;
            }

        TPtr defaultScreenNamePtr( defaultScreenName->Des() );
        screenNamePtr.Copy( defaultScreenNamePtr.Left( screenNamePtr.MaxLength() ) );
        delete defaultScreenName;
        }

    // otherwise just return the last used screen name
    return iScreenName;
    }

// ---------------------------------------------------------
// CCAAppUi::IMUIPIConForcedClear
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::IMUIPIConForcedClear()
    {
    TInt error( iNotifierAPI->SetInt( MCANotifierInterface::EUIPIndKey, EFalse ) );
    if ( error )
        {
        CActiveScheduler::Current()->Error( error );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::SetSoftNotificationCounter
// (other items were commented in a header).
// ---------------------------------------------------------
//
void  CCAAppUi::SetSoftNotificationCounterL( TInt aCount ) const
    {
    CAknSoftNotifier* softNotifier = CAknSoftNotifier::NewLC();
    softNotifier->SetNotificationCountL( EChatMessageNotification, aCount );
    CleanupStack::PopAndDestroy( softNotifier );
    }

// ---------------------------------------------------------
// CCAAppUi::PlayBeep
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::PlayBeep()
    {
    if ( !iPlayOnce )
        {
        TRAP_IGNORE( iAudioManager->PlayL() );
        }
    else
        {
        // Set beep to be played next time when PlayBeep
        // is called
        iPlayOnce = EFalse;
        }
    }

// ---------------------------------------------------------
// CCAAppUi::MbmFullPath
// (other items were commented in a header).
// ---------------------------------------------------------
//
TDes& CCAAppUi::MbmFullPath()
    {
    return iChatMbmFullPath;
    }

// ---------------------------------------------------------
// CCAAppUi::CaptureEventsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::CaptureEventsL()
    {
    if ( !AreEventsCaptured() )
        {
        iInputAbsorber->CaptureEventsL();
        }
    }

// ---------------------------------------------------------
// CCAAppUi::ReleaseCapturingL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::ReleaseCapturingL()
    {
    if ( AreEventsCaptured() )
        {
        iInputAbsorber->Release();
        }
    }

// ---------------------------------------------------------
// CCAAppUi::AreEventsCaptured
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAppUi::AreEventsCaptured() const
    {
    return iInputAbsorber->IsCaptured();
    }

// ---------------------------------------------------------
// CCAAppUi::AddLayoutChangeObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::AddLayoutChangeObserver( MCALayoutChangeObserver* aObserver )
    {
    TInt err( iLayoutObservers.Append( aObserver ) );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::RemoveLayoutChangeObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::RemoveLayoutChangeObserver( MCALayoutChangeObserver* aObserver )
    {
    // remove from end first (LIFO) to avoid
    // tweaking the index while removing
    TInt count( iLayoutObservers.Count() );
    for ( TInt i( count - 1 ); i >= 0; i-- )
        {
        if ( iLayoutObservers[i] == aObserver )
            {
            iLayoutObservers.Remove( i );
            }
        }
    iLayoutObservers.Compress();
    }

// ---------------------------------------------------------
// CCAAppUi::AddResourceChangeObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::AddResourceChangeObserver( MCAResourceChangeObserver* aObserver )
    {
    TInt err( iResourceObservers.Append( aObserver ) );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::RemoveResourceChangeObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::RemoveResourceChangeObserver( MCAResourceChangeObserver* aObserver )
    {
    // remove from end first (LIFO) to avoid
    // tweaking the index while removing
    TInt count( iResourceObservers.Count() );
    for ( TInt i( count - 1 ); i >= 0; i-- )
        {
        if ( iResourceObservers[i] == aObserver )
            {
            iResourceObservers.Remove( i );
            }
        }
    iResourceObservers.Compress();
    }

// ---------------------------------------------------------
// CCAAppUi::NotifyResourceChange
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::NotifyResourceChangeL()
    {
    TInt count( iResourceObservers.Count() );
    TInt err = KErrNone;
    for ( TInt i( 0 ); i < count; ++i )
        {
        // Notify every observer (even in case of leave)
        TRAPD( error, iResourceObservers[i]->ResourceChangedL() );

        // store last error
        if ( error != KErrNone )
            {
            err = error;
            }
        }

    // Show that something went wrong (only last error)
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::HandleInvitation
// handle an incoming invitation
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::HandleInvitationEventL( TEnumsPC::TInviteType aInviteType,
                                       const TDesC& aUserID,
                                       const TDesC& aMessage )
    {
    CHAT_DP_TXT( "CCAAppUi::HandleInvitation, Invitation received." );

    if ( aInviteType == TEnumsPC::ENewInvite )
        {
        AddToPreviewListL( aUserID, aMessage );

        if ( !iInfoNoteShowing )
            {
            ShowInfoPopupL();
            }
        PlayBeep();
        }
    else if ( aInviteType != TEnumsPC::EInviteRead )
        {
        return;
        }
    UpdateIMUIPIcon();
    }

// ---------------------------------------------------------
// CCAAppUi::HandleInvitationResponse
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::HandleInvitationResponse( TBool aAcceptance,
                                         const TDesC& aUserId,
                                         const TDesC& aGroupName,
                                         const TDesC& aResponse )
    {
    CHAT_DP_TXT( "CCAAppUi::HandleInvitationResponse" );
    PlayBeep();

    TRAPD( err, DoHandleInvitationResponseL(
               aAcceptance, aUserId, aGroupName, aResponse ) );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::HandleWsEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination )
    {
    if (aEvent.Key()->iCode == EKeyEscape && iGroupUtils)
        {
        if (iGroupUtils->GetJoinGroupID() .Compare(KEmptyDesC) != KErrNone)
            {
            iGroupManagerInterface = iEngine.GetGroupInterface();
            iGroupManagerInterface->CancelGroupOperationL(iGroupUtils->GetJoinGroupID());   
            }
        }

    if (aEvent.Key()->iCode == EKeyEscape)
        {
         iIsFSWExit = ETrue;   
        }
    
    switch ( aEvent.Type() )
        {
        case  KAknUidValueEndKeyCloseEvent:
            {
            // Chat goes to background only, does not close the app because
            // we'd have to show a query; not possible when pressing end key
            // a long time
            TVwsViewId idleViewid;
            TInt res = AknDef::GetPhoneIdleViewId( idleViewid );
            if ( res != KErrNone ) //go to application shell if no idle available
                {
                /*ActivateViewL( KAknApplicationShellViewId );*/
                TApaTaskList taskList(CCoeEnv::Static()->WsSession());
                TApaTask chatng = taskList.FindApp( KUidChatClient );
                
                // Send the IM to background
                chatng.SendToBackground();
                }
            else
                {
                ActivateViewL( idleViewid );
                }
            break;
            }
        default:
            {
            CAknViewAppUi::HandleWsEventL( aEvent, aDestination );
            break;
            }
        }
    }


// ---------------------------------------------------------
// CCAAppUi::HandleCommandL(TInt aCommand)
// From CEikAppUi, takes care of command handling.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::HandleCommandL( TInt aCommand )
    {
    CHAT_DP( D_CHAT_LIT( "AppUI received command: %d" ), aCommand );
    CHAT_DP( D_CHAT_LIT( "--EEikCmdExit is %d" ), ( TInt )EEikCmdExit );
    CHAT_DP( D_CHAT_LIT( "--EAknCmdExit is %d" ), ( TInt )EAknCmdExit );
    CHAT_DP( D_CHAT_LIT( "--EAknSoftkeyExit is %d" ), ( TInt )EAknSoftkeyExit );
    CHAT_DP( D_CHAT_LIT( "--EChatClientCmdExit is %d" ),
             ( TInt )EChatClientCmdExit );

    switch ( aCommand )
        {
        case EAknCmdExit:           // fall-through, handled similarily
        case EEikCmdExit:           // fall-through, handled similarily
        case EAknSoftkeyExit:       // fall-through, handled similarily
        case EChatClientCmdExit:
            {
            if ( IsBackgroundTaskPendingL() )
                {
                iIsFSDExitNeeded = ETrue;
                break;
                }
            else
                {
                iStopUpdateCba = EFalse;
                }

            CHAT_DP_FUNC_DP( "HandleCommandL", "exit command" );
            // exit softkey:            EAknSoftkeyExit
            // exit from menu:          EChatClientCmdExit
            // forced exit (oom):       EEikCmdExit
            // forced exit (fastswap):  EEikCmdExit

            TInt ret( EAknSoftkeyOk );
            TInt chatLoginType( IMUtils::WVSettingsChatLoginValue() );

            if ( ( aCommand != EEikCmdExit ) &&
                 ( chatLoginType != EWVSettingsChatLoginAutoAlways ) &&
                 ( chatLoginType != EWVSettingsChatLoginAutoInHomeNW ) )
                {
                // don't show query if user is not logged in or if this
                // is a forced exit
                // checking login status only when needed, ie. no need
                // check if forced exit or login type is always online
                if ( iUISessionManager->IsLoggedIn() )
                    {
                    CCAGlobalNoteHandler* note = CCAGlobalNoteHandler::NewL();
                    CleanupStack::PushL( note );

                    ret = note->ShowNoteL( R_QTN_CHAT_CLOSE_CONN_APP_CONV );
                    CleanupStack::PopAndDestroy( note );
                    ReleaseCapturingL();
                    }
                }
            else if ( aCommand != EEikCmdExit   )
                {
                iUISessionManager->LoginL( MCAUiLoginCmdCB::EExit );
                }

            // Check what the user responded
            if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
                {
                iEngine.GetBlockingInterface()->SetAppUiExitFlag( ETrue );
                iQueryAccepted = ETrue;
                CHAT_DP_FUNC_DP( "HandleCommandL", "Dismissing wait dialog..." );
                DismissWaitDialogL( KErrNone );
                CHAT_DP_FUNC_DP( "HandleCommandL", "done" );
                CHAT_DP_FUNC_DP( "HandleCommandL", "ShutDialogsL..." );
                AknDialogShutter::ShutDialogsL( *iEikonEnv );
                CHAT_DP_FUNC_DP( "HandleCommandL", "done." );
                CHAT_DP_FUNC_DP( "HandleCommandL", "IMUIPIconForcedClear..." );
                IMUIPIConForcedClear();
                CHAT_DP_FUNC_DP( "HandleCommandL", "done" );
                CHAT_DP_FUNC_DP( "HandleCommandL", "CancelAllRequests..." );
                iProcessManager.GetLogInInterface()->CancelAllRequests();
                CHAT_DP_FUNC_DP( "HandleCommandL", "done" );
                // Reset soft notification counter so that
                // the note does not pop up after exit.
                CHAT_DP_FUNC_DP( "HandleCommandL", "Reset SoftNotif count.." );
                TRAPD( err, SetSoftNotificationCounterL( 0 ) );

                if ( err )
                    {
                    CActiveScheduler::Current()->Error( err );
                    }
                if ( ( chatLoginType != EWVSettingsChatLoginAutoAlways ) &&
                     ( chatLoginType != EWVSettingsChatLoginAutoInHomeNW ) )
                    {
                    CHAT_DP_FUNC_DP( "HandleCommandL", "LogoutL..." );
                    TRAP( err, iUISessionManager->LoginL( MCAUiLoginCmdCB::ELogout ) );
                    if ( err )
                        {
                        CActiveScheduler::Current()->Error( err );
                        }
                    CHAT_DP_FUNC_DP( "HandleCommandL", "done..." );

                    }

                // If we have some asynchronously launched dialogs, we need
                // to give them time to return.
                if ( iProcessManager.GetLogInInterface()->ReadyForShutdown() && iPendingDialogCounter <= 0 )
                    {
                    TInt level = LevelOfActiveScheduler();
                    if( level > 1 )
                    	{
                    	CActiveScheduler::Current()->Stop();
                    	}
                    CHAT_DP_FUNC_DP( "HandleCommandL",
                                     "Engine is ready for shutdown & no async dialogs pending" );
                    Exit();
                    }
                else
                    {
                    CHAT_DP_FUNC_DP( "HandleCommandL",
                                     "Engine is NOT ready for shutdown or async dialogs pending" );
                    CHAT_DP_FUNC_DP( "HandleCommandL", "Starting exitIdle." );
                    if ( !iExitIdle->IsActive() )
                        {
                        iExitIdle->Start( TCallBack( ExitCallback, this ) );
                        }
                    }
                }
            else
                {
                // Connection and application is not closed.
                // Go to application shell.
                /*ActivateViewL( KAknApplicationShellViewId );*/
                TApaTaskList taskList(CCoeEnv::Static()->WsSession());
                TApaTask chatng = taskList.FindApp( KUidChatClient );
                
                // Send the IM to background
                chatng.SendToBackground();
                }

            break;
            }
        case EChatClientMainViewCmdLogin:
            {
            TBool loginDone = EFalse;
            TRAPD( err, loginDone = iUISessionManager->LoginL( MCAUiLoginCmdCB::ELogin ) );
            if ( err != KErrNone || ! loginDone )
                {
                // got some error, build the views before passing leave code
                // forward
                FinalizeChatConstructionL();
                if ( err != KErrNone )
                    {
                    // IMPS Common UI has shown the error notes
                    User::Leave( err );
                    }
                }

            break;
            }
        case EChatClientMainViewCmdLogout:
            {
            iProcessManager.GetLogInInterface()->CancelAllRequests();
            iUISessionManager->LoginL( MCAUiLoginCmdCB::ELogout );
            break;
            }
        case EChatPrivateListCmdSettings:   // fall-through, handled similarily
        case EChatClientMainViewSettings:   // fall-through, handled similarily
        case EChatClientChatListSettings:
            {
            if ( !IsBackgroundTaskPendingL() )
                {
                // check for backgroundtask complete or not
#ifdef RD_SETTINGS_FACELIFT
                LaunchServerSettingsDialogL();
#else
                SwitchViewL( KUidSettingsView );
#endif

                // When the settings dialog exit, set it back
                iStopUpdateCba = EFalse;
                }
            break;
            }
        case EChatClientChatListViewCmdJoin:
            {
            SwitchViewL( KUidChatView );
            break;
            }
        case EChatClientMainViewCmdChangeStatus:
            {
            ChangeStatusProcessL();
            break;
            }
        case EChatClientCmdStatusMessage:
            {

            MCAMainViewArrayPC* mainViewArrayPC = iProcessManager.GetArrayInterface();
            TInt topIndex( 0 ) ;
            if ( mainViewArrayPC->GetType( topIndex ) == TEnumsPC::EOwnStatusItem )
                {
                AskStatusMessageL( mainViewArrayPC->GetOnlineStatus( topIndex ), ETrue );
                }
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                                 AppHelpContextL() );
            break;
            }

        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAAppUi::DoHandleInvitationResponseL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::DoHandleInvitationResponseL( TBool aAcceptance,
                                            const TDesC& aUserId,
                                            const TDesC& aGroupName,
                                            const TDesC& aResponse )
    {
    // load the title texts
    HBufC* title = iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_INVITE_RESPONSE );

    TInt resourceId = R_QTN_CHAT_INVITE_REJECTED;
    if ( aAcceptance )
        {
        resourceId = R_QTN_CHAT_INVITE_ACCEPT;
        }

    HBufC* acceptreject = iEikonEnv->AllocReadResourceLC( resourceId );

    HBufC* group = iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_INVITE_GROUP );
    HBufC* message = iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_INVITE_MESSAGE );

    // find the identification for contact
    TPtrC identification( DisplayId( aUserId ) );
    identification.Set( iApplicationNGPC.Identification( aUserId ) );

    // calculate the length of the buffer
    TInt lfLength( KLineFeed().Length() );
    TInt length(
        message->Length() + lfLength +
        aResponse.Length() + lfLength +

        acceptreject->Length() + lfLength +
        identification.Length() + lfLength +
        group->Length() + lfLength +
        aGroupName.Length()
    );

    HBufC* msgBuf = HBufC::NewLC( length );
    TPtr msg( msgBuf->Des() );
    msg.Zero();

    if ( !aAcceptance && ( aResponse.Length() > 0 ) )
        {
        // rejected response has the reason also
        msg.Append( message->Des() );
        msg.Append( KLineFeed );
        msg.Append( aResponse );
        msg.Append( KLineFeed );
        }

    // accepted response (and the end of rejected response)
    msg.Append( acceptreject->Des() );
    msg.Append( KLineFeed );
    msg.Append( identification );
    msg.Append( KLineFeed );
    msg.Append( group->Des() );
    msg.Append( KLineFeed );
    msg.Append( CCAPCUtils::DisplayId( aGroupName ) );

    IMDialogUtils::DisplayMessageQueryDialogL(
        R_CHATCLIENT_INVITE_RESPONSE_DIALOG,
        msg, title->Des(),
        ETrue  // T9
    );

    CleanupStack::PopAndDestroy( 5, title ); // msgBuf, message, group, acceptreject, title
    }

// ---------------------------------------------------------
// CCAAppUi::ReceivedMsgs
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAAppUi::ReceivedInvites() const
    {
    return iProcessManager.GetInvitationsInterface()->UnreadInvitesCount();
    }

// ---------------------------------------------------------
// CCAAppUi::GroupUtils
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAGroupUtils* CCAAppUi::GroupUtils()
    {
    return iGroupUtils;
    }

// ---------------------------------------------------------
// CCAAppUi::SetForwardMessageL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::SetForwardMessageL( const MCAConversationMessage* aFwdMessage )
    {
    // if NULL
    if ( !aFwdMessage )
        {
        delete iForwardMessage;
        iForwardMessage = NULL;
        }
    // if not the same message
    else if ( iForwardMessage != aFwdMessage )
        {
        delete iForwardMessage;
        iForwardMessage = NULL;
        iForwardMessage = IMMessageUtilsPC::CloneMessageL( *aFwdMessage );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::ForwardMessage
// (other items were commented in a header).
// ---------------------------------------------------------
//
const MCAConversationMessage* CCAAppUi::ForwardMessage()
    {
    return iForwardMessage;
    }

// ---------------------------------------------------------
// CCAAppUi::ShowWaitDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::ShowWaitDialogL( const TDesC& aText,
                                TBool aShowImmediately /*= EFalse*/ )
    {
    if ( iWaitDialog )
        {
        // hide the previous dialog and wait for it to really disappear
        DismissWaitDialogL( KErrNone, ETrue );
        }

    // start capturing the events
    CaptureEventsL();
    // show the dialog
    iWaitDialogDismissed = EFalse;
    iWaitDialog = new( ELeave )CAknWaitDialog( ( CEikDialog** )&iWaitDialog,
                                               aShowImmediately );
    iWaitDialog->PrepareLC( R_CHAT_WAIT_NOTE_TEMPLATE );
    iWaitDialog->SetTone( CAknNoteDialog::ENoTone );
    iWaitDialog->SetTextL( aText );
    iWaitDialog->SetCallback( this );
    iWaitDialog->RunLD();   // CSI: 50 # iWaitDialog is not owned by us
    }

// ---------------------------------------------------------
// CCAAppUi::ShowWaitDialogLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::ShowWaitDialogLC( const TDesC& aText,
                                 TBool aShowImmediately /* = EFalse  */ )
    {
    ShowWaitDialogL( aText, aShowImmediately );
    CleanupStack::PushL( TCleanupItem( CCAAppUi::CloseDialog, this ) );
    }

// ---------------------------------------------------------
// CCAAppUi::ShowWaitDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::ShowWaitDialogL( TInt aTextResource,
                                TBool aShowImmediately /*= EFalse*/ )
    {
    HBufC* text = iEikonEnv->AllocReadResourceLC( aTextResource );
    ShowWaitDialogL( *text, aShowImmediately );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------
// CCAAppUi::ShowWaitDialogLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::ShowWaitDialogLC( TInt aTextResource,
                                 TBool aShowImmediately /*= EFalse*/ )
    {
    ShowWaitDialogL( aTextResource, aShowImmediately );
    CleanupStack::PushL( TCleanupItem( CCAAppUi::CloseDialog, this ) );
    }

// ---------------------------------------------------------
// CCAAppUi::DismissWaitDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::DismissWaitDialogL( TInt aErrorCode,
                                   TBool aWaitUntilDismissed /*= EFalse*/,
                                   const TDesC& aAdditionalData )
    {
    if ( iWaitDialog )
        {
        TRAPD( err, iWaitDialog->ProcessFinishedL() );
        if ( err != KErrNone )
            {
            // don't know for sure that ProcessFinishedL deletes the dialog
            // in all cases, so let's delete it
            delete iWaitDialog;
            iWaitDialog = NULL;
            iWaitDialogDismissed = ETrue;
            User::Leave( err );
            }
        else
            {
            // start waiting for actual hiding if requested
            if ( !iWaitDialogDismissed && aWaitUntilDismissed )
                {
                if ( !iWait.IsStarted() )
                    {
                    iWait.Start();  // CSI: 10 # iWait is not an active object
                    }
                }
            }
        // stop capturing the events
        ReleaseCapturingL();
        }

    // leave on everything else but imps errors (and timeout)
    if ( ( aErrorCode > Imps_ERROR_BASE ) && ( aErrorCode != KErrTimedOut )
         && ( aErrorCode != KErrCancel ) )
        {
        User::LeaveIfError( aErrorCode );
        }
    else
        {
        // otherwise show error note, if this is really an error
        IMNoteMapper::ShowNoteL( aErrorCode, aAdditionalData );
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::IconLoader
// Return iconloader
// -----------------------------------------------------------------------------
//
CCAIconLoader& CCAAppUi::IconLoader() const
    {
    if ( !iIconLoader )
        {
        CActiveScheduler::Current()->Error( KErrNotFound );
        }
    return *iIconLoader;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::MessageHandler
// Return message handler
// -----------------------------------------------------------------------------
//
CCAMessageExtensionsHandler& CCAAppUi::MessageHandler() const
    {
    if ( !iMessageHandler )
        {
        CActiveScheduler::Current()->Error( KErrNotFound );
        }
    return *iMessageHandler;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::DisplayId
// Hide some parts of userid/groupid/listid
// -----------------------------------------------------------------------------
//
TPtrC CCAAppUi::DisplayId( const TDesC& aId, TBool aListHiding /* = EFalse */ )
    {
    return CCAPCUtils::DisplayId( aId, aListHiding );
    }

// -----------------------------------------------------------------------------
// CCAAppUi::ReadBrandedResourcesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::ReadBrandedResourcesL()
    {
    // check if we have resource-file variated "wv:"-part showing support
    // enabled and cache it to storage (everything uses that)
    TInt hideWV = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HIDE_WV );
    iApplicationNGPC.SetWVHiding( hideWV ? ETrue : EFalse );

    // UI CR : Domain part showing. Only prefix hiding
    TInt hideWVprefix = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HIDE_ONLY_PREFIX );
    iApplicationNGPC.SetWVHidingPrefixOnly( hideWVprefix ? ETrue : EFalse );

    TInt capitalEnabled = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_CONTACT_LISTS_CAPITALIZED );
    iApplicationNGPC.SetCapitalizingEnabled( capitalEnabled ? ETrue : EFalse );

    // Not used in backported 3.0 environment
    TInt overRideDefaultTone =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_OVERRIDE_DEFAULT_TONE );
    if ( overRideDefaultTone && iUISessionManager )
        {

        // When application is started there might not be any SAPs defined,
        // ignore KErrNotFound leave.
        CIMPSSAPSettings* currentSap = NULL;
        TRAPD( err,
               currentSap = iUISessionManager->CurrentSAPLC();
               // currentSap has to be popped before exiting TRAP harness
               CleanupStack::Pop( currentSap );
             );  // TRAPD

        // Push it back to CleanupStack
        CleanupStack::PushL( currentSap );

        if ( err == KErrNone )
            {
            TPtrC currentTone;
            TInt retVal =
                currentSap->GetOpaqueDesC16( KIMAlertTonePath(), currentTone );
            if ( retVal == KErrNotFound )
                {
                // Set default sound only once from resources
                HBufC* tone = iCoeEnv->AllocReadResourceAsDes16LC( R_DEFAULT_TONE );
                currentSap->SetOpaqueDesC16( KIMAlertTonePath(), *tone );
                iSAPSettingsStore->UpdateOldSAPL( currentSap, currentSap->Uid() );
                CleanupStack::PopAndDestroy( tone );
                }
            }

        CleanupStack::PopAndDestroy( currentSap );
        }
    // this method is also done when we get a SAP change event
    }

// ---------------------------------------------------------
// CCAAppUi::DialogDismissedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::DialogDismissedL( TInt /*aButtonId*/ )
    {
    iWaitDialogDismissed = ETrue;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    // stop capturing the events
    ReleaseCapturingL();
    }

// ---------------------------------------------------------
// CCAAppUi::SwitchViewBackL( TUid aViewId )
//
// ---------------------------------------------------------
//
void CCAAppUi::SwitchViewBackL( TUid aViewId, TUid aCustomMessageId /* = NULL*/,
                                const TDesC8& aCustomMessage /* = KNullDesC*/ )
    {
    TInt switchViewIndex = iSwitchView.Count() - 1;
    TInt switchMsgIndex = iSwitchMessage.Count() - 1;
    TInt switchMsgContentIndex = iSwitchMessageContent.Count() - 1;

    if ( switchViewIndex >= 0 && iSwitchMessageContent[ switchMsgContentIndex ] )
        {
        TRAPD( err, SwitchViewL( iSwitchView[ switchViewIndex ],
                                 iSwitchMessage[ switchMsgIndex ],
                                 *iSwitchMessageContent[ switchMsgContentIndex ] ) );

        if ( iSwitchView[ switchViewIndex ] == KUidChatView ||
             iSwitchView[ switchViewIndex ] == KUidConversationsView ||
             iSwitchView[ switchViewIndex ] == KUidRecordedChatView )

            {
            iLastView = TUid::Uid( 0 );
            }

        iSwitchView[ switchViewIndex ] = TUid::Uid( 0 );
        iSwitchMessage[ switchMsgIndex ] = TUid::Uid( 0 );

        delete iSwitchMessageContent[ switchMsgContentIndex ];
        iSwitchMessageContent[ switchMsgContentIndex ] = NULL;

        iSwitchView.Remove( switchViewIndex );
        iSwitchMessage.Remove( switchMsgIndex );
        iSwitchMessageContent.Remove( switchMsgContentIndex );

        User::LeaveIfError( err );

        return;
        }
    else if ( switchViewIndex >= 0 && iSwitchView[ switchViewIndex ] !=
              TUid::Uid( 0 ) )
        {
        SwitchViewL( iSwitchView[ switchViewIndex ] );
        iSwitchView[ switchViewIndex ] = TUid::Uid( 0 );
        iSwitchView.Remove( switchViewIndex );
        return;
        }

    if ( iLastView != TUid::Uid( 0 )  & iLastView != KUidInvitationView )
        {
        SwitchViewL( iLastView, aCustomMessageId, aCustomMessage );
        iLastView = TUid::Uid( 0 );
        }
    else
        {
        SwitchViewL( aViewId, aCustomMessageId, aCustomMessage );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::SwitchViewL( TUid aViewId )
// From MCAViewSwitcher, handles view-switch requests
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::SwitchViewL( TUid aViewId )
    {
    CHAT_DP( D_CHAT_LIT( "CCAAppUi::SwitchViewL, requested view: %d" ),
             aViewId.iUid );

    if ( iIsAppForeground )
        {
        CHAT_DP_FUNC_DP( "SwitchViewL", "Application was in foreground" );
        TUid myLastView = ActiveViewId();
        if ( myLastView != aViewId && myLastView != KUidRefreshView )
            {
            iLastView = myLastView;
            }

        ActivateLocalViewL( aViewId );

        iOkToSwitch = EFalse;
        }
    else
        {
        iOkToSwitch = ETrue;
        }

    iActiveViewId = aViewId;
    iCustomMessageId = KNullUid;
    delete iCustomMessage;
    iCustomMessage = NULL;

    ReleaseCapturingL();
    }

// ---------------------------------------------------------
// CCAAppUi::SwitchViewL
// From MCAViewSwitcher, handles view-switch requests
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::SwitchViewL( TUid aViewId,
                            TUid aCustomMessageId,
                            const TDesC8& aCustomMessage )
    {
    CHAT_DP( D_CHAT_LIT(
                 "CCAAppUi::SwitchViewL, requested view: %d, messageID: %d, message: %S" ),
             aViewId.iUid, aCustomMessageId.iUid, &aCustomMessage );

    if ( iIsAppForeground )
        {
        CHAT_DP_FUNC_DP( "SwitchViewL", "Application was in foreground" );
        TUid myLastView = ActiveViewId();
        if ( myLastView != aViewId && myLastView != KUidRefreshView )
            {
            iLastView = myLastView;
            }

        ActivateLocalViewL( aViewId, aCustomMessageId, aCustomMessage );

        iOkToSwitch = EFalse;
        }
    else
        {
        iOkToSwitch = ETrue;
        }

    HBufC8* tmp = aCustomMessage.AllocLC();
    delete iCustomMessage;
    iCustomMessage = tmp;
    CleanupStack::Pop(); // tmp

    iActiveViewId = aViewId;
    iCustomMessageId = aCustomMessageId;

    ReleaseCapturingL();
    }

// ---------------------------------------------------------
// CCAAppUi::RegisterSwitchBack
// From MCAViewSwitcher
// ---------------------------------------------------------
//
void CCAAppUi::RegisterSwitchBack( TUid aViewId,
                                   TUid aCustomMessageId,
                                   HBufC8* aCustomMessage,
                                   TUid aNextView )
    {
    TUid prevViewId;

    if ( iSwitchView.Count() > 0 )
        {
        prevViewId = iSwitchView[ iSwitchView.Count() - 1 ];
        }
    else
        {
        prevViewId = KNullUid;
        }

    // Check if we are forwarding for the second time
    if ( ( prevViewId == KUidChatView || prevViewId == KUidConversationsView )
         && ( aViewId == KUidChatView || aViewId == KUidConversationsView ) )
        {
        if ( iSwitchView.Count() > 0 )
            {
            // remove the previous view
            delete iSwitchMessageContent[ iSwitchMessageContent.Count()-1 ];
            iSwitchMessageContent[ iSwitchMessageContent.Count()-1 ] = NULL;

            iSwitchView.Remove( iSwitchView.Count() - 1 );
            iSwitchMessage.Remove( iSwitchMessage.Count() - 1 );
            iSwitchMessageContent.Remove( iSwitchMessageContent.Count() - 1 );
            }

        delete aCustomMessage;

        if ( aNextView == KUidConversationsView )
            {
            // add the conversation list view.
            iSwitchView.Append( KUidChatPrivateChatListView );
            }
        else // aNextView == KUidChatView
            {
            // add the chat group list view.
            iSwitchView.Append( KUidChatListView );
            }

        iSwitchMessage.Append( KNullUid );
        iSwitchMessageContent.Append( NULL );
        }
    else
        {
        // normal behaviour
        iSwitchView.Append( aViewId );
        iSwitchMessage.Append( aCustomMessageId );
        iSwitchMessageContent.Append( aCustomMessage );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::DeRegisterSwitchBack
// From MCAViewSwitcher
// ---------------------------------------------------------
//
void CCAAppUi::DeRegisterSwitchBack( TUid aViewId,
                                     const TDesC& aTarget )
    {
    TInt index = 0;
    TInt count = iSwitchView.Count();

    // Check all views in queue
    for ( ; index < count; index++ )
        {
        if ( aViewId == iSwitchView[index] )
            {
            TInt compare = -1;

            // Check if this is view to be deregistered
            if ( aViewId == KUidChatView )
                {
                TCADnlChatViewBuf viewBuf;
                viewBuf.Copy( *iSwitchMessageContent[index] );
                compare = CAUtils::NeutralCompare( aTarget, viewBuf().iGroupId );
                }
            else if ( aViewId == KUidConversationsView )
                {
                TCADnlConvViewBuf viewBuf;
                viewBuf.Copy( *iSwitchMessageContent[index] );
                compare = CAUtils::NeutralCompare( aTarget, viewBuf().iWVID );
                }

            // Deregister view
            if ( compare == 0 )
                {
                iSwitchView.Remove( index );
                iSwitchMessage.Remove( index );
                delete iSwitchMessageContent[index];
                iSwitchMessageContent.Remove( index );
                count = iSwitchView.Count();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::ViewExists
// Checks if view exists
// -----------------------------------------------------------------------------
//
TBool CCAAppUi::ViewExists( TUid aViewId )
    {
    if ( View( aViewId ) )
        {
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCAAppUi::ActiveViewId
// -----------------------------------------------------------------------------
//
TUid CCAAppUi::ActiveViewId()
    {
    TVwsViewId viewId;
    GetActiveViewId( viewId );
    return viewId.iViewUid;
    }

// ---------------------------------------------------------
// CCAAppUi::HandleResourceChangeL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknViewAppUi::HandleResourceChangeL( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // skin change for this app ui class
        iLayoutInfo->LayoutChangedL( aType );

        CalculatePreviewPaneWidth();

        // Update skin layout
        CCAVariantFactory* varFact =
            static_cast<CCAApp*>( Application() )->VariantFactory();
        varFact->SkinVariantL()->UpdateLayout();
        }


    if ( aType == KEikDynamicLayoutVariantSwitch ||
         aType == KAknsMessageSkinChange )
        {
        TInt count( iLayoutObservers.Count() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            iLayoutObservers[i]->LayoutChangedL( aType );
            }
        // Set the icons
        if ( iUISessionManager )
            {
            iUISessionManager->SetStatusPaneIconsL();
            }
        }
    }

// ---------------------------------------------------------
// CCAAppUi::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CCAAppUi::ConstructL()
    {
    CHAT_DP_TXT( "CCAAppUi::ConstructL() starting" );

    // The Settings are now stored in Cenrep, and it needs to be fetched using the instance of CRepository, so InitializeLibL will create the instance of CRepository
    //using the instance of CRepository, so InitializeLibL will create the instance of CRepository
    IMUtils::InitializeLibL();
    iIsFSWExit = EFalse;
    iDestructingFlag = EFalse;

    BaseConstructL( EAknEnableSkin | ENoAppResourceFile |
                    EAppOrientationAutomatic | EAknEnableMSK );


    //create Splash Screen
    iSplashScreen = CCASplashScreen::NewL( this );
    CAVersion::UpdateVersion( iCoeEnv->FsSession()
                              , KPrivateVersionFile
                              , KPublishedVersionFile );

    // Get mem low note variation status
    iShowMemLowNote =
        ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
          & EIMFeatMemLowWarningNote );

    // Read CR variation flag
    iBrandableToneList =
        ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
          & EIMFeatBrandableTones );

    iStatusMessage = HBufC::NewL( KPEngMaxWVStatusTextLength );

    iLayoutInfo = CCALayoutInfo::NewL( TChatListBoxLayout::EWithoutEditor );
    iLayoutInfo->LayoutChangedL();

    CalculatePreviewPaneWidth();

    //Creating exit idle
    iExitIdle = CIdle::NewL( CActive::EPriorityIdle );

    FeatureManager::InitializeLibL();

    /** CR - 106 - 1357
      * KFeatureIdChatNG, if not supported, then we should not allow the
      * Application to launch further, so we leave with an error
      * "Feature Not Supported!".
      */
    if ( !FeatureManager :: FeatureSupported ( KFeatureIdChatNG ) )
        {
        User :: Leave ( KErrNotSupported );
        }

    CHAT_DP_TXT( "CCAAppUi::ConstructL() featuremanager initialized" );

    // Brand UI
    CCAVariantFactory* varFact =
        static_cast<CCAApp*>( Application() )->VariantFactory();

    // CR : 101-39724 Error note needed when application cannot be opened because of the lack of the memory
    MCAOpBrandVariant* opBrand = NULL ;
    TRAPD( err, opBrand = varFact->OpBrandVariantL() );
    if ( err )
        {
        // Using  CErrorUI, which matches the errorcode passed with coreesponding error string
        // and displays the note.
        CErrorUI* errorUI = CErrorUI::NewLC( );
        errorUI->ShowGlobalErrorNoteL( err );
        CleanupStack::PopAndDestroy( errorUI );
        User::Leave( err );
        }

    RetrieveBrandL( *opBrand );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() brand retrieved" );

    MCASkinVariant* skin = varFact->SkinVariantL();

    CHAT_DP_TXT( "CCAAppUi::ConstructL() skinvariant constructed" );

    iIconLoader = CCAIconLoader::NewL( iChatMbmFullPath, *skin );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() iconloader constructed" );

    // Initialize Connection UI agent
    iConnUIAgent = CIMPSConnectionUiAgent::NewL( EIMPSConnClientIM );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() connuiagent constructed" );

    iConnUIAgent->StartServiceL(
        EIMPSQueryActiveConnectionScheduledClose );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() connuiagent started 1" );

    iConnUIAgent->StartServiceL(
        EIMPSQueryActiveConnectionSuppressForScheduledConnection );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() connuiagent started 2" );

    iConnUIAgent->StartServiceL(
        EIMPSReqisterAsSignificantConnectionClientInScheduling );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() connuiagent started 3" );

    iConnUIAgent->StartServiceL(
        EIMPSNotifyActiveConnectionLostReasons );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() connuiagent started 4" );

    // SAP settings store
    iSAPSettingsStore = CIMPSSAPSettingsStore::NewL();
    //raj
    iSAPSettingsStore->AddObserverL( this, EIMPSIMAccessGroup );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() sapsettingstore constructed" );

    // Initialize Universal Indicator Pane handling
    InitializeUIPHandlingL();

    CHAT_DP_TXT( "CCAAppUi::ConstructL() uiphandling initialized" );

    // Centralized handler for navi-pane changes
    iStatusPaneHandler = CCAStatusPaneHandler::NewL();

    CHAT_DP_TXT( "CCAAppUi::ConstructL() statuspanehandler constructed" );

    iConnUI = CIMPSPresenceConnectionUi::NewL( EIMPSConnClientIM );

    // Audio manager for playing simple sounds
    iAudioManager = CCAAudioManager::NewL( *iConnUI );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() audiomanager constructed" );

    // Input absorber
    iInputAbsorber = CCAInputAbsorber::NewL();

    CHAT_DP_TXT( "CCAAppUi::ConstructL() inputabsorber constructed" );

    // screen name holder
    iScreenName = HBufC::NewL( KScreenNameMaxLength );

    //Message extension handler. This must be created before views.
    iMessageHandler = CCAMessageExtensionsHandler::NewL( *skin, *this );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() messagehandler constructed" );

    // Create just the main view. Other views are created when the engine
    // has been fully constructed, since they depend on it. We can't
    // create the engine now because it will boot up the IMPS Server which
    // will take a long time.
    //CAViewCreator::CreateMainViewL( *this );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() main view created" );

    // This has to be set before UISessionManager is created.
    // Needed for branding purposes
    // and for refresh view.
    iActiveViewId = KUidFriendsListView;


    // Create ui session manager (it takes care of needed UI sequences
    // concerning UI construction)
    iUISessionManager = CCAUISessionManager::NewL(  *iConnUI, skin,
                                                    opBrand,
                                                    this,
                                                    this,
                                                    iStatusPaneHandler,
                                                    &MbmFullPath() );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() UISessionManager constructed" );

    // Show Splash Screen, if variated so
    //  IM client UI customization, phase 2
    //  Based on Variation flag, Display SplashScreen
    TBool showSplashScreen = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_SPLASH_SCREEN );
    if ( showSplashScreen )
        {
        ShowSplashScreenL( *skin );
        }
    else
        {
        SplashScreenShowed();
        }

    }

// -----------------------------------------------------------------------------
// CCAAppUi::InitializeEngineObserversL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::InitializeEngineObserversL()
    {
    // engine is valid before this is called

    MCAGlobalNotificationObserverPC& globalNotificationObserverPC ( *iProcessManager.GetGlobalNotificationInterface() );


    globalNotificationObserverPC.AddGlobalObserverL( this );

    //iEngine.MessageUtils().MessageErrorInformer().RegisterObserver( this );
    iEngineInitialized = ETrue;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::FinalizeChatConstructionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::FinalizeChatConstructionL()
    {
    if ( iChatFullyConstructed )
        {
        // we're already fully constructed
        return;
        }

    iProcessManager.FinalizeEngineConstructionL();
    InitializeEngineObserversL();
    CAViewCreator::CreateOtherViewsL( *this );

    iProcessManager.GetRecordedChatsArrayPC()->FinalizeRecChatsArrayPCConstructionL();
    // after this call, chat application is now fully constructed
    iChatFullyConstructed = ETrue;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::SetPlayOnce
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::SetPlayOnce( TBool aPlayOnce )
    {
    iPlayOnce = aPlayOnce;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::AfterRefreshViewId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CCAAppUi::AfterRefreshViewId( const TUid& aPrevious )
    {
    if ( iDefaultRefreshInUse )
        {
        return aPrevious;
        }
    else
        {
        iDefaultRefreshInUse = ETrue;
        return KUidFriendsListView;
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::SplashScreenShowed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::SplashScreenShowed()
    {
    // Create just the main view. Other views are created when the engine
    // has been fully constructed, since they depend on it. We can't
    // create the engine now because it will boot up the IMPS Server which
    // will take a long time.
    CAViewCreator::CreateMainViewL( *this );
    CHAT_DP_TXT( "CCAAppUi::ConstructL() main view created" );

    iGroupUtils = CCAGroupUtils::NewLC( *this );
    CleanupStack::Pop( iGroupUtils );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() grouputils constructed" );

    ReadBrandedResourcesL();

    CHAT_DP_TXT( "CCAAppUi::ConstructL() branded resources read" );


    FinalizeChatConstructionL();

    CCACommandManagerFactory* commandManagerFactory =  CCACommandManagerFactory::InstanceL();

    commandManagerFactory->InitializeSessionHandlerL( iProcessManager, *iUISessionManager );

    StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );

    CHAT_DP_TXT( "CCAAppUi::ConstructL() ready" );


    //create infopopup note controller
    iInfoPopup = CAknInfoPopupNoteController::NewL();

    iInfoPopup->AddObserverL( *this );

    iPreviewMessageList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );

    iFocusFlag = EFalse;

    iQueryAccepted = EFalse;
    iForward = TEnumsPC::EUnregistered;

    iIsFSDExitNeeded = EFalse;

    iLaunchSettingsDialogWait = EFalse;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::ShowSplashScreenL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::ShowSplashScreenL( MCASkinVariant& aSkinVariant )
    {
    // Read Splash Screen duration from resource
    //	IM client UI customization, phase 2
    //  Based on Variation flag, SplashScreen  Display duration
    TBool aSplashDelay = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SPLASH_SCREEN_DURATION );
//    TInt aSplashDelay = 4; //in seconds
    /*
        CFbsBitmap* bitmapmain = new (ELeave) CFbsBitmap();
    	CFbsBitmap* bitmapmask = new (ELeave) CFbsBitmap();
    	TSize size(240,320);
    	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    	AknsUtils::CreateAppIconLC(skin, TUid::Uid(EAknsMinorGenericQgnGrafShutdown), EAknsAppIconTypeContext,bitmapmain,bitmapmask);
    	// EAknsMajorScreensaver EAknsMinorGenericQgnPropImSmileysHappy EAknsMinorGenericQgnGrafShutdown EAknsAppIconType3D EAknsAppIconTypeList
    	AknIconUtils::SetSize(bitmapmain,size);
    	AknIconUtils::SetSize(bitmapmask,size);

    	CGulIcon *Icon = CGulIcon::NewL(bitmapmain,bitmapmask);
    	CleanupStack::Pop(2);
    */
    /*
    	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    	CGulIcon* Icon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                            KAknsIIDQgnPropSetButton,
                                            AknIconUtils::AvkonIconFileName(),
                                            EMbmAvkonQgn_prop_set_button,
                                            EMbmAvkonQgn_prop_set_button_mask  );
    */
    /*
    	CGulIcon* Icon = aSkinVariant.LoadBitmapL( EMbmChatngQgn_prop_im_smileys_happy,
                                                   EMbmChatngQgn_prop_im_smileys_happy_mask,
    				                               iChatMbmFullPath );
    */
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    TSize size;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, size );
    AknIconUtils::CreateIconL( bitmap, mask, KFileName, EMbmChatngQgn_graf_im_splash_screen, EMbmChatngQgn_graf_im_splash_screen_mask );
    AknIconUtils::SetSize( bitmap, size, EAspectRatioPreserved );
    AknIconUtils::SetSize( mask, size, EAspectRatioPreserved );

    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );
    CGulIcon* Icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 ); // bitmap and mask

    iSplashScreen->ShowSplashScreenL( this, aSplashDelay, Icon, EFalse );
    return;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::SetAfterRefreshMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::SetAfterRefreshMode( TBool aMode )
    {
    iDefaultRefreshInUse = aMode;
    }

// ---------------------------------------------------------
// CCAAppUi::SolveResPathLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCAAppUi::SolveResPathLC( const TDesC& aPathWithoutDrive ) const
    {
    CHAT_DP_TXT( "Before add resource file" );

    TFileName path = Application()->AppFullName();
    TDriveName drive( TParsePtrC( path ).Drive( ) ); // solve drive

    // leave to stack, LC method
    HBufC* resourceFilePath = HBufC::NewLC( KMaxResourcePathLength );
    TPtr ptr( resourceFilePath->Des() );
    ptr.Append( aPathWithoutDrive ); // without drive
    ptr.Insert( 0, drive ); // insert drive

    // NearestLanguageFile takes only TFileName
    path.Zero();
    path.Append( *resourceFilePath );

    if ( path.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), path );
        }

    *resourceFilePath = path;

    return resourceFilePath;
    }

// ---------------------------------------------------------
// CCAAppUi::TabChangedL( TInt aIndex )
// From MAknTabObserver, handles changes in the tabs groups.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::TabChangedL( TInt /*aIndex*/ )
    {
    /*
    CHAT_DP( D_CHAT_LIT("Tab changed to index %d"), aIndex );
    CAknTabGroup* tabGroup = CAStatusPane()->TabGroup();
    if ( tabGroup )
        {
        tabGroup->SetActiveTabByIndex( aIndex );
        }
    */
    }

// ---------------------------------------------------------
// CCAAppUi::HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType)
// From CEikAppUi, handles key events.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCAAppUi::HandleKeyEventL( const TKeyEvent& /* aKeyEvent */,
                                        TEventCode /* aType */ )
    {
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CCAAppUi::HandleForegroundEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::HandleForegroundEventL( TBool aForeground )
    {
    CHAT_DP_FUNC_ENTER( "HandleForegroundEventL" );

    iIsAppForeground = aForeground;

    if ( aForeground && iOkToSwitch )
        {
        // coming from background to foreground,
        // so show the view
        CHAT_DP_FUNC_DP( "HandleForegroundEventL",
                         "Going from background to foreground" );

        if ( iCustomMessageId == KNullUid )
            {
            // no custom message
            CHAT_DP_FUNC_DP( "HandleForegroundEventL", "No custom message" );
            SwitchViewL( iActiveViewId );
            }
        else
            {
            // custom message
            CHAT_DP_FUNC_DP( "HandleForegroundEventL", "Custom message" );
            SwitchViewL( iActiveViewId, iCustomMessageId, *iCustomMessage );
            }
        }
    else
        {
        CHAT_DP_FUNC_DP( "HandleForegroundEventL", "Custom message" );
        }

    if ( !aForeground )
        {
        // application switched to background, update softnotifier
        if ( iEngine.IsFinalized() )
            {
            // Universal indicator is not shown when one new message is received
            UpdateIMUIPIcon();
            }

        // Set to false -> beep is played next time when
        // new msg is received
        iPlayOnce = EFalse;
        }
    else
        {
        // Disable IM's system attribute. After this call, IM is again under normal
        // memory management. It might get killed if more memory is needed by the fw.
        iEikonEnv->SetSystem( EFalse );
        }

    CAknViewAppUi::HandleForegroundEventL( aForeground );

    CHAT_DP_FUNC_DONE( "HandleForegroundEventL" );
    }


// ---------------------------------------------------------
// CCAAppUi::HandleChatEvent
// (other items were commented in a header).
// ---------------------------------------------------------
void CCAAppUi::HandleChatEvent( const TEnumsPC::TChatEventType aEvent,
                                const TDesC& aWvid,
                                const TDesC& aContactId,
                                const TDesC& aMessage,
                                const TEnumsPC::TMessagerType aMsgrType,
                                const TEnumsPC::TMessageType aMsgType )
    {
    CHAT_DP_FUNC_ENTER( "CCAAppUi::HandleChatEvent" );
    switch ( aEvent )
        {
        case TEnumsPC::EUnreadCountChanged:
            {

            //if message was not sent then wvid will be KNullDesC
            //then dont check for aMsgrType, aMsgType
            //also dont show the preview pane
            if ( aWvid.Length() )
                {
                // own message
                if ( aMsgrType == TEnumsPC::EMessageSent
                     // not PtoP message
                     || aMsgType == TEnumsPC::EMessageInvalid )
                    {
                    return;
                    }
                //play the message tone
                PlayBeep();

                TBool showPreview( EFalse );

                if ( aMsgType == TEnumsPC::EMessagePTOP )
                    {
                    //conversation message
                    MCAConversationPC& convPC( *iProcessManager.GetConversationInterface() );
                    showPreview = !convPC.IsActiveConversation( aWvid ) ;
                    }
                else
                    {
                    //rest are group chat related messages
                    MCAGroupPC& groupPC ( *iProcessManager.GetGroupInterface() );
                    showPreview = !groupPC.IsActiveGroupChat( aWvid ) ;

                    }

                //check if the new message is for the current active conversation only
                //if the current active view is conversation -
                if ( showPreview )
                    {
                    //show preview pane only if the message is not for the
                    //current active conversation

                    TRAP_IGNORE(
                        {
                        AddToPreviewListL( aContactId, aMessage );
                        if ( !iInfoNoteShowing )
                            {
                            ShowInfoPopupL();
                            }

                        }

                    );
                    }
                }
            //update the universal indicator
            //shows the universal indicator
            UpdateIMUIPIcon();
            break;
            }

        case TEnumsPC::EMemoryLow:
            {
            if ( iShowMemLowNote )
                {
                TRAPD( err, ShowMemLowNoteL() );
                if ( err )
                    {
                    CActiveScheduler::Current()->Error( err );
                    }
                }
            break;
            }
        default:
            {
            // No need to handle other events here
            break;
            }
        }
    CHAT_DP_FUNC_DONE( "CCAAppUi::HandleChatEvent" );
    }

// -----------------------------------------------------------------------------
// CCAAppUi::ChangeStatusProcessL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::ChangeStatusProcessL()
    {
    if ( iStatusFlag )
    	{
    	return;
    	}
    
	iStatusFlag = ETrue;
    if ( iUISessionManager->IsLoggedIn() )
        {
        // Check variation
        TInt showAppearOffline =
            IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_APPEAR_OFFLINE );
        TInt showAway =
            IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_AWAY_STATUS );
        TInt showBusy =
            IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BUSY_STATUS );

        TInt awayIndex( 0 );
        TInt busyIndex( 0 );
        TInt appOfflineIndex( 0 );

        // first icon is online
        // calculate indexes for other icons
        if ( showAway )
            {
            awayIndex++;
            busyIndex++;
            appOfflineIndex++;
            }
        else
            {
            awayIndex = -1;
            }

        if ( showBusy )
            {
            busyIndex++;
            appOfflineIndex++;
            }
        else
            {
            busyIndex = -1;
            }

        if ( showAppearOffline )
            {
            appOfflineIndex++;
            }
        else
            {
            appOfflineIndex = -1;
            }

        TInt status( 0 );
        CAknListQueryDialog* dlg = new ( ELeave )
        CAknListQueryDialog( &status );
        dlg->PrepareLC( R_CHAT_STATUS_CHANGE_DLG );

        // Format the texts
        // Create a array for texts
        CDesCArray* listItemsArray =
            new ( ELeave ) CDesCArrayFlat( KAmountOfOwnStatusStates );
        CleanupStack::PushL( listItemsArray );

        // Online
        HBufC* onlineText = NULL;
        onlineText = StringLoader::LoadLC( R_CHAT_CHANGE_OWN_ONLINE );

        HBufC* itemTxtOnline =
            HBufC::NewLC( KTabFormatLenght + onlineText->Length() );
        TPtr itemTxtPtrOnline( itemTxtOnline->Des() );

        itemTxtPtrOnline.Format( KTabFormat, KOnlineIconIndex );
        itemTxtPtrOnline.Append( *onlineText );
        listItemsArray->AppendL( itemTxtPtrOnline );
        CleanupStack::PopAndDestroy( itemTxtOnline );
        CleanupStack::PopAndDestroy( onlineText );

        // Away
        if ( showAway )
            {
            HBufC* awayText = NULL;
            awayText = StringLoader::LoadLC( R_CHAT_CHANGE_OWN_AWAY );

            HBufC* itemTxtAway = HBufC::NewLC(
                                     KTabFormatLenght + awayText->Length() );
            TPtr itemTxtPtrAway( itemTxtAway->Des() );

            itemTxtPtrAway.Format( KTabFormat, awayIndex );
            itemTxtPtrAway.Append( *awayText );
            listItemsArray->AppendL( itemTxtPtrAway );
            CleanupStack::PopAndDestroy( itemTxtAway );
            CleanupStack::PopAndDestroy( awayText );
            }

        // Busy
        if ( showBusy )
            {
            HBufC* busyText = NULL;
            busyText = StringLoader::LoadLC( R_CHAT_CHANGE_OWN_BUSY );

            HBufC* itemTextBusy = HBufC::NewLC(
                                      KTabFormatLenght + busyText->Length() );
            TPtr itemTxtPtrBusy( itemTextBusy->Des() );

            itemTxtPtrBusy.Format( KTabFormat, busyIndex );
            itemTxtPtrBusy.Append( *busyText );
            listItemsArray->AppendL( itemTxtPtrBusy );
            CleanupStack::PopAndDestroy( itemTextBusy );
            CleanupStack::PopAndDestroy( busyText );
            }
        // Appear offline
        if ( showAppearOffline )
            {
            HBufC* offlineText = NULL;
            offlineText = StringLoader::LoadLC( R_CHAT_CHANGE_OWN_APPEAR_OFFLINE );

            HBufC* itemTextOffline = HBufC::NewLC(
                                         KTabFormatLenght + offlineText->Length() );
            TPtr itemTxtPtrOffline( itemTextOffline->Des() );

            itemTxtPtrOffline.Format( KTabFormat, appOfflineIndex );
            itemTxtPtrOffline.Append( *offlineText );
            listItemsArray->AppendL( itemTxtPtrOffline );
            CleanupStack::PopAndDestroy( itemTextOffline );
            CleanupStack::PopAndDestroy( offlineText );
            }

        // Set the array
        dlg->SetOwnershipType( ELbmOwnsItemArray );
        CleanupStack::Pop( listItemsArray );
        dlg->SetItemTextArray( listItemsArray );

        // Add icons
        CArrayPtr< CGulIcon >* icons = new( ELeave )
        CAknIconArray( KAmountOfOwnStatusStates );
        CleanupStack::PushL( icons );

        MCASkinVariant* skinVar = static_cast<CCAApp*>(
                                      Application() )->VariantFactory()->SkinVariantL();

        // Online
        CGulIcon* iconOnline = skinVar->LoadBitmapL(
                                   EMbmChatngQgn_prop_im_friend_on,
                                   EMbmChatngQgn_prop_im_friend_on_mask,
                                   MbmFullPath() );
        CleanupStack::PushL( iconOnline );
        icons->AppendL( iconOnline );
        CleanupStack::Pop( iconOnline );

        // Away
        CGulIcon* iconAway = NULL;
        if ( showAway )
            {
            iconAway = skinVar->LoadBitmapL(
                           EMbmChatngQgn_prop_im_friend_away,
                           EMbmChatngQgn_prop_im_friend_away_mask,
                           MbmFullPath() );
            CleanupStack::PushL( iconAway );
            icons->AppendL( iconAway );
            CleanupStack::Pop( iconAway );
            }

        // Busy
        CGulIcon* iconBusy = NULL;
        if ( showBusy )
            {
            iconBusy = skinVar->LoadBitmapL(
                           EMbmChatngQgn_prop_im_friend_busy,
                           EMbmChatngQgn_prop_im_friend_busy_mask,
                           MbmFullPath() );
            CleanupStack::PushL( iconBusy );
            icons->AppendL( iconBusy );
            CleanupStack::Pop( iconBusy );
            }
        // Appear offline
        CGulIcon* iconOffline = NULL;
        if ( showAppearOffline )
            {
            iconOffline = skinVar->LoadBitmapL(
                              EMbmChatngQgn_prop_im_friend_invisible,
                              EMbmChatngQgn_prop_im_friend_invisible_mask,
                              MbmFullPath() );
            CleanupStack::PushL( iconOffline );
            icons->AppendL( iconOffline );
            CleanupStack::Pop( iconOffline );
            }

        dlg->SetIconArrayL( icons ); // transfers ownership
        CleanupStack::Pop( icons );

        // get own status

        TInt presenceStatus( TEnumsPC::EUnknown );
        MCAMainViewArrayPC* mainViewArrayPC = iProcessManager.GetArrayInterface();
        TInt topIndex( 0 );
        if ( mainViewArrayPC->GetType( topIndex ) == TEnumsPC::EOwnStatusItem )
            {
            presenceStatus =  mainViewArrayPC->GetOnlineStatus( topIndex ) ;
            }
        // get dialog's listbox
        CEikListBox* listBox = dlg->ListBox();

        // move focus in the listbox to current status
        switch ( presenceStatus )
            {
            case TEnumsPC::EOnline:
                {
                listBox->SetCurrentItemIndex( 0 );
                break;
                }
            case TEnumsPC::EAway:
                {

                if ( showAway || showBusy || showAppearOffline )
                    {
                    listBox->SetCurrentItemIndex( 1 );
                    }
                break;
                }
            case TEnumsPC::EBusy:
                {
                if ( showBusy )
                    {
                    listBox->SetCurrentItemIndex( busyIndex );
                    }
                else
                    {
                    if ( showAway )
                        {
                        // Show as away if busy is disabled
                        listBox->SetCurrentItemIndex( awayIndex );
                        }
                    else
                        {
                        listBox->SetCurrentItemIndex( 0 );
                        }

                    }
                break;
                }
            case TEnumsPC::EInvisible:
                {
                if ( showAppearOffline )
                    {
                    // index of invisible is 3 if busy is visible, else 2.
                    if ( showAway )
                        {
                        listBox->SetCurrentItemIndex( showBusy ? 3 : 2 );
                        }
                    else
                        {
                        // away is not shown, index is 2 or 1
                        listBox->SetCurrentItemIndex( showBusy ? 2 : 1 );
                        }
                    }
                else
                    {
                    if ( showAway )
                        {
                        // invisible, but appear offline disabled,
                        // so show as "away"
                        listBox->SetCurrentItemIndex( 1 );
                        }
                    else
                        {
                        // invisible, but appear offline and away disabled,
                        // so show as online
                        listBox->SetCurrentItemIndex( 0 );
                        }
                    }
                break;
                }
            default:
                {
                // nothing to do
                break;
                }
            }

        TInt result = dlg->RunLD();

        TInt error( KErrNone );
        if ( ( result == EAknSoftkeyOk ) ||
             ( result == EAknSoftkeySelect ) )
            {
            if ( status == 0 )
                {
                TInt autoStatusMsgEnabledForOnline = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_ONLINE_AUTO );
                if ( autoStatusMsgEnabledForOnline )
                    {
                    error = AskStatusMessageL( TEnumsPC::EOnline, EFalse );
                    }
                if ( error == KErrNone )
                    {
                    CCAWaitNote* note = CCAWaitNote::ShowWaitNoteLC( R_QTN_CHAT_CHANGE_STATUS_CHANGING );
                    error = mainViewArrayPC->ChangeStatusL(
                                TEnumsPC::EOnline, *iStatusMessage );
                    CleanupStack::PopAndDestroy( note );
                    }
                }
            if ( status == awayIndex ) /* Away */
                {
                // can't get here unless Away is enabled
                TInt autoStatusMsgEnabledForAway =
                    IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_AWAY_AUTO );
                if ( autoStatusMsgEnabledForAway )
                    {
                    error = AskStatusMessageL( TEnumsPC::EAway, EFalse );
                    }
                if ( error == KErrNone )
                    {
                    CCAWaitNote* note = CCAWaitNote::ShowWaitNoteLC( R_QTN_CHAT_CHANGE_STATUS_CHANGING );
                    error = mainViewArrayPC->ChangeStatusL(
                                TEnumsPC::EAway, *iStatusMessage );
                    CleanupStack::PopAndDestroy( note );
                    }
                }

            if ( status == busyIndex ) // Busy
                {
                // can't get here unless Busy is enabled
                TInt autoStatusMsgEnabledForBusy =
                    IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BUSY_AUTO  );
                if ( autoStatusMsgEnabledForBusy )
                    {
                    error = AskStatusMessageL( TEnumsPC::EBusy, EFalse );
                    }
                if ( error == KErrNone )
                    {
                    CCAWaitNote* note = CCAWaitNote::ShowWaitNoteLC( R_QTN_CHAT_CHANGE_STATUS_CHANGING );
                    error = mainViewArrayPC->ChangeStatusL(
                                TEnumsPC::EBusy, *iStatusMessage );
                    CleanupStack::PopAndDestroy( note );
                    }
                }
            if ( status == appOfflineIndex ) //Offline
                {
                // can't get here unless Appear Offline is enabled
                CCAWaitNote* note = CCAWaitNote::ShowWaitNoteLC( R_QTN_CHAT_CHANGE_STATUS_CHANGING );
                error = mainViewArrayPC->ChangeStatusL(
                            TEnumsPC::EOffline, KNullDesC );
                CleanupStack::PopAndDestroy( note );
                }


            if ( error == KErrNone )
                {
                // show confirmation note
                HBufC* text = iCoeEnv->AllocReadResourceLC( R_QTN_CHAT_CHANGE_STATUS_CHANGED );
                CAknConfirmationNote* dlg = new( ELeave )CAknConfirmationNote();
                dlg->ExecuteLD( *text );
                CleanupStack::PopAndDestroy( text );
                }
            else if ( error == KErrCancel )
                {
                // user cancelled, status was not updated
                IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_CHANGE_STATUS_NOT_CHANGED );
                }
            else
                {
                // change status failed
                HBufC* failed = StringLoader::LoadLC( R_QTN_CHAT_CHANGE_STATUS_FAILED );
                IMDialogUtils::DisplayErrorNoteL( *failed );
                CleanupStack::PopAndDestroy( failed );
                }
            }
        else
            {
            IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_CHANGE_STATUS_NOT_CHANGED );
            }
        iStatusFlag = EFalse;

        iUISessionManager->SetStatusPaneIconsL();
        }
    }

// ---------------------------------------------------------
// CCAAppUi::AskStatusMessageL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAAppUi::AskStatusMessageL( TEnumsPC::TOnlineStatus aStatus,
                                  TBool aUpdateToNetwork )
    {
    CDesCArray* statusMessages = new( ELeave )CDesCArrayFlat( KMaxStatusMessageCount );
    CleanupStack::PushL( statusMessages );

    HBufC* firstLine = StringLoader::LoadLC( R_QTN_CHAT_CHANGE_NEW_MESSAGE );
    statusMessages->AppendL( *firstLine );
    CleanupStack::PopAndDestroy( firstLine );

    // Read status messages
    ReadStatusMessagesL( aStatus, *statusMessages );

    // show the query
    TInt selectionIndex( statusMessages->Count() > 1 ? 1 : 0 );

    // Force to continue if selection query is not shown
    TInt ret = EAknSoftkeyOk;

    if ( selectionIndex == 1 )
        {
        // There were previous status messages, show selection query
        HBufC* titleText = StringLoader::LoadLC( R_QTN_CHAT_CHANGE_SELECT_MESSAGE );
        ret = IMDialogUtils::DisplayListQueryDialogL( &selectionIndex, statusMessages,
                                                      R_PRIVATE_CHAT_LIST_SELECT_RECIP_DLG,
                                                      *titleText, KNullDesC, ETrue );
        CleanupStack::PopAndDestroy( titleText );
        }

    if ( !( ret == EAknSoftkeyOk || ret == EAknSoftkeyDone || ret == EAknSoftkeySelect ) )
        {
        CleanupStack::PopAndDestroy( statusMessages );
        return KErrCancel;
        }

    TPtr statusTxt( iStatusMessage->Des() );
    statusTxt.Zero();

    if ( selectionIndex != 0 )
        {
        statusTxt.Copy( ( *statusMessages ) [ selectionIndex ] );
        }

    TInt result = IMDialogUtils::DisplayTextQueryDialogL(
                      statusTxt ,
                      R_CHAT_ENTER_STATUS_MESSAGE,
                      R_CHATCLIENT_ENTER_STATUS_MESSAGE_QUERY,
                      ETrue, // T9
                      ETrue // LSK always visible
                  );

    if ( ( result == EAknSoftkeyOk || result == EAknSoftkeyDone ) && ( iUISessionManager->IsLoggedIn() ) )
        {
        CHAT_DP( D_CHAT_LIT( "iStatusMessage = %S" ), iStatusMessage );

        // Delete first line.
        statusMessages->Delete( 0 );
        statusMessages->InsertL( 0, statusTxt );
        WriteStatusMessagesL( aStatus, *statusMessages );
        if ( aUpdateToNetwork )
            {
            CCAWaitNote* note = CCAWaitNote::ShowWaitNoteLC( R_CHAT_CHANGE_MESSAGE_CHANGING );

            MCAMainViewArrayPC* mainViewArrayPC = iProcessManager.GetArrayInterface();
            TInt error( mainViewArrayPC->ChangeStatusMessageL( statusTxt ) );
            CleanupStack::PopAndDestroy( note );
            if ( error == KErrNone )
                {
                // show note qtn_chat_change_message_changing
                IMDialogUtils::DisplayInformationNoteL( R_CHAT_CHANGE_MESSAGE_CHANGED );
                }
            else if ( error == KErrCancel )
                {
                IMDialogUtils::DisplayInformationNoteL( R_CHAT_CHANGE_MESSAGE_NOT_CHANGED );
                }
            else
                {
                IMDialogUtils::DisplayInformationNoteL( R_CHAT_CHANGE_MESSAGE_FAILED );
                }
            }
        }
    else
        {
        CleanupStack::PopAndDestroy( statusMessages );
        return KErrCancel;
        }
    CleanupStack::PopAndDestroy( statusMessages ); //statusMessages
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::ReadStatusMessagesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::ReadStatusMessagesL( TEnumsPC::TOnlineStatus aStatus,
                                    CDesCArray& aArray )
    {
    MCASettingsPC* settingsPC = iProcessManager.GetSettingsInterface();
    if ( !settingsPC )
        {
        User::Leave( KErrNotReady );
        }

    RBuf buffer;

    buffer.Assign( settingsPC->GetSapSettingValuePCL(
                       ConvertToTEnumsStatusMsgValue( aStatus ) ) );

    TBuf< KStatusMessageHeaderLength > header;
    CleanupClosePushL( buffer );

    TInt err( KErrNone );
    TInt countOfMessages( 0 );
    TInt offset( 1 ); // First character is for header length
    TInt headerLength( 0 );
    if ( buffer.Length() )
        {
        TLex lexer( buffer.Left( 1 ) );
        err = lexer.Val( headerLength );
        }
    else
        {
        err = KErrEof;
        }
    while ( err == KErrNone && countOfMessages < KMaxStatusMessageCount )
        {
        TPtrC ptr( buffer.Mid( offset ) );
        if ( ptr.Length() > headerLength )
            {
            header.Copy( ptr.Left( headerLength ) );
            header.Trim();
            TLex lexer( header );
            offset += headerLength;
            TInt messageLength( 0 );
            err = lexer.Val( messageLength );
            if ( err == KErrNone )
                {
                ptr.Set( buffer.Mid( offset ) );
                if ( ptr.Length() >= messageLength )
                    {
                    aArray.AppendL( ptr.Left( messageLength ) );
                    offset += messageLength;
                    ++countOfMessages;
                    }
                else
                    {
                    err = KErrEof;
                    }
                }
            }
        else
            {
            err = KErrEof;
            }
        }
    CleanupStack::PopAndDestroy(); // buffer
    }

// -----------------------------------------------------------------------------
// CCAAppUi::WriteStatusMessagesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CCAAppUi::WriteStatusMessagesL( TEnumsPC::TOnlineStatus aStatus,
                                     MDesCArray& aArray )
    {
    MCASettingsPC* settingsPC = iProcessManager.GetSettingsInterface();
    if ( !settingsPC )
        {
        User::Leave( KErrNotReady );
        }

    TPtrC firstLine = aArray.MdcaPoint( 0 );
    TInt countOfMessages( 0 );
    TInt arrayCount( aArray.MdcaCount() );
    TInt index( 0 );

    RBuf buffer;
    buffer.CreateL( RProperty::KMaxPropertySize );
    CleanupClosePushL( buffer );

    TBuf< KStatusMessageHeaderLength > header;

    while ( index < arrayCount && countOfMessages < KMaxStatusMessageCount )
        {
        if ( index == 0 )
            {
            header.Format( KStatusMessageHeaderLengthFormat,
                           KStatusMessageHeaderLength );
            buffer.Append( header );
            }
        TPtrC message = aArray.MdcaPoint( index );
        if ( index == 0 || message.CompareF( firstLine ) != 0 )
            {
            header.Format( KStatusMessageHeaderFormat, message.Length() );
            buffer.Append( header );
            buffer.Append( message );
            ++countOfMessages;
            }
        ++index;
        }

    settingsPC->SetSapSettingValuePCL( ConvertToTEnumsStatusMsgValue( aStatus ) , buffer );
    CleanupStack::PopAndDestroy(); // buffer
    }

// -----------------------------------------------------------------------------
// CCAAppUi::ShowMemLowNoteL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::ShowMemLowNoteL()
    {
    // Don't launch note if one is already displayed
    if ( !iMemLowNote )
        {
        // Give pointer to iMemLowNote to dialog ->
        // gets nullified when dialog is destroyed
        iMemLowNote = new( ELeave ) CAknNoteDialog(
            &iMemLowNote,
            CAknNoteDialog::EConfirmationTone );
        iMemLowNote->PrepareLC( R_CHAT_MEM_LOW_NOTE );
        // Pointer to dialog is needed as member
        // variable to be able to know when dialog
        // is destroyed
        iMemLowNote->RunLD();   // CSI: 50 #see above
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::HandleContactDelete
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::HandleDelete( const TDesC& /*aContactId*/ , TEnumsPC::TItem /*aType*/ )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CCAAppUi::HandleAddition
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::HandleAddition( TEnumsPC::TItem /*aType*/ )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CCAAppUi::HandleChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::HandleChange( TEnumsPC::TItem /*aType*/,
                             TEnumsPC::TChange aChangeType )
    {
    if ( aChangeType == TEnumsPC::EChanged ||
         aChangeType == TEnumsPC::EMultipleChanges )
        {
        TRAPD( err, iUISessionManager->SetStatusPaneIconsL() );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }
    }
void CCAAppUi::HandleSettingsChangeL( TInt /*aChangedSettingEnum */ )
    {
    //Right now there is no implementation needed here
    }

// -----------------------------------------------------------------------------
// CCAAppUi::HandleWatcherEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*void CCAAppUi::HandleWatcherEvent(const TDesC& aString ,
						TEnumsPC::TOnlineStatus aOnlineStatus)
	{
	}*/

// -----------------------------------------------------------------------------
// CCAAppUi::HandleMessageError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::HandleMessageError( TInt aError, const TDesC& /*aInfo*/ )
    {
    if ( aError == KErrNoMemory )
        {
        TRAPD( err, IMNoteMapper::ShowNoteL( aError ) );
        if ( err )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }
    }


// -----------------------------------------------------------------------------
// CCAAppUi::HandleContactChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*void CCAAppUi::HandleContactChange( const TDesC& aContactId )
	{
	}*/


// -----------------------------------------------------------------------------
// CCAAppUi::ExitCallback
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAAppUi::ExitCallback( TAny* aInstance )
    {
    CHAT_DP_FUNC_ENTER( "ExitCallback" );
    return static_cast<CCAAppUi*>( aInstance )->DoExitCallback();
    }

// -----------------------------------------------------------------------------
// CCAAppUi::DoExitCallback
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAAppUi::DoExitCallback()
    {
    CHAT_DP_FUNC_ENTER( "DoExitCallback" );
    if (  iProcessManager.GetLogInInterface()->ReadyForShutdown() && iPendingDialogCounter <= 0 )
        {
        TInt level = LevelOfActiveScheduler();
        if( level > 1 )
        	{
        	CActiveScheduler::Current()->Stop();
        	}
        CHAT_DP_FUNC_DP( "DoExitCallback", "Engine is ready for shutdown" );
        Exit();
        }
    CHAT_DP_FUNC_DP( "DoExitCallback", "Engine is NOT ready for shutdown" );
    return ETrue; //just a true value
    }

// -----------------------------------------------------------------------------
// CCAAppUi::CloseDialog
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::CloseDialog( TAny* aInstance )
    {
    TRAPD( leave, static_cast<CCAAppUi*>( aInstance )->DismissWaitDialogL( KErrNone ) );
    if ( leave != KErrNone )
        {
        CActiveScheduler::Current()->Error( leave );
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::RetrieveBrandL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::RetrieveBrandL( MCAOpBrandVariant& aBrand )
    {
    iChatMbmFullPath = Application()->BitmapStoreName();

    TFileName appRes( Application()->ResourceFileName() );
    _LIT( KResourceEngineFilePathWithoutDrive, "CaEngineNG.rsc" );
    _LIT( KResourcePresenceFilePathWithoutDrive, "WVUIPresenceVariationNG.rsc" );
    _LIT( KResourceVariationFilePathWithoutDrive, "CAVariationNG.rsc" );

    TFileName engResPathWithoutDrive;
    engResPathWithoutDrive.Zero();
    engResPathWithoutDrive.Append( KDC_RESOURCE_FILES_DIR() );
    engResPathWithoutDrive.Append( KResourceEngineFilePathWithoutDrive() );
    HBufC* engResPath = SolveResPathLC( engResPathWithoutDrive );

    TFileName presResPathWithoutDrive;
    presResPathWithoutDrive.Zero();
    presResPathWithoutDrive.Append( KDC_RESOURCE_FILES_DIR() );
    presResPathWithoutDrive.Append( KResourcePresenceFilePathWithoutDrive() );
    HBufC* presResPath = SolveResPathLC( presResPathWithoutDrive );

    TFileName variationResPathWithoutDrive;
    variationResPathWithoutDrive.Zero();
    variationResPathWithoutDrive.Append( KDC_RESOURCE_FILES_DIR() );
    variationResPathWithoutDrive.Append( KResourceVariationFilePathWithoutDrive() );
    HBufC* variationResPath = SolveResPathLC( variationResPathWithoutDrive );



    TPtrC cuiResPath( KNullDesC );
    aBrand.RetrieveSavedSettingsL( iCoeEnv,
                                   appRes,
                                   *engResPath,
                                   *presResPath,
                                   *variationResPath,
                                   cuiResPath,
                                   MbmFullPath() );

    CleanupStack::PopAndDestroy( 3, engResPath ); // variationResPath, presResPath, engResPath
    }

// -----------------------------------------------------------------------------
// CCAAppUi::InitializeUIPHandlingL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::InitializeUIPHandlingL()
    {
    // Connect to server
    iNotifierAPI = CANotifierFactory::CreateNotifierL( NULL, KSDUidSysApLocal );
    }

// -----------------------------------------------------------------------------
// CCAAppUi::UpdateIMUIPIcon
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::UpdateIMUIPIcon()
    {
    TInt dummy( 0 );
    TInt count( iApplicationNGPC.MessagesPendingCount( dummy,
                                                       TEnumsPC::EUnreadReceived ) +
                iProcessManager.GetInvitationsInterface()->UnreadInvitesCount()
                + iApplicationNGPC.ChatGroupMessagesPendingCount( dummy ) );
    TInt error( iNotifierAPI->SetInt( MCANotifierInterface::EUIPIndKey, count ) );

    if ( error )
        {
        CActiveScheduler::Current()->Error( error );
        }

    // Update unread message count to PS
    // Value is used by the ImIndicatorPlugin
    RProperty::Define(
        KPSUidIMUI, KIMUnreadMsgKey, RProperty::EInt,
        KIMUnreadMsgReadPolicy, KIMUnreadMsgWritePolicy  );
    RProperty::Set( KPSUidIMUI, KIMUnreadMsgKey, count );


    //if count=iUnreadMsgCount then there is no need of soft notification
    if ( !iIsAppForeground && count != iUnreadMsgCount )
        {
        // we show the soft notifaction only if chat is in backround
        UpdateSoftNote( count );
        iUnreadMsgCount = count;
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::UpdateSoftNote
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::UpdateSoftNote( TInt aUnreadCount )
    {
    CHAT_DP_TXT( "CCAAppUi::UpdateSoftNote, signal SoftNotifier about new chat message!" );

    TRAPD( err,
           CAknSoftNotifier* softNotifier = CAknSoftNotifier::NewLC();
           softNotifier->SetNotificationCountL( EChatMessageNotification, aUnreadCount );
           CleanupStack::PopAndDestroy( softNotifier );
         );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        CHAT_DP( D_CHAT_LIT( "CCAAppUi::UpdateSoftNote - softnotifier left with: %d" ), err );
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::LayoutInfo
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCALayoutInfo* CCAAppUi::LayoutInfo()
    {
    return iLayoutInfo;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::SetRefreshed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::SetRefreshed()
    {
    iRefreshed = ETrue;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::ListRefreshed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAAppUi::ListRefreshed()
    {
    return iRefreshed;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::ChangeConnUiAgentResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::ChangeConnUiAgentResourceL( const TDesC& aNewResource )
    {
    if ( iConnUIAgent )
        {
        iConnUIAgent->UseResourceFileL( aNewResource );
        }
    }


// -----------------------------------------------------------------------------
// CCAAppUi::RefreshWaitFlag
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt& CCAAppUi::RefreshWaitFlag()
    {
    return iRefreshWaitFlag;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::IsUnderDestruction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAAppUi::IsUnderDestruction()
    {
    return iDestructingFlag;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::DefaultServerId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CCAAppUi::DefaultSAPUid()
    {
    TUint32 SAPUid = 0;

    //if there are no SAPs then don't try the get default server Id
    if ( iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) > 0 )
        {
        CIMPSSAPSettings* sapSettings = CIMPSSAPSettings::NewLC();
        TRAPD( err, iSAPSettingsStore->GetDefaultL( sapSettings, EIMPSIMAccessGroup ) );

        if ( err == KErrNone )
            {
            SAPUid = sapSettings->Uid();
            }
        CleanupStack::PopAndDestroy( sapSettings );
        }
    return SAPUid;
    }


// -----------------------------------------------------------------------------
// CCAAppUi::LaunchServerSettingsDialogL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::LaunchServerSettingsDialogL()
    {
    TFileName resPath;
    TFileName resIMCUVarPath;

    // resIMCUVarPath, is added to load the new resource file that contains the settings for IM UI.


    // if there are no SAPs then don't try the get default
    if ( iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) > 0 )
        {
        CIMPSSAPSettings* sap = static_cast<CCAAppUi*>( iEikonEnv->AppUi() )
                                ->UISessionManager().CurrentSAPLC();
        TPtrC sapName( sap->SAPName() );
        TPtrC wvSettRes( KWVSettingsResFileName );
        TPtrC wvIMCUResVar( KIMPSCUVariationResFileName );

        CCAVariantFactory* varFact =
            static_cast<CCAApp*>( Application() )->VariantFactory();

        // CR : 101-39724 Error note needed when application cannot be opened because of the lack of the memory
        MCAOpBrandVariant* opBrand = NULL;
        TRAPD( err, opBrand = varFact->OpBrandVariantL() );
        if ( err )
            {
            // Using  CErrorUI, which matches the errorcode passed with coreesponding error string
            // and displays the note.
            CErrorUI* errorUI = CErrorUI::NewLC( );
            errorUI->ShowGlobalErrorNoteL( err );
            CleanupStack::PopAndDestroy( errorUI );
            User::Leave( err );
            }

        opBrand->CurrentResourceFileL( sapName, wvSettRes, resPath );
        opBrand->CurrentResourceFileL( sapName, wvIMCUResVar, resIMCUVarPath );


        if ( resPath.Length() != 0 )
            {
            BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resPath );
            }

        if ( resIMCUVarPath.Length() != 0 )
            {
            BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resIMCUVarPath );
            }

        CleanupStack::PopAndDestroy( sap );
        }

    TInt exitReason( 0 );
    iServSettingsDialog = CWVSettingsUIDialog::NewL();

    iUISessionManager->SetStatusPaneIconsL();

    TBool loggedIn = iUISessionManager->IsLoggedIn();

    TUint32 SAPUid1;
    TUint32 SAPUid2;
    SAPUid1 = DefaultSAPUid();

    // CodeScanner warning ignored because
    // iServSettingsDialog is not owned by us
    TInt retVal( iServSettingsDialog->RunDialogLD( *iSAPSettingsStore, // CSI: 50 # See comment above
                                                   this,
                                                   resPath,
                                                   resIMCUVarPath,
                                                   exitReason,
                                                   this,
                                                   &iServSettingsDialog ) );

    SAPUid2 = DefaultSAPUid();

    if ( SAPUid1 != SAPUid2 )
        {
        iNeedRefresh = ETrue;
        }

    iServSettingsDialog = NULL;
    iUISessionManager->SetStatusPaneIconsL();
    // this is for updating settings view
    if ( iNeedRefresh || ( loggedIn != iUISessionManager->IsLoggedIn() ) )
        {
        SwitchViewL( KUidRefreshView );
        }

    if ( retVal == EWVSettingsViewExitCalled )
        {
        HandleCommandL( EChatClientCmdExit );
        }
    }

#ifdef RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
// ---------------------------------------------------------
// CCAAppUi::IsFullyConstructed
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAppUi::IsFullyConstructed() const
    {
    return iChatFullyConstructed;
    }
#endif  // RD_CHAT_GROUP_MESSAGE_INDICATION_NEW

#ifdef RD_SETTINGS_FACELIFT
// ---------------------------------------------------------
// CCAAppUi::IsSettingsDialogActive
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAppUi::IsSettingsDialogActive() const
    {
    // CodeScanner warning ignored because we are not performing
    // NULL equality checks here to check for validity of pointer.
    // Check is needed to get boolean return value.
    return iServSettingsDialog != NULL; // CSI: 64 # See above.
    }
#endif  // RD_SETTINGS_FACELIFT

// ---------------------------------------------------------
// CCAAppUi::ReportPendingDialogClosed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::ReportPendingDialogClosed()
    {
    // Counter must be over 1
    __CHAT_ASSERT_DEBUG( iPendingDialogCounter > 0 );
    iPendingDialogCounter--;
    }

// ---------------------------------------------------------
// CCAAppUi::ReportPendingDialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::ReportPendingDialog()
    {
    iPendingDialogCounter++;
    }
// CCAAppUi::LaunchSettingsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAAppUi::LaunchSettingsL ( CIMPSSAPSettings& aServer )
    {
    MCASettingsPC* SettingsPC = iProcessManager.GetSettingsInterface();
    CCAAppSettingsDialog* dlg = new ( ELeave ) CCAAppSettingsDialog( *iStatusPaneHandler,
                                                                     *SettingsPC,
                                                                     *iUISessionManager,
                                                                     &aServer );

    CleanupStack::PushL( dlg );
    dlg->ConstructL( R_CHATCLIENT_APPSETTINGS_MENUBAR );
    CleanupStack::Pop( dlg );

    return dlg->ExecuteLD( R_CHATCLIENT_APPSETTINGS_DLG );
    }


// -----------------------------------------------------------------------------
// CCAAppUi::ServerChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::ServerChangedL ( const TDesC& aNewServerName,
                                TBool aDefaultServerchanged /* =
                                EFalse */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAAppUi::ServerChangedL(%S)" ), &aNewServerName );
    if ( aNewServerName.Length() == 0 ||
         iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) == 0 )
        {
        // no servers
        return;
        }


    iNeedRefresh = UISessionManager().BrandUIL( EFalse, aNewServerName );

    // Stop here if brand did not actually change
    if ( ! iNeedRefresh && ! iServSettingsDialog )
        {
        return;
        }

    // Update status-pane
    UISessionManager().SetStatusPaneIconsL();

    // Get resource file for settings dialog
    TFileName resPath;
    TFileName resIMCUVariationPath;

    TPtrC resId( KWVSettingsResFileName() );
    TPtrC resIMCUVariationId( KIMPSCUVariationResFileName() );

    CCAVariantFactory* varFact =
        static_cast<CCAApp*>( Application() )->VariantFactory();

    // CR : 101-39724 Error note needed when application cannot be opened because of the lack of the memory
    MCAOpBrandVariant* opBrand = NULL ;
    TRAPD( err, opBrand = varFact->OpBrandVariantL() );
    if ( err )
        {
        // Using  CErrorUI, which matches the errorcode passed with coreesponding error string
        // and displays the note.
        CErrorUI* errorUI = CErrorUI::NewLC( );
        errorUI->ShowGlobalErrorNoteL( err );
        CleanupStack::PopAndDestroy( errorUI );
        User::Leave( err );
        }

    opBrand->CurrentResourceFileL( aNewServerName, resId, resPath );
    opBrand->CurrentResourceFileL( aNewServerName, resIMCUVariationId, resIMCUVariationPath );

    if ( resPath.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resPath );
        }

    if ( resIMCUVariationPath.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resIMCUVariationPath );
        }

    CHAT_DP( D_CHAT_LIT( "CCAAppUi::ServerChangedL, signalling service settings about resPath: %S" ), &resPath );
    if ( iServSettingsDialog )
        {
        iServSettingsDialog->ChangeResourceFileL( resPath, resIMCUVariationPath );
        }
    // Change resource file in iConnUIAgent
    resPath.Zero();
    TPtrC resId2( KIMPSConnUIResFileName() );
    opBrand->CurrentResourceFileL( aNewServerName, resId2, resPath );
    if ( resPath.Length() > 0 )
        {
        BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resPath );
        ChangeConnUiAgentResourceL( resPath );
        }
    if ( aDefaultServerchanged )
        {
        iProcessManager.GetArrayInterface()->ResetArray( ETrue );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::CurrentServerNameLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCAAppUi::CurrentServerNameLC()
    {
    HBufC* sapName = NULL;
    TRAPD( err,
           CIMPSSAPSettings* sap = UISessionManager().CurrentSAPLC();
           // CodeScanner warning ignored because sapName is pushed to
           // cleanupstack later and there is no leaving code before it
           sapName = sap->SAPName().AllocL();  // CSI: 35 # See comment above
           CleanupStack::PopAndDestroy( sap );
         );

    if ( err != KErrNone || !sapName )
        {
        sapName = KNullDesC().AllocL();
        }

    CleanupStack::PushL( sapName );
    return sapName;
    }


// ---------------------------------------------------------
// CCAAppUi::ConvertStatusToStatusMsgValue
// (other items were commented in a header).
// ---------------------------------------------------------
//
TEnumsPC::TCASettingStrings CCAAppUi::ConvertToTEnumsStatusMsgValue(
    TEnumsPC::TOnlineStatus aStatus )
    {
    // Map status to setting item
    switch ( aStatus )
        {
        case TEnumsPC::EAway:
            {
            return TEnumsPC::EStatusMsgAway;
            // no need to break after return
            }
        case TEnumsPC::EBusy:
            {
            return TEnumsPC::EStatusMsgBusy;
            // no need to break after return
            }
        case TEnumsPC::EOnline: // flowthrough
        default:
            {
            return TEnumsPC::EStatusMsgOnline;
            // no need to break after return
            }
        }
    }


// ---------------------------------------------------------
// CCAAppUi::ShowInfoPopupL()
// ---------------------------------------------------------
//
void CCAAppUi::ShowInfoPopupL()
    {
    iInfoPopup->HideInfoPopupNote();

    HBufC* previewBuffer = HBufC::NewLC( iTotalPreviewMsgLength + ( 3 * KLineFeed().Length() ) );

    TPtr msgPtr( previewBuffer->Des() );

    for ( TInt index( iPreviewMessageList->Count() - 1 ); index >= 0; --index )
        {
        msgPtr.Append( iPreviewMessageList->MdcaPoint( index ) );
        msgPtr.Append( KLineFeed );
        }

    iPreviewMessageList->Reset();
    iInfoPopup->SetTextL( msgPtr );
    CleanupStack::PopAndDestroy(); //previewBuffer
    iTotalPreviewMsgLength = 0;
    iInfoNoteShowing = ETrue;
    iInfoPopup->ShowInfoPopupNote();

    }

// ---------------------------------------------------------
// CCAAppUi::HandleInfoPopupNoteEvent()
// ---------------------------------------------------------
//
void CCAAppUi::HandleInfoPopupNoteEvent( CAknInfoPopupNoteController* /*aController*/,
                                         TAknInfoPopupNoteEvent aEvent )
    {

    switch ( aEvent )
        {
        case EInfoPopupNoteShown:
            {
            iInfoNoteShowing = ETrue;
            break;
            }
        case EInfoPopupNoteHidden:
            {

            iInfoNoteShowing = EFalse;

            if ( iPreviewMessageList->Count() )
                {
                //play the message tone and show preview pane
                PlayBeep();
                TRAP_IGNORE( ShowInfoPopupL() );
                }

            break;
            }
        default:
            break;
        }

    }

// ---------------------------------------------------------
// CCAAppUi::AddToPreviewListL()
// ---------------------------------------------------------
//
void CCAAppUi::AddToPreviewListL( const TDesC& aContactId, const TDesC& aMsg )
    {
    CDesCArray* array = new( ELeave )CDesCArrayFlat( 2 ); // two items
    CleanupStack::PushL( array );
    array->AppendL( aContactId );
    array->AppendL( aMsg );


    HBufC* msg = StringLoader::LoadLC( R_QTN_CHAT_MESSAGE_PREVIEW,
                                       *array );
    TPtr msgPtrTmp( msg->Des() );

    TInt numOfChars = iCurrentFont->TextCount( msgPtrTmp, iPreviewPaneRect.Width() );

    TPtrC msgPtr( KNullDesC() );
    msgPtr.Set( msg->Left( numOfChars ) );

    iTotalPreviewMsgLength += msgPtr.Length();

    if ( iPreviewMessageList->Count() == 3 )
        {
        iPreviewMessageList->Delete( 0 );
        }

    iPreviewMessageList->AppendL( msgPtr );

    CleanupStack::PopAndDestroy( 2, array );
    }

// ---------------------------------------------------------
// CCAAppUi::CalculatePreviewPaneWidth()
// ---------------------------------------------------------
//
void CCAAppUi::CalculatePreviewPaneWidth()
    {
    // Get parameter and table limits for popup preview text window
    TAknLayoutScalableParameterLimits limits =
        AknLayoutScalable_Avkon::popup_preview_text_window_ParamLimits();

    TAknLayoutScalableTableLimits tableLimits =
        AknLayoutScalable_Avkon::popup_preview_text_window_t_Limits();

    // Get layout rects
    TRect rectScreen = iAvkonAppUi->ApplicationRect();
    TRect rectMainPane = rectScreen;

    // Use first variety to be able to get the font for text parsing
    TInt firstVariety = SelectWindowVariety( 1, limits );

    TRect rectPopupWindow = RectFromLayout( rectMainPane,
                                            AknLayoutScalable_Avkon::popup_preview_text_window( firstVariety ) );

    TInt firstIndex = tableLimits.FirstIndex();
    TInt firstLineVariety = AknLayoutScalable_Avkon::
                            popup_preview_text_window_t_ParamLimits( firstIndex ).FirstVariety();

    TAknTextLineLayout popupTextLayout =
        AknLayoutScalable_Avkon::popup_preview_text_window_t(
            firstIndex, firstLineVariety );

    TAknLayoutText layoutText;
    layoutText.LayoutText( rectPopupWindow, popupTextLayout );

    iPreviewPaneRect = layoutText.TextRect();

    iCurrentFont = const_cast<CFont*> ( layoutText.Font() );

    }

// ---------------------------------------------------------
// CCAAppUi::SelectWindowVariety()
// ---------------------------------------------------------
//
TInt CCAAppUi::SelectWindowVariety( const TInt aNumberOfLines,
                                    const TAknLayoutScalableParameterLimits& aLimits ) const
    {
    TInt index = aNumberOfLines - 1;
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        index += 5;
        }

    return Min( Max( index, aLimits.FirstVariety() ),
                aLimits.LastVariety() );
    }

// ---------------------------------------------------------
// CCAAppUi::RectFromLayout()
// ---------------------------------------------------------
//
TRect CCAAppUi::RectFromLayout( const TRect& aParent,
                                const TAknWindowComponentLayout& aComponentLayout ) const
    {
    TAknWindowLineLayout lineLayout = aComponentLayout.LayoutLine();
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( aParent, lineLayout );
    return layoutRect.Rect();
    }


// ---------------------------------------------------------
// CCAAppUi::OfferTabEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCAAppUi::OfferTabEventL( const TKeyEvent&
                                       aKeyEvent, TEventCode aType )
    {

    TKeyResponse keyResponse = EKeyWasConsumed;

    CAknTabGroup* tabGroup = CAStatusPane()->TabGroup();

    if ( tabGroup )
        {
        keyResponse = tabGroup->OfferKeyEventL( aKeyEvent, aType );
        }

    return keyResponse;
    }

// ---------------------------------------------------------
// CCAAppUi::AddServerChangeObsL
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCAAppUi::AddServerChangeObsL( MCAServerChangeNotify* aObserver )
    {
    if ( aObserver )
        {
        User::LeaveIfError( iServerChangeNotify.Append( aObserver ) );
        }
    }

// ---------------------------------------------------------
// CCAAppUi::RemoveServerChangeObs
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::RemoveServerChangeObs( MCAServerChangeNotify* aObserver )
    {
    TInt status( iServerChangeNotify.Find( aObserver ) );
    if ( status != KErrNotFound )
        {
        iServerChangeNotify.Remove( status );
        iServerChangeNotify.Compress();
        }

    status = iServerChangeNotify.Count();
    if ( status == 0 )
        {
        iServerChangeNotify.ResetAndDestroy();
        iServerChangeNotify.Close();
        }
    }

// ---------------------------------------------------------
// CCAAppUi::HandleSAPEvent
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::HandleSAPEvent( TSAPEvent aEvent )
    {
    if ( aEvent == ESAPSettingChanged )
        {
        TInt count = iServerChangeNotify.Count();
        TRAP_IGNORE(
            {
            for ( TInt i = 0; i < count; i++ )
                {
                if ( ( !iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) ) )
                    {
                    iServerChangeNotify[i]->HandleServerChangeL( MCAServerChangeNotify::ENoServer );
                    }

                else
                    {
                    iServerChangeNotify[i]->HandleServerChangeL( MCAServerChangeNotify::EOtherChange );
                    }
                }
            }
        );
        }

    }
// ---------------------------------------------------------
// CCAAppUi::SetFocusFlag
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppUi::SetFocusFlag( TBool aFlag )
    {
    iFocusFlag = aFlag;
    }
// ---------------------------------------------------------
// CCAAppUi::FocusFlag
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAppUi::FocusFlag()
    {
    return  iFocusFlag;
    }

// ---------------------------------------------------------
// CCAAppUi::IsBackgroundTaskPending
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAppUi::IsBackgroundTaskPendingL()
    {
    // When the settings dialog or the exit begins, set it true
    iStopUpdateCba = ETrue;
    TBool ret = EFalse;

    CHAT_DP_TXT( "CCAAppUi::IsBackgroundTaskPendingL started" );

    MCALoginPC* loginPC = iProcessManager.GetLogInInterface();

    iBackgroundTaskPending = loginPC->IsBackgroundTaskPending();

    // check for group synch state
    if ( iBackgroundTaskPending  )
        {
        loginPC->RegisterBackGroundTaskObserver( this );

        ret = WaitToCompleteBackgroundTaskL();

        }

    CHAT_DP_TXT( "CCAAppUi::IsBackgroundTaskPendingL completed" );

    return ret;
    }

// ---------------------------------------------------------
// CCAAppUi::HandleBackGroundTaskComplete
// (other items were commented in a header).
// ---------------------------------------------------------

void CCAAppUi::HandleBackGroundTaskCompleteL( TBool aCompleted )
    {

    CHAT_DP_TXT( "CCAAppUi::HandleBackGroundTaskComplete started" );

    MCALoginPC* loginPC = iProcessManager.GetLogInInterface();

    iBackgroundTaskPending = aCompleted;

    if ( !iBackgroundTaskPending ) //group synchro is complete
        {
        // hide wait note
        if ( iWaitVisible )
            {
            TRAPD( err, DismissWaitDialogL( KErrNone ) );
            if ( err != KErrNone )
                {
                User::Leave( err );
                }
            iWaitVisible = EFalse;
            }

        loginPC->UnRegisterBackGroundTaskObserver( this );

        if ( iIsFSDExitNeeded )
            {
            HandleCommandL( EEikCmdExit );
            iIsFSDExitNeeded = EFalse;
            }

        else
            {
            // every thinh is ok ,now call actual operation now
#ifdef RD_SETTINGS_FACELIFT
            LaunchServerSettingsDialogL();
#else
            SwitchViewL( KUidSettingsView );
#endif
            // When the settings dialog exit, set it back
            iStopUpdateCba = EFalse;
            }
        }
    if ( iIsFSDExitNeeded )
        {
        HandleCommandL( EEikCmdExit );
        iIsFSDExitNeeded = EFalse;
        }
    CHAT_DP_TXT( "CCAAppUi::HandleBackGroundTaskComplete done" );
    }

// ---------------------------------------------------------
// CCAAppUi::WaitToCompleteBackgroundTask
// (other items were commented in a header).
// ---------------------------------------------------------

TBool CCAAppUi::WaitToCompleteBackgroundTaskL()
    {
    CHAT_DP_TXT( "CCAAppUi::WaitToCompleteBackgroundTask started" );

    if ( ! iWaitVisible && iBackgroundTaskPending )
        {
        CHAT_DP_TXT(
            "CCASessionHandler::GroupSyncL - ! iWaitVisible && iGroupSyncProgress" );
        // launch the wait note
        iWaitVisible = ETrue;

        ShowWaitDialogL( R_QTN_CHAT_SEARCHVIEW_PROCESSING, ETrue );

        CHAT_DP_TXT( "CCAAppUi::WaitToCompleteBackgroundTask done" );
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CCAAppUi::IsQueryAccepted
// (other items were commented in a header).
// ---------------------------------------------------------
TBool CCAAppUi::IsQueryAccepted()
    {
    return iQueryAccepted;
    }
// ---------------------------------------------------------
// CCAAppUi::IsQueryAccepted
// (other items were commented in a header).
// ---------------------------------------------------------
void CCAAppUi::SetResetForwardTo( TEnumsPC::TForwardStatus aForward )
    {
    iForward = aForward;
    }

// ---------------------------------------------------------
// CCAAppUi::IsQueryAccepted
// (other items were commented in a header).
// ---------------------------------------------------------
TEnumsPC::TForwardStatus CCAAppUi::RetForwardTo()
    {
    return iForward;
    }

// ---------------------------------------------------------
// CCAAppUi::IsQueryAccepted
// (other items were commented in a header).
// ---------------------------------------------------------
void CCAAppUi::UnRegisterPreviousview()
    {
    TInt index = 0;
    TInt count = iSwitchView.Count();

    // Check all views in queue
    for ( ; index < count; index++ )
        {
        if ( index == count - 1
             && ( iSwitchView[ index ] == KUidChatView ||
                  iSwitchView[ index ] == KUidConversationsView ||
                  iSwitchView[ index ] ==  KUidRecordedChatView ) )
            {
            iLastView = TUid::Uid( 0 );
            iSwitchView[ index ] = TUid::Uid( 0 );
            iSwitchMessage[ index ] = TUid::Uid( 0 );
            delete iSwitchMessageContent[ index ];
            iSwitchMessageContent[ index ] = NULL;

            iSwitchView.Remove( index );
            iSwitchMessage.Remove( index );
            iSwitchMessageContent.Remove( index );
            count = iSwitchView.Count();
            }
        }
    }
#ifdef RD_MULTIPLE_DRIVE
// -----------------------------------------------------------------------------
// CCAAppUi::GetDriveStatusL()
// Check the status of the drive.
// -----------------------------------------------------------------------------
//

TBool CCAAppUi::GetDriveStatusL( const TDriveNumber aDriveNumber )
    {
    RFs& fs = CCoeEnv::Static()->FsSession();
    _LIT( KFat, "Fat" );

    // Check if the drive is already mounted
    TFullName fsName;
    TInt error( fs.FileSystemName( fsName, aDriveNumber ) );
    if ( error )
        {
        return EFalse;
        }

    // check if MMC already mounted
    if ( fsName.Length() == 0 )
        {
        // MMC drive isnt mounted at present, so try it now....
        error = fs.MountFileSystem( KFat, aDriveNumber );

        // If it's a locked MMC and the password is already known it'll be
        // unlocked automatically when it's mounted., otherwise the mount will
        // return with KErrLocked.....
        switch ( error )
            {
            case KErrNone:
            case KErrLocked:
                {
                break;
                }
            default:
                {
                return EFalse;
                }
            }
        }
    TDriveInfo driveInfo;
    error = fs.Drive( driveInfo, aDriveNumber );
    if ( error )
        {
        return EFalse;
        }

    // MMC is in slot
    if ( driveInfo.iMediaAtt & KMediaAttLocked )
        {
        return EFalse;
        }

    TVolumeInfo volumeInfo;
    error = fs.Volume( volumeInfo, aDriveNumber );
    if ( error )
        {
        return EFalse;
        }

    // If type is remote drive and aConnectionState is required
    if ( driveInfo.iDriveAtt & KDriveAttRemote )
        {
        TChar driveLetter;
        fs.DriveToChar( aDriveNumber, driveLetter );
        // This statement migth cause leave.. to be solved
        CRsfwMountMan* mountMgr = CRsfwMountMan::NewL( 0, NULL );
        TRsfwMountInfo mountInfo;
        error = mountMgr->GetMountInfo( driveLetter, mountInfo );
        delete mountMgr;

        if ( error )
            {
            return EFalse;
            }
        }
    return ETrue;
    }
#endif
// -----------------------------------------------------------------------------
// CCAAppUi::GetStopUpdateCba()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAAppUi::GetStopUpdateCba()
    {
    return iStopUpdateCba;
    }


// -----------------------------------------------------------------------------
// CCAAppUi::GetIsFSWExitFlag()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAAppUi::GetIsFSWExitFlag()
    {
    return iIsFSWExit;
    }

// -----------------------------------------------------------------------------
// CCAAppUi::SetFSWExitFlag()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppUi::SetFSWExitFlag( TBool aValue )
    {
      iIsFSWExit = aValue;   
    }

// -----------------------------------------------------------------------------
// CCAAppUi::CKludgeScheduler::PublicLevel()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAAppUi::CKludgeScheduler::PublicLevel() const
    {
    return Level(); 
    }

// -----------------------------------------------------------------------------
// CCAAppUi::LevelOfActiveScheduler()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAAppUi::LevelOfActiveScheduler()
    {
    return STATIC_CAST( CCAAppUi::CKludgeScheduler*, 
        CActiveScheduler::Current() )->PublicLevel();
    }
// End of File


