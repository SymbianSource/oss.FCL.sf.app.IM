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
* Description:  Tracked Presence ID lookup implementation.
*
*/

// INCLUDE FILES
#include "CPEngTrackedPresenceIDEntryLookUp.h"

#include <CPEngTrackedPresenceID2.h>
#include <E32Std.h>





// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntryLookUp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDEntryLookUp* CPEngTrackedPresenceIDEntryLookUp::NewL()
    {
    CPEngTrackedPresenceIDEntryLookUp* self = new ( ELeave )
    CPEngTrackedPresenceIDEntryLookUp;
    return self;
    }


// Destructor
CPEngTrackedPresenceIDEntryLookUp::~CPEngTrackedPresenceIDEntryLookUp()
    {
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntryLookUp::CPEngTrackedPresenceIDEntryLookUp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDEntryLookUp::CPEngTrackedPresenceIDEntryLookUp()
        : iLookupKey( NULL, 0 )
    {
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntryLookUp::PresenceID()
// Overridden Presence ID accessor to return lookup data
// -----------------------------------------------------------------------------
//
TPtrC CPEngTrackedPresenceIDEntryLookUp::PresenceID() const
    {
    return iLookupKey;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntryLookUp::SetLookupKey()
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDEntryLookUp::SetLookupKey( const TDesC& aPresenceID )
    {
    iLookupKey.Set( aPresenceID );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntryLookUp::Reset()
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDEntryLookUp::Reset()
    {
    iLookupKey.Set( NULL, 0 );
    }



//  End of File








