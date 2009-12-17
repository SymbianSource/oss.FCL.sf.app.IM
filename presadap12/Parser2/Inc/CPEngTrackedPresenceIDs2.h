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

#ifndef __CPENGTRACKEDPRESENCEIDS2_H
#define __CPENGTRACKEDPRESENCEIDS2_H


//  INCLUDES
#include <E32Base.h>


//FORWARD DECLARATIONS
class CPEngTrackedPresenceIDCollection;
class CPEngTrackedPresenceID2;



/**
 * Tracked Presence IDs.
 *
 * Presents a collection of tracked Presence IDs.
 * One tracked presence ID is presented with CPEngTrackedPresenceID2
 * interface which gives access to individual attributes tracked for
 * the presence ID.
 *
 * In case of notification, holds information of presence IDs that
 * have changed. (Changed attributes for one Presence ID can be
 * iterated from CPEngTrackedPresenceID2.)
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngTrackedPresenceIDs2 : public CBase
    {
        //-----------------------------------------------------------------------
    protected: /* Construction */

        /**
         * C++ constructor.
         */
        CPEngTrackedPresenceIDs2( CPEngTrackedPresenceIDCollection& aImp );


        /**
         * Destructor.
         */
        ~CPEngTrackedPresenceIDs2();




        //-----------------------------------------------------------------------
    public: /* Iterators for Tracked presence IDs*/

        /**
         * Count of tracked presence IDs.
         *
         * @since 3.0
         * @return Count of tracked presence IDs.
         */
        IMPORT_C TInt TrackedPresenceIDsCount() const;


        /**
         * Gets first tracked Presence ID.
         * Tracked Presence IDs are kept in
         * ascending key order.
         *
         * @since 3.0
         * @return First tracked Presence ID.
         * If there isn't any tracked Presence IDs,
         * returns  NULL. Object ownership stays on
         * the collection.
         */
        IMPORT_C CPEngTrackedPresenceID2* FirstTrackedPresenceID();


        /**
         * Gets next tracked Presence ID.
         *
         * @since 3.0
         * @return Next tracked Presence ID.
         * If there isn't any more tracked Presence IDs,
         * returns  NULL. Object ownership stays on
         * the collection.
         */
        IMPORT_C CPEngTrackedPresenceID2* NextTrackedPresenceID();


        /**
         * Resets the tracked Presence ID iterator to its
         * initial state, so the a subsequent call to
         * NextTrackedPresenceID() method go to the first
         * tracked item.
         */
        IMPORT_C void ResetTrackedIterator();



        //-----------------------------------------------------------------------
    public: /* Iterators for changed attributes */


        /**
         * Gets first changed Presence ID.
         *
         * Changed Presence IDs are kept in
         * ascending key order. Changed Presence IDs
         * are available only after presence change
         * notification.
         *
         * @since 3.0
         * @return First changed Presence ID.
         * If there isn't any changed Presence IDs,
         * returns  NULL. Object ownership stays on
         * the collection.
         */
        IMPORT_C CPEngTrackedPresenceID2* FirstChangedPresenceID();


        /**
         * Gets next changed Presence ID.
         *
         * Changed attributes are available only
         * after presence change notification.
         *
         * @since 3.0
         * @return Next changed Presence ID.
         * If there isn't any changed Presence IDs,
         * returns  NULL. Object ownership stays on
         * the collection.
         */
        IMPORT_C CPEngTrackedPresenceID2* NextChangedPresenceID();


        /**
         * Resets the changed Presence ID iterator to its
         * initial state, so the a subsequent call to
         * NextChangedPresenceID() method go to the first
         * changed item.
         *
         * @since 3.0
         */
        IMPORT_C void ResetChangedIterator();



        //-----------------------------------------------------------------------
    private: /* Data */


        /**
         * Implementation is friend.
         * @since 3.0
         */
        friend class CPEngTrackedPresenceIDCollection;		// CSI: 36 #


        //REF: Implementation
        CPEngTrackedPresenceIDCollection&  iImp;

        //OWN: Iterator index
        TInt iNextTrackedIdIx;

        //OWN: Iterator index
        TInt iNextChangedIdIx;

    };

#endif      //__CPENGTRACKEDPRESENCEIDS2_H

//End of file



