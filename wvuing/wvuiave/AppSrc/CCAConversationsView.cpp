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
* Description:  view class for conversation view
*
*/


// INCLUDE FILES
#include	"chatngclient.hrh"
#include	"CCAConversationsView.h"
#include	"CCAConversationsContainer.h"
#include    "CCAUISessionManager.h"
#include	"CCAStatusPaneHandler.h"
#include	"CCAAppUi.h"
#include	"CAExternalInterface.h"
#include	"CCAMessageEditor.h"
#include	"ChatDebugPrint.h"
#include    "ChatDefinitions.h"
#include    "CAExternalInterface.h"
#include    "IMNoteMapper.h"
#include    "IMDialogUtils.h"
#include	"ImpsCSPAllErrors.h"
#include    "CCAMessageExtensionsHandler.h"
#include    "CCAFadeControl.h"
#include    "MCAIndicatorTerminator.h"
#include    "MCAMessageExtension.h"
#include    "IMUtils.h"
#include    "CCAUiMessageUtils.h"
#include    "CAUtils.h"
#include    "CCAPCUtils.h"
#include	"CCAApp.h"
#include    "CCAContactSelectionDialog.h"
#include	"CCAVariantFactory.h"

#include    "MCAConversationMessage.h"
#include    "MCAMessageCreator.h"
#include    "MCASettingsPC.h"
#include    "mcamessagecontainerinfo.h"
#include    "IMPRivateCRKeys.h"
#include    "IMVariant.hrh"

#include	<eikbtgpc.h>
#include	<eikgted.h> //For the CEikGlobalTextEditor
#include	<eikmenup.h>
#include	<eikmenub.h>
#include	<aknpopup.h>
#include	<aknnotewrappers.h>
#include    <stringloader.h>
#include    <aknenv.h>
#include 	<aknlists.h>
#include	<flogger.h>
#include	<utf.h>
#include 	<txtrich.h>
#include	<aknquerydialog.h>
#include	<chatNG.rsg>
#include	<CAVariationNG.rsg>
#include    <NewFileService.hrh> // TNewServiceFileType

#include	<bldvariant.hrh>
#include    <hlplch.h>
#include    <FeatMgr.h>

#include    <finditem.hrh>
#include    <finditemmenu.h>

#include    <commonuiinternalcrkeys.h>

