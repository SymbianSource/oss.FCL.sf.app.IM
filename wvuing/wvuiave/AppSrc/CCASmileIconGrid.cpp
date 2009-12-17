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
* Description:  Grid for smile icons
*
*/


// INCLUDE FILES
#include "CCASmileIconGrid.h"
#include "IMSmileUtilityNG.hrh"
#include "layoutDebugPrint.h"
#include "mcatapeventobserver.h"
#include "catouchutils.h"

#include <aknutils.h>
#include <akniconutils.h>
#include <aknlayout.lag>
#include <gulicon.h>
#include <eikdialg.h>
#include <AknsDrawUtils.h>
#include <AknAppUi.h>

#include <AknLayoutScalable_Avkon.cdl.h>

#include <akndef.h>

// CONSTANTS
// Max amount of rows and colums in grid
const TInt KMaxGridColumnCount( 7 );
const TInt KMaxGridRowCount( 7 );

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCASmileIconGrid* CCASmileIconGrid::NewL( CEikDialog* aParent,
                                          const RPointerArray<CGulIcon>& aIconArray )
    {
    CCASmileIconGrid* self = new ( ELeave ) CCASmileIconGrid( aParent,
                                                              aIconArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// destructor
CCASmileIconGrid::~CCASmileIconGrid()
    {
    }

// C++ constructor can NOT contain any code, that
// might leave.
//
CCASmileIconGrid::CCASmileIconGrid( CEikDialog* aParent,
                                    const RPointerArray<CGulIcon>& aIconArray )
        : iParent( aParent ), iIconArray( aIconArray )
    {
    }

// default constructor can leave.
void CCASmileIconGrid::ConstructL()
    {
    iIconCount = iIconArray.Count();

    iIsMirrored = ETrue ;
    SetContainerWindowL( *iParent );
    }


// ---------------------------------------------------------
// CCASmileIconGrid::MoveCursor
// Move cursor and redraw highlight.
// Called when arrow-key is pressed.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconGrid::MoveCursor( TInt aKeyCode )
    {
    iPrevCursorPos = iCursorPos;

    switch ( aKeyCode )
        {
        case EKeyLeftArrow:
            {
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                iCursorPos++;
                if ( iCursorPos >= iIconCount )
                    {
                    iCursorPos = 0;
                    }
                }
            else
                {
                iCursorPos--;
                if ( iCursorPos < 0 )
                    {
                    iCursorPos = iIconCount - 1;
                    }
                }
            break;
            }
        case EKeyRightArrow:
            {
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                iCursorPos--;
                if ( iCursorPos < 0 )
                    {
                    iCursorPos = iIconCount - 1;
                    }
                }
            else
                {
                iCursorPos++;
                if ( iCursorPos >= iIconCount )
                    {
                    iCursorPos = 0;
                    }
                }
            break;
            }
        case EKeyUpArrow:
            {
            if ( iCursorPos < iMaxColumns )
                {
                iCursorPos += ( iRowCount * iMaxColumns ) - 1;

                if ( iCursorPos >= iIconCount )
                    {
                    iCursorPos -= iMaxColumns;
                    }

                if ( iCursorPos < 0 )
                    {
                    iCursorPos = iIconCount - 1;
                    }
                }
            else
                {
                iCursorPos -= iMaxColumns;
                }
            break;
            }
        case EKeyDownArrow:
            {
            if ( iCursorPos < ( iIconCount - iMaxColumns ) )
                {
                iCursorPos += iMaxColumns;
                }
            else
                {
                iCursorPos %= iMaxColumns;
                iCursorPos++;
                if ( iCursorPos >= iMaxColumns ||
                     iCursorPos >= iIconCount )
                    {
                    iCursorPos = 0;
                    }
                }
            break;
            }
        default:
            {
            return;
            }
        }

    DrawNow();
    }

// ---------------------------------------------------------
// CCASmileIconGrid::SelectedBitmapId
// Called when OK-key or Select-softkey is pressed.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASmileIconGrid::SelectedBitmapId()
    {
    return iCursorPos;
    }

