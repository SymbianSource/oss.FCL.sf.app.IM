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
* Description:  View class for chat group view
*
*/


// INCLUDE FILES
#include	"CCAChatView.h"
#include	"CCAChatViewContainer.h"
#include	"chatngclient.hrh"
#include	"CCAStatusPaneHandler.h"
#include	"CCAAppUi.h"
#include	"CAExternalInterface.h"
#include	"CCAMessageEditor.h"
#include    "CCAUISessionManager.h"
#include	"CAArrayUtils.h"
#include	"ChatDebugPrint.h"
#include    "IMDialogUtils.h"
#include    "ChatDefinitions.h"
#include    "CAExternalInterface.h"
#include    "IMNoteMapper.h"
#include    "ImpsCSPAllErrors.h"
#include    "CCAMessageExtensionsHandler.h"
#include    "MCAMessageExtension.h"
#include	"CCAFadeControl.h"
#include    "CCAUiMessageUtils.h"
#include    "cautils.h"
#include    "IMUtils.h"
#include	"MCAConversationMessage.h"
#include    "IMPrivateCRKeys.h"
#include    "IMVariant.hrh"
#include    "mcagrouppc.h"
#include    "MCAProcessManager.h"
#include    "MCAMainViewArrayPC.h"
#include    "CCAPCUtils.h"
#include	"CCAGroupUtils.h"
#include	"MCAOpenChatsPC.h"
#include	"CCAProcessManagerFactory.h"
#include	"MCAGroupUtilsPC.h"

#include	<eikbtgpc.h>
#include    <stringloader.h>
#include	<eikgted.h> //For the CEikGlobalTextEditor
#include	<eikmenup.h>
#include	<eikmenub.h>
#include	<aknpopup.h>
#include	<aknnotewrappers.h>
#include    <aknenv.h>
#include	<flogger.h>
#include	<utf.h>
#include	<aknquerydialog.h>
#include	<ChatNG.rsg>
#include	<bldvariant.hrh>
#include    <FeatMgr.h>
#include    <hlplch.h>
#include    <finditem.hrh>
#include    <finditemmenu.h>
#include    <commonuiinternalcrkeys.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"



// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CCAChatView::CCAChatView()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CCAChatView::CCAChatView()
        : iMenuPane ( NULL )
    {
    }
// ---------------------------------------------------------
// CCAChatView::ConstructL( TInt aAvkonViewResourceId, TUid aViewId )
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CCAChatView::ConstructL( TInt aAvkonViewResourceId, TUid aViewId,
                              CCAMessageExtensionsHandler& aMessageHandler )
    {
    BaseConstructL( aAvkonViewResourceId, aViewId );

    iMessageHandler = &aMessageHandler;

    // Construct array for recipient list
    iRecipients = new( ELeave ) CDesCArrayFlat( KArrayGranularity );
    iJoinedMembers = new( ELeave )CDesCArrayFlat( KArrayGranularity );

    iFindContextMenu = new( ELeave ) CEikMenuBar;
    iFindContextMenu->ConstructL( this, 0, R_CHATCLIENT_MENUBAR_FINDITEMUI );

    iAddAndReplyMenu = new( ELeave ) CEikMenuBar;
    iAddAndReplyMenu->ConstructL(
        this,
        0,
        R_CHATCLIENT_MENUBAR_ADDANDREPLY );


    iGroupId = HBufC::NewL( KMaxWVIDLength );
    iRecipient = HBufC::NewL( KMaxWVIDLength );


    iGroupUtils = CAAppUi()->GroupUtils();

    iGroupPC = CAAppUi()->GetProcessManager().GetGroupInterface();

    iFindMenu = CFindItemMenu::NewL( EChatClientFindMenuPlaceHolder );
    iFindMenu->AttachItemFinderMenuL( R_CHATCLIENT_IBOX_VIEW_MENU );
    iFindMenu->AttachItemFinderMenuL( R_CHATCLIENT_MENUBAR_FINDITEMUI );

    // automatic finder on by default
    iFindMenu->HandleItemFinderCommandL( EFindItemCmdEnableAutoFind );
    iItemFinder = CItemFinder::NewL();
    }

