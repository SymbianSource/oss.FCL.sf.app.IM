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
* Description:  Container class for main view
*
*/


// INCLUDE FILES
#include	"CCAEmptyControl.h"
#include	"CAExternalInterface.h"
#include    "ChatDefinitions.h"
#include	"CCAAppUi.h"
#include    "CCAIconLoader.h"
#include    "ChatDebugPrint.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#include	<aknlists.h>
#include	<barsread.h>
#include	<EIKCLBD.H>
#include	<chatNG.rsg>
#include	<csxhelp/imng.hlp.hrh>
#include	<gulicon.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCAEmptyControl::CCAEmptyControl
// Constructor
// -----------------------------------------------------------------------------
//
CCAEmptyControl::CCAEmptyControl()
    {
    }

// -----------------------------------------------------------------------------
// CCAEmptyControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAEmptyControl* CCAEmptyControl::NewL( const TRect& aRect )
    {
    CCAEmptyControl* self = new ( ELeave ) CCAEmptyControl;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CCAEmptyControl::ConstructL()
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
void CCAEmptyControl::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    SetRect( aRect );
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CCAEmptyControl::~CCAEmptyControl
// Destructor
// -----------------------------------------------------------------------------
//
CCAEmptyControl::~CCAEmptyControl()
    {
    }

// End of File
