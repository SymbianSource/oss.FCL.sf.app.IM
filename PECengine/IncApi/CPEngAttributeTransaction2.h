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
* Description:  Presence attribute transactions.
*
*/

#ifndef __CPENGATTRIBUTETRANSACTION2_H
#define __CPENGATTRIBUTETRANSACTION2_H

//  INCLUDES
#include <E32Base.h>
#include <BamDescA.h>


// FORWARD DECLARATIONS
class CPEngAttributeTransaction2Imp;
class CPEngNWSessionSlotID2;
class MPEngAttributeTransactionObserver2;
class MPEngPresenceAttrModel2;



// CLASS DECLARATION

/**
 * Transaction API for presence attributes.
 *
 * With this API clients can:
 *
 *   1. Publish user own presence attributes to network.
 *   2. Fetch presence attributes from network.
 *
 * Attribute publish and attribute fetch are asynchronous
 * network operations, and their end results are signalled
 * to given operation observer.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngAttributeTransaction2 : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngAttributeTransaction2 object.
         *
         * Instantiates CPEngAttributeTransaction2 object and connects
         * it to identified Presence Engine NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for CPEngAttributeTransaction2
         * client side active objects. These active objects are used when
         * asynchronously delivering events to observers.
         *
         * @return New CPEngAttributeTransaction2 instance.
         */
        IMPORT_C static CPEngAttributeTransaction2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngAttributeTransaction2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngAttributeTransaction2();



    private:

        CPEngAttributeTransaction2();




        //-----------------------------------------------------------------------
    public: /* User own attribute publish */


        /**
         * Tests is the publish already active.
         *
         * @since 3.0
         * @return ETrue if operation is active. Else EFalse.
         */
        IMPORT_C TBool IsPublishOwnAttributesActive() const;



        /**
         * Publishes the user own attribute to the network server.
         *
         * Published attribute is kept locked untill the publish
         * operation completes (either succesfully or with failure).
         *
         * There can be just one active publish operation at a time
         * per one CPEngAttributeTransaction2 instance. However,
         * there can be simultaneously several publish operations
         * going on within one NWSessionSlot with distinct
         * CPEngAttributeTransaction2 objects and distinct attribute sets.
         *
         * Takes ownership to passed model(s). If method returns error,
         * the model(s) ownership remains on the client. In success sets
         * the given model pointer to NULL or resets the parameter model
         * array.
         *
         * Passed models must be loaded from same NWSessionSlot as
         * CPEngAttributeTransaction2 is connected and attribute
         * models must be locked for edit/publish.
         *
         * @since 3.0
         * @param aModel The attribute model to publish.
         * @param aObserver The observer to notify from attribute
         * publish events.
         *
         * @return Result from initiating attribute publish:
         *  - KErrNone if attribute publish succesfully initiated.
         *  - KErrInUse if attribute publish already on going from object.
         *  - KErrArgument if attributes either loaded from another
         *    NWSessionSlot or they are not locked for edit.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt PublishAndUnLockOwnAttribute(
            MPEngPresenceAttrModel2*& aModel,
            MPEngAttributeTransactionObserver2& aObserver );


        IMPORT_C TInt PublishAndUnLockOwnAttributes(
            RPointerArray<MPEngPresenceAttrModel2>& aModels,
            MPEngAttributeTransactionObserver2& aObserver );



        /**
         * Cancels the user attribute publish.
         *
         * @since 3.0
         */
        IMPORT_C void CancelPublishOwnAttributes();





        //-----------------------------------------------------------------------
    public: /* Presence attribute fetch from network */

        /**
         * Tests is the fetch already active.
         *
         * @since 3.0
         * @return ETrue if operation is active. Else EFalse.
         */
        IMPORT_C TBool IsFetchAttributesActive() const;


        /**
         * Fetches presence attributes from network server.
         *
         * Fetches presence attributes from the network
         * and stores fetched attributes to presence cache.
         * (Fetched presence attributes can be retrieved e.g. with
         * CPEngAttributeStore2::GetCachedAttributeL().)
         *
         * There can be just one active fetch operation at a time
         * per one CPEngAttributePublisher instance. However, within
         * one NWSessionSlot there can be several active fetch
         * operations going on.
         *
         * @since 3.0
         * @param aPresenceID The user to fetch.
         * @param aType The attribute type. Presence Engine known
         *        attribute types are listed in PEngWVPresenceAttributes2.h.
         * @param aObserver The observer to notify from attribute fetch events.
         * @param aOptions Reserved for future fetch options.
         *
         * @return Result from initiating attribute fetch:
         *  - KErrNone if attribute fetch succesfully initiated.
         *  - KErrInUse if attribute fetch already on going from object.
         *  - KErrNotSupported if requestSed attribute type isn't supported.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt FetchAttributesToCache(
            const TDesC& aPresenceID,
            const TArray<TUint32>& aTypes,
            MPEngAttributeTransactionObserver2& aObserver,
            TInt aOptions = 0 );

        IMPORT_C TInt FetchAttributesToCache(
            const MDesCArray& aPresenceIDs,
            const TArray<TUint32>& aTypes,
            MPEngAttributeTransactionObserver2& aObserver,
            TInt aOptions = 0 );


        /**
         * Fetches presence attributes from network server.
         *
         * Fetches presence attributes from the network
         * and returns them to client as local objects.
         * Client can ask the fetched presence attributes
         * after succesfull fetch from
         * CPEngAttributeTransaction2::GetFetchedAttributes().)
         *
         * There can be just one active fetch operation at a time
         * per one CPEngAttributePublisher instance. However, within
         * one NWSessionSlot there can be several active fetch
         * operations going on.
         *
         * @since 3.0
         * @param aPresenceID The user to fetch.
         * @param aType The attribute type. Presence Engine known
         *        attribute types are listed in PEngWVPresenceAttributes2.h.
         * @param aObserver The observer to notify from attribute fetch events.
         * @return Result from initiating attribute fetch:
         *  - KErrNone if attribute fetch succesfully initiated.
         *  - KErrInUse if attribute fetch already on going from object.
         *  - KErrNotSupported if requestSed attribute type isn't supported.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt FetchAttributesToObjects(
            const TDesC& aPresenceID,
            const TArray<TUint32>& aTypes,
            MPEngAttributeTransactionObserver2& aObserver );


        /**
         * Cancels the presence attribute fetch.
         *
         * @since 3.0
         */
        IMPORT_C void CancelFetchAttributes();



        //-----------------------------------------------------------------------
    public: /* Object get for observer callbacks */


        /**
         * Gets the fetched attribute objects produced by the last
         * FetchAttributesToObjects(). Ownership of attribute models is
         * returned to caller. Returned models can be only read.
         *
         * Fetched attributes can be asked only during the observer callback:
         * MPEngAttributeTransactionObserver2::HandleAttributeTransactionCompleteL().
         * After retrieving the models, client owns the attribute objects and
         * can freely control their lifetime.
         *
         * @since 3.0
         * @param aModel Returns retrieved attribute model(s).
         *        Retrieved attribute models are appended to the end of array.
         *        If array has existing attribute objects, those are not touched.
         *        If get fails with resource error, so far succesfully
         *        moved attributes are returned.
         *
         * @return Status code from attribute getting:
         *        - KErrNone if get succeeds.
         *        - KErrNotReady if no fetch results available
         *        - Else one of system wide error codes.
         */
        IMPORT_C TInt GetFetchedAttributes(
            RPointerArray< MPEngPresenceAttrModel2 >& aModels );




        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngAttributeTransaction2Imp*     iImp;


    };

#endif // __CPENGATTRIBUTETRANSACTION2_H

//End of file







