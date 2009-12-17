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
* Description:  Access list dialog. This dialog displays accessed persons.
 *
*/


// INCLUDE FILES
#include	"CCAAccessListDialog.h"
#include	"chatngclient.hrh"
#include	"CCAAppUi.h"
#include	"CCAAccessArray.h"
#include	"CCAStatusPaneHandler.h"
#include    "ChatDefinitions.h"
#include    "CAExternalInterface.h"
#include    "ChatDebugPrint.h"
#include    "IMUtils.h"
#include    "IMDialogUtils.h"
#include    "CAUtils.h"
#include    "CCAApp.h"
#include    "CCAVariantFactory.h"
#include    "CCAContactSelectionDialog.h"
#include    "MCASkinVariant.h"
#include    "MCAMainViewArrayPC.h"
#include    "MCASettingsPC.h"
#include    "MCAProcessManager.h"
#include    "MCAGroupPropertiesPC.h"


#include	<aknlists.h>
#include	<Chatng.rsg>
#include	<aknpopup.h>
#include	<AknMessageQueryDialog.h>
#include	<stringloader.h>
#include	<eikmenub.h>
#include    <hlplch.h>
#include    <csxhelp/imng.hlp.hrh>
#include    <featmgr.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// local variation
#include    "IMPrivateCRKeys.h"
#include    "IMVariant.hrh"

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CCAAccessListDialog::CCAAccessListDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
//
CCAAccessListDialog::CCAAccessListDialog( CCAStatusPaneHandler& aTitlePane,
                                          CDesCArray& aUserList,
                                          TBool& aIsMemberAccessModified,
                                          TBool aFirstItemCanBeEdited,
                                          TBool aRemoveAllShouldExitDialog,
                                          TInt aTitleResource,
                                          TListType aListType,
                                          CDesCArray* aFilterList,
                                          CDesCArray* aFilterList2 )
        :   iTitlePane( &aTitlePane ),
        iUserList( aUserList ),
        iIsMemberAccessModified( aIsMemberAccessModified ),
        iFirstItemCanBeEdited( aFirstItemCanBeEdited ),
        iRemoveAllShouldExitDialog( aRemoveAllShouldExitDialog ),
        iTitleResource( aTitleResource ),
        iFilterList( aFilterList ),
        iFilterList2( aFilterList2 ),
        iListType( aListType ),
        iAppUi( static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() ) ),
        iGroupPropertiesPC( iAppUi->GetProcessManager().GetGroupPropertiesPC() )
    {

    }
// ---------------------------------------------------------
// CCAAccessListDialog::ConstructL()
// destructor
// ---------------------------------------------------------
//
void CCAAccessListDialog::ConstructL( TInt aMenuBarResourceId )
    {
    // other resources depends of aTitleResource
    switch ( iTitleResource )
        {
        case R_CHATCLIENT_EDITRIGHT_TITLE:
            {
            iMenuBarResourceId = R_CHATCLIENT_EDITINGRIGHTS_MENUBAR_POPUP_MENU;
            iDeletePromptResourceId = R_DELETE_EDITINGRIGHTS_PROMPT;
            iDeleteAllPromptResourceId = R_DELETE_ALL_EDITINGRIGHTS_PROMPT;
            iHelpTopic = &KIMNG_HLP_GROUP_EDIT;
            // not used, as the user's WVID is always displayed
            iEmptyText = NULL;
            break;
            }
        case R_CHATCLIENT_BANLIST_TITLE:
            {
            iMenuBarResourceId = R_CHATCLIENT_BANNEDLIST_MENUBAR_POPUP_MENU;
            iDeletePromptResourceId = R_DELETE_BANNED_PROMPT;
            iDeleteAllPromptResourceId = R_DELETE_ALL_BANNED_PROMPT;
            iHelpTopic = &KIMNG_HLP_GROUP_BAN;
            iEmptyText = IMUtils::CombineStringFromResourceLC( R_QTN_CHAT_EMPTY_BANNED_LIST_PRIMARY, R_QTN_CHAT_EMPTY_BANNED_LIST );
            CleanupStack::Pop( iEmptyText );
            break;
            }
        default: // currently same as R_CHATCLIENT_ACLIST_TITLE
            {
            iMenuBarResourceId = R_CHATCLIENT_ACCESSLIST_MENUBAR_POPUP_MENU;
            iDeletePromptResourceId = R_DELETE_ACCESS_PROMPT;
            iDeleteAllPromptResourceId = R_DELETE_ALL_ACCESS_PROMPT;
            iHelpTopic = &KIMNG_HLP_ACCLIST ;
            iEmptyText = IMUtils::CombineStringFromResourceLC( R_QTN_CHAT_EMPTY_ACCESS_LIST_PRIMARY, R_QTN_CHAT_EMPTY_ACCESS_LIST );
            CleanupStack::Pop( iEmptyText );
            break;
            }
        }

    // read it here and store it, faster than re-reading every time in
    // DisplayManualDataQueryL

    iTextualInputMode = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_USERID_TEXTUAL_INPUTMODE );

    CAknDialog::ConstructL( aMenuBarResourceId );
    }