// ---------------------------------------------------------
// CCAChatView::~CCAChatView()
// Destructor
// ---------------------------------------------------------
//
CCAChatView::~CCAChatView()
    {
    //In manual login,on exit from options menu
    //DoDeactivate is not getting called,so free resources here
    if ( iGroupPC )
        {
        iGroupPC->UnRegisterGroupObserver( );
        iGroupPC->ReleaseResources();
        }

    if ( iAsyncCallBack )
        {
        if ( iAsyncCallBack->IsActive() )
            {
            iAsyncCallBack->Cancel();
            }
        delete iAsyncCallBack;
        }

    if ( iAddAndReplyMenu )
        {
        AppUi()->RemoveFromStack( iAddAndReplyMenu );
        }

    delete iAddAndReplyMenu;

    if ( iFindContextMenu )
        {
        AppUi()->RemoveFromStack( iFindContextMenu );
        delete iFindContextMenu;
        }

    delete iRecipients;

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    delete iContainer;
    delete iGroupId;
    delete iRecipient;
    delete iSender;
    delete iJoinedMembers;
    delete iFindMenu;
    delete iItemFinder;

    }

// ---------------------------------------------------------
// CCAChatView::HandleCommandL(TInt aCommand)
// Handles commands
// ---------------------------------------------------------
//
void CCAChatView::HandleCommandL( TInt aCommand )
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( iEikonEnv->AppUi() );
    switch ( aCommand )
        {
        case EAknSoftkeyOpen: // flowthrough
        case EChatClientOpenObject:
            {
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
            iContainer->SendObjectL( ETrue, ENewFileServiceImage );
            break;
            }
        case EChatClientFromPhotos: // flowthrough
        case EChatClientSendImage:
            {
            iCba = CEikButtonGroupContainer::Current();

            if ( iCba )
                {
                iCba->MakeCommandVisible( EAknSoftkeyOptions, EFalse );
                iCba->MakeCommandVisible( EAknSoftkeyBack, EFalse );
            	iCba->MakeCommandVisible( EChatClientChatViewContinue, EFalse );
                iCba->DrawNow();
                }

            iContainer->SendObjectL( EFalse );

            if ( iCba )
                {
                iCba->MakeCommandVisible( EAknSoftkeyOptions, ETrue );
                iCba->MakeCommandVisible( EAknSoftkeyBack, ETrue );
                iCba->MakeCommandVisible( EChatClientChatViewContinue, ETrue );
                iCba->DrawNow();
                }

            break;
            }

        //ChatClientCmdBack and EAknSoftKeyBack have same function
        case EChatClientCmdBack:
        case EAknSoftkeyBack:
            {
            CAAppUi()->CAStatusPane()->SynchroniseAndRemoveTabGroup();
            if ( TEnumsPC::ERegister == appUi->RetForwardTo() )
                {
                appUi->SetResetForwardTo( TEnumsPC::EToBeUnregistered );

                appUi->SwitchViewL( KUidFriendsListView );
                }
            else
                {
                if ( TEnumsPC::EAlreadyRegistered == appUi->RetForwardTo() )
                    {
                    appUi->UnRegisterPreviousview();
                    appUi->SetResetForwardTo( TEnumsPC::EUnregistered );
                    }
                TCADnlConvInvGroupFocus dnlConvInvGroupFocus;
                dnlConvInvGroupFocus.iInvConvGroupID.Copy( iGroupId->Des().Left( KMaxWVIDLength ) );
                dnlConvInvGroupFocus.iListID.Copy( KNullDesC() );
                dnlConvInvGroupFocus.iType = EGroupItem;
                TCADnlConvInvGroupFocusBuf convInvGroupFocusBuf( dnlConvInvGroupFocus );
                CAAppUi()->SwitchViewBackL( KUidFriendsListView, KConvInvGroupFocusId, convInvGroupFocusBuf );
                }
            break;
            }
        // Flow through
        case EAknSoftkeySend:   // MSK command
        case EChatClientIboxViewSend:
        case EChatClientChatViewCmdSend:
            {
            iContainer->SendMessageL();
            break;
            }

        case EChatClientInviteFriend:
            {
            // launch the invite without query
            DoLaunchInviteL( EFalse );
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

        case EChatClientChatViewContinue:
            {
            iContainer->StartScrollingL();
            break;
            }

        case EChatClientChatViewCmdWhisper:
            {
            ShowRecipientsListL( R_CHATVIEW_RECIPIENT_QUERY );
            break;
            }
        // flow through
        case EChatClientIboxReply:
        case EChatClientChatViewCmdReply:
            {
            if ( iAddAndReplyMenu->IsDisplayed() )
                {
                iAddAndReplyMenu->StopDisplayingMenuBar();
                }
            iContainer->ReplyL();
            break;
            }

        case EChatClientIboxForwardToGroup:
            {
            iContainer->ForwardToGroupL();
            break;
            }
        case EChatClientIboxForwardToContact:
            {
            iContainer->ForwardToContactL();
            break;
            }

        case EChatClientChatViewLogOn:
            {
            iContainer->SaveChatL();
            break;
            }



        case EChatClientChatViewCmdLeave:
            {
            iContainer->LeaveGroupL();
            //switch view is called in handleleft also
            //calling switch view twice would not create any problem
            //switch view is called here because just after creating the group if
            //we leave from the group view , we would not get callback in handleleft
            CAAppUi()->SwitchViewL( KUidFriendsListView );

            break;
            }
        case EChatClientChatViewCmdParticip:
            {
            iGroupUtils->DisplayJoinedMembersL( *iGroupId );
            break;
            }
        case EChatClientChatViewCmdGroupInfo:
            {
            iContainer->DisplayChatInfoL();
            break;
            }
        case EChatClientChatViewEdit:
            {
            iContainer->EditChatGroupPropertiesL();
            break;
            }
        case EChatClientChatViewFavor:
            {
            iGroupUtils->SaveAsFavouriteL( *iGroupId );
            break;
            }

        // Help key support EChatClientCmdHelp is assigned EAknCmdHelp in
        // chatngclient.hrh
        case EChatClientCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                                 AppUi()->AppHelpContextL() );
            break;
            }

        // flow through
        case EFindItemCmdEnableAutoFind:
        case EFindItemCmdDisableAutoFind:
            {
            iContainer->SetItemHighlight(
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
            CAAppUi()->CAStatusPane()->RemoveObserver( this ) ;
            AppUi()->HandleCommandL( aCommand );
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
                AppUi()->HandleCommandL( aCommand );
                }
            break;
            }
        }
    }


