/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef CCATEXTVIEWCONTROL_H
#define CCATEXTVIEWCONTROL_H

//  INCLUDES
#include <coecntrl.h>
#include <eiksbobs.h> // MEikScrollBarObserver
#include <eikbctrl.h>
#include <frmtlay.h> // TCursorSelection
#include "MCATextView.h"


// FORWARD DECLARATIONS
class CTextLayout;
class CTextView;
class CCACustomDraw;
class CEikScrollBarFrame;
class CAknsBasicBackgroundControlContext;


// CLASS DECLARATION

/**
*  Text viewer component
*
*  @lib chat.app
*  @since 3.0
*/
class CCATextViewControl : public CCoeControl
            , public MCATextView
            , public MEikScrollBarObserver
    {

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aRect Draw area
         * @param aParent Parent control
         * @param aTextLayout Text to be viewed
         * @return Instance of this class
         */
        static CCATextViewControl* NewL( const TRect& aRect,
                                         const CCoeControl& aParent,
                                         MLayDoc* aTextLayout );

        /**
         * Destructor.
         */
        virtual ~CCATextViewControl();

    protected:  // New functions

        /**
         * Updates scrollbars
         */
        void UpdateScrollBarsL( TBool aScrollDraw = ETrue );

        /**
         * Update the view
         */
        void UpdateViewL();

        /**
         * Set highlight state
         */
        void SetHighlighted( TBool aIsHighlighted );

        /**
         * Get scrollbars
         */
        const CEikScrollBarFrame* ScrollBarFrame();

        /**
         * Finds the document position which is located at aPoint
         * @param aPoint Location to find
         * @return Document position or
         *         KErrNotFound if not found
         */
        TInt FindDocPos( TPoint aPoint );

    public: // from MObjectProvider

        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

    protected: // from MEikScrollBarObserver

        void HandleScrollEventL( CEikScrollBar* aScrollBar,
                                 TEikScrollEvent aEventType );

    protected:  // Functions from CCoeControl

        /**
         * @see CCoeControl
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
         * @see CCoeControl
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

        /**
         * @see CCoeControl
         */
        void Draw( const TRect& aRect ) const;

        /**
         * @see CCoeControl
         */
        void SizeChanged();

        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;

    public:// Functions from MCATextView
        /**
         * @see MCATextView
         */
        TBool IsVisible( TInt aPos );


    protected:  // Functions from MCATextView

        /**
         * @see MCATextView
         */
        void HandleFormatChangedL( TCursorSelection& aChanged,
                                   TBool aPreserverFocus = EFalse );

        /**
         * @see MCATextView
         */
        void HandleInsertDeleteL( TCursorSelection& aChanged,
                                  TInt aDeletedChars );

        /**
         * @see MCATextView
         */
        void HandleAdditionL( TBool aFirst,
                              TCursorSelection& aSelection,
                              TBool aTop  );

        /**
         * @see MCATextView
         */
        void ScrollVisibleL( TCursorSelection& aSelection, TBool aTop );

        /**
         * @see MCATextView
         */
        TCursorSelection ScrollSelection();

        /**
         * @see MCATextView
         */
        void ScrollLinesL( TInt aAmount );


        /**
         * @see MCATextView
         */
        TBool ParagraphFits( TInt aPos );

        /**
         * @see MCATextView
         */
        void Redraw();

        /**
         * @see MCATextView
         */
        void SetPenBehaviour( TPenBehaviour aPen );

        /**
         * @see MCATextView
         */
        MCATextView::TPenBehaviour PenBehaviour();

        /**
         * @see MCATextView
         */
        void ProcessStylusEventL( const TPointerEvent& aPointerEvent );

        /**
         * @see MCATextView
         */
        void HandleGlobalChangeNoRedrawL(
            TViewYPosQualifier aYPosQualifier = TViewYPosQualifier() );

    protected:

        /**
        * C++ default constructor.
        */
        CCATextViewControl();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aRect Draw area
        * @param aParent Parent control
        * @param aTextLayout Text to be viewed
        */
        void ConstructL( const TRect& aRect, const CCoeControl& aParent,
                         MLayDoc* aTextLayout );


    private:    // Data

        // Owns. rectangle in which to view text
        TRect iViewRect;

        // Owns. text layout
        CTextLayout* iLayout;

        // Owns. text view
        CTextView* iTextView;

        // Owns. custom drawer for skins
        CCACustomDraw* iCustomDraw;

        // Owns. scroll bar container
        CEikScrollBarFrame* iScrollBar;

        // Tells whether the contorl is added to Stack or not
        TBool iAddedToStack;

        //owns
        CAknsBasicBackgroundControlContext* iBgContext; // Skin background control context

        // Own. Clipping region to prevent flickering
        RRegion* iClipping;

        // Highlight state inside text view
        TBool iIsHighlighted;

        // Determines how this contol reacts to pointer events
        TPenBehaviour iPenType;
    };

#endif      // CCATEXTVIEWCONTROL_H

// End of File