// ---------------------------------------------------------
// CCAAccessListDialog::~CCAAccessListDialog()
// destructor
// ---------------------------------------------------------
//
CCAAccessListDialog::~CCAAccessListDialog()
    {
    // no need to mess with the tab group, since this dialog is always invoked
    // via group properties dialog.  otherwise the tab group flashes once if we
    // prohibit it in constructor and restore in destructor.

    delete iWVUserId;
    if ( iPopupMenu )
        {
        iEikonEnv->EikAppUi()->RemoveFromStack( iPopupMenu );
        delete iPopupMenu;
        }
    delete iItemArray;

    delete iEmptyText;
    }

// ---------------------------------------------------------
// CCAAccessListDialog::DisplayManualDataQueryL()
// This method launches dataquery
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::DisplayManualDataQueryL()
    {
    CAknSingleStyleListBox* listBox = static_cast< CAknSingleStyleListBox* >
                                      ( Control( EChatClientControlIdAccessList ) );

    HBufC* prompt = iEikonEnv->AllocReadResourceLC(
                        RSC_CHAT_ACCESSLIST_PROMPT );
    TPtr wvuserid( iWVUserId->Des() );

    CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL( wvuserid,
                                                          CAknQueryDialog::ENoTone );

    dlg->SetPredictiveTextInputPermitted( ETrue ); // T9
    TInt result( dlg->ExecuteLD(
                     iTextualInputMode ?
                     R_CHATCLIENT_ACCESSLIST_MANUAL_QUERY :
                     R_CHATCLIENT_ACCESSLIST_MANUAL_QUERY2,
                     *prompt ) );

    //Codescanner warning ignored
    CleanupStack::PopAndDestroy(); //prompt

    if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes )  )
        {
        if ( CanUserBeAddedToListL( iUserList, *iWVUserId ) )
            {
            iItemArray->AppendL( *iWVUserId );
            iIsMemberAccessModified = ETrue;
            }
        listBox->HandleItemAdditionL();
        }
    }

// ---------------------------------------------------------
// CCAAccessListDialog::DisplayPopupFriendsListL()
// This method launches popup list full of friends
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::DisplayPopupFriendsListL()
    {
    MCAMainViewArrayPC& mainViewArrayPC = *( iAppUi->GetProcessManager().GetArrayInterface() );
    MCASettingsPC& settingsPC = *iAppUi->GetProcessManager().GetSettingsInterface();


    CDesCArray* selectedContacts = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selectedContacts );

    MCASkinVariant* skinVar = static_cast<CCAApp*>( iAppUi->Application() )
                              ->VariantFactory()->SkinVariantL();


    // show selection dialog
    TBool contacts( CCAContactSelectionDialog::ShowDialogL(
                        *selectedContacts,
                        mainViewArrayPC,
                        *skinVar,
                        settingsPC,
                        iAppUi->MbmFullPath(),
                        TEnumsPC::EMultiSelect,
                        R_CONTACT_SELECTION_DIALOG,
                        *iAppUi->CAStatusPane(),
                        NULL,
                        TEnumsPC::EFilterNotDefined,
                        TEnumsPC::EFilterNotDefined,
                        ETrue ) );
    if ( !contacts )
        {
        // no contacts. Note: code scanner warning is ignored
        CleanupStack::PopAndDestroy(); // selectedContacts.Close()
        return;
        }

    // handle selected contacts (if any)
    TInt count( selectedContacts->MdcaCount() );
    CHAT_DP( D_CHAT_LIT( "selection count %d" ), count );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC wvid( selectedContacts->MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "..adding %S to access list" ), &wvid );

        if ( CanUserBeAddedToListL( iUserList, wvid ) )
            {
            iItemArray->AppendL( wvid );
            iIsMemberAccessModified = ETrue;
            }
        }

    if ( iIsMemberAccessModified )
        {
        CAknSingleStyleListBox* listBox = static_cast< CAknSingleStyleListBox* >
                                          ( Control( EChatClientControlIdAccessList ) );
        listBox->HandleItemAdditionL();
        }
    //Codescanner warning ignored
    CleanupStack::PopAndDestroy(); // selectedContacts.Close()
    }

