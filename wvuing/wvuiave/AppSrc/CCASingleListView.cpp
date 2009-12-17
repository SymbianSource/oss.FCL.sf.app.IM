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
* Description:  View class for single list
*
*/



//platform includes
#include	<chatNG.rsg>
#include	<eikmenup.h>
#include	<aknglobalnote.h>
#include	<stringloader.h>
#include    <eikmenub.h>    // CEikMenuBar
#include    <eiklbx.h>
#include    <hlplch.h>
#include    <FeatMgr.h>
#include    <SysUtil.h>


#include    "ImpsCSPAllErrors.h"
#include    "ccasyncchecker.h"
// INCLUDE FILES
#include	"MCAProcessManager.h"
#include	"MCASettingsPC.h"
#include	"MCAMainViewArrayPC.h"
#include	"chatngclient.hrh"
#include	"CCAStatusPaneHandler.h"
#include	"ChatDefinitions.h"
#include	"CCAAppUi.h"
#include	"CAExternalInterface.h"
#include	"IMDialogUtils.h"
#include	"IMNoteMapper.h"

#include    "CCAGroupUtils.h"
#include	"ChatDebugPrint.h"
#include    "CAExternalInterface.h"
#include	"ChatDebugPrint.h"
#include    "CCAUISessionManager.h"
#include    "CCAIdleWait.h"
#include    "IMUtils.h"
#include    "CCASearchViewContainer.h"
#include    "IMPrivateCRKeys.h"
#include    "IMVariant.hrh"
#include    "impsbuilddefinitions.h"
#include	"MCAUiLoginCmdCB.h"
#include	"CCASingleListContainer.h"
#include    "CCAUISessionManager.h"
#include	"CCASingleListView.h"
#include	"CCAUINGUtils.h"
#include    "MCALoginPC.h"
#include    "CAUtils.h"
#include    "MCASingleListViewCB.h"
#include 	"CCACommandManager.h"
#include    "CCACommandManagerFactory.h"

#include 	"AknDlgShut.h"
#include    "MCAApplicationNGPC.h"
#include 	"MCAInvitationPC.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// ================= MEMBER FUNCTIONS =======================

// Symbian OS default constructor can leave.
CCASingleListView::~CCASingleListView()
    {
    CHAT_DP_TXT( "CCASingleListView destructor starts" );
    if ( iContextMenuBar )
        {
        iEikonEnv->EikAppUi()->RemoveFromStack( iContextMenuBar );
        delete iContextMenuBar;
        }

    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }

    iMainViewArrayPC->UnRegisterCallBack( );
    iMainViewArrayPC->RemoveObserver();

    CHAT_DP_TXT( "CCASingleListView destructor ends" );
    }

// ---------------------------------------------------------
// CCAChatListView::ConstructL()
// Symbian OS default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListView::ConstructL( TInt aAvkonViewResourceId, TUid aViewId )
    {
    BaseConstructL( aAvkonViewResourceId, aViewId );

    iAppUi = static_cast<CCAAppUi*>( iEikonEnv->AppUi() );
    iMainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();
    iCurrentIndex = 0;
    iContainerIndexId = 0;
    iMainViewArrayPC->RegisterCallBack( this );
    iConversationExits = EFalse;
    iCreateGrpFromContactList = EFalse;
    iMainViewArrayPC->ResetArray( ETrue );
    iMainViewArrayPC->AddObserver( this );
    iFirstManualRefresh = EFalse;
    }


// ---------------------------------------------------------
// CCASingleListView::DoActivateL(
//   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
//   const TDesC8& /*aCustomMessage*/)
// From CAknView, Gets called from framework when activating this view
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/, TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    CHAT_DP_TXT( "CCASingleListView::DoActivateL" );
    CHAT_DP( D_CHAT_LIT( "Container: %d" ), iContainer );

    // Unregister the view that was registered from Forward option.
    // didn't find any better place to put this.
    if ( TEnumsPC::EToBeUnregistered == iAppUi->RetForwardTo() )
        {
        iAppUi->UnRegisterPreviousview( );
        iAppUi->SetResetForwardTo( TEnumsPC::EUnregistered );
        }

    if ( aCustomMessageId == KUidChatPrivateChatListViewSoftNoteActivationId )
        {
        // Shutdown possible dialogs before changing views etc.
        // If this is not done, wrong dialog get events.
        CHAT_DP_FUNC_DP( "DoActivateL", "ShutDialogsL..." );
        AknDialogShutter::ShutDialogsL( *iEikonEnv );
        CHAT_DP_FUNC_DP( "DoActivateL", "done." );

        // we might be here before chat has been fully constructed.
        CAAppUi()->FinalizeChatConstructionL();

        // Launched from soft notification -> Beep allready played
        // Set not to be played again
        CAAppUi()->SetPlayOnce( ETrue );

        TInt countOfInvites = CAAppUi()->ReceivedInvites();
        TInt countOfPendingChats;
        TInt countOfPendingGroupChats;

        TInt countOfMessages = iAppUi->GetProcessManager().GetApplicationNGPC()
                               ->MessagesPendingCount(	countOfPendingChats, TEnumsPC::EUnreadReceived );

        countOfMessages = iAppUi->GetProcessManager().GetApplicationNGPC()
                          ->ChatGroupMessagesPendingCount( countOfPendingGroupChats );

        if ( countOfInvites + countOfPendingChats +  countOfPendingGroupChats == 1 )
            {
            if ( countOfPendingChats )
                {

                // we have a sender, so this is p2p-message
                TCADnlConvView dnlConView;
                dnlConView.iIsForwarded = EFalse;
                dnlConView.iSAPChanged = EFalse;
                dnlConView.iWVID = iMainViewArrayPC->PendingMessageInfo();
                dnlConView.iSwitchTab = EFalse;

                TCADnlConvViewBuf convMsgBuf( dnlConView );
                // Chat was already there -> beep will not
                // be played again. Set to false so beep
                // will be played next time when new
                // msg is received
                CAAppUi()->SetPlayOnce( EFalse );
                CAViewSwitcher()->SwitchViewL( KUidConversationsView,
                                               KUidConvViewMsgId, convMsgBuf );

                return; // we're done with this view

                }
            else if ( countOfInvites )
                {
                const TDesC& invitationId = iAppUi->GetProcessManager().GetInvitationsInterface()
                                            ->UnresdInvitationInfo();

                MCAInvitationPC& invitePC( *iAppUi->GetProcessManager().GetInvitationsInterface() );

                TCADnlInvView dnlInvView;
                dnlInvView.iInvitationIndex = invitePC.FindInvitationIndex( invitationId );
                dnlInvView.iInviteID = invitationId;
                dnlInvView.iSwitchTab = EFalse;

                TCADnlInvViewBuf invMsgBuf( dnlInvView );

                CAViewSwitcher()->SwitchViewL( KUidInvitationView,
                                               KUidInvViewMsgId, invMsgBuf );
                return; // we're done with this view

                }
            else
                {
                TCADnlChatView viewDnl;
                viewDnl.iIsForwarded = EFalse;
                viewDnl.iSwitchTab = EFalse;
                viewDnl.iGroupId = iMainViewArrayPC->ChatGroupPendingMessageInfo( viewDnl.iGroupName );
                viewDnl.iInviteQuery = EFalse;

                TCADnlChatViewBuf grpMsgBuf( viewDnl );

                // Chat was already there -> beep will not
                // be played again. Set to false so beep
                // will be played next time when new
                // msg is received
                CAAppUi()->SetPlayOnce( EFalse );

                CAViewSwitcher()->SwitchViewL( KUidChatView,
                                               KUidChatViewMsgId, grpMsgBuf );

                return; // we're done with this view

                }
            }
        else if ( ( countOfInvites + countOfPendingChats +  countOfPendingGroupChats ) > 1 )
            {
            TInt itemIndex = iMainViewArrayPC->FindIndexOfOpenChatsItem(
                                 TEnumsPC::EOpenChatsListItem, KNullDesC );
            itemIndex = itemIndex + KOpenChatsIndex;
            // just to make sure
            if ( iMainViewArrayPC->GetType( itemIndex ) != TEnumsPC::EOpenChatsListItem )
                {
                User::Leave( KErrGeneral );
                }
            if ( !iContainer )
                {
                CreateContainerL( aCustomMessage, aCustomMessageId );
                }
            iContainer->SetListboxIndexL( itemIndex );

            if (	iMainViewArrayPC->IsCollapsed( itemIndex ) )
                {
                // collapse ,expand now
                iContainer->ExpandCollapseListL();
                }

            //Find the index of top most pending message,To high light that item.
            itemIndex = iMainViewArrayPC->GetIndexOfTopPendingMessage();

            // focus the current item by setting itemIndex
            iContainer->SetListboxIndexL( itemIndex );

            }
        }

    // Reuse the container if it exists.
    if ( !iContainer )
        {
        CreateContainerL( aCustomMessage, aCustomMessageId );
        }
    // show manual update note if not shown yet
    CCACommandManager* commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager();
    MCASessionHandlerCmd* sessHandlerCmd =  commandManager->GetSessionHandlerCmd() ;
    TBool contactsRefreshed( EFalse );
    if ( sessHandlerCmd )
        {
        contactsRefreshed = sessHandlerCmd->ContactsRefreshed();
        }
    if ( contactsRefreshed && !iAppUi->GetProcessManager().GetSettingsInterface()
         ->GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate , NULL ) )
        {
        // only if all contact are synch
        // and if manual update.
        ShowManualUpdateNoteL();
        }
    }


