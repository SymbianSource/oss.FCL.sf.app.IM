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
*  Since	   : 3.2
*
*/


// INCLUDE FILES
#include <aknsbasicbackgroundcontrolcontext.h>
#include <aknsskininstance.h>
#include <aknsutils.h>
#include <aknutils.h>
#include <aknsdrawutils.h>
#include <gulicon.h>
#include <akniconutils.h>
#include <fbs.h>
#include <BAUTILS.h>
#include <eikbtgpc.h>
#include <stringloader.h>
#include <eikon.hrh>
#include <avkon.hrh>
#include <AknBidiTextUtils.h>
#include <LayoutMetaData.cdl.h>
#include "CCnUiSignInControl.h"
#include "MCnUiWaitNoteObserver.h"
#include <IMPSConnectionUiNG.rsg>
#include <ChatNG.mbg>
#include <AnimationConfig.h>
#include <pathinfo.h>

const TInt KMaxLength = 28;
const TInt KDriveAndColon = 2;
_LIT( KFileName, "z:\\resource\\apps\\chatng.mif" );
_LIT( KGifFileName, "signon.gif" );

CCnUiSignInControl* CCnUiSignInControl::NewL( const TDesC& aLabelText, MCnUiWaitNoteObserver* aObserver )
    {
    CCnUiSignInControl* self = CCnUiSignInControl::NewLC( aLabelText, aObserver );
    CleanupStack::Pop(); //self
    return self;
    }

CCnUiSignInControl* CCnUiSignInControl::NewLC( const TDesC& aLabelText, MCnUiWaitNoteObserver* aObserver )
    {
    CCnUiSignInControl* self = new ( ELeave ) CCnUiSignInControl( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aLabelText );
    return self;
    }

CCnUiSignInControl::CCnUiSignInControl( MCnUiWaitNoteObserver* aObserver ) : iObserver( aObserver )
    {
    }

void CCnUiSignInControl::ConstructL( const TDesC& aLabelText )
    {
    CreateWindowL();
    TRect clientRect;
    AknLayoutUtils::LayoutMetricsRect ( AknLayoutUtils::EMainPane, clientRect );
    SetCanDrawOutsideRect();

//////////create status label/////////////////////////////////////////////////
    iLabel = new( ELeave ) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetLabelAlignment( ELayoutAlignCenter );
    iLabel->SetBrushStyleFromContext();
    if ( AknLayoutUtils::Variant() == EApacVariant )
        {
        iLabel->SetFont( ApacPlain16() );
        }
    else
        {
        iLabel->SetFont( LatinBold17() );
        }
    AssignSignInText( aLabelText );

    TInt xPos = clientRect.iTl.iX + 20;
    TInt yPos = ( clientRect.iBr.iY * 9 ) / 16;
    iLabel->SetExtent( TPoint( xPos, yPos ), TSize( clientRect.iBr.iX - 40, 60 ) );
    iLabel->SetBackground( this );
    iLabel->MakeVisible( ETrue );
/////////////////////////////////////////////////////////////////////////////////

///////////Sign-in Background////////////////////////////////////////////////////
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    TSize size;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EMainPane, size );
    AknIconUtils::CreateIconL( bitmap, mask, KFileName, EMbmChatngQgn_graf_im_signin_screen, EMbmChatngQgn_graf_im_signin_screen_mask );
    AknIconUtils::SetSize( bitmap, size, EAspectRatioPreserved );
    AknIconUtils::SetSize( mask, size, EAspectRatioPreserved );

    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );
    iImage = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 ); // bitmap and mask
//////////////////////////////////////////////////////////////////////////////////

////////////Sign-in Animation/////////////////////////////////////////////////////
    TInt err = iFs.Connect();
    if ( err )
        {
        User::Leave( err );
        }
    // Get ROM and C drives
    TBuf<KDriveAndColon> romDrive;
    romDrive = PathInfo::RomRootPath().Left( KDriveAndColon );

    TFileName fn;
    iFs.PrivatePath( fn );
    BaflUtils::EnsurePathExistsL( iFs, fn );
    fn.Insert( 0, romDrive );
    fn.Append( KGifFileName );

    CICLAnimationDataProvider* aGifProvider;
    aGifProvider = new ( ELeave ) CICLAnimationDataProvider;
    CleanupStack::PushL( aGifProvider );
    aGifProvider->SetFileL( iFs, fn );
    CleanupStack::Pop( aGifProvider );

    TAnimationConfig aGifConfig;
    aGifConfig.iFlags = TAnimationConfig::ELoop;
    aGifConfig.iData = 100;

    iGifImage = CBasicAnimation::NewL( aGifProvider , TPoint( 0, 0 ), CEikonEnv::Static()->WsSession(), Window() );
    TInt animXPos = clientRect.Width() / 2 - 20;
    TInt animYPos = yPos - 45;
    iGifImage->SetPosition( TPoint( animXPos, animYPos ) );
    iGifImage->Start( aGifConfig );
////////////////////////////////////////////////////////////////////////////////////

////////Create a CBA for use with this control//////////////////////////////////////
    iPopoutCba = CEikButtonGroupContainer::NewL( CEikButtonGroupContainer::ECba,
                                                 CEikButtonGroupContainer::EHorizontal, this, R_AVKON_SOFTKEYS_CANCEL );

    iPopoutCba->MakeVisible( ETrue );
