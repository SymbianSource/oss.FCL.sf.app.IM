/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Skin variant implementation for 1.2. Actually
*				 implements the behaviour without the skin.
*
*/



// INCLUDE FILES
#include    "CCASkinVariant12.h"

#include	<e32base.h>
#include	<w32std.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCASkinVariant12::CCASkinVariant12
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCASkinVariant12::CCASkinVariant12()
    {
    }

// -----------------------------------------------------------------------------
// CCASkinVariant12::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCASkinVariant12::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CCASkinVariant12::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCASkinVariant12* CCASkinVariant12::NewL()
    {
    CCASkinVariant12* self = new( ELeave ) CCASkinVariant12;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CCASkinVariant12::~CCASkinVariant12()
    {

    }

TInt CCASkinVariant12::SkinFlag() const
    {
    return 0; // 0 is default parameter on BaseConstructL
    }

void CCASkinVariant12::ClearChatListBoxBackGround(  CWindowGc& aGc,
                                                    const TRect& aUsedPortionOfViewRect, MAknsControlContext* /*aContext*/ )
    {
    aGc.Clear( aUsedPortionOfViewRect );
    }

MAknsControlContext* CCASkinVariant12::MainPaneBackgroundContext() const
    {
    return NULL;
    }

TBool CCASkinVariant12::DrawSkinnedEditor( const CCoeControl* /*aControl*/,
                                           CWindowGc& /*aGc*/,
                                           TRect& /*aParentRect*/,
                                           TRect& /*aEditorRect*/,
                                           TRect& /*aOutlineRect*/ )
    {
    return EFalse;
    }


CGulIcon* CCASkinVariant12::LoadBitmapL( TInt /*aBitmapId*/, TInt /*aMaskId*/, const TDesC& /*aFullPath*/, TBool /*aOverrideBrand*/ )
    {
    return NULL;
    }

//  End of File
