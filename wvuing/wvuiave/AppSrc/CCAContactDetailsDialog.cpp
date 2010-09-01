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
* Description:  Contact details dialog.
 *
*/


// INCLUDE FILES
#include	"CCAContactDetailsDialog.h"
#include	"chatngclient.hrh"
#include	"CCAAppUi.h"
#include    "CCAUISessionManager.h"
#include	"CCAAccessArray.h"
#include	"CCAStatusPaneHandler.h"
#include    "ChatDefinitions.h"
#include    "CAExternalInterface.h"
#include    "ChatDebugPrint.h"
#include    "ChatDebugAssert.h"
#include    "IMUtils.h"
#include    "IMDialogUtils.h"
#include    "IMNoteMapper.h"
#include    "CAUtils.h"
#include    "CCAContactEditor.h"
#include    "ccafetchwaiter.h"
#include    "ccasyncchecker.h"
#include    "CCAApp.h"
#include    "CCAContactSelectionDialog.h"
#include 	"MCAContactEditPC.h"
#include	"MCAProcessManager.h"
#include    "CCAPCUtils.h"
#include    "CCABlockingUI.h"
#include    "MCABlockingPC.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


#include	<aknlists.h>
#include	<ChatNG.rsg>
#include	<aknpopup.h>
#include	<AknMessageQueryDialog.h>
#include	<stringloader.h>
#include	<eikmenub.h>
#include    <hlplch.h>
#include    <csxhelp/imng.hlp.hrh>
#include    <featmgr.h>
#include    <IMPSerrors.H>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAContactDetailsDialog::CCAContactDetailsDialog(
    TBool& aWasSaved,
    const HBufC* aListId,
    const TDesC& aNickname,
    TInt aIndex )
        :   iListId( aListId ),
        iNickname( aNickname ),
        iWasSaved( aWasSaved ),
        iOwnData( EFalse ),
        iValidContact( ETrue ),
    iIndexOfContact(aIndex),
    iMovingFlag( EFalse )
    {
    // nothing..
    }

// ---------------------------------------------------------
// CCAContactDetailsDialog::NewL
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAContactDetailsDialog* CCAContactDetailsDialog::NewL(
    TBool& aWasSaved,
    TInt aIndex,
    const TDesC& aListId,
    const TDesC& aNickname /* = KNullDesC */
)
    {
    HBufC* list = aListId.AllocLC();

    CCAContactDetailsDialog* temp = new ( ELeave ) CCAContactDetailsDialog(
        aWasSaved,
        list,
        aNickname,
        aIndex
    );

    CleanupStack::Pop( list );

    CleanupStack::PushL( temp );
    temp->ConstructL();
    CleanupStack::Pop( temp );
    return temp;
    }

// ---------------------------------------------------------
// CCAContactDetailsDialog::ConstructL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactDetailsDialog::ConstructL()
    {
    CAknDialog::ConstructL( R_CHAT_CDETAILS_MENUBAR );

    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    iTitlePane = appUi->CAStatusPane();

    iItemTextArray = new ( ELeave ) CDesCArrayFlat( 5 );

    iAliasEnabled = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVE_ALIAS );
    iNicknameEnabled = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVE_NICKNAME );


    iCCAContactEditPC = appUi->GetProcessManager().GetContactEditInterface();

    iCCAContactEditPC->InitialiseL( *appUi->GetProcessManager().GetArrayInterface(),
                                    iIndexOfContact, iOwnData ) ;


    //contact should have been initialised
    if ( ! iCCAContactEditPC->IsContact() )
        {
        // can't be NULL. you should use ContactEditor in this case.
        __CHAT_ASSERT_DEBUG( iCCAContactEditPC->IsContact() );
        }

    // store contactid so it's possible to verify the contact
    // without iContact pointer.
    iContactId = iCCAContactEditPC->GetCurrentContactUserId().AllocL();
    }

// Destructor
CCAContactDetailsDialog::~CCAContactDetailsDialog()
    {
    // dialog will be gone, so tab groups can be drawn again
    if ( iTitlePane )
        {
        iTitlePane->RestoreNaviPane();
        }

    if ( iItemTextArray )
        {
        iItemTextArray->Reset();
        }
    delete iItemTextArray;
    delete iListId;
    delete iContactId;

    iIndexToControlIdMapping.Close();

    // reset the iContact and iOwnData
    iCCAContactEditPC->ReleaseResources();
    }

