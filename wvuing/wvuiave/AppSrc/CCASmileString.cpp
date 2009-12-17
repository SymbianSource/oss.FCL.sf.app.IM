/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for storing one smiley with index to icon array.
*
*/


// INCLUDE FILES
#include "ccasmilestring.h"

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCASmileString* CCASmileString::NewL()
    {
    CCASmileString* self = new ( ELeave ) CCASmileString();
    return self;
    }

// Destructor
CCASmileString::~CCASmileString()
    {
    delete iSmiley;
    delete iShortestSmileyString;
    }

// C++ constructor can NOT contain any code, that
// might leave.
//
CCASmileString::CCASmileString()
    {
    }

// ---------------------------------------------------------
// CCASmileString::SmileIconIndex
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASmileString::SmileIconIndex() const
    {
    return iIndex;
    }

// ---------------------------------------------------------
// CCASmileString::SmileString
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CCASmileString::SmileString() const
    {
    return *iSmiley;
    }

// ---------------------------------------------------------
// CCASmileString::Set
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileString::Set( const HBufC* aSmileyString, TInt aIndex )
    {
    // detect (ignore) double assignment
    if ( aSmileyString != iSmiley )
        {
        delete iSmiley;
        iSmiley = aSmileyString;
        }
    iIndex = aIndex;
    }

// ---------------------------------------------------------
// CCASmileString::SetShortestSmileStringL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileString::SetShortestSmileStringL( const TDesC& aSmileyString )
    {
    HBufC* temp = aSmileyString.AllocL();
    delete iShortestSmileyString;
    iShortestSmileyString = temp;
    }

// ---------------------------------------------------------
// CCASmileString::HasShortestString
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASmileString::HasShortestString() const
    {
    return iShortestSmileyString ? ETrue : EFalse;
    }

// ---------------------------------------------------------
// CCASmileString::ShortestSmileString
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CCASmileString::ShortestSmileString() const
    {
    return *iShortestSmileyString;
    }
// End of File
