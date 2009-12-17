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
* Description:  Container for all controls in single-list view
 *
*/




//platform includes
#include	<aknlists.h>
#include	<aknsfld.h>
#include	<eikclbd.h>          	 // for columndata
#include	<chatng.rsg>
#include	<chatng.mbg>
#include	<aknlayout.lag>       	 // AVKON LAF layouts
#include	<aknlayoutdef.h>
#include	<aknnotedialog.h>
#include	<MdaAudioTonePlayer.h>
#include	<StringLoader.h>    	 // StringLoader
#include	<aknquerydialog.h>
#include    <aknpopup.h>        	 // CAknPopupList
#include    <barsread.h>
#include    <AknIconUtils.h>
#include	<AknDesCArrayDecorator.h>
#include    <EikMenuB.h>
#include    <AknListQueryDialog.h>   // CAknListQueryDialog
#include    <AknNoteWrappers.h>
#include    <AknIconArray.h>
//pbk2 changes
#include 	<tvpbkcontactstoreuriptr.h>
#include 	<cvpbkcontactstoreuriarray.h>
#include 	<mvpbkcontactstorelist.h>
#include 	<cvpbkcontactmanager.h>
#include 	<cvpbkfieldtypeselector.h>
#include 	<cvpbkcontactlinkarray.h>
#include 	<vpbkcontactstoreuris.h>
#include 	<vpbkcontactviewfilterbuilder.h>
#include 	<cpbk2storeconfiguration.h>
#include 	<cvpbkcontactstoreuriarray.h>
#include 	<MVPbkContactFieldTextData.h>
#include 	<MVPbkStoreContact.h>
#include 	<MVPbkContactOperationBase.h>
#include 	<CPbk2SortOrderManager.h>
#include 	<Pbk2ContactNameFormatterFactory.h>
#include 	<MPbk2ContactNameFormatter.h>
#include 	<MVPbkFieldType.h>
#include    <MVPbkContactLink.h>
#include    <MVPbkContactStore.h>
#include    <mvpbkcontactstorelistobserver.h>
#include    <PEngWVPresenceErrors2.h>   // wv error codes
#include    <AknGlobalNote.h>
#include    <csxhelp/imng.hlp.hrh>
#include    <MWVSettingsObserverng.h>
#include	"CAUtils.h"

#include    "ImpsCSPAllErrors.h"
#include    "ccasyncchecker.h"

// INCLUDE FILES
#include 	"MCASettingsPC.h"
#include	"TEnumsPC.h"
#include    "CCAAudioManager.h"
#include	"chatngclient.hrh"
#include    "CCAAppUi.h"
#include    "CCAChatListView.h"
#include	"CAExternalInterface.h"
#include	"CCAStatusPaneHandler.h"
#include    "ccacontactlistboxmodel.h"
#include	"MCABlocking.h"
#include	"MCAViewSwitcher.h"
#include    "chatdefinitions.h"
#include	"ChatDebugPrint.h"
#include    "IMNoteMapper.h"
#include    "CCABlockingUI.h"
#include    "CCAUISessionManager.h"
#include	"CCAUINGUtils.h"
#include    "IMUtils.h"
#include	"IMDialogUtils.h"
#include    "impsbuilddefinitions.h"
#include    "CCAContactDetailsDialog.h"
#include    "CCAGroupUtils.h"
#include    "MCASkinVariant.h"
#include    "CCAVariantFactory.h"
#include    "CCAApp.h"
#include    "CCAContactEditor.h"
#include    "ccacontactlistbox.h"
#include    "MCABlockingPC.h"
#include 	"MCACommand.h"
#include 	"MCAUiLoginCmdCB.h"
#include 	"CCACommandManager.h"
#include    "CCACommandManagerFactory.h"
#include 	"MCACommandHelper.h"
#include 	"MCAProcessManager.h"
#include 	"MCAMainViewArrayPC.h"
#include    "MCASearchDataPC.h"
#include    "MCASearchInterfacePC.h"
#include 	"CCASingleListViewArray.h"
#include 	"CCASingleListContainer.h"
#include    "CCAAppUi.h"
#include 	"CCAView.h"
#include    "MCAInvitationPC.h"
#include 	"MCAGroupPC.h"
#include	"MCAConversationPC.h"
#include	"CCAContactSelectionDialog.h"
#include	"MCAServerContactsArrayPC.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// Constants


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CCASingleListContainer::Constructor
// ---------------------------------------------------------
//
CCASingleListContainer::CCASingleListContainer
( MCACommandHelper& aCommandHelper, CCAView& aView, CCAAppUi* aAppUi )
        :   iAppUi( aAppUi ),
        iDeleteFlag( EFalse ),
        iTaskComplete( ETrue ),
        iCommandHelper( aCommandHelper ),
        iView( aView ),
        iBlocked( EFalse ), iIsMovingFlag( EFalse )
    {

    }

// ---------------------------------------------------------
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CCASingleListContainer::ConstructL( TRect aRect,
                                         MCAViewSwitcher& aViewSwitcher, TInt /* aSelectedIndexId */ )
    {

    CreateWindowL();
    iAppUi->AddLayoutChangeObserver( this );
    iForceExpandSingleList = ETrue;
    iStatusPane = iAppUi->CAStatusPane();
    if ( !iStatusPane )
        {
        User::Leave( KErrGeneral );
        }
    iViewSwitcher = &aViewSwitcher;

    iMainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();

    iMainViewArrayPC->AddObserverL( this );
    //to refresh group array
    if ( iAppUi->UISessionManager().IsLoggedIn() )
        {
        iMainViewArrayPC->PopulateGroupsListL();
        }
    // Create and construct listbox
    iListBox = new ( ELeave ) CCAContactListBox;

    iInfoPopup = CAknInfoPopupNoteController::NewL();

    iMainViewArrayPC->ForceExpanded( EFalse );

    iListBox->ConstructL( this );
    iListBox->SetContactListModel( *iMainViewArrayPC );
    iListBox->SetListBoxObserver( this );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // remove empty spaces from b-column (new Avkon-support)
    iListBox->UseEmptyIconSpace( ETrue );

    // set marquee on
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    MCASettingsPC* settings = iAppUi->GetProcessManager().GetSettingsInterface( );
    TInt resValue = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING );

    if ( !settings->GetBoolValuePC( TEnumsPC::EShowOffline, NULL ) )
        {
        iMyFilter = TEnumsPC::EFilterNonOffline;
        iShowOffline = EFalse;
        if ( resValue != 2 )
            {
            iMyFilter = TEnumsPC::EFilterNonOfflineNonBlockedAndOperation;
            }
        }
    else
        {
        iMyFilter = TEnumsPC::EFilterAll;
        iShowOffline = ETrue;
        if ( resValue != 2 )
            {
            iMyFilter = TEnumsPC::EFilterAllNonBlockedAndOperation;
            }
        }
    // type cast imyfilter to specific enum type here....
    iMainViewArrayPC->SetDefaultFilter( ( TEnumsPC::TFilterType )iMyFilter );

    iMainViewArrayPC->ResetDefaultFilter();

    //Array needs to be refreshed as filters have changed.

    iMainViewArrayPC->ResetArray( EFalse );

    //We DONT need a different filter for the list indicators.
    //The value on ORing and Anding as below is 21 always
    TEnumsPC::TFilterType singleListArrayFilter( TEnumsPC::EFilterNotDefined );
    if ( iMyFilter == TEnumsPC::EFilterNonOfflineNonBlockedAndOperation )
        {
        singleListArrayFilter = TEnumsPC::EFilterNonOfflineNonBlockedNonOfflineOperation;
        }
    else
        {
        singleListArrayFilter = TEnumsPC::EFilterAllNonBlockedNonOfflineOperation;
        }


    // Create single list array wrapper
    CCASingleListViewArray * singleListArray =
        CCASingleListViewArray::NewL(
            *iMainViewArrayPC,
            TEnumsPC::ESingleListViewSelect,
            iListBox->ItemDrawer()->ColumnData(),
            ETrue,
            settings->GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate , NULL ),
            *iListBox );

    iMainViewArrayPC->SetSingleListArrayFilter( ( TEnumsPC::TFilterType )singleListArrayFilter );
    singleListArray->SetContactListBoxModel( ( ( CCAContactListBoxModel* )iListBox->Model() ) );
    CHAT_DP( D_CHAT_LIT( " **** contacts in this list %d **** " ),
             singleListArray->MdcaCount() );

    // Ownership transfer to CAknSingleGraphicStyleListBox
    iListBox->Model()->SetItemTextArray( singleListArray );

    SetRect( aRect );
    if ( ListboxItems() )
        {
        // Create find-pane
        ActivateFindPaneL();
        }

    LoadBitmapsL();

    iSearchInterfacePC = iAppUi->GetProcessManager().GetSearchInterface();

    iDataInterfacePC = iAppUi->GetProcessManager().GetSearchDataInterface();

    iBlockingPC = iAppUi->GetProcessManager().GetBlockingInterface( );
    iBlockingUI = CCABlockingUI::NewL( iBlockingPC );
    SetCbaLockL( EFalse );
    iAppUi->AddServerChangeObsL( this );
    ActivateL();

    InitPbk2SingleEntryFetchL();
    }

// ---------------------------------------------------------
// CCASingleListContainer::Destructor
// ---------------------------------------------------------
//
CCASingleListContainer::~CCASingleListContainer()
    {
    if ( iAppUi )
        {
        iAppUi->RemoveServerChangeObs( this );
        }

    CHAT_DP_TXT( "CCASingleListContainer destructor starts" );
    if ( iAppUi && !iAppUi->IsUnderDestruction() )
        {
        iAppUi->RemoveLayoutChangeObserver( this );
        }

    if ( iMainViewArrayPC )
        {
        iMainViewArrayPC->RemoveObserver( this );
        }
    delete iListBox;
    delete iBlockingUI;
    delete iFindbox;
    delete iInfoPopup;

    ReleasePbkSingleEntryFetch();

    CHAT_DP_TXT( "CCASingleListContainer destructor ends" );
    }


// ---------------------------------------------------------
// CCASingleListContainer::SelectedType
// ---------------------------------------------------------
//
TEnumsPC::TItem CCASingleListContainer::SelectedType() const
    {
    return iMainViewArrayPC->GetType( CurrentListboxIndex() );
    }
// ---------------------------------------------------------
// CCASingleListContainer::OwnStatus
// ---------------------------------------------------------
//
TEnumsPC::TOnlineStatus CCASingleListContainer::OwnStatus() const
    {
    if ( iMainViewArrayPC->GetType( 0 ) == TEnumsPC::EOwnStatusItem )
        {
        return iMainViewArrayPC->GetOnlineStatus( 0 );
        }
    else
        {
        return TEnumsPC::EUnknown;
        }
    }
// ---------------------------------------------------------
// CCASingleListContainer::GetOnlineStatus
// ---------------------------------------------------------
//
TEnumsPC::TOnlineStatus CCASingleListContainer::GetOnlineStatus() const
    {
    return iMainViewArrayPC->GetOnlineStatus( CurrentListboxIndex() );
    }

// ---------------------------------------------------------
// CCASingleListContainer::Synchronised
// ---------------------------------------------------------
//
TEnumsPC::TSyncStatus CCASingleListContainer::Synchronised() const
    {
    return iMainViewArrayPC->Synchronised( CurrentListboxIndex() );
    }
// ---------------------------------------------------------
// CCASingleListContainer::IsCollapsed
// ---------------------------------------------------------
//
TBool CCASingleListContainer::IsCollapsed() const
    {
    return iMainViewArrayPC->IsCollapsed( CurrentListboxIndex() );
    }
// ---------------------------------------------------------
// CCASingleListContainer::IsWatched
// ---------------------------------------------------------
//
TBool CCASingleListContainer::IsWatched() const
    {
    return iMainViewArrayPC->IsWatched( CurrentListboxIndex() );
    }
// ---------------------------------------------------------
// CCASingleListContainer::Count
// ---------------------------------------------------------
//
TInt CCASingleListContainer::Count() const
    {
    return iMainViewArrayPC->Count();
    }

// ---------------------------------------------------------
// CCASingleListContainer::PrepareDestruction
// ---------------------------------------------------------
//
void CCASingleListContainer::PrepareDestruction()
    {
    delete iFindbox;
    iFindbox = NULL; // so we don't accidently try to delete deleted findbox.
    }

