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
* Description:  Text viewer component
*
*/



// INCLUDE FILES
#include    "CCATextViewControl.h"
#include    "CCACustomDraw.h"
#include    "ChatDebugPrint.h"
#include    "chatdebugassert.h"
#include    "catouchutils.h"

#include    <frmtlay.h>     // CTextLayout
#include    <frmtview.h>    // CTextView
#include    <coemain.h>     // CCoeEnv

#include    <AknUtils.h>
#include    <aknsettingcache.h>
#include    <aknenv.h>
#include    <aknconsts.h>
#include    <avkon.mbg>
#include    <AknAppUi.h>

#include    <eikrted.h>
#include    <AknLayoutScalable_Apps.cdl.h>
#include    <AknsBasicBackgroundControlContext.h>
#include    <AknsDrawUtils.h>
#include    <AknsUtils.h>

#include    "impsbuilddefinitions.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCATextViewControl::CCATextViewControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCATextViewControl::CCATextViewControl() :
        iPenType( EPenForwardToChild )
    {
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCATextViewControl::ConstructL( const TRect& aRect,
                                     const CCoeControl& aParent,
                                     MLayDoc* aTextLayout )
    {
    CreateWindowL( &aParent );

    // get the drawing device
    CWindowGc& gc = SystemGc();
    CBitmapDevice* device = static_cast<CBitmapDevice*>( gc.Device() );

    // create text layout and text view

    iViewRect = aRect;
    iClipping = new ( ELeave ) RRegion( aRect );

    iLayout = CTextLayout::NewL( aTextLayout, iViewRect.Width() );
    iTextView = CTextView::NewL( iLayout, iViewRect,
                                 device, device, &Window(), NULL, &iCoeEnv->WsSession() );

    iTextView->EnableFlickerFreeRedraw();
    iTextView->SetOpaque( ETrue );

    // create our background drawer
    iCustomDraw = new( ELeave )CCACustomDraw( iViewRect );
    iLayout->SetCustomDraw( iCustomDraw );

    // and scrollbar
    if ( CATouchUtils::PenEnabled() )
        {
        // Observer
        iScrollBar = new( ELeave )CEikScrollBarFrame( this, this );
        }
    else
        {
        // No observer
        iScrollBar = new( ELeave )CEikScrollBarFrame( this, NULL );
        }

    CAknAppUi* appUi = iAvkonAppUi;
    if ( AknLayoutUtils::DefaultScrollBarType( appUi ) == CEikScrollBarFrame::EDoubleSpan )
        {
        // For EDoubleSpan type scrollbar
        // If pen is supported, SB needs to own a window
        iScrollBar->CreateDoubleSpanScrollBarsL( CATouchUtils::PenEnabled(),
                                                 EFalse, ETrue, EFalse );
        iScrollBar->SetTypeOfVScrollBar( CEikScrollBarFrame::EDoubleSpan );
        }
    else
        {
        // For EArrowHead type scrollbar
        iScrollBar->SetTypeOfVScrollBar( CEikScrollBarFrame::EArrowHead );
        }
    iScrollBar->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                         CEikScrollBarFrame::EOn );
    TEikScrollBarModel vSbarModel;
    vSbarModel.iThumbPosition = 0; // current position
    vSbarModel.iScrollSpan = 0; // how many items
    vSbarModel.iThumbSpan = 0; // ignored

    iScrollBar->Tile( &vSbarModel );
    iScrollBar->SetVFocusPosToThumbPos( vSbarModel.iThumbPosition );

    iBgContext = CAknsBasicBackgroundControlContext::NewL(
                     KAknsIIDQsnBgAreaMain, Rect(), ETrue );
    iCustomDraw->SetBackgroundContext( iBgContext, this );

    // Set extensions
    //SetRect is not necessary, because it will be called again in SizeChanged of CCAChatViewContainer and CCAConversationsContainer
    //SetRect( aRect );
    ActivateL();

    // we're ready
    static_cast<CCoeAppUi*>( iEikonEnv->AppUi() )->AddToStackL( this );
    iAddedToStack = ETrue;

    // Enable draging
    if ( DrawableWindow() && CATouchUtils::PenEnabled() )
        {
        EnableDragEvents();
        Window().SetPointerGrab( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCATextViewControl* CCATextViewControl::NewL( const TRect& aRect,
                                              const CCoeControl& aParent,
                                              MLayDoc* aTextLayout )
    {
    CCATextViewControl* self = new( ELeave ) CCATextViewControl;
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent, aTextLayout );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCATextViewControl::~CCATextViewControl()
    {
    if ( iAddedToStack )
        {
        static_cast<CCoeAppUi*>( iEikonEnv->AppUi() )->RemoveFromStack( this );
        }

    delete iScrollBar;
    delete iCustomDraw;
    delete iTextView;
    delete iLayout;

    delete iBgContext;
    if ( iClipping )
        {
        iClipping->Destroy();
        }
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::UpdateScrollBarsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::UpdateScrollBarsL( TBool aScrollDraw /* = ETrue */ )
    {
    TInt height = iLayout->FormattedHeightInPixels();
    TInt span = iViewRect.Height();
    TInt pos = iLayout->PixelsAboveBand();

    if ( span < 0 )
        {
        span = 0;
        }

    if ( pos < 0 )
        {
        pos = 0;
        }

    TEikScrollBarModel vSbarModel;
    vSbarModel.iThumbPosition = pos; // current position
    vSbarModel.iScrollSpan = height; // total length
    vSbarModel.iThumbSpan = span; // bar length

    CHAT_DP( D_CHAT_LIT( "CCATextViewControl::UpdateScrollBarsL [%d...%d/%d]" ),
             pos, pos + span, height );

    // Double span
    if ( iScrollBar &&
         iScrollBar->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {
        TAknDoubleSpanScrollBarModel vDsModel( vSbarModel );
        iScrollBar->Tile( &vDsModel );
        iScrollBar->SetVFocusPosToThumbPos( vDsModel.FocusPosition() );

        }
    // ArrowHead
    else
        {
        iScrollBar->Tile( &vSbarModel );
        iScrollBar->SetVFocusPosToThumbPos( vSbarModel.iThumbPosition );
        }
    if ( aScrollDraw )
        {
        iScrollBar->DrawScrollBarsNow();
        }

    }


// -----------------------------------------------------------------------------
// CCATextViewControl::UpdateViewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::UpdateViewL()
    {
    iTextView->FormatTextL();
    iTextView->SetSelectionVisibilityL( EFalse );
    TCursorSelection sel;
    // Don't scroll if some item is highlighted
    if ( !iIsHighlighted )
        {
        sel = TCursorSelection( iLayout->DocumentLength(), 0 );
        ScrollVisibleL( sel, ETrue );
        }
    else
        {
        sel = iTextView->Selection();
        ScrollVisibleL( sel, ETrue );
        }

    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::SetHighlighted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::SetHighlighted( TBool aIsHighlighted )
    {
    iIsHighlighted = aIsHighlighted;
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::ScrollBarFrame
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CEikScrollBarFrame* CCATextViewControl::ScrollBarFrame()
    {
    return iScrollBar;
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::ScrollBarFrame
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCATextViewControl::FindDocPos( TPoint aPoint )
    {
    TTmPosInfo2 posInfo;
    TBool found = EFalse;

    TRAPD( err, found = iTextView->FindXyPosL( aPoint, posInfo ) );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        // Return not found
        return KErrNotFound;
        }

    if ( found )
        {
        CHAT_DP( D_PLAIN_LIT( "CCATextViewControl::FindDocPos:\
  	    Pos fount at %d" ), posInfo.iDocPos.iPos );

        return posInfo.iDocPos.iPos;
        }
    // not found
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CCATextViewControl::MopSupplyObject
//
// ---------------------------------------------------------
//
TTypeUid::Ptr CCATextViewControl::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CCATextViewControl::HandleScrollEventL
//
// ---------------------------------------------------------
//
void CCATextViewControl::HandleScrollEventL( CEikScrollBar* /*aScrollBar*/,
                                             TEikScrollEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEikScrollUp:
            {
            ScrollLinesL( 1 );
            break;
            }
        case EEikScrollDown:
            {
            ScrollLinesL( -1 );
            break;
            }
        case EEikScrollPageUp:
            {
            iTextView->ScrollDisplayL( TCursorPosition::EFPageUp );
            UpdateScrollBarsL();
            DrawDeferred();
            break;
            }
        case EEikScrollPageDown:
            {
            iTextView->ScrollDisplayL( TCursorPosition::EFPageDown );
            UpdateScrollBarsL();
            DrawDeferred();
            break;
            }
        case EEikScrollThumbDragVert:
            {
            // Get position values from scrollbar
            const TEikScrollBarModel* model =
                iScrollBar->VerticalScrollBar()->Model();
            TInt thumb = model->iThumbPosition;
            if ( CEikScrollBarFrame::EDoubleSpan ==
                 iScrollBar->TypeOfVScrollBar() )
                {
                const TAknDoubleSpanScrollBarModel* dsModel =
                    static_cast <const TAknDoubleSpanScrollBarModel*>( model );
                thumb = dsModel->FocusPosition();
                }

            // Scroll the textview according to scrollbar
            TViewYPosQualifier yPosQua;
            yPosQua.SetHotSpot( TViewYPosQualifier::EFViewTopOfLine );
            TInt topPos = -thumb;
            iTextView->SetViewL( 0, topPos, yPosQua );
            break;
            }
        case EEikScrollThumbReleaseVert:
            {
            // Nothing to do here because EEikScrollThumbDragVert gets
            // the scrollbar in correct state
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::OfferKeyEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CCATextViewControl::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                 TEventCode aType )
    {
    // Commented because it generates so much log
    // CHAT_DP( D_CHAT_LIT("CCATextViewControl::OfferKeyEventL %d, type %d"),
    //          aKeyEvent, aType )

    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    switch ( aKeyEvent.iCode )
        {
        case EKeyDownArrow:
            {
            //scroll lines one line down
            ScrollLinesL( -1 );
            return EKeyWasConsumed;

            }
        case EKeyUpArrow:
            {
            //scroll lines one line up
            ScrollLinesL( 1 );
            return EKeyWasConsumed;

            }
        default:
            {
            // No handling for other events
            break;
            }
        }

    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::HandlePointerEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    if ( !CATouchUtils::PenEnabled() )
        {
        return;
        }

    switch ( iPenType )
        {
        case EPenForwardToChild:
            {
            // normal (CCoeControl) behaviour
            CCoeControl::HandlePointerEventL( aPointerEvent );
            break;
            }
        case EPenForwardToParent:
            {
            // forward all to parent
            if ( Parent() )
                {
                Parent()->HandlePointerEventL( aPointerEvent );
                }
            break;
            }
        // Process event ourself
        case EPenProcessEvent:
            {
            ProcessStylusEventL( aPointerEvent );
            break;
            }
        case EPenIgnoreEvent:
            {
            // Ignore all
            break;
            }
        default:
            {
            // Can't be
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::Draw
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();

    // Clip text control and scrollbar to prevent flickering
    gc.SetClippingRegion( *iClipping );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( iBgContext )
        {
        // Draw the skin background
        AknsDrawUtils::Background(
            skin, iBgContext, this, gc, aRect );
        }
    else
        {
        // clear the area
        gc.SetBrushColor( iEikonEnv->ControlColor( EColorWindowBackground, *this ) );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.Clear( aRect );
        }

    TRAP_IGNORE( iTextView->DrawL( aRect ) );

    // Cancel clipping
    gc.CancelClippingRegion();
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::SizeChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::SizeChanged()
    {
    TAknWindowLineLayout rpLayout( AknLayoutScalable_Apps::list_im_pane( 3 ) );
    TAknLayoutRect rpLayoutRect;
    rpLayoutRect.LayoutRect( Rect(), rpLayout );

    // update rects
    iViewRect = rpLayoutRect.Rect();

    iCustomDraw->SetRect( iViewRect );
    iTextView->SetViewRect( iViewRect );
    iLayout->SetWrapWidth( iViewRect.Width() );

    CAknAppUi* appUi = iAvkonAppUi;
    if ( AknLayoutUtils::DefaultScrollBarType( appUi ) == CEikScrollBarFrame::EDoubleSpan )
        {
        // For EDoubleSpan type scrollbar
        TAknWindowLineLayout sbLayout( AknLayoutScalable_Apps::scroll_pane_cp07( 3 ) );
        AknLayoutUtils::LayoutVerticalScrollBar( iScrollBar, Rect(), sbLayout );
        TAknLayoutRect sbLayoutRect;
        sbLayoutRect.LayoutRect( Rect(), sbLayout );
        }

    if ( iBgContext )
        {
        iBgContext->SetRect( Parent()->Rect() );
        iBgContext->SetParentPos( Parent()->PositionRelativeToScreen() );
        }

    // Setup clipping region
    iClipping->Clear();
    iClipping->AddRect( Rect() );

    // clip text control
    iClipping->SubRect( iTextView->ViewRect() );

    // clip scrollbar
    if ( iScrollBar->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {
        TAknWindowLineLayout sbLayout( AknLayoutScalable_Apps::scroll_pane_cp07( 3 ) );
        AknLayoutUtils::LayoutVerticalScrollBar( iScrollBar, Rect(), sbLayout );
        TAknLayoutRect sbLayoutRect;
        sbLayoutRect.LayoutRect( Rect(), sbLayout );
        iClipping->SubRect( sbLayoutRect.Rect() );
        }

    // and update view
    TRAPD( err, ScrollLinesL( 0 );
           UpdateViewL() );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }

    }

// -----------------------------------------------------------------------------
// CCATextViewControl::HandleFormatChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::HandleFormatChangedL( TCursorSelection& aChanged,
                                               TBool aPreserveFocus /* = EFalse */ )
    {
    // rich text has changed, inform viewer
    iTextView->SetSelectionVisibilityL( EFalse );

    if ( aPreserveFocus )
        {
        // Preserve focus
        TCursorSelection sel = iTextView->Selection();
        // This seems to be the only method which doesn't change focus.
        // --> so it's possible to preserve focus without flicker
        iTextView->HandleGlobalChangeNoRedrawL();
        ScrollVisibleL( sel, EFalse );
        }
    else
        {
        // Move focus along with changes
        iTextView->HandleRangeFormatChangeL( aChanged );
        }
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::HandleInsertDeleteL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::HandleInsertDeleteL( TCursorSelection& aChanged,
                                              TInt aDeletedChars )
    {
    // inform about insertion or deletion
    iTextView->SetSelectionVisibilityL( EFalse );
    iTextView->HandleInsertDeleteL( aChanged, aDeletedChars );
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::HandleAdditionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::HandleAdditionL( TBool aFirst,
                                          TCursorSelection& aSelection,
                                          TBool aTop )
    {
    // inform about addition
    if ( aFirst )
        {
        iTextView->FormatTextL();
        }
    else
        {
        iTextView->HandleAdditionalCharactersAtEndL();
        }

    ScrollVisibleL( aSelection, aTop );
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::ScrollVisibleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::ScrollVisibleL( TCursorSelection& aSelection, TBool aTop )
    {
    TCursorSelection sel( aSelection );
    if ( !ParagraphFits( sel.LowerPos() ) )
        {
        // selection doesn't fit to screen
        TInt pos( aTop ? aSelection.LowerPos() : aSelection.HigherPos() );
        sel.SetSelection( pos, pos );
        }

    // scroll selection to screen
    iTextView->SetSelectionVisibilityL( EFalse );
    iTextView->SetSelectionL( sel );

    // and update scroll bars
    UpdateScrollBarsL();
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::ScrollSelectionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCATextViewControl::ScrollSelection()
    {
    return iTextView->Selection();
    }


// -----------------------------------------------------------------------------
// CCATextViewControl::ScrollLinesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::ScrollLinesL( TInt aAmount )
    {
    iLayout->ScrollLinesL( aAmount );
    UpdateScrollBarsL();
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::IsVisible
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCATextViewControl::IsVisible( TInt aPos )
    {
    TTmDocPos pos( aPos, ETrue );
    TTmLineInfo lineInfo;

    if ( !iLayout->PosInBand( pos, &lineInfo ) )
        {
        // current position outside
        return EFalse;
        }

    if ( !ParagraphFits( aPos ) )
        {
        // paragraph doesn't fit to screen
        // extend scrolling one line up and down

        // iLineNumber starts from 0 and FirstCharOnLine assumes first line to be 1
        TInt lineNo( lineInfo.iLineNumber + 1 );

        // check previous line (if it exists)
        if ( lineNo > 1 )
            {
            pos.iPos = iLayout->FirstCharOnLine( lineNo - 1 );

            if ( !iLayout->PosInBand( pos ) )
                {
                // previous line outside
                return EFalse;
                }
            }

        // check next line
        pos.iPos = iLayout->FirstCharOnLine( lineNo + 1 );

        return iLayout->PosInBand( pos );
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::ParagraphFits
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCATextViewControl::ParagraphFits( TInt aPos )
    {
    return iLayout->ParagraphHeight( aPos ) <= iViewRect.Height();
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::RedrawL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::Redraw()
    {
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::SetPenBehaviour
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::SetPenBehaviour( TPenBehaviour aPen )
    {
    iPenType = aPen;
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::PenBehaviour
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCATextView::TPenBehaviour CCATextViewControl::PenBehaviour()
    {
    return iPenType;
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::ProcessStylusEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::ProcessStylusEventL(
    const TPointerEvent& /*aPointerEvent*/ )
    {
    // nothing to do. Let derived classes to handle text selection.
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::HandleGlobalChangeNoRedrawL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCATextViewControl::HandleGlobalChangeNoRedrawL(
    TViewYPosQualifier aYPosQualifier /*= TViewYPosQualifier()*/ )
    {
    iTextView->HandleGlobalChangeNoRedrawL( aYPosQualifier );
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CCATextViewControl::CountComponentControls() const
    {
    if ( iScrollBar && iScrollBar->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

// -----------------------------------------------------------------------------
// CCATextViewControl::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CCATextViewControl::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 && iScrollBar && iScrollBar->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {
        return iScrollBar->VerticalScrollBar();
        }
    else
        {
        return NULL;
        }
    }

//  End of File
