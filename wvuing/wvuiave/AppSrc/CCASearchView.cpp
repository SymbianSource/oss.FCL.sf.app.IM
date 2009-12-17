/*
* Copyright (c) 2002-2005,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  View class for search view
*
*/


// INCLUDE FILES
#include    "CCASearchView.h"
#include    "CCASearchViewContainer.h"
#include    "CAExternalInterface.h"
#include    "chatngclient.hrh"
#include    "CCAAppUi.h"
#include    "CCAStatusPanehandler.h"
#include	"IMDialogUtils.h"

#include    "CCAUISessionManager.h"
#include    "ChatDebugPrint.h"
#include    "IMUtils.h"
#include    <aknviewappui.h>
#include    <stringloader.h>
#include    <eikmenup.h>
#include	<eikmenub.h>
#include    <eiklbx.h>
#include    <chatNG.rsg>
#include	<bldvariant.hrh>

#include    <hlplch.h>
#include    <FeatMgr.h>

// local variation
#include    "IMPrivateCRKeys.h"
#include    "IMVariant.hrh"
#include    "MCAProcessManager.h"
#include	"MCASearchInterfacePC.h"
#include    "TEnumsPC.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CCASearchView::CCASearchView()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CCASearchView::CCASearchView()

    {

    iSearchInterfacePC = CAAppUi()->GetProcessManager().GetSearchInterface();
    // want all events for group synchronization
    CHAT_DP_TXT(
        "CCASearchView::CCASearchView - Registering background observer" );

    iSearchInterfacePC->RegisterBackgroundObserver( this,
                                                    TEnumsPC::EGroupFetch, ~0x0 );

    // only fire once. set to true when the group fetch finishes


    TInt status( iSearchInterfacePC->BackgroundTaskStatus(
                     TEnumsPC::EGroupFetch ) );

    CHAT_DP( D_CHAT_LIT( "group fetch status %d" ), status );

    // if background task doesn't exist, mark group sync as done
    iGroupSyncDone = ( status == KErrNotFound );

    CHAT_DP_TXT( "CCASearchView::CCASearchView - Registered background \
                 observer, everything good" );
    }



// ---------------------------------------------------------
// CCASearchView::~CCASearchView()
// C++ Destructor
// ---------------------------------------------------------
//
CCASearchView::~CCASearchView()
    {
    if ( iSearchInterfacePC )
        {
        iSearchInterfacePC->UnRegisterBackgroundObserver( this );
        }

    if ( iPopUpMenu )
        {
        AppUi()->RemoveFromViewStack( *this, iPopUpMenu );
        }
    delete iPopUpMenu;

    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        iContainer->DeleteSelfD();
        }

    }


