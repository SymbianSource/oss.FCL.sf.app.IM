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
* Description:  Layout and render info for Chat Listbox's item drawer and view
*
*
*/


// INCLUDE FILES
#include	"CCALayoutInfo.h"
#include    "LayoutDebugPrint.h"

#include	<AknUtils.h>
#include	<AppLayout.cdl.h> //cdl
#include	<F32FILE.H>
#include    <AknLayoutScalable_Apps.cdl.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCALayoutInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCALayoutInfo* CCALayoutInfo::NewL( TChatListBoxLayout::TLayout aLayoutID )
    {
    CCALayoutInfo* self = new ( ELeave ) CCALayoutInfo( aLayoutID );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCALayoutInfo::~CCALayoutInfo
// Destructor
// -----------------------------------------------------------------------------
//
CCALayoutInfo::~CCALayoutInfo()
    {
    if ( iReadingPane )
        {
        iReadingPane->Reset();
        delete iReadingPane;
        }

    if ( iWrittingPane )
        {
        iWrittingPane->Reset();
        delete iWrittingPane;
        }

    if ( iReadingField )
        {
        iReadingField->Reset();
        delete iReadingField;
        }

    if ( iReadingPaneTextsLine1 )
        {
        iReadingPaneTextsLine1->Reset();
        delete iReadingPaneTextsLine1;
        }

    if ( iIMTextElements )
        {
        iIMTextElements->Reset();
        delete iIMTextElements;
        }

    if ( iWrittingPaneTextsLine )
        {
        iWrittingPaneTextsLine->Reset();
        delete iWrittingPaneTextsLine;
        }

    if ( iIMWritingFieldLine1 )
        {
        iIMWritingFieldLine1->Reset();
        delete iIMWritingFieldLine1;
        }

    if ( iIMWritingFieldLine3 )
        {
        iIMWritingFieldLine3->Reset();
        delete iIMWritingFieldLine3;
        }

    if ( iIMNaviPaneTextsLine1 )
        {
        iIMNaviPaneTextsLine1->Reset();
        delete iIMNaviPaneTextsLine1;
        }

    if ( iIMNaviPaneTextsLine2 )
        {
        iIMNaviPaneTextsLine2->Reset();
        delete iIMNaviPaneTextsLine2;
        }
    }

// -----------------------------------------------------------------------------
// CCALayoutInfo::CCALayoutInfo
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCALayoutInfo::CCALayoutInfo( TChatListBoxLayout::TLayout aLayoutID ) :
        iLayoutID( aLayoutID )
    {
    }

// -----------------------------------------------------------------------------
// CCALayoutInfo::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CCALayoutInfo::ConstructL()
    {
    SetupMainPaneRect();
    FillLayoutL();
    LogLayoutL();
    }

// ---------------------------------------------------------
// CCALayoutInfo::LayoutID
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCALayoutInfo::LayoutID()
    {
    return iLayoutID;
    }

// ---------------------------------------------------------
// CCALayoutInfo::Mirrored
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCALayoutInfo::Mirrored()
    {
    return iMirrored;
    }

// ---------------------------------------------------------
// CCALayoutInfo::LayoutChangedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::LayoutChangedL( TInt /*aType = 0*/ )
    {
    LAYOUT_DP_TXT( "CCALayoutInfo::LayoutChangedL" );

    FillLayoutL();

    TAknLayoutRect myLayoutRect;

    TAknLayoutRect myLine;

    iMirrored = AknLayoutUtils::LayoutMirrored();

    //update MainPaneRect
    SetupMainPaneRect();

    TAknWindowLineLayout readingPane = im_reading_pane( iLayoutID );

    myLayoutRect.LayoutRect( iMainPaneRect, readingPane );

    //line height
    TAknWindowLineLayout lineLayout = im_reading_field( 0 );

    myLine.LayoutRect( myLayoutRect.Rect(), lineLayout );

    iLineHeight = myLine.Rect().Height();

    LAYOUT_DP( D_LAYOUT_LIT( "CCALayoutInfo::LayoutChangedL iLineHeight:%d" )
               , iLineHeight );

    //Max Lines

    iMaxLines = myLayoutRect.Rect().Height() / iLineHeight;

    LAYOUT_DP( D_LAYOUT_LIT( "CCALayoutInfo::LayoutChangedL iMaxLines:%d" )
               , iMaxLines );

    // NickFont

    TAknTextLineLayout myMessageHeaderLayout =
        IM_reading_pane_texts_Line_1( 1 );
    iNickFont = AknLayoutUtils::FontFromId( myMessageHeaderLayout.FontId() );

    // MaxLineWidth

    TAknLayoutText myTextLayout;
    TAknLayoutText myTextLayout2;

    myTextLayout.LayoutText( myLine.Rect(), myMessageHeaderLayout );

    iMaxLineWidth = myTextLayout.TextRect().Width();

    LAYOUT_DP( D_LAYOUT_LIT( "CCALayoutInfo::LayoutChangedL iMaxLineWidth:%d" )
               , iMaxLineWidth );

    // IconSkip
    myMessageHeaderLayout = IM_reading_pane_texts_Line_1( 2 );

    myTextLayout2.LayoutText( myLine.Rect(), myMessageHeaderLayout );

    if ( iMirrored )
        {
        iIconSkip = myTextLayout.TextRect().iBr.iX
                    - myTextLayout2.TextRect().iBr.iX;
        }
    else
        {
        iIconSkip = myTextLayout2.TextRect().iTl.iX
                    - myTextLayout.TextRect().iTl.iX;
        }

    LAYOUT_DP( D_LAYOUT_LIT( "CCALayoutInfo::LayoutChangedL iIconSkip:%d" )
               , iIconSkip );

    // MsgFont

    TAknTextLineLayout myMessageLayout = IM_reading_pane_texts_Line_2();
    iMsgFont = AknLayoutUtils::FontFromId( myMessageLayout.FontId() );

    //Indent
    //normal message text with indent
    myTextLayout.LayoutText( myLine.Rect(), myMessageLayout );

    //header with full width
    myMessageHeaderLayout = IM_reading_pane_texts_Line_1( 1 );
    myTextLayout2.LayoutText( myLine.Rect(), myMessageHeaderLayout );

    if ( iMirrored )
        {
        iIndent = myTextLayout2.TextRect().iBr.iX
                  - myTextLayout.TextRect().iBr.iX;
        }
    else
        {
        iIndent = myTextLayout.TextRect().iTl.iX
                  - myTextLayout2.TextRect().iTl.iX;
        }
    LAYOUT_DP( D_LAYOUT_LIT( "CCALayoutInfo::LayoutChangedL iIndent:%d" )
               , iIndent );

    // MaxNickWidth

    myMessageHeaderLayout = IM_reading_pane_texts_Line_1( 0 );

    myTextLayout.LayoutText( myLine.Rect(), myMessageHeaderLayout );

    iMaxNickWidth = myTextLayout.TextRect().Width() + iIndent;
    // - myTextLayout2.TextRect().iTl.iX;

    LAYOUT_DP( D_LAYOUT_LIT( "CCALayoutInfo::LayoutChangedL iMaxNickWidth:%d" )
               , iMaxNickWidth );

    LAYOUT_DP_TXT( "CCALayoutInfo::LayoutChangedL over" );
    }

// ---------------------------------------------------------
// CCALayoutInfo::NickFont
// (other items were commented in a header).
// ---------------------------------------------------------
//
const CFont* CCALayoutInfo::NickFont()
    {
    return iNickFont;
    }

// ---------------------------------------------------------
// CCALayoutInfo::MessageFont
// (other items were commented in a header).
// ---------------------------------------------------------
//
const CFont* CCALayoutInfo::MessageFont()
    {
    return iMsgFont;
    }

// ---------------------------------------------------------
// CCALayoutInfo::LineHeight
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCALayoutInfo::LineHeight()
    {
    return iLineHeight;
    }

// ---------------------------------------------------------
// CCALayoutInfo::MaxLineWidth
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCALayoutInfo::MaxLineWidth()
    {
    return iMaxLineWidth;
    }

// ---------------------------------------------------------
// CCALayoutInfo::MaxNickWidth
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCALayoutInfo::MaxNickWidth()
    {
    return iMaxNickWidth;
    }

// ---------------------------------------------------------
// CCALayoutInfo::MaxLines
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCALayoutInfo::MaxLines()
    {
    return iMaxLines;
    }

// ---------------------------------------------------------
// CCALayoutInfo::MainPaneRect
// (other items were commented in a header).
// ---------------------------------------------------------
//
TRect CCALayoutInfo::MainPaneRect()
    {
    return TRect( iMainPaneRect );
    }

// ---------------------------------------------------------
// CCALayoutInfo::SetLayoutID
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::SetLayoutID( TChatListBoxLayout::TLayout aLayout )
    {
    iLayoutID = aLayout;
    TRAPD( err, LayoutChangedL() );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::Indent
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCALayoutInfo::Indent()
    {
    return iIndent;
    }

// ---------------------------------------------------------
// CCALayoutInfo::ScreenRect
// (other items were commented in a header).
// ---------------------------------------------------------
//
TRect CCALayoutInfo::ScreenRect()
    {
    return iScreenRect;
    }

// ---------------------------------------------------------
// CCALayoutInfo::SetEditorVisibility
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::SetEditorVisibility( TBool aVisible )
    {
    iEditorVisibility = aVisible;
    }

// ---------------------------------------------------------
// CCALayoutInfo::EditorVisibility
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCALayoutInfo::EditorVisibility()
    {
    return iEditorVisibility;
    }

// ---------------------------------------------------------
// CCALayoutInfo::SetupMainPaneRect
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::SetupMainPaneRect()
    {
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                       iMainPaneRect );
    _LIT( KMain, "MainPane" );

    iMainPaneRect.Move( -iMainPaneRect.iTl.iX, -iMainPaneRect.iTl.iY );

    DPRect( KMain, iMainPaneRect );

    TRect myControlPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EControlPane,
                                       myControlPane );
    _LIT( KControlPane, "ControlPane" );
    DPRect( KControlPane, myControlPane );
    //this is screen relative lets move back to origin.
    }

