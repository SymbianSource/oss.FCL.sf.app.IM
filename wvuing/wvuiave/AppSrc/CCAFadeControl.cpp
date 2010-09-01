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
* Description:  Fader control in navi pane
*
*/


// INCLUDE FILES
#include    "CCAFadeControl.h"
#include	"MCAFadeTextObserver.h"
#include    "ChatDefinitions.h"
#include	"ChatDebugPrint.h"
#include    "LayoutDebugPrint.h"
#include	"Impserrors.h"
#include	"CCAAppUi.h"
#include	"MCALayoutInfo.h"

#include	<eikenv.h>
#include	<aknutils.h>
#include	<aknnavi.h>
#include	<aknconsts.h>
#include    <aknenv.h>
#include    <applayout.cdl.h>
#include    <AknsDrawUtils.h>
#include    <AknsBasicBackgroundControlContext.h>


//CONSTANTS
const TInt KLafCount( 10 ); //Count of LAF indexes
const TInt KDelayColorIndex( 5 ); //From LAF: Color 0 is in index 5
const TTimeIntervalMicroSeconds32 KIndicationDelay( 4000000 ); //4 seconds waiting
const TUint16 KAIMMessageIcon( 0xF808 ); //PUA icon code for AIMMESSAGE

// ========================= MEMBER FUNCTIONS ================================

