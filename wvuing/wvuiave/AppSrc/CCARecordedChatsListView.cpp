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
* Description:  Recorded chats list view
*
*/


// INCLUDE FILES
#include   "IMUtils.h"

#include   "MCARecordedChatsPC.h"
#include   "MCAProcessManager.h"

#include   "CCARecordedChatsListView.h"
#include   "CCARecordedChatsListViewContainer.h"
#include   "CCARecordedChatsArray.h"

#include   "chatngclient.hrh"
#include   "CAExternalInterface.h"
#include   "CCAStatusPaneHandler.h"
#include   "ChatDebugPrint.h"

// local variation
#include   "IMPrivateCRKeys.h"
#include   "IMVariant.hrh"

#include   <bldvariant.hrh>
#include   <chatNG.rsg>

#include   <aknlists.h>
#include   <FeatMgr.h>
#include   <eikmenup.h>
#include   <eikmenub.h>
#include   <hlplch.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// ================= MEMBER FUNCTIONS =======================
// Destructor
CCARecordedChatsListView::~CCARecordedChatsListView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer;
    delete iRecordedChatsArray;
    }

// ---------------------------------------------------------
// CCARecordedChatsListView::ShowPopUpMenuL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsListView::ShowPopUpMenuL()
    {
    MenuBar()->TryDisplayContextMenuBarL();    
    }

// ---------------------------------------------------------
// CCARecordedChatsListView::ConstructL()
// Symbian OS default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsListView::ConstructL(  TInt aAvkonViewResourceId,
                                            TUid aViewId )
    {
    BaseConstructL( aAvkonViewResourceId, aViewId );
    MCARecordedChatsPC* recordedChatsPC
    = CAAppUi()->GetProcessManager().GetRecordedChatsPC();
    MCARecordedChatsArrayPC* recordedChatsArrayPC
    = CAAppUi()->GetProcessManager().GetRecordedChatsArrayPC();

    iRecordedChatsArray = CCARecordedChatsArray::NewL( *recordedChatsPC, *recordedChatsArrayPC );
    }

// ---------------------------------------------------------
// CCARecordedChatsListView::HandleCommandL(TInt aCommand)
// Commands to handle
// ---------------------------------------------------------
//
void CCARecordedChatsListView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
            //Fall through
        case EAknSoftkeyBack:
        case EChatClientCmdBack:
            {
            CAAppUi()->SwitchViewL( KUidFriendsListView );
            CAknDoubleGraphicStyleListBox* listBox = iContainer->ListBox();
            if ( listBox )
                {
                if ( listBox->View() )
                    {
                    // CurrentItemIndex panics if no view
                    TInt index = 0;
                    listBox->SetCurrentItemIndex( index );
                    iSelectedIndex = listBox->CurrentItemIndex();
                    }
                }
            break;
            }
        //Fall through
        case EAknSoftkeyOk:
        case EAknSoftkeyOpen:   // Flowthrough, MSK command
        case EChatClientSavedListViewCmdOpen:
            {
            iContainer->OpenRecordL();
            break;
            }
        case EChatClientSavedListViewCmdDelete:
            {
            iContainer->DeleteHistoryFilesL();
            break;
            }
        case EChatClientSavedListViewCmdMark:
            {
            iContainer->MarkL( iContainer->ListBox()->CurrentItemIndex(),
                               ETrue );
            break;
            }
        case EChatClientSavedListViewCmdUnmark:
            {
            iContainer->MarkL( iContainer->ListBox()->CurrentItemIndex(),
                               EFalse );
            break;
            }
        case EChatClientSavedListViewCmdMarkAll:
            {
            iContainer->MarkAllL( ETrue );
            break;
            }
        case EChatClientSavedListViewCmdUnmarkAll:
            {
            iContainer->MarkAllL( EFalse );
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
        case EChatClientSavedListViewCmdSendOpt:
            {
            TInt err = iRecordedChatsArray->SendMTML( iContainer->ListBox()->CurrentItemIndex() );
            if ( err != KErrCancel && ( iContainer->ListBox()->View()->SelectionIndexes()->Count() > 0 ) )
                {
                iContainer->MarkAllL( EFalse );
                }
            break;
            }
        case EChatClientMainViewCmdChangeStatus:
            {
            UISessionManager().LoginL( MCAUiLoginCmdCB::ELogin );
            if ( UISessionManager().IsLoggedIn() )
                {
                AppUi()->HandleCommandL( aCommand );
                }
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
// CCARecordedChatsListView::DoActivateL
// Called when view is activated
// ---------------------------------------------------------
//
void CCARecordedChatsListView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    MCAViewSwitcher& viewSwitcher = static_cast<MCAViewSwitcher&>( *CAAppUi() );

    // here reset the engine logger headers.....so that ..we'll get the data from a new drive..
#ifdef RD_MULTIPLE_DRIVE

    MCARecordedChatsPC* recordedChatsPC
    = CAAppUi()->GetProcessManager().GetRecordedChatsPC();

    recordedChatsPC->ResetEngineLoggerL();

#endif
    iContainer = CCARecordedChatsListViewContainer::NewL(
                     *this,
                     ClientRect(),
                     *CAAppUi(),
                     viewSwitcher,
                     *iRecordedChatsArray,
                     iSelectedIndex,
                     iContextFlag
                 );

    iContainer->SetMopParent( this );

#ifndef RD_30_COMPATIBILITY_MODE
    MenuBar()->SetContextMenuTitleResourceId(
        R_CHATCLIENT_RECORDEDCHATSLIST_CONTEXTMENUBAR );
#endif  // RD_30_COMPATIBILITY_MODE

    AppUi()->AddToViewStackL( *this, iContainer );

    viewSwitcher.CAStatusPane()->SetTitleL(
        iEikonEnv, R_CHATCLIENT_RECORDEDCHATLIST_VIEW_TITLE );

    viewSwitcher.CAStatusPane()->SetNaviLabelL( KNullDesC );

    iContainer->ListBox()->UpdateScrollBarsL();

    // local variated empty texts for list
    if ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey ) & EIMFeatSimplifiedUI )
        {
        HBufC* emptyTxtBuf = IMUtils::CombineStringFromResourceLC
                             ( R_QTN_CHAT_EMPTY_SAVED_CHAT_PRIMARY,
                               R_QTN_CHAT_EMPTY_SAVED_CHAT );

        iContainer->ListBox()->View()->SetListEmptyTextL( *emptyTxtBuf );
        CleanupStack::PopAndDestroy( emptyTxtBuf );
        }

    CHAT_DP_TXT( "CCAChatListView activated" );
    }