// ---------------------------------------------------------
// CCAContactDetailsDialog::ProcessCommandL( TInt aCommand )
// This method catches commands
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactDetailsDialog::ProcessCommandL( TInt aCommand )
    {
    HideMenu();

    // Verify that contact is still logged in
    switch ( aCommand )
        {
            // commands which require login
        case EChatClientDetailsEdit:
        case EChatClientDetailsDelete: // fallthrough
            {
            CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );

            if ( !appUi->UISessionManager().LoginL( MCAUiLoginCmdCB::EQueryLogin ) )
                {
                // Login unsuccessfull. Cannot process command, so returning.
                // Do not leave because this can happen when login is failed
                // for some reason like cancel pressed by user etc.
                return;
                }


            // wait for background fetch
            if ( !CCASyncChecker::CheckSyncStateL() )
                {
                // canceled or not logged in
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
        case EEikCmdExit:	// falltrough
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

        case EChatClientDetailsEdit:
            {
            // launch contact editor to edit contact
            // with highlight in the given field
            LaunchContactEditorL();
            break;
            }

        case EChatClientDetailsDelete:
            {
            // This will close the dialog on success.
            DeleteCurrentContactL();
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
// CCAContactDetailsDialog::DynInitMenuPaneL
// Dynamically initialises menu pane
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactDetailsDialog::DynInitMenuPaneL( TInt aResourceId,
                                                CEikMenuPane* aMenuPane )
    {
    CHAT_DP_TXT( "CCAContactDetailsDialog::DynInitMenuPaneL" );

    if ( !iCCAContactEditPC->IsContact() ||
         ( aResourceId == R_CHAT_CDETAILS_MENUPANE && iOwnData ) )
        {
        // hide Delete menu option
        aMenuPane->SetItemDimmed( EChatClientDetailsDelete, ETrue );
        }

    // dim the help menu if it's not supported
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dim the help menu item
        aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
        }

    CHAT_DP_TXT( "CCAContactDetailsDialog::DynInitMenuPaneL ENDS" );
    }

// ---------------------------------------------------------
// CCAContactDetailsDialog::PreLayoutDynInitL()
// Sets the layout for dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactDetailsDialog::PreLayoutDynInitL()
    {
    // create the listbox
    CAknDoubleStyleListBox* listBox = static_cast< CAknDoubleStyleListBox* >
                                      ( Control( EChatClientControlIdDetails ) );

    listBox->SetListBoxObserver( this );

    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    listBox->HandleItemAdditionL();
    listBox->UpdateScrollBarsL();

    listBox->Model()->SetItemTextArray( iItemTextArray );
    listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // enable marquee
    listBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    UpdateItemTextArrayL();

    UpdateTitleBarL();
    }

// ---------------------------------------------------------
// CCAContactDetailsDialog::OkToExitL( TInt aButtonId )
// Handles buttonids
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAContactDetailsDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        return EFalse;
        }
    if ( aButtonId == EAknSoftkeyContextOptions )   // MSK command, show context
        // sensitive options menu
        {
        // Currently open contact editor with MSK
        ProcessCommandL( EChatClientDetailsEdit );
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CCAContactDetailsDialog::OfferKeyEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCAContactDetailsDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                      TEventCode aType )
    {
    switch ( aKeyEvent.iCode )
        {
        case EKeyEscape:    // Handle dialog shutdown
            {
            TryExitL( EAknSoftkeyExit );
            return EKeyWasNotConsumed;
            }
        case EKeyEnter:     // Flowthrough
        case EKeyOK:
            {
            ProcessCommandL( EChatClientDetailsEdit );
            return EKeyWasConsumed;
            }
        case EKeyBackspace:
            {
            // This will close the dialog on success.
            ProcessCommandL( EChatClientDetailsDelete );
            return EKeyWasConsumed;
            }
        case EKeyPhoneSend:
            {
            return EKeyWasConsumed;
            }
        default:
            {
            break;
            }
        }

    CAknDoubleStyleListBox* listBox = static_cast< CAknDoubleStyleListBox* >
                                      ( Control( EChatClientControlIdDetails ) );

    // pass to the listbox
    return listBox->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CCAContactDetailsDialog::HandleListBoxEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactDetailsDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                   TListBoxEvent /*aEventType*/ )
    {
    }

// -----------------------------------------------------------------------------
// CCAContactDetailsDialog::UpdateItemTextArrayL
// -----------------------------------------------------------------------------
//
void CCAContactDetailsDialog::UpdateItemTextArrayL()
    {
    // The contact given as parameter for us exists always, because we are
    // viewing or editing it.  If it didn't exist (NULL) we should be in
    // Contact Editor and Contact Details Dialog should not have been
    // invoked.
    // The contact can never be NULL once we reach this method
    __CHAT_ASSERT_DEBUG( iCCAContactEditPC->IsContact() );

    iItemTextArray->Reset();
    iIndexToControlIdMapping.Reset();

    // online status - can't be empty
    HBufC* statusString = NULL;

    switch ( iCCAContactEditPC->GetOnlineStatus() )
        {
        case TEnumsPC::EUnknown :
            {
            statusString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_STATUS_UNKNOWN );
            break;
            }

        case TEnumsPC::EOffline :
            {
            statusString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_STATUS_OFFLINE );
            break;
            }

        case TEnumsPC::EOnline :
            {
            statusString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_STATUS_ONLINE );
            break;
            }

        case TEnumsPC::EInvisible :
            {
            // this can only come for own status item, no need to check since
            // for other contacts this state is impossible
            statusString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_STATUS_APPEAR_OFFLINE );
            break;
            }

        case TEnumsPC::EAway :
            {
            statusString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_STATUS_AWAY );
            break;
            }

        case TEnumsPC::EBusy :
            {
            statusString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_STATUS_BUSY );
            break;
            }


        default :
            {
            // can't get here
            break;
            }
        }
    AppendToItemArrayL( *iItemTextArray, R_QTN_CHAT_CD_STATUS, *statusString );
    iIndexToControlIdMapping.Append( EChatEditDummy ); // no control for changing presence
    CleanupStack::PopAndDestroy( statusString );

    // status message
    TPtrC temp( iCCAContactEditPC->GetStatusText() );
    if ( temp.Length() > 0 )
        {
        // show non-empty message
        AppendToItemArrayL( *iItemTextArray, R_QTN_CHAT_CD_MESSAGE, temp );
        iIndexToControlIdMapping.Append( EChatEditDummy ); // no control for changing message
        }

    // device type - can't be empty
    HBufC* deviceString = NULL;

    switch ( iCCAContactEditPC->GetClientType() )
        {
        case TEnumsPC::EUnknownClient :
            {
            deviceString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_DEVICE_UNKNOWN );
            break;
            }

        case TEnumsPC::EMobile :
            {
            deviceString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_DEVICE_MOBILE );
            break;
            }

        case TEnumsPC::EPC :
            {
            deviceString =
                iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_DEVICE_PC );
            break;
            }

        default :
            {
            // can't get here
            break;
            }
        }
    AppendToItemArrayL( *iItemTextArray, R_QTN_CHAT_CD_DEVICE, *deviceString );
    iIndexToControlIdMapping.Append( EChatEditDummy ); // no control for changing device
    CleanupStack::PopAndDestroy( deviceString );

    // WVID - we always have this

    //To Remove wv and domain part from "WV User Id" Field in Contact Details Dialog
    TPtrC userIdName = CCAPCUtils::DisplayId( iCCAContactEditPC->GetCurrentContactUserId() );
    HBufC16* wvID = userIdName.AllocLC();
    AppendToItemArrayL( *iItemTextArray, R_QTN_CHAT_CD_WV_ID, *wvID );
    CleanupStack::PopAndDestroy( wvID );
    iIndexToControlIdMapping.Append( EChatEditWV );

    // alias
    if ( iAliasEnabled )
        {
        temp.Set( iCCAContactEditPC->GetCurrentContactAlias() );
        if ( temp.Length() > 0 )
            {
            // show non-empty alias
            AppendToItemArrayL( *iItemTextArray, R_QTN_CHAT_CD_ALIAS, temp );
            iIndexToControlIdMapping.Append( EChatEditAlias );
            }
        }

    // nickname
    if ( ! iOwnData && iNicknameEnabled )
        {
        // nickname is not available for "own data" view
        temp.Set( iCCAContactEditPC->GetCurrentContactNickname() );
        if ( temp.Length() > 0 )
            {
            // show non-empty nickname
            AppendToItemArrayL( *iItemTextArray, R_QTN_CHAT_CD_NICK,
                                iCCAContactEditPC->GetCurrentContactNickname() );
            iIndexToControlIdMapping.Append( EChatEditNick );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAContactDetailsDialog::AppendToItemArrayL
// -----------------------------------------------------------------------------
//
void CCAContactDetailsDialog::AppendToItemArrayL(
    CDesCArray& aItemTextArray,
    TInt aStringResource,
    const TDesC& aText )
    {
    // the length should be enough for every string used here. e.g. 50 is
    // nickname max length. at the same time, it should be generic enough for
    // alias, nickname, wvid, etc. so let's choose 120
    HBufC* temp = HBufC::NewLC( 120 );
    HBufC* label = iEikonEnv->AllocReadResourceLC( aStringResource );
    TPtr pTemp( temp->Des() );
    // format properly for the list box
    pTemp.Append( KTab );
    pTemp.Append( *label );
    pTemp.Append( KTab );
    pTemp.Append( aText );
    CleanupStack::PopAndDestroy( label );
    aItemTextArray.AppendL( *temp );
    CleanupStack::PopAndDestroy( temp );
    }

// -----------------------------------------------------------------------------
// CCAContactDetailsDialog::ShowLD
// -----------------------------------------------------------------------------
//
void CCAContactDetailsDialog::ShowLD()
    {
    // show and die
    ExecuteLD( R_CHAT_CEDITOR_DETAILS );
    }

// -----------------------------------------------------------------------------
// CCAContactDetailsDialog::MapIndexToControlId
// -----------------------------------------------------------------------------
//
TInt CCAContactDetailsDialog::MapIndexToControlId( TInt aItemIndex )
    {
    TInt maxArrayIndex = iIndexToControlIdMapping.Count();
    TInt index = aItemIndex;

    // sanity
    __CHAT_ASSERT_DEBUG( index < maxArrayIndex );

    // find suitable position
    TInt iter = 0;
    do
        {
        if ( iIndexToControlIdMapping[ index ] != EChatEditDummy )
            {
            // found a valid id, use it
            return iIndexToControlIdMapping[ index ];
            }

        // no available mapping, try to focus on next one

        // wrap to start of array if necessary
        index = ( index + 1 ) % maxArrayIndex;
        iter++;
        }
    while ( iter < maxArrayIndex ); // prevent infinite loop

    // didn't find it so let the editor decide
    return EChatEditDummy;
    }

// -----------------------------------------------------------------------------
// CCAContactDetailsDialog::UpdateTitleBarL
// -----------------------------------------------------------------------------
//
void CCAContactDetailsDialog::UpdateTitleBarL()
    {
    iTitlePane->ClearNaviPaneL();

    // set correct title
    HBufC* title = NULL;
    if ( iOwnData )
        {
        // "own data"
        title = iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_CD_OWN_DATA_TITLE );
        }
    else
        {
        // identification
        title = iCCAContactEditPC->GetCurrentContactIdentification().AllocLC();
        }

    iTitlePane->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    // set own status, but for landscape mode only
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->UISessionManager().SetStatusPaneIconsL();

    // when the dialog is active, do not show any tabs even if the underlying
    // view would like to show them
    iTitlePane->ProhibitTabGroup();
    }

// -----------------------------------------------------------------------------
// CCAContactDetailsDialog::DeleteCurrentContactL
// -----------------------------------------------------------------------------
//
TInt CCAContactDetailsDialog::DeleteCurrentContactL()
    {
    // can't delete self
    if ( iOwnData )
        {
        return KErrAccessDenied;
        }

    if ( !iCCAContactEditPC->IsContactValidL( *iListId ) )
        {
        // user does not exist anymore
        IMNoteMapper::ShowNoteL( KErrArgument );
        return KErrArgument;
        }

    iMovingFlag = ETrue;
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );

    // Get the name of the friend to be deleted
    HBufC* contactId = iCCAContactEditPC->GetCurrentContactIdentification().AllocLC();

    // Load and format confirmation string from resource
    HBufC* prompt = StringLoader::LoadLC( RSC_CHAT_DELETE_CONFIRMATION, *contactId );

    TInt ret( IMDialogUtils::DisplayQueryDialogL(
                  R_GENERIC_YES_NO_CONFIRMATION_QUERY, *prompt ) );

    TInt error( KErrCancel );

    // Check what the user responded
    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        // accepted, lets delete the contact
        appUi->ShowWaitDialogLC( R_QTN_CHAT_DELETING_CONTACT );

        HBufC* contactUserId = iCCAContactEditPC->GetCurrentContactUserId().AllocLC();
        // delete contact
        error = iCCAContactEditPC->RemoveContactL( *iListId,
                                                   *contactUserId );

        // Dismiss the delete waiting dailog
        appUi->DismissWaitDialogL( KErrNone );

        if ( error <= Imps_ERROR_BASE )
            {
            // server error
            IMNoteMapper::ShowNoteL( error );
            }

        if ( error == KErrNone )
            {
            // Start
#ifdef IMPS_BLOCK_AFTER_DELETE
            TBool blockVariation( IMUtils::IntResourceValueL(
                                      RSC_CHAT_VARIATION_BLOCK_QUERY ) );
            if ( blockVariation )
                {
                HBufC* deletePrompt = StringLoader::LoadLC( R_QTN_CHAT_BLOCKING_QUERY, *contactId );
                // no error and it's a contact
                TInt block = IMDialogUtils::DisplayYesNoConfirmationDialogL( *deletePrompt );
                if ( block )
                    {
                    MCABlockingPC* blockingPC = appUi->GetProcessManager().GetBlockingInterface( );
                    CCABlockingUI* blockingUI = CCABlockingUI::NewLC( blockingPC );
                    // block after delete requested
                    blockingUI->BlockUserL( *contactUserId );
                    CleanupStack::PopAndDestroy( blockingUI );
                    }
                //CodeScanner warning to be ignored
                CleanupStack::PopAndDestroy(  deletePrompt ); // deletePrompt
                }
#endif // IMPS_BLOCK_AFTER_DELETE
            // End
            // contact was successfully deleted, try to exit this dialog
            iWasSaved = EFalse;
            TryExitL( EAknSoftkeyCancel );
            }
        CleanupStack::PopAndDestroy(  contactUserId ); // contactUserId
        CleanupStack::PopAndDestroy(); //appUi->DismissWaitDialogL()
        }
    CleanupStack::PopAndDestroy( prompt );
    CleanupStack::PopAndDestroy( contactId );
    iMovingFlag = EFalse;
    return error;
    }