// ---------------------------------------------------------
// CCAChatView::HandleForegroundEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatView::HandleForegroundEventL( TBool aForeground )
    {
    if ( iContainer )
        {
        //don't read messages if we're not in foreground
        iContainer->FetchMessages( aForeground );
        }
    CCAView::HandleForegroundEventL( aForeground );
    }

// ---------------------------------------------------------
// CCAChatView::DoActivateL(...)
// Handles view activation
// ---------------------------------------------------------
//
void CCAChatView::DoActivateL(
    const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    CHAT_DP_TXT( "CCAChatView::DoActivateL" );
    TBool launchInvite( EFalse );

    // Unregister the view that was registered from Forward option.
    // didn't find any better place to put this.
    if ( TEnumsPC::EToBeUnregistered == CAAppUi()->RetForwardTo() )
        {
        // Deregister conversation from switch back views
        CAAppUi()->UnRegisterPreviousview();
        CAAppUi()->SetResetForwardTo( TEnumsPC::EUnregistered );
        }
    const MCAConversationMessage* forwardedMessage = NULL;

    TBool activateFromTabSwitch ( EFalse );

    if ( aCustomMessageId == KUidChatViewMsgId )
        {
        TCADnlChatViewBuf viewBuf;
        viewBuf.Copy( aCustomMessage );

        activateFromTabSwitch = viewBuf().iSwitchTab;

        if ( activateFromTabSwitch )
            {
            iTabId = viewBuf().iTabId;
            }

        HBufC* group = viewBuf().iGroupId.AllocL();
        delete iGroupId;
        iGroupId = group;
        if ( viewBuf().iGroupName.Length() > 0 )
            {
            CAAppUi()->CAStatusPane()->SetTitleL( viewBuf().iGroupName );
            }
        else
            {
            CAAppUi()->CAStatusPane()->
            SetTitleL( CCAPCUtils::DisplayId( *iGroupId ) );
            }
        if ( viewBuf().iIsForwarded )
            {
            forwardedMessage = viewBuf().iForwardedMessage;
            }

        launchInvite = viewBuf().iInviteQuery;
        }


    // Register this as chat observer

    iGroupPC->RegisterGroupObserver( this );

    // set the context menu for menubar (launched with middle softkey)
#ifndef RD_30_DISABLE_TOUCH
    MenuBar()->SetContextMenuTitleResourceId(
        R_CHATCLIENT_MENUBAR_FINDITEMUI );
#endif // RD_30_DISABLE_TOUCH

    // To prevent looping between this view and search view
    if ( aPrevViewId.iViewUid == KUidSearchView )
        {
        // If this view is activated from search view register
        // chat list view as view for back option
        TCADnlChatViewBuf viewBuf;
        viewBuf.Copy( aCustomMessage );

        TCADnlConvInvGroupFocus dnlConvInvGroupFocus;
        dnlConvInvGroupFocus.iInvConvGroupID.Copy( viewBuf().iGroupId );
        dnlConvInvGroupFocus.iListID.Copy( KNullDesC() );
        dnlConvInvGroupFocus.iType = EGroupItem;
        TCADnlConvInvGroupFocusBuf convInvGroupFocusBuf( dnlConvInvGroupFocus );

        static_cast<CCAAppUi*>( AppUi() )->RegisterSwitchBack(
            KUidFriendsListView, KConvInvGroupFocusId, convInvGroupFocusBuf.AllocL(), KNullUid );

        CAAppUi()->GetProcessManager().GetArrayInterface()->PopulateGroupsListL();
        }

    // To prevent looping between this view and refresh view
    if ( aPrevViewId.iViewUid != KUidRefreshView )
        {
        iPrevViewId = aPrevViewId;
        }

    //can be done while construction after TCADnlChatViewBuf is refactored
    iGroupPC->InitialiseResourcesL( *iGroupId );

    MCAViewSwitcher* viewSwitcher = static_cast<MCAViewSwitcher*>( CAAppUi() );

    // When forwarding from group to group DoDeactivate is never called.
    // We have to cleanup the necessary stuff ourself.
    CCAChatViewContainer* oldContainer = NULL;
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        oldContainer = iContainer;
        iContainer = NULL;

        if ( iAddAndReplyMenu )
            {
            AppUi()->RemoveFromStack( iAddAndReplyMenu );
            }

        if ( iFindContextMenu )
            {
            AppUi()->RemoveFromStack( iFindContextMenu );
            }

        CleanupStack::PushL( oldContainer );
        }

    iContainer = CCAChatViewContainer::NewL( ClientRect(), *viewSwitcher,
                                             iGroupPC,
                                             *this, this, *iMessageHandler,
                                             Cba(), activateFromTabSwitch, iTabId , this );
    // Get group properties (waiting operation)


    static_cast<MCAGroupEventObserverPC*>
    ( iContainer )->HandleGroupPropertiesChanged();

    // Restore find state
    TInt findState = IMUtils::CRKeyL( KCRUidCommonUi, KCuiAutomaticHighlight );
    iContainer->SetItemHighlight( findState );

    CCAMessageEditor& editor = iContainer->Editor();

    // Handle forwarded message
    if ( forwardedMessage )
        {
        CHAT_DP(
            D_CHAT_LIT( "CCAChatView::DoActivateL, forwarding message" ) );


        CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC();
        CDesCArray* members = new( ELeave )CDesCArrayFlat( 1 );
        CleanupStack::PushL( members );



        TBool isGroupFound = iGroupUtils->FindGroup( iGroupPC->ActiveGroupId() );

        if ( isGroupFound )
            {
            iGroupUtils->GetParticipantsL( *members, iGroupPC->ActiveGroupId() );
            }
        // Remove user from targets
        TInt index;
        if ( members->Find( iGroupPC->ScreenName(), index ) == KErrNone )
            {
            members->Delete( index );
            members->Compress();
            }
        members->Sort( ECmpCollated );



        TBool ret = utils->ForwardContentMessageL(
                        *forwardedMessage,
                        *iGroupId, // recipient
                        members,
                        *iGroupPC,
                        iContainer->IsWhisperAllowedL(), // no whispering
                        iGroupPC->ScreenName() );

        //Note: code scanner warning is ignored
        CleanupStack::PopAndDestroy( 2 ); //utils, members

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

    if ( !editor.TextLength() )
        {
        TInt editorFlags = ( editor.AknEdwinFlags() | EAknEditorFlagNoEditIndicators );
        editor.SetAknEditorFlags( editorFlags );
        }

    AppUi()->AddToStackL( *this, iContainer );
    AppUi()->AddToStackL( *this, iAddAndReplyMenu, ECoeStackPriorityDefault,
                          ECoeStackFlagRefusesFocus );
    AppUi()->AddToStackL( *this, iFindContextMenu, ECoeStackPriorityDefault,
                          ECoeStackFlagRefusesFocus );

    if ( launchInvite )
        {
        delete iAsyncCallBack;
        iAsyncCallBack = NULL;

        iAsyncCallBack = new( ELeave )CAsyncCallBack(
            TCallBack( LaunchInvite, this ), CActive::EPriorityIdle );

        // hide CBA
        iCba = CEikButtonGroupContainer::Current();
        iCba->MakeVisible( EFalse );
        iCba->DrawNow();

        iAsyncCallBack->CallBack();
        }

    if ( !activateFromTabSwitch )
        {
        //Hides navi-pane decorators.
        CAAppUi()->CAStatusPane()->ClearNaviPaneL();

        //if activation is from contact list and not from switching between
        //tabs then create tabs and show them
        iTabId = CAAppUi()->CAStatusPane()->ShowTabGroupL( TEnumsPC::EGroupItem, *iGroupId );
        }
    CAAppUi()->CAStatusPane()->AddObserver( this ) ;

    if ( oldContainer )
        {
        // Everything should be ready and we can delete the old container
        CleanupStack::PopAndDestroy(); // oldContainer
        }
    CHAT_DP_TXT( "CCAChatView::DoActivateL done" );
    }

