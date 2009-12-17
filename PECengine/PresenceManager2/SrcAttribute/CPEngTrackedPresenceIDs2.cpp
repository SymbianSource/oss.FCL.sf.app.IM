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
* Description:  Tracked Presence IDs.
*
*/

// INCLUDE FILES
#include <CPEngTrackedPresenceIDs2.h>
#include <E32Std.h>

#include "CPEngTrackedPresenceIDCollection.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDs2::CPEngTrackedPresenceIDs2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDs2::CPEngTrackedPresenceIDs2( CPEngTrackedPresenceIDCollection& aImp )
        : iImp( aImp ),
        iNextTrackedIdIx( 0 ),
        iNextChangedIdIx( 0 )
    {
    }


// Destructor
CPEngTrackedPresenceIDs2::~CPEngTrackedPresenceIDs2()
    {
    //Implementation not owned
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDs2::TrackedPresenceIDsCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngTrackedPresenceIDs2::TrackedPresenceIDsCount() const
    {
    return iImp.TrackedPresenceIDsCount();
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDs2::FirstTrackedPresenceID()
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngTrackedPresenceID2* CPEngTrackedPresenceIDs2::FirstTrackedPresenceID()
    {
    //Set index to start
    iNextTrackedIdIx = 0;

    return iImp.NextTrackedPresenceID( iNextTrackedIdIx );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDs2::NextTrackedPresenceID()
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngTrackedPresenceID2* CPEngTrackedPresenceIDs2::NextTrackedPresenceID()
    {
    return iImp.NextTrackedPresenceID( iNextTrackedIdIx );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDs2::ResetTrackedIterator()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngTrackedPresenceIDs2::ResetTrackedIterator()
    {
    //Set index to start
    iNextTrackedIdIx = 0;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDs2::FirstChangedPresenceID()
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngTrackedPresenceID2* CPEngTrackedPresenceIDs2::FirstChangedPresenceID()
    {
    //Set index to start
    iNextChangedIdIx = 0;
    return iImp.NextChangeMarkedPresenceID( iNextChangedIdIx );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDs2::NextChangedPresenceID()
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngTrackedPresenceID2* CPEngTrackedPresenceIDs2::NextChangedPresenceID()
    {
    return iImp.NextChangeMarkedPresenceID( iNextChangedIdIx );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDs2::ResetChangedIterator()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngTrackedPresenceIDs2::ResetChangedIterator()
    {
    //Set index to start
    iNextChangedIdIx = 0;
    }


//  End of File