// ---------------------------------------------------------
// CCAAccessListDialog::ProcessCommandL( TInt aCommand )
// This method catches commands
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::ProcessCommandL( TInt aCommand )
    {
    HideMenu();
    if ( iPopupMenu->IsDisplayed() )
        {
        iPopupMenu->StopDisplayingMenuBar();
        }

    switch ( aCommand )
        {
        case EChatClientAccessListManual:
            {
            DisplayManualDataQueryL();
            break;
            }
        case EChatClientAccessListFriend:
            {
            DisplayPopupFriendsListL();
            break;
            }
        case EChatClientAccessListRemove:
            {
            DisplayRmvConfirmationL();
            break;
            }
        case EChatClientAccessListRemoveAll:
            {
            DisplayRmvAllConfirmationL();
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
// CCAAccessListDialog::DynInitMenuPaneL
// Dynamically initialises menu pane
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::DynInitMenuPaneL( TInt aResourceId,
                                            CEikMenuPane* aMenuPane )
    {
    CHAT_DP_TXT( "CCAAccessListDialog::DynInitMenuPaneL" );

    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dimm the help menu item
        aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
        }

    if ( aResourceId == R_CHATCLIENT_ACCESSLIST_OPTIONS_MENU ||
         aResourceId == R_CHATCLIENT_EDITINGRIGHTS_OPTIONS_MENU ||
         aResourceId == R_CHATCLIENT_BANNEDLIST_OPTIONS_MENU ||
         aResourceId == R_CHATCLIENT_ACCESSLIST_POPUP_MENU ||
         aResourceId == R_CHATCLIENT_EDITINGRIGHTS_POPUP_MENU ||
         aResourceId == R_CHATCLIENT_BANNEDLIST_POPUP_MENU )
        {
        CAknSingleStyleListBox* listBox =
            static_cast< CAknSingleStyleListBox* >(
                Control( EChatClientControlIdAccessList ) );
        TInt index( listBox->CurrentItemIndex() );
        TInt emptyCount( iFirstItemCanBeEdited ? 0 : 1 );
        if ( iItemArray->MdcaCount() == emptyCount )
            {
            aMenuPane->SetItemDimmed( EChatClientAccessListRemove, ETrue );

            if ( ( aResourceId != R_CHATCLIENT_ACCESSLIST_POPUP_MENU ) &&
                 ( aResourceId != R_CHATCLIENT_EDITINGRIGHTS_POPUP_MENU ) &&
                 ( aResourceId != R_CHATCLIENT_BANNEDLIST_POPUP_MENU ) )
                {
                // popup menu doesn't contain "remove all"-option
                aMenuPane->SetItemDimmed( EChatClientAccessListRemoveAll,
                                          ETrue );
                }
            }
        else if ( index == 0 && !iFirstItemCanBeEdited )
            {
            aMenuPane->SetItemDimmed( EChatClientAccessListRemove, ETrue );
            }
        }
    CHAT_DP_TXT( "CCAAccessListDialog::DynInitMenuPaneL ENDS" );
    }


// ---------------------------------------------------------
// CCAAccessListDialog::HandleListBoxEventL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                               TListBoxEvent aEventType )
    {
    if ( aEventType == EEventItemDoubleClicked )
        {
        DisplayPopupMenuL();
        }
    }

// ---------------------------------------------------------
// CCAAccessListDialog::PreLayoutDynInitL()
// Sets the layout for dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::PreLayoutDynInitL()
    {
    iPopupMenu = new( ELeave ) CEikMenuBar;
    iPopupMenu->ConstructL( this, 0, iMenuBarResourceId );
    iEikonEnv->EikAppUi()->AddToStackL( iPopupMenu );

    // Context sensitive menu
#ifndef RD_30_DISABLE_TOUCH
    iMenuBar->SetContextMenuTitleResourceId( iMenuBarResourceId );
#endif

    iWVUserId = HBufC::NewL( KMaxWVIDLength );
    CAknSingleStyleListBox* listBox = static_cast< CAknSingleStyleListBox* >
                                      ( Control( EChatClientControlIdAccessList ) );

    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    listBox->HandleItemAdditionL();
    listBox->UpdateScrollBarsL();
    listBox->SetListBoxObserver( this );

    // If simplified flag is set -> then show descriptive empty list text
    if ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey ) & EIMFeatSimplifiedUI &&
         iEmptyText )
        {
        // we should set the empty text
        listBox->View()->SetListEmptyTextL( *iEmptyText  );

        }

    iItemArray = CCAAccessArray::NewL(
                     iGroupPropertiesPC,
                     iUserList );

    listBox->Model()->SetItemTextArray( iItemArray );
    listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    iTitlePane->ClearNaviPaneL();

    iTitlePane->SetTitleL( iEikonEnv, iTitleResource );
    }