// ---------------------------------------------------------
// CCARecordedChatsListView::DoDeactivate
// Called when view is deactivated
// ---------------------------------------------------------
//
void CCARecordedChatsListView::DoDeactivate()
    {
    if ( iContainer )
        {
        CAknDoubleGraphicStyleListBox* listBox = iContainer->ListBox();
        if ( listBox )
            {
            if ( listBox->View() )
                {
                // CurrentItemIndex panics if no view
                iSelectedChat.Set( iContainer->HighlightedChatName() );
                }
            }
        iSelectedIndex = listBox->CurrentItemIndex();
        ( KErrNotFound == iSelectedIndex ) ? ( iSelectedIndex = 0 ) : ( iSelectedIndex );

        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    iContainer = NULL;

    CHAT_DP_TXT( "CCAChatListView deactivated" );
    }

// ---------------------------------------------------------
// CCARecordedChatsListView::DynInitMenuPaneL()
// Dynamical menu initialization.
// ---------------------------------------------------------
//
void CCARecordedChatsListView::DynInitMenuPaneL( TInt aResourceId,
                                                 CEikMenuPane* aMenuPane )
    {
    TInt markedCount( iContainer->ListBox()->SelectionIndexes()->Count() );
    TInt itemCount( iRecordedChatsArray->MdcaCount() );

    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU )
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) || iContextFlag )
            {
            aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
            }
        if ( iContextFlag )
            {
            aMenuPane->SetItemDimmed( EChatClientCmdExit, ETrue );
            }

        }
    else if ( aResourceId == R_CHATCLIENT_RECORDEDCHATSLIST_VIEW_MENU )
        {
        if ( markedCount != 0 || iContextFlag )
            {
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdOpen, ETrue );
            }
        else if ( itemCount == 0 )
            {
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdOpen, ETrue );
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdDelete, ETrue );
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdSendOpt, ETrue );
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdMarkOpt, ETrue );
            }

        if ( iContextFlag )
            {
            aMenuPane->SetItemDimmed( EChatClientMainViewSettings, ETrue );
            }

        // check if this menu has log in / log out options available
        TInt pos( 0 );
        if ( aMenuPane->MenuItemExists( EChatClientMainViewCmdLogin, pos ) )
            {
            // this menu contains log in and log out, display correct one
            if ( UISessionManager().IsLoggedIn() )
                {
                aMenuPane->SetItemDimmed( EChatClientMainViewCmdLogin, ETrue );
                if ( iContextFlag )
                    {
                    aMenuPane->SetItemDimmed( EChatClientMainViewCmdLogout, ETrue );
                    }
                }
            else
                {
                aMenuPane->SetItemDimmed( EChatClientMainViewCmdLogout, ETrue );
                if ( iContextFlag )
                    {
                    aMenuPane->SetItemDimmed( EChatClientMainViewCmdLogin, ETrue );
                    }
                }
            }
        }
    else if ( aResourceId == R_CHATCLIENT_RECORDEDCHATSLIST_MARK_SUB_MENU )
        {
        // All unmarked or not
        if ( markedCount == 0 )
            {
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdUnmarkAll, ETrue );
            }
        if ( markedCount == itemCount )
            {
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdMarkAll, ETrue );
            }

        MCARecordedChatsPC* recordedChatsPC
        = CAAppUi()->GetProcessManager().GetRecordedChatsPC();

        // Marked or not marked
        if ( recordedChatsPC->IsMarked( iContainer->ListBox()->CurrentItemIndex() ) )
            {
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdMark, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EChatClientSavedListViewCmdUnmark, ETrue );
            }
        }
    }
// -----------------------------------------------------------------------------
// CCARecordedChatsListView::RecordedChatArrayProvider
// Provide recordedchatprovider for other views.
// -----------------------------------------------------------------------------
//
MCARecordedChatProvider& CCARecordedChatsListView::RecordedChatArrayProvider()
    {
    return *iRecordedChatsArray;
    }

// ---------------------------------------------------------
// CCARecordedChatsListView::ShowRecipientsListL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCARecordedChatsListView::ShowRecipientsListL( TInt /*aResourceId*/ )
    {
    return User::LeaveIfError( -1 );
    }
// End of File