///----------------------------------------------------------
///This functiate will Instantiate iContainer, if it is NULL
///----------------------------------------------------------
void CCASingleListView::CreateContainerL( const TDesC8& aCustomMessage, TUid aCustomMessageId )
    {
    CAAppUi()->CAStatusPane()->SynchroniseAndRemoveTabGroup();

    iContainer = new ( ELeave ) CCASingleListContainer( *this, *this, iAppUi );
    iContainer->ConstructL( ClientRect(),
                            *CAViewSwitcher(), iContainerIndexId );
    AppUi()->AddToViewStackL( *this, iContainer );


    iContainer->SetMopParent( this );

    if ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
         & EIMFeatSimplifiedUI )
        {
        CCAUISessionManager& uisessionmanager = iAppUi->UISessionManager();
        if ( !uisessionmanager.IsServerDefined() )
            {
            HBufC* emptyTxtBuf = IMUtils::CombineStringFromResourceLC(
                                     R_QTN_CHAT_EMPTY_COMMUNITY_VIEW_PRIMARY,
                                     R_QTN_CHAT_EMPTY_COMMUNITY_VIEW );
            iContainer->ListBox()->View()->SetListEmptyTextL( *emptyTxtBuf );
            CleanupStack::PopAndDestroy( emptyTxtBuf );
            }
        else
            {
            HBufC* emptyTxtBuf = IMUtils::CombineStringFromResourceLC(
                                     R_QTN_CHAT_EMPTY_COMMUNITY_VIEW_NOT_VISIBLE_PRIMARY,
                                     R_QTN_CHAT_EMPTY_COMMUNITY_VIEW_NOT_VISIBLE );
            iContainer->ListBox()->View()->SetListEmptyTextL( *emptyTxtBuf );
            CleanupStack::PopAndDestroy( emptyTxtBuf );

            }
        }


    // Update navi-pane
    CAAppUi()->CAStatusPane()->ShowTabGroupL( KUidFriendsListView );
    CAAppUi()->CAStatusPane()->ClearNaviPaneL();
    UpdateTitleL();
    iContainer->ListBox()->UpdateScrollBarsL();
    // Logged in with a different userid,so focus owndata.
    TBool isNewLogIn = EFalse;
    MCALoginPC* loginPC = iAppUi->GetProcessManager().GetLogInInterface();

    TRAP_IGNORE( isNewLogIn = loginPC->FreshLoginL( ) )
    if ( isNewLogIn )
        {
        iCurrentIndex = 0;
        }

    //focus item as iCurrentIndex
    else
        {
        iCurrentIndex = iMainViewArrayPC->GetArrayItemIdIndexL( iItemDetails );
        }
    if ( KErrNotFound == iCurrentIndex )
        {
        iCurrentIndex = 0;
        }

    TCADnlConvInvGroupFocusBuf viewBuf;
    viewBuf.Copy( aCustomMessage );

    if ( aCustomMessageId == KConvInvGroupFocusId && KNullDesC() == viewBuf().iListID )
        {
        TBool collapsed = iMainViewArrayPC->IsCollapsed( KOpenChatsIndex );
        if ( collapsed )
            {
            iMainViewArrayPC->SetCollapsed( !collapsed, KOpenChatsIndex );
            }

        TEnumsPC::TItem itemType = TEnumsPC::EInvalid;
        switch ( viewBuf().iType )
            {
            case EConversationItem:
                {
                itemType = 	TEnumsPC::EConversationItem;
                break;
                }
            case EInviteItem:
                {
                itemType = 	TEnumsPC::EInviteItem;
                break;
                }
            case EGroupItem:
                {
                itemType = 	TEnumsPC::EGroupItem;
                break;
                }
            default://we should never be here.
                break;
            }

        iCurrentIndex =	iMainViewArrayPC->FindIndexOfOpenChatsItem(
                            itemType, viewBuf().iInvConvGroupID );
        iCurrentIndex += KOpenChatsIndex;
        iContainer->ListBox()->HandleItemAdditionL();
        }

    iContainer->SetListboxIndexL( iCurrentIndex );
    iItemDetails.aItem = TEnumsPC::EInvalid;
    iItemDetails.aItemID.Copy( KNullDesC );
    iItemDetails.aListID.Copy( KNullDesC );

    ConstructContextMenuBarL();
    }



// ---------------------------------------------------------
// CCASingleListView::DoDeactivate()
// From AknView, Gets called from framework when deactivating this view
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListView::DoDeactivate()
    {
    CHAT_DP_TXT( "CCASingleListView::DoDeactivate" );
    if ( iContextMenuBar )
        {
        iEikonEnv->EikAppUi()->RemoveFromStack( iContextMenuBar );
        delete iContextMenuBar;
        iContextMenuBar = NULL;
        }

    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );

        // Capture events because if they are not captured
        // and user is quick to write keys
        // when opening conversation. -> crash.
        if ( iContainer->OpeningConversation() )
            {
            CHAT_DP_TXT( "start capturing events" );
            TRAPD( err, CAAppUi()->CaptureEventsL() );
            CHAT_DP_TXT( "started capturing events.." );

            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            }

        if ( iContainer->OpeningConversation() )
            {
            // Stop capturing events.
            TRAPD( err, CAAppUi()->ReleaseCapturingL() );
            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            }
        if ( iCreateGrpFromContactList )
            {
            iCreateGrpFromContactList = EFalse;
            }

        // delete container safely.
        // if container has some ongoing task. it will complete the task first
        // and then delete itself. Owner is not responsible for deleting the
        // object after this call
        iContainer->DeleteSelfD();
        iConversationExits = EFalse;
        if ( iContainer && iContainer->IsBckGrdTaskComplete() )
            {
            iContainer = NULL;
            }

        }
    }
// ---------------------------------------------------------
// CCASingleListView::AddCreateNewSubmenuL()
// add "create new" sub menu or just one item
// ---------------------------------------------------------
//

void CCASingleListView::AddCreateNewSubmenuL( CEikMenuPane* aMenuPane )
    {
    if ( !UISessionManager().IsSupported( CCAUISessionManager::ESearch ) )
        {
        // search not supported => don't add submenu
        IMUtils::AddMenuItemL( ETrue, *aMenuPane, R_QTN_CHAT_FRIEND_OM_CRNEW,
                               EChatClientFriendsListViewCmdAddManually );
        return;
        }

    IMUtils::AddMenuItemL( ETrue, *aMenuPane, R_QTN_CHAT_FRIEND_OM_CRNEW, 0,
                           R_QTN_CHAT_FRIEND_OM_CRNEW_SUBMENU );
    }
// ---------------------------------------------------------
// CCASingleListView::AddOwnOptionsSubMenuL()
// add "Own options" sub menu depending on the online status
// ---------------------------------------------------------
//
void CCASingleListView::AddOwnOptionsSubMenuL( CEikMenuPane* aMenuPane )
    {
    TEnumsPC::TOnlineStatus onlineStatus = iContainer->GetOnlineStatus();
    if ( onlineStatus == TEnumsPC::EUnknown )
        {
        return;
        }
    else
        {
        IMUtils::AddMenuItemL( ETrue, *aMenuPane, R_QTN_CHAT_FRIEND_OWN_OPTIONS, 0,
                               R_QTN_CHAT_FRIEND_OWN_OPTIONS_SUBMENU );
        }
    }

// ---------------------------------------------------------
// CCASingleListView::AddContactEditOptionsSubMenuL()
// add "Edit options" sub menu when focus is on a contact
// ---------------------------------------------------------
//
void CCASingleListView::AddContactEditOptionsSubMenuL( CEikMenuPane* aMenuPane, TBool aIsContact )
    {

    IMUtils::AddMenuItemL( aIsContact, *aMenuPane, R_QTN_CHAT_FRIEND_EDIT_OPTIONS,
                           0, R_QTN_CHAT_FRIEND_EDIT_OPTIONS_SUBMENU );

    }

// ---------------------------------------------------------
// CCASingleListView::AddNewChatSubmenuL()
// add "Edit options" sub menu when focus is on a contact
// ---------------------------------------------------------
//
void CCASingleListView::AddNewChatSubmenuL( CEikMenuPane* aMenuPane )
    {
    IMUtils::AddMenuItemL( ETrue, *aMenuPane, R_QTN_CHAT_OPT_NEW_CHAT,
                           0, R_CHAT_FRIEND_NEW_CHAT_SUBMENU );

    }
