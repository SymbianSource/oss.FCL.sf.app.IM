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
* Description:  Active implementation of skin feature.
*
*/



// INCLUDE FILES
#include    "CCASkinVariant20.h"
#include    "CABitmapMapping.h"     // defines the skinned bitmap ids
#include    "ccaiconfileprovider.h"
#include	"MCAOpBrandVariant.h"
#include    "ChatDebugPrint.h"
#include    "ChatDebugAssert.h"
#include	<aknappui.h>
#include	<AknsSkinInstance.h>	// skins 
#include	<AknsUtils.h>
#include	<AknUtils.h>
#include	<AknsDrawUtils.h>
#include	<AknsBasicBackgroundControlContext.h>
#include	<gulicon.h>
#include    <akniconutils.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCASkinVariant20::CCASkinVariant20
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCASkinVariant20::CCASkinVariant20()
        : iFs( CCoeEnv::Static()->FsSession() )
    {
    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCASkinVariant20::ConstructL( MCAOpBrandVariant* aOpBrandVariant )
    {
    User::LeaveIfError( iFs.ShareProtected() );

    iOpBrandVariant = aOpBrandVariant;
    TRect mainPane( 0, 0, 0, 0 );

    // Return value can be safely ignored.
    // from AknLayoutUtils::LayoutMetricsRect documentation:
    //
    // Returns EFalse for status pane descendants if requested
    // layout component is not available in current layout. For
    // other components returns always ETrue (returned rectangle is
    // from layout definition).
    //
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, mainPane );

    iBgContext = CAknsBasicBackgroundControlContext::NewL(
                     KAknsIIDQsnBgAreaMain, mainPane, EFalse );
    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCASkinVariant20* CCASkinVariant20::NewL( MCAOpBrandVariant* aOpBrandVariant )
    {
    CCASkinVariant20* self = new( ELeave ) CCASkinVariant20;

    CleanupStack::PushL( self );
    self->ConstructL( aOpBrandVariant );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCASkinVariant20::~CCASkinVariant20()
    {
    delete iBgContext;
    delete iBitmapFile;
    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::SkinFlag
// returns skins-enabled flag
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCASkinVariant20::SkinFlag() const
    {
    return CAknAppUi::EAknEnableSkin;
    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::ClearChatListBoxBackGround
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASkinVariant20::ClearChatListBoxBackGround( CWindowGc& aGc,
                                                   const TRect& aUsedPortionOfViewRect,
                                                   MAknsControlContext* aContext )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::Background( skin, aContext, NULL, aGc,
                               aUsedPortionOfViewRect );
    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::MainPaneBackgroundContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MAknsControlContext* CCASkinVariant20::MainPaneBackgroundContext() const
    {
    return iBgContext;
    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::UpdateLayout
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASkinVariant20::UpdateLayout()
    {
    TRect screen( 0, 0, 0, 0 );
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );
    iBgContext->SetRect( screen );
    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::DrawSkinnedEditor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASkinVariant20::DrawSkinnedEditor( const CCoeControl* aControl,
                                           CWindowGc& aGc,
                                           TRect& /*aParentRect*/,
                                           TRect& aEditorRect,
                                           TRect& aOutlineRect )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // draw editor surroundings
    AknsDrawUtils::BackgroundBetweenRects( skin, MainPaneBackgroundContext(),
                                           aControl, aGc, aOutlineRect,
                                           aEditorRect );

    TRect innerRect( aOutlineRect );
    //why 6 ? don't ask me. It was 3x3 for 176x208.
    innerRect.Shrink( TSize( 6, 6 ) );
    // draw editor frames
    return  AknsDrawUtils::DrawFrame( skin, aGc, aOutlineRect, innerRect,
                                      KAknsIIDQsnFrInput, KAknsIIDQsnFrInputCenter );

    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::LoadBitmapL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGulIcon* CCASkinVariant20::LoadBitmapL( TInt aBitmapId, TInt aMaskId,
                                         const TDesC& aFullPath, TBool aOverrideBrand,
                                         TRgb* aCustomColor )
    {
    CHAT_DP_TXT( "CCASkinVariant20::LoadBitmapL - start" );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    if ( iOpBrandVariant )
        {
        // brand overrides skinned bitmaps if bitmap is not forced to be
        // read through skins (aOverrideBrand)
        if ( iOpBrandVariant->BrandBitmapsActivatedL() && !aOverrideBrand )
            {
            CHAT_DP_TXT( "CCASkinVariant20::LoadBitmapL - \
                          returning branded icon" );

            // get a copy of filename
            HBufC* temp = aFullPath.AllocL();
            delete iBitmapFile;
            iBitmapFile = temp;

            // icon server take ownership of of icon file provider
            CCAIconFileProvider* ifp = CCAIconFileProvider::NewL( iFs, *iBitmapFile );
            CleanupStack::PushL( ifp );

            AknIconUtils::CreateIconLC( bitmap, mask, *ifp, aBitmapId,
                                        aMaskId );

            CleanupStack::Pop( 3, ifp ); // bitmap, mask, ifp

            CGulIcon* gulIcon = CGulIcon::NewL( bitmap, mask );
            return gulIcon;
            }

        }

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TAknsItemID itemId( KAknsIIDNone );
    TInt colorGroupId( 0 );
    MapFromBitmapIdToAknsItemId( aBitmapId, itemId, colorGroupId );

    if ( colorGroupId != 0 )
        {
        CHAT_DP_TXT( "CCASkinVariant20::LoadBitmapL - color change group" );

        if ( aBitmapId == EMbmChatngQgn_prop_im_tome
             || aBitmapId == EMbmChatngQgn_prop_im_from )
            {
            // Special case, these icons are using the same color
            // and color group as text
            if ( aCustomColor )
                {
                // If user has set a special color for text
                AknIconUtils::CreateIconLC( bitmap, mask, aFullPath,
                                            aBitmapId, aMaskId );
                AknIconUtils::SetIconColor( bitmap, *aCustomColor );
                }
            else
                {
                AknsUtils::CreateColorIconLC( skin,
                                              itemId, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6,
                                              bitmap, mask, aFullPath, aBitmapId, aMaskId, KRgbBlack );
                }
            }
        else
            {
            AknsUtils::CreateColorIconLC( skin,
                                          itemId, KAknsIIDQsnIconColors, colorGroupId,
                                          bitmap, mask, aFullPath, aBitmapId, aMaskId, KRgbBlack );
            }
        }
    else
        {
        AknsUtils::CreateIconLC( skin,
                                 itemId,
                                 bitmap, mask, aFullPath, aBitmapId, aMaskId );
        }

    CGulIcon* gulIcon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 ); // bitmap, mask

    CHAT_DP_TXT( "CCASkinVariant20::LoadBitmapL - returning skinned or default \
                  icon" );
    return gulIcon;
    }

// -----------------------------------------------------------------------------
// CCASkinVariant20::MapFromBitmapIdToAknsItemId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASkinVariant20::MapFromBitmapIdToAknsItemId( TInt aBitmapId,
                                                    TAknsItemID& aItemId, TInt& aColorGroupId )
    {
    for ( TInt i( 0 ); KCABitmaps[i].iBitmapId != KErrNotFound; i++ )
        {
        if ( KCABitmaps[ i ].iBitmapId == aBitmapId )
            {
            // found it
            CHAT_DP( D_CHAT_LIT( "CCASkinVariant20::MapFromBitmapIdToAknsItemId \
                                  - id found for %d!" ), aBitmapId );
            aItemId = *( KCABitmaps[ i ].iItemId );
            aColorGroupId = KCABitmaps[ i ].iColorGroupId;
            return;
            }
        }

    // not found
    CHAT_DP( D_CHAT_LIT( "CCASkinVariant20::MapFromBitmapIdToAknsItemId - \
                          id not found for %d!" ), aBitmapId );
    CHAT_DP_TXT( "!!!!!!!! every bitmap enumeration should be found from \
                  CABitmapMapping.h !!!!!!!! " );

    __CHAT_ASSERT_DEBUG( EFalse );
    }


//  End of File