// ---------------------------------------------------------
// CCASmileIconGrid::HeightInRows
// Return row count of grid.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASmileIconGrid::HeightInRows()
    {
    return iRowCount;
    }

// ---------------------------------------------------------
// CCASmileIconGrid::SetLayout
// Set layout of identifier grid.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconGrid::SetLayout()
    {
    iIsMirrored = AknLayoutUtils::LayoutMirrored() ;

    // popup_grid_graphic_window (Parent)
    TRect parentRect( iParent->Rect() );
    LAYOUT_DP_RECT( "CCASmileIconGrid popup_grid_graphic_window (parent)",
                    parentRect );

    // listscroll_popup_graphic_pane (this compoment)
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        parentRect,
        AknLayoutScalable_Avkon::listscroll_popup_graphic_pane() );
    LAYOUT_DP_RECT( "CCASmileIconGrid listscroll_popup_graphic_pane",
                    listLayoutRect.Rect() );

    // grid_graphic_popup_pane
    TAknLayoutRect gridLayoutRect;
    gridLayoutRect.LayoutRect(
        listLayoutRect.Rect(),
        AknLayoutScalable_Avkon::grid_graphic_popup_pane( 0 ) );
    LAYOUT_DP_RECT( "CCASmileIconGrid grid_graphic_popup_pane",
                    gridLayoutRect.Rect() );

    // cell_graphic_popup_pane (upper left cell)
    TAknLayoutRect oneCellRect;
    oneCellRect.LayoutRect(
        gridLayoutRect.Rect(),
        AknLayoutScalable_Avkon::cell_graphic_popup_pane( 0, 0, 0 ) );
    LAYOUT_DP_RECT( "CCASmileIconGrid cell_graphic_popup_pane",
                    oneCellRect.Rect() );

    iFirstCell = oneCellRect.Rect();
    iCellWidth = iFirstCell.Width();
    iCellHeight = iFirstCell.Height();

    TAknLayoutRect myIconRect;
    myIconRect.LayoutRect(
        iFirstCell,
        AknLayoutScalable_Avkon::cell_graphic2_pane_g5( 0 ) );

    iIconSize = myIconRect.Rect().Size();
    }

// ---------------------------------------------------------
// CCASmileIconGrid::MinimumSize
// Return minimum size to use the control.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TSize CCASmileIconGrid::MinimumSize()
    {
    TAknLayoutRect gridRect;
    gridRect.LayoutRect(
        iParent->Rect(),
        AknLayoutScalable_Avkon::listscroll_popup_graphic_pane() );

    LAYOUT_DP_RECT( "CCASmileIconGrid::MinimumSize()", gridRect.Rect() );
    return gridRect.Rect().Size();
    }

// ---------------------------------------------------------
// CCASmileIconGrid::SizeChanged
// Control rectangle is set.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconGrid::SizeChanged()
    {
    LAYOUT_DP_RECT( "CCASmileIconGrid::SizeChanged CCoeControl", Rect() );
    SetLayout();

    TInt iconCount( iIconArray.Count() );
    for ( TInt i = 0; i < iconCount; i++  )
        {
        AknIconUtils::SetSize( iIconArray[i]->Bitmap(),
                               iIconSize );

        }
    }

// ---------------------------------------------------------
// CCASmileIconGrid::HandleResourceChange
// Notifier for changing layout
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCASmileIconGrid::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetLayout();
        }
    else
        {
        CCoeControl::HandleResourceChange( aType );
        }
    }


