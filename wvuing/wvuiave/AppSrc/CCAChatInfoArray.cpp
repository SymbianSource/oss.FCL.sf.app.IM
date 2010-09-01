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
* Description:  Array of chat info structure
 *
*/


// INCLUDE FILES
#include    "CCAChatInfoArray.h"
#include    "ChatDefinitions.h"
#include	<eikenv.h>
#include	<aknutils.h>
#include    <aknlayoutscalable_avkon.cdl.h>

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CCAChatInfoArray::CCAChatInfoArray()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//

CCAChatInfoArray::CCAChatInfoArray()
    {
    }
// ---------------------------------------------------------
// CCAChatInfoArray::ConstructL()
//Symbian OS default constructor can leave.
//
// ---------------------------------------------------------
//
void CCAChatInfoArray::ConstructL()
    {
    iLine = HBufC::NewL( KMaxLengthOfHBufC + KMaxTabLength );
    iTextFields = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    iLineWidthArray = new ( ELeave ) CArrayFixFlat<TInt>( KArrayGranularity );

    iFont = LatinBold13(); // LAF

    TAknLayoutRect temp, layout;
    TAknLayoutText myLayoutText;

    TRect myMainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                       myMainPaneRect );

    layout.LayoutRect( myMainPaneRect, AknLayoutScalable_Avkon::main_pane( 1 ) );
    temp.LayoutRect( layout.Rect(), AknLayoutScalable_Avkon::popup_menu_window( 13 ) );
    layout.LayoutRect( temp.Rect(), AknLayoutScalable_Avkon::listscroll_menu_pane( 0 ) );
    temp.LayoutRect( layout.Rect(), AknLayoutScalable_Avkon::list_menu_pane( 0 ) );
    myLayoutText.LayoutText( temp.Rect(), AknLayoutScalable_Avkon::list_single_heading_pane_t1_cp2( 0 ) );

    iFont = myLayoutText.Font();

    iSubCellWidth = myLayoutText.TextRect().Size().iWidth; // * 4 / 11;

    }

// ---------------------------------------------------------
// CCAChatInfoArray::SetLineWidthArrayL()
// Sets the linewidth array full of integers
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatInfoArray::SetLineWidthArrayL( CArrayFix<TInt>* aLineWidthArray,
                                           const TDesC& aTextField )
    {


    TInt maxSize( iFont->TextWidthInPixels( aTextField ) / iSubCellWidth );
    for ( TInt i( 0 ); i <= maxSize; ++i )
        {
        aLineWidthArray->AppendL( iSubCellWidth );
        }
    }

// ---------------------------------------------------------
// CCAChatInfoArray::NewL()
//
// ---------------------------------------------------------
//

CCAChatInfoArray* CCAChatInfoArray::NewL()
    {
    CCAChatInfoArray* self = new ( ELeave ) CCAChatInfoArray;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CCAChatInfoArray::~CCAChatInfoArray()
// Destructor
// ---------------------------------------------------------
//
CCAChatInfoArray::~CCAChatInfoArray()
    {
    delete iLine;
    delete iTextFields;
    delete iLineWidthArray;
    }

// ---------------------------------------------------------
// CCAChatInfoArray::AddLabelAndTextL()
// Add labels and field texts to array
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatInfoArray::AddLabelAndTextL( TInt aLabelResourceId,
                                         const TDesC& aFieldText,
                                         TBool /*aWrap*/ )
    {
    SetLineWidthArrayL( iLineWidthArray, aFieldText );

    //temp array that contains TPtrC values. Number 1 is granularity
    CArrayFixFlat<TPtrC>* tempArray = new ( ELeave ) CArrayFixFlat<TPtrC>( 1 );
    CleanupStack::PushL( tempArray );
    AknTextUtils::WrapToArrayL( aFieldText, *iLineWidthArray, *iFont,
                                *tempArray );

    HBufC* itemBuf = HBufC::NewLC( KMaxLengthOfHBufC );
    TPtr itemPtr( itemBuf->Des() );

    HBufC* label = CEikonEnv::Static()->
                   AllocReadResourceLC( aLabelResourceId );

    TInt arrayCount( tempArray->Count() );
    for ( TInt i( 0 ); i < arrayCount; i++ )
        {
        itemPtr.Copy( tempArray->At( i ).Left( itemPtr.MaxLength() ) );
        itemPtr.Insert( 0, KTabulator );
        if ( i == 0 )
            {
            itemPtr.Insert( 0, *label );
            }
        iTextFields->AppendL( *itemBuf );
        itemPtr.Zero();
        }
    CleanupStack::PopAndDestroy( 3, tempArray ); // itemBuf, label, tempArray
    }

// ---------------------------------------------------------
// CCAChatInfoArray::MdcaCount() const
// Returns number of items that are in array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAChatInfoArray::MdcaCount() const
    {
    __ASSERT_ALWAYS( iTextFields,
                     User::Panic( KPanicText, EChatInfoArrayNotExists ) );
    return iTextFields->MdcaCount();
    }

// ---------------------------------------------------------
// CCAChatInfoArray::MdcaPoint( TInt aIndex ) const
// Returns formatted descriptor for given index
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC CCAChatInfoArray::MdcaPoint( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( iTextFields,
                     User::Panic( KPanicText, EChatInfoArrayNotExists ) );

    __ASSERT_DEBUG( aIndex >= 0,
                    User::Panic( KPanicText, EChatInfoArrayIndexOutOfBounds ) );

    __ASSERT_DEBUG( aIndex < iTextFields->MdcaCount(),
                    User::Panic( KPanicText, EChatInfoArrayIndexOutOfBounds ) ) ;

    return iTextFields->MdcaPoint( aIndex );
    }

//  End of File