// ---------------------------------------------------------
// CCASearchView::HandleCommandL( TInt aCommand )
// This class all commands from menu
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchView::HandleCommandL( TInt aCommand )
    {
    CHAT_DP( D_CHAT_LIT( "SearchView: HandleCommand( %d )" ), aCommand );
    if ( iPopUpMenu && iPopUpMenu->IsDisplayed() )
        {
        iPopUpMenu->StopDisplayingMenuBar();
        }

    switch ( aCommand )
        {
        case EChatClientSearchNew:
        case EChatClientSearchMore:
        case EChatClientSearchPrev:
        case EChatClientSearchStartConversation:
        case EChatClientSearchAddFriends:
        case EChatClientSearchInvite:
        case EChatClientSearchBlock:
        case EChatClientSearchBlockEnter:
        case EChatClientSearchBlockShow:
        case EChatClientSearchJoin:
            {
            UISessionManager().LoginL( MCAUiLoginCmdCB::EQueryLogin );
            if ( !UISessionManager().IsLoggedIn() )
                {
                // if still not logged in just return
                return;
                }
            break;
            }
        default:
            {
            break;
            }
        }

    switch ( aCommand )
        {
            //Next two commands have same function
        case EAknSoftkeyBack:
        case EChatClientCmdBack:
            {
            iContainer->SetObserverToNull();
            CAAppUi()->SwitchViewL( iPrevViewId.iViewUid );
            break;
            }
        case EChatClientSearchNew:
            {
            iContainer->NewSearchL();
            break;
            }
        case EChatClientSearchMore:
            {
            TRAPD( searchMoreErr, iContainer->SearchMoreL() );
            if ( searchMoreErr == KErrNoMemory
                 || searchMoreErr == KErrDiskFull )
                {
                User::Leave( searchMoreErr );
                }
            if ( searchMoreErr != KErrNone )
                {
                CActiveScheduler::Current()->Error( searchMoreErr );
                }
            break;
            }
        case EChatClientSearchPrev:
            {
            TRAPD( searchPrevErr, iContainer->SearchPreviousL() );
            if ( searchPrevErr == KErrNoMemory
                 || searchPrevErr == KErrDiskFull )
                {
                User::Leave( searchPrevErr );
                }
            if ( searchPrevErr != KErrNone )
                {
                CActiveScheduler::Current()->Error( searchPrevErr );
                }
            break;
            }

        case EChatClientSearchStartConversation:
            {
            iContainer->StartConversationL();
            break;
            }

        case EChatClientSearchAddFriends:
            {
            iContainer->AddToFriendsL();
            break;
            }

        case EChatClientSearchInvite:
            {
            if ( UISessionManager().IsSupported( CCAUISessionManager::EInvite ) )
                {
                iContainer->InviteL();
                }
            else
                {
                IMDialogUtils::DisplayNoteDialogL(
                    R_CHAT_ERROR_NOT_SUP_FUNCTIONS );
                }
            break;
            }

        case EChatClientSearchBlock:
            {
            iContainer->BlockUserL();
            break;
            }
        case EChatClientSearchBlockEnter:
            {
            iContainer->BlockUserWVIdL();
            break;
            }
        case EChatClientSearchBlockShow:
            {
            iContainer->DisplayBlockedListL();
            break;
            }
        case EChatClientSearchJoin:
            {
            iContainer->JoinToGroupL();
            break;
            }
        case EChatClientSearchFavor:
            {
            iContainer->SaveAsFavouriteL();
            break;
            }
        case EChatClientSearchEdit:
            {
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
        case EAknSoftkeyContextOptions: // MSK command show context menu
            {
            ShowPopUpMenuL();
            break;
            }
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCASearcView::DynInitMenuPaneL()
// Initialises menu pane when needed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchView::DynInitMenuPaneL( TInt aResourceId,
                                      CEikMenuPane* aMenuPane )
    {

    TInt curIndex( iContainer->ListBox()->CurrentItemIndex() );

    if ( aResourceId == R_CHATCLIENT_SEARCH_VIEW_MENU ||
         aResourceId == R_CHATCLIENT_SEARCH_VIEW_CONTEXT_MENU )
        {

        if ( iContainer->SearchType()
             == CCASearchViewContainer::ESearchGroups || curIndex < 0 )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchAddFriends, ETrue );
            aMenuPane->SetItemDimmed(
                EChatClientSearchStartConversation, ETrue );
            aMenuPane->SetItemDimmed( EChatClientSearchInvite, ETrue );
            }
        else
            if ( !iContainer->IsOnline()
                 && !( IMUtils::IntResourceValueL(
                           RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE_NOTIFY )
                       || IMUtils::IntResourceValueL(
                           RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE ) ) )
                {
                aMenuPane->SetItemDimmed(
                    EChatClientSearchStartConversation, ETrue );
                aMenuPane->SetItemDimmed( EChatClientSearchInvite, ETrue );
                }

        TInt joinedGroups( iSearchInterfacePC->JoinedGroups() );
        CHAT_DP( D_CHAT_LIT( "Number of joined groups: %d" ), joinedGroups );
        if (  joinedGroups == 0 )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchInvite, ETrue );
            }

        if ( iContainer->SearchType() == CCASearchViewContainer::ESearchUsers ||
             curIndex < 0 )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchJoin, ETrue );
            }

        if ( iContainer->SearchType() == CCASearchViewContainer::ESearchUsers ||
             curIndex < 0 || iContainer->IsFavouriteChatGroup() )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchFavor, ETrue );
            }


        if ( iContainer->SearchType() == CCASearchViewContainer::ESearchUsers
             && iSearchInterfacePC->FindAnyContactL( iContainer->SearchData() ) )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchAddFriends, ETrue );
            }

        }

    // Block submenu
    if ( ( aResourceId == R_CHATCLIENT_SEARCH_BLOCK_SUBMENU ) &&
         ( iContainer->SearchType() == CCASearchViewContainer::ESearchUsers ) )
        {
        if ( iContainer->IsBlocked() )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchBlock, ETrue );
            }
        // show/hide "show blocked list" option
        if ( !IMUtils::IntResourceValueL(
                 RSC_CHAT_VARIATION_SHOW_BLOCKED_LIST ) )
            {
            aMenuPane->SetItemDimmed(
                EChatClientSearchBlockShow, ETrue );
            }
        }

    // Items only in Context specific menu
    if ( aResourceId == R_CHATCLIENT_SEARCH_VIEW_CONTEXT_MENU )
        {
        // put them here
        }

    // Items only in Options menu
    if ( aResourceId == R_CHATCLIENT_SEARCH_VIEW_MENU )
        {
        if ( curIndex < 0 || !iContainer->PrevResultsAvailable() )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchPrev, ETrue );
            }

        if ( curIndex < 0 || !iContainer->MoreResultsAvailable() )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchMore, ETrue );
            }

        // check if user block/unblock is supported according to variation
        if ( !UISessionManager().IsSupported( CCAUISessionManager::EBlock ) )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchBlockOpt, ETrue );
            }
        if ( iContainer->SearchType()
             == CCASearchViewContainer::ESearchGroups || curIndex < 0 )
            {
            aMenuPane->SetItemDimmed( EChatClientSearchBlockOpt, ETrue );
            }
        }
    // if we're displaying general menu and help feature is not supported..
    if ( ( aResourceId == R_CHATCLIENT_GENERAL_MENU ) &&
         ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) ) )
        {
        // dimm the help menu item
        aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
        }
    }


