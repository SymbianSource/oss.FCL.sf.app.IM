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
* Description:  Blocking contacts dialog
*
*/


// INCLUDE FILES

#include "CCABlockingDialog.h"
#include "CCAAppUi.h"
#include "CCAStatusPaneHandler.h"
#include "chatngclient.hrh"
#include "ChatDebugAssert.h"
#include "IMUtils.h"
#include <csxhelp/imng.hlp.hrh>

#include <ChatNG.rsg>
#include <aknutils.h>
#include <aknsfld.h>
#include <aknlists.h>
#include <aknnavi.h>
#include <avkon.hrh>
#include <barsread.h>
#include <coemain.h>
#include <eikcapc.h>
#include <aknconsts.h>
#include <eikclbd.h>
#include <eikfrlbd.h>
#include <eikmenub.h>
#include <akniconutils.h>
#include <FeatMgr.h>
#include <hlplch.h>
#include <catouchutils.h>
// local variation
#include    "IMPrivateCRKeys.h"
#include    "IMVariant.hrh"

// LOCAL FUNCTION PROTOTYPES
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


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

CCABlockingDialog* CCABlockingDialog::NewL( TInt &aValue, MDesCArray *aArray, TInt aMenuBarResourceId,
                                            TInt aOkMenuBarResourceId, MEikCommandObserver *aCommand,
                                            TBool aDontShowTabGroup )
    {
    CCABlockingDialog *dialog = NewLC( aValue, aArray, aMenuBarResourceId, aOkMenuBarResourceId,
                                       aCommand, aDontShowTabGroup );
    CleanupStack::Pop();
    return dialog;
    }
// ---------------------------------------------------------
// CCABlockingDialog::NewLC
// ---------------------------------------------------------
//
CCABlockingDialog *CCABlockingDialog::NewLC( TInt &aValue, MDesCArray *aArray, TInt aMenuBarResourceId,
                                             TInt aOkMenuBarResourceId, MEikCommandObserver *aCommand,
                                             TBool aDontShowTabGroup )
    {
    CCABlockingDialog *dialog = new( ELeave )CCABlockingDialog( aValue,
                                                                aArray,
                                                                aMenuBarResourceId,
                                                                aOkMenuBarResourceId,
                                                                aCommand,
                                                                aDontShowTabGroup );
    CleanupStack::PushL( dialog );
    dialog->ConstructL( aMenuBarResourceId );
    return dialog;
    }
// ---------------------------------------------------------
// CCABlockingDialog::CCABlockingDialog
// ---------------------------------------------------------
//
CCABlockingDialog::CCABlockingDialog( TInt &aIndex,
                                      MDesCArray *aArray,
                                      TInt aMenuBarResourceId,
                                      TInt aOkMenuBarResourceId,
                                      MEikCommandObserver *aCommand,
                                      TBool aDontShowTabGroup )
        : iEnterKeyPressed( EFalse ),
        iSelectedItem( &aIndex ),
        iArray( aArray ),
        iCmdObserver( aCommand ),
        iMenuBarResourceId( aMenuBarResourceId ),
        iOkMenuBarResourceId( aOkMenuBarResourceId ),
        iFindBoxEnabled( EFalse ),
        iDontShowTabGroup( aDontShowTabGroup ),
        iExitEnabled( ETrue ),
        iAllowPenEvent( ETrue )
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    iTitlePane = appUi->CAStatusPane();
    }

// ---------------------------------------------------------
// CCABlockingDialog::ConstructL
// ---------------------------------------------------------
//
void CCABlockingDialog::ConstructL( TInt aMenuBarResourceId )
    {
    CAknDialog::ConstructL( aMenuBarResourceId );

#ifndef RD_30_DISABLE_TOUCH
    iMenuBar->SetContextMenuTitleResourceId( iOkMenuBarResourceId );
#endif // RD_30_DISABLE_TOUCH
    }

// ---------------------------------------------------------
// CCABlockingDialog::~CCABlockingDialog
// ---------------------------------------------------------
//
CCABlockingDialog::~CCABlockingDialog()
    {

    // delete the same
    delete iFindPaneText;

    // dialog will be gone, so tab groups can be drawn again
    if ( iTitlePane )
        {
        iTitlePane->RestoreNaviPane();
        }
    else
        {
        iTitlePane->ProhibitTabGroup( EFalse );
        }

    if ( iAvkonAppUi )
        {
        iAvkonAppUi->RemoveFromStack( this );
        }
    }