// -----------------------------------------------------------------------------
// CCAContactDetailsDialog::LaunchContactEditorL
// -----------------------------------------------------------------------------
//
void CCAContactDetailsDialog::LaunchContactEditorL()
    {
    if ( !iCCAContactEditPC->IsContactValidL( *iListId ) )
        {
        // user does not exist anymore
        IMNoteMapper::ShowNoteL( KErrArgument );
        return;
        }
    if( iMovingFlag )
    	{
    	return;
    	}
    CAknDoubleStyleListBox* listBox = static_cast< CAknDoubleStyleListBox* >
                                      ( Control( EChatClientControlIdDetails ) );
    TInt controlId = MapIndexToControlId( listBox->CurrentItemIndex() );

    // Use local variable in case we get deleted.
    TBool wasSaved( EFalse );

    //The contact details dialog already intialised the PC component related to ContactEdit
    //so contact editor need'nt initialise/release PC coponent resources
    CCAContactEditor* editor = CCAContactEditor::NewL( iIndexOfContact, wasSaved, EFalse );
    TInt retVal = editor->EditContactLD( controlId, TEnumsPC::EEditContact, *iListId );

    // Don't do anything if we have cancel
    if ( retVal != 0 )
        {

        if ( !iCCAContactEditPC->IsContact() )
            {
            // contact was deleted -> exit details dialog
            iWasSaved = EFalse;
            TryExitL( EAknSoftkeyCancel );
            }

        // Only update if we have saved. If we got shut down by dialog shutter
        // we can't use any internal variables.
        if ( wasSaved )
            {
            iWasSaved = wasSaved;

            // Update ID
            HBufC* id = iCCAContactEditPC->GetCurrentContactUserId().AllocL();
            delete iContactId;
            iContactId = id;

            UpdateItemTextArrayL();

            // Ui was getting distorted here

            CAknDoubleStyleListBox* listBox = static_cast< CAknDoubleStyleListBox* >
                                              ( Control( EChatClientControlIdDetails ) );

            listBox->HandleItemAdditionL();
            listBox->UpdateScrollBarsL();
            listBox->Model()->SetItemTextArray( iItemTextArray );
            listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

            UpdateTitleBarL();
            DrawNow();
            }
        }
    }

// ---------------------------------------------------------
// CCAContactDetailsDialog::SetSizeAndPosition
// ---------------------------------------------------------
//
void CCAContactDetailsDialog::SetSizeAndPosition( const TSize& /* aSize*/ )
    {

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    SetRect( rect );

    }

/**
 * Help Key Support
 * This function is called by the Framework to get the context to launch
 * Help
 *
 * @param aContext	The context that contains the appid and the help id.
 */
void CCAContactDetailsDialog::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    if ( iOwnData )
        {
        aContext.iContext = KIMNG_HLP_CONT_OWN_DATA ;
        }
    else
        {
        aContext.iContext = KIMNG_HLP_CONTACTDETAILS ;
        }
    }
//  End of File
