/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Dialog for selecting smile icon.

*
*/




// INCLUDE FILES

#include "CCASmileIconDialog.h"

#include "CCASmileIconGrid.h"

#include "IMSmileUtilityNG.hrh"

#include "layoutdebugprint.h"

#include "catouchutils.h"



#include <akndef.h>

#include <aknappui.h>

#include <aknutils.h>

#include <aknconsts.h>

#include <aknborders.h>

#include <AknsDrawUtils.h>

#include <AknLayoutScalable_Avkon.cdl.h>

#include <eikbtgpc.h>





// CONSTANTS

const TInt KAknSctCBaButtonDirections = 3; // bottom, right and left





// ================= MEMBER FUNCTIONS =======================





// Two-phased constructor.

CCASmileIconDialog* CCASmileIconDialog::NewL(

    const RPointerArray<CGulIcon>& aIconArray,

    TInt& aSelectedIconId )

    {

    CCASmileIconDialog* self =

        new ( ELeave ) CCASmileIconDialog( aIconArray, aSelectedIconId );

    return self;

    }



// Destructor

CCASmileIconDialog::~CCASmileIconDialog()

    {

    }



// C++ constructor can NOT contain any code, that

// might leave.

//

CCASmileIconDialog::CCASmileIconDialog(

    const RPointerArray<CGulIcon>& aIconArray,

    TInt& aSelectedIconId )

        : iIconArray( aIconArray ), iIconId( aSelectedIconId ), iIsMirrored( EFalse )

    {

    }



// ---------------------------------------------------------

// CCASmileIconDialog::OfferKeyEventL

// Handle key event.

// If key code is other than OK and arrow key, no action.

// (other items were commented in a header).

// ---------------------------------------------------------

//

TKeyResponse CCASmileIconDialog::OfferKeyEventL(

    const TKeyEvent& aKeyEvent, TEventCode aType )

    {

    TKeyResponse response( EKeyWasConsumed );



    switch ( aKeyEvent.iCode )

        {

            // fall through

        case EKeyLeftArrow:

        case EKeyRightArrow:

        case EKeyUpArrow:

        case EKeyDownArrow:

            {

            CCASmileIconGrid* grid =

                static_cast< CCASmileIconGrid*>(

                    Control( EIMIdSelectSmileGrid ) );

            grid->MoveCursor( aKeyEvent.iCode );

            break;

            }

//      case EKeyEscape: // handle dialog shutdown. let it go in its default way
        case EKeyEnter:
            {

            TryExitL( EAknSoftkeySelect );

            return EKeyWasConsumed;

            }

        default:

            {

            response = CAknDialog::OfferKeyEventL( aKeyEvent, aType );

            break;

            }

        }

    return response;

    }

// ---------------------------------------------------------

// CCASmileIconDialog::OkToExitL

// Called when CBA-key or OK-key is pressed.

// (other items were commented in a header).

// ---------------------------------------------------------

//

void CCASmileIconDialog::HandleResourceChange( TInt aType )

    {

    if ( aType == KEikDynamicLayoutVariantSwitch )

        {

        SetLayout();

        }

    CAknDialog::HandleResourceChange( aType );

    }



// ---------------------------------------------------------

// CCASmileIconDialog::OkToExitL

// Called when CBA-key or OK-key is pressed.

// (other items were commented in a header).

// ---------------------------------------------------------

//

TBool CCASmileIconDialog::OkToExitL( TInt aButtonId )

    {

    if ( aButtonId == EAknSoftkeySelect ||

         aButtonId == EEikBidOk )

        {

        CCASmileIconGrid* grid =

            static_cast< CCASmileIconGrid*>(

                Control( EIMIdSelectSmileGrid ) );

        iIconId = grid->SelectedBitmapId();
        return ETrue;
        }

    return EFalse;

    }



// ---------------------------------------------------------

// CCASmileIconDialog::PreLayoutDynInitL

// Prepare for drawing dialog.

// (other items were commented in a header).

// ---------------------------------------------------------

//

void CCASmileIconDialog::PreLayoutDynInitL()

    {

    SetLineNonFocusing( EIMIdSelectSmileLabel );

    EnableDragEvents();

    }



// ---------------------------------------------------------

// CCASmileIconDialog::SetSizeAndPosition

// Control size is set. This is called just after PreLayoutDynInitL().

// (other items were commented in a header).

// ---------------------------------------------------------

//

void CCASmileIconDialog::SetSizeAndPosition( const TSize& /*aSize*/ )

    {

    SetLayout();

    }



// ---------------------------------------------------------

// CCASmileIconDialog::CalculatePopupVariety

