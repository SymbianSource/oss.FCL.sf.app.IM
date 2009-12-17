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
* Description:  Tracked presence entry filter.
*
*/

#ifndef TPENGTRACKEDPRESENCEENTRYFILTER_H__
#define TPENGTRACKEDPRESENCEENTRYFILTER_H__

//  INCLUDES
#include "CPEngTrackedPresenceIDEntry.h"

#include <E32Std.h>
#include <BaDesCa.h>


// CLASS DECLARATION
/**
 * Tracked presence entry filter.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( TPEngTrackedPresenceEntryFilter ) : public MDesCArray
    {
public: //Constructor & destructor

    /**
     * C++ constructor.
     * Takes the array to filter / adapt and also
     * flag value determining when to include the entry
     * to filtered list.
     */
    inline TPEngTrackedPresenceEntryFilter(
        RPointerArray< CPEngTrackedPresenceIDEntry >&  aIDsToFilter,
        TInt aIncludeFlag );


public: //From MDesCArray

    inline TInt MdcaCount() const;
    inline TPtrC MdcaPoint( TInt aIndex ) const;


private: //Data

    //REF: Presence IDs to filter
    RPointerArray< CPEngTrackedPresenceIDEntry >&  iIDsToFilter;

    //OWN: Filter value when to include entry to filtered list
    TInt iIncludeFlag;

    };


// Inline methods
#include "TPEngTrackedPresenceEntryFilter.inl"


#endif      //TPENGTRACKEDPRESENCEENTRYFILTER_H__
//  End of File