// ---------------------------------------------------------
// CCAChatView::ShowRecipientsListL()
// This method launches recipient list
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAChatView::ShowRecipientsListL( TInt aResourceId )
    {
    CArrayFixFlat<TInt>* indexArray =
        new ( ELeave ) CArrayFixFlat<TInt>( KArrayGranularity );
    CleanupStack::PushL( indexArray );

    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( items );

    HBufC* text = HBufC::NewLC( KWVNameMaxLength + KIconTabulator().Size() );
    TPtr textPtr( text->Des() );


    TBool isGroupFound = iGroupUtils->FindGroup( *iGroupId );
    if ( isGroupFound )
        {
        iJoinedMembers->Reset();
        iGroupUtils->GetParticipantsL( *iJoinedMembers, *iGroupId );
        }


    iJoinedMembers->Sort( ECmpCollated );

    TInt count( iJoinedMembers->Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        textPtr.Copy( KIconTabulator );
        textPtr.Append( ( *iJoinedMembers )[i] );
        items->AppendL( *text );
        }
    CleanupStack::PopAndDestroy( text );

    TInt result( 0 );
    CArrayFixFlat<TInt>* selectedArray =
        CAArrayUtils::SetSelectionsLC( *iRecipients, *iJoinedMembers );
    result = IMDialogUtils::DisplayListQueryDialogL( indexArray,
                                                     items,
                                                     aResourceId,
                                                     selectedArray );
    CleanupStack::PopAndDestroy( 2, items ); //selectedArray, items

    if ( result )
        {
        // use indexArray here
        TInt selectionCount( indexArray->Count() );
        iRecipients->Reset();
        iContainer->Recipients().Reset();
        HBufC* userIdBuf = HBufC::NewLC( KMaxWVIDLength );
        TPtr userIdPtr( userIdBuf->Des() );
        TBool groupMsg( EFalse );
        if ( selectionCount > 0 )
            {
            for ( TInt i( 0 ); i < selectionCount; i++ )
                {
                //Cache user id to descriptor
                userIdPtr.Copy( ( *iJoinedMembers )[ indexArray->At( i ) ] );
                iContainer->Recipients().AppendL( *userIdBuf );
                iRecipients->AppendL( *userIdBuf );
                }
            if ( selectionCount > 1 )
                {
                groupMsg = ETrue;
                }
            iContainer->InitPrivateMessageL(
                iContainer->Recipients().MdcaPoint( 0 ), groupMsg );
            }
        else
            {
            result =  0;
            }
        CleanupStack::PopAndDestroy( userIdBuf );
        }

    CleanupStack::PopAndDestroy( indexArray );

    return result;
    }

