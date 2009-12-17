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
* Description:  Dialog for viewing and setting application settings
 *
*/


#include "ImpsCSPAllErrors.h"
// INCLUDE FILES
//#include "CCAAppSettingsSAPExt.h"
#include "CCAStatusPaneHandler.h"
#include "chatngclient.hrh"
#include "ChatDefinitions.h"
#include "ChatDebugPrint.h"
#include "CCAAppUi.h"
#include "IMUtils.h"
#include "IMDialogUtils.h"

#include "CCAUISessionManager.h"
#include "CCAToneSettingItem.h"
#include "IMPRivateCRKeys.h"
#include "imvariant.hrh"
#include "impsbuilddefinitions.h"

#include "CCACommandManagerFactory.h"
#include "CCACommandManager.h"
#include "MCAProcessManager.h"
#include "MCACommand.h"
#include "MCASettingsCommand.h"
#include "MCAUiSettingsDialogCmdCB.h"
#include "MCASettingSapExt.h"
#include "MCASettingsPC.h"
#include "CCAAppSettingsDialog.h"

#include <AknColourSelectionGrid.h>
#include <csxhelp/imng.hlp.hrh>
#include <FeatMgr.h>
#include <aknlists.h>
#include <aknsettingitemlist.h>
#include <akntextsettingpage.h>
#include <ChatNG.rsg>
#include <centralrepository.h>

#include <hlplch.h>

#include "CCAColourSettingItem.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
#include "ccasyncchecker.h"
#endif // IMPS_CONTACT_FETCH_BACKGROUND

#if defined( IMPS_CONTACT_FETCH_BACKGROUND ) || defined( RD_SETTINGS_FACELIFT )
#include <cimpssapsettings.h>
#endif // defined( IMPS_CONTACT_FETCH_BACKGROUND ) || defined( RD_SETTINGS_FACELIFT )


//CONSTANTS
const TInt KMessageFlowMin    = 1; //Min message flow value
const TInt KMessageFlowNormal = 2; //Normal message flow
const TInt KMessageFlowMax    = 3; //Max message flow value


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAAppSettingsDialog::CCAAppSettingsDialog( CCAStatusPaneHandler& aTitleBar,
                                            MCASettingsPC& aSettingsPC,
                                            CCAUISessionManager& aUISessionManager,
                                            CIMPSSAPSettings* aServer )
        : iTitlePane( &aTitleBar ), iUISessionManager( &aUISessionManager ),
        iDefaultNick( EFalse ), iToneFileNamePtr( NULL, 0 ),  iToneFileInUse( EFalse ),
        iSettingsPC( &aSettingsPC ), iServer( aServer ), iSavingFlag( EFalse )
    {
    }