// ---------------------------------------------------------
// CCAAccessListDialog::OkToExitL( TInt aButtonId )
// Handles buttonids
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAccessListDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        return EFalse;
        }
    if ( aButtonId == EAknSoftkeyContextOptions )
        {
        DisplayPopupMenuL();    // MSK command, show context sensitive
        // options menu
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CCAAccessListDialog::OfferKeyEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCAAccessListDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                  TEventCode aType )
    {
    CAknSingleStyleListBox* listBox = static_cast< CAknSingleStyleListBox* >
                                      ( Control( EChatClientControlIdAccessList ) );

    if ( aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK )
        {
        DisplayPopupMenuL();
        return EKeyWasConsumed;
        }
    if ( aKeyEvent.iCode == EKeyBackspace )
        {
        CAknSingleStyleListBox* listBox =
            static_cast< CAknSingleStyleListBox* >(
                Control( EChatClientControlIdAccessList ) );
        TInt index( listBox->CurrentItemIndex() );

        if ( ( index == 0 && iFirstItemCanBeEdited ) || index > 0 )
            {
            DisplayRmvConfirmationL();
            return EKeyWasConsumed;
            }
        }
    if ( aKeyEvent.iCode == EKeyEscape ) // handle dialog shutdown.
        {
        TryExitL( EAknSoftkeyCancel );
        return EKeyWasConsumed;
        }
    return listBox->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CCAAccessListDialog::MappedCommandId()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAAccessListDialog::MappedCommandId( TInt aButtonId )
    {
    if ( aButtonId == EEikBidCancel )
        {
        // return something else than cancel.
        // the returned value is irrelevant
        return EEikBidCancel + 1;
        }

    return aButtonId;
    }

// ---------------------------------------------------------
// CCAAccessListDialog::DisplayRmvConfirmationL()
// Displays confirmation note
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::DisplayRmvConfirmationL()
    {
    CAknSingleStyleListBox* listBox =
        static_cast< CAknSingleStyleListBox* >(
            Control( EChatClientControlIdAccessList ) );

    TInt index( listBox->CurrentItemIndex() );
    TPtrC wvid( iItemArray->RawData( index ) );


    HBufC* prompt = StringLoader::LoadLC( iDeletePromptResourceId,
                                          iGroupPropertiesPC->GetIdentificationL( wvid ) );
    // Show confirmation
    TInt ret( IMDialogUtils::DisplayQueryDialogL( R_DELETE_ACCESS_QUERY,
                                                  *prompt ) );
    CleanupStack::PopAndDestroy( prompt );

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        iIsMemberAccessModified = ETrue;
        iItemArray->Delete( index, 1 );
        listBox->HandleItemRemovalL();

        TInt currentItemIndex( index );
        if ( currentItemIndex < 0 )
            {
            currentItemIndex = 0;
            }
        if ( currentItemIndex >= iItemArray->MdcaCount() )
            {
            currentItemIndex = iItemArray->MdcaCount() - 1;
            }
        if ( iItemArray->MdcaCount() > 0 )
            {
            listBox->SetCurrentItemIndexAndDraw( currentItemIndex );
            }
        }
    DrawNow();
    }