////////////////////////////////////////////////////////////////////////////////////

    // Sets rectangle of frame.
    SetRect( clientRect );

    ActivateL();
    // Activates the window. (Ready to draw
    MakeVisible( ETrue );
    }

// Fix codescanner warning: Pointer access in destructors
void CCnUiSignInControl::MakeCbaInvisible()
    {
    iPopoutCba->MakeVisible( EFalse );
    }
CCnUiSignInControl::~CCnUiSignInControl()
    {
    iObserver = NULL;

    MakeVisible( EFalse );
    delete iLabel;

    MakeCbaInvisible();
    delete iPopoutCba;

    delete iImage;
    iFs.Close();
    delete iGifImage;
    }

void CCnUiSignInControl::SizeChanged()
    {
    //
    }

// ---------------------------------------------------------
// CCnUiSignInControl::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CCnUiSignInControl::CountComponentControls() const
    {
    return 1; // return numberr of controls inside this container
    }

// ---------------------------------------------------------
// CCnUiSignInControl::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CCnUiSignInControl::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iLabel;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CCnUiSignInControl::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CCnUiSignInControl::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
//    gc.SetPenStyle( CGraphicsContext::ENullPen );
//    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
//    gc.DrawRect( aRect );

    if ( iImage )
        {
        //TSize imageSize = iImage->Bitmap()->SizeInPixels();
        TSize imageSize;
        AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EMainPane, imageSize );
        TInt leftPadding = 0;
        TInt upperPadding = 0;
        if ( imageSize.iWidth < Rect().Width() )
            {
            leftPadding = ( Rect().Width() - imageSize.iWidth ) / 2;
            }

        if ( imageSize.iHeight < Rect().Height() )
            {
            upperPadding = ( Rect().Height() - imageSize.iHeight ) / 2;
            }

        gc.Clear(); //(rect);
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.BitBltMasked( TPoint( leftPadding, upperPadding ), iImage->Bitmap(), aRect, iImage->Mask(), EFalse );
        }

    if ( iGifImage )
        {
        iGifImage->Draw( gc );
        }
    }

void CCnUiSignInControl::Draw( CWindowGc& /*aGc*/, const CCoeControl& /*aControl*/, const TRect& /*aRect*/ ) const
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    }

void CCnUiSignInControl::ProcessCommandL( TInt aCommandId )
    {
    //TO DO
    // Respond to softkey events
    switch ( aCommandId )
        {
        case EAknSoftkeyCancel:
            if ( iObserver )
                {
                iObserver->HandleWaitNoteCancel();
                }
            break;
        }
    }

void CCnUiSignInControl::HideStatus()
    {
    iPopoutCba->MakeVisible( EFalse );
    }

void CCnUiSignInControl::StopAnimation()
    {
    iGifImage->Stop();
    }
void CCnUiSignInControl::AssignSignInText( const TDesC& aLabelText )
    {
    TInt aTempLength = aLabelText.Length();
    if ( aTempLength > KMaxLength )
        {
        TPtrC aTempPtr;
        aTempPtr.Set( aLabelText.Left( KMaxLength ) );
        TBuf<56> aTruncateRecipient( aTempPtr );
        aTruncateRecipient.Append( _L( "\n" ) );
        aTruncateRecipient.Append( aLabelText.Right( aTempLength - KMaxLength ) );
        iLabel->SetTextL( aTruncateRecipient );
        }
    else
        {
        iLabel->SetTextL( aLabelText );
        }
    }
TKeyResponse CCnUiSignInControl::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( ( aType == EEventKey ) && ( aKeyEvent.iCode == EKeyOK ) )
        {
        if ( iObserver )
            {
            iObserver->HandleWaitNoteCancel();
            }
        return EKeyWasConsumed;
        }
    return EKeyWasNotConsumed;
    }

void CCnUiSignInControl::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect ( AknLayoutUtils::EMainPane, rect );
    SetRect( rect );

    TInt xPos = rect.iTl.iX + 20;
    TInt yPos = ( rect.iBr.iY * 9 ) / 16;

    if ( LandscapeOrientation() )
        {
        delete iImage;
        iImage = NULL;

        CFbsBitmap* aChangedBitmap;
        CFbsBitmap* aChangedMask;
        AknIconUtils::CreateIconL( aChangedBitmap, aChangedMask, KFileName, EMbmChatngQgn_graf_im_signin_screen, EMbmChatngQgn_graf_im_signin_screen_mask );
        AknIconUtils::SetSize( aChangedBitmap, rect.Size(), EAspectRatioPreserved );
        AknIconUtils::SetSize( aChangedMask, rect.Size(), EAspectRatioPreserved );

        CleanupStack::PushL( aChangedBitmap );
        CleanupStack::PushL( aChangedMask );
        iImage = CGulIcon::NewL( aChangedBitmap, aChangedMask );
        CleanupStack::Pop( 2 ); // bitmap and mask

        iLabel->SetExtent( TPoint( xPos, yPos ), TSize( rect.iBr.iX - 40, 60 ) );
        }
    DrawNow();
    }

// ----------------------------------------------------------------------------
// CCnUiSignInControl::LandscapeOrientation
// ----------------------------------------------------------------------------
//
TBool CCnUiSignInControl::LandscapeOrientation() const
    {
    TBool landscape = EFalse;
    landscape = Layout_Meta_Data::IsLandscapeOrientation();
    return landscape;
    }

// End of File