// ---------------------------------------------------------
// CCABlockingDialog::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CCABlockingDialog::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == CAknDialog::CountComponentControls() )
        {
        return FindBox();
        }
    return CAknDialog::ComponentControl( aIndex );
    }

// ---------------------------------------------------------
// CCABlockingDialog::CountComponentControls
// ---------------------------------------------------------
//
TInt CCABlockingDialog::CountComponentControls() const
    {
    TInt newitems = 0;
    if ( FindBox() )
        {
        newitems++;
        }

    return CAknDialog::CountComponentControls() + newitems;
    }

// ---------------------------------------------------------
// CCABlockingDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CCABlockingDialog::OkToExitL( TInt aButtonId )
    {
    // If exit not enable, return false
    if ( !iExitEnabled )
        {
        return EFalse;
        }

    TBool b = CAknDialog::OkToExitL( aButtonId );
    *iSelectedItem = ListBox()->CurrentItemIndex();
    if ( FindBox() )
        {
        if ( *iSelectedItem != -1 )
            {
            *iSelectedItem = STATIC_CAST( CAknFilteredTextListBoxModel*, ListBox()->
                                          Model() )->Filter()->FilteredItemIndex( *iSelectedItem );
            }
        }

    if ( aButtonId == EAknSoftkeyContextOptions )
        {
        // MSK command, show context menu
        ShowContextMenuL();
        return EFalse;
        }

    return b;
    }

// ---------------------------------------------------------
// CCABlockingDialog::Draw
// ---------------------------------------------------------
//
void CCABlockingDialog::Draw( const TRect & ) const
    {
    // this removes flicker from CEikDialog's Draw().
    }

void CCABlockingDialog::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dim the help menu item
        aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
        }

    switch ( aResourceId )
        {
        case R_CHATCLIENT_MENUPANE_BLOCKING :
            {
            if ( ! ListBox()->Model()->NumberOfItems() )
                {
                aMenuPane->SetItemDimmed( EChatClientBlockingUnblock,
                                          ETrue );
                }
            break;
            }
        default :
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCABlockingDialog::ProcessCommandL
// ---------------------------------------------------------
//
void CCABlockingDialog::ProcessCommandL( TInt aCommand )
    {
    iAllowPenEvent = EFalse;
    
    // launch help
    if ( aCommand == EChatClientCmdHelp )
        {

        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                             ( CCoeEnv::Static()->AppUi() )->AppHelpContextL () );
        return;
        }

    // Handle exit command
    if ( aCommand == EChatClientCmdExit )
        {
        TryExitL( aCommand );
        return;
        }

    CAknDialog::ProcessCommandL( aCommand );

    if ( FindBox() && iFindType == EPopupFind )
        {
        AknFind::HandleFindPopupProcessCommandL( aCommand, ListBox(), FindBox(), this );

        }
    //update selected index
    *iSelectedItem = ListBox()->CurrentItemIndex();

    if ( iCmdObserver )
        {
        iCmdObserver->ProcessCommandL( aCommand );
        }
    
    iAllowPenEvent = ETrue;
    }

// ---------------------------------------------------------
// CCABlockingDialog::SelectionListProcessCommandL
// ---------------------------------------------------------
//
void CCABlockingDialog::SelectionListProcessCommandL( TInt aCommand )
    {
    //update selected index
    *iSelectedItem = ListBox()->CurrentItemIndex();

    AknSelectionService::HandleSelectionListProcessCommandL( aCommand, ListBox() );
    }

void CCABlockingDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            {
            ShowContextMenuL();
            // Should do nothing after this - the dialog might be deleted..
            break;
            }
        default:
            {
            break;
            }
        };
    }