// ---------------------------------------------------------
// CCAChatView::ShowPopUpMenuL()
// This method shows popup menu
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatView::ShowPopUpMenuL( )
    {
    if ( iContainer->SelectedItemType() != KErrNotFound )
        {
#ifndef RD_30_DISABLE_TOUCH
        // S60 COMMON
        MenuBar()->StopDisplayingMenuBar();
        MenuBar()->TryDisplayContextMenuBarL();
#else
        // S60 3.0
        iFindContextMenu->StopDisplayingMenuBar();
        iFindContextMenu->TryDisplayMenuBarL();
#endif //RD_30_DISABLE_TOUCH
        }
    }

// ---------------------------------------------------------
// CCAChatView::DoDeactivate()
// Handles view deactivation
// ---------------------------------------------------------
//
void CCAChatView::DoDeactivate()
    {
    CHAT_DP_TXT( "DoDeactivate" );
    if ( !CAAppUi()->IsUnderDestruction() )
        {

        CCAStatusPaneHandler* statuspaneHandler = CAAppUi()->CAStatusPane();
        if ( statuspaneHandler )
            {
            statuspaneHandler->RemoveObserver( this ) ;
            }
        }
    if ( iGroupPC )
        {
        iGroupPC->UnRegisterGroupObserver( );
        iGroupPC->ReleaseResources();
        }



    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    if ( iAddAndReplyMenu )
        {
        AppUi()->RemoveFromStack( iAddAndReplyMenu );
        }

    if ( iFindContextMenu )
        {
        AppUi()->RemoveFromStack( iFindContextMenu );
        }

    delete iContainer;
    iContainer = NULL;
    }