// ---------------------------------------------------------
// CCALayoutInfo::im_reading_pane
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::im_reading_pane( TInt aIndex_H )
    {
    return ( *iReadingPane )[ aIndex_H ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::im_reading_field
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::im_reading_field( TInt aIndex_t )
    {
    return ( *iReadingField )[ aIndex_t ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_text_elements
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::IM_text_elements( TInt aLineIndex )
    {
    return ( *iIMTextElements )[ aLineIndex ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_reading_pane_texts_Line_1
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknTextLineLayout CCALayoutInfo::IM_reading_pane_texts_Line_1( TInt aCommon1 )
    {
    return ( *iReadingPaneTextsLine1 )[ aCommon1 ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_reading_pane_texts_Line_2
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknTextLineLayout CCALayoutInfo::IM_reading_pane_texts_Line_2()
    {
    return iReadingPaneTextsLine2;
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_text_elements_Line_1
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::IM_text_elements_Line_1()
    {
    return AppLayout::IM_text_elements_Line_1();
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_text_elements_Line_2
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::IM_text_elements_Line_2()
    {
    return AppLayout::IM_text_elements_Line_2();
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_reading_field_highlight_graphics_Line_1
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::IM_reading_field_highlight_graphics_Line_1(
    const TRect& aParentRect )
    {
    return AppLayout::IM_reading_field_highlight_graphics_Line_1( aParentRect );
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_reading_field_highlight_graphics_Line_2
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::IM_reading_field_highlight_graphics_Line_2(
    const TRect& aParentRect )
    {
    return AppLayout::IM_reading_field_highlight_graphics_Line_2( aParentRect );
    }

// ---------------------------------------------------------
// CCALayoutInfo::im_writing_pane
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::im_writing_pane( TInt aCommon1 )
    {
    return ( *iWrittingPane )[ aCommon1 ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_writing_field_elements_Line_1
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::IM_writing_field_elements_Line_1(
    TInt aIndex_H )
    {
    return ( *iIMWritingFieldLine1 )[ aIndex_H ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_writing_field_elements_Line_2
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::IM_writing_field_elements_Line_2()
    {
    return iIMWritingFieldLine2;
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_writing_field_elements_Line_3
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknWindowLineLayout CCALayoutInfo::IM_writing_field_elements_Line_3(
    TInt aIndex_H )
    {
    return ( *iIMWritingFieldLine3 )[ aIndex_H ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::Multiline_IM_writing_pane_texts_Line_1
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknMultiLineTextLayout CCALayoutInfo::Multiline_IM_writing_pane_texts_Line_1(
    TInt aNumberOfLinesShown )
    {
    return ( *iWrittingPaneTextsLine )[ aNumberOfLinesShown ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::IM_navi_pane_texts_Line_1
// (other items were commented in a header).
// ---------------------------------------------------------
//
TAknTextLineLayout CCALayoutInfo::IM_navi_pane_texts_Line_1(
    TInt aIndex_C, TInt /*aIndex_W*/ )
    {
    return ( *iIMNaviPaneTextsLine1 )[ aIndex_C ];
    }

// ---------------------------------------------------------
// CCALayoutInfo::LogLayout
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::LogLayoutL()
    {
#ifdef _DEBUG

    _LIT( K1, "im_reading_pane( 0 ) " );
    DPLayout( K1, AppLayout::im_reading_pane( 0 ) );
    DPLayout( K1, im_reading_pane( 0 ) );
    _LIT( K2, "im_reading_pane( 1 ) " );
    DPLayout( K2, AppLayout::im_reading_pane( 1 ) );
    DPLayout( K2, im_reading_pane( 1 ) );
    _LIT( K3, "im_reading_pane( 2 ) " );
    DPLayout( K3, AppLayout::im_reading_pane( 2 ) );
    DPLayout( K3, im_reading_pane( 2 ) );

    _LIT( KWP1, "im_writing_pane( 0 )" );
    DPLayout( KWP1, AppLayout::im_writing_pane( 0 ) );
    DPLayout( KWP1, im_writing_pane( 0 ) );
    _LIT( KWP2, "im_writing_pane( 1 )" );
    DPLayout( KWP2, AppLayout::im_writing_pane( 1 ) );
    DPLayout( KWP2, im_writing_pane( 1 ) );

    _LIT( K4, "im_reading_field( 0 ) " );
    DPLayout( K4, AppLayout::im_reading_field( 0 ) );
    DPLayout( K4, im_reading_field( 0 ) );
    _LIT( K5, "im_reading_field( 1 ) " );
    DPLayout( K5, AppLayout::im_reading_field( 1 ) );
    DPLayout( K5, im_reading_field( 1 ) );
    _LIT( K6, "im_reading_field( 2 ) " );
    DPLayout( K6, AppLayout::im_reading_field( 2 ) );
    DPLayout( K6, im_reading_field( 2 ) );
    _LIT( K7, "im_reading_field( 3 ) " );
    DPLayout( K7, AppLayout::im_reading_field( 3 ) );
    DPLayout( K7, im_reading_field( 3 ) );
    _LIT( K8, "im_reading_field( 4 ) " );
    DPLayout( K8, AppLayout::im_reading_field( 4 ) );
    DPLayout( K8, im_reading_field( 4 ) );
    _LIT( K9, "im_reading_field( 5 ) " );
    DPLayout( K9, AppLayout::im_reading_field( 5 ) );
    DPLayout( K9, im_reading_field( 5 ) );
    _LIT( K10, "im_reading_field( 6 ) " );
    DPLayout( K10, AppLayout::im_reading_field( 6 ) );
    DPLayout( K10, im_reading_field( 6 ) );

    TAknLayoutScalableParameterLimits limits(
        AknLayoutScalable_Apps::list_im_single_pane_ParamLimits() );

    if ( 7 <= limits.LastRow() )
        {
        _LIT( K11, "im_reading_field( 7 ) " );
        DPLayout( K11, AppLayout::im_reading_field( 7 ) );
        DPLayout( K11, im_reading_field( 7 ) );
        }

    _LIT( KRPT0, "IM_reading_pane_texts_Line_1( 0 )" );
    DPLayout( KRPT0, AppLayout::IM_reading_pane_texts_Line_1( 0 ) );
    DPLayout( KRPT0, IM_reading_pane_texts_Line_1( 0 ) );
    _LIT( KRPT1, "IM_reading_pane_texts_Line_1( 1 )" );
    DPLayout( KRPT1, AppLayout::IM_reading_pane_texts_Line_1( 1 ) );
    DPLayout( KRPT1, IM_reading_pane_texts_Line_1( 1 ) );
    _LIT( KRPT2, "IM_reading_pane_texts_Line_1( 2 )" );
    DPLayout( KRPT2, AppLayout::IM_reading_pane_texts_Line_1( 2 ) );
    DPLayout( KRPT2, IM_reading_pane_texts_Line_1( 2 ) );
    _LIT( KRPT3, "IM_reading_pane_texts_Line_2()" );
    DPLayout( KRPT3, AppLayout::IM_reading_pane_texts_Line_2() );
    DPLayout( KRPT3, IM_reading_pane_texts_Line_2() );

    _LIT( KIMT1, "IM_text_elements( 0 )" );
    DPLayout( KIMT1, AppLayout::IM_text_elements( 0 ) );
    DPLayout( KIMT1, IM_text_elements( 0 ) );
    _LIT( KIMT2, "IM_text_elements( 1 )" );
    DPLayout( KIMT2, AppLayout::IM_text_elements( 1 ) );
    DPLayout( KIMT2, IM_text_elements( 1 ) );
    _LIT( KIMT3, "IM_text_elements( 2 )" );
    DPLayout( KIMT3, AppLayout::IM_text_elements( 2 ) );
    DPLayout( KIMT3, IM_text_elements( 2 ) );

    _LIT( KMWPT0, "Multiline_IM_writing_pane_texts_Line_1( 0 )" );
    DPLayout( KMWPT0, AppLayout::Multiline_IM_writing_pane_texts_Line_1( 0 ) );
    DPLayout( KMWPT0, Multiline_IM_writing_pane_texts_Line_1( 0 ) );

    _LIT( KMWPT1, "Multiline_IM_writing_pane_texts_Line_1( 1 )" );
    DPLayout( KMWPT1, AppLayout::Multiline_IM_writing_pane_texts_Line_1( 1 ) );
    DPLayout( KMWPT1, Multiline_IM_writing_pane_texts_Line_1( 1 ) );

    _LIT( KMWPT2, "Multiline_IM_writing_pane_texts_Line_1( 2 )" );
    DPLayout( KMWPT2, AppLayout::Multiline_IM_writing_pane_texts_Line_1( 2 ) );
    DPLayout( KMWPT2, Multiline_IM_writing_pane_texts_Line_1( 2 ) );

    _LIT( KWFL10, "IM_writing_field_elements_Line_1( 0 )" );
    DPLayout( KWFL10, AppLayout::IM_writing_field_elements_Line_1( 0 ) );
    DPLayout( KWFL10, IM_writing_field_elements_Line_1( 0 ) );

    _LIT( KWFL11, "IM_writing_field_elements_Line_1( 1 )" );
    DPLayout( KWFL11, AppLayout::IM_writing_field_elements_Line_1( 1 ) );
    DPLayout( KWFL11, IM_writing_field_elements_Line_1( 1 ) );

    _LIT( KWFL2, "IM_writing_field_elements_Line_2()" );
    DPLayout( KWFL2, AppLayout::IM_writing_field_elements_Line_2() );
    DPLayout( KWFL2, IM_writing_field_elements_Line_2() );

    _LIT( KWFL30, "IM_writing_field_elements_Line_3( 0 )" );
    DPLayout( KWFL30, AppLayout::IM_writing_field_elements_Line_3( 0 ) );
    DPLayout( KWFL30, IM_writing_field_elements_Line_3( 0 ) );

    _LIT( KWFL31, "IM_writing_field_elements_Line_3( 1 )" );
    DPLayout( KWFL31, AppLayout::IM_writing_field_elements_Line_3( 1 ) );
    DPLayout( KWFL31, IM_writing_field_elements_Line_3( 1 ) );

    HBufC* myStr = HBufC::NewLC( 60 );
    TPtr myPtr = myStr->Des();

    _LIT( KNPTL, "IM_navi_pane_texts_Line_1( " );
    _LIT( KNPTL0, "AppLayout::" );
    _LIT( KNPTL1, ", 0 )" );
    _LIT( KNPTL2, ", 1 )" );
    TInt i( 0 );
    for ( i = 0; i < 10; i++ )
        {
        myPtr.Copy( KNPTL0() );
        myPtr.Append( KNPTL() );
        myPtr.AppendNum( i );
        myPtr.Append( KNPTL1() );
        DPLayout( myPtr, IM_navi_pane_texts_Line_1( i , 0 ) );

        myPtr.Copy( KNPTL() );
        myPtr.AppendNum( i );
        myPtr.Append( KNPTL1() );
        DPLayout( myPtr, IM_navi_pane_texts_Line_1( i , 0 ) );
        }
    for ( i = 0; i < 10; i++ )
        {
        myPtr.Copy( KNPTL0() );
        myPtr.Append( KNPTL() );
        myPtr.AppendNum( i );
        myPtr.Append( KNPTL2() );
        DPLayout( myPtr, IM_navi_pane_texts_Line_1( i , 1 ) );

        myPtr.Copy( KNPTL() );
        myPtr.AppendNum( i );
        myPtr.Append( KNPTL2() );
        DPLayout( myPtr, IM_navi_pane_texts_Line_1( i , 1 ) );
        }

    CleanupStack::PopAndDestroy( myStr );

#endif // _DEBUG
    }

// ---------------------------------------------------------
// CCALayoutInfo::DPLayout
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::DPLayout( const TDesC& aStr, const TAknTextLineLayout aLayout )
    {
    TPtrC myStr( aStr );
    LAYOUT_DP( D_LAYOUT_LIT(
                   "%S iC il ir iB iW iJ iFont iBaselineSkip iNumberOfLinesShown" )
               , &myStr );
    LAYOUT_DP( D_LAYOUT_LIT( "%d %d %d %d %d %d %d %d %d" ), aLayout.iC
               , aLayout.il
               , aLayout.ir
               , aLayout.iB
               , aLayout.iW
               , aLayout.iJ
               , aLayout.iFont
               , aLayout.iBaselineSkip
               , aLayout.iNumberOfLinesShown );
    }

// ---------------------------------------------------------
// CCALayoutInfo::DPLayout
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::DPLayout( const TDesC& aStr,
                              const TAknWindowLineLayout aLayout )
    {
    TPtrC myStr( aStr );
    LAYOUT_DP( D_LAYOUT_LIT( "%S iC il  it ir ib iW iH" ), &myStr  );
    LAYOUT_DP( D_LAYOUT_LIT( "%d %d %d %d %d %d %d" )
               , aLayout.iC
               , aLayout.il
               , aLayout.it
               , aLayout.ir
               , aLayout.ib
               , aLayout.iW
               , aLayout.iH );
    }

// ---------------------------------------------------------
// CCALayoutInfo::DPRect
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::DPRect( const TDesC& aStr, const TRect& aRect )
    {
    TPtrC myStr( aStr );
    LAYOUT_DP( D_LAYOUT_LIT( "%S TRect %d %d %d %d" ), &myStr
               , aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY );
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillLayoutL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillLayoutL()
    {
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, iScreenRect );
    if ( iLastScreenRect != iScreenRect )
        {
        FillReadingPaneL();
        FillWrittingPaneL();
        FillReadingFieldL();
        FillReadingPaneTextsLine1L();
        FillReadingPaneTextsLine2();
        FillIMTextElementsL();
        FillWrittingPaneTextsLineL();
        FillIMWritingFieldLine1L();
        FillIMWritingFieldLine2();
        FillIMWritingFieldLine3L();
        FillIMNaviPaneTextsLineL();
        }
    iLastScreenRect = iScreenRect;
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillReadingPaneL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillReadingPaneL()
    {
    if ( iReadingPane )
        {
        iReadingPane->Reset();
        delete iReadingPane;
        iReadingPane = NULL;
        }

    iReadingPane = new( ELeave )CArrayFixFlat<TAknWindowLineLayout>( 3 );
    for ( TInt i( 0 ); i < 3; i++ )
        {
        iReadingPane->AppendL( AppLayout::im_reading_pane( i ) );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillReadingFieldL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillReadingFieldL()
    {
    if ( iReadingField )
        {
        iReadingField->Reset();
        delete iReadingField;
        iReadingField = NULL;
        }

    TAknLayoutScalableParameterLimits limits(
        AknLayoutScalable_Apps::list_im_single_pane_ParamLimits() );

    // items from FirstRow to LastRow
    iReadingField = new( ELeave )CArrayFixFlat<TAknWindowLineLayout>(
        limits.LastRow() - limits.FirstRow() + 1 );

    for ( TInt i( limits.FirstRow() ); i <= limits.LastRow(); ++i )
        {
        iReadingField->AppendL( AppLayout::im_reading_field( i ) );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillReadingPaneTextsLine1
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillReadingPaneTextsLine1L()
    {
    if ( iReadingPaneTextsLine1 )
        {
        iReadingPaneTextsLine1->Reset();
        delete iReadingPaneTextsLine1;
        iReadingPaneTextsLine1 = NULL;
        }

    iReadingPaneTextsLine1 = new( ELeave )CArrayFixFlat<TAknTextLineLayout>( 3 );
    for ( TInt i( 0 ); i < 3; i++ )
        {
        iReadingPaneTextsLine1->AppendL(
            AppLayout::IM_reading_pane_texts_Line_1( i ) );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillIMTextElementsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillIMTextElementsL()
    {
    if ( iIMTextElements )
        {
        iIMTextElements->Reset();
        delete iIMTextElements;
        iIMTextElements = NULL;
        }

    iIMTextElements = new( ELeave )CArrayFixFlat<TAknWindowLineLayout>( 3 );
    for ( TInt i( 0 ); i < 3; i++ )
        {
        iIMTextElements->AppendL( AppLayout::IM_text_elements( i ) );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillReadingPaneTextsLine2
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillReadingPaneTextsLine2()
    {
    iReadingPaneTextsLine2 = AppLayout::IM_reading_pane_texts_Line_2();
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillWrittingPaneTextsLineL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillWrittingPaneTextsLineL()
    {
    if ( iWrittingPaneTextsLine )
        {
        iWrittingPaneTextsLine->Reset();
        delete iWrittingPaneTextsLine;
        iWrittingPaneTextsLine = NULL;
        }

    iWrittingPaneTextsLine = new( ELeave )CArrayFixFlat<TAknMultiLineTextLayout>( 3 );
    for ( TInt i( 0 ); i < 3; i++ )
        {
        iWrittingPaneTextsLine->AppendL(
            AppLayout::Multiline_IM_writing_pane_texts_Line_1( i ) );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillIMWritingFieldLine1
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillIMWritingFieldLine1L()
    {
    if ( iIMWritingFieldLine1 )
        {
        iIMWritingFieldLine1->Reset();
        delete iIMWritingFieldLine1;
        iIMWritingFieldLine1 = NULL;
        }

    iIMWritingFieldLine1 = new( ELeave )CArrayFixFlat<TAknWindowLineLayout>( 3 );
    for ( TInt i( 0 ); i < 2; i++ )
        {
        iIMWritingFieldLine1->AppendL(
            AppLayout::IM_writing_field_elements_Line_1( i ) );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillIMWritingFieldLine2
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillIMWritingFieldLine2()
    {
    iIMWritingFieldLine2 =
        AppLayout::IM_writing_field_elements_Line_2();
    iIMWritingFieldLine2.iH = 1;
    iIMWritingFieldLine2.iW = ELayoutP;
    iIMWritingFieldLine2.iC = 221;
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillIMWritingFieldLine3L
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillIMWritingFieldLine3L()
    {
    if ( iIMWritingFieldLine3 )
        {
        iIMWritingFieldLine3->Reset();
        delete iIMWritingFieldLine3;
        iIMWritingFieldLine3 = NULL;
        }

    iIMWritingFieldLine3 = new ( ELeave ) CArrayFixFlat< TAknWindowLineLayout >(
        3 );
    for ( TInt i( 0 ); i < 2; i++ )
        {
        iIMWritingFieldLine3->AppendL(
            AppLayout::IM_writing_field_elements_Line_3( i ) );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillWrittingPaneL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillWrittingPaneL()
    {
    if ( iWrittingPane )
        {
        iWrittingPane->Reset();
        delete iWrittingPane;
        iWrittingPane = NULL;
        }

    iWrittingPane = new( ELeave )CArrayFixFlat<TAknWindowLineLayout>( 2 );
    for ( TInt i( 0 ); i < 2; i++ )
        {
        iWrittingPane->AppendL( AppLayout::im_writing_pane( i ) );
        }
    }

// ---------------------------------------------------------
// CCALayoutInfo::FillIMNaviPaneTextsLine
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALayoutInfo::FillIMNaviPaneTextsLineL()
    {
    // fader colors from LAF. These are hardcoded as the LAF (impl.) adviced.
    const TInt16 colors[] = { 234, 232, 230, 228, 226, 0, 226, 227, 228, 229 };

    if ( iIMNaviPaneTextsLine1 )
        {
        iIMNaviPaneTextsLine1->Reset();
        delete iIMNaviPaneTextsLine1;
        iIMNaviPaneTextsLine1 = NULL;
        }

    iIMNaviPaneTextsLine1 =
        new ( ELeave ) CArrayFixFlat<TAknTextLineLayout>( 10 );

    TAknTextLineLayout text;
    for ( TInt i( 0 ); i < 10; i++ )
        {
        text = AknLayoutScalable_Apps::navi_pane_im_t1();
        text.iC = colors[ i ];
        iIMNaviPaneTextsLine1->AppendL( text );
        }
    }

// End of File