// (other items were commented in a header).

// ---------------------------------------------------------

//

TInt CCASmileIconDialog::CalculatePopupVariety()

    {

    CCASmileIconGrid* grid =

        static_cast< CCASmileIconGrid*>(

            Control( EIMIdSelectSmileGrid ) );



    TAknLayoutScalableParameterLimits smileyDialogVariety =

        AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();

    TInt maxVariety = smileyDialogVariety.LastVariety();



    // CBA location affects popup layout

    AknLayoutUtils::TAknCbaLocation cbaLocation = AknLayoutUtils::CbaLocation();



    // bottom variety

    TInt maxVarietyOffset = 0;

    TInt varietyOffset = maxVariety + 1;



    // right variety

    if ( cbaLocation == AknLayoutUtils::EAknCbaLocationRight )

        {

        varietyOffset = ( maxVariety + 1 ) / KAknSctCBaButtonDirections;

        maxVarietyOffset = varietyOffset;

        }



    // left variety

    else if ( cbaLocation == AknLayoutUtils::EAknCbaLocationLeft )

        {

        varietyOffset = ( maxVariety + 1 ) / KAknSctCBaButtonDirections;

        maxVarietyOffset = varietyOffset + varietyOffset;

        }

    if ( ( maxVarietyOffset + varietyOffset ) > maxVariety )
        return maxVariety;
    else
        return maxVarietyOffset + varietyOffset;
    }



// ---------------------------------------------------------

// CCASmileIconDialog::SetLayout

// Set dialog layout.

// (other items were commented in a header).

// ---------------------------------------------------------

//

void CCASmileIconDialog::SetLayout()

    {

    iIsMirrored = AknLayoutUtils::LayoutMirrored() ;
    TRect clientRect = iAvkonAppUi->ApplicationRect();

    //check LAF docs for constants

    TAknLayoutRect mainPane;

    mainPane.LayoutRect( clientRect, AknLayoutScalable_Avkon::main_pane( 6 ) );

    LAYOUT_DP_RECT( "CCASmileIconDialog main_pane( 6 )",

                    mainPane.Rect() );

    CCASmileIconGrid* grid =

        static_cast< CCASmileIconGrid*>(

            Control( EIMIdSelectSmileGrid ) );

    // Select correct popup layout

    TInt varietyNumber = CalculatePopupVariety();
    TRect rect = grid->GetFirstCellRect();
    TInt iconCount = iIconArray.Count();
    TInt areaOfIcon = iconCount * rect.Width() * rect.Height();
    TInt variety = varietyNumber;
    TInt viewableWidth = 0;

    for ( TInt i = varietyNumber; i > 0; i-- )
        {

        TAknWindowLineLayout popupGridDialogLayout(
            AknLayoutScalable_Avkon::popup_grid_graphic_window( i ) );


        TAknLayoutRect dialogRect;

        dialogRect.LayoutRect(
            mainPane.Rect(), popupGridDialogLayout );

        viewableWidth = CheckDialog( dialogRect, rect );
        if ( viewableWidth )
            {
            varietyNumber = i;
            break;
            }
        }

    if ( viewableWidth )
        {
        TAknWindowLineLayout popupGridDialogLayout(
            AknLayoutScalable_Avkon::popup_grid_graphic_window( varietyNumber ) );

        TAknLayoutRect dialogRect;

        dialogRect.LayoutRect(
            mainPane.Rect(), popupGridDialogLayout );

        LAYOUT_DP_RECT( "CCASmileIconDialog popup_grid_graphic_window",
                        dialogRect.Rect() );

        grid->SetViewableWindowWidth( viewableWidth );

        SetRect( dialogRect.Rect() );
        }
    // If there is no suitable solution for the smiley dialog layout
    else
        {
        // Use the max variety number
        varietyNumber = CalculatePopupVariety();

        TAknWindowLineLayout popupGridDialogLayout(
            AknLayoutScalable_Avkon::popup_grid_graphic_window( varietyNumber ) );

        TAknLayoutRect dialogRect;

        dialogRect.LayoutRect(
            mainPane.Rect(), popupGridDialogLayout );

        LAYOUT_DP_RECT( "CCASmileIconDialog popup_grid_graphic_window",
                        dialogRect.Rect() );

        if ( !iIsMirrored )
            {
            // Right Margin of the top left (top right) icon's X
            TInt leftMargin = rect.iTl.iX;

            // Use the max viewable width
            viewableWidth = dialogRect.Rect().iBr.iX - dialogRect.Rect().iTl.iX - leftMargin;
            }
        else
            {
            // Left Margin of the top left (top right) icon's X
            TInt rightMargin = rect.iBr.iX;

            // Use the max viewable width
            viewableWidth = dialogRect.Rect().iBr.iX - dialogRect.Rect().iTl.iX + rightMargin;
            }

        grid->SetViewableWindowWidth( viewableWidth );

        if ( !iIsMirrored )
            SetRect( dialogRect.Rect() );
        // When the Hebrew is the language as well as the Arb languages
        // The align is not right
        else
            {
            TRect mirroredDialogRect;

            // Align to right.
            mirroredDialogRect = dialogRect.Rect();

            // Get the rect of the cba pane.
            TRect controlPaneRect;
            AknLayoutUtils::LayoutMetricsRect(
                AknLayoutUtils::EControlPane, controlPaneRect );

            TInt cbaWidth = controlPaneRect.iBr.iX - controlPaneRect.iTl.iX;

            mirroredDialogRect.iTl.iX -= cbaWidth;
            mirroredDialogRect.iBr.iX -= cbaWidth;

            SetRect( mirroredDialogRect );
            }
        }
    }

