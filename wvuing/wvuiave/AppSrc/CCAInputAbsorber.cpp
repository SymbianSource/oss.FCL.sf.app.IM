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
* Description:  Instance of this class catches key events when activated
*
*/



// INCLUDE FILES
#include    "CCAInputAbsorber.h"
#include	<eikenv.h>
#include	<eikappui.h>
#include	<avkon.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAInputAbsorber::CCAInputAbsorber
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAInputAbsorber::CCAInputAbsorber()
    {
    }

// -----------------------------------------------------------------------------
// CCAInputAbsorber::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAInputAbsorber::ConstructL()
    {
    CreateWindowL();
    SetExtent( TPoint( 0, 0 ), TSize( 0, 0 ) );
    ActivateL();
    ClaimPointerGrab( ETrue );
    iAppUi = iEikonEnv->EikAppUi();
    }

// -----------------------------------------------------------------------------
// CCAInputAbsorber::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAInputAbsorber* CCAInputAbsorber::NewL()
    {
    CCAInputAbsorber* self = new( ELeave ) CCAInputAbsorber;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCAInputAbsorber::~CCAInputAbsorber()
    {
    if ( iIsCaptured )
        {
        if ( iAppUi )
            {
            iAppUi->RemoveFromStack( this );
            }
        }
    }

// ---------------------------------------------------------
// CCAInputAbsorber::CaptureEventsL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInputAbsorber::CaptureEventsL()
    {
    if ( !iIsCaptured )
        {
        iAppUi->AddToStackL( this );
        SetPointerCapture();
        }

    iIsCaptured = ETrue;
    }

// ---------------------------------------------------------
// CCAInputAbsorber::Release()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInputAbsorber::Release()
    {
    if ( iIsCaptured )
        {
        iAppUi->RemoveFromStack( this );
        SetPointerCapture( EFalse );
        }

    iIsCaptured = EFalse;
    }


// ---------------------------------------------------------
// CCAInputAbsorber::IsCaptured()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAInputAbsorber::IsCaptured() const
    {
    return iIsCaptured;
    }


// -----------------------------------------------------------------------------
// CCAInputAbsorber::OfferKeyEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CCAInputAbsorber::OfferKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                               TEventCode /*aType*/ )
    {
    // consume all keys
    return EKeyWasConsumed;
    }

// -----------------------------------------------------------------------------
// CCAInputAbsorber::HandlePointerEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAInputAbsorber::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    // For debugging purposes.
    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

//  End of File
