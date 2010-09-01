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
* Description:  View class for private chat list view
*
*/


// INCLUDE FILES
#include	"CCAAppUi.h"
#include	"CCAStatusPaneHandler.h"
#include    "chatngclient.hrh"
#include    "ccainvitationview.h"
#include    "ccainvitationcontainer.h"
#include    "caexternalinterface.h"
#include    "ChatDebugPrint.h"
#include    "CCAUISessionManager.h"
#include    "MCABlockingPC.h"
#include    "CCABlockingUI.h"
#include    "MCAInvitationPC.h"
#include    "MCAProcessManager.h"

#include    "IMDialogUtils.h"
#include    "IMUtils.h"
#include    "CCATextQuerydialog.h"
#include    "CCAGroupUtils.h"
#include	"MCAUiLoginCmdCB.h"
#include    "MCALoginPC.h"

// local variation
#include    "IMPrivateCRKeys.h"
#include    "IMVariant.hrh"
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

#include	"CCASyncChecker.h"
#include    <barsread.h>
#include	<bldvariant.hrh>
#include    <eikmenup.h>
#include    <chatNG.rsg>
#include    <hlplch.h>
#include    <FeatMgr.h>
#include    <AknDlgShut.h>
#include    <StringLoader.h>
#include	<aknglobalnote.h>
#include	<stringloader.h>
#include    <eiklbx.h>
#include    <SysUtil.h>

#include	<csxhelp/imng.hlp.hrh>

// ================= MEMBER FUNCTIONS =======================



// -----------------------------------------------------------------------------
// CCAInvitationView::CCAInvitationView()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAInvitationView::CCAInvitationView() : iChatJoinRejectFlag( EFalse )
    {
    }


// Destructor
CCAInvitationView::~CCAInvitationView()
    {
    if ( iContainer && iAppUi )
        {
        iAppUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }

    delete iBlockingUI;
    delete iAsyncCallBack;
    delete iIdentification;

    }
// -----------------------------------------------------------------------------
// CCAInvitationView::ConstructL()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

void CCAInvitationView::ConstructL( TInt aAvkonViewResourceId,
                                    TUid aViewId )
    {
    BaseConstructL( aAvkonViewResourceId, aViewId );

    iAppUi = static_cast<CCAAppUi*>( CEikonEnv::Static()->AppUi() );

    MCABlockingPC* blockingPC = iAppUi->GetProcessManager().GetBlockingInterface();

    iInvitationPC =  iAppUi->GetProcessManager().GetInvitationsInterface();
    iBlockingUI = CCABlockingUI::NewL( blockingPC );
    iRejectReason = iInvitationPC->RejectReason();
    iTitlePane = iAppUi->CAStatusPane();
    }