// ---------------------------------------------------------
// CCAAccessListDialog::DisplayRmvAllConfirmationL()
// Displays confirmation if all are removed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::DisplayRmvAllConfirmationL()
    {
    CAknSingleStyleListBox* listBox =
        static_cast< CAknSingleStyleListBox* >(
            Control( EChatClientControlIdAccessList ) );

    HBufC* prompt = iEikonEnv->AllocReadResourceLC(
                        iDeleteAllPromptResourceId );
    TInt ret( IMDialogUtils::DisplayQueryDialogL( R_DELETE_ALL_ACCESS_QUERY,
                                                  *prompt ) );
    CleanupStack::PopAndDestroy( prompt );

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        iIsMemberAccessModified = ETrue;
        if ( iFirstItemCanBeEdited )
            {
            iItemArray->Reset();
            }
        else
            {

            if ( iItemArray->MdcaCount() > 0 ) //added array bound check
                {
                // don't delete first item
                iItemArray->Delete( 1, iItemArray->MdcaCount() - 1 );
                }
            listBox->SetCurrentItemIndex( 0 );
            }

        listBox->HandleItemRemovalL();

        if ( iRemoveAllShouldExitDialog )
            {
            // exit the dialog
            TryExitL( EAknSoftkeyOk );
            }
        }
    }

// ---------------------------------------------------------
// CCAAccessListDialog::DisplayPopupMenuL
// Displays pop-up menu
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessListDialog::DisplayPopupMenuL()
    {
#ifndef RD_30_DISABLE_TOUCH
    iMenuBar->TryDisplayContextMenuBarL();
#else
    iPopupMenu->TryDisplayMenuBarL();
#endif //RD_30_DISABLE_TOUCH
    }

// ---------------------------------------------------------
// CCAAccessListDialog::CanUserBeAddedToListL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAAccessListDialog::CanUserBeAddedToListL( const CDesCArray& aUserList,
                                                  const TDesC& aUserId )
    {
    // Check first that id does not exists in filter lists.
    TInt index( IMUtils::NeutralFind( *iFilterList, aUserId ) );
    TInt index2 = KErrNotFound;
    if ( iFilterList2 )
        {
        index2 = IMUtils::NeutralFind( *iFilterList2, aUserId );
        }

    if ( index != KErrNotFound || index2 != KErrNotFound )
        {
        TInt resource;
        switch ( iListType )
            {
            case EAccessList:
                {
                resource = R_QTN_CHAT_ACLIST_MOVE_TO_ACCESS;
                break;
                }
            case EBannedList:
                {
                resource = R_QTN_CHAT_ACLIST_MOVE_TO_BANNED;
                break;
                }
            default:
                {
                resource = R_QTN_CHAT_ACLIST_MOVE_TO_ADMIN;
                break;
                }
            }
        // ask from user should be moved from other list.
        HBufC* text = StringLoader::LoadLC( resource,
                                            iGroupPropertiesPC->GetIdentificationL( aUserId ) );
        TInt okToMove( IMDialogUtils::DisplayYesNoConfirmationDialogL( *text ) );
        CleanupStack::PopAndDestroy( text );
        if ( okToMove )
            {
            // Delete from correct filter list
            if ( index != KErrNotFound )
                {
                iFilterList->Delete( index );
                }
            if ( index2 != KErrNotFound )
                {
                iFilterList2->Delete( index2 );
                }
            }
        else
            {
            return EFalse;
            }
        }
    return ( IMUtils::NeutralFind( aUserList, aUserId ) == KErrNotFound );
    }


/**
 * Help Key Support
 * This function is called by the Framework to get the context to launch
 * Help
 *
 * @param aContext	The context that contains the appid and the help id.
 */
void CCAAccessListDialog::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = *iHelpTopic;
    }
//  End of File
