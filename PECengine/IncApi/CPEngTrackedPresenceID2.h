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


#ifndef __CPENGTRACKEDPRESENCEID2_H
#define __CPENGTRACKEDPRESENCEID2_H

//  INCLUDES
#include <E32Base.h>

//FORWARD DECLARATIONS
class CPEngTrackedPresenceIDEntry;




/**
 * Tracked Presence ID.
 *
 * Presents one tracked Presence ID and list of
 * presence attributes tracked for the Presence ID.
 *
 * In case of notification, holds also information
 * of presence attributes that have changed for the Presence ID.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngTrackedPresenceID2 : public CBase
    {

        //-----------------------------------------------------------------------
    protected: /* Construction */

        /**
         * C++ constructor.
         */
        CPEngTrackedPresenceID2( CPEngTrackedPresenceIDEntry& aImp );


        /**
         * Destructor.
         */
        ~CPEngTrackedPresenceID2();




        //-----------------------------------------------------------------------
    public: /* Tracked presence data */


        /**
         * Tracked presence ID.
         *
         * @since 3.0
         * @return Tracked presence ID.
         */
        IMPORT_C TPtrC PresenceID() const;




        //-----------------------------------------------------------------------
    public: /* Iterators for tracked attributes */

        /**
         * Gets first tracked presence attribute
         * for the Presence ID. Tracked attributes are
         * kept in unsigned key order.
         *
         * @since 3.0
         * @return First tracked presence attribute.
         * If there isn't any tracked attributes, returns
         * KPEngNullAttributeType.
         */
        IMPORT_C TUint32 FirstTrackedAttribute();



        /**
         * Gets next tracked presence attribute
         * for the Presence ID.
         *
         * @since 3.0
         * @return Next tracked presence attribute.
         * If there isn't any more tracked attributes,
         * returns KPEngNullAttributeType.
         */
        IMPORT_C TUint32 NextTrackedAttribute();



        /**
         * Resets the tracked attribute iterator to its
         * initial state, so the a subsequent call to
         * NextTrackedAttribute() method go to the first
         * tracked item.
         */
        IMPORT_C void ResetTrackedIterator();




        //-----------------------------------------------------------------------
    public: /* Iterators for changed attributes */


        /**
         * Gets first changed presence attribute for the
         * Presence ID. Changed attributes are kept
         * in unsigned key order.
         *
         * Changed attributes are available only
         * after presence change notification.
         *
         * @since 3.0
         * @return First changed presence attribute.
         * If there isn't any changed attributes, returns
         * KPEngNullAttributeType.
         */
        IMPORT_C TUint32 FirstChangedAttribute();



        /**
         * Gets next changed presence attribute
         * for the Presence ID.
         *
         * Changed attributes are available only
         * after presence change notification.
         *
         * @since 3.0
         * @return Next changed presence attribute.
         * If there isn't any more changed attributes,
         * returns KPEngNullAttributeType.
         */
        IMPORT_C TUint32 NextChangedAttribute();



        /**
         * Resets the changed attribute iterator to its
         * initial state, so the a subsequent call to
         * NextChangedAttribute() method go to the first
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
        friend class CPEngTrackedPresenceIDEntry;		// CSI: 36 #


        //REF: Implementation
        CPEngTrackedPresenceIDEntry&    iImp;

        //OWN: Iterator index
        TInt iNextTrackedAttributeIx;

        //OWN: Iterator index
        TInt iNextChangedAttributeIx;

    };

#endif      //__CPENGTRACKEDPRESENCEID2_H


//End of file