// ---------------------------------------------------------
// CCASingleListContainer::SetCbaLockL()
// Sets the lock status of cba keys.
// ---------------------------------------------------------
//
void CCASingleListContainer::SetCbaLockL( TBool aLock )
    {
    iCbaLock = aLock;
    if ( !aLock )
        {
        UpdateCbaL();
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCASingleListContainer::SizeChanged()
    {
    SetLayout(); //layouting controls

    if ( iFindbox && iFindPaneIsVisible )
        {
        iFindbox->MakeVisible( ETrue );
        }
    //fix for the TSW bug id : ECJA-7M68YH
    FocusChanged( EDrawNow );
    }

// ---------------------------------------------------------
// CCASingleListContainer::Draw()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCASingleListContainer::Draw( const TRect& /*aRect*/ ) const
    {
    /*if( ListboxItems() )
        {
        if( iFindbox )
            {
            iFindbox->MakeVisible( ETrue );
            }
        }*/
    // XXX we always have at least one item
    }

// ---------------------------------------------------------
// CCASingleListContainer::CountComponentControls() const
// From CoeControl, Returns the number of control contained
// by this class. (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASingleListContainer::CountComponentControls() const
    {
    // return number of controls inside this container
    if ( !iFindbox )
        {
        // Find-pane does not exist when there is no data in listbox,
        // so only one control exists (listbox)
        return 1; //Ignore CodeScanner warning
        }
    else
        {
        // listbox and findbox exists. that makes it two
        return 2; //Ignore CodeScanner warning
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::ComponentControl(TInt aIndex) const
// From CCoeControl, Returns handle to control pointed by aIndex
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCoeControl* CCASingleListContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
            // the caller wants to have the first control
            // so it's always listbox
        case 0:
            {
            return iListBox;
            }
        // the caller wants to have the second control
        // so it's always the findbox if it exists
        case 1:
            {
            if ( iFindbox )
                {
                return iFindbox;
                }
            return NULL;
            }
        default:
            {
            return NULL;
            }
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::SelectedUserId
// ---------------------------------------------------------
//
TPtrC CCASingleListContainer::SelectedUserId() const
    {
    TInt curIndex = CurrentListboxIndex();
    return iMainViewArrayPC->GetSelectedContactUserId( curIndex );
    }
// ---------------------------------------------------------
// CCASingleListContainer::DoRefreshViewL
// ---------------------------------------------------------
//
void CCASingleListContainer::DoRefreshViewL(
    TBool /* aSortList */ ) const
    {
    CHAT_DP_FUNC_ENTER( "DoRefreshViewL" );
    iListBox->DrawNow();
    CHAT_DP_FUNC_DONE( "DoRefreshViewL" );
    }

// ---------------------------------------------------------
// CCASingleListContainer::RefreshView
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCASingleListContainer::RefreshView( TBool aSortList ) const
    {
    TRAPD( err, DoRefreshViewL( aSortList ) );
    HandleError( err );
    }

// ---------------------------------------------------------
// CCASingleListContainer::CurrentListboxIndex() const
// Get's current index of listbox
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASingleListContainer::CurrentListboxIndex() const
    {
    if ( !iListBox )
        {
        return KErrNotFound;
        }
    if ( !iListBox->View() )
        {
        // CurrentItemIndex panics if the listbox has no view
        return KErrNotFound;
        }

    TInt curIndex( iListBox->CurrentItemIndex() );

    if ( curIndex == iListBox->TopItemIndex() )
        {
        curIndex = 0;
        }
    if ( curIndex < 0 )
        {
        return KErrNotFound;
        }
    CCAContactListBoxModel* model =
        static_cast<CCAContactListBoxModel*>( iListBox->Model() );

    if ( model )
        {
        return model->FilteredItemIndex( curIndex );
        }
    return KErrNotFound;
    }
// -----------------------------------------------------------------------------
// CCASingleListContainer::DeleteFriendL
// Creates a new contact list and shows the appropriate dialogs
// -----------------------------------------------------------------------------
//
void CCASingleListContainer::DeleteFriendL( )
    {

    TInt index( CurrentListboxIndex() );
    if ( index == KErrNotFound )
        {
        // listbox is empty
        return;
        }

    // going to delete the friend, set the contact moving state to true
    iIsMovingFlag = ETrue;
    TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( index );

    TPtrC name( KNullDesC );

#ifdef IMPS_BLOCK_AFTER_DELETE
    HBufC* deletePrompt = NULL;
    HBufC* contactId = NULL;
    TBool deletingContact = EFalse;
#endif // IMPS_BLOCK_AFTER_DELETE

    switch ( itemtype )
        {
        case TEnumsPC::EContactListItem:
            {
            name.Set( iMainViewArrayPC->DisplayName( index ) );
            break;
            }
        case TEnumsPC::EContactItem:
            {
            TPtrC itemname = iMainViewArrayPC->GetSelectedContactIdentification( index );
            name.Set( itemname );
#ifdef IMPS_BLOCK_AFTER_DELETE
            // must make copies because the contact will be destroyed upon
            // successful deletion
            deletePrompt = StringLoader::LoadLC( R_QTN_CHAT_BLOCKING_QUERY, itemname );
            contactId = iMainViewArrayPC->GetSelectedContactUserId( index ).AllocLC();
#endif // IMPS_BLOCK_AFTER_DELETE
            break;
            }
        default:
            {
            // nothing to do here because own data item cannot be deleted
            iIsMovingFlag = EFalse;
            return;
            }
        }

    // load, format and show confirmation note
    HBufC* prompt = NULL;

    if ( itemtype == TEnumsPC::EContactListItem &&
         iMainViewArrayPC->CountOfContactsInList( index ) > 0 )
        {
        prompt = StringLoader::LoadLC( R_QTN_CHAT_CONTACTS_DELETE_LIST );
        }
    else
        {
        prompt = StringLoader::LoadLC( RSC_CHAT_DELETE_CONFIRMATION, name );
        }

    TInt ret( IMDialogUtils::DisplayQueryDialogL( R_GENERIC_YES_NO_CONFIRMATION_QUERY, *prompt ) );
    CleanupStack::PopAndDestroy( prompt );
    TInt err( KErrNone );

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        // accepted, lets delete the contact
        iAppUi->ShowWaitDialogLC( R_QTN_CHAT_DELETING_CONTACT );

        if ( itemtype == TEnumsPC::EContactListItem )
            {
            err = iMainViewArrayPC->DeleteContactListL( index );
            UpdateViewAfterDeleteL();
            }
        else
            {
            TRAP( err, iMainViewArrayPC->DeleteServerContactL( index ) );


#ifdef IMPS_BLOCK_AFTER_DELETE
            // item will cease to exist upon successful deletion
            deletingContact = ETrue;
#endif // IMPS_BLOCK_AFTER_DELETE
            }

        //CodeScanner warning to be ignored
        CleanupStack::PopAndDestroy(); // waitnote
        if ( err != KErrNone )
            {
            if ( itemtype == TEnumsPC::EContactListItem )
                {
                IMDialogUtils::DisplayErrorConfirmationNoteL(
                    R_QTN_CHAT_CONTACT_LIST_DELETING_FAILED );
                }
            else
                {
#ifdef IMPS_BLOCK_AFTER_DELETE
                //CodeScanner warning to be ignored
                CleanupStack::PopAndDestroy( 2, deletePrompt ); // contactId, deletePrompt
#endif // IMPS_BLOCK_AFTER_DELETE
                IMNoteMapper::ShowNoteL( err );
                }
            }
#ifdef IMPS_BLOCK_AFTER_DELETE
        else if ( deletingContact )
            {
            // no error and it's a contact
            // Ask blocking query depending upon variation Flag.
            // UI CR  ID :101-39727: Service provider issues
            TBool blockVariation( IMUtils::IntResourceValueL(
                                      RSC_CHAT_VARIATION_BLOCK_QUERY ) );
            if ( blockVariation )
                {
                TInt block = IMDialogUtils::DisplayYesNoConfirmationDialogL(
                                 *deletePrompt );

                if ( block )
                    {
                    // block after delete requested
                    iBlockingUI->BlockUserL( *contactId );
                    }
                }
            //Co deScanner warning to be ignored
            CleanupStack::PopAndDestroy( 2, deletePrompt ); // contactId, deletePrompt
            }
#endif // IMPS_BLOCK_AFTER_DELETE
        UpdateCbaL();
        }
    
    // everything is done, set the moving state back
    iIsMovingFlag = EFalse;
    }


// ---------------------------------------------------------
// CCASingleListContainer::EditFriendL()
// (other items were commented in a header).
// Note: CodeScanner is likely to return false positives for
// this situation, because some returned TInt values will not
// be error codes.
// ---------------------------------------------------------
//
TInt CCASingleListContainer::EditFriendL( TEnumsPC::TContactEditorMode aCreateNew )
    {

    CHAT_DP( D_CHAT_LIT( "CCAFriendListViewContainer::EditFriend begin" ) );

    if ( iFindbox )
        {
        iFindbox->SetFocus( EFalse );
        }

    // index initialised to -1. This will not initialise the ContactEditor's PC.
    // This is used to indicate that it is an addition of new contact
    TInt listIndex( CurrentListboxIndex() );
    TInt editorEditableDataIndex = -1;

    // aCreateNew = EFalse if edit operation / ETrue if add operation
    if ( !( aCreateNew == TEnumsPC::EAddToContact || aCreateNew == TEnumsPC::ECreateNew ) )
        {
        //since edit will take place we need current index
        editorEditableDataIndex = listIndex;
        }

    TPtrC itemname = iMainViewArrayPC->GetSelectedContactUserId( listIndex );

    TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( listIndex );

    TPtrC listId( KNullDesC() );
    if ( itemtype != TEnumsPC::EOwnStatusItem )
        {
        // don't need to check for contact lists for own status
        if ( ( itemtype == TEnumsPC::EContactItem ) || ( itemtype == TEnumsPC::EContactListItem ) )
            {
            // we have a list
            listId.Set( iMainViewArrayPC->GetSelectedListId( listIndex ) ) ;
            }
        // make sure we have contact list
        else
            {
            CCAUINGUtils::TChatContactListCreationType listCreation =
                CCAUINGUtils::VerifyContactlistL();

            if ( listCreation == CCAUINGUtils::EChatContactListWasCreated )
                {
                iListBox->HandleItemAdditionL();
                UpdateFilterL();
                UpdateCbaL();
                }
            else
                {
                if ( listCreation == CCAUINGUtils::EChatNoContactList )
                    {
                    // Could not create contact list
                    return NULL;
                    }
                }
            }
        }

    TPtrC itemName;
    if ( ( itemtype == TEnumsPC::EContactItem ||
           itemtype == TEnumsPC::EOwnStatusItem ) &&
         aCreateNew != TEnumsPC::ECreateNew )
        {
        itemName.Set( itemname );
        }

    CHAT_DP( D_CHAT_LIT(
                 "CCAFriendListViewContainer::EditFriend before editcontact" ) );

    HBufC* userName = HBufC::NewMaxL( KServerWVUserIdMaxLength );
    userName->Des().SetLength( 0 );
    CleanupStack::PushL( userName );
    TPtr userNamePtr = userName->Des();

    // Show domain selection query, if variated so
    TBool cursorToBeginning = EFalse;
    if ( aCreateNew == TEnumsPC::ECreateNew
         && IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_DOMAIN_SELECTION ) )
        {
        // Fill the user name with domain, if user selected it
        TInt retVal = iAppUi->UISessionManager().DisplayDomainSelectionQueryL(
                          userNamePtr );
        if ( retVal == 0 )
            {
            // User cancelled the procedure
            CleanupStack::PopAndDestroy( userName );
            return NULL;
            }
        else
            {
            cursorToBeginning = ETrue;
            }
        }

    TBool contactSaved( EFalse );
    // the contact editor should not return to details view at all,
    // even if Editing existing one

    CCAContactEditor* editor = CCAContactEditor::NewL(
                                   editorEditableDataIndex, contactSaved );


    // If user selected a domain, pass the prefilled user name
    iCbaLock = ETrue;
    TInt err = KErrNone;
    if ( userName->Length() > 0 )
        {

        TRAP( err, editor->EditContactLD( aCreateNew,
                                          listId,
                                          userNamePtr,
                                          KNullDesC,
                                          EFalse,
                                          EFalse,
                                          cursorToBeginning ) );
        }
    else
        {

        TRAP( err, editor->EditContactLD( aCreateNew, listId, itemname ) );
        }
    iCbaLock = EFalse;
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy( userName ); // userName

    if ( iFindbox )
        {
        /*
         * The FindBox is cleared, so the filter also
         * needs to be cleared.
         */
        TRAPD( error, static_cast<CAknFilteredTextListBoxModel*>( ListBox()->Model() )
               ->Filter()->ResetFilteringL(); );

        ResetFindBoxL();
        if ( !iAppUi->IsDisplayingDialog())
        	{
                iFindbox->SetFocus( ETrue );
        	}
        UpdateFilterL();
        }

    if ( !contactSaved )
        {
        return listIndex;
        }
    if ( aCreateNew == TEnumsPC::EAddToContact )
        {
        // return the old index of item
        // need to focus the item from where adding started
        return listIndex;
        }
    return CurrentListboxIndex();

    }

// ---------------------------------------------------------
// CCASingleListContainer::MoveContactL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::MoveContactL()
    {
    if ( ListboxItems() )
        {

        iIsMovingFlag = ETrue;
        TInt index = CurrentListboxIndex();

        // current item type..which should be moved
        TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( index );

        if ( itemtype == TEnumsPC::EContactItem || itemtype == TEnumsPC::EOwnStatusItem )
            {
            TInt focusIndex = index + 1;// focus should goto next item....
            if ( focusIndex >= Count() )
                {
                // we reached end of array and don't have any more items..
                // so move the focus to before item...
                focusIndex = index - 1;
                }


            //NEXT OR PREVIOUS ITEM DETAILS
            // get the type of item here.....
            TEnumsPC::TItem focusIndexType = iMainViewArrayPC->GetType( focusIndex );

            // if move is success then get the next or previous item details
            HBufC* focusWVid = NULL;		// user id of next or previous item,where focus should be,
            HBufC* focusListid = NULL;

            if ( focusIndexType == TEnumsPC::EContactItem )
                {
                // get the list id and contact index...
                focusWVid = iMainViewArrayPC->GetSelectedContactUserId( focusIndex ).AllocLC();
                focusListid = iMainViewArrayPC->GetSelectedListId( focusIndex ).AllocLC();
                }
            else
                {
                // for this case both are holding the same string...
                focusWVid = iMainViewArrayPC->GetSelectedListId( focusIndex ).AllocLC();
                focusListid = iMainViewArrayPC->GetSelectedListId( focusIndex ).AllocLC();
                }

            // we got all the details....so let's invoke dialog to move contact....

            //END OF NEXT OR PREVIOUS DETAILS
            TBool contactSaved( EFalse );
            HBufC* wvid 	= iMainViewArrayPC->GetSelectedContactUserId( index ).AllocLC();
            TPtrC listid 	= iMainViewArrayPC->GetSelectedListId( index );
            TInt findIndex 	= -1;

            TBool moveFocusToNextItem = EFalse; // whether the contact should be moved to next item or not

            CCAContactEditor* editor =
                CCAContactEditor::NewLC( index, contactSaved );

            HBufC* movedListId;
            TBool collapsed;

            SetSkipStorageEvents( ETrue );

            // Fix warning: C2874W: extMoved may be used before being set
            // Set extMoved a default value
            TBool extMoved = EFalse;
            TRAPD( err, extMoved = editor->MoveContactL( listid, *&movedListId, collapsed ) );
            if ( err != KErrNone )
                {
                SetSkipStorageEvents( EFalse );
                CleanupStack::PopAndDestroy( 4, focusWVid );
                if ( movedListId )
                    {
                    delete  movedListId;
                    }
                iIsMovingFlag = EFalse;
                User::Leave( err );
                }

            if ( !extMoved ) // if move failed...then focus need not be changed..
                {
                // move failed.b'coz of multiple reasons...
                // 1) user canceled the move operation.
                // 2) contact was deleted from old list.
                //    as it's existing in the list, where it was moved.
                findIndex = iMainViewArrayPC->FindContactL( listid, *wvid );

                // if this index is valid, whom the user treid to moved, then user cancelled operation
                if ( findIndex != KErrNotFound )
                    {
                    // we found the contact..so set the focused items correctly here...
                    focusIndex = findIndex;// focus should goto same item
                    // get the type of item here.....
                    focusIndexType = iMainViewArrayPC->GetType( focusIndex );
                    }
                else
                    {
                    // Reset array has to called explictly( reset arry
                    // on deletion of a contact is not done on pc side
                    // due to SetSkipStorageEvents)

                    iMainViewArrayPC->ResetArray( EFalse );
                    UpdateViewAfterDeleteL();
                    moveFocusToNextItem = ETrue;
                    }
                }
            else
                {
                // move was success......
                moveFocusToNextItem = ETrue;
                }

            iIsMovingFlag = EFalse;

            SetSkipStorageEvents( EFalse );

            // now get the index of item,where focus should be if move was success
            if ( moveFocusToNextItem )
                {
                // move was success...focus should be on next item..
                // OR contact has been removed from old list.....
                // now get the index of next item and set the focus to that..
                if ( movedListId )
                    {
                    if ( collapsed )
                        {
                        iMainViewArrayPC->SetCollapsed( collapsed, iMainViewArrayPC->FindContactListIndexL( movedListId->Des() ) );
                        }
                    }
                if ( focusIndexType == TEnumsPC::EContactItem )
                    {
                    focusIndex = iMainViewArrayPC->FindContactL( *focusListid, *focusWVid );
                    focusIndexType = iMainViewArrayPC->GetType( focusIndex );
                    }
                else
                    {
                    focusIndex = iMainViewArrayPC->FindContactListIndexL( *focusListid );
                    focusIndexType = iMainViewArrayPC->GetType( focusIndex );
                    }
                }



            if ( movedListId )
                {
                delete movedListId;
                }

            UpdateViewL( focusIndex, focusIndexType, ETrue );
            UpdateCbaL();
            // update the view accordingly....
            CleanupStack::PopAndDestroy( 4, focusWVid ); // editor, wvid,focusListid,focuswvid
            }

        else
            {
            // if item is anything else, don't do anything just return from this function.....
            return;
            }
        }
    }
// ---------------------------------------------------------
// CCASingleListContainer::UpdateViewL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::UpdateViewL(
    TInt aIndex, TEnumsPC::TItem aType, TBool aSyncSuccess )
    {
    if ( !aSyncSuccess )
        {
        //do nothing if not synchronized
        return;
        }

    CCAContactListBoxModel* list =
        ( ( CCAContactListBoxModel* ) iListBox->Model() );
    TInt count = list->NumberOfItems();
    TInt index;
    switch ( aType )
        {
        case TEnumsPC::EContactItem:
        case TEnumsPC::EOwnStatusItem:
            {
            /*Not attempting to get the index of contact because aIndex is the
            same as this index and we pass it on to the contactlistboxmodel's
            visibleitem method*/
            index = list->VisibleItemIndex( aIndex );
            if ( index != KErrNotFound )
                {
                CHAT_DP( D_CHAT_LIT(
                             "CCASingleListContainer::UpdateViewL, setting index %d" ),
                         index );
                iListBox->ScrollToMakeItemVisible( index - 1 );
                iListBox->SetCurrentItemIndex( index );
                iListBox->DrawNow();
                }
            else if ( iBlocked && iIndexPrevBlocking < count )
                {
                iListBox->SetCurrentItemIndexAndDraw( iIndexPrevBlocking );
                iBlocked = EFalse;
                }
            else
                {
                // Contact not found
                TInt maxindex = list->NumberOfItems() - 1;
                if ( maxindex >= 0 )
                    {
                    TInt curIndex = CurrentListboxIndex() - 1;
                    if ( !Rng( 0, curIndex, maxindex ) )
                        {
                        // cur index out of range -> set to last item
                        curIndex = maxindex;
                        }
                    iListBox->SetCurrentItemIndex( curIndex );
                    }
                iListBox->DrawNow();
                }
            break;
            }
        case TEnumsPC::EContactListItem:
        case TEnumsPC::EOpenChatsListItem:
            {
            /*We get the index of list because and we pass it on to the
            contactlistboxmodel's visibleitem method*/

            index = list->VisibleItemIndex( iMainViewArrayPC->IndexOfList( aIndex ) );
            if ( index != KErrNotFound )
                {
                CHAT_DP( D_CHAT_LIT(
                             "CCASingleListContainer::UpdateViewL, setting index %d" ),
                         index );
                iListBox->ScrollToMakeItemVisible( index - 1 );
                iListBox->SetCurrentItemIndex( index );
                iListBox->DrawNow();
                }
            break;
            }
        case TEnumsPC::EConversationItem:
            {
            /* we have to focused the the existing item after
             add to IM contact and sroll the list box to be visible */
            index = list->VisibleItemIndex( aIndex );
            if ( index != KErrNotFound )
                {
                CHAT_DP( D_CHAT_LIT(
                             "CCASingleListContainer::UpdateViewL, setting index %d" ),
                         index );

                iListBox->ScrollToMakeItemVisible( index - 1 );

                iListBox->SetCurrentItemIndexAndDraw( index );
                }

            break;
            }
        case TEnumsPC::EInviteItem:
            {
            /* we have to focused the the existing item.
               And sroll the list box to be visible */
            index = list->VisibleItemIndex( aIndex );
            if ( index != KErrNotFound )
                {
                CHAT_DP( D_CHAT_LIT(
                             "CCASingleListContainer::UpdateViewL, setting index %d" ),
                         index );

                iListBox->ScrollToMakeItemVisible( index - 1 );

                iListBox->SetCurrentItemIndexAndDraw( index );
                }

            break;
            }
        case TEnumsPC::EGroupItem:
            {
            /* we have to focused the the existing item.
               And sroll the list box to be visible */
            index = list->VisibleItemIndex( aIndex );
            if ( index != KErrNotFound )
                {
                CHAT_DP( D_CHAT_LIT(
                             "CCASingleListContainer::UpdateViewL, setting index %d" ),
                         index );

                iListBox->ScrollToMakeItemVisible( index - 1 );

                iListBox->SetCurrentItemIndexAndDraw( index );
                }

            break;
            }
        default:
            {
            //nothing to do
            break;
            }
        }



    CHAT_DP( D_CHAT_LIT( "CCAFriendListViewContainer::UpdateViewL end" ) );
    }

// ---------------------------------------------------------
// CCASingleListContainer::RefreshPresenceL()
// ---------------------------------------------------------
//
void CCASingleListContainer::RefreshPresenceL( TBool aManualLaunch )
    {
    TBool autoUpdate( iAppUi->GetProcessManager().GetSettingsInterface()->GetBoolValuePC(
                          TEnumsPC::EAutomaticPresenceUpdate , NULL ) );

    if ( autoUpdate )
        {
        // in automatic update mode, don't do refresh (unless variated)
        if ( !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_FETCH_PRESENCE ) )
            {
            // variant doesn't allow refreshing in automatic mode
            return;
            }
        }

    // show dialog immediately
    iAppUi->ShowWaitDialogL( R_QTN_CHAT_WAIT_REFRESH, ETrue );

    TRAPD( err, iMainViewArrayPC->RefreshFriendsL() );

    CHAT_DP( D_CHAT_LIT( "**CAPresenceManager::RefreshFriendsL leave code %d" ),
             err );

    if ( err != KErrNone )
        {
        User::Leave( err );
        }
    // Server sent us some non-OK error. Maybe we had some bogus ID in the list.
    // We shouldn't show any dialog here, because information about the
    // non-successful event is shown by the confirmation query below.
    iAppUi->DismissWaitDialogL( err );

    RefreshView( ETrue );

    CCACommandManager* commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager();
    MCASessionHandlerCmd* sessHandlerCmd =  commandManager->GetSessionHandlerCmd() ;
    if ( sessHandlerCmd )
        {
        sessHandlerCmd->SetContactsRefreshed( ETrue );
        }

    // If something failed, then show confirmation query
    if ( err != KErrNone )
        {
        if ( IMDialogUtils::DisplayQueryDialogL(
                 R_FRIENDS_CANNOT_UPDATE_QUERY ) == EAknSoftkeyDetails )
            {
            // If user choosed details, then show explanation of
            // error
            IMDialogUtils::DisplayQueryDialogL(
                R_FRIENDS_CANNOT_UPDATE_REASON_QUERY );
            }
        }

    if ( !aManualLaunch && !autoUpdate )
        {
        // manual presence update and user didn't start this, show info note
        HBufC* text = iEikonEnv->AllocReadResourceLC(
                          R_QTN_CHAT_MANUAL_UPDATE_ACT );
        IMDialogUtils::DisplayNoteDialogL( *text, R_AVKON_SOFTKEYS_OK_EMPTY__OK,
                                           CAknNoteDialog::ENoTimeout );
        CleanupStack::PopAndDestroy( text );
        }

    iAppUi->ReleaseCapturingL();
    }