// ---------------------------------------------------------
// CCASearchView::ShowPopUpMenuL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchView::ShowPopUpMenuL()
    {

#ifdef	RD_30_DISABLE_TOUCH
    // In 3.0 we need to launch context menu by ourself
    iPopUpMenu->TryDisplayMenuBarL();
#else
    MenuBar()->TryDisplayContextMenuBarL();
#endif //RD_30_DISABLE_TOUCH   

    }


// ---------------------------------------------------------
// CCASearchView::ShowRecipientsListL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASearchView::ShowRecipientsListL( TInt /*aResourceId*/ )
    {
    return User::LeaveIfError( -1 );
    }


// ---------------------------------------------------------
// CCASearchView::DoActivateL()
// Activates view
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchView::DoActivateL( const TVwsViewId& aPrevViewId,
                                 TUid aCustomMessageId, const TDesC8& /* aCustomMessage*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchView::DoActivateL( viewid=%d" ),
             aPrevViewId.iViewUid );

    // Unregister the view that was registered from Forward option.
    // didn't find any better place to put this.
    if ( TEnumsPC::EToBeUnregistered == CAAppUi()->RetForwardTo() )
        {
        CAAppUi()->UnRegisterPreviousview( );
        CAAppUi()->SetResetForwardTo( TEnumsPC::EUnregistered );
        }
    // To prevent looping between this view and ...
    // - refresh view
    // - conversation view
    if ( aPrevViewId.iViewUid != KUidRefreshView &&
         aPrevViewId.iViewUid != KUidConversationsView &&
         aPrevViewId.iViewUid != KUidChatView &&
         aPrevViewId.iViewUid != KUidInvitationView )
        {
        iPrevViewId = aPrevViewId;
        }

    CAAppUi()->CAStatusPane()->ClearNaviPaneL();

    if ( !iContainer )
        {
        TBool users( aCustomMessageId == KUidSearchViewMsgIdUser );

        CHAT_DP( D_CHAT_LIT(
                     "CCASearchView::DoActivateL constructing container" ) );
        iContainer = CCASearchViewContainer::NewL( iSearchInterfacePC, ClientRect(),
                                                   *CAViewSwitcher(), *this, users, iGroupSyncDone,
                                                   KNullDesC );

        iContainer->SetMopParent( this );

        AppUi()->AddToViewStackL( *this, iContainer );
        }

    // SimplifiedUI localisation variation for empty list
    if ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey ) &
         EIMFeatSimplifiedUI )
        {
        iContainer->SetEmptyTextsToListboxL();
        }

    // Context sensitive menu
#ifndef RD_30_DISABLE_TOUCH
    // S60 Common
    MenuBar()->SetContextMenuTitleResourceId( R_SEARCH_VIEW_POPUP_MENU_BAR );
#else
    // S60 3.0
    if ( !iPopUpMenu )
        {
        iPopUpMenu = new( ELeave ) CEikMenuBar;
        iPopUpMenu->ConstructL( this, 0, R_SEARCH_VIEW_POPUP_MENU_BAR );
        iEikonEnv->EikAppUi()->AddToStackL( iPopUpMenu, ECoeStackPriorityMenu,
                                            ECoeStackFlagRefusesFocus );
        }

    CHAT_DP( D_CHAT_LIT( "CCASearchView::DoActivateL ends" ) );
#endif //RD_30_DISABLE_TOUCH

    }

// ---------------------------------------------------------
// CCASearchView::DoDeactivate()
// Deactivates the view
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchView::DoDeactivate()
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchView::DoDeactivate" ) );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    iContainer->DeleteSelfD();
    iContainer = NULL;


    if ( iPopUpMenu )
        {
        AppUi()->RemoveFromViewStack( *this, iPopUpMenu );
        }
    delete iPopUpMenu;
    iPopUpMenu = NULL;
    }


// ---------------------------------------------------------
// CCASearchView::HandleBackgroundEventPC()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchView::HandleBackgroundEventPC(
    TEnumsPC::TCABackgroundTasks aEventSource,
    TEnumsPC::TCABackgroundStatus aStatus,
    TInt  /*aSubTaskNumber*/,
    TInt  /*aLeaveCode*/ )
    {
    CHAT_DP_FUNC_ENTER( "HandleBackgroundEvent" );

    if ( iGroupSyncDone )
        {
        // we're not interested in this event anymore, but apparently
        // we can't unregister it inside here
        return;
        }

    // Currently we are interested only group fetching events
    if ( aEventSource == TEnumsPC::EGroupFetch )
        {
        CHAT_DP_FUNC_ENTER( "HandleBackgroundEvent - EGroupFetch" );
        if ( aStatus == TEnumsPC::ECompleted
             || aStatus == TEnumsPC::EFailedStatus )
            {
            CHAT_DP_TXT( "CCASearchView::HandleBackgroundEvent - \
                         Got ECompleted/EFailed, nuke wait note" );

            iGroupSyncDone = ETrue;

            if ( iContainer )
                {
                iContainer->SetGroupSyncDone();
                }
            }
        }

    CHAT_DP_FUNC_DONE( "HandleBackgroundEvent" );
    }

//  End of File