// ---------------------------------------------------------
// CCAChatView::DynInitMenuPaneL
// Called when menupane is about to be shown.
// ---------------------------------------------------------
//
void CCAChatView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    CHAT_DP_FUNC_ENTER( "DynInitMenuPaneL" );

    TBool showFwdToContact = iContainer->FwdContactVisible();
    TBool showFwdToGroup   = iContainer->FwdGroupVisible();
    TBool showForwardMenu = showFwdToGroup || showFwdToContact;

    iMenuPane = aMenuPane;
    switch ( aResourceId )
        {
        case R_CHATCLIENT_GENERAL_MENU:
            {
            if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                // dims the help menu item
                aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
                }
            break;
            }

        case R_CHATCLIENT_ADDANDREPLY_MENU:
            {
            aMenuPane->SetItemDimmed( EChatClientSearchAddFriends, ETrue );
            if ( iContainer->Editor().IsFocused()
                 || !iContainer->IsWhisperAllowedL()
                 || iContainer->ApplicationUserSentThis()
                 || !iContainer->Received() )
                {
                // editor is focused, whispering is not allowed,
                // this is our own message or this is not received from server
                aMenuPane->SetItemDimmed( EChatClientIboxReply, ETrue );
                }
            break;
            }

        case R_CHATCLIENT_CHAT_VIEW_MENU:
            {
            TBool text( iContainer->Editor().TextLength() != 0 );
            TBool openable = EFalse;
            TBool savable = EFalse;
            TBool object = iContainer->IsObjectSelected( openable, savable );

            TBool isInviteAllowed( iContainer->IsInvitationAllowed() );
            TBool isUserAdmin( iContainer->IsUserAdmin() );
            TBool isGroupOpen( iContainer->IsGroupOpen() );

            // "send"
            aMenuPane->SetItemDimmed( EChatClientChatViewCmdSend, !text );

            // "insert smiley"
            if ( iContainer->Editor().IsFocused() )
                {
                aMenuPane->SetItemDimmed( EChatClientChatViewInsertSmiley,
                                          !iContainer->ExtensionFits( KChatSmileyLength ) );
                }
            else
                {
                aMenuPane->SetItemDimmed( EChatClientChatViewInsertSmiley,
                                          ETrue );
                }

            // "open"
            aMenuPane->SetItemDimmed( EChatClientOpenObject, !openable );

            // "save"
            aMenuPane->SetItemDimmed( EChatClientSaveObject, !savable );


            // item finder menu
            aMenuPane->SetItemDimmed( EChatClientFindMenuPlaceHolder, ETrue );

            if ( iContainer->IsStopped() )
                {
                UpdateItemTypeL();

                iFindMenu->AddItemFindMenuL( iItemFinder, aMenuPane,
                                             EChatClientFindMenuPlaceHolder,
                                             KNullDesC ); // We can ignore sender here
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

            // forward hiding logic case 3
            // Hide also when focus on object that is not openable
            if ( !showForwardMenu || ( object && !openable ) )
                {
                CHAT_DP_TXT(
                    "CCAChatView::DynInitMenuPaneL Hide the whole forward menu" );
                aMenuPane->SetItemDimmed(
                    EChatClientChatViewCmdForward, ETrue );
                }
            else
                {
                CHAT_DP_TXT(
                    "CCAChatView::DynInitMenuPaneL Hide the whole forward menu" );
                aMenuPane->SetItemDimmed( EChatClientChatViewCmdForward,
                                          !iContainer->IsStopped() );
                }

            TBool inviteSupported( UISessionManager().IsSupported(
                                       CCAUISessionManager::EInvite ) );

            if ( !inviteSupported )
                {
                aMenuPane->SetItemDimmed( EChatClientInviteFriend,
                                          ETrue );
                }
            else if ( isInviteAllowed )
                {
                if ( !isUserAdmin )
                    {
                    aMenuPane->SetItemDimmed( EChatClientInviteFriend,
                                              ( !isGroupOpen )  );
                    }
                }
            else
                {
                aMenuPane->SetItemDimmed( EChatClientInviteFriend,
                                          ETrue );
                }

            aMenuPane->SetItemDimmed( EChatClientChatViewCmdWhisper,
                                      ( iContainer->IsStopped() || !iContainer->IsWhisperAllowedL() ) );

            if ( iContainer->Editor().IsFocused()
                 || !iContainer->IsWhisperAllowedL()
                 || iContainer->ApplicationUserSentThis()
                 || !iContainer->Received() )
                {
                // editor is focused, whispering is not allowed,
                // this is our own message or this is not received from server
                aMenuPane->SetItemDimmed( EChatClientChatViewCmdReply, ETrue );
                }



            break;
            }

        case R_CHATCLIENT_CHATVIEW_SUB_MENU:
            {


            aMenuPane->SetItemDimmed( EChatClientChatViewFavor,
                                      iGroupUtils->IsFavouriteChatGroup( *iGroupId ) );
            aMenuPane->SetItemDimmed( EChatClientChatViewRmv,
                                      ETrue );

            // branding things: edit group properties
            if ( ! IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_ALLOW_EDIT_GROUP_PROPERTIES ) )
                {
                // branded as inactive
                aMenuPane->SetItemDimmed( EChatClientChatViewEdit, ETrue );
                }
            else
                {
                // not branded
                aMenuPane->SetItemDimmed( EChatClientChatViewEdit,
                                          !iContainer->IsUserAdmin() );
                }
            break;
            }

        case R_CHATCLIENT_IBOX_FORWARD_SUBMENU:
            {
            // forward-menu funkiness
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
// CCAChatView::UpdateItemTypeL
// ---------------------------------------------------------
//
void CCAChatView::UpdateItemTypeL()
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
    item.iItemDescriptor = iContainer->SelectedItemL(); // takes the ownership

    // this logic comes from CItemFinder::ResolveAndSetItemTypeL.
    // should be in ItemFinder engine, but for some reason it isn't,
    // so, next few lines are copypasted from AknItemFinder.cpp..
    if ( item.iItemType == CItemFinder::EUrlAddress )
        {
        // old url types need prefix in order to work w/ schemehandler
        const TDesC& pref = item.iItemDescriptor->Des().Left( KHttpLength );
        if ( pref.CompareF( KIMHTTPPREFIX().Left( KHttpLength ) ) != 0 )
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
        const TDesC& pref = item.iItemDescriptor->Des().Left( KHttpLength );
        if ( pref.CompareF( KIMHTTPPREFIX().Left( KHttpLength ) ) == 0
             || pref.CompareF( KIMRTSP ) == 0 )
            {
            item.iItemType = CItemFinder::EUrlAddress;
            }
        }
    }