// ---------------------------------------------------------

// CCASmileIconDialog::CheckDialog

// Check if Dialog is suitable to fit all smile icon

// (other items were commented in a header).

// ---------------------------------------------------------

TInt CCASmileIconDialog::CheckDialog( TAknLayoutRect aDialogRect, TRect aRect )
    {
    CCASmileIconGrid* grid = static_cast< CCASmileIconGrid* >( Control( EIMIdSelectSmileGrid ) );

    TInt topMargin = aRect.iTl.iY;

    TInt viewableWidth = 0;

    if ( !iIsMirrored )
        {
        TInt leftMargin = aRect.iTl.iX;

        viewableWidth = aDialogRect.Rect().iBr.iX - aDialogRect.Rect().iTl.iX - 2 * leftMargin;
        }
    else
        {
        TInt rightMargin = aRect.iBr.iX;

        viewableWidth = aDialogRect.Rect().iBr.iX - aDialogRect.Rect().iTl.iX + 2 * rightMargin;
        }
    TInt viewableHight = aDialogRect.Rect().iBr.iY - aDialogRect.Rect().iTl.iY - topMargin;

    TInt viewableArea = viewableWidth * viewableHight;

    TInt iconCount = iIconArray.Count();
    TInt areaOfIcon = iconCount * aRect.Width() * aRect.Height();

    if ( areaOfIcon > viewableArea )
        {
        return 0;
        }
    else
        {
        return viewableWidth;
        }
    }

// ---------------------------------------------------------

// CCASmileIconDialog::CreateCustomControlL

// Create CCASmileIconGrid class in order to display

// icons in grid.

// This is called when dialog is created.

// (other items were commented in a header).

// ---------------------------------------------------------

//

SEikControlInfo CCASmileIconDialog::CreateCustomControlL(

    TInt aControlType )

    {

    SEikControlInfo controlInfo;

    controlInfo.iControl = NULL;

    controlInfo.iTrailerTextId = 0;

    controlInfo.iFlags = 0;



    if ( aControlType == EIMCtrlSelectSmileGrid )

        {

        controlInfo.iControl = CCASmileIconGrid::NewL( this, iIconArray );

        static_cast<CCASmileIconGrid*>

        ( controlInfo.iControl )->SetTapObserver( this, 0 );

        }

    return controlInfo;

    }



// ---------------------------------------------------------

// CCASmileIconDialog::HandleDialogPageEventL

// (other items were commented in a header).

// ---------------------------------------------------------

//

void CCASmileIconDialog::HandleDialogPageEventL( TInt aEventID )

    {
    if ( CATouchUtils::PenEnabled() )
        {
        if ( iSmileyPointed &&
             aEventID == MEikDialogPageObserver::EDialogPageTapped )
            {
            // Exit requested, exit with ok.
            TryExitL( EAknSoftkeyOk );
            }
        //else keep dialog as it is do not exit.
        }
    }

// ---------------------------------------------------------

// CCASmileIconDialog::HandleTapEventL

// (other items were commented in a header).

// ---------------------------------------------------------

//

void CCASmileIconDialog::HandleTapEventL( TTapEvent aEvent,

                                          TUint /*aControlId*/ )

    {

    // User tapped on smiley.

    if ( aEvent == MCATapEventObserver::ESingleTap )

        {

        // Smiley was selected but we can't exit directly from here.

        // Instead we put the flag on and wait for

        // HandleDialogPageEventL.

        iSmileyPointed = ETrue;

        }

    }



// End of File