// ---------------------------------------------------------
// CCASmileIconGrid::Draw
// Drawing control.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconGrid::Draw( const TRect& /* aRect */ ) const
    {
    CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );

    if ( !iDragEvent )
        {
        //draw the grid
        gc.SetPenStyle( CGraphicsContext::ESolidPen );
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
        gc.SetPenSize( TSize( 1, 1 ) );
        gc.SetPenColor( AKN_LAF_COLOR( 215 ) );

        // For some reason the dialog does not draw it's background
        // completely, so we'll have to do it here.
        // This should work with bgRect = Rect(), but it doesn't.
        // Change this if you know how it should be done.
        TRect bgRect = iParent->Rect();
        bgRect.iTl.iY = Rect().iTl.iY;
        // ---

        TBool skins = AknsDrawUtils::Background( skin, cc, this, gc, bgRect );

        if ( !skins )
            {
            TRgb color = AKN_LAF_COLOR( 0 );
            AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG1 );
            gc.SetBrushColor( color );
            gc.Clear( bgRect );
            }

        TInt lastRowIconsCount = iIconCount % iMaxColumns;
        if ( lastRowIconsCount == 0 && iIconCount > 0 )
            {
            // last row is full
            lastRowIconsCount = iMaxColumns;
            }

        TInt i( 0 );

        if ( !iIsMirrored )
            {
            //draw horizontal lines
            for ( i = 0; i <= iRowCount; ++i )
                {
                TPoint startPoint( iFirstCell.iTl );
                TPoint endPoint( iFirstCell.iTl );
                startPoint.iY += i * iCellHeight;
                endPoint.iY += i * iCellHeight;
                endPoint.iX += ( ( i == iRowCount ) || ( i == 0 && iRowCount == 1 )
                                 ? ( lastRowIconsCount )
                                 * iCellWidth : iMaxColumns * iCellWidth );
                // add 1 pixel to remove the gap from bottom right corners
                ++endPoint.iX;
                gc.DrawLine( startPoint, endPoint );
                }

            //draw vertical lines
            for ( i = 0; i <= iMaxColumns; ++i )
                {
                TPoint startPoint( iFirstCell.iTl );
                TPoint endPoint( iFirstCell.iTl );
                startPoint.iX += i * iCellWidth;
                endPoint.iX += i * iCellWidth;
                endPoint.iY += ( i <= lastRowIconsCount ?
                                 iCellHeight * iRowCount : iCellHeight * ( iRowCount - 1  ) );
                gc.DrawLine( startPoint, endPoint );
                }
            }
        else
            {
            //draw horizontal lines
            for ( i = 0; i <= iRowCount; ++i )
                {
                TPoint startPoint( iFirstCell.iBr.iX, iFirstCell.iTl.iY );
                TPoint endPoint( iFirstCell.iBr.iX, iFirstCell.iTl.iY );
                startPoint.iY += i * iCellHeight;
                endPoint.iY += i * iCellHeight;
                endPoint.iX -= ( ( i == iRowCount ) || ( i == 0 && iRowCount == 1 )
                                 ? ( lastRowIconsCount )
                                 * iCellWidth : iMaxColumns * iCellWidth );
                // sub 1 pixel to remove the gap from bottom left corners
                --endPoint.iX;
                gc.DrawLine( startPoint, endPoint );
                }

            //draw vertical lines
            for ( i = 0; i <= iMaxColumns; ++i )
                {
                TPoint startPoint( iFirstCell.iBr.iX, iFirstCell.iTl.iY );
                TPoint endPoint( iFirstCell.iBr.iX, iFirstCell.iTl.iY );
                startPoint.iX -= i * iCellWidth;
                endPoint.iX -= i * iCellWidth;
                endPoint.iY += ( i <= lastRowIconsCount ?
                                 iCellHeight * iRowCount : iCellHeight * ( iRowCount - 1  ) );
                gc.DrawLine( startPoint, endPoint );
                }

            }

        //draw icons
        for ( i = 0; i < iIconCount; ++i )
            {
            DrawItem( gc, skins, skin, cc, i, i == iCursorPos );
            }
        }
    else
        {
        // only selection changed, highlight new pos
        DrawItem( gc, cc != NULL, skin, cc, iCursorPos, ETrue );
        // and clear old
        DrawItem( gc, cc != NULL, skin, cc, iPrevCursorPos, EFalse );
        }
    }

