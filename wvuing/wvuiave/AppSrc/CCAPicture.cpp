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
* Description:  Picture wrapper for icons and smileys
*
*/


// INCLUDE FILES
#include    "CCAPicture.h"
#include    "ChatDebugPrint.h"
#include    "ChatDefinitions.h"
#include    "fbs.h"
#include	<gulicon.h>

// -----------------------------------------------------------------------------
// CCAPicture::CCAPicture
// -----------------------------------------------------------------------------
//
CCAPicture::CCAPicture( MGraphicsDeviceMap& aMap, CGulIcon* aIcon,
                        TInt aIndex /* = -1 */, TBool aExternalIcon /* = ETrue  */ )
        : iIndex( aIndex ),
        iIcon( aIcon ),
        iExternalIcon( aExternalIcon ),
        iGfxMap( aMap )
    {
    __ASSERT_ALWAYS( iIcon,
                     User::Panic( KPanicText, EIconDoesNotExist ) );

    SetTwips();
    }

// -----------------------------------------------------------------------------
// CCAPicture::~CCAPicture
// -----------------------------------------------------------------------------
//
CCAPicture::~CCAPicture()
    {
    if ( !iExternalIcon )
        {
        delete iIcon;
        }
    }

// -----------------------------------------------------------------------------
// CCAPicture::LineBreakPossible
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAPicture::LineBreakPossible( TUint /*aClass*/,
                                     TBool /*aBeforePicture*/,
                                     TBool /*aHaveSpaces*/ ) const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAPicture::Draw
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPicture::Draw( CGraphicsContext& aGc,
                       const TPoint& aTopLeft,
                       const TRect& aClipRect,
                       MGraphicsDeviceMap* /*aMap*/ ) const
    {
    TSize size( iIcon->Bitmap()->SizeInPixels() );
    CBitmapContext& bc = static_cast<CBitmapContext&>( aGc );

    CHAT_DP( D_CHAT_LIT( "CCAPicture::Draw %dx%d to %d,%d" ),
             size.iWidth, size.iHeight, aTopLeft.iX, aTopLeft.iY );

    aGc.Reset();
    aGc.SetClippingRect( aClipRect );

    if ( iIcon->Mask() )
        {
        bc.BitBltMasked( aTopLeft, iIcon->Bitmap(), size, iIcon->Mask(), ETrue );
        }
    else
        {
        bc.BitBlt( aTopLeft, iIcon->Bitmap(), size );
        }
    }

// -----------------------------------------------------------------------------
// CCAPicture::ExternalizeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPicture::ExternalizeL( RWriteStream& /*aStream*/ ) const
    {
    }

// -----------------------------------------------------------------------------
// CCAPicture::GetOriginalSizeInTwips(
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPicture::GetOriginalSizeInTwips( TSize& aSize ) const
    {
    if ( iIcon->Bitmap() )
        {
        aSize = iIcon->Bitmap()->SizeInTwips();

        if ( aSize.iHeight == 0 || aSize.iWidth == 0 )
            {
            SetTwips();
            aSize = iIcon->Bitmap()->SizeInTwips();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAPicture::Index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPicture::Index() const
    {
    return iIndex;
    }

// -----------------------------------------------------------------------------
// CCAPicture::SetTwips
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPicture::SetTwips() const
    {
    TSize sizeP( iIcon->Bitmap()->SizeInPixels() );
    TSize sizeT( iGfxMap.HorizontalPixelsToTwips( sizeP.iWidth ),
                 iGfxMap.VerticalPixelsToTwips( sizeP.iHeight ) );
    iIcon->Bitmap()->SetSizeInTwips( sizeT );

    CHAT_DP( D_CHAT_LIT( "CCAPicture::CCAPicture() %dx%d (%dx%d)" ),
             sizeP.iWidth, sizeP.iHeight,
             sizeT.iWidth, sizeT.iHeight );
    }