// ---------------------------------------------------------
// CCAInvitationView::HandleCommandL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationView::HandleCommandL( TInt aCommand )
    {
    HBufC* screenName = iAppUi->ScreenNamePtr();
    TPtr screenNamePtr = screenName->Des();
    TPtr rejectReasonPtr = iRejectReason->Des();
    if ( iChatJoinRejectFlag )
        {
        return;
        }
    switch ( aCommand )
        {
            // These cases require, that we are logged in
        case EChatInvitationDlgCmdJoin:
        case EChatInvitationDlgCmdReject:
        case EChatClientFriendsListViewCmdBlock:
        case EChatClientFriendsListViewCmdBlockEnter:
        case EChatClientFriendsListViewShowBlockedList:
        case EChatClientFriendsListViewUnblock:
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
            // These cases require, that we are logged in
            //case EChatInvitationDlgCmdJoin:
            //case EChatInvitationDlgCmdReject:
        case EChatClientFriendsListViewCmdBlock:
        case EChatClientFriendsListViewCmdBlockEnter:
        case EChatClientFriendsListViewShowBlockedList:
        case EChatClientFriendsListViewUnblock:
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
#endif //IMPS_CONTACT_FETCH_BACKGROUND   
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EChatClientCmdBack:
            {
            if ( !iContainer->IsInvitationOpen() )
                {
                if ( iTitlePane )
                    {
                    iTitlePane->SynchroniseAndRemoveTabGroup();
                    }
                TCADnlConvInvGroupFocus dnlConvInvGroupFocus;
                dnlConvInvGroupFocus.iInvConvGroupID.Copy( iAppUi->GetProcessManager().GetInvitationsInterface()->GetInviteID( iInvitationIndex ) );
                dnlConvInvGroupFocus.iListID.Copy( KNullDesC() );
                dnlConvInvGroupFocus.iType = EInviteItem;
                TCADnlConvInvGroupFocusBuf convInvGroupFocusBuf( dnlConvInvGroupFocus );
                iAppUi->SwitchViewBackL( KUidFriendsListView, KConvInvGroupFocusId, convInvGroupFocusBuf );
                }
            break;
            }
        case EChatInvitationDlgCmdJoin:
            {
            // ask the screen name
            TInt result( EAknSoftkeyOk );
            result = iAppUi->GroupUtils()->ScreenNameQueryL( screenNamePtr );

            if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes )  )
                {
                iChatJoinRejectFlag = ETrue;
                iContainer->JoinToGroupL( iInvitationIndex );
                iChatJoinRejectFlag = EFalse;
                }

            break;
            }

        case EChatInvitationDlgCmdReject:
            {
            // ask the reason
            // CodeScanner warning ignored because CS does not
            // notice that PrepareLC puts the dialog to cleanupstack
            CCATextQueryDialog* dlg = CCATextQueryDialog::NewL( rejectReasonPtr ); // CSI: 35 # See comment above
            dlg->PrepareLC( R_CHATCLIENT_REJECT_REASON_QUERY );
            dlg->SetTitleL( R_QTN_CHAT_INVIT_REJECT_REASON );
            dlg->SetPredictiveTextInputPermitted( ETrue ); // T9
            TInt result( dlg->RunLD() );

            if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes )  )
                {
                iChatJoinRejectFlag = ETrue;
                iContainer->RejectInvitationL( iInvitationIndex, rejectReasonPtr );
                iChatJoinRejectFlag = EFalse;
                if ( iTitlePane )
                    {
                    iTitlePane->SynchroniseAndRemoveTabGroup();
                    }
                iAppUi->SwitchViewBackL( KUidFriendsListView );
                }

            break;
            }
        case EKeyDelete:
        case EChatInvitationDlgCmdDelete:
            {
            // ask the confirmation
            HBufC* prompt =
                iEikonEnv->AllocReadResourceLC( R_CHAT_CONV_LIST_INV_DEL );
            CAknQueryDialog* dlg = new( ELeave )CAknQueryDialog();
            CleanupStack::PushL( dlg );
            dlg->SetPromptL( prompt->Des() );
            CleanupStack::Pop( dlg );
            TInt ret( dlg->ExecuteLD( R_CLOSE_COVERSATION_QUERY ) );
            CleanupStack::PopAndDestroy( prompt );

            // should be deleted
            if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
                {
                iChatJoinRejectFlag = ETrue;
                iContainer->DeleteInvitationL( iInvitationIndex );
                iChatJoinRejectFlag = EFalse;
                if ( iTitlePane )
                    {
                    iTitlePane->SynchroniseAndRemoveTabGroup();
                    }
                iAppUi->SwitchViewBackL( KUidFriendsListView );
                }

            break;
            }

        case EChatClientFriendsListViewCmdBlock:
            {
            iBlockingUI->BlockUserL( iInvitationPC->InvitationUserIDL( /*iInvitationIndex */ 0 ) );
            break;
            }

        case EChatClientFriendsListViewCmdBlockEnter:
            {
            iBlockingUI->DisplayBlockTextQueryL();
            break;
            }

        case EChatClientFriendsListViewShowBlockedList:
            {
            iBlockingUI->DisplayBlockedListL();
            break;
            }

        case EChatClientFriendsListViewUnblock:
            {
            iBlockingUI->DisplayUnblockListQueryL();
            break;
            }

        case EChatClientCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( CEikonEnv::Static()->WsSession(),
                                                 ( CCoeEnv::Static()->AppUi() )->AppHelpContextL () );
            break;
            }
        case EAknCmdExit:           // fall-through, handled similarily
        case EEikCmdExit:           // fall-through, handled similarily
        case EAknSoftkeyExit:       // fall-through, handled similarily
        case EChatClientCmdExit:
            {
            iAppUi->HandleCommandL( EAknSoftkeyExit );
            break;
            }
        case EAknSoftkeyOk:
            {
            // Show context menu with selection key
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
// CCAInvitationView::DoActivateL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationView::DoActivateL( const TVwsViewId& /* aPrevViewId */,
                                     TUid aCustomMessageId,
                                     const TDesC8& aCustomMessage )
    {
    CHAT_DP( D_CHAT_LIT( "CCAInvitationView::DoActivateL, customMessageId:\
                          %d" ), aCustomMessageId );
    // Unregister the view that was registered from Forward option.
    // didn't find any better place to put this.
    if ( TEnumsPC::EToBeUnregistered == iAppUi->RetForwardTo() )
        {
        iAppUi->UnRegisterPreviousview( );
        iAppUi->SetResetForwardTo( TEnumsPC::EUnregistered );
        }
    iChatJoinRejectFlag = EFalse;
    iInvitationPC->RegisterObserver( this );

    TBool activateFromTabSwitch = EFalse;
    if ( aCustomMessageId == KUidInvViewMsgId )
        {
        TCADnlInvViewBuf viewBuf;
        viewBuf.Copy( aCustomMessage.Left( viewBuf.MaxLength() ) );

        activateFromTabSwitch = viewBuf().iSwitchTab;
        if ( activateFromTabSwitch )
            {
            iInvitationIndex = iInvitationPC->FindInvitationIndex( viewBuf().iInviteID );
            iTabId = viewBuf().iTabId;
            }
        else
            {
            iInvitationIndex = viewBuf().iInvitationIndex;
            if ( iTitlePane )
                {
                //hide navi pane decorators
                iTitlePane->ClearNaviPaneL();

                iTabId = iTitlePane->ShowTabGroupL( TEnumsPC::EInviteItem,
                                                    viewBuf().iInviteID );
                }
            }
        }
    iIdentification = iInvitationPC->InvitationUserIDL( iInvitationIndex ).AllocL();


    if ( !iInvitationPC->IsInvitationUnread( iInvitationIndex ) )
        {
        iInvitationPC->SetInvitationAsReadL( iInvitationIndex );
        }

    // Reuse the container if it exists.
    iContainer = CCAInvitationContainer::NewL( ClientRect(),
                                               iInvitationIndex,
                                               iRejectReason->Des(),
                                               *CAViewSwitcher(),
                                               *this, iTabId,
                                               iContextFlag,
                                               *this );
                                               
    CEikButtonGroupContainer   *cba   =   CEikButtonGroupContainer::Current();
    cba->MakeCommandVisible( EAknSoftkeyOptions, EFalse ); 
    cba->MakeCommandVisible( EAknSoftkeyBack, EFalse );
    iContainer->SetMopParent( this );
    if ( iTitlePane )
        {
        iTitlePane->SetTitleL( iEikonEnv,
                               R_QTN_CHAT_INVITATION_TITLE );
        }
    iAppUi->SetAfterRefreshMode( ETrue );

    iInvitationPC->SetActiveInvitation( iInvitationIndex );

    if ( iTitlePane )
        {
        iTitlePane->AddObserver( this ) ;
        }
    iContainer->PrepareTextL( iInvitationIndex );
    cba->MakeCommandVisible( EAknSoftkeyOptions, ETrue );
    cba->MakeCommandVisible( EAknSoftkeyBack, ETrue );
    iAppUi->AddToViewStackL( *this, iContainer );

    MenuBar()->SetContextMenuTitleResourceId( R_CHATCLIENT_INVITATIONS_CONTEXTMENUBAR );
    }