// ---------------------------------------------------------
// CCABlockingDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CCABlockingDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
    // must pass escape event to dialog - findbox and listbox "eat" escape events
    TKeyResponse res = EKeyWasNotConsumed;
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
        {
        iCmdObserver->ProcessCommandL( EChatClientCmdExit );

        res = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }

    if ( res == EKeyWasConsumed )
        {
        return res;
        }

    // Set the needRefresh to true
    TBool needRefresh = ETrue;

    // Offer key events to find box if it is visible
    if ( FindBox()->IsVisible() )
        {
        res = AknFind::HandleFindOfferKeyEventL(
                  aKeyEvent, aType, this, ListBox(), FindBox(),
                  iFindType == EPopupFind, needRefresh );
        }

    if( needRefresh && FindBox() && FindBox()->IsVisible() )
        {
        if ( iFindType == EPopupFind )
            {
            AknFind::HandlePopupFindSizeChanged( this, ListBox(), FindBox() );
            }
        else if ( iFindType == EFixedFind )
            {
            AknFind::HandleFixedFindSizeChanged(
                this, ( CAknColumnListBox* )ListBox(), FindBox() );
            }

        DrawNow();
        }

    if ( res == EKeyWasConsumed )
        {
        return res;
        }

    // Give the events to listbox first. This is because otherwise all events do not go to the controls.
    // Dialogs filter out KeyUp/KeyDown events. If we do not do this, marking is broken; Sometimes it does
    // not mark list items correctly.
    if ( aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow )
        {
        // Multipage dialogs need the left/right keys
        res = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    else
        {
        res = ListBox()->OfferKeyEventL( aKeyEvent, aType );
        }

    if ( ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter ) && ( aType == EEventKey ) )
        {
        ShowContextMenuL();
        res = EKeyWasConsumed;
        }

    if ( aKeyEvent.iCode == EKeyPhoneSend )
        {
        res = EKeyWasConsumed;
        }

    if ( res == EKeyWasNotConsumed )
        {
        res = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }

    if ( iEnterKeyPressed )
        {
        TryExitL( EAknSoftkeyOk );
        }


    // should do nothing after this.
    return res;
    }

// ---------------------------------------------------------
// CCABlockingDialog::ListBox
// ---------------------------------------------------------
//
CEikListBox *CCABlockingDialog::ListBox() const
    {
    return ( CEikListBox* )Control( ESelectionListControl );
    }

// ---------------------------------------------------------
// CCABlockingDialog::FindBox
// ---------------------------------------------------------
//
CAknSearchField *CCABlockingDialog::FindBox() const
    {
    CFindControl *control = ( CFindControl* )ControlOrNull( EFindControl ) ;
    if ( control )
        {
        return control->iFind;
        }
    return NULL;
    }


// ---------------------------------------------------------
// CCABlockingDialog::GetFindBoxTextL
// ---------------------------------------------------------
//
const TDesC& CCABlockingDialog::GetFindBoxTextL()
    {

    //if already some data available, free the same
    if ( iFindPaneText )
        {
        delete iFindPaneText;
        iFindPaneText = NULL;
        }

    if ( iFindBoxEnabled )
        {
        CAknSearchField *findBox = FindBox();

        if ( findBox )
            {
            //gets the text from the findBox
            iFindPaneText = findBox->Editor().GetTextInHBufL();

            if ( !iFindPaneText )
                {
                //if no text in the findPane initialise with KNullDesC()
                iFindPaneText = KNullDesC().AllocL();
                }

            return *iFindPaneText;
            }
        }
    return KNullDesC;

    }

// ---------------------------------------------------------
// CCABlockingDialog::SetupFind
// ---------------------------------------------------------
//
void CCABlockingDialog::SetupFind( TFindType aType )
    {
    iFindType = aType;
    }

// ---------------------------------------------------------
// CCABlockingDialog::IsAcceptableListBoxType
// ---------------------------------------------------------
//
TBool CCABlockingDialog::IsAcceptableListBoxType(
    TInt /*aControlType*/,
    TBool &/*aIsFormattedCellListBox*/ ) const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CCABlockingDialog::IsFormattedCellListBox