// ---------------------------------------------------------
// CCASingleListContainer::ShowDetailsL()
// Add friend from phonebook
// ---------------------------------------------------------
//
void CCASingleListContainer::ShowDetailsL()
    {
    if ( ListboxItems() )
        {
        TInt index  = CurrentListboxIndex();
        TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( index );

        if ( itemType == TEnumsPC::EContactItem || itemType == TEnumsPC::EOwnStatusItem )
            {
            iStatusPane->ClearNaviPaneL();
            TBool saved( EFalse );

            TPtrC list( KNullDesC() );

            // set list id if it exists
            if ( itemType == TEnumsPC::EContactItem )
                {
                list.Set( iMainViewArrayPC->GetSelectedListId( index ) );
                }


            CCAContactDetailsDialog* dialog =
                CCAContactDetailsDialog::NewL( saved, index, list );

            if ( iFindbox )
                {
                iFindbox->SetFocus( EFalse );
                }

            // Prevent view from changing softkeys while dialog is active
            iCbaLock = ETrue;
            dialog->ShowLD();
            iCbaLock = EFalse;

            if ( iFindbox )
                {
                iFindbox->SetFocus( ETrue );
                }

            iStatusPane->ShowTabGroupL( KUidFriendsListView );

            UpdateCbaL();
            }
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::ListboxItems() const
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASingleListContainer::ListboxItems() const
    {
    return ( iListBox->Model()->NumberOfItems() > 0 );
    }

// ---------------------------------------------------------
// CCASingleListContainer::DisplayBlockedListL()
// Displays list of blocked persons
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayBlockedListL()
    {
    //SetSkipStorageEvents( ETrue );

    iBlockingUI->DisplayBlockedListL();

    //SetSkipStorageEvents( EFalse );

    iMainViewArrayPC->ResetArray( EFalse );
    TInt index  = CurrentListboxIndex();
    iListBox->HandleItemAdditionL();
    TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( index );
    UpdateViewL( index, itemType, ETrue );
    UpdateFilterL();
    }

// ---------------------------------------------------------
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayUnblockListQueryL()
    {
    iBlockingUI->DisplayUnblockListQueryL();
    }

// ---------------------------------------------------------
// CCASingleListContainer::BlockUserL()
// Blocks focused user
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::BlockUserL()
    {
    iIsMovingFlag = ETrue;
    TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( CurrentListboxIndex() );
    if ( itemType == TEnumsPC::EInvalid )
        {
        User::Leave( KErrNotFound );
        }

    switch ( itemType )
        {

        case TEnumsPC::EContactItem:
            {

            TInt indexOfList = 0;
            TInt countOfContactsInList = 0;

            iIndexPrevBlocking = CurrentListboxIndex(); //iListBox->CurrentItemIndex();

            if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) == 1 )
                {
                indexOfList = iMainViewArrayPC->IndexOfList( iIndexPrevBlocking );
                countOfContactsInList = iMainViewArrayPC->CountOfContactsInList( indexOfList );
                }

            iBlocked = ETrue;
            iBlockingUI->BlockUserL( iMainViewArrayPC->
                                     GetSelectedContactUserId( iIndexPrevBlocking ) );

            if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) == 1 )
                {
                if ( indexOfList + countOfContactsInList  == iIndexPrevBlocking  )
                    {
                    iListBox->SetCurrentItemIndex( iIndexPrevBlocking - 1 );
                    }
                iMainViewArrayPC->ResetArray( EFalse );
                }

            break;
            }

        case TEnumsPC::EConversationItem:
        case TEnumsPC::EInviteItem:
            {
            TInt listIndex = iListBox->CurrentItemIndex();

            iBlockingUI->BlockUserL( iMainViewArrayPC->
                                     GetSelectedContactUserId( listIndex ) );

            break;
            }

        default:
            {
            break;
            }
        }
    UpdateCbaL();
    iIsMovingFlag = EFalse;
    }


// ---------------------------------------------------------
// CCASingleListContainer::IsBlocked()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASingleListContainer::IsBlocked() const
    {
    TInt index = CurrentListboxIndex();
    TBool isBlocked = iMainViewArrayPC->IsBlocked( index );
    return isBlocked;
    }

// ---------------------------------------------------------
// CCASingleListContainer::FindJoinedChatsL()
// Finds joined chats and displays dialog.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::FindJoinedChatsL()
    {
    TInt index = CurrentListboxIndex();
    TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( index );
    if ( itemType != TEnumsPC::EContactItem && itemType == TEnumsPC::EOwnStatusItem )//!contact )
        {
        User::Leave( KErrNotFound );
        }

    HBufC* wvUIDBuf = iMainViewArrayPC->GetSelectedContactUserId( index ).AllocLC();
    HBufC* identification = iMainViewArrayPC->GetSelectedContactIdentification( index ).AllocLC();
    HBufC* prompt = StringLoader::LoadLC( R_CHAT_FIND_JOINED_PRTX, *identification );

    //granularity is one
    CSearchPairs* pairs = new ( ELeave ) CSearchPairs( 1 );
    CleanupStack::PushL( pairs );

    CImpsSearchRequest* request = CImpsSearchRequest::NewL();
    CleanupStack::PushL( request );

    request->SetRequestL( EImpsGroupUserIDJoined, *wvUIDBuf );
    pairs->AppendL( request );

    TInt err( KErrNone );
    iAppUi->ShowWaitDialogL( R_QTN_CHAT_WNOTE_SEARCHING );
    TRAPD( leave, err = iSearchInterfacePC->StartSearchL(
                            KSearchLimit, NULL, pairs ) );
    iAppUi->DismissWaitDialogL( leave );

    if ( err == KErrNone )
        {
        // displayid'd version of group id
        CDesCArrayFlat* itemList = new ( ELeave ) CDesCArrayFlat(
            KArrayGranularity );
        CleanupStack::PushL( itemList );

        // contains the full group id with wv: and domain
        CDesCArrayFlat* itemListFullId = new ( ELeave ) CDesCArrayFlat(
            KArrayGranularity );
        CleanupStack::PushL( itemListFullId );

        TInt resultCount( iDataInterfacePC->SearchDataCount() );

        // If we have found some joined chats -> show list
        if ( resultCount > 0 )
            {
            for ( TInt count( 0 ); count < resultCount; ++count )
                {
                // identical arrays, the index of itemList is used for itemListFullId
                itemList->AppendL( iAppUi->DisplayId(
                                       iDataInterfacePC->SearchData( count ) ) );
                itemListFullId->AppendL( iDataInterfacePC->SearchData( count ) );
                }

            TInt indexOfChat( 0 );
            TInt ret( IMDialogUtils::DisplayListQueryDialogL( &indexOfChat, itemList,
                                                              R_FIND_JOINED_CHATS_RESULTS_DLG, *prompt ) );

            if ( ( ret == EChatClientJoin || ret == EAknSoftkeyOk ) && ( indexOfChat >
                                                                         KErrNotFound ) )
                {
                iAppUi->GroupUtils()->JoinGroupL( EFalse, itemListFullId->MdcaPoint(
                                                      indexOfChat ) );
                }
            }
        // no joined chats -> show only info note
        else
            {
            IMDialogUtils::DisplayInformationNoteL(
                R_QTN_CHAT_CONTACTS_NO_JOINED_FOUND );
            }

        CleanupStack::PopAndDestroy( itemListFullId );
        CleanupStack::PopAndDestroy( itemList );
        }
    else
        {
        IMNoteMapper::ShowNoteL( err );
        }

    //CodeScanner warning to be ignored
    CleanupStack::PopAndDestroy( 5, wvUIDBuf ); // request, pairs, prompt, identification, wvUIDBuf

    }

// ---------------------------------------------------------
// CCASingleListContainer::DisplayConfirmationNoteL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayConfirmationNoteL(
    const TInt aTextResourceId )
    {
    HBufC* text = iCoeEnv->AllocReadResourceLC( aTextResourceId );
    CAknConfirmationNote* dlg = new( ELeave )CAknConfirmationNote();
    dlg->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------
// CCASingleListContainer::UpdatePresenceL()
// (other items were commented in a header).
// Note: CodeScanner is likely to return false positives for
// this situation
// ---------------------------------------------------------
//
TInt CCASingleListContainer::UpdatePresenceL( TInt aIndex )
    {
    TInt err( KErrNone );
    TBool autoUpdate( iAppUi->GetProcessManager().GetSettingsInterface()->GetBoolValuePC(
                          TEnumsPC::EAutomaticPresenceUpdate , NULL ) );

    CHAT_DP_TXT( "CCASingleListContainer::UpdatePresenceL" );
    if ( !autoUpdate )
        {

        TPtrC userId = iMainViewArrayPC->GetSelectedContactUserId( aIndex );
        // no automatic update
        // => fetch the presence information
        CHAT_DP_TXT( "no auto update, fetch presence" );

        if ( userId.Length() > 0 )
            {
            CHAT_DP( D_CHAT_LIT( "fetching presence for %S" ), &userId );
            // array has size of one because only one user will be added
            CDesCArray* users = new( ELeave )CDesCArrayFlat( 1 );
            CleanupStack::PushL( users );
            users->AppendL( userId );

            HBufC* text = StringLoader::LoadLC( R_QTN_CHAT_CHECKING_AVAILABILITY,
                                                iMainViewArrayPC->GetSelectedContactIdentification( aIndex ) );

            iAppUi->ShowWaitDialogL( *text );
            TRAP( err, iMainViewArrayPC->GetOnlineUsersL( users, NULL, NULL, ETrue ) );
            CHAT_DP( D_CHAT_LIT( "GetOnlineUsersL leave code %d" ), err );
            if ( err == KErrInUse )
                {
                // user may open convesation while objects are refreshings
                // in background so ignore this error
                err = KErrNone;
                }
            iAppUi->DismissWaitDialogL( err, EFalse, userId );

            //CodeScanner warning to be ignored
            CleanupStack::PopAndDestroy( 2, users ); // text, users
            }
        }
    return err;
    }

// ---------------------------------------------------------
// CCASingleListContainer::OpenConversationL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::OpenConversationL()
    {

    if ( !iIsMovingFlag )
        {
        TInt index = CurrentListboxIndex();

        const TDesC& wvId( iMainViewArrayPC->GetSelectedContactUserId( index ) );
        const TDesC& identification( iMainViewArrayPC->GetSelectedContactIdentification( index ) );
        const TDesC& listID( iMainViewArrayPC->GetSelectedListId( index ) );
        //SetReadInterfaceL() function is called in OpenConversations.
        //This causes HandleChatEvent() which increments the iContactListIndex.
        //Better we store the user id at this point and pass it on
        //to the conversation view so that iContactListIndex change doesn't affect
        //the logic of getting the user id
        TCADnlConvView dnlConView;

        //collecting only first KMaxWVIDLength character in iWVID.
        dnlConView.iWVID = ( CAUtils::DisplayId( wvId ) ).Left( KMaxWVIDLength );
        if ( listID != KNullDesC )
            {
            dnlConView.iListID = listID;
            }
        else
            {
            dnlConView.iListID = KNullDesC;
            }
        dnlConView.iIsForwarded = EFalse;
        dnlConView.iSAPChanged = EFalse;
        dnlConView.iSwitchTab = EFalse;

        if ( !iMainViewArrayPC->ChatExistsL( index ) )
            {
            CHAT_DP_TXT( " not found, fetching presence.." );
            if ( !iAppUi->UISessionManager().IsLoggedIn() )
                {
                iAppUi->UISessionManager().LoginL( MCAUiLoginCmdCB::ELogin );
                if ( !iAppUi->UISessionManager().IsLoggedIn() )
                    {
                    return;
                    }
                }

            if ( UpdatePresenceL( index ) != KErrNone )
                {
                // invalid wv id or server error
                return;
                }

            if ( iMainViewArrayPC->IgnoreOfflineStatusL( index ) != TEnumsPC::EOnline )
                {
                TPtrC contactname = iMainViewArrayPC->GetItemNameText( index );

                iOpeningConversation = ETrue;

                // User does not want to send msg to offline user
                if ( IMUtils::IgnoreOfflineStatusL( contactname ) < 0 )
                    {
                    iOpeningConversation = EFalse;
                    return;
                    }
                }

            CHAT_DP_TXT( " trying to create chatdata.." );
            iMainViewArrayPC->SetMessageReadInterfaceL( index );
            }

        UpdateCbaL( ETrue );
        SetCbaLockL( ETrue );
        iOpeningConversation = ETrue;

        //Add the conversation item to open chats array before switching the view
        iMainViewArrayPC->InsertConversationItemL( wvId, identification );

        TCADnlConvViewBuf convMsgBuf( dnlConView );
        iViewSwitcher->SwitchViewL( KUidConversationsView, KUidConvViewMsgId, convMsgBuf );
        }
    }

// ---------------------------------------------------------
// CloseConversationL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::CloseConversationL()
    {
    TInt index = CurrentListboxIndex();

    HBufC* identification =
        iMainViewArrayPC->GetItemNameText( index ).AllocLC();

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
        MCAConversationPC* ConversationPC = iAppUi->GetProcessManager().GetConversationInterface();
        //update the index before closing bcoz some new conversation might have recieved in between
        index = CurrentListboxIndex();
        TInt indexOfList = iMainViewArrayPC->IndexOfList( index );
        TInt countOfContactsInList = iMainViewArrayPC->CountOfContactsInList( indexOfList );

        TBuf<KMaxWVIDLength> wvId;

        wvId.Copy( iMainViewArrayPC->GetSelectedContactUserId( index ).Left(
                       wvId.MaxLength() ) );
        //To remove the item from open chats array when conv is closed
        iMainViewArrayPC->RemoveConversationItem( wvId );

        ConversationPC->DeleteChatL( KNullDesC,
                                     KNullDesC,
                                     wvId );

        }
    UpdateCbaL();
    UpdateFilterL();
    }
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CEikListBox* CCASingleListContainer::ListBox() const
    {
    return iListBox;
    }

// ---------------------------------------------------------
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayBlockTextQueryL()
    {
    if ( iFindbox )
        {
        iFindbox->SetFocus( EFalse );
        }

    iBlockingUI->DisplayBlockTextQueryL();
	
    if ( iFindbox )
        {
        iFindbox->SetFocus( ETrue );
        }
    }

// ---------------------------------------------------------
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASingleListContainer::OpeningConversation()
    {
    return iOpeningConversation;
    }


