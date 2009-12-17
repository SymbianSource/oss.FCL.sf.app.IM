/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Dialog for displaying servers
*
*/

// INCLUDE FILES
#include	"CWVSettingsUIServerListDialog.h"
#include	"CWVSettingsUI.hrh"
#include	"CWVSettingsUIServersArray.h"
#include	"CWVSettingsUIServerSelector.h"
#include	"MWVSettingsObserverng.h"
#include	"CWVSettingsUIDialogTitlePaneHandler.h"

#include	<csxhelp/imng.hlp.hrh>

#include    <data_caging_path_literals.hrh>
#include	<aknlists.h>
#include	<StringLoader.h>    // StringLoader
#include	<aknquerydialog.h>
#include	<aknradiobuttonsettingpage.h>
#include	<aknnotewrappers.h>
#include	<cimpssapsettingsstore.h>
#include	<cimpssapsettingslist.h>
#include	<cimpssapsettings.h>
#include	<CWVSettingsUIng.rsg>
#include	<AknIconArray.h>
#include	<eikapp.h>
#include	<apsettings.mbg>
#include    <AknsUtils.h>
#include    <AknsConstants.h>
#include    <hlplch.h>
#include    <featmgr.h>
#include    <cimpspresenceconnectionuing.h>
#include    <impspresenceconnectionuiconstsng.h>
#include    <pathinfo.h>
#include    <cpengnwsessionslotid2.h>
#include	"CAExternalInterface.h"
#include	<CAVariationNG.rsg>
#include	<BARSREAD.H>
#include 	"VariantKeys.h"

//FORWARD DECLARATION
class CIMPSSAPSettingsStore;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWVSettingsUIServerListDialog::CWVSettingsUIServerListDialog(
    CIMPSSAPSettingsStore& aSAPSettingsStore,
    MWVSettingsObserver& aObserver,
    CWVSettingsUIDialogTitlePaneHandler& aTitlePaneHandler,
    CWVSettingsUIDialogImp& aDialog,
    CIMPSPresenceConnectionUi* aConnectionUi )
        : iSAPSettingsStore( &aSAPSettingsStore ),
        iObserver( &aObserver ),
        iProtectedServers( 4 ),  // granularity = 4, because it's not likely that there are more protected servers
        iTitlePaneHandler( &aTitlePaneHandler ),
        iDialog( &aDialog ),
        iConnectionUI( aConnectionUi ),
        iEditorActive( EFalse ),
        iAdded( EFalse )
        , iIsOpenedTargettDlg( EFalse )
    {
    }

