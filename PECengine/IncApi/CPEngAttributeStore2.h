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
* Description:  Store API to retrieve presence attribute models.
*
*/


#ifndef __CPENGATTRIBUTESTORE2_H__
#define __CPENGATTRIBUTESTORE2_H__

// INCLUDES
#include <E32Base.h>


//FORWARD DECLARATIONS
class CPEngAttributeStore2Imp;
class CPEngNWSessionSlotID2;
class MPEngPresenceAttrModel2;




/**
 * Store API to retrieve presence attribute models.
 *
 * With this API clients can access presence attributes
 * from two different category:
 *
 *   1. Cached presence attributes
 *      Cached presence attributes are subscribed or fetched
 *      presence attributes stored to presence cache.
 *      Clients can only read values from these attributes.
 *      Cached attribute objects are available to clients
 *      only when attribute data is in the storage.
 *      (Attribute is either subscribed or fetched from network.)
 *      Cached data is available only during active network session.
 *
 *
 *   2. User own presence attributes
 *      Own presence attributes present the user own presence status.
 *      Presence Engine has always user own attributes defined.
 *      On terminal hold user own attributes are initially in their
 *      default state. Each concrete attribute type defines
 *      its own default state.
 *
 *      To user own presence attributes clients can perform:
 *      - edit user own presence attributes
 *      - store edited user own attributes back to presence storage
 *      - publish user own presence attributes to network
 *
 *      User own attributes are stored to permanent presence storage.
 *      User own attribuets have empty Presence ID. See
 *      KPEngUserOwnPresenceId from PEngPresenceEngineConsts2.h
 *
 *
 * When loading attribute objects, ownership of created
 * attribute objects is returned to caller.
 *
 * NOTE!! Attribute objects can't be pushed directly with
 * CleanupStack::PushL() to CleanupStack. CleanupStack consideres
 * MPEngPresenceAttrModel2 objects as TAny pointers and frees their
 * memory directly without deleting properly such objects. Thus clients
 * need to use CleanupClosePushL( *presenceAttrModel2 ) to push
 * retrieved objects to CleanupStack.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngAttributeStore2 : public CBase
    {
        //-----------------------------------------------------------------------
    public: /* Construction */


        /**
         * Instantiates CPEngAttributeStore2 object.
         *
         * Instantiates CPEngAttributeStore2 object and connects it to
         * identified Presence Engine side NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for Presence Engine client side active
         * objects. These active objects are used when asynchronously delivering
         * events to observers or maintaining Presence Cache state.
         *
         * @return New CPEngAttributeStore2 instance.
         */
        IMPORT_C static CPEngAttributeStore2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngAttributeStore2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngAttributeStore2();



    private:

        CPEngAttributeStore2();




        //-----------------------------------------------------------------------
    public: /* Cached presence attributes */

        /**
         * Gets a cached presence attribute.
         * Ownership of created model is transferred to caller.
         *
         * @since 3.0
         * @param aPresenceID The user's presence id
         * @param aType The attribute type. Presence Engine known
         *        attribute types are listed in PEngWVPresenceAttributes2.h.
         * @param aModel Returns retrieved attribute model. If retrieve
         *        fails with error, NULL pointer is returned.
         *        Ownership of created model is transferred to caller.
         *        Object can be destroyed either with normal delete operator
         *        or MPEngPresenceAttrModel2::Close() method. If CleanupStack
         *        is used, object must be pushed with CleanupClosePushL().
         *
         * @return Status code from attribute loading:
         *        - KErrNone if loading succeeds.
         *        - KErrNotSupported if attribute type isn't supported.
         *        - KErrNotFound if attribute isn't found from presence cache.
         *        - Else one of system wide error codes.
         */
        IMPORT_C TInt GetCachedAttribute(
            const TDesC& aPresenceID,
            TUint32 aType,
            MPEngPresenceAttrModel2*& aModel ) const;


        /**
         * Gets a cached presence attribute for editing.
         * Ownership of created model is transferred to caller.
         *
         * Loaded model can be read, edited and stored back to cache.
         *
         * Loading tries to gain exclusive write lock for attribute.
         * If some other client already has a exclusive write lock
         * for the attribute, the attribute model loading fails with
         * KErrLocked. Once write locked attribute stays locked until
         * the attribute model instance owning the lock is destroyed.
         *
         * @since 3.0
         * @param aType The attribute type. Presence Engine known
         *        attribute types are listed in PEngWVPresenceAttributes2.h.
         * @param aModel Returns retrieved attribute model. If retrieve
         *        fails with error, NULL pointer is returned.
         *        Ownership of created model is transferred to caller.
         *        Object can be destroyed either with normal delete operator
         *        or MPEngPresenceAttrModel2::Close() method. If CleanupStack
         *        is used, object must be pushed with CleanupClosePushL().
         *
         * @return Status code from attribute loading:
         *        - KErrNone if loading succeeds.
         *        - KErrNotSupported if attribute type isn't supported.
         *        - KErrLocked if attribute model is already locked.
         *        - Else one of system wide error codes.
         */
        IMPORT_C TInt GetAndLockCachedAttribute(
            const TDesC& aPresenceID,
            TUint32 aType,
            MPEngPresenceAttrModel2*& aModel );


        /**
         * Stores editable cached presence attribute to presence
         * storage. Ownership of model stays on the caller.
         * The attribute model edit lock is kept or released
         * depending the aReleaseEditLock parameter.
         *
         * @since 3.0
         * @param aModel The attribute model to store.
         * @param aKeepLocked If ETrue, attribute model
         * edit lock is released if storing succeeds. If EFalse,
         * attribute model is kept edit locked in all cases.
         * @return Status code from attribute storing:
         *         - KErrNone if storing succeeds.
         *         - KErrArgument if attribute is either loaded from another
         *           NWSessionSlot or it isn't locked for edit.
         *         - Else one of system wide error codes.
         */
        IMPORT_C TInt StoreCachedAttribute(
            MPEngPresenceAttrModel2& aModel,
            TBool aReleaseEditLock = ETrue );



        //-----------------------------------------------------------------------
    public: /* User own presence attributes  */

        /**
         * Gets a user own presence attribute for reading.
         * Ownership of created model is transferred to caller.
         *
         * Loaded model can be only read. If attribute value is tried
         * to edit or save to storage, those operations will fail with
         * error.
         *
         * @since 3.0
         * @param aType The attribute type. Presence Engine known
         *        attribute types are listed in PEngWVPresenceAttributes2.h.
         * @param aModel Returns retrieved attribute model. If retrieve
         *        fails with error, NULL pointer is returned.
         *        Ownership of created model is transferred to caller.
         *        Object can be destroyed either with normal delete operator
         *        or MPEngPresenceAttrModel2::Close() method. If CleanupStack
         *        is used, object must be pushed with CleanupClosePushL().
         *
         * @return Status code from attribute loading:
         *        - KErrNone if loading succeeds.
         *        - KErrNotSupported if attribute type isn't supported.
         *        - Else one of system wide error codes.
         */
        IMPORT_C TInt GetOwnAttribute(
            TUint32 aType,
            MPEngPresenceAttrModel2*& aModel ) const;



        /**
         * Gets a user own presence attribute for editing.
         * Ownership of created model is transferred to caller.
         *
         * Loaded model can be read or edited or published to network.
         *
         * Loading tries to gain exclusive write lock for attribute.
         * If some other client already has a exclusive write lock
         * for the attribute, the attribute model loading fails with
         * KErrLocked. Once write locked attribute stays locked until
         *  - the attribute model instance owning the lock is destroyed
         *  - the lock is freed with CPEngAttributeStore2::UnLockOwnAttribute()
         *  - the attribute model is published to network with
         *    CPEngAttributeTransaction2::PublishAndUnLockOwnAttribute()
         *
         * @since 3.0
         * @param aType The attribute type. Presence Engine known
         *        attribute types are listed in PEngWVPresenceAttributes2.h.
         * @param aModel Returns retrieved attribute model. If retrieve
         *        fails with error, NULL pointer is returned.
         *        Ownership of created model is transferred to caller.
         *        Object can be destroyed either with normal delete operator
         *        or MPEngPresenceAttrModel2::Close() method. If CleanupStack
         *        is used, object must be pushed with CleanupClosePushL().
         *
         * @return Status code from attribute loading:
         *        - KErrNone if loading succeeds.
         *        - KErrNotSupported if attribute type isn't supported.
         *        - KErrLocked if attribute model is already locked.
         *        - Else one of system wide error codes.
         */
        IMPORT_C TInt GetAndLockOwnAttribute(
            TUint32 aType,
            MPEngPresenceAttrModel2*& aModel );


        /**
         * Stores editable user own presence attribute to presence
         * storage. Ownership of model stays on the caller.
         * The attribute model edit lock is kept or released
         * depending the aReleaseEditLock parameter.
         *
         * @since 3.0
         * @param aModel The attribute model to store.
         * @param aKeepLocked If ETrue, attribute model
         * edit lock is released if storing succeeds. If EFalse,
         * attribute model is kept edit locked in all cases.
         * @return Status code from attribute storing:
         *         - KErrNone if storing succeeds.
         *         - KErrArgument if attribute is either loaded from another
         *           NWSessionSlot or it isn't locked for edit.
         *         - Else one of system wide error codes.
         */
        IMPORT_C TInt StoreOwnAttribute(
            MPEngPresenceAttrModel2& aModel,
            TBool aReleaseEditLock = ETrue );


        /**
         * Frees the write lock owned by the model.
         * Ownership of model stays on the caller.
         *
         * @since 3.0
         * @param aModel The attribute model which write lock to free.
         * @return Status code from attribute storing:
         *         - KErrNone if unlocking succeeds.
         *         - KErrArgument if attribute is either loaded from another
         *           NWSessionSlot or it isn't locked for edit. (In this case
         *           the lock isn't freed.)
         */
        IMPORT_C TInt UnLockOwnAttribute(
            MPEngPresenceAttrModel2& aModel );




        /**
         * Checks is the given user owned attribute
         * write locked or not.
         *
         * @since 3.0
         * @param aType The attribute type. Presence Engine known attribute
         *        types are listed in PEngWVPresenceAttributes2.h.
         * @return Status code from lock checking:
         *         - KErrNone if not locked.
         *         - KErrNotSupported if attribute type isn't supported.
         *         - KErrLocked if attribute model is locked.
         *         - Else one of system wide error codes.
         */
        IMPORT_C TInt OwnAttributeAvailable(
            TUint32 aType ) const;




        //-----------------------------------------------------------------------
    public: /* Common utilities */

        /**
         * Checks is the given attribute type supported or not.
         *
         * Attribute types that Presence Engine knows are listed
         * in PEngWVPresenceAttributes2.h. Support for some of
         * attribute types is variated with CenRep local variation
         * keys or depends from currently used WV CSP protocol version.
         *
         * This method can be used to check is certain attribute type
         * supported currently or not.
         *
         * @since 3.0
         * @param aType The attribute type. Presence Engine known attribute
         *        types are listed in PEngWVPresenceAttributes2.h.
         * @return Status code from lock checking:
         *         - KErrNone if attribute type is supported.
         *         - KErrNotSupported if attribute type isn't supported.
         */
        IMPORT_C TInt AttributeTypeSupported( TUint32 aType ) const;



        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngAttributeStore2Imp*     iImp;

    };



#endif      //  __CPENGATTRIBUTESTORE2_H__