// Destructor
CCAAppSettingsDialog::~CCAAppSettingsDialog()
    {
    if ( iSettingItemArray )
        {
        iSettingItemArray->ResetAndDestroy();
        }

    delete iSettingItemArray;
    delete iDefaultNickName;
    delete iOldDefaultNickName;

    delete iToneFileName;
    // delete iSAPExtension;

    delete iAsyncCallBack;

    delete iCenRep;
    delete iCommandSetings;

    CCoeEnv* env = CCoeEnv::Static();
    if ( env->IsActive() )
        {
        env->Cancel();
        }
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::ProcessCommandL
// Command handler
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::ProcessCommandL( TInt aCommand )
    {
    HideMenu();

    if ( iSavingFlag )
        {
        return;
        }

    switch ( aCommand )
        {
        case EAknSoftkeyEdit:   // Flowthrough, MSK command
        case EChatClientAppSetChange:
            {
            SetIsTargetDialogOpened( ETrue );

            CEikListBox* listBox = static_cast< CEikListBox* >(
                                       Control( EChatClientControlIdAppSettings ) );
            HandleListBoxEventL( listBox, EEventEditingStarted );
            break;
            }

        case EChatClientCmdExit:
            {
            TryExitL( EAknSoftkeyExit );
            break;
            }
        case EChatClientCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( CEikonEnv::Static()->WsSession(),
                                                 ( CCoeEnv::Static()->AppUi() )->AppHelpContextL () );
            break;
            }
        default:
            {
            CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::HandleListBoxEventL()
// Callback for listbox events
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::HandleListBoxEventL( CEikListBox* aListBox,
                                                TListBoxEvent aEventType )
    {
    CHAT_DP( D_CHAT_LIT( "CCAAppSettingsDialog::HandleListBoxEvent %d" ),
             aEventType );

    if ( aEventType == EEventEditingStarted ||
         aEventType == EEventEnterKeyPressed ||
         aEventType == EEventItemDoubleClicked )
        {
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
        CCAAppUi* appUi = static_cast< CCAAppUi* >( iEikonEnv->AppUi() );
        CIMPSSAPSettings* currentSap = appUi->UISessionManager().CurrentSAPLC();
        if ( iServer ) // can be NULL
            {
            if ( currentSap->SAPName().CompareC( iServer->SAPName(), KCollationLevel, NULL ) == 0 )
                {
                CAknSettingItem* item = iSettingItemArray->At( iEditIndex );
                TSettingListLayout identifier = ( TSettingListLayout )item->Identifier();
                switch ( identifier )
                    {
                    case EFriendsListUpdate:
                    case EAuthorizePresence:
                        {
                        // if we're logged in, we must wait for
                        // list synchronization to complete
                        if ( appUi->UISessionManager().IsLoggedIn() &&
                             !CCASyncChecker::CheckSyncStateL() )
                            {
                            CleanupStack::PopAndDestroy( currentSap );
                            return;
                            }
                        break;
                        }
                    default:
                        {
                        break;
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy( currentSap );
#endif //IMPS_CONTACT_FETCH_BACKGROUND

        // Launch dialogs asynchronously, so we can exit this event chain.
        // If we do this synchronously, a view change (with dialogshutter)
        // after opening a dialog will crash the application.
        if ( !iAsyncCallBack->IsActive() )
            {
            iIsPopUp = aEventType == EEventEditingStarted ? ETrue : EFalse;
            iEditIndex = aListBox->CurrentItemIndex();
            iAsyncCallBack->Call();
            }
        }
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::HandleResourceChange
// Called when resources change
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    // This is called to update background of slider if skin is
    // changed while dialog is active
    TRAP_IGNORE( iSettingItemArray->RecalculateVisibleIndicesL() );

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        Layout();
        }
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::OfferKeyEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCAAppSettingsDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {
    if ( aKeyEvent.iCode == EKeyEscape )
        {
        // Forced exit by dialog shutter
        TryExitL( EAknSoftkeyExit );
        return EKeyWasConsumed;
        }

    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::DisplayMenuL
// Display the option menu
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::DisplayMenuL()
    {
    if ( !GetIsTargetDialogOpened() )
        CAknDialog::DisplayMenuL();
    else return;
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::HandleCBAChangeL()
// Handles CBA changes
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::HandleCBAChangeL( const TInt aResourceId )
    {
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->SetCommandSetL( aResourceId );
    cba->DrawNow();
    }

//----------------------------------------------------------
// CCAAppSettingsDialog::AddToneSettingItemL
//----------------------------------------------------------
void CCAAppSettingsDialog::AddToneSettingItemL( TBool aBranded )
    {
    iToneFileNamePtr.Set( iToneFileName->Des() );
    iSAPExtension->GetToneFileNameL( iToneFileNamePtr );
    CAknSettingItem* toneSett = new ( ELeave ) CCAToneSettingItem( EMessageTone,
                                                                   iToneFileNamePtr, *iCenRep, aBranded );
    CleanupStack::PushL( toneSett );
    HBufC* itemTitle =
        iEikonEnv->AllocReadResourceLC( R_CHAT_AUDIO_SETTING_TEXT );
    toneSett->ConstructL( EFalse, 0, *itemTitle, NULL, NULL, NULL );
    iSettingItemArray->AppendL( toneSett );
    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( toneSett );
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::DynInitMenuPaneL
// Setup the menupane
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::DynInitMenuPaneL( TInt aResourceId,
                                             CEikMenuPane* aMenuPane )
    {
    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dimm the help menu item
        aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
        }
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::PreLayoutDynInitL
// Setup the dialog dynamically
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::PreLayoutDynInitL()
    {
    iCenRep = CRepository::NewL( KCRUidIM );

    CCAAppUi* appUi = static_cast< CCAAppUi* >( iEikonEnv->AppUi() );
    MCACommandFactory* commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager();
    iCommandSetings = static_cast< MCASettingsCommand* > ( commandManager->NewCommandL( EChatClientApplicationSettings, appUi->GetProcessManager(), *this ) );

    iCommandSetings->ExecuteCommandL();

    iSAPExtension = iCommandSetings->GetAppSettingsSAPExt();

    CIMPSSAPSettings* server = NULL;

#ifdef RD_SETTINGS_FACELIFT
    server = iServer;
    iTitlePane->SetTitleL( server->SAPName() );
#else
    UpdateTitlePaneL( R_CHATCLIENT_APPSETTINGS_TITLE );
#endif //RD_SETTINGS_FACELIFT


    iDefaultNickName = HBufC::NewL( KNicknameMaxLength );
    iOldDefaultNick = iDefaultNick = iSettingsPC->GetBoolValuePC( TEnumsPC::EDefaultScreenNameInUse, server );


    iOldAuthorizeIMPr = iAuthorizeIMPr = iSettingsPC->GetIntValuePC( TEnumsPC::EAuthorizeIMPresence, server );


    iOldReceiveIMessages = iReceiveIMessages = iSettingsPC->GetIntValuePC( TEnumsPC::EReceiveIMessages, server );


    iOldReceiveInvitations = iReceiveInvitations = iSettingsPC->GetIntValuePC( TEnumsPC::EReceiveInvitations, server );


    iOldMsgFlow = iMsgFlow = iSettingsPC->GetIntValuePC( TEnumsPC::EMessageFlowSettingLevel,  server );


    iOldShowTimeStamp = iShowTimeStamp = iSettingsPC->GetBoolValuePC( TEnumsPC::EShowTimeStamps, server );


    iOldAutoUpdFriends = iAutoUpdFriends = iSettingsPC->GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate, server );


    iOldShowOffline = iShowOffline = iSettingsPC->GetBoolValuePC( TEnumsPC::EShowOffline, server );


    iOldOrderAlphabetically = iOrderAlphabetically = iSettingsPC->IsAlphabeticalPC( server );


    // If this is the first time when settings are opened, then set message
    // flow to "normal"
    if ( iMsgFlow < KMessageFlowMin || iMsgFlow > KMessageFlowMax )
        {
        iMsgFlow = iOldMsgFlow = KMessageFlowNormal;
        }

    // Set default nickname, if it's used
    if ( iDefaultNick )
        {
        TPtr nickNamePtr( iDefaultNickName->Des() );
        HBufC* screenname = iSettingsPC->GetSapSettingValuePCL( TEnumsPC::EDefaultScreenName,
                                                                server  );
        nickNamePtr.Copy( ( *screenname ).Left( nickNamePtr.MaxLength() ) );
        delete screenname;
        iOldDefaultNickName = nickNamePtr.AllocL();
        }

    // Initialize variables needed for adding new items
    iListbox = static_cast< CAknSettingStyleListBox* >( Control(
                                                            EChatClientControlIdAppSettings ) );
    //2 is array granularity and 1 is first ordinal
    iSettingItemArray = new ( ELeave ) CAknSettingItemArray( 2, EFalse, 1 );
    //2 is array granularity
    // CodeScanner warning is ignored because array is not used here
    // and ownership is transferred to list box immediately
    CArrayPtr< CGulIcon >* icons = new( ELeave ) CArrayPtrFlat< CGulIcon >( 2 );    // CSI: 6 # See comment above
    // Transfer icons-array ownership to listbox
    iListbox->ItemDrawer()->ColumnData()->SetIconArray( icons );

    // read variation flag values
    TInt haveScreenName =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SCREENNAME_SETTING );
    TInt haveGrant =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVEGRANT );
    TInt havePresenceAuth =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVEPRESENCEAUTH );
    // Check this value IM SingleView
    TInt updateContactsSetting =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_UPDATE_CONTACTS_SETTING );



    // and check dynamic features
    if ( havePresenceAuth )
        {
        havePresenceAuth = appUi->UISessionManager().IsSupported(
                               CCAUISessionManager::EAttrList );
        }

    TBool groups( appUi->UISessionManager().IsSupported(
                      CCAUISessionManager::EGroup ) );
    TBool invitations( appUi->UISessionManager().IsSupported(
                           CCAUISessionManager::EInvite ) );


    if ( haveScreenName && groups )
        {
        // Add item for nickname setting
        AddBinaryItemL( iDefaultNick,
                        ENickName,
                        R_CHATCLIENT_APPSETTINGS_DEFNICK_TITLE,
                        R_CHAT_SET_DEF_NICK,
                        R_CHAT_SET_DEF_NICK_TEXTS );
        }


    if ( havePresenceAuth )
        {
        // Add item for authorize IM presence setting
        AddEnumTextItemL( iAuthorizeIMPr,
                          EAuthorizePresence,
                          R_CHATCLIENT_APPSETTINGS_AUTHORIZEIMPR_TITLE,
                          R_CHAT_SET_AUTHORIZEIMPR,
                          R_CHAT_SET_AUTHORIZEIMPR_TEXTS );
        }

    if ( haveGrant )
        {
        // Add item for receive instant messages setting
        AddEnumTextItemL( iReceiveIMessages,
                          EReceiveIMessages,
                          R_CHATCLIENT_APPSETTINGS_RECEIVEIMESSAGES_TITLE,
                          R_CHAT_SET_RECEIVEIMESSAGES,
                          R_CHAT_SET_RECEIVEIMESSAGES_TEXTS );
        }

    if ( groups && invitations )
        {
        // Add item for receive invitations setting
        AddEnumTextItemL( iReceiveInvitations,
                          EReceiveInvitations,
                          R_CHATCLIENT_APPSETTINGS_RECEIVEINVITATIONS_TITLE,
                          R_CHAT_SET_RECEIVEINVITATIONS,
                          R_CHAT_SET_RECEIVEINVITATIONS_TEXTS );
        }


    // Add item for contact list ordering
    AddBinaryItemL( iOrderAlphabetically,
                    EContactOrdering,
                    R_CHATCLIENT_APPSETTINGS_ORDERCONTACTS_TITLE,
                    R_CHAT_SET_ORDERCONTACTS,
                    R_CHAT_SET_ORDERCONTACTS_TEXTS );

    // Add item for presence automatic update setting
    if ( updateContactsSetting == 0 )
        // 0: automatic and manual modes are supported
        {
        AddBinaryItemL( iAutoUpdFriends,
                        EFriendsListUpdate,
                        R_CHATCLIENT_APPSETTINGS_FRIENDUPD_TITLE,
                        R_CHAT_SET_FRIENDUPD,
                        R_CHAT_SET_FRIENDUPD_TEXTS );
        }

    // Add item for "show offline" setting
    AddBinaryItemL( iShowOffline,
                    EShowOffline,
                    R_QTN_CHAT_SET_SHOW_OFFLINE_CONTACTS,
                    R_CHAT_SET_SHOW_OFFLINE,
                    R_CHAT_SET_SHOW_OFFLINE_TEXTS );

    //Tone settings
#ifdef RD_30_COMPATIBILITY_MODE

    iBrandedTones =
        IMUtils::IntResourceValueL( R_AUDIO_FILELIST_MODEL_ENABLED );

#else // S60 Common

    // Check CR variation
    if ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
         & EIMFeatBrandableTones )
        {
        // The brandtones setting is fetched only from Resource file, so IntResourceValueFromRssL is used instead of IntResourceValueL (which avoids searching the id in Cenrep)
        iBrandedTones =
            IMUtils::IntResourceValueFromRssL( RSC_CHAT_BRANDED_TONES );
        }

#endif  // RD_30_COMPATIBILITY_MODE
    iToneFileName = HBufC::NewL( KMaxPath );
    //Add tone setting item, if curren SAP is valid
    CIMPSSAPSettings* currSAP = NULL;
    TRAPD( err,
        {
        currSAP = iUISessionManager->CurrentSAPLC();
        CleanupStack::PopAndDestroy( currSAP );
        }
         );

    if ( !err )
        {
        // the color settings
        iOwnColorDefined = iSAPExtension->IntValueL( KIMOwnMessageColourKey )
                           != KErrNotFound;
        iOthersColorDefined =
            iSAPExtension->IntValueL( KIMOthersMessageColourKey )
            != KErrNotFound;

        AddColourItemL(
            KIMOwnMessageColourKey,
            iOwnColorDefined,
            EOwnColor,
            R_QTN_CHAT_SET_FONT_COLOURS_OWN/*,
            R_CHAT_SET_MESSAGE_COLOR*/ );

        AddColourItemL(
            KIMOthersMessageColourKey,
            iOthersColorDefined,
            EOtherColor,
            R_QTN_CHAT_SET_FONT_COLOURS_OTHER/*,
            R_CHAT_SET_MESSAGE_COLOR*/ );
        }

    // Add item for "timestamp" setting
    AddBinaryItemL( iShowTimeStamp,
                    EShowTimeStamp,
                    R_QTN_CHAT_SETTINGS_TIMESTAMP,
                    R_CHAT_SET_TIMESTAMP,
                    R_CHAT_SET_TIMESTAMP_TEXTS );

    if ( !err )
        {
        // we have valid SAP
        AddToneSettingItemL( iBrandedTones );
        iToneFileInUse = ETrue;
        }

    CTextListBoxModel* model = iListbox->Model();
    model->SetItemTextArray( iSettingItemArray );
    // Ownership retained by us
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );

    iSettingItemArray->RecalculateVisibleIndicesL();
    iListbox->SetMopParent( this );
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );
    iListbox->HandleItemAdditionL();
    iListbox->SetListBoxObserver( this );
    iListbox->UpdateScrollBarsL();

    // create callback object
    iAsyncCallBack = new( ELeave )CAsyncCallBack(
        TCallBack( CallBack, this ), CActive::EPriorityIdle );
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::OkToExitL
// Can exit the dialog or not
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAppSettingsDialog::OkToExitL( TInt aButtonId )
    {


    // if edit item is ongoing,avoid to shut dialog and destruct.
    if ( iIsEditItemFlag )
        {
        return EFalse;
        }

    if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        return EFalse;
        }

    if ( aButtonId == EAknSoftkeyBack || aButtonId == EAknSoftkeyExit )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        cba->MakeCommandVisible( EAknSoftkeyBack, EFalse );
        iSavingFlag = ETrue;
        }

    if ( aButtonId == EAknSoftkeyEdit )
        {
        // MSK command
        HandleListBoxEventL( iListbox, EEventEnterKeyPressed );
        return EFalse;
        }

    CCAAppUi* appUi = static_cast< CCAAppUi* >( iEikonEnv->AppUi() );
    appUi->ShowWaitDialogL( R_QTN_CHAT_WAIT_SAVINGSETTINGS );


    TRAPD( leave, StoreValuesL() );
    // the dismisswaitdialogL leaves with the error code
    // and we must trap it to be able to exit the settings
    TRAPD( leave2, appUi->DismissWaitDialogL( leave ) );

    // show any system errors
    if ( leave2 )
        {
        CActiveScheduler::Current()->Error( leave2 );
        }

    UpdateTitlePaneL( R_CHATCLIEN_SETTINGS_VIEW_TITLE );

    if ( ( leave2 == KErrDiskFull || leave2 == KErrNoMemory ) &&
         ( aButtonId != EAknSoftkeyExit && aButtonId != EEikCmdExit
           && aButtonId != EChatClientCmdExit )
       )
        {
        // no memory/disk due to failing network operations,
        // can't do rollback so we must stay here
        // BUT if the user really wants exit via Options/Exit then we must
        // go, no matter what.
        iSavingFlag = EFalse;
        return EFalse;
        }

    return ETrue;
    }


// ---------------------------------------------------------
// CCAAppSettingsDialog::DisplayTextSettingPageL
// Launches text-setting page
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::DisplayTextSettingPageL()
    {
    TPtr defaultNickName( iDefaultNickName->Des() );
    CAknSettingPage* dlg = new( ELeave ) CAknTextSettingPage(
        R_CHAT_SET_DEF_NICK_EDIT, defaultNickName,
        CAknTextSettingPage::EZeroLengthNotOffered
        | CAknTextSettingPage::EPredictiveTextEntryPermitted );

    dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
    }


// ---------------------------------------------------------
// CCAAppSettingsDialog::AddBinaryItemL
// Add binary-type item to the settings array
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::AddBinaryItemL( TBool& aBuffer, TInt aId,
                                           TInt aTitleResource, TInt aSettingPageResource,
                                           TInt aAssociatedResource )
    {
    // Create new setting item
    CAknSettingItem* settingItem =
        new ( ELeave ) CAknBinaryPopupSettingItem( aId,  aBuffer );
    CleanupStack::PushL( settingItem );

    // Construct setting item with parametrized values
    HBufC* itemTitle = iEikonEnv->AllocReadResourceLC( aTitleResource );
    settingItem->ConstructL( EFalse, iOrdinal, *itemTitle, NULL,
                             aSettingPageResource, EAknCtPopupSettingList, NULL,
                             aAssociatedResource );
    CleanupStack::PopAndDestroy( itemTitle );
    // Append item to settingitem-array
    iSettingItemArray->AppendL( settingItem );
    CleanupStack::Pop( settingItem );

    // New item added -> increase item's ordinal value
    iOrdinal++;
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::AddEnumTextItemL
// Add enumerated text-type item to the settings array
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::AddEnumTextItemL(
    TInt& aBuffer,
    TInt aId,
    TInt aTitleResource,
    TInt aSettingPageResource,
    TInt aAssociatedResource )
    {
    // Create new setting item
    CAknSettingItem* settingItem =
        new( ELeave )CAknEnumeratedTextPopupSettingItem( aId,  aBuffer );
    CleanupStack::PushL( settingItem );

    // Construct setting item with parametrized values
    HBufC* itemTitle = iEikonEnv->AllocReadResourceLC( aTitleResource );
    settingItem->ConstructL( EFalse, iOrdinal, *itemTitle, NULL,
                             aSettingPageResource, EAknCtPopupSettingList,
                             NULL, aAssociatedResource );
    CleanupStack::PopAndDestroy( itemTitle );
    // Append item to settingitem-array
    iSettingItemArray->AppendL( settingItem );

    CleanupStack::Pop( settingItem );

    // New item added -> increase item's ordinal value
    iOrdinal++;
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::AddColourItemL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::AddColourItemL(
    const TDesC& aKey,
    TBool& aBuffer,
    TInt aId,
    TInt aTitleResource )
    {
    // Create new colour setting item
    CCAColourSettingItem* settingItem =
        new ( ELeave ) CCAColourSettingItem( *iSAPExtension, aBuffer, aId );
    CleanupStack::PushL( settingItem );

    // Construct setting item with parametrized values
    HBufC* itemTitle = iEikonEnv->AllocReadResourceLC( aTitleResource );
    CArrayPtr<CGulIcon>* iconArray =
        iListbox->ItemDrawer()->ColumnData()->IconArray();

    settingItem->ConstructL(
        aKey,
        iOrdinal,
        *itemTitle,
        iconArray );

    CleanupStack::PopAndDestroy( itemTitle );
    // Append item to settingitem-array
    iSettingItemArray->AppendL( settingItem );

    CleanupStack::Pop( settingItem );

    // New item added -> increase item's ordinal value
    iOrdinal++;
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::StoreValuesL
// Store the setting values to persistent storage
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::StoreValuesL()
    {
    StoreAllL();
    // Attention ///////////////////////////////////////////
    // Store first all settings that don't cause any network operations
    //
    CIMPSSAPSettings* server = NULL;

#ifdef RD_SETTINGS_FACELIFT
    server = iServer;
#endif //RD_SETTINGS_FACELIFT	
    // nickname
    if ( iDefaultNick && iDefaultNickName->Length() > 0 )
        {
        iSettingsPC->SetBoolValuePCL( TEnumsPC::EDefaultScreenNameInUse, ETrue, NULL, server );
        iSettingsPC->SetSapSettingValuePCL( TEnumsPC::EDefaultScreenName, *iDefaultNickName, NULL, server );
        }
    else
        {
        iSettingsPC->SetBoolValuePCL( TEnumsPC::EDefaultScreenNameInUse, EFalse, NULL, server );

        if ( iOldDefaultNick )
            {
            CCAAppUi* appUi = static_cast< CCAAppUi* >( iEikonEnv->AppUi() );
            TPtr ptr( appUi->ScreenNamePtr()->Des() );
            ptr.Copy( KNullDesC() );
            }
        }

    // flow control
    iSettingsPC->SetIntValuePCL( TEnumsPC::EMessageFlowSettingLevel, iMsgFlow, NULL, server );

    // contact list ordering
    TInt order( TEnumsPC::EPresence );
    if ( iOrderAlphabetically )
        {
        order = TEnumsPC::EAlphabetical;
        }
    iSettingsPC->SetIntValuePCL( TEnumsPC::EFriendsListOrdering, order, NULL, server );

    // "show offline contacts"
    iSettingsPC->SetBoolValuePCL( TEnumsPC::EShowOffline, iShowOffline, NULL, server );

    // receive invitations - this is currently handled locally,
    // so no network operations needed for changing this
    iSettingsPC->SetIntValuePCL( TEnumsPC::EReceiveInvitations,
                                 iReceiveInvitations, NULL, server );

    // Show "timestamp" setting
    iSettingsPC->SetBoolValuePCL( TEnumsPC::EShowTimeStamps,
                                  iShowTimeStamp, NULL, server );

    // Attention//////////
    // The following setting items might require a network operation
    //

    RArray<TInt> ignoreRollback;
    CleanupClosePushL( ignoreRollback );
    // ignore rollback operation of settings manager
    // in following error situations
    User::LeaveIfError( ignoreRollback.Append( ECSPPartiallySuccessful ) );
    User::LeaveIfError( ignoreRollback.Append( ECSPUnknownUserId ) );

    // receive IMessages from
    TRAPD( errReceiveIMsg, iSettingsPC->SetIntValuePCL( TEnumsPC::EReceiveIMessages,
                                                        iReceiveIMessages, &ignoreRollback, server ); );
    HandleErrorL( errReceiveIMsg );

    // authorize IM presence to
    TRAPD( errAuthorize,  iSettingsPC->SetIntValuePCL(
               TEnumsPC::EAuthorizeIMPresence, iAuthorizeIMPr, &ignoreRollback, server ); );
    HandleErrorL( errAuthorize );

    // automatic presence update
    TRAPD( errAutoUpd, iSettingsPC->SetBoolValuePCL(
               TEnumsPC::EAutomaticPresenceUpdate,
               iAutoUpdFriends, &ignoreRollback, server ); );

    HandleErrorL( errAutoUpd );

    if ( iToneFileInUse )
        {
        iSAPExtension->SetToneFileNameL( *iToneFileName  ); //;SetToneFileNameL( *iToneFileName );
        }

    // Flush data to persistent storage
    iSettingsPC->FlushDataPC();

    if ( errReceiveIMsg || errAuthorize || errAutoUpd )
        {
        CHAT_DP_TXT( "**Settings not updated properly**" );
        // something went wrong, leave with general server error code
        // unless it's partially succesful
        if ( errReceiveIMsg != ECSPPartiallySuccessful &&
             errAuthorize != ECSPPartiallySuccessful &&
             errAutoUpd != ECSPPartiallySuccessful )
            {
            CHAT_DP_TXT( "**leaving**" );
            User::Leave( EGeneralServerError );
            }
        }

    CleanupStack::PopAndDestroy(); // ignoreRollback.Close()
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::HandleErrorL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::HandleErrorL( TInt aErrorCode )
    {
    CHAT_DP( D_CHAT_LIT( "CCAAppSettingsDialog::HandleErrorL %d" ), aErrorCode );
    if ( ( aErrorCode < KErrNone ) && ( aErrorCode > Imps_ERROR_BASE ) )
        {
        // leave on everything but IMPS errors
        User::Leave( aErrorCode );
        }
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::StoreAllL
// Externalizes the values of setting items' iternal buffers
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::StoreAllL() const
    {
    TInt settingCount( iSettingItemArray->Count() );
    for ( TInt ii( 0 ); ii < settingCount ; ++ii )
        {
        ( *iSettingItemArray )[ii]->StoreL();
        }
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::UpdateTitlePaneL
// Updates title-pane text with text found from given resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::UpdateTitlePaneL( const TInt aResourceId )
    {
    iTitlePane->SetTitleL( iEikonEnv, aResourceId );
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::LaunchDialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAAppSettingsDialog::CallBack( TAny* aInstance )
    {
    TInt retVal( EFalse );
    TRAPD( error, static_cast<CCAAppSettingsDialog*>( aInstance )->DoHandleCallBackL(); );
    if ( error )
        {
        CActiveScheduler::Current()->Error( error );
        }

    static_cast<CCAAppSettingsDialog*>( aInstance )->SetIsTargetDialogOpened( EFalse );

    return retVal;
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::DoHandleCallBackL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAAppSettingsDialog::DoHandleCallBackL()
    {
    // If one item is in editing, just return
    if ( iIsEditItemFlag )
        return EFalse;

    __ASSERT_ALWAYS( iEditIndex >= 0 &&
                     iEditIndex < iSettingItemArray->MdcaCount(),
                     User::Leave( KErrArgument ) );

    // Check disk space before editing
    IMUtils::FFSSpaceBelowCriticalLevelL();

    TBool reload( EFalse );

    CAknSettingItem* item = iSettingItemArray->At( iEditIndex );
    TSettingListLayout identifier = ( TSettingListLayout )item->Identifier();

    // edit it
    // lock edit item operationto to avoid OkToExit() invoke
    // before EditItem() function finished.
    iIsEditItemFlag = ETrue;
    TRAPD( err, item->EditItemL( iIsPopUp ) );
    iIsEditItemFlag = EFalse;
    if ( err == KErrCancel )
        {
        // if canceled return immediately.
        return EFalse;
        }
    item->StoreL();

    iListbox->DrawItem( iEditIndex );

    // Do special settings handling for certain items
    switch ( identifier )
        {
        case ENickName:
            {
            if ( iDefaultNick )
                {
                DisplayTextSettingPageL();
                // If user did not give default nickname, then turn default
                // nickname option off
                if ( iDefaultNickName->Length() == 0 )
                    {
                    iDefaultNick = EFalse;
                    reload = ETrue;
                    }
                }
            break;
            }
        case EFriendsListUpdate:
            {
            if ( !iAutoUpdFriends )
                {
                IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_SET_WATCH_DEACTIVATED );
                }
            break;
            }

        default:
            {
            // avkon handles other items
            break;
            }
        }

    if ( reload )
        {
        // reload and update item
        item->LoadL();
        item->UpdateListBoxTextL();
        iListbox->DrawItem( iEditIndex );
        }

    HandleCBAChangeL( R_CHAT_SOFTKEYS_OPTIONS_BACK__CHANGE );
    return EFalse;
    }

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void CCAAppSettingsDialog::GetParameters( MCAUiSettingsDialogCmdCB::TSettingsDialogParams& aSettingsDialogParams )
    {

    aSettingsDialogParams.iPreserveSessionIdInExit = EFalse;

    aSettingsDialogParams.iSap = iServer;
    }



/**
 * Help Key Support
 * This function is called by the Framework to get the context to launch
 * Help
 *
 * @param aContext	The context that contains the appid and the help id.
 */
void CCAAppSettingsDialog::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_APPSETT;
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::SetIsTargetDialogOpened
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAppSettingsDialog::SetIsTargetDialogOpened( TBool aBool )
    {
    iIsTargetDialogOpened = aBool;
    }

// ---------------------------------------------------------
// CCAAppSettingsDialog::GetIsTargetDialogOpened
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAppSettingsDialog::GetIsTargetDialogOpened()
    {
    return iIsTargetDialogOpened;
    }

//  End of File
