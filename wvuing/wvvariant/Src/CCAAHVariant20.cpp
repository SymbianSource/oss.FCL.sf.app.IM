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
* Description:  Concrete implementation of res.file& bitmap changing feature.
*
*/



// INCLUDE FILES
#include    "CCAAHVariant20.h"
#include	"CCAStatusPaneHandler.h"
#include	"CCAAppUi.h"

#include	<akntabgrp.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAAHVariant20::CCAAHVariant20
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAAHVariant20::CCAAHVariant20()
    {
    }

// -----------------------------------------------------------------------------
// CCAAHVariant20::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAAHVariant20* CCAAHVariant20::NewL()
    {
    CCAAHVariant20* self = new( ELeave ) CCAAHVariant20;
    return self;
    }


// Destructor
CCAAHVariant20::~CCAAHVariant20()
    {
    }

CAknNavigationDecorator*  CCAAHVariant20::CreateTabGroupL(
    CAknNavigationControlContainer& aNaviPane,
    CCAAppUi& aAppUi,
    CCAStatusPaneHandler& aStatusPane )
    {
    // set values to be used for tab changing
    iNaviPane = &aNaviPane;
    iAppUi = &aAppUi;
    iStatusPane = &aStatusPane;

    return iNaviPane->CreateTabGroupL( this );
    }


TBool CCAAHVariant20::HandleTabsL( const TKeyEvent& aKeyEvent,
                                   TEventCode aType )
    {
    TKeyResponse keyResp;
    keyResp = iStatusPane->TabGroup()->OfferKeyEventL( aKeyEvent, aType );
    if ( keyResp == EKeyWasConsumed )
        {
        return ETrue;
        }
    return EFalse;
    }

void CCAAHVariant20::TabChangedL( TInt aIndex )
    {
    iStatusPane->ActivateTab( aIndex );
    iAppUi->ActivateLocalViewL( iStatusPane->ActiveTabViewId() );
    }

//  End of File