// ---------------------------------------------------------
//
TBool CCABlockingDialog::IsFormattedCellListBox() const
    {
    CEikCaptionedControl *line = Line( ESelectionListControl );
    TInt controlType = line->iControlType;
    TBool result = EFalse;
    if ( IsAcceptableListBoxType( controlType, result ) )
        {
        return result;
        }

    switch ( controlType )
        {
        case EAknCtSingleListBox:
        case EAknCtSingleNumberListBox:
        case EAknCtSingleHeadingListBox:
        case EAknCtSingleGraphicListBox:
        case EAknCtSingleGraphicHeadingListBox:
        case EAknCtSingleNumberHeadingListBox:
        case EAknCtSingleLargeListBox:
            {
            return EFalse;
            }
        case EAknCtDoubleListBox:
        case EAknCtDoubleNumberListBox:
        case EAknCtDoubleTimeListBox:
        case EAknCtDoubleLargeListBox:
        case EAknCtDoubleGraphicListBox:
        case EAknCtSettingListBox:
        case EAknCtSettingNumberListBox:
            {
            return ETrue;
            }
        default:
            // If you hit this assert, you should derive from this dialog and
            // implement IsAcceptableListBoxType() method.
            __CHAT_ASSERT_DEBUG( EFalse );
            return ETrue;
        }
    }

// ---------------------------------------------------------
// CCABlockingDialog::IconArray
// ---------------------------------------------------------
//
CArrayPtr<CGulIcon>* CCABlockingDialog::IconArray() const
    {
    if ( IsFormattedCellListBox() )
        {
        return ( ( CEikFormattedCellListBox* )ListBox() )->ItemDrawer()->FormattedCellData()->IconArray();
        }
    else
        {
        return ( ( CEikColumnListBox* )ListBox() )->ItemDrawer()->ColumnData()->IconArray();
        }
    }

// ---------------------------------------------------------
// CCABlockingDialog::SetIconArrayL
// ---------------------------------------------------------
//
void CCABlockingDialog::SetIconArrayL( CArrayPtr<CGulIcon>* aIcons )
    {
    CArrayPtr<CGulIcon>* oldicons = IconArray();
    if ( oldicons )
        {
        oldicons->ResetAndDestroy();
        delete oldicons;
        }

    if ( IsFormattedCellListBox() )
        {
        ( ( CEikFormattedCellListBox* )ListBox() )->ItemDrawer()->FormattedCellData()->
        SetIconArray( aIcons );
        }
    else
        {
        ( ( CEikColumnListBox* )ListBox() )->ItemDrawer()->ColumnData()->SetIconArray( aIcons );
        }

    }

// ---------------------------------------------------------
// CCABlockingDialog::HandleItemRemovalL
// ---------------------------------------------------------
//
void CCABlockingDialog::HandleItemRemovalL()
    {
    CEikListBox* listBox = ListBox();
    listBox->HandleItemRemovalL();
    TInt index( listBox->CurrentItemIndex() );
    TInt myItems( listBox->Model()->NumberOfItems() );

    if ( index > myItems || index == KErrNotFound && myItems > 0 )
        {
        listBox->SetCurrentItemIndex( myItems - 1 );
        }

    if ( myItems == 0 && iFindBoxEnabled )
        {
        // Hide find box
        TSize ignore( 0, 0 );
        SetSizeAndPosition( ignore );
        listBox->DrawDeferred();
        }
    // item removed now reset find box and show all
    ResetFindBoxL();
    }

// ---------------------------------------------------------
// CCABlockingDialog::SetDialogTitleL
// ---------------------------------------------------------
//
void CCABlockingDialog::RefreshDialogTitleL()
    {
    HBufC* title = iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_BLOCKED_CONTACTS_TITLE );
    iTitlePane->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );
    }

// ---------------------------------------------------------
// CCABlockingDialog::HandleItemAdditionL
// ---------------------------------------------------------
//
void CCABlockingDialog::HandleItemAdditionL()
    {
    ListBox()->HandleItemAdditionL();
    if ( !iFindBoxEnabled )
        {
        // Show find box
        TSize ignore( 0, 0 );
        SetSizeAndPosition( ignore );
        }
    // item added now reset find box and show all
    ResetFindBoxL();
    }

// ---------------------------------------------------------
// CCABlockingDialog::CreateCustomControlL
// ---------------------------------------------------------
//
SEikControlInfo CCABlockingDialog::CreateCustomControlL( TInt aControlType )
    {
    CFindControl *control1 = NULL;
    TBool findControl = EFalse;
    if ( aControlType == EAknCtSelectionListFixedFind )
        {
        iFindType = EFixedFind;
        findControl = ETrue;
        }

    if ( aControlType == EAknCtSelectionListPopupFind )
        {
        iFindType = EPopupFind;
        findControl = ETrue;
        }

    if ( findControl )
        {
        control1 = new ( ELeave ) CFindControl;
        }

    SEikControlInfo info;
    info.iControl = control1;
    info.iFlags = 0;
    info.iTrailerTextId = 0;
    return info;
    }

