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

#ifndef CPENGTRACKEDPRESENCEIDENTRYLOOKUP_H
#define CPENGTRACKEDPRESENCEIDENTRYLOOKUP_H

//  INCLUDES
#include "CPEngTrackedPresenceIDEntry.h"
#include <E32Base.h>



//FORWARD DECLARATIONS
class CPEngTrackedPresenceID2;


/**
 * Tracked Presence ID lookup implementation.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngTrackedPresenceIDEntryLookUp ) : public CPEngTrackedPresenceIDEntry
    {
    //-----------------------------------------------------------------------
public: /* Construction */

    /**
     * Instantiates CPEngTrackedPresenceIDEntryLookUp object.
     *
     * @param aPresenceID Tracked Presence ID.
     * @return New CPEngTrackedPresenceIDEntryLookUp instance.
     */
    static CPEngTrackedPresenceIDEntryLookUp* NewL();


    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngTrackedPresenceIDEntryLookUp();



private:  // Constructor

    /**
     * C++ default constructor.
     */
    CPEngTrackedPresenceIDEntryLookUp();



    //-----------------------------------------------------------------------
public: /* Lookup services */


    /**
     * Overridden Presence ID accessor to return lookup data.
     *
     * @since 3.0
     * @return Presence ID lookup key as set
     * previously with SetLookupKey().
     */
    virtual TPtrC PresenceID() const;


    /**
     * Sets new look up key.
     * Note! Sets lookup key to point given data.
     *
     * @since 3.0
     * @param aPresenceID New Presence ID lookup key
     */
    void SetLookupKey( const TDesC& aPresenceID );


    /**
     * Resets the lookup key to empty descriptor.
     *
     * @since 3.0
     */
    void Reset();


private: //Data

    //OWN: Presence Id lookup key
    TPtrC iLookupKey;
    };

#endif      // CPENGTRACKEDPRESENCEIDENTRYLOOKUP_H