// ---------------------------------------------------------
// CCAInvitationView::DoDeactivate()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationView::DoDeactivate()
    {

    delete iIdentification;

    iIdentification = NULL;

    if ( iTitlePane )
        {
        iTitlePane->RemoveObserver( this );
        }

    if ( iContainer )
        {
        iAppUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    iInvitationPC->UnRegisterObserver();
    }



// ---------------------------------------------------------
// CCAInvitationView::HandleInvitationEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationView::HandleInvitationEventL( TEnumsPC::TInviteType aInviteType, TInt aIndex )
    {
    switch ( aInviteType )
        {
        case TEnumsPC::ENewInvite:
            {
            break;
            }
        case TEnumsPC::EInviteCancelOrExpired:
            {
            //if the expired one is the current one which is being viewd
            //close the current view and switch the view back
            if ( aIndex == iInvitationIndex )
                {
                //since this is called by the system dont use the following
                //CAAppUi()->CAStatusPane()->DeleteTab(iTabId); method to delete
                //this invitation
                if ( iTitlePane )
                    {
                    iTitlePane->SynchroniseAndRemoveTabGroup();
                    }
                iAppUi->SwitchViewBackL( KUidFriendsListView );
                }
            }
        default:
            {
            break;
            }

        }

    }

// ---------------------------------------------------------
// CCAInvitationView::HandleInvitationResponse
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationView::HandleInvitationResponse( TBool /*aAcceptance*/,
                                                  const TDesC& /*aUserId*/,
                                                  const TDesC&  /*aGroupName*/,
                                                  const TDesC& /*aResponse*/ )
    {
    //Nothing to do here..
    //handled in appui
    }


// ---------------------------------------------------------
// CCAInvitationView::SwitchTabL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationView::SwitchTabL( const TDesC& aWvId, const TInt aTabIndex )
    {

    iInvitationIndex = iInvitationPC->FindInvitationIndex( aWvId );
    //if valid index then only perform the switch
    if ( iInvitationIndex != -1 )
        {
        iInvitationPC->SetActiveInvitation( iInvitationIndex );

        //Check whether the invitation is read else mark it read
        if ( !iInvitationPC->IsInvitationUnread( iInvitationIndex ) )
            {
            iInvitationPC->SetInvitationAsReadL( iInvitationIndex );
            }

        iTabId = aTabIndex;

        //remove the controls from view stack
        iAppUi->RemoveFromViewStack( *this, iContainer );

        iContainer->InvitationSwitchTabL( iInvitationIndex );

        //again add those controls to view stack
        iAppUi->AddToViewStackL( *this, iContainer );

        iChatJoinRejectFlag = EFalse;
        }


    }


// -----------------------------------------------------------------------------
// CCAInvitationView::DynInitMenuPaneL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CCAInvitationView::DynInitMenuPaneL( TInt aResourceId,
                                          CEikMenuPane* aMenuPane )
    {
    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dim the help menu item
        aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
        }
    switch ( aResourceId )
        {
        case R_CHATCLIENT_FRIENDSLIST_SUBMENU:
            {
            // Always hide this option as unblock shoul be should
            // be showed only in blocklist view.
            aMenuPane->SetItemDimmed( EChatClientFriendsListViewUnblock,
                                      ETrue );

            aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdBlock,
                                      iContainer->IsBlocked( iInvitationPC->
                                                             InvitationUserIDL( iInvitationIndex
                                                                              ) ) );
            // show/hide "show blocked list" option
            if ( !IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_SHOW_BLOCKED_LIST ) )
                {
                aMenuPane->SetItemDimmed(
                    EChatClientFriendsListViewShowBlockedList, ETrue );
                }

            break;
            }
        default:
            {
            //we are never here.
            break;
            }

        }
    }

// ---------------------------------------------------------
// CCAInvitationView::ShowPopUpMenuL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationView::ShowPopUpMenuL( )
    {
#ifndef RD_30_DISABLE_TOUCH
    // S60 COMMON
    MenuBar()->StopDisplayingMenuBar();
    MenuBar()->TryDisplayContextMenuBarL();
#endif //RD_30_DISABLE_TOUCH
    }

// ---------------------------------------------------------
// CCAInvitationView::ShowRecipientsListL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInvitationView::ShowRecipientsListL( TInt /*aResourceId*/ )
    {
    return KErrNotFound;
    }

//  End of File