// ---------------------------------------------------------
// CCABlockingDialog::FocusChanged
// ---------------------------------------------------------
//
void CCABlockingDialog::FocusChanged(TDrawNow aDrawNow)
	{
	if ( FindBox() )
		{
	    FindBox()->SetFocus( IsFocused() );
		}
	}

// ---------------------------------------------------------
// CCABlockingDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void CCABlockingDialog::PreLayoutDynInitL()
    {
    CEikCaptionedControl *line = Line( ESelectionListControl );

    CFindControl *control1 = ( CFindControl* )ControlOrNull( EFindControl );
    CAknSearchField *control = NULL;
    CAknSearchField::TSearchFieldStyle flags = CAknSearchField::EFixed;
    if ( control1 )
        {
        if ( iFindType == EPopupFind )
            {
            flags = CAknSearchField::EPopup;
            }
        if ( iFindType == EFixedFind )
            {
            flags = CAknSearchField::EFixed;
            }
        control = CAknSearchField::NewL( *control1, flags, 0, 40 );
        control1->iFind = control;
        }

    CAknDialog::PreLayoutDynInitL();
    iAvkonAppUi->AddToStackL( this );

    CEikListBox *listbox = ListBox();
    CEikTextListBox *txtlistbox = ( CEikTextListBox* )listbox;

    // Listbox items
    if ( iArray )
        {
        txtlistbox -> Model() -> SetItemTextArray( iArray );
        txtlistbox -> Model() -> SetOwnershipType( ELbmDoesNotOwnItemArray );
        }

    // Scrollbar
    listbox->CreateScrollBarFrameL( ETrue );
    listbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Listbox observer
    listbox->SetListBoxObserver( this );

    // SimplifiedUI localisation variation for empty list
    if ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey ) &
         EIMFeatSimplifiedUI )
        {
        HBufC* emptyTxtBuf = IMUtils::CombineStringFromResourceLC
                             ( R_QTN_CHAT_BLOCKED_CONTACTS_EMPTY,
                               R_QTN_CHAT_BLOCKED_CONTACTS_EMPTY_SECONDARY );

        listbox->View()->SetListEmptyTextL( *emptyTxtBuf );
        CleanupStack::PopAndDestroy( emptyTxtBuf );
        }


    // set marquee on
    if ( IsFormattedCellListBox() )
        {
        ( ( CEikFormattedCellListBox* )ListBox() )->ItemDrawer()->FormattedCellData()
        ->EnableMarqueeL( ETrue );
        }
    else
        {
        ( ( CEikColumnListBox* )ListBox() )->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
        }

    if ( FindBox() )
        {
        if ( iFindType == EPopupFind )
            {
            STATIC_CAST( CAknFilteredTextListBoxModel*, ListBox()->Model() )->CreateFilterL( ListBox(), FindBox() );
            }
        else if ( iFindType == EFixedFind )
            {
            // Fixed find is only available with list_single_graphic_pane
            STATIC_CAST( CAknFilteredTextListBoxModel*, ListBox()->Model() )->CreateFilterL( ListBox(), FindBox() );
            }
        }

    iTitlePane->ClearNaviPaneL();

    HBufC* title = iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_BLOCKED_CONTACTS_TITLE );
    iTitlePane->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    // when the dialog is active, do not show any tabs even if the underlying
    // view would like to show them
    iTitlePane->ProhibitTabGroup();
    }

