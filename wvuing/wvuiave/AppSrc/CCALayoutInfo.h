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
* Description:  Utility for selecting smile icon and converting
*                between smile id and smile string.
*
*/


#ifndef CCALAYOUTINFO_H
#define CCALAYOUTINFO_H

// INCLUDES

#include "MCALayoutInfo.h"
#include "TCAChatListBoxLayout.h"
#include "MCALayoutChangeObserver.h"

#include <e32base.h>
#include <AknLayout2Def.h>
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Layout and render info for Chat Listbox's item drawer and view
*  Helps to calculate and draw items.
*
*  @lib chat.app
*  @since 2.8
*/
class CCALayoutInfo : public CBase,
            public MCALayoutInfo,
            public MCALayoutChangeObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor using default smile icon array.
        */
        static CCALayoutInfo* NewL( TChatListBoxLayout::TLayout aLayoutID );

        /**
        * Destructor.
        */
        virtual ~CCALayoutInfo();

    public: // New methods


    public: // from MCALayoutInfo

        void SetLayoutID( TChatListBoxLayout::TLayout aLayout );

        TInt LayoutID();

        TBool Mirrored();

        const CFont* NickFont();

        const CFont* MessageFont();

        TInt LineHeight();

        TInt MaxLineWidth();

        TInt MaxNickWidth();

        TInt MaxLines();

        TRect MainPaneRect();

        TInt Indent();

        TRect ScreenRect();

        void SetEditorVisibility( TBool aVisible );

        TBool EditorVisibility();

        TAknWindowLineLayout im_reading_pane( TInt aIndex_H );

        TAknWindowLineLayout im_reading_field( TInt aIndex_t );

        TAknWindowLineLayout IM_text_elements( TInt aLineIndex );

        TAknTextLineLayout IM_reading_pane_texts_Line_1( TInt aCommon1 );

        TAknTextLineLayout IM_reading_pane_texts_Line_2();

        TAknWindowLineLayout IM_text_elements_Line_1();

        TAknWindowLineLayout IM_text_elements_Line_2();

        TAknWindowLineLayout IM_reading_field_highlight_graphics_Line_1(
            const TRect& aParentRect );

        TAknWindowLineLayout IM_reading_field_highlight_graphics_Line_2(
            const TRect& aParentRect );

        TAknWindowLineLayout im_writing_pane( TInt aCommon1 );

        TAknWindowLineLayout IM_writing_field_elements_Line_1( TInt aIndex_H );

        TAknWindowLineLayout IM_writing_field_elements_Line_2();

        TAknWindowLineLayout IM_writing_field_elements_Line_3( TInt aIndex_H );

        TAknMultiLineTextLayout Multiline_IM_writing_pane_texts_Line_1(
            TInt aNumberOfLinesShown );

        TAknTextLineLayout IM_navi_pane_texts_Line_1( TInt aIndex_C, TInt aIndex_W );

        void DPLayout( const TDesC& aStr, const TAknTextLineLayout aLayout );

        void DPLayout( const TDesC& aStr, const TAknWindowLineLayout aLayout );

        void DPRect( const TDesC& aStr, const TRect& aRect );
    public: // from MCALayoutChangeObserver

        virtual void LayoutChangedL( TInt aType = 0 );

    private:  // Constructors and destructor

        /**
        * C++ constructor.
        */
        CCALayoutInfo( TChatListBoxLayout::TLayout aLayoutID );

        /**
        * Second-phased constructor
        */
        void ConstructL();

    private:	// New methods

        void LogLayoutL();

        void FillLayoutL();

        void FillReadingPaneL();

        void FillReadingFieldL();

        void FillReadingPaneTextsLine1L();

        void FillReadingPaneTextsLine2();

        void FillIMTextElementsL();

        void FillWrittingPaneTextsLineL();

        void FillIMWritingFieldLine1L();

        void FillIMWritingFieldLine2();

        void FillIMWritingFieldLine3L();

        void FillWrittingPaneL();

        void FillIMNaviPaneTextsLineL();

        void SetupMainPaneRect();

    private: // Data

        TChatListBoxLayout::TLayout iLayoutID;

        TBool iMirrored;

        // doesn't own
        const CFont* iNickFont;

        const CFont* iMsgFont;

        TInt iLineHeight;

        TInt iMaxLineWidth;

        TInt iMaxNickWidth;

        TInt iMaxLines;

        TRect iMainPaneRect;

        TRect iScreenRect;

        TRect iLastScreenRect;

        TInt iIconSkip;

        TInt iIndent;

        CArrayFixFlat<TAknWindowLineLayout>* iReadingPane;

        CArrayFixFlat<TAknWindowLineLayout>* iWrittingPane;

        CArrayFixFlat<TAknWindowLineLayout>* iReadingField;

        CArrayFixFlat<TAknTextLineLayout>* iReadingPaneTextsLine1;

        CArrayFixFlat<TAknWindowLineLayout>* iIMTextElements;

        TAknTextLineLayout iReadingPaneTextsLine2;

        CArrayFixFlat<TAknMultiLineTextLayout>* iWrittingPaneTextsLine;

        CArrayFixFlat<TAknWindowLineLayout>* iIMWritingFieldLine1;

        TAknWindowLineLayout iIMWritingFieldLine2;

        CArrayFixFlat<TAknWindowLineLayout>* iIMWritingFieldLine3;

        CArrayFixFlat<TAknTextLineLayout>* iIMNaviPaneTextsLine1;

        CArrayFixFlat<TAknTextLineLayout>* iIMNaviPaneTextsLine2;

        TBool iEditorVisibility;
    };

#endif  // CCALAYOUTINFO_H