// ---------------------------------------------------------
// CCAChatView::LaunchInvite
// ---------------------------------------------------------
//
TInt CCAChatView::LaunchInvite( TAny *aInstance )
    {
    // launch the invite with query
    TRAPD(
        err, static_cast<CCAChatView*>( aInstance )->DoLaunchInviteL( ETrue ) );

    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CCAChatView::DoLaunchInviteL
// ---------------------------------------------------------
//
void CCAChatView::DoLaunchInviteL( TBool aShowQuery )
    {
    if ( iCba )
        {
        iCba->MakeVisible( ETrue );
        iCba->DrawNow();
        }

    if ( aShowQuery )
        {
        TInt retVal( IMDialogUtils::DisplayYesNoConfirmationDialogL(
                         R_QTN_CHAT_INVITE_FRIENDS_NOW ) );
        if ( retVal != EAknSoftkeyOk && retVal != EAknSoftkeyYes )
            {
            // user declined the query
            return;
            }
        }

    if ( UISessionManager().IsSupported( CCAUISessionManager::EInvite ) )
        {
        iContainer->InviteFriendsToGroupL();
        }
    else
        {
        // We don't need the returned keycode.
        IMDialogUtils::DisplayNoteDialogL( R_CHAT_ERROR_NOT_SUP_FUNCTIONS );
        }
    }


// ---------------------------------------------------------
// CCAChatView::HandleChatEvent
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatView::HandleChatEvent( const TDesC& aWvid ,
                                   const TDesC& aContactId  )
    {

    TRAPD( err, DoHandleChatEventL( aWvid, aContactId ) );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }

    }