#include 	"MCAConversationPC.h"
#include	"MCAProcessManager.h"
#include	"MCAServerContactsArrayPC.h"
#include    "CCAGroupUtils.h"
#include    "MCALoginPC.h"
#include    "CCaSyncChecker.h"
#include 	"MCAMainViewArrayPC.h"
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
const TInt KMaxTitleLength = 32;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAConversationsView::CCAConversationsView()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAConversationsView::CCAConversationsView()
        : iCleanSender( NULL, 0 ),
        iLastUnreadIndex( KErrNotFound ),
        iForeground( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CCAConversationsView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAConversationsView::ConstructL( TInt aAvkonViewResourceId, TUid aViewId,
                                       CCAMessageExtensionsHandler& aMessageHandler )
    {
    BaseConstructL( aAvkonViewResourceId, aViewId );

    iMessageHandler = &aMessageHandler;

    iFindContextMenu = new( ELeave ) CEikMenuBar;
    iFindContextMenu->ConstructL( this, 0, R_CHATCLIENT_MENUBAR_FINDITEMUI );

    iAddAndReplyMenu = new( ELeave ) CEikMenuBar;
    iAddAndReplyMenu->ConstructL(
        this,
        0,
        R_CHATCLIENT_MENUBAR_ADDANDREPLY );

    iContactId = HBufC::NewL( KMaxChatGroupLenght );
    iListId = HBufC::NewL( KMaxChatGroupLenght );
    iFindMenu = CFindItemMenu::NewL( EChatClientFindMenuPlaceHolder );
    iFindMenu->AttachItemFinderMenuL( R_CHATCLIENT_IBOX_VIEW_MENU );
    // automatic finder on by default
    iFindMenu->HandleItemFinderCommandL( EFindItemCmdEnableAutoFind );
    iItemFinder = CItemFinder::NewL();

    iAppUI = static_cast <CCAAppUi*> ( CCoeEnv::Static()->AppUi() );

    iConversationPC = iAppUI->GetProcessManager().GetConversationInterface();

    iTitlePane = iAppUI->CAStatusPane();
    }

// Destructor
CCAConversationsView::~CCAConversationsView()
    {



    if ( iContainer && iAppUI )
        {
        iAppUI->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;

    if ( iAddAndReplyMenu && iAppUI )
        {
        iAppUI->RemoveFromViewStack( *this, iAddAndReplyMenu );
        }

    if ( iFindContextMenu && iAppUI )
        {
        iAppUI->RemoveFromStack( iFindContextMenu );
        }

    delete iFindContextMenu;
    delete iAddAndReplyMenu;
    delete iContactId;
    if ( iListId )
        {
        delete iListId;
        }
    delete iTitle;
    delete iSender;
    delete iFindMenu;
    delete iItemFinder;
    delete iEditorContent;
    }


// -----------------------------------------------------------------------------
// CCAConversationsView::HandleCbaChangeL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAConversationsView::HandleCbaChangeL( TInt aResourceId )
    {
    Cba()->SetCommandSetL( aResourceId );
    Cba()->DrawNow();
    }

// ---------------------------------------------------------
// CCAConversationsView::SetLastUnreadIndex()
// Sets last unread message index
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::SetLastUnreadIndex(
    const TInt aLastUnreadIndex /* = -1  */ )
    {
    iLastUnreadIndex = aLastUnreadIndex;
    }


// ---------------------------------------------------------
// CCAConversationsView::CloseConversationL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::CloseConversationL()
    {

    MCAMainViewArrayPC* mainViewArrayPC = iAppUI->GetProcessManager().GetArrayInterface();

    if ( iTitlePane )
        {
        HBufC* identification = iTitlePane->GetTabItemIdentification( iTabId ).AllocLC();


        HBufC* prompt = StringLoader::LoadLC( R_CLOSE_CONVERSATION_PROMPT,
                                              *identification );


        CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog(
            CAknQueryDialog::ENoTone );

        CleanupStack::PushL( dlg );

        dlg->SetPromptL( *prompt );

        CleanupStack::Pop( dlg );

        TInt ret( dlg->ExecuteLD( R_CLOSE_COVERSATION_QUERY ) );

        CleanupStack::PopAndDestroy( 2, identification ); // prompt, identification


        if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
            {
            // hide button group to prevent back button using during close
            iCba = CEikButtonGroupContainer::Current();
            iCba->MakeVisible( EFalse );
            iCba->DrawNow();

            iConversationPC->DeleteChatL( KNullDesC,
                                          KNullDesC,
                                          *iContactId );

            //To remove the item from open chats array when conv is closed
            mainViewArrayPC->RemoveConversationItem( *iContactId );

            // Deregister conversation from switch back views
            iAppUI->DeRegisterSwitchBack( KUidConversationsView,
                                          *iContactId );
            if ( iTitlePane )
                {
                //iTitlePane->DeleteTab(iTabId);
                iTitlePane->SynchroniseAndRemoveTabGroup();
                }

            iAppUI->SwitchViewBackL( KUidFriendsListView );
            //iAppUI->SwitchViewL( KUidFriendsListView );

            // XXX before or after SwitchView
            iCba->MakeVisible( ETrue );
            iCba->DrawNow();
            }
        }
    }

// ---------------------------------------------------------
// CCAConversationsView::DynInitMenuPaneL
// Called when menupane is about to be shown.
// ---------------------------------------------------------
//
void CCAConversationsView::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    TInt Temp = aMenuPane->NumberOfItemsInPane();
    TBool showFwdToContact = iContainer->FwdContactVisible();
    TBool showFwdToGroup   = iContainer->FwdGroupVisible();

    if ( ! iAppUI->UISessionManager().IsSupported( CCAUISessionManager::EGroup ) )
        {
        // groups are not supported, hide forward to group
        showFwdToGroup = EFalse;
        }

    TBool showForwardMenu = showFwdToGroup || showFwdToContact;

    // if we're displaying general menu and help feature is not supported..
    switch ( aResourceId )
        {

            //added for edit text, writing language options
            /*  case R_CHATCLIENT_TEXTEDIT_MENU:
              	{

              	TBool editorFocus = iContainer->Editor().IsFocused();

              	aMenuPane->SetItemDimmed( EChatClientEditText, !editorFocus );
              	aMenuPane->SetItemDimmed( EAknCmdInputLanguage, !editorFocus );
              	break;

              	}*/

        case R_CHATCLIENT_GENERAL_MENU:
            {
            aMenuPane->SetItemDimmed( EChatClientCmdHelp,
                                      !FeatureManager::FeatureSupported( KFeatureIdHelp ) );
            break;
            }
        case R_CHATCLIENT_ADDANDREPLY_MENU:
            {
            aMenuPane->SetItemDimmed( EChatClientIboxReply, ETrue );
            if ( iConversationPC->FindAnyContact( *iContactId ) )
                {
                aMenuPane->SetItemDimmed( EChatClientSearchAddFriends, ETrue );
                }
            break;
            }
        case R_CHATCLIENT_IBOX_VIEW_MENU:
            {
            TBool text( iContainer->Editor().TextLength() != 0 );
            TBool openable = EFalse;
            TBool savePossible( EFalse );
            TBool object = iContainer->IsObjectSelected( openable, savePossible );

            //	IM client UI customization, phase 2
            //  Based on Variation flag, Show Templates
            TBool showTemplates = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_TEMPLATES );
            // Convention does not allow comparison to NULL, using ugly if/else
            TBool contactInList;

            if ( iConversationPC->FindAnyContact( *iContactId ) )
                {
                contactInList = ETrue;
                }
            else
                {
                contactInList = EFalse;
                }

            // "send"
            aMenuPane->SetItemDimmed( EChatClientIboxViewSend, !text );

            // "insert smiley"
            if ( !iContainer->Editor().IsFocused() )
                {
                aMenuPane->SetItemDimmed(
                    EChatClientChatViewInsertSmiley, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EChatClientChatViewInsertSmiley,
                                          !iContainer->ExtensionFits( KChatSmileyLength ) );
                }

            // "Templates"
            aMenuPane->SetItemDimmed( EChatClientIboxInsertTemplate, !showTemplates );

            // "open"
            aMenuPane->SetItemDimmed( EChatClientOpenObject, !openable );

            // "save"
            aMenuPane->SetItemDimmed( EChatClientSaveObject, !savePossible );

            // item finder menu
            aMenuPane->SetItemDimmed( EChatClientFindMenuPlaceHolder, ETrue );
            if ( iContainer->IsStopped() )
                {
                UpdateItemTypeL();
                iFindMenu->AddItemFindMenuL( iItemFinder, aMenuPane,
                                             EChatClientFindMenuPlaceHolder,
                                             KNullDesC ); // we can ignore the sender here
                }

            // "send image"
            if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEND_IMAGE ) == 0 )
                {
                aMenuPane->SetItemDimmed( EChatClientSendImage, ETrue );
                aMenuPane->SetItemDimmed( EChatClientSendImageCascade, ETrue );
                }
            else
                {
                TBool cam( FeatureManager::FeatureSupported(
                               KFeatureIdCamera ) );
                CHAT_DP( D_CHAT_LIT( " Camera %d" ), cam );

                // if camera is supported, show cascade menu
                // otherwise hide cascade menu
                aMenuPane->SetItemDimmed( EChatClientSendImage, cam );
                aMenuPane->SetItemDimmed( EChatClientSendImageCascade, !cam );
                }

            // "forward" hiding logic case 3
            // Hide also when focus on object that is not openable
            if ( !showForwardMenu || ( object && !openable ) )
                {
                CHAT_DP_TXT( "CCAConversationsView::DynInitMenuPaneL \
			              Hide the whole forward menu" );
                aMenuPane->SetItemDimmed( EChatClientIboxForward, ETrue );
                }
            else
                {
                CHAT_DP_TXT( "CCAConversationsView::DynInitMenuPaneL \
			              Show the forward menu" );
                aMenuPane->SetItemDimmed(
                    EChatClientIboxForward, !iContainer->IsStopped() );
                }

            // "add to contacts"
            aMenuPane->SetItemDimmed( EChatClientSearchAddFriends,
                                      contactInList );

            // "save to conversations always show"
            aMenuPane->SetItemDimmed( EChatClientIboxRecord, EFalse );

            // "block", check if user block/unblock is supported
            if ( !iAppUI->UISessionManager().IsSupported( CCAUISessionManager::EBlock ) )
                {
                aMenuPane->SetItemDimmed( EChatClientIboxBlockOpt, ETrue );
                }
            //As per new UI spec change own status is not avilable in conversation
            //view menu
            aMenuPane->SetItemDimmed( EChatClientMainViewCmdChangeStatus, ETrue );

            break;
            }
        case R_CHATCLIENT_IBOX_BLOCK_SUBMENU:
            {
            TBool blocked( iContainer->IsBlocked() );
            if ( blocked )
                {
                aMenuPane->SetItemDimmed( EChatClientIboxBlock, ETrue );
                // show/hide "show blocked list" option
                if ( !IMUtils::IntResourceValueL(
                         RSC_CHAT_VARIATION_SHOW_BLOCKED_LIST ) )
                    {
                    aMenuPane->SetItemDimmed( EChatClientIboxBlockShow, ETrue );
                    }
                }
            break;
            }
        case R_CHATCLIENT_IBOX_FORWARD_SUBMENU:
            {
            aMenuPane->SetItemDimmed( EChatClientIboxForwardToGroup,
                                      ! showFwdToGroup );
            aMenuPane->SetItemDimmed( EChatClientIboxForwardToContact,
                                      ! showFwdToContact );
            break;
            }

        case R_CHATCLIENT_FINDITEMUI_MENU:
            {
            if ( iContainer->IsStopped() )
                {
                UpdateItemTypeL();
                iFindMenu->AddItemFindMenuL( iItemFinder, aMenuPane,
                                             EFindItemContextMenuPlaceHolder,
                                             KNullDesC, // we can ignore the sender here
                                             EFalse, ETrue ); // context menu
                }
            break;
            }

        default:
            {
            // update find menu
            iFindMenu->UpdateItemFinderMenuL( aResourceId, aMenuPane );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAConversationsView::UpdateItemTypeL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::UpdateItemTypeL()
    {
    CItemFinder::CFindItemExt& item = iItemFinder->CurrentItemExt();
    switch ( iContainer->SelectedItemType() )
        {
        case CFindItemEngine::EFindItemSearchPhoneNumberBin:
            {
            item.iItemType = CItemFinder::EPhoneNumber;
            break;
            }
        case CFindItemEngine::EFindItemSearchMailAddressBin:
            {
            item.iItemType = CItemFinder::EEmailAddress;
            break;
            }
        case CFindItemEngine::EFindItemSearchURLBin:
            {
            item.iItemType = CItemFinder::EUrlAddress;
            break;
            }
        case CFindItemEngine::EFindItemSearchScheme:
            {
            item.iItemType = CItemFinder::EUriScheme;
            break;
            }
        case KErrNotFound:  // flowthrough
        default:
            {
            item.iItemType = CItemFinder::ENoneSelected;
            break;
            }
        }
    delete item.iItemDescriptor;
    item.iItemDescriptor = NULL;
    item.iItemDescriptor = iContainer->SelectedItemL();  // takes the ownership

    // this logic comes from CItemFinder::ResolveAndSetItemTypeL.
    // should be in ItemFinder engine, but for some reason it isn't,
    // so, next few lines are copypasted from AknItemFinder.cpp..
    if ( item.iItemType == CItemFinder::EUrlAddress )
        {
        // old url types need prefix in order to work w/ schemehandler
        const TDesC& pref = item.iItemDescriptor->Des().Left( 4 );

        //To avoid adding :http://" to rtsp links, we check it before it is done
        //and return.
        if ( pref.CompareF ( KIMRTSP().Left ( 4 ) ) == 0 )
            {
            return;
            }

        if ( pref.CompareF( KIMHTTPPREFIX().Left( 4 ) ) != 0 )
            {
            HBufC* tmp = item.iItemDescriptor->ReAlloc(
                             item.iItemDescriptor->Length() + KIMHTTPPREFIX().Length() );
            if ( tmp )
                {
                // realloc succeeded
                item.iItemDescriptor = tmp;
                item.iItemDescriptor->Des().Insert( 0, KIMHTTPPREFIX );
                }
            }
        }
    if ( item.iItemType == CItemFinder::EUriScheme )
        {
        // some schemes (ie. "old ones") have special handling
        const TDesC& pref = item.iItemDescriptor->Des().Left( 4 );
        if ( pref.CompareF( KIMHTTPPREFIX().Left( 4 ) ) == 0
             || pref.CompareF( KIMRTSP ) == 0 )
            {
            item.iItemType = CItemFinder::EUrlAddress;
            }
        }
    }

// ---------------------------------------------------------
// CCAConversationsView::UpdateTitleL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::UpdateTitleL()
    {
    if ( iTitlePane )
        {

        HBufC* tempTitle = iTitlePane->GetTabItemIdentification( iTabId ).AllocL();

        delete iTitle;
        iTitle = tempTitle;

        iTitlePane->SetTitleL( *iTitle );
        }
    }

// ---------------------------------------------------------
// CCAConversationsView::StoreContent
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::StoreContent( const HBufC* aContent )
    {
    // Delete previous stored content
    delete iEditorContent;
    iEditorContent = NULL;
    iEditorContent = aContent;
    }


// ---------------------------------------------------------
// CCAConversationsView::CheckBlockedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::CheckBlockedL()
    {
    iContainer->CheckBlockedL();
    }

// ---------------------------------------------------------
// CCAConversationsView::HandleCommandL(TInt aCommand)
// Handles commands
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::HandleCommandL( TInt aCommand )
    {
    // This case is used to display the login query in offline mode.
    switch ( aCommand )
        {
        case EChatClientNewImage:
        case EChatClientFromPhotos:
        case EChatClientSendImage:
        case EChatClientIboxBlockEnter:
        case EChatClientIboxBlockShow:
        case EChatClientIboxBlock:
        case EChatClientIboxUnblock:
        case EChatClientIboxForwardToGroup:
        case EChatClientIboxForwardToContact:
        case EChatClientSearchAddFriends:
            {
            if ( !UISessionManager().LoginL( MCAUiLoginCmdCB::EQueryLogin ) )
                {
                return;
                }
            break;
            }
        default:
            {
            break;
            }

        }

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
    switch ( aCommand )
        {

        case EChatClientIboxBlockEnter:
        case EChatClientIboxBlockShow:
        case EChatClientIboxBlock:
        case EChatClientIboxUnblock:
        case EChatClientIboxForwardToGroup:
        case EChatClientIboxForwardToContact:
        case EChatClientSearchAddFriends:
            {
            if ( !CCASyncChecker::CheckSyncStateL() )
                {
                return;
                }
            break;
            }
        default:
            {
            break;
            }

        }
#endif

    switch ( aCommand )
        {
        case EAknSoftkeyOpen:   // Flowthrough, MSK command
        case EChatClientOpenObject:
            {
            // Fix TELYG-7H2A6N
            if ( iObjectOpenedSaved )
                {
                return;
                }

            MCAConversationMessage* msg = iContainer->SelectedMessage();
            if ( msg )
                {
                CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC();
                // Set the indicator to true
                iObjectOpenedSaved = ETrue;
                TRAPD( err, utils->OpenObjectL( *msg ) );
                // Set it back
                iObjectOpenedSaved = EFalse;

                CleanupStack::PopAndDestroy( utils );
                User::LeaveIfError( err );
                }
            break;
            }
        case EChatClientSaveObject:
            {
            // Fix TELYG-7H2A6N
            if ( iObjectOpenedSaved )
                {
                return;
                }

            MCAConversationMessage* msg = iContainer->SelectedMessage();
            if ( msg )
                {
                CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC();

                // Set the indicator to true
                iObjectOpenedSaved = ETrue;
                TRAPD( err, utils->SaveObjectL( *msg ) );
                // Set it back
                iObjectOpenedSaved = EFalse;

                CleanupStack::PopAndDestroy( utils );
                User::LeaveIfError( err );
                }
            break;
            }
        case EChatClientNewImage:
            {
            // new file
            if ( iAppUI->UISessionManager().IsLoggedIn() )
                {
                iContainer->CheckReplyL();

                CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC( this );
                utils->SendNewFileL( ENewFileServiceImage,
                                     *iConversationPC );
                CleanupStack::PopAndDestroy( utils );

                }
            break;
            }
        case EChatClientFromPhotos: // flowthrough
        case EChatClientSendImage:
            {
            if ( iAppUI->UISessionManager().IsLoggedIn() )
                {
                iContainer->CheckReplyL();
                CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC( this );

                iCba = CEikButtonGroupContainer::Current();

                if ( iCba )
                    {
                    iCba->MakeCommandVisible( EAknSoftkeyOptions, EFalse );
                    iCba->MakeCommandVisible( EAknSoftkeyBack, EFalse );
                	iCba->MakeCommandVisible( EChatClientChatViewContinue, EFalse );
                    iCba->DrawNow();
                    }

                utils->SendImageL( *iConversationPC );
                iCba = CEikButtonGroupContainer::Current();

                if ( iCba )
                    {
                    iCba->MakeCommandVisible( EAknSoftkeyOptions, ETrue );
                    iCba->MakeCommandVisible( EAknSoftkeyBack, ETrue );
                	iCba->MakeCommandVisible( EChatClientChatViewContinue, ETrue );
                    iCba->DrawNow();
                    }

                CleanupStack::PopAndDestroy( utils );
                }
            break;
            }

        // flow through
        case EChatClientCmdBack:
        case EAknSoftkeyBack:
            {
            if ( iTitlePane )
                {
                iTitlePane->SynchroniseAndRemoveTabGroup();
                }
            if ( TEnumsPC::ERegister == iAppUI->RetForwardTo() )
                {
                iAppUI->SetResetForwardTo( TEnumsPC::EToBeUnregistered );

                iAppUI->SwitchViewL( KUidFriendsListView );
                }
            else
                {
                if ( TEnumsPC::EAlreadyRegistered == iAppUI->RetForwardTo() )
                    {
                    // Deregister conversation from switch back views
                    iAppUI->UnRegisterPreviousview();
                    iAppUI->SetResetForwardTo( TEnumsPC::EUnregistered );
                    }
                TCADnlConvInvGroupFocus dnlConvInvGroupFocus;
                dnlConvInvGroupFocus.iInvConvGroupID.Copy( iContactId->Des().Left( KMaxWVIDLength ) );
                dnlConvInvGroupFocus.iListID.Copy( iListId->Des().Left( KMaxWVIDLength ) );
                dnlConvInvGroupFocus.iType = EConversationItem;
                TCADnlConvInvGroupFocusBuf convInvGroupFocusBuf( dnlConvInvGroupFocus );
                iAppUI->SwitchViewBackL( KUidFriendsListView, KConvInvGroupFocusId, convInvGroupFocusBuf );
                }
            break;
            }
        // flow through
        case EAknSoftkeySend:   // MSK command
        case EChatClientIboxViewSend:
        case EChatClientChatViewCmdSend:
            {
            iContainer->SendMessageL();
            break;
            }

        case EChatClientChatViewContinue:
            {
            iContainer->StartScrollingL();
            iAppUI->SetFocusFlag( ETrue );
            break;
            }
        case EChatClientSearchAddFriends:
            {
            iContainer->AddToFriendsL();
            break;
            }

        case EChatClientChatViewInsertSmiley:
            {
            TBool dialogCancelled = EFalse;
            iMessageHandler->InsertExtensionL( iContainer->Editor(),
                                               EMessageExtensionSmiley,
                                               dialogCancelled );

            // Resize icons, use reading pane text line element as parent
            iContainer->ResizeIcons( iContainer->TextRect() );

            if ( !dialogCancelled )
                {
                iContainer->RefreshEditorL();
                }
            break;
            }
        case EChatClientIboxInsertTemplate:
            {
            ShowTemplatesL();
            break;
            }
        case EChatClientIboxForwardToGroup:
            {
            ForwardToGroupL();
            break;
            }
        case EChatClientIboxForwardToContact:
            {
            ForwardToContactL();
            break;
            }
        case EChatClientIboxRecord:
            {
            iContainer->RecordChatL();
            break;
            }
        case EChatClientIboxBlockEnter:
            {
            if ( iAppUI->UISessionManager().IsLoggedIn() )
                {
                iContainer->BlockUserWVIdL();
                }
            break;
            }
        case EChatClientIboxBlockShow:
            {
            if ( iAppUI->UISessionManager().IsLoggedIn() )
                {
                iContainer->DisplayBlockedListL();
                }
            break;
            }
        case EChatClientIboxBlock:
            {
            if ( iAppUI->UISessionManager().IsLoggedIn() )
                {
                iContainer->BlockUserL();
                }
            break;
            }
        case EChatClientIboxUnblock:
            {
            if ( iAppUI->UISessionManager().IsLoggedIn() )
                {
                iContainer->UnBlockL();
                }
            break;
            }
        case EChatClientIboxClose:
            {
            CloseConversationL();
            break;
            }

        // Help key support EChatClientCmdHelp is assigned EAknCmdHelp in
        // chatngclient.hrh
        case EChatClientCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                                 iAppUI->AppHelpContextL() );
            break;
            }

        // flow through
        case EFindItemCmdEnableAutoFind:
        case EFindItemCmdDisableAutoFind:
            {
            iContainer->SetItemHighlightL(
                aCommand == EFindItemCmdEnableAutoFind );
            iFindMenu->HandleItemFinderCommandL( aCommand );
            if ( MenuBar()->IsDisplayed() )
                {
                MenuBar()->StopDisplayingMenuBar();
                }
            if ( iFindContextMenu->IsDisplayed() )
                {
                iFindContextMenu->StopDisplayingMenuBar();
                }
            break;
            }
        case EChatClientCall:
            {
            UpdateItemTypeL();
            iFindMenu->HandleCallL( *iItemFinder->CurrentItemExt().iItemDescriptor );
            break;
            }
        case EAknCmdExit:           // fall-through, handled similarily
        case EEikCmdExit:           // fall-through, handled similarily
        case EAknSoftkeyExit:       // fall-through, handled similarily
        case EChatClientCmdExit:
            {

            if ( iFindMenu->CommandIsValidL( aCommand ) )
                {
                iFindMenu->HandleItemFinderCommandL( aCommand );
                }

            iAppUI->HandleCommandL( aCommand );
            break;
            }
        default:
            {
            // check if it is find menu command
            if ( iFindMenu->CommandIsValidL( aCommand ) )
                {
                iFindMenu->HandleItemFinderCommandL( aCommand );
                }
            else
                {
                //As per new UI spec change own status is not avilable in conversation
                //view menu
                iAppUI->HandleCommandL( aCommand );
                }
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAConversationsView::ShowTemplatesL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::ShowTemplatesL()
    {
    CDesCArray* aResultArray = NULL;
    CEikonEnv* env = CEikonEnv::Static();
    aResultArray = env->ReadDesC16ArrayResourceL( R_CHATVIEW_TEMPLATES_LIST );

    // Create CEikTextListBox instance, list
    CEikTextListBox* nTemplateList = new( ELeave ) CAknSinglePopupMenuStyleListBox;

    // Push nProfileList pointer to CleanupStack.
    CleanupStack::PushL( nTemplateList );

    // Create CAknPopupList instance, popupList
    CAknPopupList* popupList = CAknPopupList::NewL( nTemplateList,
                                                    R_AVKON_SOFTKEYS_SELECT_CANCEL,
                                                    AknPopupLayouts::EMenuWindow );
    // Push popupList'pointer to CleanupStack.
    CleanupStack::PushL( popupList );


    // Initialize listbox.
    nTemplateList->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    nTemplateList->CreateScrollBarFrameL( ETrue );
    nTemplateList->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Set title
    // Allocate TBuf with constant length.
    TBuf<KMaxTitleLength> title;

    // Reads a resource into a descriptor.
    env->ReadResourceL( title, R_QTN_CHAT_TEMPLATE_LIST_TITLE );

    popupList->SetTitleL( title );
    //popupList->SetTitleL( _L("Select Templates:") );

    // Set listitems.
    CTextListBoxModel* model = nTemplateList->Model();
    model->SetOwnershipType( ELbmOwnsItemArray );
    model->SetItemTextArray( aResultArray );

    // Show popup nProfileList and then show return value.
    TBool popupOk = popupList->ExecuteLD();
    CleanupStack::Pop();

    // Pop the popupList's pointer from CleanupStack

    if ( popupOk )
        {
        TInt index = nTemplateList->CurrentItemIndex();
        CCAMessageEditor& editor = iContainer->Editor();
        TInt cursorPos( editor.CursorPos() );
        editor.RichText()->InsertL( cursorPos, ( *aResultArray )[index] );
        //editor.SetTextL( &((*aResultArray)[index]) );
        editor.SetCursorPosL( editor.TextLength() + 1, EFalse );
        iContainer->StartScrollingL();
        }

    // Pop and Destroy the nProfileList pointer from CleanupStack
    CleanupStack::PopAndDestroy( nTemplateList );
    }

// ---------------------------------------------------------
// CCAConversationsView::HandleForegroundEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::HandleForegroundEventL( TBool aForeground )
    {
    iForeground = aForeground;

    if ( iContainer )
        {
        //don't read messages if we're not in foreground
        iContainer->FetchMessages( aForeground );
        }
    }


// ---------------------------------------------------------
// CCAConversationsView::HandleContactChange(...)
// ---------------------------------------------------------
//
void CCAConversationsView::HandleContactChange( const TDesC& /*aContact*/ )
    {
    // this contact was added/edited to contact store, update title
    TRAPD( err, UpdateTitleL() );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }


// ---------------------------------------------------------
// CCAConversationsView::DoActivateL(...)
// Handles view activation
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::DoActivateL(
    const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    // Unregister the view that was registered from Forward option.
    // didn't find any better place to put this.
    if ( TEnumsPC::EToBeUnregistered == iAppUI->RetForwardTo() )
        {
        // Deregister conversation from switch back views
        iAppUI->UnRegisterPreviousview();
        iAppUI->SetResetForwardTo( TEnumsPC::EUnregistered );
        }

    const MCAConversationMessage* forwardedMessage = NULL;
    TBool storedEditorContent = EFalse;
    TPtr groupId( iContactId->Des() );
    TPtr listId( iListId->Des() );
    TBool activateFromTabSwitch = EFalse;
    if ( aCustomMessageId == KUidConvViewMsgId ||
         aCustomMessageId == KUidExtViewActivationId )
        {
        TCADnlConvViewBuf viewBuf;
        viewBuf.Copy( aCustomMessage );

        groupId.Copy( viewBuf().iWVID.Left( groupId.MaxLength() ) );
        listId.Copy( viewBuf().iListID.Left( listId.MaxLength() ) );
        activateFromTabSwitch = viewBuf().iSwitchTab;

        if ( activateFromTabSwitch )
            {
            iTabId = viewBuf().iTabId;
            }

        if ( viewBuf().iIsForwarded )
            {
            CHAT_DP( D_CHAT_LIT( "--forwarding message %S" ), &viewBuf().iForwardedMessage->Text() );
            forwardedMessage = viewBuf().iForwardedMessage;
            }

        storedEditorContent = viewBuf().iSAPChanged;
        }

    // When forwarding from contact to contact DoDeactivate is never called.
    // We have to cleanup the necessary stuff ourself.
    CCAConversationsContainer* oldContainer = NULL;
    if ( iContainer && iAppUI )
        {
        // if we were in a middle of sending message, don't delete the container
        if ( !iContainer->Sending() )
            {
            // Container
            iAppUI->RemoveFromStack( iContainer );
            oldContainer = iContainer;
            CleanupStack::PushL( oldContainer );
            iContainer = NULL;

            if ( iAddAndReplyMenu )
                {
                iAppUI->RemoveFromViewStack( *this, iAddAndReplyMenu );
                }

            if ( iFindContextMenu )
                {
                iAppUI->RemoveFromViewStack( *this, iFindContextMenu );
                }
            }
        }


    iConversationPC->InitialiseResourcesL( *iContactId );

    // Register this as chat observer
    iConversationPC->AddObserver( this );

    // Register switch back if conversation view is opened from
    // search view
    if ( iAppUI && aPrevViewId.iViewUid == KUidSearchView )
        {

        TCADnlConvViewBuf viewBuf;
        viewBuf.Copy( aCustomMessage );

        TCADnlConvInvGroupFocus dnlConvInvGroupFocus;
        dnlConvInvGroupFocus.iInvConvGroupID.Copy( iContactId->Des().Left( KMaxWVIDLength ) );
        dnlConvInvGroupFocus.iListID.Copy( iListId->Des().Left( KMaxWVIDLength ) );
        dnlConvInvGroupFocus.iType = EConversationItem;
        TCADnlConvInvGroupFocusBuf convInvGroupFocusBuf( dnlConvInvGroupFocus );

        iAppUI->RegisterSwitchBack(
            KUidFriendsListView,
            KConvInvGroupFocusId,
            convInvGroupFocusBuf.AllocL(),
            KNullUid );
        }


    // To prevent looping between this view and refresh view
    if ( aPrevViewId.iViewUid != KUidRefreshView )
        {
        iPrevViewId = aPrevViewId;
        }


    if ( iContainer )
        {
        // container already exists, reset sending flag
        iContainer->ResetSending();
        }
    else
        {
        iContainer = CCAConversationsContainer::NewL( ClientRect(),
                                                      *static_cast<MCAViewSwitcher*>( iAppUI ),
                                                      iConversationPC,
                                                      *this, this, *iMessageHandler, *this, *this, Cba(),
                                                      activateFromTabSwitch, iTabId );
        }


    // Restore find state
    TInt findState = IMUtils::CRKeyL( KCRUidCommonUi, KCuiAutomaticHighlight );
    CCAMessageEditor& editor = iContainer->Editor();

    iContainer->SetItemHighlightL( findState );

    // Handle forwarded message
    if ( forwardedMessage )
        {
        CHAT_DP(
            D_CHAT_LIT( "CCAConversationsView::DoActivateL, forwarding message" ) );

        MCASettingsPC* settingsPC = iAppUI->GetProcessManager().GetSettingsInterface();

        HBufC* ownId = settingsPC->GetSapSettingValuePCL(
                           TEnumsPC::EOwnWVUserID );
        CleanupStack::PushL( ownId );

        CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC();
        TBool ret = utils->ForwardContentMessageL(
                        *forwardedMessage,
                        groupId, // recipient
                        NULL,
                        *iConversationPC,
                        EFalse, // no whispering
                        ownId->Des() );

        CleanupStack::PopAndDestroy( 2, ownId ); //utils, ownId

        // Message was not handled so we put it into the editor.
        if ( !ret )
            {
            // Wether to put part or whole message
            if ( forwardedMessage->ForwardPart() )
                {
                editor.SetTextL( forwardedMessage->ForwardPart() );
                }
            else
                {
                editor.SetTextL( &forwardedMessage->Text() );
                }
            editor.SetCursorPosL( editor.TextLength(), EFalse );

            iContainer->StartScrollingL();

            }
        }

    // Handle stored editor content
    if ( storedEditorContent )
        {
        editor.SetTextL( iEditorContent );
        editor.SetCursorPosL( editor.TextLength(), EFalse );
        iContainer->SendMessageL();
        }

    if ( !editor.TextLength() )
        {
        TInt editorFlags = ( editor.AknEdwinFlags() | EAknEditorFlagNoEditIndicators );
        editor.SetAknEditorFlags( editorFlags );
        }

    iAppUI->AddToViewStackL( *this, iContainer );
    iAppUI->AddToViewStackL( *this, iAddAndReplyMenu, 0,
                             ECoeStackFlagRefusesFocus );
    iAppUI->AddToStackL( *this, iFindContextMenu, ECoeStackPriorityDefault,
                         ECoeStackFlagRefusesFocus );
    if ( iTitlePane )
        {

        if ( !activateFromTabSwitch )
            {

            //hide navi pane decorators
            iTitlePane->ClearNaviPaneL();

            //if activation is from contact list and not from switching between
            //tabs then create tabs and show them
            iTabId = iTitlePane->ShowTabGroupL( TEnumsPC::EConversationItem, iContactId->Des() );
            }

        iTitlePane->AddObserver( this ) ;

        // Get temp id handler handle
        // Get identification through temp identification handler
        // code merge 14 nov, 2006
        MCAMainViewArrayPC* mainViewArrayPC = iAppUI->GetProcessManager().GetArrayInterface();

        delete iTitle;
        iTitle = NULL;
        TRAPD( err, iTitle = iTitlePane->GetTabItemIdentification( iTabId ).AllocL() );
        if ( err )
            {
            // Contact list was not synchronized yet, just strip the "wv:" etc.
            iTitle = CCAPCUtils::DisplayId( *iContactId ).AllocL();
            }


        iTitlePane->SetTitleL( *iTitle );


        }
#ifndef RD_30_DISABLE_TOUCH
    // set the context menu for menubar (launched with middle softkey)
    MenuBar()->SetContextMenuTitleResourceId( R_CHATCLIENT_MENUBAR_FINDITEMUI );
#endif



    if ( oldContainer )
        {
        // Everything should be ready and we can delete the old container
        CleanupStack::PopAndDestroy(); // oldContainer
        }



    }

// ---------------------------------------------------------
// CCAConversationsView::HandleCommandL()
// Handles view deactivation
// ---------------------------------------------------------
//
void CCAConversationsView::DoDeactivate()
    {
    CHAT_DP_TXT( "DoDeactivate" );

    iConversationPC->RemoveObserver();
    iConversationPC->ReleaseResources();

    if ( iTitlePane )
        {
        iTitlePane->RemoveObserver( this );
        }
    if ( iContainer )
        {
        iAppUI->RemoveFromViewStack( *this, iContainer );

        if ( !iContainer->Sending() )
            {
            // not in a middle of sending message
            delete iContainer;
            iContainer = NULL;
            }
        }

    if ( iAddAndReplyMenu )
        {
        iAppUI->RemoveFromViewStack( *this, iAddAndReplyMenu );
        }

    if ( iFindContextMenu )
        {
        iAppUI->RemoveFromViewStack( *this, iFindContextMenu );
        }

    iLastUnreadIndex = KErrNotFound;

    delete iTitle;
    iTitle = NULL;
    delete iSender;
    iSender = NULL;
    }

// ---------------------------------------------------------
// CCAConversationsView::ShowRecipientsListL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAConversationsView::ShowRecipientsListL( TInt /*aResourceId*/ )
    {
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CCAConversationsView::ShowPopUpMenuL()
// This method shows popup menu
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::ShowPopUpMenuL( )
    {
    if ( iContainer->SelectedItemType() != KErrNotFound )
        {
#ifndef RD_30_DISABLE_TOUCH
        MenuBar()->StopDisplayingMenuBar();
        MenuBar()->TryDisplayContextMenuBarL();
#else
        iFindContextMenu->StopDisplayingMenuBar();
        iFindContextMenu->TryDisplayMenuBarL();
#endif
        }
    }

// ---------------------------------------------------------
// CCAConversationsView::HandleChatEvent
// (other items were commented in a header).
// ---------------------------------------------------------
void CCAConversationsView::HandleChatEvent( const TDesC& aWvid,
                                            const TDesC& aContactId )
    {

    TRAPD( err, DoHandleChatEventL( aWvid, aContactId ) );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }

    }

// ---------------------------------------------------------
// CCAConversationsView::DoHandleChatEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::DoHandleChatEventL( const TDesC& aWvid,
                                               const TDesC& aContactId )
    {
    HBufC* tempSender = aWvid.AllocL();
    delete iSender;
    iSender = tempSender;

    // nickname or wv-less contact id
    iCleanSender.Set( aContactId );

    }

//added for multitab support


// ---------------------------------------------------------
// CCAConversationsView::SwitchTabL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::SwitchTabL( const TDesC& aWvId, const TInt aTabIndex )
    {
    //change the contact id based on tteh currenttab
    TPtr groupId( iContactId->Des() );
    TPtr listId( iListId->Des() );
    if ( iListId )
        {
        listId = KNullDesC();
        }
    groupId.Copy( aWvId.Left( groupId.MaxLength() ) );

    iTabId = aTabIndex;

    UpdateTitleL();

    iConversationPC->SetActiveConversationL( aWvId );

    //remove the controls from view stack
    if ( iContainer )
        {
        iAppUI->RemoveFromViewStack( *this, iContainer );
        }

    if ( iAddAndReplyMenu )
        {
        iAppUI->RemoveFromViewStack( *this, iAddAndReplyMenu );
        }

    if ( iFindContextMenu )
        {
        iAppUI->RemoveFromViewStack( *this, iFindContextMenu );
        }


    iContainer->SwitchViewL();

    //again add those controls to view stack
    if ( iContainer )
        {
        iAppUI->AddToViewStackL( *this, iContainer );
        }

    if ( iAddAndReplyMenu )
        {
        iAppUI->AddToViewStackL( *this, iAddAndReplyMenu, 0,
                                 ECoeStackFlagRefusesFocus );
        }

    if ( iFindContextMenu )
        {
        iAppUI->AddToStackL( *this, iFindContextMenu, ECoeStackPriorityDefault,
                             ECoeStackFlagRefusesFocus );
        }

    }


// ---------------------------------------------------------

// ---------------------------------------------------------
// CCAConversationsView::HandleMessageError
// ---------------------------------------------------------
//
void CCAConversationsView::HandleMessageError( TInt aError,
                                               const TDesC& aInfo,
                                               TEnumsPC::TContentType aMsgContentType )
    {
    TRAPD( err, DoHandleMessageErrorL( aError, aInfo, aMsgContentType ) );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }


// ---------------------------------------------------------
// CCAConversationsView::DoHandleMessageErrorL
// ---------------------------------------------------------
//
void CCAConversationsView::DoHandleMessageErrorL( TInt aError,
                                                  const TDesC& aInfo,
                                                  TEnumsPC::TContentType aMsgContentType )
    {
    if ( aError == KErrNoMemory ) // handled in AppUi::HandleMessageError
        {
        return;
        }
    if ( TEnumsPC::EContentPicture == aMsgContentType )
        {
        TInt res( aError == KErrOverflow ?
                  R_QTN_CHAT_SCALING_IMAGE_TOO_BIG :
                  R_QTN_CHAT_SCALING_ERROR );

        // sending of image failed
        IMDialogUtils::DisplayInformationNoteL( res );
        return;
        }
    if ( aError == ECSPUnknownUserId )
        {
        // This way error can be recognized in note mapper
        aError += Imps_ERROR_BASE;
        }

    IMNoteMapper::ShowNoteL( aError, aInfo );
    }


// ---------------------------------------------------------
// CCAConversationsView::ForwardToContactL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::ForwardToContactL()
    {
    MCAConversationMessage* message = iContainer->SelectedMessage();
    if ( !message )
        {
        // nothing selected
        return;
        }

    //Set the focus flag as true 
    iAppUI->SetFocusFlag(ETrue);

    CDesCArray* selectedContacts = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selectedContacts );

    CDesCArray* selectedIdentifications = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selectedIdentifications );

    MCASkinVariant* skinVar = static_cast<CCAApp*>( iAppUI->Application() )
                              ->VariantFactory()->SkinVariantL();

    // show selection dialog

    MCAViewSwitcher& aViewSwitcher( *static_cast<MCAViewSwitcher*>( iAppUI ) );

    if ( !CCAContactSelectionDialog::ShowDialogL(
             *selectedContacts,
             *iAppUI->GetProcessManager().GetArrayInterface(),
             *skinVar,
             *iAppUI->GetProcessManager().GetSettingsInterface(),
             iAppUI->MbmFullPath(),
             TEnumsPC::ESingleSelect,
             R_CONTACT_SELECTION_DIALOG,
             *aViewSwitcher.CAStatusPane(),
             selectedIdentifications  ) )
        {
        // no contacts
        CleanupStack::PopAndDestroy( 2, selectedContacts ); // selectedContacts,selectedIdentifications
        return;
        }

    // allow tabs - suppose user would have received new messages
    iTitlePane->RestoreNaviPane();

    // handle selected contact (if any)
    if ( selectedContacts->MdcaCount() == 1 )
        {

        TPtrC wvid( selectedContacts->MdcaPoint( 0 ) );

        MCAMainViewArrayPC* mainViewArrayPC = iAppUI->GetProcessManager().GetArrayInterface();

        //Add the conversation item to open chats array before switching the view
        mainViewArrayPC->InsertConversationItemL( wvid,
                                                  selectedIdentifications->MdcaPoint( 0 ) );

        // Check if there is highlighted item
        HBufC* item = iContainer->SelectedItemL();
        CleanupStack::PushL( item );

        if ( item->Length() > 0 )
            {
            // Set forwarded message
            message->SetForwardPartL( item );
            }
        else
            {
            // Clear previous forward part
            message->SetForwardPartL( NULL );
            }

        CleanupStack::PopAndDestroy( item );

        iAppUI->SetForwardMessageL( message );

        const MCAConversationMessage* fwdMessage = iAppUI->ForwardMessage();

        if ( !fwdMessage )
            {
            CleanupStack::PopAndDestroy(); // selectedContacts.Close()
            return;
            }

        if ( TEnumsPC::EUnregistered == iAppUI->RetForwardTo()  )
            {
            iAppUI->SetResetForwardTo( TEnumsPC::ERegister );
            iAppUI->GroupUtils()->PrepareToSwitchBackL( iContactId->Des(),
                                                        KUidConversationsView,
                                                        KUidConversationsView );
            }
        else if ( TEnumsPC::ERegister == iAppUI->RetForwardTo() )
            {
            iAppUI->SetResetForwardTo( TEnumsPC::EAlreadyRegistered );
            }

        if ( !iConversationPC->IsActiveConversation( wvid ) && iTitlePane )
            {

            iTabId = iTitlePane->ShowTabGroupL( TEnumsPC::EConversationItem, wvid );

            SwitchTabL( wvid, iTabId );

            }
        //Check whether we have fwded the message to any other contact
        //if so add a tab and switch the tab to focus on the forwarded contact
        MCASettingsPC* settingsPC = iAppUI->GetProcessManager().GetSettingsInterface();

        HBufC* ownId = settingsPC->GetSapSettingValuePCL(
                           TEnumsPC::EOwnWVUserID );
        CleanupStack::PushL( ownId );

        CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC();
        TBool ret = utils->ForwardContentMessageL(
                        *fwdMessage,
                        wvid, // recipient
                        NULL,
                        *iConversationPC,
                        EFalse, // no whispering
                        ownId->Des() );

        CleanupStack::PopAndDestroy( 2, ownId ); //utils, ownId

        CCAMessageEditor& editor = iContainer->Editor();

        // Message was not handled so we put it into the editor.
        if ( !ret )
            {
            // Wether to put part or whole message
            if ( fwdMessage->ForwardPart() )
                {
                editor.SetTextL( fwdMessage->ForwardPart() );
                }
            else
                {
                editor.SetTextL( &fwdMessage->Text() );
                }
            editor.SetCursorPosL( editor.TextLength(), EFalse );

            iContainer->StartScrollingL();

            }

        }

    CleanupStack::PopAndDestroy( 2, selectedContacts ); // selectedContacts,selectedIdentifications
    }