// ---------------------------------------------------------
// CCABlockingDialog::SetSizeAndPosition
// ---------------------------------------------------------
//
void CCABlockingDialog::SetSizeAndPosition( const TSize &/*aSize*/ )
    {
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    SetRect( rect );

    // Show find box if there are items in list box
    if( FindBox() && iArray->MdcaCount() != 0/*Check the data source*/ )
		{				    				
        /*if( iFindType == EPopupFind )
        	{
        	FindBox()->MakeVisible( EFalse );
        	AknFind::HandlePopupFindSizeChanged( this, ListBox(), FindBox() );
        	}
        else if( iFindType == EFixedFind )
        	{
        	AknFind::HandleFixedFindSizeChanged(
        	                this,( CAknColumnListBox* )ListBox(), FindBox() );
        	FindBox()->MakeVisible( ETrue );
        	}*/

        FindBox()->MakeVisible( EFalse );
        iFindBoxEnabled = ETrue;
        SetLayout();//layouting controls.
        FindBox()->MakeVisible( ETrue );
	    FindBox()->SetFocus( IsFocused() );	
        }
    else
        {
        // Reset filtering
        TRAPD( error, static_cast<CAknFilteredTextListBoxModel*>( ListBox()->Model() )
               ->Filter()->ResetFilteringL(); );
        if ( error )
            {
            CActiveScheduler::Current()->Error( error );
            }

        // Signal listbox that find box went away
        static_cast<CAknColumnListBoxView*>( ListBox()->View() )
        ->SetFindEmptyListState( EFalse );

        // Hide find box
        if ( FindBox() )
            {
            FindBox()->MakeVisible( EFalse );
            FindBox()->SetFocus( EFalse );
            }

        // Layout list box correctly
        iFindBoxEnabled = EFalse;
        SetLayout();//layouting controls.
        }
    }

// ---------------------------------------------------------
// CCABlockingDialog::HandlePointerEventL
// ---------------------------------------------------------
//
void CCABlockingDialog::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
	if( !iAllowPenEvent )
		{
		return;
		}
	
    if ( !iArray->MdcaCount() == 0 )
        {
        ListBox()->HandlePointerEventL( aPointerEvent );
        return;
        }

    if ( !CATouchUtils::PenEnabled() )
        {
        return;
        }

    // Single tap on main pane (not on scroll bar)
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        ShowContextMenuL();
        }
    else
        {
        CAknDialog::HandlePointerEventL( aPointerEvent );
        }
    }

// ---------------------------------------------------------
// CCABlockingDialog::ResetFindBoxL
// ---------------------------------------------------------
//

void CCABlockingDialog::ResetFindBoxL()
    {
    if ( FindBox() && ListBox()->Model()->NumberOfItems() != 0 )
        {
        FindBox()->ResetL();
        FindBox()->SetSearchTextL( KNullDesC );
        FindBox()->DrawNow();
        FindBox()->SetFocus( ETrue );
        }
    }
// ---------------------------------------------------------
// CCABlockingDialog::ShowContextMenuL
// ---------------------------------------------------------
//
void CCABlockingDialog::ShowContextMenuL()
    {
#ifndef RD_30_DISABLE_TOUCH
    // S60 COMMON
    iMenuBar->TryDisplayContextMenuBarL();

#else
    // S60 3.0
    iMenuBar->SetMenuTitleResourceId( iOkMenuBarResourceId );
    // This must be trapped so that menubar will be restored on leave
    TRAP_IGNORE( iMenuBar->TryDisplayMenuBarL() );
    iEikonEnv->EikAppUi()->HandleStackChanged();
    iMenuBar->SetMenuTitleResourceId( iMenuBarResourceId );
#endif // RD_30_DISABLE_TOUCH   
    }

// ---------------------------------------------------------
// CCABlockingDialog::SetLayout()
// Layouts current components according to AVKON LAF
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCABlockingDialog::SetLayout()
    {
    const TRect rect( Rect() );

    if ( ListBox() && FindBox() && iFindBoxEnabled )
        {
        AknLayoutUtils::LayoutControl( ListBox(), rect, AKN_LAYOUT_WINDOW_list_gen_pane( 1 ) );
        AknLayoutUtils::LayoutControl( FindBox(), rect, AKN_LAYOUT_WINDOW_find_pane );
        }
    else if ( ListBox() )
        {
        AknLayoutUtils::LayoutControl( ListBox(), rect, AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );
        }
    }



/**
 * Help Key Support
 * This function is called by the Framework to get the context to launch
 * Help
 *
 * @param aContext	The context that contains the appid and the help id.
 */
void CCABlockingDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_BLOCKED_VIEW ;
    }

// ---------------------------------------------------------
// CCABlockingDialog::SetExitEnabled( TBool value )
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCABlockingDialog::SetExitEnabled( TBool aValue )
    {
    iExitEnabled = aValue;
    }

//  End of File
