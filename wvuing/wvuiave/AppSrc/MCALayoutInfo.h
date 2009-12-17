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
* Description:  Layout Info interface
*
*/


#ifndef MCALAYOUTINFO_H
#define MCALAYOUTINFO_H

//INCLUDES
#include "TCAChatListBoxLayout.h"

#include <e32base.h>

// FORWARD DECLARATIONS

class CFont;
class TAknWindowLineLayout;
class TAknTextLineLayout;
class TAknMultiLineTextLayout;

// CLASS DECLARATION

/**
 *  Interface for Layout and render info for Chat Listbox's item drawer and view
 *
 *  @since 2.8
 */
class MCALayoutInfo
    {
    public: // Interface

        virtual void SetLayoutID( TChatListBoxLayout::TLayout aLayout ) = 0;

        virtual TInt LayoutID() = 0;

        virtual TBool Mirrored() = 0;

        virtual const CFont* NickFont() = 0;

        virtual const CFont* MessageFont() = 0;

        virtual TInt LineHeight() = 0;

        virtual TInt MaxLineWidth() = 0;

        virtual TInt MaxNickWidth() = 0;

        virtual TInt MaxLines() = 0;

        virtual TRect MainPaneRect() = 0;

        virtual TInt Indent() = 0;

        virtual TRect ScreenRect() = 0;

        virtual void SetEditorVisibility( TBool aVisible ) = 0;

        virtual TBool EditorVisibility() = 0;

        virtual TAknWindowLineLayout im_reading_pane( TInt aIndex_H ) = 0;

        virtual TAknWindowLineLayout im_reading_field( TInt aIndex_t ) = 0;

        virtual TAknWindowLineLayout IM_text_elements( TInt aLineIndex ) = 0;

        virtual TAknTextLineLayout IM_reading_pane_texts_Line_1(
            TInt aCommon1 ) = 0;

        virtual TAknTextLineLayout IM_reading_pane_texts_Line_2() = 0;

        virtual TAknWindowLineLayout IM_text_elements_Line_1() = 0;

        virtual TAknWindowLineLayout IM_text_elements_Line_2() = 0;

        virtual TAknWindowLineLayout IM_reading_field_highlight_graphics_Line_1(
            const TRect& aParentRect ) = 0;

        virtual TAknWindowLineLayout IM_reading_field_highlight_graphics_Line_2(
            const TRect& aParentRect ) = 0;

        virtual TAknWindowLineLayout im_writing_pane( TInt aCommon1 ) = 0;

        virtual TAknWindowLineLayout IM_writing_field_elements_Line_1(
            TInt aIndex_H ) = 0;

        virtual TAknWindowLineLayout IM_writing_field_elements_Line_2() = 0;

        virtual TAknWindowLineLayout IM_writing_field_elements_Line_3(
            TInt aIndex_H ) = 0;

        virtual TAknMultiLineTextLayout Multiline_IM_writing_pane_texts_Line_1(
            TInt aNumberOfLinesShown ) = 0;

        virtual TAknTextLineLayout IM_navi_pane_texts_Line_1(
            TInt aIndex_C, TInt aIndex_W ) = 0;

        virtual void DPLayout( const TDesC& aStr,
                               const TAknTextLineLayout aLayout ) = 0;

        virtual void DPLayout( const TDesC& aStr,
                               const TAknWindowLineLayout aLayout ) = 0;

        virtual void DPRect( const TDesC& aStr, const TRect& aRect ) = 0;
    protected:

        virtual ~MCALayoutInfo() {};

    };

#endif      // MCALAYOUTINFO_H

// End of File