// ---------------------------------------------------------
// CCAConversationsView::ForwardToGroupL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsView::ForwardToGroupL()
    {
    MCAConversationMessage* message = iContainer->SelectedMessage();
    if ( !message )
        {
        // nothing selected
        return;
        }

    MCAServerContactsArrayPC* pairsArray ( iConversationPC->PopulateGroupDetailsLC() );

    HBufC* emptyText = iEikonEnv->AllocReadResourceLC(
                           R_CHAT_IBOX_FRIENDS_OFFLINE );
    HBufC* title = iEikonEnv->AllocReadResourceLC(
                       R_PRIVATE_CHAT_FORWARD_TITLE_GROUP );

    TInt selectedIndex( 0 );
    TInt retVal( IMDialogUtils::DisplayListQueryDialogL( &selectedIndex,
                                                         pairsArray,
                                                         R_PRIVATE_CHAT_LIST_SELECT_RECIP_DLG,
                                                         *title,
                                                         *emptyText, ETrue ) );

    CleanupStack::PopAndDestroy( 2, emptyText ); // title, emptyText

    if ( retVal == EAknSoftkeyOk || retVal == EAknSoftkeyDone
         || retVal == EAknSoftkeySelect )
        {
        // Check if there is highlighted item
        HBufC* item = iContainer->SelectedItemL();
        CleanupStack::PushL( item );

        if ( TEnumsPC::EUnregistered == iAppUI->RetForwardTo()  )
            {
            iAppUI->SetResetForwardTo( TEnumsPC::ERegister );
            iAppUI->GroupUtils()->PrepareToSwitchBackL( iContactId->Des(),
                                                        KUidConversationsView,
                                                        KUidChatView );
            }
        else if ( TEnumsPC::ERegister == iAppUI->RetForwardTo() )
            {
            iAppUI->SetResetForwardTo( TEnumsPC::EAlreadyRegistered );
            }
        if ( item->Length() > 0 )
            {
            // Set forwarded message
            message->SetForwardPartL( item );
            }
        else
            {
            // Clear previous forward part
            message->SetForwardPartL( NULL );
            }

        CleanupStack::PopAndDestroy( item );


        TInt err( iAppUI->GroupUtils()->JoinGroupL( ETrue,
                                                    pairsArray->WVIdL( selectedIndex )->iWVID,
                                                    KNullDesC, message ) );

        if ( err )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }
    //always dont use CleanupStack::PopAndDestroy(pairsArray)
    //because there might be a object slicing happening
    //when the pairsArray is transferred from the creator
    //to the caller. Hence one will end up deleting the wrong ptr.
    //Hence it is better to use CleanupStack::PopAndDestroy(), as
    //we dont know wat the creator has pushed onto the CleanupStack
    CleanupStack::PopAndDestroy();
    }


//  End of File