// ---------------------------------------------------------
// CCAChatView::DoHandleChatEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatView::DoHandleChatEventL( const TDesC& aWvid ,
                                      const TDesC& aContactId  )
    {
    HBufC* tempSender = aWvid.AllocL();
    delete iSender;
    iSender = tempSender;

    // nickname or wv-less contact id


    iCleanSender.Set( aContactId );

    }

// ---------------------------------------------------------
// CCAChatView::SwitchTabL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatView::SwitchTabL( const TDesC& aWvId, const TInt aTabIndex )
    {
    //change the contact id based on tteh currenttab
    HBufC* group = aWvId.AllocL();
    delete iGroupId;
    iGroupId = group;

    iTabId = aTabIndex;

    CAAppUi()->CAStatusPane()->SetTitleL( CAAppUi()->CAStatusPane()->GetTabItemIdentification( iTabId ) );


    iGroupPC->SetActiveGroupChatL( aWvId );

    //remove the controls from view stack
    if ( iContainer )
        {
        CAAppUi()->RemoveFromViewStack( *this, iContainer );
        }

    if ( iAddAndReplyMenu )
        {
        CAAppUi()->RemoveFromViewStack( *this, iAddAndReplyMenu );
        }

    if ( iFindContextMenu )
        {
        CAAppUi()->RemoveFromViewStack( *this, iFindContextMenu );
        }


    iContainer->SwitchTabL();

    //again add those controls to view stack
    if ( iContainer )
        {
        CAAppUi()->AddToViewStackL( *this, iContainer );
        }

    if ( iAddAndReplyMenu )
        {
        CAAppUi()->AddToViewStackL( *this, iAddAndReplyMenu, ECoeStackPriorityDefault,
                                    ECoeStackFlagRefusesFocus );
        }

    if ( iFindContextMenu )
        {
        CAAppUi()->AddToStackL( *this, iFindContextMenu, ECoeStackPriorityDefault,
                                ECoeStackFlagRefusesFocus );
        }

    }


// ---------------------------------------------------------
// CCAChatView::HandleMessageError
// ---------------------------------------------------------
//
void CCAChatView::HandleMessageError( TInt aError,
                                      const TDesC& aNick )
    {
    TRAPD( err, DoHandleMessageErrorL( aError, aNick ) );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// ---------------------------------------------------------
// CCAChatView::DoHandleMessageErrorL
// ---------------------------------------------------------
//
void CCAChatView::DoHandleMessageErrorL( TInt aError,
                                         const TDesC& aNick )
    {
    if ( aError == KErrNoMemory ) // handled in AppUi::HandleMessageError
        {
        return;
        }
    else if ( aError == TEnumsPC::EContentPicture )
        {
        TInt res( aError == KErrOverflow ?
                  R_QTN_CHAT_SCALING_IMAGE_TOO_BIG :
                  R_QTN_CHAT_SCALING_ERROR );

        // sending of image failed
        IMDialogUtils::DisplayInformationNoteL( res );
        return;
        }

    CHAT_DP(
        D_CHAT_LIT( "CCAChatViewContainer::DoHandleMessageErrorL %d %S" ),
        aError,
        &aNick );

    IMNoteMapper::ShowNoteL( aError, aNick );


    }

// ---------------------------------------------------------
// CCAChatView::GetIfMenuPopup()
// ---------------------------------------------------------
//
TBool CCAChatView::GetIfMenuPopup()
    {
    TBool reValue = EFalse;
    if ( !iMenuPane )
        {
        reValue = EFalse;
        }
    else
        {
        reValue = iMenuPane->IsFocused();
        }
    return reValue;
    }
// End of File
