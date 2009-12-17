/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Lookup key container.
*
*/

// INCLUDE FILES
#include    "e32std.h"
#include    "CIMPSSAPLookupKeyValuePair.h"



// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CIMPSSAPLookupKeyValuePair::NewL()
// -----------------------------------------------------------------------------
//
CIMPSSAPLookupKeyValuePair* CIMPSSAPLookupKeyValuePair::NewL()
    {
    CIMPSSAPLookupKeyValuePair* self = new ( ELeave ) CIMPSSAPLookupKeyValuePair;
    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPLookupKeyValuePair::~CIMPSSAPLookupKeyValuePair()
// Destructor
// -----------------------------------------------------------------------------
//
CIMPSSAPLookupKeyValuePair::~CIMPSSAPLookupKeyValuePair()
    {
    }


// -----------------------------------------------------------------------------
// CIMPSSAPLookupKeyValuePair::CIMPSSAPLookupKeyValuePair()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPLookupKeyValuePair::CIMPSSAPLookupKeyValuePair()
    {
    }


// -----------------------------------------------------------------------------
// CIMPSSAPLookupKeyValuePair::Key() const
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPLookupKeyValuePair::Key() const
    {
    return iLookupKey;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPLookupKeyValuePair::SetLookupKey()
// -----------------------------------------------------------------------------
//
void CIMPSSAPLookupKeyValuePair::SetLookupKey( const TDesC& aLookUpKey )
    {
    iLookupKey.Set( aLookUpKey );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPLookupKeyValuePair::ResetLookupKey()
// -----------------------------------------------------------------------------
//
void CIMPSSAPLookupKeyValuePair::ResetLookupKey()
    {
    iLookupKey.Set( NULL, 0 );
    }




//  End of File

