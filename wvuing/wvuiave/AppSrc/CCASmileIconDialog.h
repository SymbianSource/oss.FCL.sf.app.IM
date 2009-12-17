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


#ifndef CCASMILEICONDIALOG_H
#define CCASMILEICONDIALOG_H

// INCLUDES
#include "mcatapeventobserver.h"
#include <AknDialog.h>
#include <aknlists.h>

// FORWARD DECLARATIONS
class CGulIcon;

// CLASS DECLARATION

/**
* CCASmileIconDialog
*
* Dialog for selecting smile icon.
*/
class CCASmileIconDialog : public CAknDialog,
            public MCATapEventObserver
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aIconArray      : Reference to smile icon array.
        * @param aSelectedIconId : For getting selected bitmap number.
        */
        static CCASmileIconDialog* NewL(
            const RPointerArray<CGulIcon>& aIconArray,
            TInt& aSelectedIconId );

        /**
        * Destructor.
        */
        virtual ~CCASmileIconDialog();

    private:  // New functions

        /**
        * C++ constructor.
        */
        CCASmileIconDialog( const RPointerArray<CGulIcon>& aIconArray,
                            TInt& aSelectedIconId );

    private: // Functions from base classes

        /**
        * From CEikDialog : Handle key events.
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CEikDialog : Prepare for opening dialog.
        */
        void HandleResourceChange( TInt aType );

        /**
        * From CEikDialog : Handle CBA-keys.
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From CEikDialog : Prepare for opening dialog.
        */
        void PreLayoutDynInitL();

        /**
        * From CCoeControl : Control size is set.
        */
        void SetSizeAndPosition( const TSize& aSize );

        /**
        * From CCoeControl : Create custom control.
        */
        SEikControlInfo CreateCustomControlL( TInt aControlType );

        /**
        * From CCoeControl : Event handler
        */
        void HandleDialogPageEventL( TInt aEventID );

        /**
        * Check if dialog is suitable for Smile Icon.
        * @param aDialogRect Dialog inside which smile are shown.
        * @param aRect Rectangle for top left smile icon.
        * @return ViewableArea if all smile can fit in viewable area other wise return 0.
        */
        TInt CheckDialog( TAknLayoutRect aDialogRect, TRect aRect );

    private: // From MCATapEventObserver

        void HandleTapEventL( TTapEvent aEvent, TUint aControlId );

    private: // helpers

        /**
        * Set dialog layout.
        */
        void SetLayout();

        /**
        * Calculates variety used for popup_grid_graphic_window
        * according to rows in grid.
        * @return Variety value.
        */
        TInt CalculatePopupVariety();


    private:

        // Reference to smile icon array.
        const RPointerArray<CGulIcon>& iIconArray;

        // Selected icon id reference.
        TInt&  iIconId;

        // Has user selected a smiley with pen
        TBool iSmileyPointed;

        // Whether the layout is mirrored or not.
        TBool iIsMirrored;
    };

#endif  // CCASMILEICONDIALOG_H
