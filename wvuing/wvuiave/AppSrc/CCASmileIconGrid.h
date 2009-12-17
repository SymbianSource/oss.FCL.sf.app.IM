/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Grid for smile icons.
*
*/


#ifndef CCASMILEICONGRID_H
#define CCASMILEICONGRID_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CEikDialog;
class CGulIcon;
class TAknLayoutRect;
class MCATapEventObserver;
class MAknsSkinInstance;
class MAknsControlContext;

// CLASS DECLARATION
/**
*  CCASmileIconGrid
*
*  Grid for icons.
*/
class CCASmileIconGrid : public CCoeControl
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aParent         : Parent control of grid.
        * @param aIconArray      : Reference to smile icon array.
        */
        static CCASmileIconGrid* NewL( CEikDialog* aParent,
                                       const RPointerArray<CGulIcon>& aIconArray );

        /**
        * Destructor.
        */
        virtual ~CCASmileIconGrid();

    public: // New methods

        /**
        * Move cursor
        * @param aKeyCode : EKeyLeftArrow | EKeyRightArrow |
        *                   EKeyUpArrow | EKeyDownArrow
        */
        void MoveCursor( TInt aKeyCode );

        /**
        * Return selected cursor position.
        * @return : Selected cursor position if cursor is pointed a bitmap.
        *           KPinbEmptySpecialCharId if cursor is pointed empty cell.
        */
        TInt SelectedBitmapId();

        /**
        * Return row count of grid.
        * @return : Row count.
        */
        TInt HeightInRows();

        /**
         * Set observer for handling tap events.
         * Supports only single observer.
         * @param aObserver observer to handle tap events
         * @param aId control id which is used by the observer to
         *        identify this object.
         */
        void SetTapObserver( MCATapEventObserver* aObserver, TUint aId );

        /**
         * Calculate number of row and columns in smile grid
         * aViewableWidth Width of Dialog in which smile are shown
         */
        void SetViewableWindowWidth( TInt aViewableWidth );

        /**
         *@return First smile icon rectangle in smile icon grid.
         */
        TRect GetFirstCellRect();


    private: // New methods

        /**
        * Set layout of identifier grid
        */
        void SetLayout();

        /**
        * Draws one item
        */
        void DrawItem( CWindowGc& aGc,
                       TBool aSkinEnabled,
                       MAknsSkinInstance* aSkin,
                       MAknsControlContext* aSkinCc,
                       TInt aIndex, TBool aSelected ) const;

    private: // Functions from base classes

        /**
        * From CCoeControl : Return minimum size of the control.
        */
        TSize MinimumSize();

        /**
        * From CCoeControl : Size of the control is set.
        */
        void SizeChanged();

        /**
        * From CCoeControl : Notifier for changing layout
        * @param aType : Type of resource change
        */
        void HandleResourceChange( TInt aType );

        /**
        * From CCoeControl : Drawing control.
        * @param aRect : Rectangle of control
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl : Touch support.
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );


    private: // Constructors and destructor

        /**
        * default constructor.
        * @param aParent : Parent control of grid.
        * @param aCountOfIcons : Count of icons
        */
        CCASmileIconGrid( CEikDialog* aParent,
                          const RPointerArray<CGulIcon>& aIconArray );

        /**
        * Second-phased constructor.
        */
        void ConstructL();

    private: // Data

        TInt iCursorPos;
        TInt iPrevCursorPos;
        TBool iDragEvent;

        CEikDialog* iParent; // Not own

        // Reference to smile icon array.
        const RPointerArray<CGulIcon>& iIconArray;

        // Counts
        TInt iIconCount;
        TInt iRowCount;

        TRect iFirstCell;
        TSize iIconSize;
        TInt  iCellWidth;
        TInt  iCellHeight;

        // Not owned. Pointer to observer
        MCATapEventObserver* iTapObserver;

        // ID which needs to be reported back to TapObserver
        TUint iTapControlId;
        TBool iIsMirrored ;

        TInt iMaxColumns;
    };

#endif // CCASMILEICONGRID_H

// End of File