// -----------------------------------------------------------------------------
// CCAFadeControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAFadeControl* CCAFadeControl::NewL( const TRect& aRect )
    {
    CCAFadeControl* self = CCAFadeControl::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAFadeControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAFadeControl* CCAFadeControl::NewLC( const TRect& aRect )
    {
    CCAFadeControl* self = new ( ELeave ) CCAFadeControl();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// ---------------------------------------------------------------------------
// CCAFadeControl::ConstructL(const TRect& aRect)
// Symbian OS two phased constructor
//
// ---------------------------------------------------------------------------
//
void CCAFadeControl::ConstructL( const TRect& aRect )
    {

    iLayoutInfo = static_cast<CCAAppUi*>( iEikonEnv->AppUi() )->
                  LayoutInfo();
    // Currently we have 2 observers, so granularity of 2 is logical choice.
    iObserverArray = new ( ELeave ) CArrayPtrSeg<MCAFadeTextObserver>( 2 );
    iIMessage = HBufC::NewL( KMaxWVIDLength + 1 );//1 is indication character
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    iBgContext = CAknsBasicBackgroundControlContext::NewL(
                     KAknsIIDQsnBgNavipaneSolid, aRect, EFalse );
    iViewRect = TRect( aRect );
    CreateWindowL();
    MakeVisible( EFalse );
    }

// Constructor
CCAFadeControl::CCAFadeControl()
        : iTimerState( ETimerStopped )
    {

    }

// Destructor
CCAFadeControl::~CCAFadeControl()
    {

    if ( iObserverArray )
        {
        iObserverArray->Reset();
        delete iObserverArray;
        }
    delete iTimer;
    delete iIMessage;
    delete iClippedMessage;
    delete iBgContext;
    iTextLayouts.Close();
    }

// ---------------------------------------------------------------------------
// CCAFadeControl::SizeChanged()
// called by framework when the view size is changed
//
// ---------------------------------------------------------------------------
//
void CCAFadeControl::SizeChanged()
    {
    TRect myNavePaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ENaviPane,
                                       myNavePaneRect );

    LAYOUT_DP( D_LAYOUT_LIT( "CCAFadeControl::SizeChanged %d %d %d %d" ),
               myNavePaneRect.iTl.iX, myNavePaneRect.iTl.iY,
               myNavePaneRect.iBr.iX, myNavePaneRect.iBr.iY );

    myNavePaneRect.Move( -myNavePaneRect.iTl.iX, -myNavePaneRect.iTl.iY );

    iTextLayouts.Reset();
    for ( TInt i( 0 ); i < KLafCount; i++ )
        {
        TAknLayoutText text;
        TAknTextLineLayout textLayout;
        textLayout = iLayoutInfo->IM_navi_pane_texts_Line_1( i, 0 );

        text.LayoutText( myNavePaneRect, textLayout );
        iTextLayouts.Append( text );
        LAYOUT_DP( D_LAYOUT_LIT(
                       "CCAFadeControl::SizeChanged textRect %d %d %d %d" ),
                   text.TextRect().iTl.iX, text.TextRect().iTl.iY,
                   text.TextRect().iBr.iX, text.TextRect().iBr.iY );
        }
    TAknWindowLineLayout navipaneLayout =
        AknLayout::Status_pane_elements_Line_1();
    iNavipaneRect.LayoutRect( myNavePaneRect, navipaneLayout );
    LAYOUT_DP( D_LAYOUT_LIT( "CCAFadeControl::SizeChanged %d %d %d %d" ),
               iNavipaneRect.Rect().iTl.iX, iNavipaneRect.Rect().iTl.iY,
               iNavipaneRect.Rect().iBr.iX, iNavipaneRect.Rect().iBr.iY );

    UpdateClippedMessage();
    }

// ---------------------------------------------------------------------------
// CCAFadeControl::CountComponentControls() const
//
//
// ---------------------------------------------------------------------------
//
TInt CCAFadeControl::CountComponentControls() const
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// CCAFadeControl::ComponentControl(TInt aIndex) const
//
//
// ---------------------------------------------------------------------------
//
CCoeControl* CCAFadeControl::ComponentControl( TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCAFadeControl::Draw(const TRect& aRect) const
//
//
// ---------------------------------------------------------------------------
//
void CCAFadeControl::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = iBgContext;
    if ( !AknsDrawUtils::Background( skin, cc, gc, iViewRect ) )
        {
        gc.Clear( iViewRect );
        }
    iTextLayouts[iColorIndex].DrawText( gc, *iClippedMessage );

    LAYOUT_DP( D_LAYOUT_LIT( "CCAFadeControl::Draw TextRect %d %d %d %d" ),
               iTextLayouts[iColorIndex].TextRect().iTl.iX,
               iTextLayouts[iColorIndex].TextRect().iTl.iY,
               iTextLayouts[iColorIndex].TextRect().iBr.iX,
               iTextLayouts[iColorIndex].TextRect().iBr.iY );
    }

// ---------------------------------------------------------------------------
// CCAFadeControl::HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType)
//
//
// ---------------------------------------------------------------------------
//
void CCAFadeControl::HandleControlEventL( CCoeControl* /*aControl*/,
                                          TCoeEvent /*aEventType*/ )
    {
    }

// ---------------------------------------------------------
// CCAFadeControl::FadeText()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAFadeControl::FadeText( TAny* aAnyPtr )
    {
    if ( !aAnyPtr )
        {
        return EFalse;
        }

    CCAFadeControl* view = reinterpret_cast< CCAFadeControl* >( aAnyPtr );

    if ( view->iColorIndex < KLafCount - 1 )
        {
        view->iColorIndex++;
        view->ReportChange();
        if ( view->iColorIndex == KDelayColorIndex )
            {
            //Delayed color is displayed 4 seconds
            view->StartTicker( KIndicationDelay );
            }
        return ETrue;
        }

    view->iColorIndex = 0;
    view->Stop();
    TInt count( view->iObserverArray->Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        MCAFadeTextObserver* obs = view->iObserverArray->At( i );
        if ( obs )
            {
            obs->NotifyWhenCompleted();
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCAFadeControl::Start()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::Start()
    {
    TRect myNavePaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ENaviPane,
                                       myNavePaneRect );
    myNavePaneRect.Move( -myNavePaneRect.iTl.iX, -myNavePaneRect.iTl.iY );

    TAknWindowLineLayout navipanelayout = AknLayout::navi_pane( 0 );
    AknLayoutUtils::LayoutControl( this, myNavePaneRect, navipanelayout );

    iTimerTick = KTimerInterval;

    if ( !IsActivated() )
        {
        TRAPD( err, ActivateL() );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }

    if ( !IsVisible() )
        {
        MakeVisible( ETrue );
        }

    StartTicker( 0 );
    }

// ---------------------------------------------------------
// CCAFadeControl::StartTicker()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::StartTicker(
    const TTimeIntervalMicroSeconds32 aDelayInternal )
    {
    if ( iTimerState == ETimerActivated )
        {
        iTimer->Cancel();
        iTimerState = ETimerStopped;
        }
    TCallBack cb( FadeText, this );
    iTimer->Start( aDelayInternal, iTimerTick, cb );
    iTimerState = ETimerActivated;
    }

// ---------------------------------------------------------
// CCAFadeControl::Stop()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::Stop()
    {
    if ( IsVisible() )
        {
        MakeVisible( EFalse );
        }
    StopTicker();
    }

// ---------------------------------------------------------
// CCAFadeControl::AddObserver()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::AddObserver( MCAFadeTextObserver* aObserver )
    {
    TRAPD( err, iObserverArray->AppendL( aObserver ) );
    HandleError( err );
    }

// ---------------------------------------------------------
// CCAFadeControl::SetMessage()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::SetMessage( const TDesC& aMsg )
    {
    HBufC* temp = NULL;
    TRAPD( err, temp = HBufC::NewL( aMsg.Length() + 1 ) );
    HandleError( err );
    if ( temp )
        {
        delete iIMessage;
        iIMessage = temp;
        TPtr imMsgPtr( iIMessage->Des() );
        imMsgPtr.Append( TChar( KAIMMessageIcon ) );
        imMsgPtr.Append( aMsg );

        UpdateClippedMessage();
        }
    }

// ---------------------------------------------------------
// CCAFadeControl::Message()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC CCAFadeControl::Message() const
    {
    return *iIMessage;
    }

// ---------------------------------------------------------
// CCAFadeControl::IsActive()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAFadeControl::IsActive()
    {
    if ( iTimer )
        {
        return iTimer->IsActive();
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCAFadeControl::Reset()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::Reset()
    {
    if ( iIMessage )
        {
        TPtr mesgPtr( iIMessage->Des() );
        mesgPtr.Zero();
        }

    if ( iClippedMessage )
        {
        TPtr clipPtr( iClippedMessage->Des() );
        clipPtr.Zero();
        DrawNow();
        }
    }

// ---------------------------------------------------------
// CCAFadeControl::StopTicker()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::StopTicker()
    {
    if ( iTimerState == ETimerActivated )
        {
        iTimer->Cancel();
        iTimerState = ETimerStopped;
        }
    }

// ---------------------------------------------------------
// CCAFadeControl::ReportChange()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::ReportChange()
    {
    TRAPD( err, ReportEventL( MCoeControlObserver::EEventStateChanged ) );
    HandleError( err );
    }

// ---------------------------------------------------------
// CCAFadeControl::HandleError
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::HandleError( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT( "CCAFadeControl::HandleError, aError = %d" ), aError );
    if ( ( aError > Imps_ERROR_BASE ) && ( aError < KErrNone ) )
        {
        // propagate system errors to current active scheduler,
        // it should show a note
        CActiveScheduler::Current()->Error( aError );
        }

    // imps errors are ignored at the moment as we don't have any
    // notes specified for them
    }

// ---------------------------------------------------------
// CCAFadeControl::UpdateClippedMessage
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAFadeControl::UpdateClippedMessage()
    {
    HBufC* temp = NULL;
    TRAPD( err, temp = iIMessage->AllocL() );
    if ( err )
        {
        delete temp;
        HandleError( err );
        }

    if ( temp )
        {
        delete iClippedMessage;
        iClippedMessage = temp;
        TPtr clipPtr( iClippedMessage->Des() );

        const CFont* font = iTextLayouts[iColorIndex].Font();
        AknTextUtils::ClipToFit( clipPtr, *font, Rect().Width() );
        }
    }

// End of file
