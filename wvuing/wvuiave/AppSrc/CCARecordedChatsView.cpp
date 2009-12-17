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
* Description:  View for recorded chats.
*
*/




// INCLUDE FILES
#include "MCAProcessManager.h"
#include "MCASettingsPC.h"
#include "IMUtils.h"
#include "CCARecordedChatsView.h"
#include "CCARecordedChatsContainer.h"
#include "MCARecordedChatProvider.h"
#include "CCAUiMessageUtils.h"
#include "chatngclient.hrh"
#include "CCAAppUi.h"
#include "CCAStatusPaneHandler.h"
#include "CAExternalInterface.h"
#include "ChatDebugPrint.h"
#include "ChatDefinitions.h"
#include "IMPrivateCRKeys.h"
#include "IMVariant.hrh"
#include "MCARecordedChatsPC.h"

#include <chatNG.rsg>
#include <eikmenup.h>
#include <aknlists.h>
#include <FeatMgr.h>
#include <StringLoader.h>
#include <hlplch.h>
#include <bldvariant.hrh>
#include <eikmenub.h>
#include <senduiconsts.h>
#include <finditem.hrh>
#include <finditemmenu.h>
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


const TInt KGranularity = 2;
// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CCARecordedChatsView::~CCARecordedChatsView
// Destructor
// -----------------------------------------------------------------------------
//
CCARecordedChatsView::~CCARecordedChatsView()
    {
    if ( iContainer && iAppUI )
        {
        iAppUI->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer;
    delete iFormat;
    if ( iContextMenu && iAppUI )
        {
        iAppUI->RemoveFromViewStack( *this, iContextMenu );
        }
    delete iContextMenu;
    delete iFindMenu;
    delete iItemFinder;
    }

// ---------------------------------------------------------
// CCARecordedChatsView::ConstructL()
// Symbian OS default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsView::ConstructL( TInt aAvkonViewResourceId, TUid aViewId )
    {
    BaseConstructL( aAvkonViewResourceId, aViewId );

    // get and format the time/date string
    HBufC* tFormat = CCoeEnv::Static()->AllocReadResourceLC(
                         R_QTN_TIME_USUAL_WITH_ZERO );
    HBufC* dFormat = CCoeEnv::Static()->AllocReadResourceLC(
                         R_QTN_DATE_SHORT_WITH_ZERO );
    CDesCArray* array = new( ELeave ) CDesCArrayFlat( KGranularity ); // two items
    CleanupStack::PushL( array );
    array->AppendL( *tFormat );
    array->AppendL( *dFormat );
    iFormat = StringLoader::LoadL( R_QTN_CHAT_RECORD_END_NAVI, *array );

    CleanupStack::PopAndDestroy( 3, tFormat ); // tFormat,dFormat,array

    iNewOptionsMenu = IMUtils::CRKeyL( KCRUidIMVariation,
                                       KIMVariationKey ) & EIMFeatRecordedChatOptions;

    // Context sensitivie menu for S60 3.0
    iContextMenu = new( ELeave ) CEikMenuBar;
    TInt contextResource = iNewOptionsMenu ?
                           R_CHATCLIENT_MENUBAR_FINDITEMUI :
                           R_CHATCLIENT_RECORDEDCHATS_MENUBAR_CONTEXT;
    iContextMenu->ConstructL( this, 0, contextResource );

    iFindMenu = CFindItemMenu::NewL( EChatClientFindMenuPlaceHolder );
    iFindMenu->AttachItemFinderMenuL( R_CHATCLIENT_RECORDEDCHATS_VIEW_MENU );
    // automatic finder on by default

    if ( iNewOptionsMenu )
        {
        iFindMenu->HandleItemFinderCommandL( EFindItemCmdEnableAutoFind );
        }
    else
        {
        iFindMenu->HandleItemFinderCommandL( EFindItemCmdDisableAutoFind );
        }

    iItemFinder = CItemFinder::NewL();

    iAppUI = static_cast <CCAAppUi*> ( CCoeEnv::Static()->AppUi() );
    }

// ---------------------------------------------------------
// CCARecordedChatsView::HandleCommandL(TInt aCommand)
// Handle commands
// ---------------------------------------------------------
//
void CCARecordedChatsView::HandleCommandL( TInt aCommand )
    {
    HideContextMenu();
    switch ( aCommand )
        {
            // new stuff
        case EAknSoftkeyOpen: // flowthrough
        case EChatClientOpenObject:
            {
            if ( iOpenObject )
                {
                return;
                }

            MCAConversationMessage* msg = iContainer->SelectedMessage();
            if ( msg )
                {
                CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC();

                // Set the indicator to true
                iOpenObject = ETrue;

                TRAPD( err, utils->OpenObjectL( *msg ) );

                // Set it back
                iOpenObject = EFalse;

                CleanupStack::PopAndDestroy( utils );
                User::LeaveIfError( err );
                }
            break;
            }

        case EAknSoftkeyEmpty:
            {
            // Show context menu with selection key
            if ( !iNewOptionsMenu )
                {
                ShowPopUpMenuL();
                }
            break;
            }

        case EChatClientSaveObject:
            {
            MCAConversationMessage* msg = iContainer->SelectedMessage();
            if ( msg )
                {
                CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC();
                TInt err = utils->SaveObjectL( *msg );
                //if the image is saved then we have to update
                //the file. So mark iDirty as ETrue
                //fix - refer Ui Spec Approved Version 1.0
                //(Instant Messaging NG 001 151006.pdf)
                //Section 10.2.10 Pg 131 -
                //"In case user has saved the image already or sent it
                //by himself, this option(save) is not available."
                if ( err == KErrNone )
                    {
                    iDirty = ETrue;
                    }
                CleanupStack::PopAndDestroy( utils );
                }
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

        // flow through
        case EFindItemCmdEnableAutoFind:
        case EFindItemCmdDisableAutoFind:
            {
            iContainer->SetItemHighlightL(
                aCommand == EFindItemCmdEnableAutoFind );
            iFindMenu->HandleItemFinderCommandL( aCommand );
            if ( iContextMenu->IsDisplayed() )
                {
                iContextMenu->StopDisplayingMenuBar();
                }
            break;
            }

        // Fall through
        case EAknSoftkeyBack:
        case EChatClientCmdBack:
            {
            iContainer->FreeMemoryForViewSwitch();
            iAppUI->SwitchViewL( KUidRecordedChatListView );
            break;
            }
        case EChatClientSavedListViewCmdDelete:
            {
            if ( iRecChatArrayProvider->DeleteChatL() )
                {
                iContainer->FreeMemoryForViewSwitch();
                iAppUI->SwitchViewBackL( KUidRecordedChatListView );
                }
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
        case EChatClientSavedListViewCmdSendOpt:
            {
            iRecChatArrayProvider->SendChatViaMTML();
            break;
            }
        case EChatClientSavedListViewCmdSendSMS:
            {
            iRecChatArrayProvider->SendChatViaMTML(
                TUid::Uid( KSenduiMtmSmsUidValue ) );
            break;
            }
        case EChatClientSavedListViewCmdSendMultimedia:
            {
            iRecChatArrayProvider->SendChatViaMTML(
                TUid::Uid( KSenduiMtmMmsUidValue ) );
            break;
            }
        case EChatClientSavedListViewCmdSendInfrared:
            {
            iRecChatArrayProvider->SendChatViaMTML(
                TUid::Uid( KSenduiMtmIrUidValue ) );
            break;
            }
        case EChatClientSavedListViewCmdSendBluetooth:
            {
            iRecChatArrayProvider->SendChatViaMTML(
                TUid::Uid( KSenduiMtmBtUidValue ) );
            break;
            }
        case EChatClientCall:
            {
            UpdateItemTypeL();
            iFindMenu->HandleCallL( *iItemFinder->CurrentItemExt().iItemDescriptor );
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
                if ( aCommand == EChatClientMainViewCmdChangeStatus )
                    {
                    UISessionManager().LoginL( MCAUiLoginCmdCB::ELogin );
                    if ( !UISessionManager().IsLoggedIn() )
                        {
                        return;
                        }
                    }
                iAppUI->HandleCommandL( aCommand );
                }
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCARecordedChatsView::DoActivateL(...)
// called when view is activated
// ---------------------------------------------------------
//
void CCARecordedChatsView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    //This must be set before view is used with SetRecordedChatProvider method.
    if ( !iRecChatArrayProvider )
        {
        User::Leave( KErrNotFound );
        }

    // Unregister the view that was registered from Forward option.
    // didn't find any better place to put this.
    if ( TEnumsPC::EToBeUnregistered == iAppUI->RetForwardTo() )
        {
        iAppUI->UnRegisterPreviousview( );
        iAppUI->SetResetForwardTo( TEnumsPC::EUnregistered );
        }
    iContainer = CCARecordedChatsContainer::NewL(
                     ClientRect(),
                     static_cast<MCAViewSwitcher&>( *iAppUI ),
                     *iRecChatArrayProvider,
                     *this,
                     *this,
                     *( iAppUI->GetProcessManager().GetSettingsInterface() ),
                     iAppUI,
                     *( iAppUI->GetProcessManager().GetRecordedChatsPC() ) );

    iAppUI->AddToViewStackL( *this, iContainer );
    iAppUI->AddToViewStackL( *this, iContextMenu, 0,
                             ECoeStackFlagRefusesFocus );

    const TDesC& chatViewTitle = iRecChatArrayProvider->RecordedChatName();
    HBufC* chatNaviPane = HBufC::NewLC( iFormat->Length() ); //check length

    TPtr ptrDate( chatNaviPane->Des() );
    iRecChatArrayProvider->RecordedEndDate().FormatL( ptrDate, *iFormat );
    AknTextUtils::LanguageSpecificNumberConversion( ptrDate );

    iAppUI->CAStatusPane()->SetTitleL( chatViewTitle );
    iAppUI->CAStatusPane()->SetNaviLabelL( *chatNaviPane );
    CleanupStack::PopAndDestroy( chatNaviPane );

    // set the context menu for menubar (launched with middle softkey)
#ifndef RD_30_DISABLE_TOUCH
    TInt contextResource = iNewOptionsMenu ?
                           R_CHATCLIENT_MENUBAR_FINDITEMUI :
                           R_CHATCLIENT_RECORDEDCHATS_MENUBAR_CONTEXT;
    MenuBar()->SetContextMenuTitleResourceId( contextResource );
#endif

    // Automatic highlight
    iContainer->SetItemHighlightL( iNewOptionsMenu ? ETrue : EFalse );

    //Initialise
    iDirty = EFalse;

    CHAT_DP_TXT( "CCARecordedChatsView activated" );
    }

// ---------------------------------------------------------
// CCARecordedChatsView::HandleCommandL(TInt aCommand)
// Called when view is deactivated
// ---------------------------------------------------------
//
void CCARecordedChatsView::DoDeactivate()
    {

    //if any image is saved from recorded chats view
    //the file is dirty and has to be recommitted
    //fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
    //Section 10.2.10 Pg 131 -
    //"In case user has saved the image already or sent it by himself,
    // this option(save) is not available."
    if ( iDirty )
        {
        TRAP_IGNORE( iAppUI->GetProcessManager().GetRecordedChatsPC()->ReLoggingL() );
        }


    if ( iContainer )
        {
        iAppUI->RemoveFromViewStack( *this, iContainer );
        }
    if ( iContextMenu )
        {
        iAppUI->RemoveFromViewStack( *this, iContextMenu );
        }

    delete iContainer;
    iContainer = NULL;

    //reset
    iDirty = EFalse;

    // Free memory used by message container in engine side
    TRAP_IGNORE( iRecChatArrayProvider->DeleteRecordedChatContainerL() );

    CHAT_DP_TXT( "CCARecordedChatsView deactivated" );
    }

// ---------------------------------------------------------
// CCARecordedChatsView::DynInitMenuPaneL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsView::DynInitMenuPaneL( TInt aResourceId,
                                             CEikMenuPane* aMenuPane )
    {
    TBool showFwdToContact = iContainer->FwdContactVisible();
    TBool showFwdToGroup   = iContainer->FwdGroupVisible();

    // if we're displaying general menu and help feature is not supported..
    switch ( aResourceId )
        {
        case R_CHATCLIENT_GENERAL_MENU:
            {
            if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                // dimm the help menu item
                aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
                }
            break;
            }

        case R_CHATCLIENT_RECORDEDCHATS_VIEW_MENU:
            {
            if ( iNewOptionsMenu )
                {

                if ( ! iAppUI->UISessionManager().IsSupported(
                         CCAUISessionManager::EGroup ) )
                    {
                    // groups are not supported, hide forward to group
                    showFwdToGroup = EFalse;
                    }

                TBool showForwardMenu = showFwdToGroup || showFwdToContact;

                TBool openable = EFalse;
                TBool savePossible = EFalse;
                TBool object = iContainer->IsObjectSelected( openable , savePossible );

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

                // "forward" hiding logic case 3
                // Hide also when focus on object that is not openable
                if ( !showForwardMenu || ( object && !openable ) )
                    {
                    CHAT_DP_TXT( "CCARecordedChatsView::DynInitMenuPaneL \
 			              Hide the whole forward menu" );
                    aMenuPane->SetItemDimmed( EChatClientIboxForward, ETrue );
                    }
                else
                    {
                    CHAT_DP_TXT( "CCARecordedChatsView::DynInitMenuPaneL \
 			              Show the forward menu" );
                    aMenuPane->SetItemDimmed(
                        EChatClientIboxForward, !iContainer->IsStopped() );
                    }
                }
            else
                {
                // Old options menu
                aMenuPane->SetItemDimmed( EChatClientOpenObject, ETrue );
                aMenuPane->SetItemDimmed( EChatClientSaveObject, ETrue );
                aMenuPane->SetItemDimmed( EChatClientFindMenuPlaceHolder, ETrue );
                aMenuPane->SetItemDimmed( EChatClientIboxForward, ETrue );
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
            if ( iContainer->IsStopped() && iNewOptionsMenu )
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
            if ( iNewOptionsMenu )
                {
                iFindMenu->UpdateItemFinderMenuL( aResourceId, aMenuPane );
                }

            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsView::SetRecordedChatProvider
// Set provider for recorded chats.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsView::SetRecordedChatProvider(
    MCARecordedChatProvider& aRecChatArrayProvider )
    {
    iRecChatArrayProvider = &aRecChatArrayProvider;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsView::HideContextMenu
// Hide context sensitive menu if active
// -----------------------------------------------------------------------------
//
void CCARecordedChatsView::HideContextMenu()
    {
    if ( iContextMenu->IsDisplayed() )
        {
        iContextMenu->StopDisplayingMenuBar();
        }
    }

// ---------------------------------------------------------
// CCARecordedChatsView::UpdateItemTypeL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsView::UpdateItemTypeL()
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
// CCARecordedChatsView::ShowRecipientsListL()
// Leaving function dummy implemented from base class
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCARecordedChatsView::ShowRecipientsListL( TInt /*aResourceId*/ )
    {
    // Nothing to do here
    User::Leave( KErrGeneral );
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CCARecordedChatsView::ShowPopUpMenuL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsView::ShowPopUpMenuL( )
    {
#ifndef RD_30_DISABLE_TOUCH
    // S60 COMMON
    MenuBar()->StopDisplayingMenuBar();
    MenuBar()->TryDisplayContextMenuBarL();
#else
    // S60 3.0
    iContextMenu->StopDisplayingMenuBar();
    iContextMenu->TryDisplayMenuBarL();
#endif //RD_30_DISABLE_TOUCH
    }
// ---------------------------------------------------------
// CCARecordedChatsView::ProcessCommandL
// from CAknView::ProcessCommandL .
// ---------------------------------------------------------
void CCARecordedChatsView::ProcessCommandL(TInt aCommand)
	  {
	  if ( iContainer )
	      {
	      CAknView::ProcessCommandL(aCommand);
	  	  }
	  }
// End of File