// ---------------------------------------------------------
// CCASingleListContainer::ActivateFindPaneL()
// Activates find-pane
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::ActivateFindPaneL()
    {
    if ( !iFindbox )
        {
        iFindbox = CAknSearchField::NewL( *this,
                                          CAknSearchField::ESearch,
                                          NULL, KTextLimit );
        static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() )
        ->CreateFilterL( iListBox, iFindbox );

        }
    iFindbox->SetObserver( this );
    iFindbox->ActivateL();
    iFindbox->ResetL();
    iFindbox->SetSearchTextL( KNullDesC );

    // Create filtering

    // Signal listbox that findbox got activated

    static_cast<CAknColumnListBoxView*>( iListBox->View() )
    ->SetFindEmptyListState( ETrue );
    iFindPaneIsVisible = ETrue;
    SetLayout();

    if ( IsActivated() )
        {
        iFindbox->MakeVisible( ETrue );
        iFindbox->SetFocus( ETrue );
        iListBox->SizeChanged();
        DrawNow();
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::DeactivateFindPaneL()
// De-activates find-pane
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DeactivateFindPaneL()
    {
    static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() )
    ->Filter()->ResetFilteringL();

    iFindbox->MakeVisible( EFalse );
    iFindbox->SetFocus( EFalse );
    iMainViewArrayPC->ForceExpanded( EFalse );

    // Signal listbox that findbox went away
    static_cast<CAknColumnListBoxView*>( iListBox->View() )
    ->SetFindEmptyListState( EFalse );
    iFindPaneIsVisible = EFalse;
    SetLayout();

    if ( IsActivated() )
        {
        DrawNow();
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::SetLayout()
// Layouts current components according to AVKON LAF
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::SetLayout()
    {
    const TRect rect( Rect() );
    if ( iListBox && iFindPaneIsVisible )
        {
        AknLayoutUtils::LayoutControl( iListBox,
                                       rect,
                                       AKN_LAYOUT_WINDOW_list_gen_pane( 1 ) );

        AknLayoutUtils::LayoutControl( iFindbox,
                                       rect,
                                       AKN_LAYOUT_WINDOW_find_pane );

        }
    else if ( iListBox )
        {
        AknLayoutUtils::LayoutControl( iListBox,
                                       rect,
                                       AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::DisplayInfoNoteL()
// Displays info note defined in resource
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayInfoNoteL( const TInt aResourceId )
    {
    CAknNoteDialog* infoNote =
        new ( ELeave ) CAknNoteDialog( CAknNoteDialog::EConfirmationTone,
                                       CAknNoteDialog::EShortTimeout );

    infoNote->ExecuteLD( aResourceId );

    }

// ---------------------------------------------------------
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::UpdateScrollBars( CEikListBox* aListBox )
    {
    if ( aListBox )
        {
        TInt pos = CurrentListboxIndex();
        if ( aListBox->ScrollBarFrame() )
            {
            if ( pos >= 0 && pos < aListBox->Model()->NumberOfItems() )
                {
                aListBox->ScrollBarFrame()->MoveVertThumbTo( pos );
                }
            }
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::UpdateViewAfterDeleteL
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCASingleListContainer::UpdateViewAfterDeleteL()
    {
    // prevent index from being in the last slot
    // if it's going to be deleted, to prevent
    // HandleItemRemovalL from exploding
    // by default focus next items
    TInt index = iListBox->CurrentItemIndex();
    TInt items = iListBox->Model()->NumberOfItems();
    if ( index >= ( items - 1 ) || index == KErrNotFound )
        {
        CHAT_DP_TXT( "fixing index to last element" );
        iListBox->SetCurrentItemIndex( items - 1 );
        }


    iListBox->HandleItemRemovalL();

    UpdateFilterL();

    CHAT_DP( D_CHAT_LIT(
                 "CCASingleListContainer::UpdateViewAfterDeleteL, index %d" ),
             index );

    // We have to manually recalculate focused index after item removal
    TInt item = iListBox->Model()->NumberOfItems();

    if ( ( index == item ) && ( index > 0 ) )
        {
        CHAT_DP_TXT( "decreasing index" );

        /* need to check focus for  contact item
        * if it only one contact is available then focus shoud be on
        * contact list or not from where contact is deleted
        */
        iListBox->SetCurrentItemIndex( item - index );

        }

    if ( !ListFilter()->NonFilteredNumberOfItems() )
        {
        // list is really empty
        DeactivateFindPaneL();
        }

    iListBox->DrawNow();

#ifdef _DEBUG
    index = iListBox->CurrentItemIndex();
    CHAT_DP( D_CHAT_LIT(
                 "CCASingleListContainer::UpdateViewAfterDeleteL done, index %d" ),
             index );
#endif
    }


// ---------------------------------------------------------
// CCASingleListContainer::UpdateFilterL
// ---------------------------------------------------------
//
void CCASingleListContainer::UpdateFilterL() const
    {
    if ( iFindPaneIsVisible && iFindbox )
        {
        // Avkon apparently changed the logic of listboxes
        // so that the filter needs to be updated even though
        // there's nothing in it.
        // findpane is visible and contains something, notify it about
        // the changed array

        CCAContactListBoxModel* model =
            static_cast<CCAContactListBoxModel*>( iListBox->Model() );
        if ( model )
            {
            CAknListBoxFilterItems* filter =
                static_cast<CAknListBoxFilterItems*>( model->Filter() );

            TInt index = iListBox->CurrentItemIndex();

            if ( index == KErrNotFound )
                {
                // for some reason the listbox loses the current
                // index sometimes, breaking any calls to FilteredItemIndex.
                // draw it nevertheless. otherwise when erasing a character
                // from the filter causes nothing to be displayed.
                if ( iListBox )
                    {
                    iListBox->DrawNow();
                    }
                return;
                }

            // HandleItemArrayChangeL resets the listbox index,
            // work-around it

            // Handle Item addition only when there are visible items
            iListBox->HandleItemAdditionL();

            TInt top   = iListBox->TopItemIndex();

            TInt filteredIndex = model->FilteredItemIndex( index );
            TInt filteredTop = model->FilteredItemIndex( top );

            if ( filter )
                {
                // this resets the listbox index
                filter->HandleItemArrayChangeL();
                }

            TInt filteredItems = model->NumberOfItems();
            if ( filteredItems )
                {
                // we have some items to show
                if ( filteredIndex >= filteredItems )
                    {
                    // original index was beyond the last visible item,
                    // so move the index there
                    filteredIndex = filteredItems - 1;
                    }

                iListBox->SetCurrentItemIndex( filteredIndex );

                if ( filteredTop <= filteredIndex )
                    {
                    // move the filtered top item to the top
                    iListBox->SetTopItemIndex( filteredTop );
                    }
                else
                    {
                    iListBox->SetTopItemIndex( filteredIndex );
                    }
                }

            if ( iListBox )
                {
                iListBox->DrawNow();
                }
            }
        }
    }


// ---------------------------------------------------------
// CCASingleListContainer::LoadBitmapsL
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCASingleListContainer::LoadBitmapsL()
    {
    CColumnListBoxData* listBoxData = iListBox->ItemDrawer()->ColumnData();
    if ( !listBoxData )
        {
        //No listbox data
        User::Leave( KErrNotFound );
        }

    MCASkinVariant* skinVar = static_cast<CCAApp*>(
                                  iAppUi->Application() )->VariantFactory()->SkinVariantL();


    CAknIconArray* icons = CCASingleListViewArray::LoadIconsLC(
                               iAppUi->MbmFullPath(), *skinVar );

    CArrayPtr<CGulIcon>* oldIconArray = listBoxData->IconArray();
    if ( oldIconArray )
        {
        oldIconArray->ResetAndDestroy();
        delete oldIconArray;
        listBoxData->SetIconArray( NULL );
        }

    // Set icon-array to listbox's drawer
    listBoxData->SetIconArray( icons );
    CleanupStack::Pop( icons );
    }

// ---------------------------------------------------------
// CCASingleListContainer::OfferKeyEventL(
//												const TKeyEvent& aEvent,
//												TEventCode aType )
// From CCoeControl, Handles key-events
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCASingleListContainer::OfferKeyEventL(
    const TKeyEvent& aEvent,
    TEventCode aType )
    {
    CHAT_DP( D_CHAT_LIT( "CCASingleListContainer:: EventL %d %d" ),
             aEvent, aType );

    if ( iAppUi->AreEventsCaptured() )
        {
        // for some reason the input absorber goes
        // to the wrong position in view stack...
        return EKeyWasConsumed;
        }

    switch ( aEvent.iCode )
        {
        case EKeyBackspace:
            {
            CHAT_DP_TXT( "backspace event" );
            if ( iFindbox )
                {
                if ( iFindbox->TextLength() == 0 )
                    {
                    TEnumsPC::TItem item = iMainViewArrayPC->GetType( CurrentListboxIndex() );

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                    if ( item == TEnumsPC::EContactListItem ||
                         item == TEnumsPC::EContactItem )
                        {
                        if ( !CCASyncChecker::CheckSyncStateL() )
                            {
                            return EKeyWasConsumed;
                            }
                        }
#endif //IMPS_CONTACT_FETCH_BACKGROUND

                    switch ( item )
                        {
                        case TEnumsPC::EContactListItem:
                            {
                            TBool listCreateDelete( IMUtils::IntResourceValueL(
                                                        RSC_CHAT_VARIATION_CREATE_DELETE_CONTACT_LIST ) );
                            if ( listCreateDelete )
                                {
                                DeleteFriendL();
                                return EKeyWasConsumed;
                                }
                            return  EKeyWasNotConsumed;
                            }
                        case TEnumsPC::EContactItem:
                            {
                            DeleteFriendL();
                            return EKeyWasConsumed;
                            }
                        case TEnumsPC::EConversationItem:
                            {
                            CloseConversationL();
                            return EKeyWasConsumed;
                            }
                        case TEnumsPC::EInviteItem:
                            {
                            DeleteInvitationL();
                            return EKeyWasConsumed;
                            }
                        case TEnumsPC::EGroupItem:
                            {
                            LeaveGroupL();
                            return EKeyWasConsumed;
                            }
                        default:
                            {
                            //we should never be here.
                            break;
                            }
                        }

                    }
                }
            break;
            }
        case EKeyPhoneSend:
            {
            // modified for all item types
            TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( CurrentListboxIndex() );
            if ( !( itemtype == TEnumsPC::EOwnStatusItem
                    || itemtype == TEnumsPC::EOpenChatsListItem
                    || itemtype == TEnumsPC::EContactListItem
                    || itemtype == TEnumsPC::EConversationItem
                    || itemtype == TEnumsPC::EInviteItem ) 
                    && IsFocused() )
                {
                //if item type is only contact item then send key works
                //open the conversation with selected contact.
                HandleListBoxEventL( iListBox, EEventEnterKeyPressed );
                return EKeyWasConsumed;
                }
            }
        // Flow-through
        case EKeyRightArrow:
        case EKeyLeftArrow:
            {
            // change default sofkeys prior to view change
            //UpdateCbaL( ETrue );
            return EKeyWasNotConsumed;
            }
        default:
            {
            break;
            }
        }

    // got some key, let find box process it
    TKeyResponse response = EKeyWasNotConsumed;

    if ( iFindPaneIsVisible && iFindbox )
        {
        if ( iFindbox->OfferKeyEventL( aEvent, aType ) == EKeyWasNotConsumed &&
             iListBox )
            {
            if ( aEvent.iCode == EKeyUpArrow )
                {
                TInt topIndex = iListBox->TopItemIndex();
                TInt curIndex = iListBox->CurrentItemIndex();
                if ( topIndex == curIndex - 1 && topIndex )
                    {
                    iListBox->SetTopItemIndex( topIndex - 1 );
                    iListBox->DrawNow();
                    }
                }
            response = iListBox->OfferKeyEventL( aEvent, aType );
            }
        UpdateCbaL();
        }

    if ( response == EKeyWasConsumed )
        {
        // listbox consumed the key
        ShowInfoPopupL();
        }

    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CCASingleListContainer::HandleListBoxEventL(
//			CEikListBox* /*aListBox*/, TListBoxEvent /*aEventType*/ )
// From MEikListBoxObserver, Handles event's generated by listbox
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    // Refresh softkeys
    UpdateCbaL();

    if ( aEventType != EEventEnterKeyPressed &&
         aEventType != EEventItemDoubleClicked &&
         aEventType != EEventItemClicked )
        {
        return;
        }
    if ( iOpeningConversation )
        {
        return;
        }

    TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( CurrentListboxIndex() );

    if (
        ( ( itemType == TEnumsPC::EContactListItem ) ||
          ( itemType == TEnumsPC::EOpenChatsListItem ) )
        && ( aEventType != EEventItemDoubleClicked ) // double click is inactive
    )

        {
        // if this is list, collapse/expand it
        ExpandCollapseListL();
        return;
        }

    //own data item
    if ( itemType == TEnumsPC::EOwnStatusItem &&
         aEventType != EEventItemClicked ) // single click inactive

        {
        iCommandHelper.InvokeCommandL( EChatClientMainViewCmdChangeStatus );
        return;
        }

    //invitation item in chats folder
    if ( itemType == TEnumsPC::EInviteItem &&
         aEventType != EEventItemClicked )
        {
        iCommandHelper.InvokeCommandL( EChatPrivateListCmdOpen );
        return;
        }

    //Check if it has to be done for double click or single click event
    //group item in chats folder
    if ( itemType == TEnumsPC::EGroupItem
         && aEventType != EEventItemClicked )
        {
        iCommandHelper.InvokeCommandL( EChatClientSingleListViewCmdGroupOpen );
        return;
        }

    //conversation item in chats folder
    if ( itemType == TEnumsPC::EConversationItem &&
         aEventType != EEventItemClicked )
        {
        iCommandHelper.InvokeCommandL( EChatPrivateListCmdOpen );
        return;
        }


    if ( itemType != TEnumsPC::EContactItem
         ||  aEventType == EEventItemClicked ) // single click inactive
        {
        ShowInfoPopupL();
        return;
        }

    OpenConversationL();
    }

// ---------------------------------------------------------
// CCASingleListContainer::HandleListBoxEventL
//From MCAMainViewObserverPC, called when a contact is deleted.
// ---------------------------------------------------------
//

void CCASingleListContainer::HandleDelete( const TDesC& /*aContactId*/ , TEnumsPC::TItem /*type*/ )
    {
    CHAT_DP_TXT( "CCASingleListContainer::HandleDelete" );

    if ( iMainViewArrayPC->GetSkipStorageEvents() )
        {
        return;
        }

    TRAPD( err, UpdateViewAfterDeleteL() );
    HandleError( err );
    }

// ---------------------------------------------------------
// CCASingleListContainer::HandleInvitationEventL
//From MCAMainViewObserverPC, when a invitation is received.
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleInvitationEventL( TEnumsPC::TInviteType /*aInviteType*/,
                                                     const TDesC& /* aUserID */,
                                                     const TDesC& /* aMessage */ )
    {


    }


// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleInvitationResponse()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleInvitationResponse( TBool /* aAcceptance */,
                                                       const TDesC& /*aUserId */,
                                                       const TDesC& /*aGroupName */,
                                                       const TDesC& /*aResponse */ )
    {
    // nothing to do here since CCAppUi shows the dialog
    }

// ---------------------------------------------------------
// CCASingleListContainer::HandleListBoxEventL
// From MCAMainViewObserverPC, called when a contact is added
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleAddition( TEnumsPC::TItem aType, TInt aIndex )
    {
    if ( iMainViewArrayPC->GetSkipStorageEvents() )
        {
        return;
        }

    TRAPD( err,
           iListBox->HandleItemAdditionL();
           UpdateViewL( aIndex , aType, ETrue );
         );

    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::HandleListBoxEventL
// From MCAMainViewObserverPC, called when a contact is changed.
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleChange( TEnumsPC::TItem aType,
                                           TInt aIndex, TEnumsPC::TChange changeType )
    {
    CHAT_DP_FUNC_ENTER( "HandleChange" );

    if ( iMainViewArrayPC->GetSkipStorageEvents() )
        {
        return;
        }

    TInt error( KErrNone );

    switch ( aType )
        {
            // first handle for own data if it has..
        case TEnumsPC::EOwnStatusItem:
            // second contact ..handle it here..
        case TEnumsPC::EContactItem:
            {
            switch ( changeType )
                {
                    /*14th nov 2006 wvui20 changes
                    case TEnumsPC::EPreChange:
                        {
                        // Pre change events are received
                        // before contact lists are resorted,
                        // store current item to keep focus correct
                        // after resorting
                        iPrevItem = SelectedItem();
                        return;
                        }
                    */
                case TEnumsPC::EMultipleChanges:
                    {
                    // When logged out, items are removed from list. So for to be sure
                    // call handleItemRemoval.
                    //Note: Code Scanner warning ignored.
                    TRAP( error, iListBox->HandleItemRemovalL();

                          if ( !iFindPaneIsVisible && iListBox->Model()->NumberOfItems() > 0 )
                    {
                    ActivateFindPaneL();
                        }

                    UpdateFilterL();
                        );
                    if ( iMainViewArrayPC->Count() < 1
                         || iListBox->Model()->NumberOfItems() == 1 )
                        {
                        iListBox->SetCurrentItemIndexAndDraw( 0 );
                        }
                    TRAP( error, UpdateCbaL() );

                    break;
                    }

                case TEnumsPC::EAddition:
                case TEnumsPC::EDelete:
                case TEnumsPC::EChanged:
                    {
                    //Note: Code Scanner warning ignored.
                    TRAP( error, iListBox->HandleItemRemovalL();

                          if ( !iFindPaneIsVisible && iListBox->Model()->NumberOfItems() > 0 )
                    {
                    ActivateFindPaneL();
                        }

                    UpdateFilterL();
                        );
                    iListBox->DrawNow();
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }

        case TEnumsPC::EContactListItem:
            {
            switch ( changeType )
                {
                    /*14th nov 2006 wvui20 changes
                    case TEnumsPC::EPreChange:
                        {
                        // Pre change events are received
                        // before contact lists are resorted,
                        // store current item to keep focus correct
                        // after resorting
                        iPrevItem = SelectedItem();
                        return;
                        }
                    */
                case TEnumsPC::EChanged:
                case TEnumsPC::EAddition:
                    {
                    TBool failedFromAll = ETrue;
                    if ( iMainViewArrayPC->IsAllSynchronised( ) != TEnumsPC::ESyncSuccess )
                        {
                        // If fetching is on-going, set aList to NULL
                        // to avoid list box index updating in UpdateViewL
                        failedFromAll = EFalse;
                        }
                    //Note: Code Scanner warning ignored.
                    TRAP( error, iListBox->HandleItemAdditionL();

                          if ( !iFindPaneIsVisible && iListBox->Model()->NumberOfItems() > 0 )
                    {
                    ActivateFindPaneL();
                        }

                    UpdateFilterL();
                    UpdateViewL( aIndex, aType, failedFromAll );
                    UpdateCbaL()
                        );
                    break;
                    }
                case TEnumsPC::EDelete:
                case TEnumsPC::EMultipleChanges:
                    {
                    TRAP( error, iListBox->HandleItemRemovalL();
                          if ( !iFindPaneIsVisible && iListBox->Model()->NumberOfItems() > 0 )
                    {
                    ActivateFindPaneL();
                        }

                    UpdateFilterL();
                        );

                    if ( iMainViewArrayPC->Count() < 1
                         || iListBox->Model()->NumberOfItems() == 1 )
                        {
                        iListBox->SetCurrentItemIndexAndDraw( 0 );
                        }
                    /*14th nov 2006 wvui20 changes */
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                    else
                        {
                        TInt newIndex = CurrentListboxIndex();
                        if ( newIndex != KErrNotFound )
                            {
                            // Get real visible index for newIndex
                            CCAContactListBoxModel* list =
                                ( ( CCAContactListBoxModel* ) iListBox->Model() );
                            newIndex = list->VisibleItemIndex( newIndex );
                            }
                        if ( newIndex != KErrNotFound )
                            {
                            // Set correct index
                            iListBox->SetCurrentItemIndexAndDraw( newIndex );
                            }
                        }
#endif // IMPS_CONTACT_FETCH_BACKGROUND
                    TRAP( error, UpdateCbaL() );
                    break;
                    }
                default :
                    {
                    //Handle error condition here
                    break;
                    }
                }
            // third contact list.
            break;
            }
        case TEnumsPC::EInviteItem:
            {
            switch ( changeType )
                {
                case TEnumsPC::EDelete:
                case TEnumsPC::EChanged:
                case TEnumsPC::EAddition:
                    {
                    TRAP( error, iListBox->HandleItemRemovalL();

                          if ( !iFindPaneIsVisible && iListBox->Model()->NumberOfItems() > 0 )
                    {
                    ActivateFindPaneL();
                        }
                    UpdateFilterL();
                        );
                    iListBox->DrawNow();
                    break;
                    }
                default :
                    {
                    break;
                    }
                }
            // third contact list
            break;
            }

        case TEnumsPC::EOpenChatsListItem:
            {
            switch ( changeType )
                {
                case TEnumsPC::EChanged:
                    {
                    TCAItemDetails itemDetails = iMainViewArrayPC->GetArrayItemIdDetails( KErrNotFound );
                    TInt currentIndex( 0 );
                    TRAP( error, currentIndex = iMainViewArrayPC->GetArrayItemIdIndexL( itemDetails );
                          iListBox->HandleItemRemovalL();
                          if ( !iFindPaneIsVisible && iListBox->Model()->NumberOfItems() > 0 )
                    {
                    ActivateFindPaneL();
                        }
                    UpdateFilterL();
                        );
                    if ( KErrNotFound == currentIndex )
                        {
                        currentIndex = 0;
                        }

                    // Don't set the index if there is anything in
                    // find pane.
                    if ( iFindbox->TextLength() == 0 )
                        {
                        iListBox->SetCurrentItemIndex( currentIndex );
                        }
                    iListBox->DrawNow();
                    break;
                    }
                case TEnumsPC::EAddition:
                    {
                    TCAItemDetails itemDetails = iMainViewArrayPC->GetArrayItemIdDetails( KErrNotFound );
                    TInt currentIndex( 0 );
                    TRAP( error, currentIndex = iMainViewArrayPC->GetArrayItemIdIndexL( itemDetails );
                          iListBox->HandleItemAdditionL();
                          if ( !iFindPaneIsVisible && iListBox->Model()->NumberOfItems() > 0 )
                    {
                    ActivateFindPaneL();
                        }
                    UpdateFilterL();
                        );
                    if ( KErrNotFound == currentIndex )
                        {
                        currentIndex = 0;
                        }

                    // Don't set the index if there is anything in
                    // find pane.
                    if ( iFindbox->TextLength() == 0 )
                        {
                        iListBox->SetCurrentItemIndex( currentIndex );
                        }
                    iListBox->DrawNow();
                    break;
                    }
                case TEnumsPC::EDelete:
                    {
                    TCAItemDetails itemDetails = iMainViewArrayPC->GetArrayItemIdDetails( KErrNotFound );
                    TInt currentIndex( 0 );
                    TRAP( error, currentIndex = iMainViewArrayPC->GetArrayItemIdIndexL( itemDetails );
                          iListBox->HandleItemRemovalL();
                          if ( !iFindPaneIsVisible && iListBox->Model()->NumberOfItems() > 0 )
                    {
                    ActivateFindPaneL();
                        }

                    UpdateFilterL();
                        );
                    if ( KErrNotFound == currentIndex )
                        {
                        currentIndex = 0;
                        }

                    // Don't set the index if there is anything in
                    // find pane.
                    if ( iFindbox->TextLength() == 0 )
                        {
                        iListBox->SetCurrentItemIndex( currentIndex );
                        }
                    iListBox->DrawNow();
                    break;
                    }
                default:
                    {
                    //should never be here.
                    break;
                    }
                }
            break;
            }

        // default throw exception
        default:
            {
            //Handle error condition here
            break;
            }
        }

    iListBox->DrawNow();
    
    if ( error )
        {
        CActiveScheduler::Current()->Error( error );
        }

    CHAT_DP_FUNC_DONE( "HandleChange" );
    }

// ---------------------------------------------------------
// void CCASingleListContainer::GetHelpContext()
// Gets help context
// ---------------------------------------------------------
//
void CCASingleListContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_COMMUNITY_VIEW ;
    }

// ---------------------------------------------------------
// CCASingleListContainer::FocusChanged
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::FocusChanged( TDrawNow /* aDrawNow */ )
    {
    if ( iListBox )
        {
        // give focus to list box so that highlight animations
        // are done properly
        iListBox->SetFocus( IsFocused() );
        }
    if ( iFindbox )
        {
        iFindbox->SetFocus( IsFocused() );
        }
    }

// ---------------------------------------------------------
// void CCASingleListContainer::LayoutChangedL()
// From MCALayoutChangeObserver
// Called when layout or skin changes
// ---------------------------------------------------------
//
void CCASingleListContainer::LayoutChangedL( TInt aType /*= 0*/ )
    {
    // load this view's bitmaps
    LoadBitmapsL();
    // update status pane's bitmaps
    iStatusPane->ShowTabGroupL( KUidFriendsListView );

    if ( iFindbox && iFindPaneIsVisible )
        {
        iFindbox->MakeVisible( EFalse );
        }

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    SetRect( mainPaneRect );

    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CCASingleListContainer::HandleError( TInt aError ) const
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleError( TInt aError ) const
    {
    CHAT_DP( D_CHAT_LIT(
                 "CCASingleListContainer::HandleError, aError = %d" ),
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


// -----------------------------------------------------------------------------
// CCASingleListContainer::ResetFindBoxL
// Resets find box.
// -----------------------------------------------------------------------------
//
void CCASingleListContainer::ResetFindBoxL()
    {
    if ( iMainViewArrayPC )//!=NULL )
        {
        iMainViewArrayPC->ForceExpanded( EFalse );
        }
    if ( iForceExpandChats )
        {
        //expand the chats folder if it was collapsed.
        iMainViewArrayPC->SetCollapsed( EFalse, 1 );
        iCurrentState =	iMainViewArrayPC->IsCollapsed( 1 );
        iForceExpandChats = EFalse;
        }
    if ( iFindbox )
        {
        iFindbox->ResetL();
        iFindbox->SetSearchTextL( KNullDesC );
        iFindbox->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CCASingleListContainer::SetSkipRefreshL
// Set flag for skipping refresh
// -----------------------------------------------------------------------------
//
void CCASingleListContainer::SetSkipStorageEvents( TBool aSkip )
    {
    iSkipStorageEvents = aSkip;
    iMainViewArrayPC->SetSkipStorageEvents( aSkip );
    }

// ---------------------------------------------------------
// CCASingleListContainer::ExpandCollapseListL
// ---------------------------------------------------------
//
void CCASingleListContainer::ExpandCollapseListL()
    {
    TInt index( CurrentListboxIndex() );
    if ( index == KErrNotFound )
        {
        // listbox is empty
        return;
        }

    TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( index );
    if ( itemtype == TEnumsPC::EContactListItem ||
         itemtype == TEnumsPC::EContactItem ||
         itemtype == TEnumsPC::EOpenChatsListItem ||
         itemtype == TEnumsPC::EConversationItem ||
         itemtype == TEnumsPC::EInviteItem ||
         itemtype == TEnumsPC::EGroupItem )
        {
        // there's a contact list or contact in current position

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
        // If contact list is not synchronized, do not collapse/expand.
        TEnumsPC::TSyncStatus syncState = iMainViewArrayPC->Synchronised( index );

        if ( syncState != TEnumsPC::ESyncSuccess && itemtype == TEnumsPC::EContactListItem )
            {
            // If current list is failed, user can choose if lists should
            // be fetched again.
            if ( syncState == TEnumsPC::ESyncFailed )
                {
                TInt ret( IMDialogUtils::DisplayYesNoConfirmationDialogL(
                              R_QTN_CHAT_CONTACTLIST_FETCHAGAIN ) );
                if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
                    {
                    iMainViewArrayPC->FetchContactsL();
                    }
                }
            return;
            }
#endif //IMPS_CONTACT_FETCH_BACKGROUND

        TInt  focusedIndex = iMainViewArrayPC->IndexOfList( index, EFalse, EFalse );
        TInt items = iListBox->Model()->NumberOfItems();

        TInt filteredCount = iMainViewArrayPC->GetFilteredCount(
                                 ( TEnumsPC::TFilterType )iMyFilter, index );

        TBool collapsed( iMainViewArrayPC->IsCollapsed( index ) );
        TBool isForceExpanded( EFalse );
        if ( iFindbox && iFindbox->TextLength() > 0 )
            {
            if ( itemtype == TEnumsPC::EContactListItem ||
                 itemtype == TEnumsPC::EContactItem )
                {
                isForceExpanded = iMainViewArrayPC->IsForceExpanded();
                if ( isForceExpanded && collapsed )
                    {
                    iMainViewArrayPC->SetCollapsed( collapsed, index );
                    }
                else
                    {
                    iMainViewArrayPC->SetCollapsed( !collapsed, index );
                    }
                }
            else
                {
                isForceExpanded = iForceExpandChats;
                if ( !isForceExpanded )
                    {
                    iMainViewArrayPC->SetCollapsed( !collapsed, index );
                    }
                }

            }
        else
            {
            iMainViewArrayPC->SetCollapsed( !collapsed, index );
            }

        TInt curIndex( iListBox->CurrentItemIndex() );

        ResetFindBoxL();
        UpdateFilterL();

        if ( collapsed )
            {
            // list was expanded => more items
            iListBox->HandleItemAdditionL();

            // Shift group upwards so that as much
            // contacts as possible is shown
            items = iListBox->Model()->NumberOfItems();
            iListBox->ScrollToMakeItemVisible( curIndex + filteredCount );
            if ( focusedIndex > ( items - 1 ) )
                {
                focusedIndex = items - 1;
                }
            }
        else
            {
            // list was collapsed => less items
            iListBox->HandleItemRemovalL();
            // TInt bottomIndex = iListBox->BottomItemIndex();
            items = iListBox->Model()->NumberOfItems();
            // Collapsed list name is shown as up on the display as possible
            // No need to scroll any more by default
            // HandleItemRemovalL will take care of scrolling

            if ( focusedIndex > ( items - 1 ) )
                {
                focusedIndex = items - 1;
                }
            }

        TInt topIndex = iListBox->TopItemIndex();
        // condition for "greater than" may be removed if the focus is correct
        if ( topIndex >= focusedIndex && focusedIndex )
            {
            iListBox->SetCurrentItemIndex( focusedIndex );
            iListBox->SetTopItemIndex( focusedIndex - 1 );
            iListBox->DrawNow();
            }
        else
            {
            iListBox->SetCurrentItemIndexAndDraw( focusedIndex );
            }
        UpdateCbaL();
        }

    }

// ---------------------------------------------------------
// CCASingleListContainer::RenameListL
// ---------------------------------------------------------
//
void CCASingleListContainer::RenameListL()
    {
    TInt index( CurrentListboxIndex() );
    if ( index == KErrNotFound )
        {
        // listbox is empty
        return;
        }
    TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( index );
    if ( itemtype == TEnumsPC::EContactListItem )
        {
        // there's a contact list in current position
        //TPtrC itemname = SelectedItem();

        TPtrC listId( KNullDesC() );
        listId.Set( iMainViewArrayPC->GetSelectedListId( index ) );

        TPtrC displayName = iMainViewArrayPC->DisplayName( index );
        HBufC* name = HBufC::NewLC( KMaxWVIDLength );
        TPtr namePtr( name->Des() );
        namePtr.Copy( displayName.Left( namePtr.MaxLength() ) );
        TInt err( KErrCancel );

        // We loop until we get a valid name or user cancels
        do
            {
            TInt result( IMDialogUtils::DisplayTextQueryDialogL( namePtr,
                                                                 R_QTN_CHAT_CONTACT_LIST_NAME_QUERY,
                                                                 R_CHATCLIENT_CONVLIST_RECIP_QUERY,
                                                                 ETrue // T9
                                                               ) );
            if ( result == EAknSoftkeyOk || result == EAknSoftkeyDone )
                {
                err = iMainViewArrayPC->ListAlreadyExistsL( namePtr );

                if ( err == KErrNone )
                    {
                    iAppUi->ShowWaitDialogLC( R_QTN_GEN_NOTE_SAVING );
                    if ( CAUtils::CapitalizingEnabled() )
                        {
                        //  UI CR :
                        // Extra checking is to keep the current functionality and Newly added functionality
                        // in seperate parts.
                        HBufC* capitalizedList = CAUtils::CapitalizeListNameL( *name );
                        CleanupStack::PushL( capitalizedList );
                        err = iMainViewArrayPC->SetDisplayNameL( index, *capitalizedList );
                        CleanupStack::PopAndDestroy();
                        }
                    else
                        {
                        err = iMainViewArrayPC->SetDisplayNameL( index, *name );
                        }

                    //CodeScanner warning to be ignored
                    CleanupStack::PopAndDestroy(); // waitnote
                    if ( err != KErrNone )
                        {
                        HBufC* text = CCoeEnv::Static()->AllocReadResourceLC(
                                          R_QTN_CHAT_CONTACT_LIST_RENAMING_FAILED );
                        IMDialogUtils::DisplayErrorNoteL( *text );
                        CleanupStack::PopAndDestroy( text );
                        }
                    else
                        {
                        // rename was successfull now sort the list
                        // and return new index of renamed contact list
                        TInt newIndex = KErrNotFound;
                        newIndex = iMainViewArrayPC->SortContactListAfterRenameL( listId );
                        if ( newIndex != KErrNotFound )
                            {
                            //set index for focus
                            index = newIndex ;
                            }
                        }
                    }

                // contact list already exists
                else if ( err == KErrAlreadyExists  )
                    {
                    HBufC* text = NULL;
                    text = StringLoader::LoadLC(
                               R_QTN_FLDR_NAME_ALREADY_USED, namePtr );
                    IMDialogUtils::DisplayInformationNoteL( *text );
                    CleanupStack::PopAndDestroy( text );
                    }
                //Other error cases (like KErrUnknown )would be handled later
                }
            else
                {
                err = KErrCancel;
                }

            } while ( err != KErrNone && err != KErrCancel );
        CleanupStack::PopAndDestroy( name );
        UpdateViewL( index, TEnumsPC::EContactListItem, ETrue );
        }

    }

// ---------------------------------------------------------
// CCASingleListContainer::ContactSelectionFromPhoneBookL()
// Add friend from phonebook
// ---------------------------------------------------------
void CCASingleListContainer::ContactSelectionFromPhoneBookL()
    {
    // Check that we have even one contact list where to add the contact
    if ( CCAUINGUtils::EChatNoContactList == CCAUINGUtils::VerifyContactlistL() )
        {
        return;
        }

    iStoresOpened = EFalse;
    iOperationComplete = ETrue;
    // As user may not use the "Add From Phonebook" functionality very often
    // Contact stores should be opened only when he selects this options
    // So this piece of code should not be moved to ConstructL()
    if ( !iContactManager )
        {
        //general error for phonebook can be shown
        //we can later have a specific error string "Phonebook error" etc
        //for phonebook related error
        IMNoteMapper::ShowNoteL( EGeneralServerError );
        return;
        }
    iContactManager->ContactStoresL().OpenAllL( *this );

    }

// ---------------------------------------------------------
// CCASingleListContainer::InitPbk2SingleEntryFetchL()
// Add friend from phonebook
// ---------------------------------------------------------
void CCASingleListContainer::InitPbk2SingleEntryFetchL()
    {
    iServiceHandler = CAiwServiceHandler::NewL();
    // Attach interests
    iServiceHandler->AttachL( R_AIWSELECT_INTEREST );

    CPbk2StoreConfiguration* configuration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( configuration );
    CVPbkContactStoreUriArray* uriArray = configuration->CurrentConfigurationL();
    CleanupStack::PushL( uriArray );
    if ( !( uriArray->IsIncluded( VPbkContactStoreUris::DefaultCntDbUri() ) ) )
        {
        uriArray->AppendL( VPbkContactStoreUris::DefaultCntDbUri() );
        }

    iContactManager = CVPbkContactManager::NewL( *uriArray );

    //Note : Code scanner warning to be ignored.
    CleanupStack::PopAndDestroy( 2 ); // uriArray, configuration

    iSortOrderManager =
        CPbk2SortOrderManager::NewL( iContactManager->FieldTypes() );

    iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL(
                         iContactManager->FieldTypes(),
                         *iSortOrderManager );

    // Construct empty filter
    iContactViewFilter =
        CVPbkFieldTypeSelector::NewL( iContactManager->FieldTypes() );

    // Append the filter object with suitable criteria
    VPbkContactViewFilterBuilder::BuildContactViewFilterL
    ( *iContactViewFilter, EVPbkContactViewFilterPhoneNumber,
      *iContactManager );

    }

// ---------------------------------------------------------
// CCASingleListContainer::CloseAllContactStores()
// Add friend from phonebook
// ---------------------------------------------------------
void CCASingleListContainer::CloseAllContactStores()
    {
    if ( iContactManager )
        {
        TRAP_IGNORE( iContactManager->ContactStoresL().CloseAll( *this ) );
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::ReleasePbkSingleEntryFetch()
// Add friend from phonebook
// ---------------------------------------------------------
void CCASingleListContainer::ReleasePbkSingleEntryFetch()
    {
    if ( iServiceHandler )
        {
        TRAP_IGNORE( iServiceHandler->DetachL( R_AIWSELECT_INTEREST ) );
        }
    delete iNameFormatter;
    iNameFormatter = NULL;

    delete iSortOrderManager;
    iSortOrderManager = NULL;

    delete iContactViewFilter;
    iContactViewFilter = NULL;

    delete iContactManager;
    iContactManager = NULL;

    delete iServiceHandler;
    iServiceHandler = NULL;
    }

// --------------------------------------------------------------------------
// CCASingleListContainer::ExecutePbkSingleEntryFetchL
// --------------------------------------------------------------------------
//
void CCASingleListContainer::ExecutePbkSingleEntryFetchL
( TAiwSingleEntrySelectionDataV2 aData,
  const TDesC& aUri1,
  const TDesC& aUri2,
  const TDesC& aUri3 )
    {
    TAiwSingleEntrySelectionDataV2Pckg  dataPckg( aData );
    TAiwVariant variant( dataPckg );
    TAiwGenericParam param( EGenericParamContactSelectionData, variant );
    CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    inParamList.AppendL( param );

    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewL();
    CleanupStack::PushL( uriArray );
    if ( aUri1.Length() > 0 ) uriArray->AppendL( aUri1 );
    if ( aUri2.Length() > 0 ) uriArray->AppendL( aUri2 );
    if ( aUri3.Length() > 0 ) uriArray->AppendL( aUri3 );

    HBufC8* packedUris = uriArray->PackLC();
    if ( uriArray->Count() > 0 )
        {
        inParamList.AppendL(
            TAiwGenericParam(
                EGenericParamContactStoreUriArray,
                TAiwVariant( *packedUris ) ) );
        }

    iServiceHandler->ExecuteServiceCmdL(
        KAiwCmdSelect,
        inParamList,
        iServiceHandler->OutParamListL(),
        KAiwOptASyncronous,
        this );
    //Note: Code Scanner warning to be ignored
    CleanupStack::PopAndDestroy( 2 ); // packedUris, uriArray

    }


// ---------------------------------------------------------------------------
// Retrive the selected contact from parameter list
// ---------------------------------------------------------------------------
//
TInt CCASingleListContainer::HandleNotifyL( TInt aCmdId, TInt aEventId,
                                            CAiwGenericParamList& aEventParamList, const
                                            CAiwGenericParamList& /*aInParamList*/ )
    {
    TInt result( 0 );
    if ( aCmdId == KAiwCmdSelect )
        {
        if ( aEventId == KAiwEventCompleted )
            {
            TInt paramIndex = 0;

            //Retrive the parameter from parameter list
            //
            const TAiwGenericParam* contactParam =
                aEventParamList.FindFirst( paramIndex,
                                           EGenericParamContactLinkArray );

            if ( !iStoresOpened )
                {
                CloseAllContactStores();
                IMNoteMapper::ShowNoteL( EGeneralServerError );
                return result;
                }

            // Retrive the storecontact from parameter
            //
            if ( contactParam && iStoresOpened )
                {
                //Get the contact link from parameter
                //
                TPtrC8 result = contactParam->Value().AsData();
                MVPbkContactLinkArray* contactLinkArray =
                    iContactManager->CreateLinksLC( result );

                const MVPbkContactLink* contactLink = contactLinkArray->At( 0 ).CloneLC();
                //Use the contact manager to retrive the contact store
                iContactManager->RetrieveContactL( *contactLink, *this );

                //Note:CodeScanner warning to be ignored.
                CleanupStack::PopAndDestroy( 2 ); //contactLink, contactLinkArray
                }
            }

        else if ( aEventId == KAiwEventCanceled )
            {
            CloseAllContactStores();
            }
        else if ( aEventId == KAiwEventOutParamCheck )
            {
            // should always be 1(yes)
            result = 1;
            }
        else if ( aEventId == KAiwEventError || aEventId == KAiwEventStopped )
            {
            CloseAllContactStores();
            IMNoteMapper::ShowNoteL( EGeneralServerError );
            }
        }
    return result;
    }


// -----------------------------------------------------------------------------
// CCASingleListContainer::GetWVIdOfSelPbkContactL
// Adds a new contact from phone book
// -----------------------------------------------------------------------------
//
HBufC* CCASingleListContainer::GetWVIdOfSelPbkContactL( const TDesC& aTitle,
                                                        const TDesC& aNumberToSearch	)
    {
    HBufC* myWVID = HBufC::NewLC( KWVUserIDMaxLength );

    if ( aNumberToSearch.Length() > 0 )
        {
        TPtrC myDebug( aNumberToSearch );
        CHAT_DP( D_CHAT_LIT(
                     "CCASingleListContainer::GetWVIdOfSelPbkContactL number to search %S" )
                 , &myDebug );

        myDebug.Set( aTitle );
        CHAT_DP( D_CHAT_LIT(
                     "CCASingleListContainer::GetWVIdOfSelPbkContactL title:%S" ),
                 &myDebug );

        HBufC* text = StringLoader::LoadLC( R_QTN_CHAT_SEARCHVIEW_PROCESSING );
        iAppUi->ShowWaitDialogL( *text, ETrue );
        CleanupStack::PopAndDestroy( text );

        CSearchPairs* pairs = new ( ELeave ) CSearchPairs( 1 );
        CleanupStack::PushL( pairs );

        CImpsSearchRequest* request = CImpsSearchRequest::NewL();
        CleanupStack::PushL( request );

        request->SetRequestL( EImpsUserMobileNumber, aNumberToSearch );
        pairs->AppendL( request );

        TInt err( KErrNone );

        TRAPD( leave, err = iSearchInterfacePC->StartSearchL(
                                KSearchLimit, NULL, pairs ) );
        iAppUi->DismissWaitDialogL( leave );

        if ( err == KErrNone )
            {

            TInt resultCount( iDataInterfacePC->SearchDataCount() );
            CHAT_DP( D_CHAT_LIT(
                         "CCASingleListContainer::GetWVIdOfSelPbkContactL results:%d" ),
                     resultCount );

            if ( resultCount > 0 )
                {
                if ( resultCount > 1 )
                    {
                    text = StringLoader::LoadLC( R_QTN_CHAT_FOUND_MANY_IDS,
                                                 resultCount );

                    IMDialogUtils::DisplayQueryDialogL( R_CHAT_CONFOK_TEMPLATE, *text );

                    CleanupStack::PopAndDestroy( text );

                    CDesCArrayFlat* itemList = new ( ELeave )
                    CDesCArrayFlat(
                        KArrayGranularity );
                    CleanupStack::PushL( itemList );

                    for ( TInt count( 0 ); count < resultCount; ++count )
                        {
                        myDebug.Set( iDataInterfacePC->SearchData( count ) );
                        CHAT_DP( D_CHAT_LIT(
                                     "CCASingleListContainer::GetWVIdOfSelPbkContactL result:%S" ),
                                 &myDebug );
                        itemList->AppendL( iAppUi->DisplayId(
                                               iDataInterfacePC->SearchData( count ) ) );
                        }

                    TInt selectionIndex( 0 );
                    TInt ret( IMDialogUtils::DisplayListQueryDialogL(
                                  &selectionIndex,
                                  itemList,
                                  R_CHAT_ADDPB_LIST_QUERY ) );

                    CHAT_DP( D_CHAT_LIT(
                                 "CCASingleListContainer::GetWVIdOfSelPbkContactL ret %d" )
                             , ret );

                    CleanupStack::PopAndDestroy( itemList );

                    if ( ret == 0 )
                        {
                        // User cancelled list query, cleanup and return NULL
                        CleanupStack::PopAndDestroy( 3, myWVID );
                        return NULL;
                        }

                    ( *myWVID ) = iDataInterfacePC->SearchData( selectionIndex );
                    }
                else
                    {
                    //else part of 4th condition,if ( resultCount > 1 )
                    myDebug.Set( iDataInterfacePC->SearchData( 0 ) );
                    CHAT_DP( D_CHAT_LIT(
                                 "CCASingleListContainer::GetWVIdOfSelPbkContactL result:%S" )
                             , &myDebug );

                    ( *myWVID ) = iDataInterfacePC->SearchData( 0 );
                    }
                //ok just one selected
                }
            else
                {
                //else part of 3rd condition, if ( resultCount > 0 )
                //no hits
                IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_CONT_NOT_FOUND, aTitle );
                CHAT_DP_TXT(
                    "CCASingleListContainer::GetWVIdOfSelPbkContactL no user found by number" );

                TUint fetchFlags = 0;
                //remove setflags if not required
                ExecutePbkSingleEntryFetchL( TAiwSingleEntrySelectionDataV2().
                                             SetFetchFilter( iContactViewFilter ).
                                             SetFlags( fetchFlags )
                                           );
                //Reset the flag to so that contact stores should not get closed
                // for this case as still single entry fetch is going on
                iOperationComplete = EFalse;
                }

            }
        else
            {
            //else part of 2nd condition ,if ( err == KErrNone )
            IMNoteMapper::ShowNoteL( err );
            }

        //CodeScanner warning to be ignored
        CleanupStack::PopAndDestroy( 2, pairs ); // request, pairs
        }
    else
        {
        //else part of first condition, if(aNumberToSearch.Length() >0)
        //oops, no number , handle no. of results = zero
        }



    //lets check if we have data
    if ( myWVID->Length() )
        {
        CleanupStack::Pop( myWVID );
        }
    else
        {
        //remove and return NULL if we dont have anything
        CleanupStack::PopAndDestroy( myWVID );
        myWVID = NULL;
        }

    return myWVID;

    }


// ---------------------------------------------------------
// CCASingleListContainer::AddSelectedPbk2ContactL()
// Note: CodeScanner is likely to return false positives for
// this situation
// ---------------------------------------------------------
//
void CCASingleListContainer::AddSelectedPbk2ContactL( MVPbkStoreContact* aContact )
    {

    HBufC* myTitle = HBufC::NewLC( KWVUserIDMaxLength );
    HBufC* myNumberToSearch = HBufC::NewLC( KWVUserIDMaxLength );

    //check return value
    TPtr myTitlePtr( myTitle->Des() );
    TPtr myNumberToSearchPtr( myNumberToSearch->Des() );

    CDesCArray* selectedContacts = new ( ELeave ) CDesCArrayFlat( 10 );
    CleanupStack::PushL( selectedContacts );
    TInt fieldCount = aContact->Fields().FieldCount();
    for ( TInt i( 0 ); i < fieldCount; ++i )
        {
        const MVPbkStoreContactField& field =  aContact->Fields().FieldAt( i );
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( field.FieldData().DataType() == EVPbkFieldStorageTypeText )
            {
            if ( ContainsFieldType( fieldType->VersitProperties(), EVPbkVersitNameTEL ) )
                {
                TPtrC text = MVPbkContactFieldTextData::Cast( field.FieldData() ).Text();
                selectedContacts->AppendL( text );
                }
            }
        }

    if ( iNameFormatter )
        {
        HBufC* tempTitle = iNameFormatter->GetContactTitleL( aContact->Fields(), 0 );
        myTitlePtr = *tempTitle;
        delete tempTitle;
        }
    // check if there are multiple mobile numbers, if only one Mobile number, then no need
    // to show the dialog.
    if ( 1 < selectedContacts->Count() )
        {
        TInt selectionIndex = -1;
        IMDialogUtils::DisplayPopupListL( selectionIndex, selectedContacts, myTitlePtr, ELbmDoesNotOwnItemArray, KErrNotFound );

        // if Back is pressed then the list of contacts is shown.
        if ( -1 == selectionIndex )
            {
            TUint fetchFlags = 0;
            //remove setflags if not required
            ExecutePbkSingleEntryFetchL( TAiwSingleEntrySelectionDataV2().
                                         SetFetchFilter( iContactViewFilter ).
                                         SetFlags( fetchFlags )
                                       );
            //Reset the flag to so that contact stores should not get closed
            // for this case as still single entry fetch is going on
            iOperationComplete = EFalse;
            CleanupStack::PopAndDestroy( 3 ); //selectedcontacts,myNumberToSearch,myTitle
            return;
            }

        myNumberToSearchPtr = selectedContacts->MdcaPoint( selectionIndex ) ;
        }

    else if ( 0 < selectedContacts->Count() )
        {
        // since there is only one contact, hence the index is 0
        myNumberToSearchPtr = selectedContacts->MdcaPoint( 0 ) ;
        }
    else
        {
        // There will be a note here.
        }

    HBufC* myWVID = GetWVIdOfSelPbkContactL( *myTitle, *myNumberToSearch );
    if ( myWVID )
        {
        CleanupStack::PushL( myWVID );
        TPtrC myDebug( myWVID->Des() );
        CHAT_DP( D_CHAT_LIT(
                     "CCASingleListContainer::AddSelectedPbk2ContactL number to search %S" )
                 , &myDebug );

        HBufC* titleText = iStatusPane->Title()->AllocLC();

        TInt index = CurrentListboxIndex();
        // creating new
        TBool contactSaved( EFalse );

        //as we are goin to add a new contact the index passed to Contact Editor
        //must be -1. So we can avoid initialisation with wrong data
        CCAContactEditor* contactEditor =
            CCAContactEditor::NewL( -1, contactSaved );


        TPtrC listId( KNullDesC() );
        TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( index );
        if ( itemType == TEnumsPC::EContactListItem || itemType == TEnumsPC::EContactListItem )
            {
            listId.Set( iMainViewArrayPC->GetSelectedListId( index ) ) ;
            }

        TInt err;
        TRAP( err, contactEditor->EditContactLD( TEnumsPC::EAddToContact,
                                                 listId, *myWVID, KNullDesC ) );
        iStatusPane->SetTitleL( *titleText );

        //CodeScanner warning to be ignored
        CleanupStack::PopAndDestroy(); // titleText
        CleanupStack::PopAndDestroy( ); //myWVID
        }

    CleanupStack::PopAndDestroy( 3 ); //selectedcontacts,myNumberToSearch,myTitle

    // restore the navipane since CPbkMultipleEntryFetchDialog leaves it empty
    iStatusPane->ShowTabGroupL( KUidFriendsListView );
    }

// -----------------------------------------------------------------------------
// CCASingleListContainer::ContainsFieldType
// -----------------------------------------------------------------------------
//
TBool CCASingleListContainer::ContainsFieldType( TArray<TVPbkFieldVersitProperty> aArray,
                                                 TVPbkFieldTypeName aName )
    {
    TBool ret = EFalse;
    const TInt size = aArray.Count();
    for ( TInt i = 0; i < size; ++i )
        {
        /*if (aArray[i].Name() == aName)
            {
            ret = ETrue;
            break;
            }*/

        if ( aArray[i].Parameters().Contains( EVPbkVersitParamCELL ) )
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }


// --------------------------------------------------------------------------
// CCASingleListContainer::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------

void CCASingleListContainer::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& aOperation,
    MVPbkStoreContact* aContact )
    {
    TRAP_IGNORE( AddSelectedPbk2ContactL( aContact ) );
    delete &aOperation;
    delete aContact;
    if ( iOperationComplete )
        {
        CloseAllContactStores();
        }
    //reset it for next single entry fetch from Phonebook
    iOperationComplete = ETrue;
    }


// --------------------------------------------------------------------------
// CCASingleListContainer::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------

void CCASingleListContainer::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& aOperation,
    TInt aError )
    {
    delete &aOperation;
    CloseAllContactStores();
    TRAP_IGNORE( IMNoteMapper::ShowNoteL( aError ) );
    }


// --------------------------------------------------------------------------
// CCASingleListContainer::OpenComplete
// --------------------------------------------------------------------------
//
void CCASingleListContainer::OpenComplete( )
    {
    TUint fetchFlags = 0;
    //remove setflags if not required
    TRAPD( err, ExecutePbkSingleEntryFetchL( TAiwSingleEntrySelectionDataV2().
                                             SetFetchFilter( iContactViewFilter ).
                                             SetFlags( fetchFlags )
                                           ) );
    if ( err != KErrNone )
        {
        TRAP_IGNORE( IMNoteMapper::ShowNoteL( err ) );
        }
    }

// --------------------------------------------------------------------------
// CCASingleListContainer::StoreReady
// --------------------------------------------------------------------------
//
void CCASingleListContainer::StoreReady( MVPbkContactStore& /*aContactStore*/ )
    {
    iStoresOpened = ETrue;
    }

// --------------------------------------------------------------------------
// CCASingleListContainer::StoreUnavailable
// --------------------------------------------------------------------------
//
void CCASingleListContainer::StoreUnavailable
( MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CCASingleListContainer::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CCASingleListContainer::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/,
                                                TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Do nothing
    }

// ---------------------------------------------------------
// CCASingleListView::HandleWatcherEvent
// @see MCAWatcherObserver
// ---------------------------------------------------------
void CCASingleListContainer::HandleWatcherEvent( const TDesC& aString,
                                                 TEnumsPC::TOnlineStatus aOnlineStatus )
    {
    CHAT_DP_TXT( "UI received WATCHER EVENT!" );
    // We can't leave here so we have to ignore
    // possible watcher event handling errors
    TRAPD( err, DoHandleWatcherEventL( aString, aOnlineStatus ) );
    HandleError( err );
    }


// ---------------------------------------------------------
// CCASingleListContainer::DoHandleWatcherEventL()
// Handles watcher events
// @see MCAWatcherObserver for more information
// ---------------------------------------------------------
//
void CCASingleListContainer::DoHandleWatcherEventL( const TDesC& aString,
                                                    TEnumsPC::TOnlineStatus aOnlineStatus )
    {

    TInt globalSoftNoteId = -1;

    CHAT_DP_TXT( "CCASingleListContainer::DoHandleWatcherEventL - show note" );
    CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
    globalNote->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY );
    HBufC* text = NULL;

    switch ( aOnlineStatus )
        {
        case TEnumsPC::EUnknown:
            {
            return;
            }
        case TEnumsPC::EOnline:
            {
            text = StringLoader::LoadLC( R_CHAT_SNOTIF_WATCH_CHG_ON, aString );
            break;
            }
        case TEnumsPC::EInvisible:
            {
            break;
            }
        case TEnumsPC::EAway:
            {
            text = StringLoader::LoadLC( R_CHAT_SNOTIF_WATCH_CHG_AWAY, aString );
            break;
            }
        case TEnumsPC::EBusy:
            {
            text = StringLoader::LoadLC( R_CHAT_SNOTIF_WATCH_CHG_BUSY, aString );
            break;
            }
        default:
            {
            text = StringLoader::LoadLC( R_CHAT_SNOTIF_WATCH_CHG_OFF, aString );
            break;
            }
        }

    globalSoftNoteId = globalNote->ShowNoteL( EAknGlobalInformationNote, *text );
    CHAT_DP( D_CHAT_LIT( " **** note id %d **** " ), globalSoftNoteId );

    //CodeScanner warning to be ignored
    CleanupStack::PopAndDestroy( 2, globalNote );
    }


// ---------------------------------------------------------
// CCASingleListContainer::ShowInfoPopupL()
// ---------------------------------------------------------
//
void CCASingleListContainer::ShowInfoPopupL()
    {
    iInfoPopup->HideInfoPopupNote();

    TInt index = CurrentListboxIndex();
    TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( index );
    if ( itemType == TEnumsPC::EOwnStatusItem || itemType == TEnumsPC::EContactItem )
        {
        const TDesC& statusText = iMainViewArrayPC->StatusText( index );
        TEnumsPC::TOnlineStatus status = iMainViewArrayPC->GetOnlineStatus( index );
        if ( ( status == TEnumsPC::EOnline ||
               status == TEnumsPC::EAway   ||
               status == TEnumsPC::EBusy )
             && statusText.Length() > 0 && !iMainViewArrayPC->IsBlocked( index ) )
            {
            iInfoPopup->SetTextL( statusText );
            iInfoPopup->ShowInfoPopupNote();
            }
        }
    }
// ---------------------------------------------------------
// CCASingleListContainer::ListFilter()
// ---------------------------------------------------------
//
CAknListBoxFilterItems* CCASingleListContainer::ListFilter()
    {
    return static_cast<CCAContactListBoxModel*>( iListBox->Model() )->Filter();
    }

// ---------------------------------------------------------
// CCASingleListContainer::UpdateCbaL()
// Updates Softkeys according to current focus.
// ---------------------------------------------------------
//
void CCASingleListContainer::UpdateCbaL( TBool aUseDefaultCba /*= EFalse*/ )
    {
    // If true, stop the cba update; otherwise go update cba
    if ( iAppUi->GetStopUpdateCba() )
        return;

    TInt cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__EMPTY;
    if ( !iCbaLock )
        {
        TInt index = CurrentListboxIndex();
        if ( index < 0 || index > Count() )
            {
            if ( iCba )
                {
                iCba->SetCommandSetL( cbaRes );
                iCba->DrawNow();
                }
            return;
            }

        TPtrC contact = iMainViewArrayPC->GetItemNameText( index );
        TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( index );
        TBool updateinvalid( iLastFocusedItemType == TEnumsPC::EInvalid );
        if ( !iCba )
            {
            iCba = CEikButtonGroupContainer::Current();
            }

        // default softkeys
        if ( aUseDefaultCba || updateinvalid || itemtype == TEnumsPC::EContactListItem
             || itemtype == TEnumsPC::EOpenChatsListItem )
            {
            if ( aUseDefaultCba )
                {
                iLastFocusedItemType = TEnumsPC::EOwnStatusItem;
                }
            else
                {
                iLastFocusedItemType = itemtype;
                }

            if ( iCba )
                {
                if ( itemtype == TEnumsPC::EOwnStatusItem )
                    {
                    // Own status
                    // Options/Back/Change status
                    cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__CHANGESTATUS;
                    }

                else if ( itemtype == TEnumsPC::EContactListItem )
                    {

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                    if ( iMainViewArrayPC->Synchronised( index ) ==
                         TEnumsPC::ESyncFailed )
                        {
                        // Failed list
                        // Options/Back/Fetch
                        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__FETCH;
                        }
                    else if ( iMainViewArrayPC->Synchronised( index ) ==
                              TEnumsPC::ESyncNotDone )
                        {
                        // Not yet synhronised
                        // Options/Back
                        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__EMPTY;
                        }
                    else
#endif  // IMPS_CONTACT_FETCH_BACKGROUND
                        if ( iMainViewArrayPC->IsCollapsed( index )
                             && !iMainViewArrayPC->IsForceExpanded() )
                            {
                            // Collapsed
                            // Options/Back/Expand
                            cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__EXPAND;
                            }
                        else
                            {
                            // Expanded
                            // Options/Back/Collapse
                            cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__COLLAPSE;
                            }
                    }
                else if ( itemtype == TEnumsPC::EOpenChatsListItem )
                    {
                    if ( iMainViewArrayPC->IsCollapsed( index )
                         && !iForceExpandChats )
                        {
                        // Collapsed
                        // Options/Back/Expand
                        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__EXPAND;
                        }
                    else
                        {
                        // Expanded
                        // Options/Back/Collapse
                        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__COLLAPSE;
                        }
                    }
                iCba->SetCommandSetL( cbaRes );
                iCba->DrawNow();
                }
            }

        // collapse softkey ( when focus is on contact )
        else if ( updateinvalid || itemtype == TEnumsPC::EContactItem )
            {
            iLastFocusedItemType = itemtype;
            if ( iCba )
                {
                // Options/Collapse/Chat
                iCba->SetCommandSetL( R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__CHAT );
                iCba->DrawNow();
                }
            }
        // collapse softkey ( when focus is on ConversationItem )
        else if ( updateinvalid || itemtype == TEnumsPC::EConversationItem )
            {
            iLastFocusedItemType = itemtype;
            if ( iCba )
                {
                // Options/Collapse/Chat
                iCba->SetCommandSetL( R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__OPEN );
                iCba->DrawNow();
                }
            }
        // Focus on own contact item
        else if ( updateinvalid || itemtype == TEnumsPC::EOwnStatusItem )
            {
            iLastFocusedItemType = itemtype;
            if ( iCba )
                {
                iCba->SetCommandSetL( R_CHAT_SOFTKEYS_OPTIONS_EXIT__CHANGESTATUS );
                iCba->DrawNow();
                }
            }
        // Focus on invitation item
        else if ( updateinvalid || itemtype == TEnumsPC::EInviteItem )
            {
            iLastFocusedItemType = itemtype;
            if ( iCba )
                {
                iCba->SetCommandSetL( R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__OPENINVITATION );
                iCba->DrawNow();
                }
            }

        // Focus on group item
        else if ( updateinvalid || itemtype == TEnumsPC::EGroupItem )
            {
            iLastFocusedItemType = itemtype;
            if ( iCba )
                {
                //check if group is joined or not n accordingly
                //set msk as open or join
                if ( IsJoinedToGroup() )
                    {
                    iCba->SetCommandSetL( R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__OPENGROUP );
                    }
                else
                    {
                    iCba->SetCommandSetL( R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__JOINGROUP );
                    }
                iCba->DrawNow();
                }
            }

        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::DeleteSelfD
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DeleteSelfD()
    {
    iDeleteFlag = ETrue;

    // if task is already completed -> delete immediately
    if ( iTaskComplete )
        {
        delete this;
        }
    }
// ---------------------------------------------------------
// CCASingleListContainer::IsChatsForceExpanded
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASingleListContainer::IsChatsForceExpanded()
    {
    return iForceExpandChats;
    }
// ---------------------------------------------------------
// CCASingleListContainer::HandleControlEventL
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleControlEventL( CCoeControl* /*aControl*/, TCoeEvent aEventType )
    {
    // We only observe iFindbox
    if ( EEventStateChanged == aEventType )
        {
        iCurrentState = iMainViewArrayPC->IsCollapsed( 1 );
        TBool forceExpand = EFalse;
        iForceExpandChats = EFalse;
        if ( iFindbox && iFindbox->TextLength() > 0 )
            {
            // we have active search going on --> expand all contact lists
            forceExpand = ETrue;
            //Force expansion on the chats folder is done only if its collapsed.
            if ( iCurrentState )
                {
                iForceExpandChats = ETrue;
                }
            iPreviousState = iCurrentState;
            }
        iMainViewArrayPC->ForceExpanded( forceExpand );
        if ( iForceExpandChats )
            {
            //expand the chats folder if it was collapsed.
            iMainViewArrayPC->SetCollapsed( EFalse, 1 );
            iCurrentState =	iMainViewArrayPC->IsCollapsed( 1 );
            }

        // set the chats folder to the previous state.
        else if ( EFalse == iForceExpandChats && iPreviousState != iCurrentState )
            {
            iMainViewArrayPC->SetCollapsed( EFalse, 1 );
            iPreviousState = iMainViewArrayPC->IsCollapsed( 1 );
            }
        // Update filter
        ( ( CAknFilteredTextListBoxModel* ) iListBox->Model() )
        ->Filter()->HandleOfferkeyEventL();
        UpdateFilterL();

        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::SetListboxIndexL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::SetListboxIndexL( TInt aIndex )
    {
    if ( iListBox )
        {
        if ( aIndex < 0 || aIndex >= iListBox->Model()->NumberOfItems() )
            {
            aIndex = 0;
            }

        iListBox->SetCurrentItemIndexAndDraw( aIndex );

        // If true, stop the cba update; otherwise go update cba
        if ( iAppUi->GetStopUpdateCba() )
            return;

        //iCba = CEikButtonGroupContainer::Current();
        // Update softkeys
        if ( iCba )
            {
            TPtrC curitem = iMainViewArrayPC->GetItemNameText( aIndex );
            TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( aIndex );

            TInt cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__EMPTY;
            if ( itemtype == TEnumsPC::EOwnStatusItem )
                {
                // Own status
                // Options/Back/Change status
                cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__CHANGESTATUS;
                }
            else if ( itemtype == TEnumsPC::EContactListItem )
                {
                // Contact list
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                if ( iMainViewArrayPC->Synchronised( aIndex ) ==
                     TEnumsPC::ESyncFailed )
                    {
                    // Failed list
                    // Options/Back/Fetch
                    cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__FETCH;
                    }
                else if ( iMainViewArrayPC->Synchronised( aIndex ) ==
                          TEnumsPC::ESyncNotDone )
                    {
                    // Not yet synhronised
                    // Options/Back
                    cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__EMPTY;
                    }
                else
#endif  // IMPS_CONTACT_FETCH_BACKGROUND
                    if ( iMainViewArrayPC->IsCollapsed( aIndex ) )
                        {
                        // Collapsed
                        // Options/Back/Expand
                        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__EXPAND;
                        }
                    else
                        {
                        // Expanded
                        // Options/Back/Collapse
                        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__COLLAPSE;
                        }
                }

            else if ( itemtype == TEnumsPC::EOpenChatsListItem )
                {
                if ( iMainViewArrayPC->IsCollapsed( aIndex ) )
                    {
                    // Collapsed
                    // Options/Back/Expand
                    cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__EXPAND;
                    }
                else
                    {
                    // Expanded
                    // Options/Back/Collapse
                    cbaRes = R_CHAT_SOFTKEYS_OPTIONS_EXIT__COLLAPSE;
                    }
                }
            else if ( itemtype == TEnumsPC::EContactItem )
                {
                cbaRes = R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__CHAT;
                }

            else if ( itemtype == TEnumsPC::EConversationItem )
                {
                cbaRes = R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__OPEN;
                }

            else if ( itemtype == TEnumsPC::EInviteItem )
                {
                cbaRes = R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__OPENINVITATION;
                }
            else if ( itemtype == TEnumsPC::EGroupItem )
                {
                //check if group is joined or not n accordingly
                //set msk as open or join
                if ( IsJoinedToGroup() )
                    {
                    cbaRes = R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__OPENGROUP;
                    }
                else
                    {
                    cbaRes = R_CHAT_SOFTKEYS_OPTIONS_COLLAPSE__JOINGROUP;
                    }
                }

            iCba->SetCommandSetL( cbaRes );
            iCba->DrawNow();
            }
        }
    }


// ---------------------------------------------------------
// CCASingleListContainer::HandleSettingsChangeL
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleSettingsChangeL( TInt aChangedSettingEnum )
    {
    CHAT_DP_FUNC_ENTER( "CCASingleListContainer::HandleSettingsChangeL" );

    if ( ( aChangedSettingEnum != TEnumsPC::EShowOffline ) &&
         ( aChangedSettingEnum != TEnumsPC::EAutomaticPresenceUpdate ) )
        {
        // it's not for us
        CHAT_DP_FUNC_DP( "HandleSettingsChangeL",
                         "Got event but it was not for me" );
        return;
        }

    MCASettingsPC* settings = iAppUi->GetProcessManager().GetSettingsInterface( );

    if ( aChangedSettingEnum == TEnumsPC::EShowOffline )
        {
        TEnumsPC::TCASettingValues newValue = ( TEnumsPC::TCASettingValues )
                                              settings->GetBoolValuePC( TEnumsPC::EShowOffline, NULL );

        if ( newValue == iShowOffline )
            {
            CHAT_DP_FUNC_DP( "HandleSettingsChangeL",
                             "Unchanged setting state for filter" );
            return;
            }
        TInt resValue = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING );
        if ( !newValue )
            {
            iMyFilter = TEnumsPC::EFilterNonOffline;
            iShowOffline = EFalse;
            if ( resValue != 2 )
                {
                iMyFilter = TEnumsPC::EFilterNonOfflineNonBlockedAndOperation;
                }
            }
        else
            {
            iMyFilter = TEnumsPC::EFilterAll;
            iShowOffline = ETrue;
            if ( resValue != 2 )
                {
                iMyFilter = TEnumsPC::EFilterAllNonBlockedAndOperation;
                }
            }
        //// WVUi20 merged code as of wk45 14Nov,2006
        // Get current item
        TCAItemDetails itemDetails;
        TInt oldIndex = CurrentListboxIndex();
        itemDetails = iMainViewArrayPC->GetArrayItemIdDetails( oldIndex );

        //Set the filter for engine's contact list model
        iMainViewArrayPC->SetDefaultFilter( ( TEnumsPC::TFilterType )iMyFilter );
        iMainViewArrayPC->ResetDefaultFilter();
        //We DONT need a different filter for the list indicators.
        //The value on ORing and Anding as below is 21 always
        TEnumsPC::TFilterType singleListArrayFilter( TEnumsPC::EFilterNotDefined );
        if ( iMyFilter == TEnumsPC::EFilterNonOfflineNonBlockedAndOperation )
            {
            singleListArrayFilter = TEnumsPC::EFilterNonOfflineNonBlockedNonOfflineOperation;
            }
        else
            {
            singleListArrayFilter = TEnumsPC::EFilterAllNonBlockedNonOfflineOperation;
            }

        iMainViewArrayPC->ResetArray( EFalse );
        CCASingleListViewArray* singleListArray =
            static_cast<CCASingleListViewArray*>( iListBox->Model()->ItemTextArray() );
        //Set the filter for single list array
        iMainViewArrayPC->SetSingleListArrayFilter( ( TEnumsPC::TFilterType )singleListArrayFilter );

        // Once the presence info is changed list box need to be updated, ex
        // when the presence settings has changed from online to offline
        UpdateFilterL();

        // Set index correct
        TInt index = KErrNotFound;

        TBool needSetCurrentItemIndex = ETrue;

        // Try to find item
        if ( itemDetails.aItem == TEnumsPC::EContactListItem )
            {
            index = iMainViewArrayPC->FindContactListIndexL( itemDetails.aListID );
            }
        if ( itemDetails.aItem == TEnumsPC::EContactItem )
            {
            index = iMainViewArrayPC->FindContactL( itemDetails.aListID, itemDetails.aItemID );
            }
        if ( index == KErrNotFound )
            {
            // Item was not found anymore, keep old index
            TInt itemCount = iListBox->Model()->NumberOfItems();

            if ( itemCount <= 0 )
                {
                needSetCurrentItemIndex = EFalse;
                }
            else if ( oldIndex >= itemCount )
                {
                // Check for overflow
                index = itemCount - 1;
                }
            else if ( ( oldIndex >= 0 ) && ( oldIndex < itemCount ) )
                {
                index = oldIndex;
                }
            else
                {
                index = 0;
                }
            }

        if ( needSetCurrentItemIndex )
            {
            iListBox->SetCurrentItemIndex( index );
            }
        }

    if ( aChangedSettingEnum == TEnumsPC::EAutomaticPresenceUpdate )
        {
        // get the automatic presence update info
        TBool newAutoUpdate = ( TBool )
                              settings->GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate, NULL );
        CCASingleListViewArray* friendsArray =
            static_cast<CCASingleListViewArray*>( iListBox->Model()->ItemTextArray() );
        friendsArray->SetAutomaticRefresh( newAutoUpdate );
        }

    iListBox->DrawNow();
    }
// ---------------------------------------------------------
// CCASingleListContainer::HandleItemAdditionL
// ---------------------------------------------------------
//
void CCASingleListContainer::HandleItemAdditionL()
    {

    iListBox->HandleItemAdditionL();

    iListBox->DrawNow();
    }


// ---------------------------------------------------------
// CCASingleListContainer::OpenInvitationL()
// (other items were commented in a header).
// ---------------------------------------------------------
void CCASingleListContainer::OpenInvitationL( )

    {
    TInt index = CurrentListboxIndex();

    // just to make sure
    if ( iMainViewArrayPC->GetType( index ) != TEnumsPC::EInviteItem )
        {
        User::Leave( KErrGeneral );
        }

    // Leave with KErrNotFound if there are no items.
    if ( index == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }

    MCAInvitationPC& invitePC( *iAppUi->GetProcessManager().GetInvitationsInterface() );

    TCADnlInvView dnlInvView;
    dnlInvView.iInvitationIndex = invitePC.FindInvitationIndex(
                                      iMainViewArrayPC->GetInviteItemID( index ) );
    dnlInvView.iInviteID = iMainViewArrayPC->GetInviteItemID( index );
    dnlInvView.iSwitchTab = EFalse;
    TCADnlInvViewBuf invMsgBuf( dnlInvView );

    UpdateCbaL( ETrue );
    SetCbaLockL( ETrue );
    iViewSwitcher->SwitchViewL( KUidInvitationView, KUidInvViewMsgId, invMsgBuf  );

    }

// -----------------------------------------------------------------------------
// CCASingleListContainer::DeleteInvitationL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCASingleListContainer::DeleteInvitationL()
    {
    TInt index = CurrentListboxIndex();
    // just to make sure
    if ( iMainViewArrayPC->GetType( index ) != TEnumsPC::EInviteItem )
        {
        User::Leave( KErrGeneral );
        }

    // display the query
    HBufC* prompt = iEikonEnv->AllocReadResourceLC( R_CHAT_CONV_LIST_INV_DEL );
    TInt ret( IMDialogUtils::DisplayQueryDialogL( R_CLOSE_COVERSATION_QUERY,
                                                  *prompt ) );
    CleanupStack::PopAndDestroy( prompt );

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        TInt invitationIndex = iMainViewArrayPC->GetItemEngineIndex( index );
        TInt indexOfList = iMainViewArrayPC->IndexOfList( index );
        TInt countOfContactsInList = iMainViewArrayPC->CountOfContactsInList( indexOfList );
        TPtrC inviteID( iMainViewArrayPC->GetInviteItemID( index ) );

        iMainViewArrayPC->FindInvitationAndDelete( index/*invitationIndex*/ );
        //Deletion of a invitation requires the invitation to be deleted
        //form mainviewarraypc and invitationpc
        MCAInvitationPC& invitePC( *iAppUi->GetProcessManager().GetInvitationsInterface() );

        invitePC.DeleteSingleListViewInvitationL( inviteID /*invitationIndex*/ );


        //Plus One because one contact has been deleted	in the above
        //deleteFromNetwork() call
        if ( indexOfList + countOfContactsInList == index  )
            {
            iListBox->SetCurrentItemIndex( index - 1 );
            }
        iListBox->HandleItemRemovalL();
        UpdateFilterL();
        }
    }


// ---------------------------------------------------------
// CCASingleListContainer::CreateNewChatL()
// Creates new chat
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCASingleListContainer::CreateNewChatL()
    {
    // Ignore CBA update events so that user can't mess up the
    // group creation operation
    //iIgnoreCbaUpdateEvents = ETrue;
    TInt currentIndex( CurrentListboxIndex() );
    if ( currentIndex == KErrNotFound )
        {
        // listbox is empty
        return NULL;
        }
    HBufC* group = NULL;
    TEnumsPC::TListSelectionType selType( TEnumsPC::EMultiSelect );
    TBool isContactFound( EFalse );
    TBool inviteSupported( iAppUi->UISessionManager().IsSupported( CCAUISessionManager::EInvite ) );

    if ( inviteSupported )
        {

        TEnumsPC::TItem itemtype = iMainViewArrayPC->GetType( currentIndex );
        MCASettingsPC* settings = iAppUi->GetProcessManager().GetSettingsInterface();
        TBool isShowOffline = settings->GetBoolValuePC( TEnumsPC::EShowOffline, NULL );
        TBool isSendMsgOffline = IMUtils::IntResourceValueL(
                                     RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE_NOTIFY )
                                 || IMUtils::IntResourceValueL(
                                     RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE );

        TInt resValue = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING );
        TEnumsPC::TFilterType filter = TEnumsPC::EFilterNonOffline;

        if ( ( !isShowOffline || ( isShowOffline && !isSendMsgOffline ) ) )
            {
            // offline contacts are not shown
            filter = TEnumsPC::EFilterNonOffline;
            if ( resValue != 2 )
                {
                // offline and blocked contacts are not shown
                filter = TEnumsPC::EFilterNonOfflineNonBlockedAndOperation;
                }
            }
        else if ( isShowOffline && isSendMsgOffline  )
            {
            // online/offline/blocked contacts are shown
            filter = TEnumsPC::EFilterAll;
            if ( resValue != 2 )
                {
                //only blocked contacts are hidden
                filter = TEnumsPC::EFilterAllNonBlockedAndOperation;
                }
            }

        if ( itemtype == TEnumsPC::EContactListItem )
            {
            selType = TEnumsPC::ESingleListMultiSelect;
            TInt contactsCount =  iMainViewArrayPC->GetFilteredCount( filter , currentIndex );
            isContactFound = contactsCount ? ETrue : EFalse;

            }
        else
            {
            selType = TEnumsPC::EMultiSelect;
            TInt count( iMainViewArrayPC->Count() );

            for ( TInt index( 0 ); index < count && !isContactFound; index++ )
                {
                TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( index );
                if ( TEnumsPC::EContactListItem == itemType )
                    {
                    if ( iMainViewArrayPC->GetFilteredCount( filter , index ) )
                        {
                        isContactFound = ETrue;
                        }
                    }
                }
            }
        }

    SetSkipStorageEvents( ETrue );

    TRAP_IGNORE( group = iAppUi->GroupUtils()->CreateNewChatL(
                             R_CHATCLIENT_MAIN_VIEW_TITLE, KUidFriendsListView,
                             isContactFound, selType, currentIndex ) );
    CleanupStack::PushL( group );

    SetSkipStorageEvents( EFalse );

    iMainViewArrayPC->ResetArray( EFalse );
    iListBox->HandleItemAdditionL();
    TInt newIndex  = CurrentListboxIndex();
    TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( newIndex );
    UpdateViewL( currentIndex, itemType , ETrue );
    UpdateFilterL();

    //iIgnoreCbaUpdateEvents = EFalse;

    // We should be joining a group so no need to update cba
    CleanupStack::Pop();
    return group;
    }

// ---------------------------------------------------------
// CCASingleListContainer::JoinGroupL()
// Joins group
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::JoinGroupL( const TDesC& aGroupId )
    {
    CHAT_DP_FUNC_ENTER( "JoinGroupL()" );
    //TInt index( CurrentListboxIndex() );
    //TPtrC userid = iMainViewArrayPC->GetSelectedContactUserId( index );

    //Errors are handled in group utils
    iAppUi->GroupUtils()->JoinGroupL( EFalse, aGroupId );
    }


// ---------------------------------------------------------
// CCASingleListContainer::IsJoinedToGroup()
// Checks if already joined to group
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASingleListContainer::IsJoinedToGroup(  )
    {

    TInt index( CurrentListboxIndex() );
    TPtrC groupid = iMainViewArrayPC->GetSelectedContactUserId( index );

    //Errors are handled in group utils
    return iAppUi->GroupUtils()->IsJoinedToGroup( groupid );
    }

// ---------------------------------------------------------
// CCASingleListContainer::IsFavourite()
// Checks if indexed group is favourite
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASingleListContainer::IsFavourite( )
    {

    TInt index( CurrentListboxIndex() );
    TPtrC groupId = iMainViewArrayPC->GetSelectedContactUserId( index );

    //Errors are handled in group utils
    return iAppUi->GroupUtils()->IsFavouriteChatGroup( groupId );
    }

// ---------------------------------------------------------
// CCASingleListContainer::SaveAsFavouriteL()
// Saves as favourite
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::SaveAsFavouriteL()
    {
    TInt index( CurrentListboxIndex() );
    TPtrC groupid = iMainViewArrayPC->GetSelectedContactUserId( index );

    iAppUi->GroupUtils()->SaveAsFavouriteL( groupid );
    UpdateFilterL();
    }

// ---------------------------------------------------------
// CCASingleListContainer::DeleteGroupL()
// Deletes group
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DeleteGroupL( TPtrC aGroupId )
    {

    //TInt index = CurrentListboxIndex();
    //TPtrC selectedGroupId = iMainViewArrayPC->GetSelectedContactUserId(index);

    TInt ret = iAppUi->GroupUtils()->DeleteGroupL( aGroupId );
    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        UpdateCbaL();
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::LeaveGroupL()
// Leaves from group
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::LeaveGroupL()
    {
    TInt index = CurrentListboxIndex();

    //once the group is deleted the userId returned will be deleted
    //hence we store in a local buffer so that this can be used
    //for later operations
    TBuf<2 * KMaxWVIDLength> groupId;

    groupId.Copy( iMainViewArrayPC->GetSelectedContactUserId( index ).Left(
                      groupId.MaxLength() ) );

    TInt err( KErrNone );

    TInt indexOfList = iMainViewArrayPC->IndexOfList( index );
    TInt countOfContactsInList = iMainViewArrayPC->CountOfContactsInList( indexOfList );

    TRAPD( leave, err = iAppUi->GroupUtils()->LeaveGroupL( groupId ) );


    if ( err != KErrNone || leave != KErrNone )
        {
        IMNoteMapper::ShowNoteL( err );
        }

    // If the group is not owned or saved and it is the last item in the
    // openchats folder.Then move the focus to openchatsfolder.
    if ( indexOfList + countOfContactsInList  == index &&
         !iAppUi->GroupUtils()->IsOwnGroup( groupId ) &&
         !iAppUi->GroupUtils()->IsFavouriteChatGroup( groupId ) )
        {
        iListBox->SetCurrentItemIndex( index - 1 );
        }
    // Deregister group from switch back views
    iAppUi->DeRegisterSwitchBack( KUidChatView, groupId );
    UpdateCbaL();

    }


// ---------------------------------------------------------
// CCASingleListContainer::DisplayJoinedMembersL()
// Displays joined members
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayJoinedMembersL()
    {

    TInt index( CurrentListboxIndex() );
    TPtrC groupId = iMainViewArrayPC->GetSelectedContactUserId( index );
    iAppUi->GroupUtils()->DisplayJoinedMembersL( groupId );
    UpdateFilterL();
    }

// ---------------------------------------------------------
// CCASingleListContainer::EditChatGroupPropertiesL()
// Launches chat group properties dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::EditChatGroupPropertiesL()
    {
    TInt index( CurrentListboxIndex() );
    TPtrC groupId = iMainViewArrayPC->GetSelectedContactUserId( index );
    iAppUi->GroupUtils()->EditChatGroupPropertiesL( groupId );

    UpdateFilterL();
    }



// ---------------------------------------------------------
// CCASingleListContainer::DisplayChatInfoL()
// Displays chat info
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayChatInfoL()
    {
    TInt index( CurrentListboxIndex() );
    TPtrC groupId = iMainViewArrayPC->GetSelectedContactUserId( index );

    iAppUi->GroupUtils()->DisplayChatInfoL( groupId );
    UpdateFilterL();
    }


// ---------------------------------------------------------
// CCASingleListContainer::IsUserAdmin()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASingleListContainer::IsUserAdmin() const
    {
    TInt index( CurrentListboxIndex() );
    TPtrC groupid = iMainViewArrayPC->GetSelectedContactUserId( index );

    return ( iAppUi->GroupUtils()->IsAdmin( groupid ) ||
             iAppUi->GroupUtils()->IsOwnGroup( groupid ) );
    }
// ---------------------------------------------------------
// CCASingleListContainer::IsUserAdmin()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASingleListContainer::IsOwnGroup() const
    {
    TInt index( CurrentListboxIndex() );
    TPtrC groupId = iMainViewArrayPC->GetSelectedContactUserId( index );

    return 	iAppUi->GroupUtils()->IsOwnGroup( groupId );
    }

// ---------------------------------------------------------
// CCASingleListContainer::DisplayRecipientTextQueryForGrpIDL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayRecipientTextQueryForGrpIDL( TDes& aDataPtr )
    {
    // Show domain selection query, if variated so
    IMDialogUtils::TInitialSelectionMode selectionMode =
        IMDialogUtils::ESelectAll;

    TBool domainSelection =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_DOMAIN_SELECTION );

    if ( domainSelection )
        {
        // Fill the user name with domain, if user selected it
        TInt retVal =
            iAppUi->UISessionManager().DisplayDomainSelectionQueryL( aDataPtr );
        if ( retVal == 0 )
            {
            // User cancelled the procedure
            //	CleanupStack::PopAndDestroy( data );
            return;
            }
        else
            {
            selectionMode = IMDialogUtils::ESelectNone;
            }
        }

    TBool textualInputMode = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_USERID_TEXTUAL_INPUTMODE );
    TInt result( IMDialogUtils::DisplayTextQueryDialogL( aDataPtr,
                                                         R_CHATCLIENT_CHATLIST_ENTER_CHATGROUP,
                                                         textualInputMode ?
                                                         R_CHATCLIENT_SEARCH_WVID_QUERY :
                                                         R_CHATCLIENT_SEARCH_WVID_QUERY2 ,
                                                         ETrue, // T9
                                                         EFalse, // LSK visibility, visible when needed
                                                         IMDialogUtils::EDefault,   // Left soft key
                                                         selectionMode,
                                                         domainSelection
                                                       ) );

    if ( result == EAknSoftkeyOk || result == EAknSoftkeyDone )
        {
        TInt errorCode = iAppUi->GroupUtils()->JoinGroupL( EFalse, aDataPtr, KNullDesC, NULL, CCAGroupUtils::EManualId );
        if ( errorCode != KErrNone )
            {
            DisplayRecipientTextQueryForGrpIDL( aDataPtr );
            }
        }

    }

// ---------------------------------------------------------
// CCASingleListContainer::DisplayRecipientTextQueryL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayRecipientTextQueryL()
    {


    HBufC* data = HBufC::NewLC( KMaxWVIDLength );
    TPtr dataPtr( data->Des() );

    // Show domain selection query, if variated so
    IMDialogUtils::TInitialSelectionMode selectionMode =
        IMDialogUtils::ESelectAll;

    TBool domainSelection =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_DOMAIN_SELECTION );

    if ( domainSelection )
        {
        // Fill the user name with domain, if user selected it
        TInt retVal =
            iAppUi->UISessionManager().DisplayDomainSelectionQueryL( dataPtr );
        if ( retVal == 0 )
            {
            // User cancelled the procedure
            CleanupStack::PopAndDestroy( data );
            return;
            }
        else
            {
            selectionMode = IMDialogUtils::ESelectNone;
            }
        }

    TBool textualInputMode = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_USERID_TEXTUAL_INPUTMODE );
    TInt result( IMDialogUtils::DisplayTextQueryDialogL( dataPtr,
                                                         R_CHATCLIENT_CONVLIST_ENTER_RECIP,
                                                         textualInputMode ?
                                                         R_CHATCLIENT_SEARCH_WVID_QUERY :
                                                         R_CHATCLIENT_SEARCH_WVID_QUERY2 ,
                                                         ETrue, // T9
                                                         EFalse, // LSK visibility, visible when needed
                                                         IMDialogUtils::EDefault,   // Left soft key
                                                         selectionMode,
                                                         domainSelection
                                                       ) );

    if ( result == EAknSoftkeyOk || result == EAknSoftkeyDone )

        {
        MCAConversationPC* ConversationPC = iAppUi->GetProcessManager().GetConversationInterface();

        TRAPD( error, ConversationPC->SetMessageReadInterfaceL( *data ) );


        if ( error != KErrNone )
            {
            HandleError( error );
            }
        else
            {
            //Add the conversation item to open chats array before switching the view
            iMainViewArrayPC->InsertConversationItemL( *data, KNullDesC );

            SwitchToConvViewL( *data );
            }
        }
    else if ( result == EAknSoftkeyExit )
        {
        // Editor cleared by user
        DisplayRecipientTextQueryL();
        }

    CleanupStack::PopAndDestroy( data );


    }


// ---------------------------------------------------------
// CCASingleListContainer::SwitchToConvViewL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::SwitchToConvViewL(
    const TDesC& aGroupId /*=KNullDesC*/ )
    {
    TCADnlConvView dnlConView;
    dnlConView.iIsForwarded = EFalse;
    dnlConView.iSAPChanged = EFalse;
    dnlConView.iSwitchTab = EFalse;
    if ( aGroupId.Length() > 0 )
        {
        //Store last known wvid
        //iLastKnownWVID->Des() = aGroupId;
        dnlConView.iWVID = aGroupId;
        TCADnlConvViewBuf convMsgBuf( dnlConView );
        iViewSwitcher->SwitchViewL( KUidConversationsView, KUidConvViewMsgId,
                                    convMsgBuf );
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::DisplayRecipientListQueryL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListContainer::DisplayRecipientListQueryL()
    {
    TInt itemIndex  = CurrentListboxIndex();
    TEnumsPC::TItem itemIndexType = iMainViewArrayPC->GetType( itemIndex );
    //check if there is any contacts - If any only then call
    //contact selection dialog.
    TBool contactfound = EFalse;
    TInt count( iMainViewArrayPC->Count() );

    for ( TInt index( 0 ); index < count; index++ )
        {
        TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( index );
        if ( TEnumsPC::EContactListItem == itemType )
            {
            if ( iMainViewArrayPC->CountOfContactsInList( index ) > 0 )
                {
                contactfound = ETrue;
                break;
                }
            }
        }

    if ( !contactfound )
        {
        IMDialogUtils::DisplayInformationNoteL(
            R_QTN_CHAT_NO_CONTACTS_AVAILABLE );
        return;
        }


    MCASkinVariant* skinVar = static_cast<CCAApp*>( iAppUi->Application() )
                              ->VariantFactory()->SkinVariantL();

    CDesCArray* selectedContacts = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selectedContacts );

    CDesCArray* selContactsIdentification = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selContactsIdentification );

    SetSkipStorageEvents( ETrue );

    // show selection dialog
    if ( !CCAContactSelectionDialog::ShowDialogL( *selectedContacts,
                                                  *iAppUi->GetProcessManager().GetArrayInterface(),
                                                  *skinVar,
                                                  *iAppUi->GetProcessManager().GetSettingsInterface(),
                                                  iAppUi->MbmFullPath(),
                                                  TEnumsPC::EMultiSelect,
                                                  R_CONTACT_SELECTION_DIALOG,
                                                  *iViewSwitcher->CAStatusPane(),
                                                  selContactsIdentification ) )
        {
        // no contacts
        CleanupStack::PopAndDestroy( 2, selectedContacts ); // selectedContacts,selContactsIdentification

        SetSkipStorageEvents( EFalse );

        iMainViewArrayPC->ResetArray( EFalse );
        iListBox->HandleItemAdditionL();
        UpdateViewL( itemIndex, itemIndexType , ETrue );
        UpdateFilterL();

        return;
        }

    SetSkipStorageEvents( EFalse );
    iMainViewArrayPC->ResetArray( EFalse );
    iListBox->HandleItemAdditionL();
    UpdateViewL( itemIndex, itemIndexType , ETrue );
    UpdateFilterL();


    TInt index  = CurrentListboxIndex();
    TEnumsPC::TItem itemType = iMainViewArrayPC->GetType( index );
    if ( selectedContacts->MdcaCount() == 1 )
        {
        TPtrC wvid( selectedContacts->MdcaPoint( 0 ) );
        //Add the conversation item to open chats array before switching the view
        iMainViewArrayPC->InsertConversationItemL( wvid, selContactsIdentification->MdcaPoint( 0 ) );

        SwitchToConvViewL( wvid );

        }
    else if ( selectedContacts->MdcaCount() > 1 )
        {
        // get the message to be sent to the user
        TInt msgLength = IMUtils::MaxMsgLength();
        HBufC* sendMessage = HBufC::NewLC( msgLength );
        TPtr sendMessagePtr( sendMessage->Des() );

        CAknQueryDialog* dlg = CAknQueryDialog::NewL( sendMessagePtr );
        dlg->PrepareLC( R_CHATCLIENT_SEND_TEXT_QUERY );
        dlg->SetPredictiveTextInputPermitted( ETrue );
        TInt result( dlg->RunLD() );

        if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes ) )
            {
            MCAConversationPC* ConversationPC = iAppUi->GetProcessManager().GetConversationInterface();
            ConversationPC->SetMessageWriteInterfaceL();
            for ( TInt i = 0; i < selectedContacts->MdcaCount(); i++ )
                {
                TPtrC wvid( selectedContacts->MdcaPoint( i ) );
                CHAT_DP( D_CHAT_LIT( "starting conversation with %S" ), &wvid );

                ConversationPC->SetMessageReadInterfaceL( wvid );

                //Add the conversation item to open chats array before switching the view
                iMainViewArrayPC->InsertConversationItemL( wvid, selContactsIdentification->MdcaPoint( i ) );


                ConversationPC->SendMessageL( sendMessagePtr ) ;


                }
            // after creating multiple conversation
            // focus the 1st open conversation in chat folder
            TBool collapsed( iMainViewArrayPC->IsCollapsed( index ) );

            if ( itemType == TEnumsPC::EOpenChatsListItem )
                {
                // focus is on open chat folder, now focus 1st conversation item
                if ( collapsed )
                    {
                    // not expanded ,expand now
                    ExpandCollapseListL();
                    }
                iListBox->SetCurrentItemIndex( index + 1 );
                iListBox->DrawNow();
                }
            else if ( itemType == TEnumsPC::EOwnStatusItem )
                {
                // focus is on own data item, now focus 1st conversation item
                // set index to open chat folder
                iListBox->SetCurrentItemIndex( index + 1 );
                TInt curIndex = iListBox->CurrentItemIndex();
                //check open chat folder is expanded or not
                collapsed = iMainViewArrayPC->IsCollapsed( curIndex );
                if ( collapsed )
                    {
                    // not expanded ,expand now
                    ExpandCollapseListL();
                    }
                // expanded ,focus 1st item in open chat folder
                iListBox->SetCurrentItemIndex( curIndex + 1 );
                iListBox->DrawNow();
                }
            }
        CleanupStack::PopAndDestroy( sendMessage );
        }

    CleanupStack::PopAndDestroy( 2, selectedContacts ); // selectedContacts,selContactsIdentification

    }

// ---------------------------------------------------------
// CCASingleListContainer::HandleServerChangeL
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCASingleListContainer::HandleServerChangeL ( TServerNotify aKey )
    {
    if ( aKey == MCAServerChangeNotify::ENoServer )
        {
        HBufC* emptyTxtBuf = IMUtils::CombineStringFromResourceLC(
                                 R_QTN_CHAT_EMPTY_COMMUNITY_VIEW_PRIMARY,
                                 R_QTN_CHAT_EMPTY_COMMUNITY_VIEW );
        ListBox()->View()->SetListEmptyTextL( *emptyTxtBuf );
        CleanupStack::PopAndDestroy( emptyTxtBuf );

        UpdateCbaL();
        }

    if ( aKey == MCAServerChangeNotify::EOtherChange )
        {
        HBufC* emptyTxtBuf = IMUtils::CombineStringFromResourceLC(
                                 R_QTN_CHAT_EMPTY_COMMUNITY_VIEW_NOT_VISIBLE_PRIMARY,
                                 R_QTN_CHAT_EMPTY_COMMUNITY_VIEW_NOT_VISIBLE );
        ListBox()->View()->SetListEmptyTextL( *emptyTxtBuf );
        CleanupStack::PopAndDestroy( emptyTxtBuf );

        if ( iFindbox && iListBox->Model()->NumberOfItems() <= 0 )
            {
            DeactivateFindPaneL();
            }
        SizeChanged();
        }
    }

// ---------------------------------------------------------
// CCASingleListContainer::IsBckGrdTaskComplete
// (other items were commented in a header).
// ---------------------------------------------------------
//

TBool CCASingleListContainer::IsBckGrdTaskComplete ( )
    {
    return iTaskComplete;
    }

// End of File