// ---------------------------------------------------------
// CCASingleListView::DynCreateMenuPaneL()
// construct menu pane dynamically
// ---------------------------------------------------------
//
void CCASingleListView::DynCreateMenuPaneL( CEikMenuPane* aMenuPane )
    {
    TEnumsPC::TSyncStatus syncStatus  = TEnumsPC::ESyncSuccess;

    /* All these many boolean variables should be
    *  replaced with enums and there should be single function on
    *  PC side get give us status.*/
    TBool search( UISessionManager().IsSupported(
                      CCAUISessionManager::ESearch ) );
    TBool groups( UISessionManager().IsSupported( CCAUISessionManager::EGroup ) );
    TBool block ( UISessionManager().IsSupported( CCAUISessionManager::EBlock ) );

    TBool manualUpdate( !iAppUi->GetProcessManager().GetSettingsInterface()
                        ->GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate, NULL ) );

    // UI CR : 101-39731: Add variation to contact list handling

    /* Contactlist creation and deletion is
     * based upon flag "CHAT_VARIATION_CREATE_DELETE_CONTACT_LIST"
     * Contactlist renaming
     * based upon flag " CHAT_VARIATION_MODIFY_CONTACT_LIST" */

    TBool listCreateDelete( IMUtils::IntResourceValueL(
                                RSC_CHAT_VARIATION_CREATE_DELETE_CONTACT_LIST ) );  // UI CR : 101-39731
    TBool listEdit( IMUtils::IntResourceValueL(
                        RSC_CHAT_VARIATION_MODIFY_CONTACT_LIST ) );
    TBool findJoinedChats = IMUtils::IntResourceValueL(
                                RSC_CHAT_VARIATION_SEARCH_GROUPMEMBER );
    TBool contact( EFalse );

    TEnumsPC::TOnlineStatus status = iContainer->OwnStatus();

    TBool synchronised = ETrue;
    TBool failed = EFalse;

#ifdef IMPS_CONTACT_FETCH_BACKGROUND

    syncStatus = iMainViewArrayPC->IsAllSynchronised();

#endif //IMPS_CONTACT_FETCH_BACKGROUND

    TEnumsPC::TItem itemtype = iContainer->SelectedType();

    TBool ownStatusOffLine( status == TEnumsPC::EOffline );

    switch ( itemtype )
        {
        case TEnumsPC::EOwnStatusItem:
            {
            IMUtils::AddMenuItemL( ETrue, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_CHANGE_STATUS,
                                   EChatClientMainViewCmdChangeStatus );
            AddOwnOptionsSubMenuL( aMenuPane );

            /* Added to make  what are the option available in online should
            	* be present in offline also. Add create new menu with sub menu */
            AddNewChatSubmenuL( aMenuPane );

            /* User is not offline then only add this menu*/
            if ( !ownStatusOffLine )
                {
                //add fetch failed list  only if online state
                IMUtils::AddMenuItemL( ( syncStatus == TEnumsPC::ESyncFailed ),
                                       *aMenuPane, R_QTN_CHAT_CONTACTLIST_FETCH_ALL,
                                       EChatClientFriendsListViewCmdFetchFailed );
                }

            //-----------------------------------------------------------------------
            // This option is shown when the loging is sucessful
            // and also when the contactlist fetching is ongoing.
            AddCreateNewSubmenuL( aMenuPane );

            //------------------------------------------------------------------------
            // Some options are missing in the option menu when fetching of contact
            // list failed commented.
            IMUtils::AddMenuItemL( listCreateDelete, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_NEW_GROUP,
                                   EChatClientCmdNewList );
            if ( !UISessionManager().IsSupported( CCAUISessionManager::ECLIMod ) )
                {
                aMenuPane->SetItemDimmed( EChatClientCmdNewList, ETrue );
                TEnumsPC::TItem itemtype = iContainer->SelectedType();
                if ( itemtype == TEnumsPC::EContactListItem )
                    {
                    aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdDelete, ETrue );
                    }
                }
            //-----------------------------------------------------------------------
            //REFRESHLIST
            IMUtils::AddMenuItemL( synchronised && manualUpdate, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_OM_REFRESH,
                                   EChatClientFriendsListViewCmdRefresh );
            break;
            }
        case TEnumsPC::EContactListItem:
            {
            TBool collapsed( iContainer->IsCollapsed() );
            TBool forceExpanded( iMainViewArrayPC->IsForceExpanded() );
#ifdef IMPS_CONTACT_FETCH_BACKGROUND

            /* All these boolean variables should be replaced with enums.
             * and there should be single function on PC side give us sync
             * status or other relavent data*/
            synchronised = ( iMainViewArrayPC->Synchronised( iContainer->
                                                             CurrentListboxIndex() )  == TEnumsPC::ESyncSuccess );
            failed = ( iMainViewArrayPC->Synchronised( iContainer->
                                                       CurrentListboxIndex() ) == TEnumsPC::ESyncFailed );
#endif //IMPS_CONTACT_FETCH_BACKGROUND
            IMUtils::AddMenuItemL( ( collapsed && !forceExpanded ) && synchronised, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_EXPAND_GROUP,
                                   EChatClientCmdExpandList );

            IMUtils::AddMenuItemL( ( !collapsed || forceExpanded ) && synchronised, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_COLLAPSE_GROUP,
                                   EChatClientCmdCollapseList );

            IMUtils::AddMenuItemL( synchronised && manualUpdate, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_OM_REFRESH,
                                   EChatClientFriendsListViewCmdRefresh ) ;
            IMUtils::AddMenuItemL( ( syncStatus == TEnumsPC::ESyncFailed ),
                                   *aMenuPane,
                                   R_QTN_CHAT_CONTACTLIST_FETCH_ALL,
                                   EChatClientFriendsListViewCmdFetchFailed );

            //----------------------------------------------------------------------
            // Some options are missing in the option menu when fetching of contact
            // list failed. If the list in focus is synchronised then all the options
            // should be available.
            if ( synchronised )
                {
                IMUtils::AddMenuItemL( ETrue, *aMenuPane,
                                       R_QTN_CHAT_CONTACTS_OPEN_GROUP_CONV,
                                       EChatClientCmdOpenGroupConv );

                AddCreateNewSubmenuL( aMenuPane );
                IMUtils::AddMenuItemL( listCreateDelete, *aMenuPane,
                                       R_QTN_CHAT_FRIEND_NEW_GROUP,
                                       EChatClientCmdNewList );
                
                IMUtils::AddMenuItemL( listCreateDelete && ( synchronised || failed ), *aMenuPane, 
                                       R_QTN_CHAT_FRIEND_OM_DEL, 
                                       EChatClientFriendsListViewCmdDelete );
                
                if ( !UISessionManager().IsSupported( CCAUISessionManager::ECLIMod ) )
                    {
                    aMenuPane->SetItemDimmed( EChatClientCmdNewList, ETrue );
                    TEnumsPC::TItem itemtype = iContainer->SelectedType();
                    if ( itemtype == TEnumsPC::EContactListItem )
                        {
                        aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdDelete, ETrue );
                        }
                    }
                }
            IMUtils::AddMenuItemL( listEdit && synchronised, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_RENAME_GROUP,
                                   EChatClientCmdRenameList );
            break;
            }
        case TEnumsPC::EContactItem:
            {
            contact = ETrue;
            //open conversation
            IMUtils::AddMenuItemL( contact, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_OM_SENDI,
                                   EChatPrivateListCmdOpen );

            //REFRESHLIST
            IMUtils::AddMenuItemL( synchronised && manualUpdate, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_OM_REFRESH,
                                   EChatClientFriendsListViewCmdRefresh );
            IMUtils::AddMenuItemL( ( syncStatus == TEnumsPC::ESyncFailed ), *aMenuPane,
                                   R_QTN_CHAT_CONTACTLIST_FETCH_ALL,
                                   EChatClientFriendsListViewCmdFetchFailed );

            //EDITOPTIONS
            AddContactEditOptionsSubMenuL( aMenuPane, contact );

            IMUtils::AddMenuItemL( search && groups && findJoinedChats,
                                   *aMenuPane,
                                   R_QTN_CHAT_FRIEND_OM_FIND,
                                   EChatClientFriendsListViewCmdFindJoinedChats );
            //-------------------------------------------------------------------------
            // Some options are missing in the option menu when fetching of contact list
            // failed. Create New and New contact lists are available as default(if
            // the list synchronised)
            AddCreateNewSubmenuL( aMenuPane );

            IMUtils::AddMenuItemL( listCreateDelete, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_NEW_GROUP,
                                   EChatClientCmdNewList );
            if ( !UISessionManager().IsSupported( CCAUISessionManager::ECLIMod ) )
                {
                aMenuPane->SetItemDimmed( EChatClientCmdNewList, ETrue );
                TEnumsPC::TItem itemtype = iContainer->SelectedType();
                if ( itemtype == TEnumsPC::EContactListItem )
                    {
                    aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdDelete, ETrue );
                    }
                }
            break;
            }
        case TEnumsPC::EConversationItem:
            {
            TBool contactExistsInAnyList( EFalse );
            TEnumsPC::TItem itemtype = iContainer->SelectedType();
            TPtrC userid = iMainViewArrayPC->GetSelectedContactUserId(
                               iContainer->CurrentListboxIndex() );
            if ( iMainViewArrayPC->FindAnyContactL( userid ) )
                {
                contactExistsInAnyList = ETrue;
                }
            IMUtils::AddMenuItemL( ETrue, *aMenuPane,
                                   R_CHAT_MENUBAR_CONVERSATION_CHATLIST_MENU_OPEN,
                                   EChatPrivateListCmdOpen );

            IMUtils::AddMenuItemL( !contactExistsInAnyList, *aMenuPane,
                                   R_CHAT_MENUBAR_CONVERSATION_CHATLIST_MENU_ADDTOFRIENDS,
                                   EChatPrivateListCmdAddToFriends );

            IMUtils::AddMenuItemL( ETrue, *aMenuPane,
                                   R_CHAT_MENUBAR_CONVERSATION_CHATLIST_MENU_CLOSECONVERS,
                                   EChatPrivateListCmdCloseConvers );

            AddNewChatSubmenuL( aMenuPane ); //add create new menu with sub menu

            break;
            }
        case TEnumsPC::EOpenChatsListItem:
            {
            TBool collapsed( iContainer->IsCollapsed() );
            TBool forceExpanded( iContainer->IsChatsForceExpanded() );

            IMUtils::AddMenuItemL( collapsed && !forceExpanded, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_EXPAND_GROUP,
                                   EChatClientCmdExpandList );

            IMUtils::AddMenuItemL( !collapsed || forceExpanded, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_COLLAPSE_GROUP,
                                   EChatClientCmdCollapseList );

            AddNewChatSubmenuL( aMenuPane ); //add create new menu with sub menu

            break;
            }


        case TEnumsPC::EInviteItem:
            {
            TBool contactExistsInAnyList( EFalse );
            TEnumsPC::TItem itemtype = iContainer->SelectedType();
            TPtrC userid =
                iMainViewArrayPC->GetSelectedContactUserId( iContainer->CurrentListboxIndex() );
            if ( iMainViewArrayPC->FindAnyContactL( userid ) )
                {
                contactExistsInAnyList = ETrue;
                }
            IMUtils::AddMenuItemL( ETrue, *aMenuPane,
                                   R_CHAT_MENUBAR_CONVERSATION_CHATLIST_MENU_OPEN,
                                   EChatPrivateListCmdOpen );

            IMUtils::AddMenuItemL( !contactExistsInAnyList, *aMenuPane,
                                   R_CHAT_MENUBAR_CONVERSATION_CHATLIST_MENU_ADDTOFRIENDS,
                                   EChatPrivateListCmdAddToFriends );
            IMUtils::AddMenuItemL( ETrue, *aMenuPane,
                                   R_CHAT_MENUBAR_CONVERSATION_CHATLIST_MENU_DELETE,
                                   EChatPrivateListCmdDelete );

            AddNewChatSubmenuL( aMenuPane ); // add new chat and sub options

            break;
            }
        case TEnumsPC::EGroupItem:
            {
            TBool isJoined  = iContainer->IsJoinedToGroup();
            IMUtils::AddMenuItemL( !ownStatusOffLine && isJoined, *aMenuPane,
                                   R_QTN_CHAT_CLIST_OM_OPEN,
                                   EChatClientSingleListViewCmdGroupOpen );

            IMUtils::AddMenuItemL( !isJoined, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_JOIN_GROUP,
                                   EChatClientSingleListViewCmdGroupJoin );


            IMUtils::AddMenuItemL( ETrue, *aMenuPane,
                                   R_QTN_CHAT_CLIST_OM_CGR,
                                   0,
                                   R_CHAT_GROUP_OPTIONS_SUBMENU
                                 );


            IMUtils::AddMenuItemL( !ownStatusOffLine && isJoined, *aMenuPane,
                                   R_QTN_CHAT_CLIST_CGR_SM_LEAVE,
                                   EChatClientSingleListViewCmdGroupLeave );

            AddNewChatSubmenuL( aMenuPane ); // add new chat and sub options
            break;
            }
        default:
            {
            IMUtils::AddMenuItemL( contact, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_OM_SENDI,
                                   EChatClientFriendsListViewCmdIMessage );

#ifndef RD_REMOVE_COLLAPSE_OPTION
            IMUtils::AddMenuItemL( contact, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_COLLAPSE_GROUP,
                                   EChatClientCmdCollapseList );
#endif // RD_REMOVE_COLLAPSE_OPTION

            IMUtils::AddMenuItemL( contact, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_OM_OPEN1,
                                   EChatClientFriendsListViewCmdOpen );

            if ( !( ( status == TEnumsPC::EUnknown ) || ( status == TEnumsPC::EOffline ) ) )
                {
                AddCreateNewSubmenuL( aMenuPane );
                }

            AddContactEditOptionsSubMenuL( aMenuPane, contact );

            TInt index = iContainer->CurrentListboxIndex();
            IMUtils::AddMenuItemL( search && groups && findJoinedChats && index != KErrNotFound,
                                   *aMenuPane,
                                   R_QTN_CHAT_FRIEND_OM_FIND,
                                   EChatClientFriendsListViewCmdFindJoinedChats );

            IMUtils::AddMenuItemL( listCreateDelete, *aMenuPane,
                                   R_QTN_CHAT_FRIEND_NEW_GROUP,
                                   EChatClientCmdNewList );

            if ( !UISessionManager().IsSupported( CCAUISessionManager::ECLIMod ) )
                {
                aMenuPane->SetItemDimmed( EChatClientCmdNewList, ETrue );
                TEnumsPC::TItem itemtype = iContainer->SelectedType();
                if ( itemtype == TEnumsPC::EContactListItem )
                    {
                    aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdDelete, ETrue );
                    }
                }


            break;
            }
        }

    //block options
    IMUtils::AddMenuItemL( block, *aMenuPane, R_QTN_CHAT_FRIEND_OM_IMSG_SET, 0,
                           R_CHATCLIENT_FRIENDSLIST_SUBMENU );

    IMUtils::AddMenuItemL( ETrue, *aMenuPane, R_QTN_CHAT_FRIEND_SAVED_CONVERSATIONS,
                           EChatClientSingleListViewCmdSavedConversations );
    TBool loggedIn( UISessionManager().IsLoggedIn() );
    IMUtils::AddMenuItemL( loggedIn, *aMenuPane, R_QTN_CHAT_MAIN_OM_LOGOUT,
                           EChatClientMainViewCmdLogout );

    IMUtils::AddMenuItemL( !loggedIn, *aMenuPane, R_QTN_CHAT_MAIN_OM_LOGIN,
                           EChatClientMainViewCmdLogin );

    IMUtils::AddMenuItemL( ETrue, *aMenuPane, R_QTN_CHAT_SETTINGS,
                           EChatClientChatListSettings );
    }

