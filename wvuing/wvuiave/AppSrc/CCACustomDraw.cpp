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
* Description:  Utility class for drawing skins
*
*/



// INCLUDE FILES
#include    "CCACustomDraw.h"

#include 	<aknsskininstance.h>
#include 	<AknsUtils.h>
#include 	<AknsDrawUtils.h>
#include 	<eikenv.h>
#include 	<eikappui.h>

#include	<aknsdrawutils.h>
#include	<AknsBasicBackgroundControlContext.h>
#include	<BidiText.h>	// A/H
#include	<AknBidiTextUtils.h>
#include	<AknLayout.cdl.h> //cdl 
#include	<AppLayout.cdl.h> //cdl 
#include    <aknutils.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCACustomDraw::CCACustomDraw
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCACustomDraw::CCACustomDraw( TRect aViewRect )
        : iViewRect( aViewRect )
    {
    }

// Destructor
CCACustomDraw::~CCACustomDraw()
    {

    }

// -----------------------------------------------------------------------------
// CCACustomDraw::SetRect
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCACustomDraw::SetRect( const TRect& aRect )
    {
    iViewRect = aRect;
    }

// -----------------------------------------------------------------------------
// CCACustomDraw::DrawBackground
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCACustomDraw::DrawBackground( const TParam& aParam,
                                    const TRgb& aBackground,
                                    TRect& aDrawn ) const
    {
    if ( aBackground != KRgbWhite || !iBgContext )
        {
        // draw background with color
        aParam.iGc.SetPenStyle( CGraphicsContext::ENullPen );
        aParam.iGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        aParam.iGc.SetBrushColor( aBackground );
        aParam.iGc.DrawRect( aParam.iDrawRect );
        }
    // if background is white -> we draw the skinned background
    else
        {
        //Draw the skin background
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsDrawUtils::Background( skin,
                                   iBgContext,
                                   iParent,
                                   static_cast<CWindowGc&>( aParam.iGc ),
                                   aParam.iDrawRect );

        }
    aDrawn = aParam.iDrawRect;
    }

// -----------------------------------------------------------------------------
// CCACustomDraw::SetBackgroundContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCACustomDraw::SetBackgroundContext( CAknsBasicBackgroundControlContext* aBgContext,
                                          CCoeControl* aParent )
    {
    iBgContext = aBgContext;
    iParent = aParent;
    }

//  End of File