// Destructor
CWVSettingsUIServerListDialog::~CWVSettingsUIServerListDialog()
    {
    if ( iSAPSettingsStore )
        {
        iSAPSettingsStore->RemoveObserver( this );
        }
    delete iTitle;
    iProtectedServers.Close();
    delete iSAP;
    if ( iDlgLauncher && iDlgLauncher->IsActive() )
        {
        iDlgLauncher->Cancel();
        }
    delete iDlgLauncher;
    // to cancel all request when dialog shut
    CCoeEnv* env = CCoeEnv::Static();
    if ( env->IsActive() )
        {
        env->Cancel();
        }

    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::RefreshTitleL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::RefreshTitleL()
    {
    if ( !iTitle )
        {
        iTitle = iEikonEnv->AllocReadResourceL( RSC_WVSETTINGSVIEW_SERVERLIST_TITLE );
        }

    iTitlePaneHandler->SetTitlePaneTextL ( *iTitle );
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::DisplayServerEditorDialogL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TWVSettingsDialogReturnValue CWVSettingsUIServerListDialog::DisplayServerEditorDialogL(
    CWVSettingsUIDefineNewServerDialog::TEditMode aEditMode )
    {
    TInt exitReason( 0 );
    if ( iSAP )
        {
        iCurrent = iSAP->Uid();
        }

    CWVSettingsUIDefineNewServerDialog* defineNewServerDialog = new ( ELeave )
    CWVSettingsUIDefineNewServerDialog(
        exitReason,
        *iSAPSettingsStore,
        aEditMode,
        *iObserver,
        *iTitlePaneHandler,
        iDialog,
        iNewUid,
        iConnectionUI,
        iSAP );

    // the ownership of iSAP has now been transferred to define new server dialog
    iSAP = NULL;

    CleanupStack::PushL( defineNewServerDialog );
    defineNewServerDialog->ConstructL( R_WVSETTINGSVIEW_DEFNEWSERVER_MENUBAR );
    CleanupStack::Pop( defineNewServerDialog );

    iEditorActive = ETrue;
    TInt returnValue ( defineNewServerDialog->ExecuteLD( R_WVSETTINGSVIEW_DEFNEWSERVER_DLG ) );
    iEditorActive = EFalse;

    // Dialog was canceled.
    if ( returnValue == EEikBidCancel )
        {
        // Dialog was shut down by a dialog shutter and we need to return
        // quickly without touching any internal variables.
        return EWVSettingsViewForceExit;
        }

    if ( ( exitReason == EMissingCompulsory )
         && aEditMode == CWVSettingsUIDefineNewServerDialog::EEditServer )
        {
        CIMPSSAPSettings* sap = GetSAPL( iListBox->CurrentItemIndex() );

        if ( sap )
            {
            CleanupStack::PushL( sap );
            // show note that prevents user from deleting protected server
            if ( sap->Protection() == ESAPBrandProtection )
                {
                // show note
                ShowBrandProtectionNoteL();
                CleanupStack::PopAndDestroy( sap );
                }
            else
                {
                // there is no brand protection for this server, it's ok to delete it
                // delete the server
                TRAPD( err, iSAPSettingsStore->DeleteSAPL( sap->Uid() ) );

                if ( err == KErrNone )
                    {
                    TInt index( iListBox->CurrentItemIndex() );

                    // the index cannot go below zero in any case
                    if ( --index < 0 )
                        {
                        index = 0;
                        }
                    // we must set the current item index to the next
                    // item, unless this is the last one
                    if ( index + 1 < iListBox->BottomItemIndex() )
                        {
                        // this is not the last item, we need to add one
                        index++;
                        }
                    // Remove data also from internal buffer
                    UpdateModelL();

                    iListBox->HandleItemRemovalL();
                    // if the list is empty index is set to 0 above
                    iListBox->SetCurrentItemIndexAndDraw( index );
                    iListBox->DrawNow();
                    }
                CleanupStack::PopAndDestroy( sap );
                }
            }
        }

    iListBox->View()->DrawItem( iListBox->CurrentItemIndex() );

    if ( returnValue == EWVSettingsViewCmdExit )
        {
        return EWVSettingsViewExitCalled;
        }
    else
        {
        return EWVSettingsViewOk;
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::DeleteSelectedServerL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::DeleteSelectedServerL()
    {
    // get the current sap
    CIMPSSAPSettings* sap = GetSAPL( iListBox->CurrentItemIndex() );

    // Get default sap's uid
    TUint32 defaultSapUid;
    iSAPSettingsStore->GetDefaultL( defaultSapUid, EIMPSIMAccessGroup );

    // Check if we are trying to delete default sap
    TBool isDefaultSap = sap->Uid() == defaultSapUid;

    TInt sapCount = iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup );

    if ( sap )
        {
        CleanupStack::PushL( sap );
        // show note that prevents user from deleting protected server
        if ( sap->Protection() == ESAPBrandProtection )
            {
            // show note
            ShowBrandProtectionNoteL();
            CleanupStack::PopAndDestroy( sap ); // sap
            return;
            }

        CPEngNWSessionSlotID2* sessionSlotID = NULL;
        TRAPD( err, sessionSlotID = iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM ) );
        CleanupStack::PushL( sessionSlotID );

        if ( !err )
            {
            // check if IM or PEC is connected to this server
            CIMPSSAPSettings* loggedInSap = CIMPSSAPSettings::NewLC();
            TInt returnValue = iConnectionUI->GetLoggedInSapL( *sessionSlotID, *loggedInSap );
            CleanupStack::Pop( loggedInSap );
            CleanupStack::PopAndDestroy( sessionSlotID );
            CleanupStack::PushL( loggedInSap );

            TIMPSConnectionOperation connOperation( EIMPSConnOppUnknown );

            // if returnValue is anything except KErrNone, nobody is logged in at the moment
            // but we must check if the login is ongoing at this moment
            if ( returnValue != KErrNone )
                {
                // we were not logged in
                // check if the log in is ongoing
                HBufC* buffer = NULL;
                connOperation = iConnectionUI->CurrentConnectionOperationL( buffer );
                if ( connOperation == EIMPSConnOppClientLogin )
                    {
                    // login ongoing, set the sap name for later checks
                    CleanupStack::PushL( buffer );
                    loggedInSap->SetSAPNameL( *buffer );
                    CleanupStack::PopAndDestroy( buffer );
                    }
                }

            // if there is an active connection or ongoing login operation
            if ( ( returnValue == KErrNone ) || ( connOperation == EIMPSConnOppClientLogin ) )
                {
                // check if the active connection is to the current sap
                if ( loggedInSap->SAPName().CompareF ( sap->SAPName() ) == 0 )
                    {
                    // the active connection or the ongoing login operation was to the current sap
                    if ( connOperation == EIMPSConnOppClientLogin )
                        {
                        // a login operation is ongoing, we cannot let the user edit the settings
                        HBufC* prompt = StringLoader::LoadLC(
                                            R_QTN_CHAT_LOGIN_ONGOING_NOTE,
                                            loggedInSap->SAPName(),
                                            iCoeEnv );
                        CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( ETrue );
                        dlg->ExecuteLD( *prompt );
                        CleanupStack::PopAndDestroy( prompt );
                        CleanupStack::PopAndDestroy( loggedInSap );
                        CleanupStack::PopAndDestroy( sap );
                        return;
                        }
                    // this is the server we are logged in to
                    else if ( !ConfirmLogoutL() )
                        {
                        // user declined logout, we cannot delete this server
                        CleanupStack::PopAndDestroy( 2 ); // loggedInSap, sap
                        return;
                        }
                    }
                }
            CleanupStack::PopAndDestroy( loggedInSap );
            }
        // ignore not found error, since then there cannot be active connection to disconnect
        else if ( err != KErrNotFound )
            {
            User::Leave( err );
            }
        else
            {
            CleanupStack::PopAndDestroy( sessionSlotID );
            }

        // Ask confirmation from user about deletion
        HBufC* prompt = prompt = StringLoader::LoadLC(
                                     R_QTN_CHAT_DELETE_COMMUNITY,
                                     sap->SAPName() );

        // Ask common confirmation
        TInt confirmationResult = ExecuteConfirmationDialogL( *prompt );
        if ( confirmationResult == 0 )
            {
            CleanupStack::PopAndDestroy( 2, sap );  // prompt, sap
            return;
            }

        CleanupStack::PopAndDestroy( prompt );
        prompt = NULL;

        if ( isDefaultSap && sapCount > 1 )

            {
            // If current server is deleted, we need new server to use.
            // The default SAP cannot be deleted from SAP settings store so
            // we have to query this before the actual deletion
            CWVSettingsUIServerSelector::ExecuteSelectionQueryLD(
                *iSAPSettingsStore, *iDialog, EFalse, ETrue );
            }

        // do the actual deletion
        iSAPSettingsStore->DeleteSAPL( sap->Uid() );

        // set the current index
        TInt index( iListBox->CurrentItemIndex() );

        if ( --index < 0 )
            {
            index = 0;
            }
        else if ( index + 1 < iListBox->BottomItemIndex() )
            {
            // this is not the last item, we need to add one
            index++;
            }
        // Remove data also from internal buffer
        UpdateModelL();

        iListBox->HandleItemRemovalL();
        iListBox->SetCurrentItemIndexAndDraw( index );

        if ( iListBox->Model()->NumberOfItems() == 0 )
            {
            // Update CBA
            CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
            cba->SetCommandSetL( R_WVSETTINGSVIEW_SOFTKEYS_OPTIONS_BACK__EMPTY );
            cba->DrawNow();
            }

        DrawNow();
        }

    CleanupStack::PopAndDestroy( sap );
    }
//	}

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::ShowBrandProtectionNoteL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::ShowBrandProtectionNoteL()
    {
    HBufC* noteText = iEikonEnv->AllocReadResourceLC( R_QTN_WVSETTINGSVIEW_BRAND_SERVER_DELNOTE );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy( noteText ); // noteText
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::ConfirmLogoutL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIServerListDialog::ConfirmLogoutL()
    {
    // check if pec and chat are logged in
    TBool pecLogged ( iConnectionUI->LoggedInL( EIMPSConnClientPEC ) );
    TBool imLogged ( iConnectionUI->LoggedInL( EIMPSConnClientIM ) );

    if ( pecLogged || imLogged )
        {
        // ask confirmation about logging out
        CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
        CleanupStack::PushL( dlg );
        // Set prompt
        HBufC* text = CEikonEnv::Static()->AllocReadResourceLC(
                          R_WVSETTINGSVIEW_ERROR_ID_ACTIVE_CONNECTION );
        dlg->SetPromptL( *text );
        CleanupStack::PopAndDestroy( text );
        CleanupStack::Pop( dlg );
        TInt ret( dlg->ExecuteLD( R_ACTIVE_CONNECTION_CONFIRMATION_QUERY ) );

        if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
            {
            iDialog->ChangeToManualL();
            // user confirmed logout -> disconnect all open connections
            CPEngNWSessionSlotID2* sessionSlotID =
                iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
            CleanupStack::PushL( sessionSlotID );
            User::LeaveIfError( iConnectionUI->LogoutL( *sessionSlotID ) );
            CleanupStack::PopAndDestroy( sessionSlotID );
            return ETrue;
            }
        else
            {
            // the user declined the logout, we can just return EFalse
            return EFalse;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::ExecuteConfirmationDialogL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIServerListDialog::ExecuteConfirmationDialogL( const TDesC& aPrompt )
    {
    // Show confirmation
    CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dlg );
    dlg->SetPromptL( aPrompt );
    CleanupStack::Pop( dlg );
    return dlg->ExecuteLD( R_WVSETTINGSVIEW_CONFIRMATION_QUERY );
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::GetSAPL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIMPSSAPSettings* CWVSettingsUIServerListDialog::GetSAPL( TInt aIndex )
    {
    // the index cannot be less than zero
    if ( aIndex < 0 )
        {
        User::Leave( KErrNotFound );
        }
    TPtrC name( iServerListAdapter->RawDataMdcaPoint( aIndex ) );

    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();
    iSAPSettingsStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );
    TInt index( 0 );

    if ( sapList->FindNameL( name, index ) == 0 )
        {
        TUint32 uid( sapList->UidForIndex( index ) );
        const CIMPSSAPSettingsListItem* sapItem = sapList->ListItemForUid( uid );
        iSAPSettingsStore->GetSAPL( sapItem->Uid(), sap );
        CleanupStack::Pop( sap );
        }
    else
        {
        CleanupStack::PopAndDestroy( sap );
        sap = NULL;
        }

    CleanupStack::PopAndDestroy( sapList );

    return sap;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::UpdateModelL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::UpdateModelL()
    {

    CIMPSSAPSettingsList* newList = CIMPSSAPSettingsList::NewLC();
    iSAPSettingsStore->PopulateSAPSettingsListL( *newList, EIMPSIMAccessGroup );
    TInt index( newList->IndexForUid( iCurrent ) );
    iCurrent = 0;
    if ( iAdded )
        {
        iAdded = EFalse;
        TInt newIndex = newList->IndexForUid( iNewUid );
        if ( newIndex > KErrNotFound )
            {
            index = newIndex;
            }
        }

    // ReplaceModel can only leave if newList is NULL, so we can pop the newList here
    CleanupStack::Pop( newList );

    // Ownership of newList is transferred to iServerListAdapter
    iServerListAdapter->ReplaceModelL( newList );

    // we don't want to go below zero
    if ( index < 0 )
        {
        index = 0;
        }

    // set the correct focus
    iListBox->SetCurrentItemIndex( index );

    if ( newList )
        {
        if ( newList->Count() > 0 )
            {
            // check that the focus stays on the screen
            if ( iListBox->CurrentItemIndex() > ( newList->Count() - 1 ) )
                {
                iListBox->SetCurrentItemIndex( newList->Count() - 1 );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::LoadListboxIconArrayL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::LoadListboxIconArrayL()
    {

    // Create array for listbox graphics
    CAknIconArray* icons = new ( ELeave ) CAknIconArray( 1 ); // one icon in array
    CleanupStack::PushL( icons );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    TFileName bitmapfilepath;
    bitmapfilepath.Append( TParsePtrC( PathInfo::RomRootPath() ).Drive() );
    bitmapfilepath.Append( KDC_APP_BITMAP_DIR() );
    bitmapfilepath.Append( KBitmapFile() );

    AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),
                             KAknsIIDQgnIndiSettProtectedAdd,
                             bitmap, mask, bitmapfilepath,
                             EMbmApsettingsQgn_indi_sett_protected_add,
                             EMbmApsettingsQgn_indi_sett_protected_add_mask );

    icons->AppendL( CGulIcon::NewL( bitmap, mask ) );
    CleanupStack::Pop( 2 ); // mask, bitmap

    // Transfer ownership to listbox
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    CleanupStack::Pop( icons );
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::ProcessCommandL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::ProcessCommandL( TInt aCommand )
    {
    HideMenu();

    switch ( aCommand )
        {
        case EAknSoftkeyEdit:               // Flowthrough, MSK command
        case EWVSettingsViewServerlistNew:  // Flowthrough
        case EWVSettingsViewServerlistEdit:
            {
            LaunchCorrectDialog( aCommand );
            break;
            }

        case EWVSettingsViewServerlistDelete:
            {
            DeleteSelectedServerL();
            break;
            }
        // Help key support EWVSettingsViewCmdHelp is assigned EAknCmdHelp in
        // CWSettingsUI.hrh
        case EWVSettingsViewCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( CEikonEnv::Static()->WsSession(),
                                                 ( CCoeEnv::Static()->AppUi() )->AppHelpContextL () );
            break;
            }

        case EWVSettingsViewCmdExit:
            {
            TryExitL( aCommand );
            break;
            }
        default:
            {
            CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::HandleListBoxEventL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::HandleListBoxEventL( CEikListBox* aListBox,
                                                         TListBoxEvent aEventType )
    {
    if ( aEventType != EEventEnterKeyPressed &&
         aEventType != EEventItemDoubleClicked )
        {
        return;
        }

    if ( aListBox == iListBox )
        {
        ProcessCommandL( EWVSettingsViewServerlistEdit );
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::OfferKeyEventL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CWVSettingsUIServerListDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                            TEventCode aType )
    {

    if ( aType == EEventKey && ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter ) )
        {
        HandleListBoxEventL( iListBox, EEventEnterKeyPressed );
        return EKeyWasConsumed;
        }
    else if ( aType == EEventKey && aKeyEvent.iCode == EKeyBackspace )
        {
        if ( iListBox->Model()->NumberOfItems() == 0 )
            {
            return EKeyWasConsumed;
            }
        DeleteSelectedServerL();
        return EKeyWasConsumed;
        }

    return CEikDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::DynInitMenuPaneL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {

    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_WVSETTINGSVIEW_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dimm the help menu item
        aMenuPane->SetItemDimmed( EWVSettingsViewCmdHelp, ETrue );
        }
    else if ( aResourceId == R_WVSETTINGSVIEW_SERVERLIST_MENUPANE )
        {
        //
        //  Based on Variation flag, Enable Locking Server Setting which removes
        //  adding/deleting New Server option in Settings View.
        TBool lockServerSettings = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_LOCK_SERVER_SETTING );

        if ( iServerListAdapter->MdcaCount() == 0 )
            {
            aMenuPane->DeleteMenuItem( EWVSettingsViewServerlistEdit );
            aMenuPane->DeleteMenuItem( EWVSettingsViewServerlistDelete );
            }
        else if ( lockServerSettings )
            {
            aMenuPane->DeleteMenuItem( EWVSettingsViewServerlistNew );
            aMenuPane->DeleteMenuItem( EWVSettingsViewServerlistDelete );
            }
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::PreLayoutDynInitL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIServerListDialog::PreLayoutDynInitL()
    {
    RefreshTitleL();

    iSAPSettingsStore->AddObserverL( this, EIMPSIMAccessGroup );

    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    iSAPSettingsStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );
    // Ownership of sapList is transferred to iServerListAdapter
    CleanupStack::Pop( sapList );
    // after this iProtectedServers contain protected servers' indexes

    iServerListAdapter = CWVSettingsUIServersArray::NewL( *sapList, iProtectedServers );

    iListBox =
        static_cast< CAknSingleStyleListBox* >( Control( EWVSettingsViewControlIdServerList ) );
    LoadListboxIconArrayL();
    HBufC* emptyText = iEikonEnv->AllocReadResourceLC( R_WVSETTINGSVIEW_SERVERLIST_NOSERV );

    iListBox->Model()->SetItemTextArray( iServerListAdapter );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );
    iListBox->HandleItemAdditionL();
    iListBox->SetListBoxObserver( this );
    iListBox->UpdateScrollBarsL();
    iListBox->View()->SetListEmptyTextL( *emptyText );

    iDlgLauncher = new ( ELeave ) CAsyncCallBack(
        TCallBack( LaunchDlgCallback, this ),
        CActive::EPriorityStandard );

    CleanupStack::PopAndDestroy( emptyText );
    }

// ---------------------------------------------------------
// CWVSettingsUIServerListDialog::HandleActiveDbEvent()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIServerListDialog::HandleSAPEvent( TSAPEvent aEvent )
    {
// Waiting for SAP Settings store improvementes in event handling

    if ( aEvent == ESAPSettingChanged )
        {
        // we have to refresh the view
        // if editor is active, we should not refresh
        // when editor is exited the view will be refresed anyway
        CIMPSSAPSettings* sap = NULL;
        // we cannot leave here
        TRAPD( ignore, sap = GetSAPL( iListBox->CurrentItemIndex() ) );

        if ( sap )
            {
            iCurrent = sap->Uid();
            delete sap;
            }

        if ( !iEditorActive )
            {
            // we cannot leave here, the display just wont be up-to-date
            TRAP( ignore, UpdateModelL() );
            }
        }
    }

// ---------------------------------------------------------
// CWVSettingsUIServerListDialog::MappedCommandId()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CWVSettingsUIServerListDialog::MappedCommandId( TInt aButtonId )
    {
    // change cancel id so avkon don't remap it to 0
    if ( aButtonId == EEikBidCancel )
        {
        return KErrDied;
        }
    return aButtonId;
    }

// ---------------------------------------------------------
// CWVSettingsUIServerListDialog::OkToExitL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CWVSettingsUIServerListDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyEmpty )
    	{
    	return EFalse;
    	}
    if ( iEditorActive )
    	{
    	return EFalse;
    	}
    if ( aButtonId == EAknSoftkeyEdit )
        {
        // MSK command
        ProcessCommandL( aButtonId );
        return EFalse;
        }

    return CAknDialog::OkToExitL( aButtonId );
    }

// ---------------------------------------------------------
// CWVSettingsUIServerListDialog::LaunchCorrectDialog()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIServerListDialog::LaunchCorrectDialog( TInt aCommand )
    {

    if ( GetTargetDlgFlag() )
        {
        return;
        }

    if ( !iDlgLauncher->IsActive() )
        {
        iDlgCommand = aCommand;
        iDlgLauncher->Call();
        }
    }

// ---------------------------------------------------------
// CWVSettingsUIServerListDialog::LaunchDlgCallback()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CWVSettingsUIServerListDialog::LaunchDlgCallback( TAny* aDlg )
    {
    CWVSettingsUIServerListDialog* dlg =
        static_cast<CWVSettingsUIServerListDialog*>( aDlg );


    dlg->SetTargetDlgFlag( ETrue );
    TRAP_IGNORE( dlg->DoLaunchCorrectDialogL() );
    dlg->SetTargetDlgFlag( EFalse );
    return EFalse;
    }

// ---------------------------------------------------------
// CWVSettingsUIServerListDialog::DoLaunchCorrectDialogL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIServerListDialog::DoLaunchCorrectDialogL()
    {
    switch ( iDlgCommand )
        {
        case EWVSettingsViewServerlistNew:
            {
            TWVSettingsDialogReturnValue returnValue( DisplayServerEditorDialogL(
                                                          CWVSettingsUIDefineNewServerDialog::ENewServer ) );
            // If dialog was shutdown by a dialog shutter. don't touch internal variables.
            if ( returnValue != EWVSettingsViewForceExit )
                {
                iAdded = ETrue;
                UpdateModelL();
                RefreshTitleL();
                iListBox->HandleItemAdditionL();
                if ( returnValue == EWVSettingsViewExitCalled )
                    {
                    TryExitL ( EWVSettingsViewCmdExit );
                    }
                }
			else
				{
				TryExitL ( EAknSoftkeyCancel );
				}
            break;
            }

        case EAknSoftkeyEdit:   // Flowthrough, MSK command
        case EWVSettingsViewServerlistEdit:
            {
            TInt listboxIndex = iListBox->CurrentItemIndex();
            // if there are no items in the list, we can just return
            if ( listboxIndex < 0 )
                {
                return;
                }
            delete iSAP;
            iSAP = NULL;
            iSAP = GetSAPL( iListBox->CurrentItemIndex() );

            // If a matching SAP for current item index can't be found a SAP then
            // we have a synchronization problem

            if ( iSAP )
                {
                TWVSettingsDialogReturnValue returnValue = DisplayServerEditorDialogL(
                                                               CWVSettingsUIDefineNewServerDialog::EEditServer );
                // If dialog was shutdown by a dialog shutter. don't touch internal variables.
                if ( returnValue != EWVSettingsViewForceExit )
                    {
                    UpdateModelL();
                    RefreshTitleL();
                    iListBox->HandleItemAdditionL();
                    if ( iListBox->Model()->NumberOfItems() == 0 )
                        {
                        // Update CBA
                        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
                        cba->SetCommandSetL( R_WVSETTINGSVIEW_SOFTKEYS_OPTIONS_BACK__EMPTY );
                        cba->DrawNow();
                        }
                    if ( returnValue == EWVSettingsViewExitCalled )
                        {
                        ProcessCommandL( EWVSettingsViewCmdExit );
                        }
                    }
    			else
    				{
    				TryExitL ( EAknSoftkeyCancel );
    				}
                }
            else
                {
                iListBox->HandleItemRemovalL();
                }

            break;
            }
        default:
            {
            // nothing to do
            }
        }
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::SetTargetDlgFlag()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIServerListDialog::SetTargetDlgFlag( TBool aFlag )
    {
    iIsOpenedTargettDlg = aFlag;
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::SetTargetDlgFlag()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CWVSettingsUIServerListDialog::GetTargetDlgFlag()
    {
    return iIsOpenedTargettDlg;
    }

/**
 * Help key support - GetHelpContext is called by the framework on
 * event EAknCmdHelp
 * --------------------------------------------------------------------------------
 * CWVSettingsUIDefineNewServerDialog::GetHelpContext
 * @param 	aContext	The context dependent on the current view is passed by the framework.
 */
void CWVSettingsUIServerListDialog::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_SERVLISTSETT_IM ;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIServerListDialog::IntResourceValueL()
// @param aResourceId 	The resource ID to be fetched from Cenrep (if present) or else from the variation file
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIServerListDialog::IntResourceValueL( TInt aResourceId )
    {

    TInt val( 0 );
    TInt err ( KErrNone );
    CRepository* rep = 0;

    TRAP( err, rep = CRepository::NewL( KCRUidIMNG ) );

    if ( err == KErrNone )
        {
        TInt key = aResourceId + KIMCUStartVariationID;

        err = rep->Get( key, val );

        delete rep;
        }

    if ( err != KErrNone )
        {

        TResourceReader reader;

        aResourceId = aResourceId + RSC_CRRSS_CHAT_VARIATION_IMPSCU_START_ID;

        CCoeEnv::Static()->CreateResourceReaderLC(
            reader,
            aResourceId );
        val = reader.ReadInt32();
        CleanupStack::PopAndDestroy(); // reader

        }

    return val;

    }
//  End of File