// ---------------------------------------------------------
// CCASmileIconGrid::DrawItem
// (other items were commented in a header).
// ---------------------------------------------------------
void CCASmileIconGrid::DrawItem( CWindowGc& aGc,
                                 TBool aSkinEnabled,
                                 MAknsSkinInstance* aSkin,
                                 MAknsControlContext* aSkinCc,
                                 TInt aIndex, TBool aSelected ) const
    {
    //lets count currect cell
    TRect myRect  = iFirstCell;
    TPoint offset;

    if ( !iIsMirrored )
        {
        offset.iX = ( aIndex % iMaxColumns ) * iCellWidth;
        }
    else
        {
        offset.iX = -( ( aIndex % iMaxColumns ) * iCellWidth );
        }

    offset.iY = aIndex / iMaxColumns * iCellHeight;

    myRect.Move( offset );

    // don't draw bg/highlight over borderlines
    TRect myHighLightRect = myRect;
    myHighLightRect.iTl.iX++;
    myHighLightRect.iTl.iY++;

    if ( aSelected )
        {
        // HIGHLIGHT
        if ( !aSkinEnabled )
            {
            aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            }

        TRgb color = AKN_LAF_COLOR( 210 );
        AknsUtils::GetCachedColor( aSkin, color, KAknsIIDQsnIconColors,
                                   EAknsCIQsnComponentColorsCG17 );
        aGc.SetBrushColor( color );
        aGc.Clear( myHighLightRect );
        }
    else
        {
        TRgb color = AKN_LAF_COLOR( 0 );
        AknsUtils::GetCachedColor( aSkin, color, KAknsIIDQsnIconColors,
                                   EAknsCIQsnIconColorsCG1 );
        aGc.SetBrushColor( color );
        if ( aSkinEnabled )
            {
            AknsDrawUtils::Background( aSkin, aSkinCc, aGc, myHighLightRect );
            }
        else
            {
            aGc.DrawRect( myHighLightRect );
            }
        }

    TAknWindowLineLayout myIconLayout( AknLayoutScalable_Avkon::cell_graphic2_pane_g5( 0 ) );

    TAknLayoutRect myIconRect;
    myIconRect.LayoutRect( myRect, myIconLayout );

    if ( aIndex < iIconArray.Count() )
        {
        myIconRect.DrawImage( aGc,
                              iIconArray[ aIndex ]->Bitmap(),
                              iIconArray[ aIndex ]->Mask() );
        }
    }

// ---------------------------------------------------------
// CCASmileIconGrid::HandlePointerEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconGrid::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    if ( !CATouchUtils::PenEnabled() )
        {
        // Ignore event
        return;
        }

    TInt oldSelection = iCursorPos;
    TPoint hitPos = aPointerEvent.iPosition;
    // Pointer useless when using the Hebrew or other Arib language.
    TInt xPos;

    // Convert XY position to linear cursor position
    if ( !iIsMirrored )
        {
        hitPos -= iFirstCell.iTl;

        xPos = hitPos.iX / iCellWidth;
        }
    // If the layout is mirrored, calculate it in another way.
    else
        {
        hitPos.iX -= iFirstCell.iBr.iX;
        hitPos.iY -= iFirstCell.iTl.iY;

        xPos = -hitPos.iX / iCellWidth;
        }
    TInt yPos = hitPos.iY / iCellHeight;
    TInt newSelection = yPos * iMaxColumns + xPos;

    // Validate the item, check whether it is correct.
    TBool validItem;
    if ( !iIsMirrored )
        {
        // Check that the selection is inside the grid
        validItem = xPos <= iMaxColumns - 1 &&
                    Rng( 0, newSelection, iIconCount - 1 ) &&
                    hitPos.iX >= 0 &&
                    hitPos.iY >= 0;
        }
    else
        {
        // Check that the selection is inside the grid
        validItem = xPos <= iMaxColumns - 1 &&
                    Rng( 0, newSelection, iIconCount - 1 ) &&
                    hitPos.iX <= 0 &&
                    hitPos.iY >= 0;
        }
    if ( !validItem )
        {
        // not valid point --> ignore event
        return;
        }

    // Only update if something new was selected
    if ( newSelection != oldSelection )
        {
        // New valid item
        iPrevCursorPos = iCursorPos;
        iCursorPos = newSelection;
        iDragEvent = ETrue;
        DrawDeferred();
        iDragEvent = EFalse;
        }

    // Notify observer
    if ( iTapObserver &&
         aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        // Smiley was tapped
        iTapObserver->HandleTapEventL( MCATapEventObserver::ESingleTap,
                                       iTapControlId );
        }

    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        // enable dragging when button1 is down
        EnableDragEvents();
        // single tap has to insert the smiley.
        //  this code is not required
        // Window().SetPointerGrab( ETrue );

        // and make sure that we get the dragging events
        //   ClaimPointerGrab( ETrue );
        }

    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

