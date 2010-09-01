/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  AppUi class of application
*  Since			 : 3.2
*
*/


// INCLUDE FILES
#include <gulicon.h>
#include <aknutils.h>
#include <akniconutils.h>
#include <LayoutMetaData.cdl.h>
#include  "CCASplashScreen.h"
#include  "CCASplashScreenControl.h"
#include  "CCAAppUi.h"
#include  <ChatNG.mbg>

_LIT( KFileName, "z:\\resource\\apps\\chatng.mif" );

CCASplashScreenControl* CCASplashScreenControl::NewL( CCASplashScreen &aParent )
    {
    CCASplashScreenControl* self = CCASplashScreenControl::NewLC( aParent );
    CleanupStack::Pop(); //self
    return self;
    }

CCASplashScreenControl* CCASplashScreenControl::NewLC( CCASplashScreen &aParent )
    {
    CCASplashScreenControl* self = new ( ELeave ) CCASplashScreenControl( aParent );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCASplashScreenControl::CCASplashScreenControl( CCASplashScreen &aParent )
        : iParent( aParent ), iSplashDismissed( EFalse )
    {
    }

void CCASplashScreenControl::ConstructL()
    {
    CreateWindowL();
    SetExtentToWholeScreen();
    ActivateL();
    MakeVisible( ETrue );
    }

CCASplashScreenControl::~CCASplashScreenControl()
    {
    delete iSplashImage;
    }

void CCASplashScreenControl::SizeChanged()
    {
    }

// ---------------------------------------------------------
// CCASplashScreenControl::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CCASplashScreenControl::CountComponentControls() const
    {
    return 0; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CCASplashScreenControl::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CCASplashScreenControl::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
            //case 0:
            //    return iLabel;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CCASplashScreenControl::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CCASplashScreenControl::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    if ( iSplashImage )
        {
        TSize splashSize = iSplashImage->Bitmap()->SizeInPixels();
//        DLINFO(( "Splash image size: width=%d height=%d",
//              splashSize.iWidth, splashSize.iHeight ));

        // calculate the splash image upper left point position
        // so that the image is centered.
        TInt leftPadding = 0;
        TInt upperPadding = 0;
        if ( splashSize.iWidth < Rect().Width() )
            {
            leftPadding = ( Rect().Width() - splashSize.iWidth ) / 2;
            }

        if ( splashSize.iHeight < Rect().Height() )
            {
            upperPadding = ( Rect().Height() - splashSize.iHeight ) / 2;
            }

        TRect rect = TRect( TPoint( 0, 0 ), splashSize );
        gc.Clear(); //(rect);
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.BitBltMasked( TPoint( leftPadding, upperPadding ), iSplashImage->Bitmap(), rect, iSplashImage->Mask(), EFalse );
        }

    }


TKeyResponse CCASplashScreenControl::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( ( iDismissable && !iSplashDismissed ) || ( ( aType == EEventKey ) && ( aKeyEvent.iCode == EKeyNo ) ) )
        {
        iParent.DismissSplashScreen();
        iSplashDismissed = ETrue;

        return EKeyWasConsumed;
        }
    else
        {
        return EKeyWasNotConsumed;
        }


    }

// ---------------------------------------------------------
// CCASplashScreenControl::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CCASplashScreenControl::HandleControlEventL(
    CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/ )
    {
    }

void CCASplashScreenControl::ShowSplashScreen( CGulIcon* aSplashImage,
                                               /*MNcdSkinData* aSkinData,*/
                                               TBool aDismissable )
    {
    delete iSplashImage;
    iSplashImage = NULL;

    iSplashImage = aSplashImage;

    iDismissable = aDismissable;

//    iSkinData = aSkinData;
    }

void CCASplashScreenControl::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    TRect rect = iEikonEnv->EikAppUi()->ApplicationRect();
    SetRect( rect );

    TSize screenSize;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen,
                                       screenSize );
    if ( LandscapeOrientation() )
        {
        delete iSplashImage;
        iSplashImage = NULL;

        CFbsBitmap* aChangedBitmap;
        CFbsBitmap* aChangedMask;
        AknIconUtils::CreateIconL( aChangedBitmap, aChangedMask, KFileName, EMbmChatngQgn_graf_im_splash_screen, EMbmChatngQgn_graf_im_splash_screen_mask );
        AknIconUtils::SetSize( aChangedBitmap, screenSize, EAspectRatioPreserved );
        AknIconUtils::SetSize( aChangedMask, screenSize, EAspectRatioPreserved );

        CleanupStack::PushL( aChangedBitmap );
        CleanupStack::PushL( aChangedMask );
        iSplashImage = CGulIcon::NewL( aChangedBitmap, aChangedMask );
        CleanupStack::Pop( 2 ); // bitmap and mask
        }
    DrawNow();
    /*
        TRect rect = iEikonEnv->EikAppUi()->ApplicationRect();

        SetRect( rect );

        delete iSplashImage;
        iSplashImage = NULL;

        if ( InLandScapeMode() )
            {
            TRAP_IGNORE( iSplashImage = iSkinData->GetSplashScreenLandscapeL() );
            }
        else
            {
            TRAP_IGNORE( iSplashImage = iSkinData->GetSplashScreenPortraitL() );
            }
    */
    //  DrawDeferred();
    //	DrawNow();
    }

// ----------------------------------------------------------------------------
// CCASplashScreenControl::LandscapeOrientation
// ----------------------------------------------------------------------------
//
TBool CCASplashScreenControl::LandscapeOrientation() const
    {
    TBool landscape = EFalse;
    landscape = Layout_Meta_Data::IsLandscapeOrientation();
    return landscape;
    }


// End of File