// ---------------------------------------------------------
// CCASingleListView::DynInitMenuPaneL()
// From MEikMenuObserver, Called by framework before constructing menupane
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListView::DynInitMenuPaneL( TInt aResourceId,
                                          CEikMenuPane* aMenuPane )
    {
    TBool ownStatusOffLine( iContainer->OwnStatus() == TEnumsPC::EOffline );

    // least 2 contact lists
    TInt minContactLists( 2 );
    switch ( aResourceId )
        {
        case R_CHATCLIENT_GENERAL_MENU:
            {
            // dim the help menu item
            aMenuPane->SetItemDimmed( EChatClientCmdHelp,
                                      !FeatureManager::FeatureSupported( KFeatureIdHelp ) );
            break;
            }
        case R_CHATCLIENT_FRIENDSLIST_VIEW_MENU:
            {
            DynCreateMenuPaneL( aMenuPane );
            break;
            }
        case R_CHATCLIENT_FRIENDSLIST_SUBMENU://Blocklist
            {
            TBool nonContact( ( iContainer->SelectedType()
                                == TEnumsPC::EContactListItem )
                              || ( iContainer->SelectedType()
                                   == TEnumsPC::EGroupItem )
                              || ( iContainer->SelectedType()
                                   == TEnumsPC::EOwnStatusItem )
                              || ( iContainer->SelectedType()
                                   == TEnumsPC::EOpenChatsListItem )
                            );

            // show/hide "show blocked list" option
            if ( !IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_SHOW_BLOCKED_LIST ) )
                {
                aMenuPane->SetItemDimmed(
                    EChatClientFriendsListViewShowBlockedList, ETrue );
                }

            TBool showUnblock = ( ( iContainer->SelectedType() != TEnumsPC::EContactItem )
                                  || !iContainer->IsBlocked() );

            aMenuPane->SetItemDimmed( EChatClientFriendsListViewUnblock,
                                      showUnblock );

            TInt index = iContainer->CurrentListboxIndex();
            aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdBlock,
                                      iContainer->IsBlocked() || nonContact || ( index == KErrNotFound ) );
            break;
            }
        case R_CHATCLIENT_FRIENDSLIST_CRNEW_SUBMENU:
            {
            // check if search is supported
            // branding: check if there are any available search criteria
            if ( !UISessionManager().IsSupported( CCAUISessionManager::ESearch ) ||
                 ( !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_NAME ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_USERID ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_PHONE ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_EMAIL ) ) )
                {
                aMenuPane->SetItemDimmed(
                    EChatClientFriendsListViewCmdAddFromSrv, ETrue );
                aMenuPane->SetItemDimmed(
                    EChatClientFriendsListViewCmdAddFromPb, ETrue );
                }
            if ( !IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_MSISDN_SUPPORT ) )
                {
                aMenuPane->SetItemDimmed(
                    EChatClientFriendsListViewCmdAddFromPb, ETrue );
                }

            // this is not used when full contact list support is on
            aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdMoveFromSrv,
                                      ETrue );
            break;
            }

        case R_QTN_CHAT_FRIEND_OM_CRNEW_SUBMENU:
            {
            // check if search is supported
            // branding: check if there are any available search criteria
            if ( !UISessionManager().IsSupported( CCAUISessionManager::ESearch ) ||
                 ( !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_NAME ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_USERID ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_PHONE ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_EMAIL ) ) )
                {
                aMenuPane->SetItemDimmed(
                    EChatClientFriendsListViewCmdAddFromSrv, ETrue );
                aMenuPane->SetItemDimmed(
                    EChatClientFriendsListViewCmdAddFromPb, ETrue );
                }
            if ( !IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_MSISDN_SUPPORT ) )
                {
                aMenuPane->SetItemDimmed(
                    EChatClientFriendsListViewCmdAddFromPb, ETrue );
                }


            break;
            }

        case R_CHATCLIENT_FRIENDSLIST_EDIT_SUBMENU:
            {
            TBool manualUpdate( !iAppUi->GetProcessManager().GetSettingsInterface()
                                ->GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate , NULL ) );

            // if manual update is on, we should not have watch in menu
            if ( manualUpdate )
                {
                aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdRemoveWatch,
                                          ETrue );
                aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdAddWatch,
                                          ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdRemoveWatch,
                                          !iContainer->IsWatched() );
                aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdAddWatch,
                                          iContainer->IsWatched() );
                }

            // "move" option is not shown if there isn't at
            // least 2 contact lists
            TInt listCount = iMainViewArrayPC->ListCountL();
            aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdMove,
                                      listCount < minContactLists );
            break;
            }
        case R_QTN_CHAT_FRIEND_EDIT_OPTIONS_SUBMENU:
            {
            TBool manualUpdate( !iAppUi->GetProcessManager().GetSettingsInterface()
                                ->GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate , NULL ) );

            // if manual update is on, we should not have watch in menu
            if ( manualUpdate )
                {
                aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdRemoveWatch,
                                          ETrue );
                aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdAddWatch,
                                          ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdRemoveWatch,
                                          !iContainer->IsWatched() );
                aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdAddWatch,
                                          iContainer->IsWatched() );
                }

            // "move" option is not shown if there isn't at
            // least 2 contact lists
            TInt listCount = iMainViewArrayPC->ListCountL();

            aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdMove,
                                      listCount < minContactLists );
            break;
            }
        case R_QTN_CHAT_FRIEND_OWN_OPTIONS_SUBMENU:
            {
            TEnumsPC::TOnlineStatus onlineStatus = iContainer->GetOnlineStatus();
            // UI CR ID :101-39726: Status message variation :  start
            TBool ownStatusOnline( onlineStatus == TEnumsPC::EOnline );
            TBool ownStatusAway( onlineStatus == TEnumsPC::EAway );
            TBool ownStatusBusy( onlineStatus == TEnumsPC::EBusy );

            /*  The option "status Message"  is available depending upon different variation flags.
             *  e.g :If  "CHAT_VARIATION_ONLINE_OPTION " is supported then
             *  user has the option "status Message"  when User status is online, not present for
             *  other status.(busy ,away)
             *  In the same way it works for " away " and "busy " status, depending upon
             *  "RSC_CHAT_VARIATION_AWAY_OPTION"  and  "RSC_CHAT_VARIATION_BUSY_OPTION" flags  */

            TBool statusMsgOnline( IMUtils::IntResourceValueL(
                                       RSC_CHAT_VARIATION_ONLINE_OPTION ) );
            TBool statusMsgAway( IMUtils::IntResourceValueL(
                                     RSC_CHAT_VARIATION_AWAY_OPTION ) );
            TBool statusMsgBusy( IMUtils::IntResourceValueL(
                                     RSC_CHAT_VARIATION_BUSY_OPTION ) );

            TBool option(   ( ownStatusOnline && statusMsgOnline )
                            || ( ownStatusAway && statusMsgAway )
                            || ( ownStatusBusy && statusMsgBusy ) );

            if (    ( onlineStatus == TEnumsPC::EOffline )
                    || ( onlineStatus == TEnumsPC::EInvisible )
                    || ( !option )  )
                {
                aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdChangeMessage,
                                          ETrue );
                }
            break;
            // UI CR ID :101-39726: Status message variation :  End

            }
        case R_CHAT_FRIEND_NEW_CHAT_SUBMENU:
            {
            // check if search is supported for contact
            // branding: check if there are any available search criteria
            if ( !UISessionManager().IsSupported( CCAUISessionManager::ESearch ) ||
                 ( !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_NAME ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_USERID ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_PHONE ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_EMAIL ) ) )
                {
                aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdSearchContact,
                                          ETrue );
                }

            // for create group option
            // check if group is supported
            TBool isGroupSupported = UISessionManager().IsSupported( CCAUISessionManager::EGroup );
            if ( ! isGroupSupported )
                {
                aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdCreateGrp, ETrue );
                }

            // check if search is supported for group
            // branding: check if there are any available search criteria
            if ( !isGroupSupported ||
                 ( !UISessionManager().IsSupported( CCAUISessionManager::ESearch ) ) ||
                 ( !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_GROUPNAME ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_GROUPTOPIC ) &&
                   !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_GROUPMEMBER ) ) )
                {
                aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdSearchGrp, ETrue );
                }

            if ( !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_JOIN_CHAT_ROOM_MANUAL ) )
                {
                aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdEnterGrpID, ETrue );
                }
            break;
            }
        case  R_CHAT_GROUP_OPTIONS_SUBMENU:
            {
            TBool isJoined  = iContainer->IsJoinedToGroup();
            TBool isFavourite = iContainer->IsFavourite();
            TBool isOwner = iContainer->IsOwnGroup();
            TBool isAdminOrOwner = iContainer->IsUserAdmin();

            if ( !isJoined || !isAdminOrOwner )
                {
                aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdGroupEdit, ETrue );
                }
            aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdGroupFavor, ownStatusOffLine || isFavourite );
            aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdGroupRmv,
                                      ( ! ( ( isFavourite || isOwner ) && !isJoined ) ) );
            aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdGroupPart, ownStatusOffLine || !isJoined );
            aMenuPane->SetItemDimmed( EChatClientSingleListViewCmdGroupInfo, ownStatusOffLine || !isJoined );
            break;
            }
        default:
            {
            // nothing to do
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCASingleListView::HandleCommandL( TInt aCommand )
// From CAknView, takes care of command handling.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListView::HandleCommandL( TInt aCommand )
    {
    // If background login is underway, avoid all single list events.
    if ( iAppUi->UISessionManager().IsBackgroundTaskRunning() )
        return;

    // hide the context menu if active
    if ( iContextMenuBar->IsDisplayed() )
        {
        iContextMenuBar->StopDisplayingMenuBar();
        }

    //Store the contact pointer to local variable
    //TPtrC contact;
    TInt focusedIndex = iContainer->CurrentListboxIndex();
  	HBufC* focusedIdBuf = iMainViewArrayPC->GetSelectedContactUserId(focusedIndex).AllocLC();
  	TPtrC focusedId = focusedIdBuf->Des();

    iCurrentIndex = iContainer->CurrentListboxIndex();
    iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iCurrentIndex );

    switch ( aCommand )
        {
        case EChatClientSingleListViewCmdMove:
        case EChatClientSingleListViewCmdDelete:
        case EChatClientSingleListViewCmdChangeMessage:
        case EChatClientSingleListViewCmdDetails:
        case EChatClientSingleListViewCmdEdit:
        case EChatClientFriendsListViewCmdAddManually:
        case EChatClientFriendsListViewCmdMoveFromSrv:
        case EChatClientFriendsListViewCmdMove:
        case EChatClientFriendsListViewCmdBlock:
        case EChatClientFriendsListViewCmdBlockEnter:
        case EChatClientFriendsListViewCmdRefresh:
        case EChatClientFriendsListViewCmdFetchFailed:
        case EChatClientFriendsListViewShowBlockedList:
        case EChatClientFriendsListViewUnblock:
        case EChatClientFriendsListViewCmdFindJoinedChats:
        case EChatClientFriendsListViewCmdAddFromSrv:
        case EChatClientFriendsListViewCmdChangeList:
        case EChatClientCmdNewList:
        case EChatClientCmdRenameList:
        case EChatClientMainViewCmdChangeStatus:
        case EChatPrivateListCmdAddToFriends:
        case EChatPrivateListWriteNewConvers:
        case EChatPrivateListCmdSelectRecipient:
        case EChatClientFriendsListViewCmdAddFromPb:
        case EChatClientSingleListViewCmdGroupOpen:
        case EChatClientSingleListViewCmdGroupJoin:
        case EChatClientSingleListViewCmdGroupLeave:
        case EChatClientSingleListViewCmdGroupRmv:
        case EChatClientSingleListViewCmdGroupPart:
        case EChatClientSingleListViewCmdGroupInfo:
        case EChatClientSingleListViewCmdGroupFavor:
        case EChatClientSingleListViewCmdGroupEdit:
        case EChatClientSingleListViewCmdCreateGrp:
        case EChatClientSingleListViewCmdSearchGrp:
        case EChatClientSingleListViewCmdSearchContact:
        case EChatClientCmdOpenGroupConv:
        case EChatClientFriendsListViewCmdEdit:
        case EChatClientSingleListViewCmdOwnEdit:
            {
            if ( !UISessionManager().LoginL( MCAUiLoginCmdCB::EQueryLogin ) )
                {
                CleanupStack::PopAndDestroy(focusedIdBuf);
				
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
        case EChatClientSingleListViewCmdMove:
        case EChatClientSingleListViewCmdDelete:
        case EChatClientSingleListViewCmdEdit:
        case EChatClientFriendsListViewCmdAddManually:
        case EChatClientFriendsListViewCmdMoveFromSrv:
        case EChatClientFriendsListViewCmdMove:
        case EChatClientFriendsListViewCmdRefresh:
            //case EChatClientFriendsListViewCmdAddFromSrv:
        case EChatClientCmdNewList:
        case EChatClientCmdRenameList:
        case EChatClientFriendsListViewCmdDelete:
        case EChatClientFriendsListViewCmdAddFromPb:
        case EChatClientFriendsListViewCmdEdit:
        case EChatPrivateListCmdAddToFriends:
            //case EChatPrivateListWriteNewConvers:
        case EChatPrivateListCmdSelectRecipient:
            //case EChatClientSingleListViewCmdSearchContact:
        case EChatClientSingleListViewCmdCreateGrp:
            //joing to a group does not require any
            //synchronization with the contact list.

        case EChatClientFriendsListViewShowBlockedList:

            //case EChatClientSingleListViewCmdSearchGrp:
            //case EChatClientSingleListViewCmdGroupOpen:
            //case EChatClientSingleListViewCmdGroupJoin:
            //case EChatClientSingleListViewCmdGroupLeave:
        case EChatClientSingleListViewCmdGroupRmv:
        case EChatClientSingleListViewCmdGroupPart:
        case EChatClientSingleListViewCmdGroupInfo:
        case EChatClientSingleListViewCmdGroupFavor:
        case EChatClientSingleListViewCmdGroupEdit:
        case EChatClientCmdOpenGroupConv:

            {
            if ( !CCASyncChecker::CheckSyncStateL() )
                {
                CleanupStack::PopAndDestroy(focusedIdBuf);
			    
                return;
                }
            break;

            }
        default:
            {
            break;
            }
        }
#endif //IMPS_CONTACT_FETCH_BACKGROUND

    switch ( aCommand )
        {
        case EChatClientCmdExpandList:  // flowthrough
        case EChatClientCmdCollapseList:
            {
            iContainer->ExpandCollapseListL();
            break;
            }
        case EChatClientCmdNewList:
            {
            TInt index = KErrNotFound;
            TInt err = CCAUINGUtils::NewContactListL( index );
            if ( index != KErrNotFound )
                {
                iContainer->UpdateViewL( index, TEnumsPC::EContactListItem, ETrue );
                }
            break;
            }
        case EChatClientCmdRenameList:
            {
            iContainer->RenameListL();
            break;
            }

        case EChatClientCmdContextMenu:
            {
            iContextMenuBar->TryDisplayMenuBarL();
            break;
            }
        case EChatClientFriendsListViewCmdDelete:
            {
            iContainer->DeleteFriendL();
            break;
            }
        case EChatClientFriendsListViewCmdOpen:
            {
            iContainer->ShowDetailsL();
            UpdateTitleL();
            break;
            }

        case EChatClientFriendsListViewCmdAddWatch:
            {
            TPtrC userid( iContainer->SelectedUserId() );
            CHAT_DP( D_CHAT_LIT(
                         "CCASingleListView -> addwatcher to %S" ), &userid );
            iMainViewArrayPC->AddWatcherL( userid );
            iContainer->RefreshView( EFalse );
            break;
            }

        case EChatClientFriendsListViewCmdRemoveWatch:
            {
            TPtrC userid( iContainer->SelectedUserId() );
            CHAT_DP( D_CHAT_LIT(
                         "CCASingleListView -> removewatcher from %S" ), &userid );
            iMainViewArrayPC->RemoveWatcherL( userid );
            iContainer->RefreshView( EFalse );
            break;
            }

        case EChatClientFriendsListViewCmdAddManually:
            {
            iContainer->SetCbaLockL( ETrue );
            TInt indexofcontact( iContainer->EditFriendL( TEnumsPC::ECreateNew ) );

            if ( iContainer )
                {
                // if the container still exists, update the view
                if ( UISessionManager().IsLoggedIn() )
                    {
                    iContainer->UpdateViewL( indexofcontact, TEnumsPC::EContactItem, ETrue );
                    iContainer->SetCbaLockL( EFalse );
                    UpdateTitleL();
                    }
                }
            break;
            }

        case EChatClientFriendsListViewCmdAddFromPb:
            {
            if ( UISessionManager().IsSupported( CCAUISessionManager::ESearch ) )
                {
                // Check disk space before creating new contact,
                // needed to do allready here in this case
                IMUtils::FFSSpaceBelowCriticalLevelL( KContactSizeEstimate );

                iContainer->ContactSelectionFromPhoneBookL();
                }
            else
                {
                IMDialogUtils::DisplayNoteDialogL(
                    R_CHAT_ERROR_NOT_SUP_FUNCTIONS );
                }
            break;
            }

        case EChatClientFriendsListViewCmdMove:
            {
            iContainer->MoveContactL();
            break;
            }

        case EChatClientFriendsListViewCmdEdit:
            {
            TInt indexofcontact( iContainer->EditFriendL( TEnumsPC::EEditContact ) );
            if ( iContainer )
                {
                // if the container still exists, update the view
                iContainer->UpdateViewL( indexofcontact, TEnumsPC::EContactItem, ETrue );
                UpdateTitleL();
                }
            break;
            }

        case EChatClientFriendsListViewCmdIMessage:
            {
            iContainer->OpenConversationL();
            break;
            }

        case EChatClientFriendsListViewCmdBlock:
            {
            iContainer->BlockUserL();
            break;
            }
        case EChatClientFriendsListViewCmdBlockEnter:
            {
            iContainer->DisplayBlockTextQueryL();
            break;
            }
        case EChatClientFriendsListViewCmdRefresh:
            {
            // this is manually selected
            iContainer->RefreshPresenceL( ETrue );
            break;
            }
        case EChatClientFriendsListViewCmdFetchFailed:
            {
            if ( TEnumsPC::ESyncFailed == iMainViewArrayPC->Synchronised( GetCurrentItemIndex( ) ) )
                {
                TInt ret( IMDialogUtils::DisplayYesNoConfirmationDialogL(
                              R_QTN_CHAT_CONTACTLIST_FETCHAGAIN ) );
                if ( !( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) ) )
                    {
                    break;
                    }

                }

            iMainViewArrayPC->FetchContactsL();
            break;
            }
        case EChatClientFriendsListViewShowBlockedList:
            {
            iContainer->DisplayBlockedListL();
            break;
            }
        case EChatClientFriendsListViewUnblock:
            {
            iContainer->DisplayUnblockListQueryL();
            break;
            }
        case EChatClientFriendsListViewCmdFindJoinedChats:
            {
            // check if the groups are supported
            if ( IsFunctionSupported( CCAUISessionManager::EGroup ) )
                {
                iContainer->FindJoinedChatsL();
                }
            break;
            }

        case EChatClientMainViewCmdLogin:
            {
            iAppUi->UISessionManager().LoginL( MCAUiLoginCmdCB::ELogin );
            break;
            }

        case EChatClientMainViewCmdLogout:
            {
            iMainViewArrayPC->AddObserver( this );
            AppUi()->HandleCommandL( EChatClientMainViewCmdLogout );
            SetCurrentItemIndexL();
            break;
            }
        case EChatClientChatListSettings:
            {
            AppUi()->HandleCommandL( EChatClientChatListSettings );
            // Force refresh the CBA!
            iContainer->UpdateCbaL();
            break;
            }
        case EAknSoftkeyOpen:
            // case EChatPrivateListCmdOpen:
            {
            if ( iMainViewArrayPC->GetType( GetCurrentItemIndex() ) == TEnumsPC::EInviteItem )
                {
                iContainer->OpenInvitationL();
                }

            break;
            }
        case EChatPrivateListCmdDelete:
            {
            iContainer->DeleteInvitationL();
            break;
            }
        case EChatPrivateListCmdAddToFriends:
            {
            iContainer->SetCbaLockL( ETrue );
            TEnumsPC::TItem itemType = iContainer->SelectedType();
            TInt prevIndex( iContainer->EditFriendL( TEnumsPC::EAddToContact ) );

            if ( iContainer )
                {
                // if the container still exists, update the view
                if ( UISessionManager().IsLoggedIn() )
                    {
                    iContainer->UpdateViewL( prevIndex, itemType, ETrue );
                    iContainer->SetCbaLockL( EFalse );
                    UpdateTitleL();
                    }
                }
            break;
            }
        // Flow-through
        case EAknSoftkeyExit:
        case EChatClientCmdExit:
            {
            CHAT_DP( D_CHAT_LIT( " CCASingleListView -> BACK command start" ) );

            if ( iAlreadyTrytoExit )
                {
                break;
                }
            iAlreadyTrytoExit = ETrue;

            TRAPD( err, AppUi()->HandleCommandL( EAknSoftkeyExit ) );

            iAlreadyTrytoExit = EFalse;
            User::LeaveIfError( err );

            // getcalled whenever the exit RSk presesd
            // it delete the findbox so that no more operation will be called
            // for iFindBox, because it was giving panic in FocusChaged of container
            if ( iAppUi->IsQueryAccepted() )
                {
                iContainer->PrepareDestruction();
                }
            CHAT_DP( D_CHAT_LIT( " CCASingleListView -> BACK command end" ) );
            break;
            }
        case EChatClientFriendsListViewCmdAddFromSrv:
        case EChatClientSingleListViewCmdSearchContact:
            {
            // check the init pending show note and then process
            CompletePendingTaskForSearchL() ;

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
        case EChatPrivateListCmdOpen:
        case EChatClientFriendsListViewCmdChat:
            {
            //TPtrC item = iContainer->SelectedItem();
            TEnumsPC::TItem itemtype = iContainer->SelectedType();
            if ( itemtype == TEnumsPC::EContactItem
                 || itemtype == TEnumsPC::EConversationItem )
                {
                // Don't try to open conversation if selected item is
                // not a contact
                iContainer->OpenConversationL();
                }

            else if ( itemtype == TEnumsPC::EInviteItem )
                {
                iContainer->OpenInvitationL();
                }
            break;
            }
        case EChatPrivateListCmdCloseConvers:
            {
            iContainer->CloseConversationL();
            break;
            }
        case EChatPrivateListWriteNewConvers:
            {
            iContainer->DisplayRecipientTextQueryL();
            break;
            }
        case EChatPrivateListCmdSelectRecipient:
            {
            iContainer->DisplayRecipientListQueryL();
            break;
            }

        case EChatClientSingleListViewCmdSearchGrp:
            {
            if ( UISessionManager().IsSupported( CCAUISessionManager::ESearch ) )
                {
                // Check disk space before creating new contact,
                // needed to do allready here in this case
                IMUtils::FFSSpaceBelowCriticalLevelL( KContactSizeEstimate );

                if ( CCASearchViewContainer::DisplaySearchCriteriaDlgL(
                         CCASearchViewContainer::ESearchGroups ) )
                    {
                    CAViewSwitcher()->SwitchViewL( KUidSearchView,
                                                   KUidSearchViewMsgIdGroup,
                                                   KNullDesC8 );
                    }
                }
            else
                {
                IMDialogUtils::DisplayNoteDialogL(
                    R_CHAT_ERROR_NOT_SUP_FUNCTIONS );
                }
            break;
            }
        case EChatClientSingleListViewCmdEnterGrpID :
            {
            HBufC* data = HBufC::NewLC( KMaxChatGroupLenght );
            TPtr dataPtr( data->Des() );
            iContainer->DisplayRecipientTextQueryForGrpIDL( dataPtr );
            CleanupStack::PopAndDestroy( data );
            break;
            }
        case EChatClientSingleListViewCmdEdit:
            {
            TInt indexofcontact( iContainer->EditFriendL( TEnumsPC::EEditContact ) );
            if ( iContainer )
                {
                // if the container still exists, update the view
                if ( UISessionManager().IsLoggedIn() )
                    {
                    iContainer->UpdateViewL( indexofcontact, TEnumsPC::EContactItem, ETrue );
                    }
                // No matter login or not, title should be updated
                UpdateTitleL();
                }
            break;
            }
        case EChatClientSingleListViewCmdDetails:
            {
            iContainer->ShowDetailsL();
            UpdateTitleL();
            break;
            }
        case EChatClientSingleListViewCmdChangeMessage:
            {
            AppUi()->HandleCommandL( EChatClientCmdStatusMessage );
            break;
            }

        case EChatClientSingleListViewCmdOwnEdit:
            {
            TInt indexofcontact( iContainer->EditFriendL( TEnumsPC::EEditContact ) );
            if ( iContainer )
                {
                // if the container still exists, update the view
                iContainer->UpdateViewL( indexofcontact, TEnumsPC::EOwnStatusItem, ETrue );
                UpdateTitleL();
                }
            }
        case EChatClientSingleListViewCmdDelete:
            {
            iContainer->DeleteFriendL();
            break;
            }
        case EChatClientSingleListViewCmdMove:
            {
            if ( iContainer->IsWatched() )
            	{
            	HBufC* userid = iContainer->SelectedUserId().AllocLC();
            	iContainer->MoveContactL();
    			iMainViewArrayPC->AddWatcherL( userid->Des() );
    			CleanupStack::PopAndDestroy( userid );
    			iContainer->RefreshView( EFalse );
            	}
            else
            	{
            	iContainer->MoveContactL();
            	}
            break;
            }
        case EChatClientSingleListViewCmdSavedConversations: //Recorded Chat list view
            {
            CAViewSwitcher()->SwitchViewL( KUidRecordedChatListView );
            break;
            }
        //Group options should be shown only if group feature is supported
        case EChatClientSingleListViewCmdCreateGrp: //Group Chat view
            {
            //iCurrentIndex = iContainer->CurrentListboxIndex();
            //iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iCurrentIndex );
            if ( IsFunctionSupported( CCAUISessionManager::EGroup ) )
                {
                HBufC* groupId = iContainer->CreateNewChatL();
                CleanupStack::PushL( groupId );
                CleanupStack::PopAndDestroy( groupId );
                }
            break;
            }
        case EChatClientCmdOpenGroupConv:
            {
            //iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iContainer->CurrentListboxIndex() );
            //iCreateGrpFromContactList = ETrue;
            if ( IsFunctionSupported( CCAUISessionManager::EGroup ) )
                {
                HBufC* groupId = iContainer->CreateNewChatL();
                CleanupStack::PushL( groupId );
                CleanupStack::PopAndDestroy( groupId );
                }
            break;
            }
        // Options for when focus is on group chat case EAknSoftkeyOpen:
        // This case can be merged with the case for invitation item (EChatPrivateListCmdOpen)
        case EChatClientSingleListViewCmdGroupOpen: // Flowthrough
        case EChatClientSingleListViewCmdGroupJoin:
            {
            //iCurrentIndex = iContainer->CurrentListboxIndex();
            //iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iCurrentIndex );

            // When we have already tried to join a group, just get out
            if ( iAlreadyTrytoJoinGroup )
        		break;

            // Try to join a group
            iAlreadyTrytoJoinGroup = ETrue;

            HBufC* groupId = focusedId.AllocLC();
            TRAPD( err, iContainer->JoinGroupL( *groupId ) );

            // Finish join the group
            iAlreadyTrytoJoinGroup = EFalse;

            CleanupStack::PopAndDestroy( groupId );
            User::LeaveIfError( err );

            break;
            }
        case EChatClientSingleListViewCmdGroupLeave:
            {
            TInt currentIndex = iContainer->CurrentListboxIndex();
            TInt indexOfList = iMainViewArrayPC->IndexOfList( currentIndex - 1 );
            TInt countOfContactsInList = iMainViewArrayPC->CountOfContactsInList( indexOfList );

            TCAItemDetails itemDetails = iMainViewArrayPC->GetArrayItemIdDetails( currentIndex );
            iContainer->LeaveGroupL();

            if ( !iAppUi->GroupUtils()->IsOwnGroup( itemDetails.aItemID )
                 && !iAppUi->GroupUtils()->IsFavouriteChatGroup( itemDetails.aItemID ) )
                {
                if ( indexOfList + countOfContactsInList  == currentIndex  )
                    {
                    --currentIndex;
                    }
                iContainer->SetListboxIndexL( currentIndex );
                }
            break;
            }
        case EChatClientSingleListViewCmdGroupRmv:
            {
            //iCurrentIndex = iContainer->CurrentListboxIndex();
            //iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iCurrentIndex );
            iAlreadyTrytoJoinGroup = ETrue;
            iContainer->DeleteGroupL( focusedId );
            iAlreadyTrytoJoinGroup = EFalse;
            break;
            }
        case EChatClientSingleListViewCmdGroupPart:
            {
            //iCurrentIndex = iContainer->CurrentListboxIndex();
            //iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iCurrentIndex );
            iContainer->DisplayJoinedMembersL();
            break;
            }
        case EChatClientSingleListViewCmdGroupInfo:
            {
            //iCurrentIndex = iContainer->CurrentListboxIndex();
            //iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iCurrentIndex );
            iContainer->DisplayChatInfoL();
            break;
            }
        case EChatClientSingleListViewCmdGroupFavor:
            {
            //iCurrentIndex = iContainer->CurrentListboxIndex();
            //iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iCurrentIndex );
            iContainer->SaveAsFavouriteL( );
            break;
            }
        case EChatClientSingleListViewCmdGroupEdit:
            {
            //iCurrentIndex = iContainer->CurrentListboxIndex();
            //iItemDetails = iMainViewArrayPC->GetArrayItemIdDetails( iCurrentIndex );
            iContainer->EditChatGroupPropertiesL();
            UpdateTitleL();
            break;
            }
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    CleanupStack::PopAndDestroy( focusedIdBuf );
    }

// ---------------------------------------------------------
// CCASingleListView::InvokeCommandL
// ---------------------------------------------------------
//
void CCASingleListView::InvokeCommandL( TInt aCommand )
    {
    HandleCommandL( aCommand );
    }

// ---------------------------------------------------------
// CCASingleListView::UpdateTitleL
// ---------------------------------------------------------
//
void CCASingleListView::UpdateTitleL()
    {
    CAAppUi()->CAStatusPane()->SetTitleL( iEikonEnv,
                                          R_CHATCLIENT_MAIN_VIEW_TITLE );
    }
// ---------------------------------------------------------
// CCASingleListView::ConstructContextMenuBarL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListView::ConstructContextMenuBarL()
    {
    if ( iContextMenuBar )
        {
        iEikonEnv->EikAppUi()->RemoveFromStack( iContextMenuBar );
        delete iContextMenuBar;
        iContextMenuBar = NULL;
        }
    TEnumsPC::TItem itemtype = iContainer->SelectedType();

    CEikMenuBar* menuBar = new ( ELeave ) CEikMenuBar();
    CleanupStack::PushL( menuBar );
    menuBar->ConstructL( this, NULL,
                         R_CHATCLIENT_FRIENDSLIST_MENUBAR_CONTEXT );
    iEikonEnv->EikAppUi()->AddToStackL( menuBar, ECoeStackPriorityMenu,
                                        ECoeStackFlagRefusesFocus );
    iContextMenuBar = menuBar;
    CleanupStack::Pop( menuBar );
    }

// ---------------------------------------------------------
// CCASingleListView::HandleError
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListView::HandleError( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT( "CCASingleListView::HandleError, aError = %d" ),
             aError );
    if ( ( aError > Imps_ERROR_BASE ) && ( aError < KErrNone ) )
        {
        // propagate system errors to current active scheduler,
        // it should show a note
        CActiveScheduler::Current()->Error( aError );
        }

    // imps errors are ignored at the moment as we don't have any
    // notes specified for them
    }

// ---------------------------------------------------------
// CCASingleListView::GetCurrentItemIndex
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt  CCASingleListView::GetCurrentItemIndex( )
    {
    if ( iContainer )
        {
        return iContainer->CurrentListboxIndex();
        }

    return iCurrentIndex;
    }

// ---------------------------------------------------------
// CCASingleListView::SetCurrentItemIndex
// (other items were commented in a header).
// ---------------------------------------------------------
//
void  CCASingleListView::SetCurrentItemIndexL( TBool aFreshLogin /* = EFalse */ )
    {
    // After the logout the focus should go to owndata
    // if it was on contactlist/contactitem
    TInt currentIndex = GetCurrentItemIndex();
    TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( currentIndex );
    if ( TEnumsPC::EInvalid == itemType  || aFreshLogin )
        {
        currentIndex = 0;
        }
    if ( iContainer )
        {
        iContainer->SetListboxIndexL( currentIndex );
        }
    else
        {
        iCurrentIndex = 0;
        }

    }

// ---------------------------------------------------------
// CCASingleListView::GetCurrentItemIndex
// (other items were commented in a header).
// ---------------------------------------------------------
//
void  CCASingleListView::NotifySyncCompleted( )
    {
    CCACommandManager* commandManager( NULL );
    TRAPD( err, commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager() );
    if ( err == KErrNone && commandManager )
        {
        MCASessionHandlerCmd* sessHandlerCmd =  commandManager->GetSessionHandlerCmd() ;
        TBool contactsRefreshed( EFalse );
        if ( sessHandlerCmd )
            {
            contactsRefreshed = sessHandlerCmd->ContactsRefreshed();
            }

        if ( !contactsRefreshed && !iAppUi->GetProcessManager().GetSettingsInterface()
             ->GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate , NULL ) )
            {
            TRAP_IGNORE(
                iMainViewArrayPC->RefreshFriendsL();
                // If something failed, we are going to ignore as in case of automatic mode
                if ( sessHandlerCmd )
            {
            sessHandlerCmd->SetContactsRefreshed( ETrue );
                }
            // show the manual update note if user in same view
            if ( iContainer && !iFirstManualRefresh )
            {
            ShowManualUpdateNoteL();
                }
            );
            }
        }
    iMainViewArrayPC->RemoveObserver( );
    }
// ---------------------------------------------------------
// CCASingleListView::HandleInitChange
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListView::HandleListInitChange( TBool aCompleted )
    {
    CHAT_DP_TXT( "CCASingleListView::HandleListInitChange started" );

    if ( aCompleted ) //list init  is complete
        {

        // hide wait note
        if ( iWaitVisible )
            {
            CHAT_DP_TXT( "CCASingleListView::HandleListInitChange - iListInitProgress" );
            CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
            //CS warning ignored
            TRAPD( err, appUi->DismissWaitDialogL( KErrNone ) );

            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            iWaitVisible = EFalse;
            }
        //CS warning ignored
        TRAP_IGNORE(  iMainViewArrayPC->UnRegisterObserverL();

                      if ( !iWaitVisible )
        {
        if ( !CCASyncChecker::CheckSyncStateL() )
                {
                return;
                }
            if ( UISessionManager().IsSupported( CCAUISessionManager::ESearch ) )
                {
                // Check disk space before creating new contact,
                // needed to do allready here in this case
                IMUtils::FFSSpaceBelowCriticalLevelL( KContactSizeEstimate );

                if ( CCASearchViewContainer::DisplaySearchCriteriaDlgL(
                         CCASearchViewContainer::ESearchUsers ) )
                    {
                    CAViewSwitcher()->SwitchViewL( KUidSearchView,
                                                   KUidSearchViewMsgIdUser,
                                                   KNullDesC8 );
                    }

                }
            else
                {
                IMDialogUtils::DisplayNoteDialogL(
                    R_CHAT_ERROR_NOT_SUP_FUNCTIONS );
                }
            }
                   );
        }
    CHAT_DP_TXT( "ASingleListView::HandleListInitChange completed" );
    //check waiting then stop
    }
// ---------------------------------------------------------
// CCASingleListView::HandleInitChange
// (other items were commented in a header).
// ---------------------------------------------------------
TBool CCASingleListView::CompletePendingTaskForSearchL()
    {
    CHAT_DP_TXT( "CCASingleListView::CompletePendingTaskForSearchL started" );

    TBool result = EFalse;

    // show the wait note if we have pending list initialization

    if ( iMainViewArrayPC->IsListInitializationCompletedL() )
        {
        iMainViewArrayPC->RegisterObserverL( this );
        // init the list
        result = InitializeListL();

        }
    else
        {
        if ( !CCASyncChecker::CheckSyncStateL() )
            {
            return EFalse;
            }
        if ( UISessionManager().IsSupported( CCAUISessionManager::ESearch ) )
            {
            // Check disk space before creating new contact,
            // needed to do allready here in this case
            IMUtils::FFSSpaceBelowCriticalLevelL( KContactSizeEstimate );

            if ( CCASearchViewContainer::DisplaySearchCriteriaDlgL(
                     CCASearchViewContainer::ESearchUsers ) )
                {
                CAViewSwitcher()->SwitchViewL( KUidSearchView,
                                               KUidSearchViewMsgIdUser,
                                               KNullDesC8 );
                }

            }
        else
            {
            IMDialogUtils::DisplayNoteDialogL(
                R_CHAT_ERROR_NOT_SUP_FUNCTIONS );
            }

        CHAT_DP_TXT( "CCASingleListView::ListInitL done" );

        }

    CHAT_DP_TXT( "CCASingleListView::CompletePendingTaskForSearchL completed" );

    return result;

    }


// -----------------------------------------------------------------------------
// CCASessionHandler::GroupSyncL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASingleListView::InitializeListL()
    {

    CHAT_DP_TXT( "CCASingleListView::InitializeListL started" );

    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );

    if ( !iWaitVisible )
        {
        CHAT_DP_TXT(
            "CCASingleListView::InitializeListL - ! iWaitVisible " );

        // launch the wait note

        iWaitVisible = ETrue;

        appUi->ShowWaitDialogL( R_QTN_CHAT_SEARCHVIEW_PROCESSING, ETrue );

        CHAT_DP_TXT( "CCASingleListView::InitializeListL done" );

        }
    // nothing interesting, do nothing, but come back here
    CHAT_DP_TXT( "CCASingleListView::InitializeListL done" );

    return ETrue;
    }
// -----------------------------------------------------------------------------
// CCASingleListView::ShowManualUpdateNoteL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void  CCASingleListView::ShowManualUpdateNoteL()
    {
    if ( !iFirstManualRefresh )
        {
        iFirstManualRefresh = ETrue;
        // manual presence update and user didn't start this, show info note
        HBufC* text = iEikonEnv->AllocReadResourceLC(
                          R_QTN_CHAT_MANUAL_UPDATE_ACT );
        IMDialogUtils::DisplayNoteDialogL( *text, R_AVKON_SOFTKEYS_OK_EMPTY__OK,
                                           CAknNoteDialog::ENoTimeout );
        CleanupStack::PopAndDestroy( text );

        }
    }

// End of File