// -----------------------------------------------------------------------------
// CCASmileIconGrid::SetTapObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASmileIconGrid::SetTapObserver( MCATapEventObserver* aObserver,
                                       TUint aId )
    {
    iTapObserver = aObserver;
    iTapControlId = aId;
    }

// -----------------------------------------------------------------------------
// CCASmileIconGrid::SetViewableWindowWidth
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASmileIconGrid::SetViewableWindowWidth( TInt aViewableWidth )
    {
    TRect parentRect( iParent->Rect() );
    LAYOUT_DP_RECT( "CCASmileIconGrid popup_grid_graphic_window (parent)",
                    parentRect );

    // listscroll_popup_graphic_pane (this compoment)
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        parentRect,
        AknLayoutScalable_Avkon::listscroll_popup_graphic_pane() );
    LAYOUT_DP_RECT( "CCASmileIconGrid listscroll_popup_graphic_pane",
                    listLayoutRect.Rect() );

    // grid_graphic_popup_pane
    TAknLayoutRect gridLayoutRect;
    gridLayoutRect.LayoutRect(
        listLayoutRect.Rect(),
        AknLayoutScalable_Avkon::grid_graphic_popup_pane( 0 ) );
    LAYOUT_DP_RECT( "CCASmileIconGrid grid_graphic_popup_pane",
                    gridLayoutRect.Rect() );

    // cell_graphic_popup_pane (upper left cell)
    TAknLayoutRect oneCellRect;
    oneCellRect.LayoutRect(
        gridLayoutRect.Rect(),
        AknLayoutScalable_Avkon::cell_graphic_popup_pane( 0, 0, 0 ) );
    LAYOUT_DP_RECT( "CCASmileIconGrid cell_graphic_popup_pane",
                    oneCellRect.Rect() );

    TInt cellWidth = oneCellRect.Rect().Width();

    iMaxColumns = aViewableWidth / cellWidth;

    iRowCount = iIconCount / iMaxColumns;

    if ( iIconCount % iMaxColumns )
        {
        iRowCount++;
        }

    if ( iIconCount < 0 ||
         iRowCount > KMaxGridRowCount )
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CCASmileIconGrid::GetFirstCellRect
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TRect CCASmileIconGrid::GetFirstCellRect()
    {
    // Dim a empty rect.
    TRect aRect;
    aRect.SetRect( 0, 0, 0, 0 );
    // popup_grid_graphic_window (Parent)
    TRect parentRect( aRect );/* iParent->Rect() ); */
    LAYOUT_DP_RECT( "CCASmileIconGrid popup_grid_graphic_window (parent)",
                    parentRect );

    // listscroll_popup_graphic_pane (this compoment)
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        parentRect,
        AknLayoutScalable_Avkon::listscroll_popup_graphic_pane() );
    LAYOUT_DP_RECT( "CCASmileIconGrid listscroll_popup_graphic_pane",
                    listLayoutRect.Rect() );

    // grid_graphic_popup_pane
    TAknLayoutRect gridLayoutRect;
    gridLayoutRect.LayoutRect(
        listLayoutRect.Rect(),
        AknLayoutScalable_Avkon::grid_graphic_popup_pane( 0 ) );
    LAYOUT_DP_RECT( "CCASmileIconGrid grid_graphic_popup_pane",
                    gridLayoutRect.Rect() );

    // cell_graphic_popup_pane (upper left cell)
    TAknLayoutRect oneCellRect;
    oneCellRect.LayoutRect(
        gridLayoutRect.Rect(),
        AknLayoutScalable_Avkon::cell_graphic_popup_pane( 0, 0, 0 ) );
    LAYOUT_DP_RECT( "CCASmileIconGrid cell_graphic_popup_pane",
                    oneCellRect.Rect() );

    return oneCellRect.Rect();
    }
// End of File
