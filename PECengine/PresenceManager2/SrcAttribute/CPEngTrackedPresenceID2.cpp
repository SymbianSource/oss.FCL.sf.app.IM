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
* Description:  Tracked Presence ID.
*
*/

// INCLUDE FILES
#include <CPEngTrackedPresenceID2.h>
#include <E32Std.h>
#include "CPEngTrackedPresenceIDEntry.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngTrackedPresenceID2::CPEngTrackedPresenceID2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceID2::CPEngTrackedPresenceID2( CPEngTrackedPresenceIDEntry& aImp )
        : iImp( aImp ),
        iNextTrackedAttributeIx( 0 ),
        iNextChangedAttributeIx( 0 )
    {
    }


// Destructor
CPEngTrackedPresenceID2::~CPEngTrackedPresenceID2()
    {
    //Implementation not owned
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceID2::PresenceID()
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CPEngTrackedPresenceID2::PresenceID() const
    {
    return iImp.PresenceID();
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceID2::FirstTrackedAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CPEngTrackedPresenceID2::FirstTrackedAttribute()
    {
    //Set index to start
    iNextTrackedAttributeIx = 0;

    return iImp.NextTrackedAttribute( iNextTrackedAttributeIx );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceID2::NextTrackedAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CPEngTrackedPresenceID2::NextTrackedAttribute()
    {
    return iImp.NextTrackedAttribute( iNextTrackedAttributeIx );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceID2::ResetTrackedIterator()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngTrackedPresenceID2::ResetTrackedIterator()
    {
    //Set index to start
    iNextTrackedAttributeIx = 0;
    }



// -----------------------------------------------------------------------------
// CPEngTrackedPresenceID2::FirstChangedAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CPEngTrackedPresenceID2::FirstChangedAttribute()
    {
    //Set index to start
    iNextChangedAttributeIx = 0;

    return iImp.NextChangedAttribute( iNextChangedAttributeIx );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceID2::NextChangedAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CPEngTrackedPresenceID2::NextChangedAttribute()
    {
    return iImp.NextChangedAttribute( iNextChangedAttributeIx );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceID2::ResetChangedIterator()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngTrackedPresenceID2::ResetChangedIterator()
    {
    //Set index to start
    iNextChangedAttributeIx = 0;
    }


//  End of File


