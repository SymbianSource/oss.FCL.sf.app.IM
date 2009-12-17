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
* Description:  Multi/single contact selection dialog
*
*/


// INCLUDE FILES
#include    "CCAContactSelectionDialog.h"
#include    "CCAWaitNote.h"
#include    "IMDialogUtils.h"
#include    "IMUtils.h"
#include    "CCAStatusPaneHandler.h"
#include    "ccacontactlistbox.h"
#include    "ccacontactlistboxmodel.h"
#include    "catouchutils.h"

#include    "chatngclient.hrh"
#include    "ChatDefinitions.h"
#include    "ChatDebugPrint.h"
#include    "ChatDebugAssert.h"


#include    <chatNG.rsg>
#include	<aknsfld.h>
#include	<eikclbd.h>
#include    <eikcapc.h>
#include    <akniconarray.h>
#include    <aknnavi.h>
#include    <stringloader.h>
#include    <aknnavide.h>
#include    <avkon.hrh>

#include "MCAProcessManager.h"
#include "CCASingleListViewArray.h"
#include "MCAMainViewArrayPC.h"
#include "MCASettingsPC.h"
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// ==================== LOCAL FUNCTIONS ====================

class CFindControl : public CCoeControl
    {
    public:
        CAknSearchField *iFind;
        ~CFindControl() {
            delete iFind;
            }
        void Draw( const TRect & ) const
            {
            // this removes flicker from CEikDialog's Draw().
            }
        void SizeChanged()
            {
            TRect appRect( iAvkonAppUi->ApplicationRect() );
            TAknLayoutRect mainPane;
            TAknLayoutRect listRect;

            mainPane.LayoutRect( appRect,
                                 AKN_LAYOUT_WINDOW_main_pane( appRect, 0, 0, 1 ) );

            listRect.LayoutRect( mainPane.Rect(), AknLayout::list_gen_pane( 0 ) );

            TRect r( listRect.Rect() );
            iPosition = TPoint( r.iTl );
            iSize = TSize( r.Width(), r.Height() );
            }
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::CCAContactSelectionDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAContactSelectionDialog::CCAContactSelectionDialog(
    CDesCArray& aSelectedContacts,
    MCAMainViewArrayPC& aMainViewArrayPC,
    MCASettingsPC& aSettings,
    MCASkinVariant& aSkinVariant,
    TDesC& aMbmPath,
    TEnumsPC::TListSelectionType aSelectMultiple,
    CCAStatusPaneHandler& aStatusPaneHandler,
    CDesCArray* aSelectedIdentifications,
    TEnumsPC::TFilterType aFilter,
    TEnumsPC::TFilterType aDisplayFilter,
    TBool* aShutDialogFromFSW,
    TBool aBlockingSelection )
        : iSelectedContacts( aSelectedContacts ),
        iMainViewArrayPC( aMainViewArrayPC ),
        iSettings( aSettings ),
        iSkinVariant( aSkinVariant ), iMbmPath( aMbmPath ),
        iSelectMultiple( aSelectMultiple ),
        iFilter ( aFilter ),
        iDisplayFilter( aDisplayFilter ),
        iStatusPaneHandler( aStatusPaneHandler ),
        iSelectedIdentifications( aSelectedIdentifications ),
        iShutDialogFromFSW( aShutDialogFromFSW ),
        iBlockingSelection( aBlockingSelection )
    {
    /*
    if( iSelectMultiple )
        {
        iMainViewArrayPC.ShowAll( ETrue );
        }
        */

    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::
// Symbian 2nd phase constructor can leConstructLave.
// -----------------------------------------------------------------------------
//
TBool CCAContactSelectionDialog::ConstructL( TInt aSelectedListIndex )
    {
    CAknDialog::ConstructL( R_CONTACT_SELECTION_DIALOG_MENUBAR );

    // If aFilter was not defined as parameter for ShowDialogL
    // use settings and variation based filtering
    TBool isShowOffline = iSettings.GetBoolValuePC( TEnumsPC::EShowOffline, NULL );
    TBool isSendMsgOffline = IMUtils::IntResourceValueL(
                                 RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE_NOTIFY )
                             || IMUtils::IntResourceValueL(
                                 RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE );

    TInt resValue = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING );
    if ( iFilter == TEnumsPC::EFilterNotDefined )
        {
        if ( ( !isShowOffline || ( isShowOffline && !isSendMsgOffline ) ) )
            {
            // offline contacts are not shown
            iFilter = TEnumsPC::EFilterNonOffline;
            if ( resValue != 2 )
                {
                // offline and blocked contacts are not shown
                iFilter = TEnumsPC::EFilterNonOfflineNonBlockedAndOperation;
                }
            }
        else if ( isShowOffline && isSendMsgOffline  )
            {
            // offline/blocked contacts are shown
            iFilter = TEnumsPC::EFilterAll;
            if ( resValue != 2 )
                {
                //offline contacts are shown but blocked contacts are hidden
                iFilter = TEnumsPC::EFilterAllNonBlockedAndOperation;
                }
            }
        }
    if ( iSelectMultiple == TEnumsPC::ESingleListMultiSelect )
        {
        iSelectionCount = iMainViewArrayPC.GetFilteredCount(
                              iFilter , aSelectedListIndex );
        }
    iMainViewArrayPC.SetFilter( iFilter );
    iMainViewArrayPC.SetContactSelectionL( iSelectMultiple, aSelectedListIndex );
    if ( iMainViewArrayPC.ToUpdatePresence( iFilter, TEnumsPC::EFilterOnline ) )
        {
        // update presence if we're in manual mode and
        // showing only online contacts
        TBool autoUpdate( iSettings.GetBoolValuePC(
                              TEnumsPC::EAutomaticPresenceUpdate, NULL ) );
        if ( !autoUpdate )
            {
            CCAWaitNote* note = CCAWaitNote::ShowWaitNoteLC(
                                    R_QTN_CHAT_WAIT_REFRESH, ETrue );
            TRAPD( err, iMainViewArrayPC.RefreshFriendsL() );
            CHAT_DP( D_CHAT_LIT(
                         "**CAPresenceManager::RefreshFriendsL leave code %d" ), err );

            // Server sent us some non-OK error. Maybe we had some
            // bogus ID in the list.
            // We shouldn't show any dialog here, because information about the
            // non-successful event is shown by the confirmation query below.
            CleanupStack::PopAndDestroy( note );
            }
        }

    // if we have no friends at all,
    // then show note and leave the construction
    TBool contactfound( EFalse );

    //To Reset the array(Excludes openchats and owndata)
    iMainViewArrayPC.ResetArray( EFalse );

    TInt count( iMainViewArrayPC.Count() );

    for ( TInt index( 0 ); index < count && !contactfound; index++ )
        {
        TEnumsPC::TItem itemType = iMainViewArrayPC.GetType( index );
        if ( TEnumsPC::EContactListItem == itemType )
            {
            if ( iMainViewArrayPC.CountOfContactsInList( index )	> 0 )
                {
                contactfound = ETrue;
                }
            }
        }

    if ( !contactfound )
        {
        IMDialogUtils::DisplayInformationNoteL(
            R_QTN_CHAT_NO_CONTACTS_AVAILABLE );
        // not success
        iDialogCreationSuccess = EFalse;
        return EFalse;
        }

    if (  iMainViewArrayPC.Count() == 0 )
        {
        // if we have no friends online,
        // then show note and leave the construction
        IMDialogUtils::DisplayInformationNoteL( R_CHAT_IBOX_FRIENDS_OFFLINE );
        // not success
        iDialogCreationSuccess = EFalse;
        return EFalse;
        }

    // get pointer to status-pane
    CEikStatusPane* statusPane =
        CEikonEnv::Static()->AppUiFactory()->StatusPane();

    // get pointer to navi-pane
    iNaviPane = static_cast<CAknNavigationControlContainer*>
                ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    // push default decorator object (empty one) to top of the stack and draw it
    iNaviPane->PushDefaultL( ETrue );
    iNaviPaneCleared = ETrue;

    if ( iSelectMultiple == TEnumsPC::EMultiSelect
         || iSelectMultiple == TEnumsPC::ESingleSelect
         || iSelectMultiple == TEnumsPC::ESingleListMultiSelect  )
        {
        SetNaviLabelL();
        }

    // get a copy of old title and set new one
    iOldTitle = iStatusPaneHandler.Title()->AllocL();
    HBufC* title = NULL;
    if ( iSelectMultiple == TEnumsPC::ESingleListMultiSelect )
        {
        title = CCoeEnv::Static()->AllocReadResourceLC(
                    R_QTN_CHAT_CONTACTS_OPEN_GROUP_PROMPT );
        }
    else
        {
        title = CCoeEnv::Static()->AllocReadResourceLC(
                    R_CHATCLIENT_FRIENDSLIST_VIEW_TITLE );
        }

    iStatusPaneHandler.SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );
    iAppUi = static_cast< CCAAppUi* >( CCoeEnv::Static()->AppUi() );
    if ( iSelectMultiple == TEnumsPC::EMultiSelect )
        {
        iMainViewArrayPC.ClearSelectedL();
        }

    iMainViewArrayPC.AddObserverL( this );
    // dialog creation success
    iDialogCreationSuccess = ETrue;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TBool CCAContactSelectionDialog::ShowDialogL(
    CDesCArray& aSelectedContacts,
    MCAMainViewArrayPC& aMainViewArrayPC,
    MCASkinVariant& aSkinVariant,
    MCASettingsPC& aSettings,
    TDesC& aMbmPath,
    TEnumsPC::TListSelectionType aSelectMultiple,
    TInt aResourceId,
    CCAStatusPaneHandler& aStatusPaneHandler,
    CDesCArray* aSelectedIdentifications,
    TEnumsPC::TFilterType aFilter,
    TEnumsPC::TFilterType aDisplayFilter,
    TBool aBlockingSelection /*= EFalse*/,
    TInt aSelectedListIndex,
    TBool* aShutDialogFromFSW /*= NULL*/ )
    {
    CCAContactSelectionDialog* self = new( ELeave ) CCAContactSelectionDialog(
        aSelectedContacts,
        aMainViewArrayPC,
        aSettings,
        aSkinVariant,
        aMbmPath,
        aSelectMultiple,
        aStatusPaneHandler,
        aSelectedIdentifications,
        aFilter,
        aDisplayFilter,
        aShutDialogFromFSW,
        aBlockingSelection );
    CleanupStack::PushL( self );
    TBool success( self->ConstructL( aSelectedListIndex ) );

    if ( success )
        {
        CleanupStack::Pop( self );
        TInt retVal( self->ExecuteLD( aResourceId ) );
        if ( retVal != EAknSoftkeyOk &&
             retVal != EAknSoftkeyYes &&
             retVal != EAknSoftkeySelect )
            {
            //dialog has been canceled
            success = EFalse;
            }
        }
    else
        {

        //The CCAContactSelectionDialog component in the destructor phase
        //makes a call to iMainViewArrayPC.ResetContactSelectionL()
        //which in turm makes a call to PECEngine through
        //which the CleanupStack gets corrupted.

        //To resolve this issue, dont call d'tor that inturn makes
        //a call to some API's which pushes and pop's the variables to/from
        //the CleanupStack.
        CleanupStack::Pop( );

        delete self;
        }
    return success;
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::~CCAContactSelectionDialog
// -----------------------------------------------------------------------------
//
CCAContactSelectionDialog::~CCAContactSelectionDialog()
    {
    if ( iDialogCreationSuccess && iAppUi )
        {
        iAppUi->RemoveFromStack( this );
        }
    TRAP_IGNORE( iMainViewArrayPC.ResetContactSelectionL() );
    // allow tabs
    iStatusPaneHandler.ProhibitTabGroup( EFalse );
    if ( iNaviPanePushed )
        {
        // remember to pop
        iNaviPane->Pop();
        }

    if ( iNaviPaneCleared )
        {
        // pop the clean navipane to reveal the tabs
        iNaviPane->Pop();
        }

    delete iNaviPaneText;

    if ( iOldTitle )
        {
        TRAPD( err, iStatusPaneHandler.SetTitleL( *iOldTitle ) );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        delete iOldTitle;
        }

    iMainViewArrayPC.RemoveObserver( this );

    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                     TListBoxEvent aEventType )
    {

    if ( aEventType != EEventEnterKeyPressed &&
         aEventType != EEventItemClicked &&
         aEventType != EEventItemDoubleClicked )
        {
        // no handling for other events
        return;
        }

    TInt currentItemIndex( ListBox()->CurrentItemIndex() );
    TInt origIndex( static_cast<CCAContactListBoxModel*>
                    ( ListBox()->Model() )->FilteredItemIndex( currentItemIndex ) );

    TEnumsPC::TItem itemType = iMainViewArrayPC.GetType( origIndex );

    switch ( itemType )
        {
        case TEnumsPC::EContactListItem:
            {
            if ( aEventType == EEventItemDoubleClicked )
                {
                // ignore double click on list
                return;
                }
            CCAContactListBoxModel* model =
                static_cast<CCAContactListBoxModel*>( ListBox()->Model() );
            TInt items = model->NumberOfItems();


            TInt focusedIndex = iMainViewArrayPC.IndexOfList( origIndex, EFalse, ETrue );
            if ( focusedIndex > ( items - 1 ) )
                {
                focusedIndex = items - 1;
                }



            TBool collapsed( iMainViewArrayPC.IsCollapsed( origIndex ) );
            iMainViewArrayPC.SetCollapsed( !collapsed, origIndex );

            CAknListBoxFilterItems* filter =
                static_cast<CAknListBoxFilterItems*>( model->Filter() );
            filter->HandleItemArrayChangeL();

            ResetFindBoxL();
            UpdateFilterL();
            if ( collapsed )
                {
                ListBox()->HandleItemAdditionL();

                // Shift group upwards so that as much
                // contacts as possible is shown
                ListBox()->ScrollToMakeItemVisible(
                    currentItemIndex +
                    iMainViewArrayPC.GetFilteredCount( iFilter, origIndex ) );

                }
            else
                {
                ListBox()->HandleItemRemovalL();
                TInt items1 = model->NumberOfItems();
                // collapsed list name is shown as up on the display as possible
                ListBox()->ScrollToMakeItemVisible( items1 - 1 );
                // Find index of list
                }

            ListBox()->SetCurrentItemIndex( focusedIndex );
            break;
            }
        case TEnumsPC::EContactItem:
            {
            if ( iSelectMultiple == TEnumsPC::ESingleSelect )
                {
                // Mark that we're exiting the dialog
                iExiting = ETrue;
                if ( aEventType != EEventItemClicked &&
                     aEventType != EEventItemDoubleClicked )
                    {
                    // Don't exit here with taps. Framework command chain will
                    // cause a KERN-EXEC 3 panic. Handle the exit in
                    // HandleDialogPageEventL().
                    TryExitL( EAknSoftkeyOk );
                    }
                return;
                }
            else if ( aEventType != EEventItemDoubleClicked )
                {
                TBool selected( iMainViewArrayPC.Selected( origIndex ) );
                if ( !selected )
                    {
                    // Don't show offline status notes if selecting
                    // contacts for blocking

                    if ( !iBlockingSelection && iMainViewArrayPC.IgnoreOfflineStatusL ( origIndex ) != TEnumsPC::EOnline  )
                        {
                        // Contact was not marked and contact is offline and user
                        // choose not to ignore offline status => don't
                        // mark contact
                        TPtrC contactname = iMainViewArrayPC.GetItemNameText( origIndex );
                        if ( IMUtils::IgnoreOfflineStatusL( contactname ) < 0 )
                            {
                            break;
                            }
                        //break;
                        }

                    }

                iMainViewArrayPC.SetSelected( !selected, origIndex );

                // increase counter
                selected ? --iSelectionCount : ++iSelectionCount;

                SetNaviLabelL();
                }
            break;
            }
        case TEnumsPC::EAll:
            {
            TBool selected( iMainViewArrayPC.Selected( origIndex ) );
            iMainViewArrayPC.SetSelected( !selected, origIndex );

            // increase counter, this is used just for hiding sofkeys
            // so it's ok to increase/decrease it by one
            selected ? --iSelectionCount : ++iSelectionCount;
            SetNaviLabelL();
            break;
            }
        default:
            {
            // shouldn't be here
            __CHAT_ASSERT_DEBUG( EFalse );
            }
        }
    UpdateCbaL();
    ListBox()->DrawNow();
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::HandleControlEventL
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::HandleControlEventL( CCoeControl* aControl,
                                                     TCoeEvent aEventType )
    {
    TBool forceExpand = EFalse;
    if ( FindBox() && FindBox()->TextLength() > 0 )
        {
        // we have active search going on --> expand all contact lists
        forceExpand = ETrue;
        }
    iMainViewArrayPC.ForceExpanded( forceExpand );

    // Forward the message to filter
    ( ( CAknFilteredTextListBoxModel* )ListBox()->Model() )
    ->Filter()->HandleControlEventL( aControl, aEventType );

    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CCAContactSelectionDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aKeyEvent.iCode == EKeyEscape ) // handle dialog shutdown.
        {
        if ( iShutDialogFromFSW )
            {
            *iShutDialogFromFSW = ETrue;
            }

        TryExitL( EAknSoftkeyCancel );
        return EKeyWasConsumed;
        }

    TBool refresh( ETrue );
    TKeyResponse res( AknFind::HandleFindOfferKeyEventL( aKeyEvent, aType, this,
                                                         ListBox(), FindBox(), EFalse, refresh ) );

    if ( refresh && FindBox() )
        {
        AknFind::HandleFixedFindSizeChanged(
            this, ( CAknColumnListBox* )ListBox(), FindBox() );

        DrawNow();
        }

    if ( res == EKeyWasNotConsumed )
        {
        if ( aKeyEvent.iCode == EKeyOK && aType == EEventKey )
            {
            HandleListBoxEventL( ListBox(), EEventEnterKeyPressed );
            res = EKeyWasConsumed;
            }
        else
            {
            res = ListBox()->OfferKeyEventL( aKeyEvent, aType );
            }

        if ( res == EKeyWasConsumed && !iExiting )
            {
            // key was consumed by listbox, check the selection
            // status and change softkeys
            UpdateCbaL();
            }
        }

    return res;
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CCAContactSelectionDialog::CountComponentControls() const
    {
    TInt newitems = 0;
    if ( FindBox() )
        {
        newitems++;
        }
    return CAknDialog::CountComponentControls() + newitems;
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CCAContactSelectionDialog::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == CAknDialog::CountComponentControls() )
        {
        return FindBox();
        }
    return CAknDialog::ComponentControl( aIndex );
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::DynInitMenuPaneL( TInt /*aResourceId*/,
                                                  CEikMenuPane* /*aMenuPane*/ )
    {
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::PreLayoutDynInitL()
    {

    CAknDialog::PreLayoutDynInitL();

    CCAContactListBox* lb = ( CCAContactListBox* )ListBox();
    lb->SetContactListModel( iMainViewArrayPC );
    lb->SetListBoxObserver( this );
    lb->CreateScrollBarFrameL( ETrue );
    lb->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );


    // remove empty spaces from b-column (new Avkon-support)
    lb->UseEmptyIconSpace( ETrue );

    CCASingleListViewArray * singleListArray =
        CCASingleListViewArray::NewL(
            iMainViewArrayPC,
            iSelectMultiple,
            lb->ItemDrawer()->ColumnData(),
            ETrue,
            iSettings.GetBoolValuePC( TEnumsPC::EAutomaticPresenceUpdate, NULL ), *lb );


    // If aDisplayFilter was not defined as parameter for ShowDialogL
    // use iFilter based filtering
    if ( iDisplayFilter == TEnumsPC::EFilterNotDefined )
        {
        if ( iFilter == TEnumsPC::EFilterNonOfflineNonBlockedAndOperation )
            {
            iDisplayFilter = TEnumsPC::EFilterNonOfflineNonBlockedNonOfflineOperation;
            }
        else
            {
            iDisplayFilter = TEnumsPC::EFilterAllNonBlockedNonOfflineOperation;
            }
        }
    iMainViewArrayPC.SetSingleListArrayFilter( iDisplayFilter );
    singleListArray->SetContactListBoxModel(
        ( ( CCAContactListBoxModel* )lb->Model() ) );

    // ownership transferred to listbox
    lb->Model()->SetItemTextArray( singleListArray );
    lb->SetListBoxObserver( this );
    lb->CreateScrollBarFrameL( ETrue );
    lb->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto );

    CAknIconArray* icons =
        CCASingleListViewArray::LoadIconsLC( iMbmPath, iSkinVariant );
    CleanupStack::Pop( icons );
    lb->ItemDrawer()->ColumnData()->SetIconArray( icons );
    lb->UpdateScrollBarsL();

    // set marquee on
    lb->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    CFindControl* control = ( CFindControl* )ControlOrNull( EFindControl );

    CAknSearchField* searchField = CAknSearchField::NewL( *control, CAknSearchField::EFixed, NULL,
                                                          KTextLimit );
    control->iFind = searchField;

    static_cast<CAknFilteredTextListBoxModel*>( lb->Model() )
    ->CreateFilterL( lb, FindBox() );
    // capture searchfield events
    FindBox()->SetObserver( this );

    iAppUi->AddToStackL( this );
    iStatusPaneHandler.ProhibitTabGroup( ETrue );
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::PostLayoutDynInitL()
    {
    CAknDialog::PostLayoutDynInitL();
    UpdateCbaL();
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CCAContactSelectionDialog::OkToExitL( TInt aButtonId )
    {

    iSelectedContacts.Reset();
    if ( iSelectedIdentifications )
        {
        iSelectedIdentifications->Reset();
        }
    if ( aButtonId == EAknSoftkeyOk ||
         aButtonId == EAknSoftkeySelect )
        {
        if ( iSelectMultiple == TEnumsPC::EMultiSelect ||
             iSelectMultiple == TEnumsPC::ESingleListMultiSelect )//multiselection mode
            {
            if ( iSelectionCount == 0 )
                {
                if ( aButtonId == EAknSoftkeyOk )
                    {
                    // check if focused item is a contact
                    TInt index( CurrentIndex() );
                    if ( index == KErrNotFound )
                        {
                        // filtered listbox empty
                        // => prevent exit
                        return EFalse;
                        }
                    TEnumsPC::TItem item = iMainViewArrayPC.GetType( index );
                    if ( item != TEnumsPC::EContactItem )
                        {
                        // multiselection mode and focused item is not contact
                        // item => prevent exit
                        return EFalse;
                        }
                    // Don't show offline status notes if selecting
                    // contacts for blocking
                    if ( !iBlockingSelection &&  iMainViewArrayPC.IgnoreOfflineStatusL( index ) != TEnumsPC::EOnline )
                        {
                        // multiselection mode and user choose not to
                        // ignore offline status => prevent exit
                        TPtrC contactname = iMainViewArrayPC.GetItemNameText( index );
                        if ( IMUtils::IgnoreOfflineStatusL( contactname ) < 0 )
                            {
                            return EFalse;
                            }

                        }
                    iSelectedContacts.AppendL( iMainViewArrayPC.GetSelectedContactUserId( index ) );

                    if ( iSelectedIdentifications )
                        {
                        iSelectedIdentifications->AppendL(  iMainViewArrayPC.GetSelectedContactIdentification( index ) );
                        }
                    }
                else
                    {
                    // multiselection mode, nothing selected
                    // and EAknSoftkeySelect => prevent exit
                    return EFalse;
                    }
                }

            iMainViewArrayPC.GetSelectedL( iSelectedContacts,
                                           iSelectedIdentifications,
                                           iFilter );
            return ETrue;
            }
        else if ( iSelectMultiple == TEnumsPC::ESingleSelect )
            {
            TInt index( CurrentIndex() );
            if ( index == KErrNotFound )
                {
                // filtered listbox empty
                // => prevent exit
                return EFalse;
                }
            TEnumsPC::TItem item = iMainViewArrayPC.GetType( index );
            if ( item != TEnumsPC::EContactItem )
                {
                // singleselection mode and focused item is not contact item
                // => prevent exit
                return EFalse;
                }
            // Don't show offline status notes if selecting
            // contacts for blocking
            if ( !iBlockingSelection &&  iMainViewArrayPC.IgnoreOfflineStatusL( index ) != TEnumsPC::EOnline )
                {
                // multiselection mode and user choose not to
                // ignore offline status => prevent exit
                TPtrC contactname = iMainViewArrayPC.GetItemNameText( index );
                if ( IMUtils::IgnoreOfflineStatusL( contactname ) < 0 )
                    {
                    return EFalse;
                    }
                // return EFalse;
                }

            iSelectedContacts.AppendL( iMainViewArrayPC.
                                       GetSelectedContactUserId( index ) );
            if ( iSelectedIdentifications )
                {
                iSelectedIdentifications->AppendL(  iMainViewArrayPC.GetSelectedContactIdentification( index ) );
                }
            }
        return ETrue;
        }

    if ( aButtonId == EChatClientCmdCollapseList ||
         aButtonId == EChatClientCmdExpandList ||
         aButtonId == EAknSoftkeyMark ||
         aButtonId == EAknSoftkeyUnmark )
        {
        // Don't exit with these commands, process them
        ProcessCommandL( aButtonId );
        return EFalse;
        }

    // ok key was not pressed.
    return EFalse;

    }

// ---------------------------------------------------------
// CCAContactSelectionDialog::CurrentIndex()
// ---------------------------------------------------------
//
TInt CCAContactSelectionDialog::CurrentIndex()
    {
    TInt current( ListBox()->CurrentItemIndex() );
    if ( current != KErrNotFound )
        {
        current = static_cast<CCAContactListBoxModel*>
                  ( ListBox()->Model() )->FilteredItemIndex( current );
        }
    return current;
    }


// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::SetNaviLabelL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::SetNaviLabelL()
    {

    HBufC* newTitle =  newTitle = StringLoader::LoadLC(
                                      R_QTN_CHAT_MULTISELECTION_AMOUNT,
                                      iSelectionCount );

    TPtr titlePtr( newTitle->Des() );

    CAknNavigationDecorator* tmpNaviPaneText =
        iNaviPane->CreateMessageLabelL( titlePtr );
    CleanupStack::PushL( tmpNaviPaneText );
    if ( iNaviPanePushed )
        {
        iNaviPane->Pop(); // pop old one out.
        }
    iNaviPane->PushL( *tmpNaviPaneText );
    iNaviPanePushed = ETrue;
    CleanupStack::Pop( tmpNaviPaneText );
    delete iNaviPaneText;
    iNaviPaneText = tmpNaviPaneText;

    CleanupStack::PopAndDestroy( newTitle );
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::ProcessCommandL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::ProcessCommandL( TInt aCommandId )
    {

    CAknDialog::ProcessCommandL( aCommandId );

    switch ( aCommandId )
        {
        case EChatClientCmdCollapseList :
        case EChatClientCmdExpandList :
            {
            TInt currentItemIndex( ListBox()->CurrentItemIndex() );
            TInt origIndex( static_cast<CCAContactListBoxModel*>
                            ( ListBox()->Model() )->FilteredItemIndex( currentItemIndex ) );


            TEnumsPC::TItem itemType = iMainViewArrayPC.GetType( origIndex );

            if ( itemType == TEnumsPC::EContactListItem ||
                 itemType == TEnumsPC::EContactItem )
                {

                TInt curIndex( ListBox()->CurrentItemIndex() );

                TInt focusedIndex = iMainViewArrayPC.IndexOfList( curIndex, ETrue, ETrue );

                TBool collapsed( iMainViewArrayPC.IsCollapsed( origIndex ) );
                iMainViewArrayPC.SetCollapsed( !collapsed, origIndex );

                TInt items = KErrNotFound;

                ResetFindBoxL();
                UpdateFilterL();

                if ( collapsed )
                    {
                    // list was expanded => more items
                    ListBox()->HandleItemAdditionL();

                    items = ( ( CCAContactListBoxModel* )ListBox()->Model() )
                            ->NumberOfItems();

                    TInt filteredCount = iMainViewArrayPC.GetFilteredCount(
                                             ( TEnumsPC::TFilterType )iFilter, origIndex );
                    // Shift group upwards so that as much
                    // contacts as possible is shown
                    ListBox()->ScrollToMakeItemVisible( curIndex + filteredCount );
                    if ( focusedIndex > ( items - 1 ) )
                        {
                        focusedIndex = items - 1;
                        }
                    }
                else
                    {
                    // list was collapsed => less items
                    ListBox()->HandleItemRemovalL();

                    items = ( ( CCAContactListBoxModel* )ListBox()->Model() )
                            ->NumberOfItems();
                    // collapsed list name is shown as up on the display
                    // as possible
                    if ( focusedIndex > ( items - 1 ) )
                        {
                        focusedIndex = items - 1;
                        }
                    }

                ListBox()->SetCurrentItemIndexAndDraw( focusedIndex );

                UpdateCbaL();
                }
            break;
            }
        case EAknSoftkeyMark:   // Flowthrough, MSK commands
        case EAknSoftkeyUnmark:
            {
            // Use HandleListBoxEventL to react to these commands
            HandleListBoxEventL( ListBox(), EEventEnterKeyPressed );
            // UpdateCbaL is called in HandleListBoxEventL
            break;
            }
        case EEikCmdExit:	// falltrough
        case EChatClientCmdExit:
            {
            iAppUi->HandleCommandL( EAknSoftkeyExit );
            break;
            }

        default :
            {
            // pass through others
            iAppUi->HandleCommandL( aCommandId );
            break;
            }
        }

    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::Draw
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::Draw( const TRect& /*aRect*/ ) const
    {
    // Nothing to draw. Components do their own drawing.

    // Don't remove this method unless you want CAknDialog::Draw to
    // cause some flickering in the list while searching contacts.
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::SetSizeAndPosition
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::SetSizeAndPosition( const TSize& /*aSize*/ )
    {
    TAknLayoutRect lr;
    TRect applicationRect( iAvkonAppUi->ApplicationRect() );
    lr.LayoutRect( applicationRect, AKN_LAYOUT_WINDOW_main_pane( applicationRect, 0, 1, 1 ) );

    SetRect( lr.Rect() );

    if ( FindBox() )
        {
        FindBox()->MakeVisible( EFalse );
        SetLayout();
        //AknFind::HandleFixedFindSizeChanged( this, ( CAknColumnListBox* )ListBox(), FindBox() );
        FindBox()->MakeVisible( ETrue );
        }

    // 2nd setrect to get captioned controls rect same as list rect.
    // It looks like dialog's Reconsiderpagesize requires this now.
    Line( ESelectionListControl )->SetRect( ListBox()->Rect() );
    if ( FindBox() )
        {
        Line( EFindControl )->SetRect( ListBox()->Rect() );
        }
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::CreateCustomControlL
// -----------------------------------------------------------------------------
//
SEikControlInfo CCAContactSelectionDialog::CreateCustomControlL( TInt aControlType )
    {
    CCoeControl* control = NULL;
    if ( aControlType == EAknCtSelectionListFixedFind )
        {
        control = new( ELeave ) CFindControl;
        }
    if ( aControlType == EChatClientControlIdContactListbox )
        {
        control = new( ELeave ) CCAContactListBox;
        }
    SEikControlInfo info;
    info.iControl = control;
    info.iFlags = 0;
    info.iTrailerTextId = 0;
    return info;
    }

// ---------------------------------------------------------
// CCAContactSelectionDialog::HandleDialogPageEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactSelectionDialog::HandleDialogPageEventL( TInt aEventID )
    {
    CAknDialog::HandleDialogPageEventL( aEventID );
    if ( CATouchUtils::PenEnabled() )
        {
        if ( aEventID == MEikDialogPageObserver::EDialogPageTapped &&
             iExiting )
            {
            // Exit requested, exit with ok.
            TryExitL( EAknSoftkeyOk );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::ResetFindBoxL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::ResetFindBoxL()
    {

    iMainViewArrayPC.ForceExpanded( EFalse );
    if ( FindBox() )
        {
        FindBox()->ResetL();
        FindBox()->SetSearchTextL( KNullDesC );
        FindBox()->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::UpdateFilterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::UpdateFilterL()
    {
    if ( FindBox() )
        {
        // findpane is visible and contains something, notify it about
        // the changed array

        CCAContactListBoxModel* model =
            static_cast<CCAContactListBoxModel*>( ListBox()->Model() );
        if ( model )
            {
            CAknListBoxFilterItems* filter =
                static_cast<CAknListBoxFilterItems*>( model->Filter() );

            // HandleItemArrayChangeL resets the listbox index,
            // work-around it
            TInt index( ListBox()->CurrentItemIndex() );
            TInt top( ListBox()->TopItemIndex() );

            if ( index == KErrNotFound )
                {
                // for some reason the listbox loses the current
                // index sometimes, breaking any calls to FilteredItemIndex.
                // draw it nevertheless. otherwise when erasing a character
                // from the filter causes nothing to be displayed.
                if ( ListBox() )
                    {
                    ListBox()->DrawNow();
                    }
                return;
                }

            if ( filter )
                {
                // this idiot resets the listbox index
                filter->HandleItemArrayChangeL();
                }

            TInt items = model->NumberOfItems();
            if ( items )
                {
                if ( index >= items )
                    {
                    index = items - 1;
                    }

                ListBox()->SetCurrentItemIndex( index );
                if ( top < index )
                    {
                    ListBox()->SetTopItemIndex( top );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::FindBox
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAknSearchField* CCAContactSelectionDialog::FindBox() const
    {
    CFindControl *control = ( CFindControl* )ControlOrNull( EFindControl );
    if ( control )
        {
        return control->iFind;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::ListBox
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CEikListBox* CCAContactSelectionDialog::ListBox() const
    {
    return ( CEikListBox* )Control( ESelectionListControl );
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::UpdateCbaL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::UpdateCbaL()
    {
    TInt cbaRes( R_AVKON_SOFTKEYS_CANCEL );

    TBool showOK = EFalse;       // SK1 is OK (ETrue) (not singlesel)
    TBool showCollapse = EFalse; // SK2 is Collapse (ETrue)
    TBool marked = EFalse;       // Is highlighted contact marked or not
    TBool collapsed = EFalse;    // Is list collapsed or expanded

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
    TEnumsPC::TSyncStatus syncState =
        TEnumsPC::ESyncNotDone;
#endif  // IMPS_CONTACT_FETCH_BACKGROUND

    if ( iSelectionCount > 0 )
        {
        showOK = ETrue;
        }

    TInt index = CurrentIndex();
    if ( index != KErrNotFound )
        {
        // something selected
        TEnumsPC::TItem item = iMainViewArrayPC.GetType( index );
        if ( item == TEnumsPC::EContactItem )
            {
            // allow collapse only when contact is focused
            showCollapse = ETrue;
            // Show OK when contact is focused
            showOK = ETrue;
            // Check if contact is marked
            marked = iMainViewArrayPC.Selected( index );
            }
        else if ( item == TEnumsPC::EContactListItem )
            {
            // Check if list is collapsed
            collapsed = iMainViewArrayPC.IsCollapsed( index );
            
            TEnumsPC::TItem item = iMainViewArrayPC.GetType( index +1 );
            if ( item == TEnumsPC::EContactItem )
                {
                collapsed = EFalse;
          	    }     

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
            syncState = iMainViewArrayPC.Synchronised( index );
#endif  // IMPS_CONTACT_FETCH_BACKGROUND
            }
        }

    if ( iSelectMultiple == TEnumsPC::EMultiSelect ||
         iSelectMultiple == TEnumsPC::ESingleListMultiSelect )
        {
        // multiselection mode
        // => ok can be shown if something selected or focus on contact
        // => show collapse if it's a contact, else show cancel

        if ( showOK && !showCollapse )
            {
            // Something selected, focus on contact list
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
            if ( syncState == TEnumsPC::ESyncNotDone ||
                 syncState == TEnumsPC::ESyncFailed )
                {
                cbaRes = R_CHAT_SOFTKEYS_OK_CANCEL__EMPTY;
                }
            else
#endif  // IMPS_CONTACT_FETCH_BACKGROUND
                if ( collapsed )
                    {
                    // OK/Cancel/Expand
                    cbaRes = R_CHAT_SOFTKEYS_OK_CANCEL__EXPAND;
                    }
                else
                    {
                    // OK/Cancel/Collapse

                    cbaRes = R_CHAT_SOFTKEYS_OK_CANCEL__COLLAPSE;

                    }
            }

        if ( showOK && showCollapse )
            {
            // Focus on contact
            if ( marked )
                {
                // OK/Collapse/Unmark

                cbaRes = R_CHAT_SOFTKEYS_OK_COLLAPSE__UNMARK;


                }
            else
                {
                // OK/Collapse/Mark

                cbaRes = R_CHAT_SOFTKEYS_OK_COLLAPSE__MARK;


                }
            }

        if ( !showOK && !showCollapse )
            {
            // No selection, focus on contact list
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
            if ( syncState == TEnumsPC::ESyncNotDone ||
                 syncState == TEnumsPC::ESyncFailed )
                {
                cbaRes = R_CHAT_SOFTKEYS_EMPTY_CANCEL__EMPTY;
                }
            else
#endif  // IMPS_CONTACT_FETCH_BACKGROUND
                if ( collapsed )
                    {
                    // Empty/Cancel/Expand

                    cbaRes = R_CHAT_SOFTKEYS_EMPTY_CANCEL__EXPAND;
                    }
                else
                    {
                    // Empty/Cancel/Collapse

                    cbaRes = R_CHAT_SOFTKEYS_EMPTY_CANCEL__COLLAPSE;


                    }
            }

        if ( !showOK && showCollapse )
            {
            // No selection, focus on contact
            if ( marked )
                {
                // Empty/Collapse/Unmark

                cbaRes = R_CHAT_SOFTKEYS_EMPTY_COLLAPSE__UNMARK;

                }
            else
                {
                // Empty/Collapse/Mark
                cbaRes = R_CHAT_SOFTKEYS_EMPTY_COLLAPSE__MARK;

                }
            }
        }
    else if ( iSelectMultiple == TEnumsPC::ESingleSelect )
        {
        // singleselection mode
        // => SK1 is select if focus is on a contact
        // => SK2 is collapse if contact is selected and list is open
        // => otherwise SK2 is cancel
        if ( !showOK && !showCollapse )
            {
            // Focus on contact list
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
            if ( syncState == TEnumsPC::ESyncNotDone ||
                 syncState == TEnumsPC::ESyncFailed )
                {
                cbaRes = R_CHAT_SOFTKEYS_EMPTY_CANCEL__EMPTY;
                }
            else
#endif  // IMPS_CONTACT_FETCH_BACKGROUND
                if ( collapsed )
                    {
                    // Empty/Cancel/Expand
                    cbaRes = R_CHAT_SOFTKEYS_EMPTY_CANCEL__EXPAND;
                    }
                else
                    {
                    // Empty/Cancel/Collapse
                    cbaRes = R_CHAT_SOFTKEYS_EMPTY_CANCEL__COLLAPSE;
                    }
            }
        else
            {
            // Focus on contact
            cbaRes = R_CHAT_SOFTKEYS_SELECT_COLLAPSE__SELECT;
            }
        }

    ButtonGroupContainer().SetCommandSetL( cbaRes );
    ButtonGroupContainer().DrawNow();

    }
// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::HandleDelete
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::HandleDelete( const TDesC& /*aContactId*/ , TEnumsPC::TItem /*type*/ )
    {

    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::HandleDelete
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::HandleWatcherEvent( const TDesC&  /*aString */, TEnumsPC::TOnlineStatus /*aOnlineStatus*/ )
    {

    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::HandleInvitationEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::HandleInvitationEventL( TEnumsPC::TInviteType /* aInviteType */,
                                                        const TDesC& /*aUserID */,
                                                        const TDesC& /* aMessage*/ )
    {

    }

// ---------------------------------------------------------
// CCAContactSelectionDialog::HandleInvitationResponse()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactSelectionDialog::HandleInvitationResponse( TBool /*aAcceptance*/,
                                                          const TDesC& /*aUserId*/,
                                                          const TDesC&  /*aGroupName*/,
                                                          const TDesC& /*aResponse*/ )
    {
    // nothing to do here since CCAppUi shows the dialog
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::HandleAddition
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactSelectionDialog::HandleAddition( TEnumsPC::TItem /* aType */, TInt aIndex )
    {

    iMainViewArrayPC.SetCollapsed( EFalse, aIndex );
    TRAPD( err, ListBox()->HandleItemAdditionL() );
    if (  err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::HandleChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCAContactSelectionDialog::HandleChange( TEnumsPC::TItem /* aType */,
                                              TInt /*aIndex,*/, TEnumsPC::TChange aChangeType )
    {
    CHAT_DP_FUNC_ENTER( "HandleChange" );
    TInt error( KErrNone );
    switch ( aChangeType )
        {
            /*case TEnumsPC::EStorageEventPreChange:
                {
                // Ignore pre-change events
                return;
               	 }
               */
        case TEnumsPC::EDelete:
        case TEnumsPC::EMultipleChanges:
        case TEnumsPC::EChanged:
            {
            // There might be new or removed contacts
            TRAP( error, ListBox()->HandleItemRemovalL() );
            ListBox()->DrawNow();
            break;
            }
        case TEnumsPC::EAddition:
            {
            TRAP( error, ListBox()->HandleItemAdditionL() );
            break;
            }
        default:
            {
            break;
            }
        }


    CHAT_DP_FUNC_DONE( "HandleChange" );
    }

// -----------------------------------------------------------------------------
// CCAContactSelectionDialog::HandleSettingsChangeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCAContactSelectionDialog::HandleSettingsChangeL( TInt /*aChangedSettingEnum*/ )
    {

    }

// ---------------------------------------------------------
// CCABlockingDialog::SetLayout()
// Layouts current components according to AVKON LAF
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactSelectionDialog::SetLayout()
    {
    const TRect rect( Rect() );

    if ( ListBox() && FindBox() )
        {
        AknLayoutUtils::LayoutControl( ListBox(), rect, AKN_LAYOUT_WINDOW_list_gen_pane( 1 ) );
        AknLayoutUtils::LayoutControl( FindBox(), rect, AKN_LAYOUT_WINDOW_find_pane );
        }
    else if ( ListBox() )
        {
        AknLayoutUtils::LayoutControl( ListBox(), rect, AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );
        }
    }
// End file
