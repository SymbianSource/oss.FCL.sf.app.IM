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
* Description:  Dialog for setting and editing group properties
*
*/


//INCLUDES
#include	"CCAGroupPropertiesDialog.h"
#include	"chatngclient.hrh"
#include	"CCAAccessListDialog.h"
#include	"CAExternalInterface.h"
#include    "IMUtils.h"
#include    "CAUtils.h"

#include    "CCAAppUi.h"
#include	"MCASettingsPC.h"
#include	"MCAProcessManager.h"
#include    "ChatDefinitions.h"
#include	"CCAStatusPaneHandler.h"
#include    "ChatDebugPrint.h"

#include	<aknlists.h>
#include	<aknradiobuttonsettingpage.h>
#include	<aknsettingpage.h>
#include	<akntextsettingpage.h>
#include	<aknsettingitemlist.h>
#include	<barsread.h>
#include	<chatNG.rsg>
#include	<stringloader.h>
#include    <bldvariant.hrh>
#include    <hlplch.h>
#include    <csxhelp/imng.hlp.hrh>
#include    <FeatMgr.h>

#include    "CCAPCUtils.h"
#include    "MCAGroupPropertiesPC.h"
#include    "ChatDefinitions.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#include 	<CAVariationNG.rsg>

const TInt KGranularity = 2;
// ---------------------------------------------------------
// CCAGroupPropertiesDialog::CCAGroupPropertiesDialog()
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
CCAGroupPropertiesDialog::CCAGroupPropertiesDialog(
    CCAStatusPaneHandler& aTitleBar,
    TBool aEditMode,
    CDesCArray& aUserList,
    CDesCArray& aAdmins,
    CDesCArray& aBannedUsers,
    const TDesC& aGroupId,
    TBool& aAreItemsEdited
)

        : iTitlePane( aTitleBar ),
        iEditMode( aEditMode ),
        iUserList( aUserList ),
        iAdmins( aAdmins ),
        iBannedUsers( aBannedUsers ),
        iGroupId( aGroupId ),
        iAreItemsEdited( aAreItemsEdited ),
        iGroupName( NULL, 0 ),
        iTopic( NULL, 0 ),
        iWelcomeNote( NULL, 0 )
    {
    iAppUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    iGroupPropertiesPC =
        iAppUi->GetProcessManager().GetGroupPropertiesPC();
    }


