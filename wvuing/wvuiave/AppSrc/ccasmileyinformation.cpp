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
* Description:  Storage to save smiley information
*
*/


// INCLUDES
#include "ccasmileyinformation.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCASmileyInformation::CCASmileyInformation()
// ---------------------------------------------------------------------------
//
CCASmileyInformation::CCASmileyInformation()
    {
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::NewL()
// ---------------------------------------------------------------------------
//
CCASmileyInformation* CCASmileyInformation::NewL()
    {
    CCASmileyInformation* self = new ( ELeave ) CCASmileyInformation;
    return self;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::~CCASmileyInformation()
// ---------------------------------------------------------------------------
//
CCASmileyInformation::~CCASmileyInformation()
    {
    delete iSmileyString;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::Position()
// ---------------------------------------------------------------------------
//
TInt CCASmileyInformation::Position() const
    {
    return iPosition;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::SetPosition()
// ---------------------------------------------------------------------------
//
void CCASmileyInformation::SetPosition( TInt aPosition )
    {
    iPosition = aPosition;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::Index()
// ---------------------------------------------------------------------------
//
TInt CCASmileyInformation::Index() const
    {
    return iIndex;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::SetIndex()
// ---------------------------------------------------------------------------
//
void CCASmileyInformation::SetIndex( TInt aIndex )
    {
    iIndex = aIndex;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::SmileyString()
// ---------------------------------------------------------------------------
//
const TDesC& CCASmileyInformation::SmileyString() const
    {
    return *iSmileyString;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::SetSmileyStringL()
// ---------------------------------------------------------------------------
//
void CCASmileyInformation::SetSmileyStringL( const TDesC& aSmiley )
    {
    HBufC* smile = aSmiley.AllocL();
    delete iSmileyString;
    iSmileyString = smile;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::IsIcon()
// ---------------------------------------------------------------------------
//
TBool CCASmileyInformation::IsIcon() const
    {
    return iIsIcon;
    }

// ---------------------------------------------------------------------------
// CCASmileyInformation::SetIcon()
// ---------------------------------------------------------------------------
//
void CCASmileyInformation::SetIcon( TBool aIsIcon )
    {
    iIsIcon = aIsIcon;
    }