// ---------------------------------------------------------
// CCAGroupPropertiesDialog::~CCAGroupPropertiesDialog()
// Desctructor
// ---------------------------------------------------------
//
CCAGroupPropertiesDialog::~CCAGroupPropertiesDialog()
    {
    // dialog will be gone, so remove the topmost decorator
    iTitlePane.PopNaviPane();

    if ( iSettingItemArray )
        {
        iSettingItemArray->ResetAndDestroy();
        delete iSettingItemArray;
        }

    if ( iGroupNameBuf )
        {
        delete iGroupNameBuf;
        }

    if ( iTopicBuf )
        {
        delete iTopicBuf;
        }

    if ( iWelcomeNoteBuf )
        {
        delete iWelcomeNoteBuf;
        }

    if ( iOldGroupName )
        {
        delete iOldGroupName;
        }
    if ( iOldTopic )
        {
        delete iOldTopic;
        }
    if ( iOldWelcomeNote )
        {
        delete iOldWelcomeNote;
        }

    if ( iDialogLaunchCallback->IsActive() )
        {
        iDialogLaunchCallback->Cancel();
        }
    delete iDialogLaunchCallback;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::ConstructL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::ConstructL( TInt aMenuResource )
    {
    CAknDialog::ConstructL( aMenuResource );
    iDialogLaunchCallback = CIdle::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::LaunchAccessListDialogL()
// Launches accesslist dialog
// (other items were commented in a header).
// Note: CodeScanner is likely to return false positives for
// this situation, because some returned TInt values will not be error codes
// ---------------------------------------------------------
//
TInt CCAGroupPropertiesDialog::LaunchAccessListDialogL()
    {
    iIsMemberAccessModified = EFalse;
    CCAAccessListDialog* dlg = new( ELeave )CCAAccessListDialog(
        iTitlePane, iUserList, iIsMemberAccessModified, ETrue, ETrue,
        R_CHATCLIENT_ACLIST_TITLE, CCAAccessListDialog::EAccessList, &iBannedUsers );

    CleanupStack::PushL( dlg );
    dlg->ConstructL( R_CHATCLIENT_MENUBAR_ACCESSLIST_DLG );
    CleanupStack::Pop( dlg );
    TInt retVal( dlg->ExecuteLD( R_CHATCLIENT_ACCESS_LIST_DLG ) );

    // Don't do anything if we exited
    if ( retVal != EAknSoftkeyCancel )
        {
        if ( iUserList.Count() > 0 )
            {
            CHAT_DP_TXT( "access to group: selected" );
            iIsOpen = TEnumsPC::EImpsPropYes;
            if ( iBannedUsers.Count() == 0 )
                {
                iIsBannedUsers = EFalse;
                }
            else
                {
                iIsBannedUsers = ETrue;
                }
            }
        else
            {
            CHAT_DP_TXT( "access to group: all" );
            iIsOpen = TEnumsPC::EImpsPropNo;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::LaunchEditingRightsListDialogL()
// Launches editing rights list dialog
// Note: CodeScanner is likely to return false positives for
// this situation, because some returned TInt values will not
// be error codes
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupPropertiesDialog::LaunchEditingRightsListDialogL()
    {
    iIsAdminStatusModified = EFalse;
    // first item is our own wvid and it can't be removed
    CCAAccessListDialog* dlg = new( ELeave )CCAAccessListDialog(
        iTitlePane, iAdmins, iIsAdminStatusModified, EFalse, ETrue,
        R_CHATCLIENT_EDITRIGHT_TITLE, CCAAccessListDialog::EAdminList, &iBannedUsers );

    CleanupStack::PushL( dlg );
    dlg->ConstructL( R_CHATCLIENT_MENUBAR_EDITINGRIGHTS_DLG );
    CleanupStack::Pop( dlg );
    TInt retVal( dlg->ExecuteLD( R_CHATCLIENT_ACCESS_LIST_DLG ) );

    if ( retVal != EAknSoftkeyCancel )
        {
        TInt adminCount( iAdmins.Count() );

        // if there are other ids than ours
        if ( adminCount > 1 )
            {
            iAdminSelected = ETrue;

            if ( iBannedUsers.Count() == 0 )
                {
                iIsBannedUsers = EFalse;
                }
            else
                {
                iIsBannedUsers = ETrue;
                }
            }
        else
            {
            iAdminSelected = EFalse;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::LaunchBannedUsersListDialogL()
// Launches banned users list dialog
// Note: CodeScanner is likely to return false positives for
// this situation, because some returned TInt values will not
// be error codes.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupPropertiesDialog::LaunchBannedUsersListDialogL()
    {
    iIsBannedListModified = EFalse;
    CCAAccessListDialog* dlg = new( ELeave )CCAAccessListDialog(
        iTitlePane, iBannedUsers, iIsBannedListModified, ETrue, ETrue,
        R_CHATCLIENT_BANLIST_TITLE, CCAAccessListDialog::EBannedList,
        &iUserList, &iAdmins );

    CleanupStack::PushL( dlg );
    dlg->ConstructL( R_CHATCLIENT_MENUBAR_BANNEDLIST_DLG );
    CleanupStack::Pop( dlg );
    TInt retVal( dlg->ExecuteLD( R_CHATCLIENT_ACCESS_LIST_DLG ) );

    if ( retVal != EAknSoftkeyCancel )
        {
        TInt bannedCount( iBannedUsers.Count() );

        if ( bannedCount > 0 )
            {
            iIsBannedUsers = ETrue;

            if ( iUserList.Count() == 0 )
                {
                iIsOpen = TEnumsPC::EImpsPropNo;
                }
            else
                {
                iIsOpen = TEnumsPC::EImpsPropYes;
                }
            }
        else
            {
            iIsBannedUsers = EFalse;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::LaunchDialogHelperL
// Show popup and handle Cancel correctly when launching list dialogs
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupPropertiesDialog::LaunchDialogHelperL(
    TInt aPopupResourceId,
    TInt aSettingResourceId,
    TInt& aSelectionIndex,
    TInt ( CCAGroupPropertiesDialog::* aCallBack )(),
    TBool aInvokeOnFalse )
    {
    TResourceReader rr;
    iEikonEnv->CreateResourceReaderLC( rr, aPopupResourceId );
    CDesCArray* itemArray = rr.ReadDesCArrayL();

    //code scanner warning ignored
    CleanupStack::PopAndDestroy(); //rr

    CleanupStack::PushL( itemArray );

    CAknSettingPage* settPage = new ( ELeave )
    CAknRadioButtonSettingPage( aSettingResourceId, aSelectionIndex,
                                itemArray );
    TBool oked( settPage->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) );

    TInt retVal = KErrNone;
    //Note: CodeScanner is likely to return false positives for this situation,
    //because some returned TInt values will not be error codes
    if ( oked )
        {
        // weird logic is needed because some calls want to invoke
        // on EFalse, and some on ETrue
        TBool invoke = aInvokeOnFalse ? !( aSelectionIndex ) : aSelectionIndex;
        if ( invoke )
            {
            retVal = ( this->*aCallBack )();
            }
        }

    CleanupStack::PopAndDestroy( itemArray );

    return retVal;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::LaunchDialogHelperL
// Show popup and handle Cancel correctly when launching list dialogs
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupPropertiesDialog::LaunchDialogHelperL(
    TInt aPopupResourceId,
    TInt aSettingResourceId,
    TInt& aSelectionIndex,
    TInt ( CCAGroupPropertiesDialog::* aCallBack )(),
    TEnumsPC::TImpsPropertyBoolPC aInvokeOnTrue )
    {
    TResourceReader rr;
    iEikonEnv->CreateResourceReaderLC( rr, aPopupResourceId );
    CDesCArray* itemArray = rr.ReadDesCArrayL();

    //code scanner warning ignored
    CleanupStack::PopAndDestroy(); //rr

    CleanupStack::PushL( itemArray );

    CAknSettingPage* settPage = new ( ELeave )
    CAknRadioButtonSettingPage( aSettingResourceId, aSelectionIndex,
                                itemArray );
    TBool oked( settPage->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) );

    TInt retVal = KErrNone;
    //Note: CodeScanner is likely to return false positives for this situation,
    //because some returned TInt values will not be error codes
    if ( oked )
        {
        TBool invoke = ( TInt )aInvokeOnTrue ? aSelectionIndex : !( aSelectionIndex );
        if ( invoke )
            {
            retVal = ( this->*aCallBack )();
            }
        }

    CleanupStack::PopAndDestroy( itemArray );

    return retVal;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::HandleCBAChangeL()
// Handles CBA changes
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::HandleCBAChangeL()
    {
    iTitlePane.ClearNaviPaneL();
    iTitlePane.SetTitleL( *iGroupNameBuf );

    TInt index = iListbox->CurrentItemIndex();

    TInt indexShift = 0;
    if ( !iEditMode && index >= ESetGroupId )
        {
        ++indexShift; // if creating new group, one setting item is missing
        }

    TInt shiftedIndex = index + indexShift;

    TInt cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__EMPTY;

    if ( shiftedIndex == ESetGroupId && iIsEdited )
        {
        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_DONE__EMPTY;
        }
    else if ( shiftedIndex != ESetGroupId && iIsEdited )
        {
        // Options/Done/Change
        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_DONE__CHANGE;
        }
    else if ( shiftedIndex == ESetGroupId && !iIsEdited )
        {
        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__EMPTY;
        }
    else if ( shiftedIndex != ESetGroupId && !iIsEdited )
        {
        // Options/Back/Change
        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__CHANGE;
        }

    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->SetCommandSetL( cbaRes );
    cba->DrawNow();
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::AreItemsEditedL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAGroupPropertiesDialog::AreItemsEdited()
    {
    if ( iOldGroupName->CompareC( *iGroupNameBuf ) != 0 )
        {
        return ETrue;
        }

    if ( iOldTopic->CompareC( *iTopicBuf ) != 0 )
        {
        return ETrue;
        }

    if ( iOldWelcomeNote->CompareC( *iWelcomeNoteBuf ) != 0 )
        {
        return ETrue;
        }

    if ( iOldSize != iSize )
        {
        return ETrue;
        }

    if ( iOldAllowWhisper != iAllowWhisper )
        {
        return ETrue;
        }

    if ( iOldAllowSearch != iAllowSearch )
        {
        return ETrue;
        }

    if ( iOldIsBannedUsers != iIsBannedUsers )
        {
        return ETrue;
        }

    if ( iIsBannedListModified )
        {
        return ETrue;
        }

    if ( iOldIsOpen != iIsOpen )
        {
        return ETrue;
        }

    if ( iIsMemberAccessModified )
        {
        return ETrue;
        }

    if ( iOldAdminSelected != iAdminSelected )
        {
        return ETrue;
        }

    if ( iIsAdminStatusModified )
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::AddTextSettingItemL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::AddTextSettingItemL(
    TInt aSettingPageResourceId,
    TInt aTitleResourceId,
    TInt aOrdinal,
    TDesC& aEmptyText,
    TDes& aModifiedBuffer )
    {
    CAknSettingItem* settingItem = new ( ELeave )
    CAknTextSettingItem( 0, aModifiedBuffer );
    CleanupStack::PushL( settingItem );
    HBufC* itemTitle = iEikonEnv->AllocReadResourceLC( aTitleResourceId );
    settingItem->SetEmptyItemTextL( aEmptyText );
    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             aSettingPageResourceId, EEikCtEdwin );
    iSettingItemArray->AppendL( settingItem );
    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::AddIntegerSettingItemL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::AddIntegerSettingItemL(
    TInt aSettingPageResourceId,
    TInt aTitleResourceId,
    TInt aOrdinal,
    TInt& aModifiedValue )
    {
    CAknSettingItem* settingItem = new ( ELeave )
    CAknIntegerEdwinSettingItem( 0, aModifiedValue );
    CleanupStack::PushL( settingItem );
    HBufC* itemTitle = iEikonEnv->AllocReadResourceLC( aTitleResourceId );
    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             aSettingPageResourceId, EAknCtIntegerEdwin );
    iSettingItemArray->AppendL( settingItem );
    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::AddBinarySettingItemL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::AddBinarySettingItemL(
    TInt aSettingPageResourceId,
    TInt aTitleResourceId,
    TInt aAssociatedResourceId,
    TInt aOrdinal,
    TEnumsPC::TImpsPropertyBoolPC& aModifiedValue )
    {

    CAknSettingItem* settingItem = new ( ELeave )
    CAknBinaryPopupSettingItem( 0, ( TInt& )aModifiedValue );

    CleanupStack::PushL( settingItem );
    HBufC* itemTitle = iEikonEnv->AllocReadResourceLC( aTitleResourceId );
    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             aSettingPageResourceId, EAknCtPopupSettingList, NULL,
                             aAssociatedResourceId );
    iSettingItemArray->AppendL( settingItem );
    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::ProcessCommandL()
// Processess commands
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::ProcessCommandL( TInt aCommand )
    {
    HideMenu();
    switch ( aCommand )
        {
        case EChatClientChatPropertiesCmdChange:
            {
            CEikListBox* listBox = static_cast< CEikListBox* >
                                   ( Control( EChatClientControlIdGroupSettings ) );
            HandleListBoxEventL( listBox, EEventEditingStarted );
            break;
            }
        case EChatClientCmdExit:
            {
            iEikonEnv->EikAppUi()->HandleCommandL( EAknSoftkeyExit );
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
            iEikonEnv->EikAppUi()->HandleCommandL( aCommand );
            break;
            }
        }

    }


// ---------------------------------------------------------
// CCAGroupPropertiesDialog::HandleListBoxEventL()
// Handles listbox events
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::HandleListBoxEventL( CEikListBox* aListBox,
                                                    TListBoxEvent aEventType )
    {
    TBool isPopUp( EFalse );
    TInt index ( aListBox->CurrentItemIndex() );
    if ( index < 0 || index >= aListBox->Model()->NumberOfItems() )
        {
        //if index is not valid then we don't need make any actions to items
        return;
        }

    if ( aEventType == EEventEditingStarted ||
         aEventType == EEventEnterKeyPressed ||
         aEventType == EEventItemDoubleClicked )
        {

        TInt indexShift( 0 );
        if ( !iEditMode && index >= ESetGroupId )
            {
            ++indexShift; // if creating new group, one setting item is missing
            }

        isPopUp = ( aEventType == EEventEditingStarted );
        TInt shiftedIndex( index + indexShift );

        //Store the setting item pointer
        CAknSettingItem* settingItem = NULL;
        if ( index >= 0 && index < iSettingItemArray->Count() )
            {
            settingItem = iSettingItemArray->At( index );
            }

        switch ( shiftedIndex )
            {
                // group name
            case ESetGroupName :
                {
                // launch a T9-enabled text setting page
                CAknSettingPage* dlg = new( ELeave ) CAknTextSettingPage(
                    R_EDIT_CHAT_GROUP_NAME, iGroupName,
                    CAknTextSettingPage::EPredictiveTextEntryPermitted );

                dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
                break;
                }

            case ESetGroupTopic :
                {
                // launch a T9-enabled text setting page
                CAknSettingPage* dlg = new( ELeave ) CAknTextSettingPage(
                    R_EDIT_CHAT_GROUP_TOPIC, iTopic,
                    CAknTextSettingPage::EZeroLengthAllowed |
                    CAknTextSettingPage::EPredictiveTextEntryPermitted );

                dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
                break;
                }

            case ESetGroupWelcomeNote :
                {
                // launch a T9-enabled text setting page
                CAknSettingPage* dlg = new( ELeave ) CAknTextSettingPage(
                    R_EDIT_CHAT_GROUP_WELCOMENOTE, iWelcomeNote,
                    CAknTextSettingPage::EZeroLengthAllowed |
                    CAknTextSettingPage::EPredictiveTextEntryPermitted );

                dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
                break;
                }

            case ESetGroupId:
                {
                // editing group id is prohibited, we can just return
                return;
                }

            // these are handled with LaunchCorrectDialogL
            case ESetGroupAdminSelected:
            case ESetGroupIsOpen:
            case ESetGroupBannedUsers:  // flowtrough
                {
                LaunchCorrectDialog();
                return; // screen update is handled in LaunchCorrectDialogL
                }

            default:
                {
                if ( settingItem )
                    {
                    settingItem->EditItemL( isPopUp );
                    settingItem->StoreL();
                    }
                aListBox->DrawItem( index );
                break;
                }
            }
        if ( settingItem )
            {
            settingItem->LoadL();
            settingItem->UpdateListBoxTextL();
            }
        aListBox->DrawItem( index );

        // restore the title pane text correctly even when Back was pressed
        iTitlePane.ClearNaviPaneL();
        iTitlePane.SetTitleL( *iGroupNameBuf );
        }

    if ( AreItemsEdited() )
        {
        iIsEdited = ETrue;
        HandleCBAChangeL();
        }

    }


// ---------------------------------------------------------
// CCAGroupPropertiesDialog::DynInitMenuPaneL(
// Dynamically initialises menubar
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::DynInitMenuPaneL( TInt aResourceId,
                                                 CEikMenuPane* aMenuPane )
    {
    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dim the help menu item
        aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
        }
    TInt currentIndex = iListbox->CurrentItemIndex();

    if ( currentIndex == ESetGroupId &&
         aResourceId == R_CHATCLIENT_CHAT_PROPERTIES_OPTIONS_MENU )
        {
        aMenuPane->SetItemDimmed( EChatClientChatPropertiesCmdChange, ETrue );
        }
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::PreLayoutDynInitL()
// Sets the layout of the dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::PreLayoutDynInitL()
    {
    CHAT_DP( D_PLAIN_LIT( "CCAGroupPropertiesDialog::PreLayoutDynInitL()" ) );

    // remember to pop this stacking decorator on destructor.
    iTitlePane.ClearNaviPaneL( ETrue );

    if ( iEditMode )
        {
        TInt err( KErrNone );
        TRAPD( leave, err = iGroupPropertiesPC->InitializeForEdittingL( iGroupId ) );
        if ( leave != KErrNone )
            {
            CActiveScheduler::Current()->Error( leave );
            return;
            }
        if ( err != KErrNone )
            {
            return;
            }
        iGroupNameBuf = HBufC::NewL( KMaxGroupNameLength );
        iGroupName.Set( iGroupNameBuf->Des() );
        iGroupName.Copy( iGroupPropertiesPC->
                         GetGroupName( iGroupId ).Left( iGroupName.MaxLength() ) );
        iOldGroupName = iGroupName.AllocL();
        CHAT_DP( D_PLAIN_LIT( "iOldGroupName = %S" ), iOldGroupName );

        iTopicBuf = HBufC::NewL( KMaxGroupTopicLength );
        iTopic.Set( iTopicBuf->Des() );
        iTopic.Copy( iGroupPropertiesPC->GetTopic().Left( iTopic.MaxLength() ) );
        iOldTopic = iTopic.AllocL();

        iWelcomeNoteBuf = HBufC::NewL( KMaxWelcomeNoteLength );
        iWelcomeNote.Set( iWelcomeNoteBuf->Des() );
        iWelcomeNote.Copy( iGroupPropertiesPC->
                           GetWelcomeNote().Left( iWelcomeNote.MaxLength() ) );
        iOldWelcomeNote = iWelcomeNote.AllocL();

        if ( iBannedUsers.MdcaCount() > 0 )
            {
            iIsBannedUsers = ETrue;
            }
        iOldIsBannedUsers = iIsBannedUsers;

        iSize = iGroupPropertiesPC->GetMaxNbrOfUsers();
        iOldSize = iSize;

        iAllowSearch = iGroupPropertiesPC->IsSearchable();
        iOldAllowSearch = iAllowSearch;

        iAllowWhisper = iGroupPropertiesPC->IsPrivateAllowed();
        iOldAllowWhisper = iAllowWhisper;

        iIsOpen = iGroupPropertiesPC->IsOpen();

        iOldIsOpen = iIsOpen;

        if ( iAdmins.MdcaCount() > 1 )
            {
            iAdminSelected = ETrue;
            }
        iOldAdminSelected = iAdminSelected;

        }
    else
        {

        MCASettingsPC* settingsInterfacePC =
            iAppUi->GetProcessManager().GetSettingsInterface();

        CHAT_DP( D_PLAIN_LIT(
                     "CCAGroupPropertiesDialog: groupInterfacePC->NumberOfCreatedGroupsDuringSession()" ) );
        TInt nbrOfChats( iGroupPropertiesPC->NumberOfCreatedGroupsDuringSession() );

        HBufC* baseName = NULL;



        if ( settingsInterfacePC->GetBoolValuePC( TEnumsPC::EDefaultScreenNameInUse,
                                                  NULL ) )
            {
            CHAT_DP( D_PLAIN_LIT( "CCAGroupPropertiesDialog: settings value" ) );
            baseName = settingsInterfacePC->GetSapSettingValuePCL(
                           TEnumsPC::EDefaultScreenName, NULL );
            }
        else
            {
            CHAT_DP( D_PLAIN_LIT( "CCAGroupPropertiesDialog: no default nickname" ) );
            TInt atIndex( 0 );

            HBufC* ownUserId = settingsInterfacePC->GetSapSettingValuePCL(
                                   TEnumsPC::EOwnWVUserID, NULL );
            CleanupStack::PushL( ownUserId );
            TPtrC userId( ownUserId->Des() );

            atIndex = userId.Find( KAt );
            if ( atIndex == KErrNotFound )
                {
                atIndex = userId.Length();
                }

            // check if there is "wv:" in the beginning of user id
            if ( KWV().CompareC( userId.Left( KWV().Length() ) ) == 0 )
                {
                // yup, "wv:" found, take the userid without it
                baseName = userId.Mid( KWV().Length(),
                                       atIndex - KWV().Length() ).AllocL();
                }
            else
                {
                // no "wv:" found, take the user id starting from the beginning
                baseName = userId.Mid( 0, atIndex ).AllocL();
                }

            CleanupStack::PopAndDestroy( ownUserId );
            CHAT_DP( D_CHAT_LIT( "NICKNAME IS HERE ************* : %S" ),
                     baseName );
            }

        CleanupStack::PushL( baseName );
        // adding one because group names cannot start count from 0
        // minimum allowed value is 1, so for first group group name
        // count is 1 (0+1)
        iGroupNameBuf = StringLoader::LoadL(
                            RSC_CHAT_GROUP_DEFAULT, *baseName, nbrOfChats + 1 );

        CleanupStack::PopAndDestroy( baseName );

        iGroupNameBuf = iGroupNameBuf->ReAlloc( KMaxGroupNameLength );
        iGroupName.Set( iGroupNameBuf->Des() );
        iOldGroupName = iGroupName.AllocL();


        iTopicBuf = iEikonEnv->AllocReadResourceL(
                        RSC_CRRSS_CHAT_VARIATION_START_ID +
                        RSC_CHAT_VARIATION_GROUP_TOPIC );
        iTopicBuf = iTopicBuf->ReAlloc( KMaxGroupTopicLength );
        iTopic.Set( iTopicBuf->Des() );
        iOldTopic = iTopic.AllocL();


        iWelcomeNoteBuf = iEikonEnv->AllocReadResourceL(
                              RSC_CRRSS_CHAT_VARIATION_START_ID +
                              RSC_CHAT_VARIATION_GROUP_WELCOME_NOTE );
        iWelcomeNoteBuf = iWelcomeNoteBuf->ReAlloc( KMaxWelcomeNoteLength );
        iWelcomeNote.Set( iWelcomeNoteBuf->Des() );
        iOldWelcomeNote = iWelcomeNote.AllocL();

        iSize = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_SIZE );
        iOldSize = iSize;


        iIsBannedUsers = EFalse;
        iOldIsBannedUsers = iIsBannedUsers;

        iIsOpen = IMUtils::IntResourceValueL(
                      RSC_CHAT_VARIATION_GROUP_OPEN ) == 1 ?
                  TEnumsPC::EImpsPropYes : TEnumsPC::EImpsPropNo;
        iOldIsOpen = iIsOpen;

        iAdminSelected = EFalse;
        iOldAdminSelected = iAdminSelected;

        iAllowSearch = IMUtils::IntResourceValueL(
                           RSC_CHAT_VARIATION_GROUP_SEARCH ) == 1 ?
                       TEnumsPC::EImpsPropYes : TEnumsPC::EImpsPropNo;
        iOldAllowSearch = iAllowSearch;

        iAllowWhisper = IMUtils::IntResourceValueL(
                            RSC_CHAT_VARIATION_GROUP_WHISPER ) == 1 ?
                        TEnumsPC::EImpsPropYes : TEnumsPC::EImpsPropNo;
        iOldAllowWhisper = iAllowWhisper;
        }

    CHAT_DP( D_PLAIN_LIT(
                 "CCAGroupPropertiesDialog: Setting item array populating" ) );
    iIsEdited = EFalse;

    CAknSettingStyleListBox* listbox =
        static_cast< CAknSettingStyleListBox* >
        ( Control( EChatClientControlIdGroupSettings ) );

    // 2 is array granularity, 1 is initial ordinal.
    iSettingItemArray = new ( ELeave ) CAknSettingItemArray( 2, EFalse, 1 );
    //2 is array granularity
    // CodeScanner warning is ignored because array is not used here
    // and ownership is transferred to list box immediately
    CArrayPtr< CGulIcon >* icons = new( ELeave ) CArrayPtrFlat< CGulIcon >( KGranularity );    // CSI: 6 # See comment above
    listbox->ItemDrawer()->ColumnData()->SetIconArray( icons );

    //Setting item for groupname
    AddTextSettingItemL( R_EDIT_CHAT_GROUP_NAME, RSC_CHAT_GROUP_TITLE,
                         ESetGroupName, *iGroupNameBuf, iGroupName );

    if ( iEditMode )
        {
        HBufC* groupBuf = CCAPCUtils::DisplayId( iGroupId ).AllocLC();
        TPtr groupId( groupBuf->Des() );
        AddTextSettingItemL( NULL, RSC_CHAT_GROUP_ID_TITLE,
                             ESetGroupId, *groupBuf, groupId );
        CleanupStack::PopAndDestroy( groupBuf );
        }

    //Setting item for topic
    AddTextSettingItemL( R_EDIT_CHAT_GROUP_TOPIC,
                         RSC_CHAT_TOPIC_TITLE,
                         ESetGroupTopic,
                         *iTopicBuf,
                         iTopic );

    //Setting item for welcome note
    AddTextSettingItemL( R_EDIT_CHAT_GROUP_WELCOMENOTE,
                         RSC_CHAT_WELCOME_TITLE,
                         ESetGroupWelcomeNote,
                         *iWelcomeNoteBuf,
                         iWelcomeNote );

    //Setting item for group size
    AddIntegerSettingItemL( R_EDIT_CHAT_GROUP_SIZE,
                            RSC_CHAT_SIZE_TITLE,
                            ESetGroupSize,
                            iSize );
    //Searchable setting item
    AddBinarySettingItemL( R_EDIT_CHAT_SEARCH_SETTING,
                           RSC_CHAT_ALLOW_SEARCH_TITLE,
                           R_EDIT_CHAT_SEARCH_TEXTS,
                           ESetGroupAllowSearch,
                           iAllowSearch );

    //Admin selection setting item
    AddBinarySettingItemL( R_EDIT_CHAT_ADMIN_SETTING,
                           RSC_CHAT_ADMIN_TITLE,
                           R_EDIT_CHAT_ADMIN_TEXTS,
                           ESetGroupAdminSelected,
                           ( TEnumsPC::TImpsPropertyBoolPC& )iAdminSelected );

    //Group openess setting item
    AddBinarySettingItemL( R_EDIT_CHAT_ACCESS_SETTING,
                           RSC_CHAT_ACCESS_TITLE,
                           R_EDIT_CHAT_ACCESS_TEXTS,
                           ESetGroupIsOpen,
                           /*(TEnumsPC::TImpsPropertyBoolPC&)*/ iIsOpen );

    //Banned users setting item

    AddBinarySettingItemL( R_EDIT_CHAT_BAN_SETTING,
                           RSC_CHAT_BANNED_TITLE,
                           R_EDIT_CHAT_BAN_TEXTS,
                           ESetGroupBannedUsers,
                           ( TEnumsPC::TImpsPropertyBoolPC& )iIsBannedUsers );

    //Whisper setting item
    AddBinarySettingItemL( R_EDIT_CHAT_WHISPER_SETTING,
                           RSC_CHAT_WHISPERING_YES_TITLE,
                           R_EDIT_CHAT_WHISPER_TEXTS,
                           ESetGroupAllowWhisper,
                           iAllowWhisper );

    CTextListBoxModel* model = listbox->Model();
    model->SetItemTextArray( iSettingItemArray );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray ); // Ownership retained by us

    iSettingItemArray->RecalculateVisibleIndicesL();
    listbox->CreateScrollBarFrameL( ETrue );
    listbox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    listbox->HandleItemAdditionL();
    listbox->SetListBoxObserver( this );
    listbox->UpdateScrollBarsL();

    iTitlePane.SetTitleL( *iGroupNameBuf );
    CHAT_DP( D_PLAIN_LIT(
                 "CCAGroupPropertiesDialog::PreLayoutDynInitL() End" ) );

    iListbox =
        static_cast< CAknSettingStyleListBox* >( Control( EChatClientControlIdGroupSettings ) );

    HandleCBAChangeL();
    }


// ---------------------------------------------------------
// CCAGroupPropertiesDialog::OkToExitL()
// Called ok button is pressed
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAGroupPropertiesDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        return EFalse;
        }

    if ( aButtonId == EAknSoftkeyEmpty )
        {
        // MSK command id when focus on group id
        return EFalse;
        }

    if ( aButtonId == EAknSoftkeyEdit )
        {
        // MSK command
        TInt index = iListbox->CurrentItemIndex();

        TInt indexShift = 0;
        if ( !iEditMode && index >= ESetGroupId )
            {
            ++indexShift; // if creating new group, one setting item is missing
            }
        TInt shiftedIndex = index + indexShift;

        switch ( shiftedIndex )
            {
            case ESetGroupAdminSelected:
            case ESetGroupIsOpen:
            case ESetGroupBannedUsers:  // Flowtrough
                {
                LaunchCorrectDialog();
                break;
                }
            default:
                {
                HandleListBoxEventL( iListbox,
                                     MEikListBoxObserver::EEventEnterKeyPressed );
                break;
                }
            }
        return EFalse;
        }

    if ( iIsEdited )
        {
        StoreValuesL();

        // remove admins if creator only selected
        if ( !iAdminSelected )
            {
            TInt count( iAdmins.Count() );
            if ( count > 1 )
                {
                iAdmins.Delete( 1, count - 1 );
                }
            }

        // remove bans if user selected no bans
        if ( !iIsBannedUsers )
            {
            iBannedUsers.Reset();
            }

        // remove access if the group is open to everyone
        if ( !iIsOpen )
            {
            iUserList.Reset();
            }
        }

    return ETrue;
    }


// ---------------------------------------------------------
// CCAGroupPropertiesDialog::StoreValuesL()
// Stores the values that are changed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::StoreValuesL()
    {
    TInt count( iSettingItemArray->Count() );
    for ( TInt ii( 0 ); ii < count; ++ii )
        {
        if ( ii != 1 || !iEditMode )
            {
            // in edit mode the second item cannot be edited,
            // so don't save it
            iSettingItemArray->At( ii )->StoreL();
            }
        }

    // store the values to new properties so that we don't change anything
    // locally.
    iGroupPropertiesPC->SetGroupNameL( *iGroupNameBuf );
    iGroupPropertiesPC->SetTopicL( *iTopicBuf );
    iGroupPropertiesPC->SetWelcomeL( *iWelcomeNoteBuf );

    iGroupPropertiesPC->SetMaxNbrOfUsers( iSize );
    iGroupPropertiesPC->SetPrivateAllowed( iAllowWhisper );
    iGroupPropertiesPC->SetSearchable( iAllowSearch );
    iGroupPropertiesPC->SetOpen( iIsOpen );

    CHAT_DP( D_CHAT_LIT( "--isOpen: %d--" ), iIsOpen );
    CHAT_DP( D_CHAT_LIT( "--AllowWhisper: %d--" ), iAllowWhisper );
    iAreItemsEdited = AreItemsEdited();
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::OfferKeyEventL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCAGroupPropertiesDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                       TEventCode aType )
    {
    if ( aKeyEvent.iCode == EKeyEscape )
        {
        TryExitL( EAknSoftkeyCancel );
        return EKeyWasConsumed;
        }

    else if ( aKeyEvent.iCode == EKeyPhoneSend )
        {
        return EKeyWasConsumed;
        }

    TKeyResponse res = iListbox->OfferKeyEventL( aKeyEvent, aType );

    if ( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow )
        {
        HandleCBAChangeL();
        }

    return res;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::DoLaunchCorrectDialogL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAGroupPropertiesDialog::DoLaunchCorrectDialogL()
    {
    TInt index ( iListbox->CurrentItemIndex() );
    if ( index < 0 || index >= iListbox->Model()->NumberOfItems() )
        {
        //if index is not valid then we don't need make any actions to items
        return EFalse;
        }

    // return value of launched dialogs
    TInt returnVal( 0 );

    TInt indexShift( 0 );
    if ( !iEditMode && index >= ESetGroupId )
        {
        ++indexShift; // if creating new group, one setting item is missing
        }

    TInt shiftedIndex( index + indexShift );


    switch ( shiftedIndex )
        {
        case ESetGroupAdminSelected:
            {
            returnVal = LaunchDialogHelperL( R_EDIT_CHAT_ADMIN_POPUP,
                                             R_EDIT_CHAT_ADMIN_SETTING,
                                             iAdminSelected,
                                             &CCAGroupPropertiesDialog::LaunchEditingRightsListDialogL );
            CHAT_DP( D_CHAT_LIT( "IsAdminStatusModified? : %d" ),
                     iIsAdminStatusModified );
            break;
            }

        case ESetGroupIsOpen:
            {
            returnVal = LaunchDialogHelperL( R_EDIT_CHAT_ACCESS_POPUP,
                                             R_EDIT_CHAT_ACCESS_SETTING,
                                             ( TInt& ) iIsOpen,
                                             &CCAGroupPropertiesDialog::LaunchAccessListDialogL,
                                             TEnumsPC::EImpsPropYes );
            CHAT_DP( D_CHAT_LIT( "IsOpen? : %d" ), iIsOpen );
            break;
            }

        case ESetGroupBannedUsers:
            {
            returnVal = LaunchDialogHelperL( R_EDIT_CHAT_BAN_POPUP,
                                             R_EDIT_CHAT_BAN_SETTING,
                                             iIsBannedUsers,
                                             &CCAGroupPropertiesDialog::LaunchBannedUsersListDialogL );
            CHAT_DP( D_CHAT_LIT( "IsBannedUsers? : %d" ), iIsBannedUsers );
            break;
            }
        default:
            {
            // no dialog to launch
            return EFalse;
            }
        }
    // don't update the view if dialog was exited abnormally
    if ( returnVal != EAknSoftkeyCancel )
        {
        // update all settings items
        TInt count( iSettingItemArray->Count() );

        for ( TInt ii( 0 ); ii < count; ++ii )
            {
            if ( ii != 1 || !iEditMode )
                {
                // in edit mode the second item cannot be edited,
                // so don't save it
                iSettingItemArray->At( ii )->LoadL();
                iSettingItemArray->At( ii )->UpdateListBoxTextL();
                }
            }

        iListbox->DrawItem( index );

        // restore the title pane text correctly even when Back was pressed
        iTitlePane.ClearNaviPaneL();
        iTitlePane.SetTitleL( *iGroupNameBuf );

        if ( AreItemsEdited() )
            {
            iIsEdited = ETrue;
            HandleCBAChangeL();
            }
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::LaunchCorrectDialogL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPropertiesDialog::LaunchCorrectDialog()
    {
    if ( !iDialogLaunchCallback->IsActive() )
        {
        iDialogLaunchCallback->Start( TCallBack( LaunchDlgCallBack, this ) );
        }
    }

// ---------------------------------------------------------
// CCAGroupPropertiesDialog::LaunchDlgCallBack()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupPropertiesDialog::LaunchDlgCallBack( TAny* aDlg )
    {
    CCAGroupPropertiesDialog* dlg =
        static_cast<CCAGroupPropertiesDialog*>( aDlg );
    //Not using iAppUi, coz it gives compilation error in using a non-static
    //variable in a static method
    CCAAppUi *appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->ReportPendingDialog();
    TRAP_IGNORE( dlg->DoLaunchCorrectDialogL() )
    appUi->ReportPendingDialogClosed();
    return EFalse;
    }


/**
 * Help Key Support
 * This function is called by the Framework to get the context to launch
 * Help
 *
 * @param aContext	The context that contains the appid and the help id.
 */
void CCAGroupPropertiesDialog::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_CREATECHAT;
    }


// End of file
