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
* Description:  Notifier API to listen presence changes.
*
*/

#ifndef __CPENGPRESENCENOTIFIER2_H
#define __CPENGPRESENCENOTIFIER2_H

//  INCLUDES
#include <E32Base.h>
#include <BamDesca.h>
#include <MPEngContactList2.h>




//FORWARD DECLARATIONS
class MPEngPresenceObserver2;
class CPEngTrackedPresenceIDs2;
class CPEngPresenceNotifier2Imp;
class CPEngNWSessionSlotID2;




/**
 * Notifier API for presence changes.
 *
 * With this API clients can
 * 1. Receive notifications from changes in cached
 *    presence attributes. Cached presence attributes
 *    are subscribed or fetched presence attributes
 *    stored to presence cache.
 *
 *    Notifications are triggered from:
 *    - Attribute creation to presence cache when
 *      the user is subscribed
 *
 *    - Attribute value changes in presence cache
 *      after receiving attribute notification from network
 *      or attributes were manually fetched from network
 *
 *    - Attribute removal from storage when user
 *      attributes are unsubscribed or fetched
 *      attributes are cleared.
 *
 *    Thus, client can request presence notifications from
 *    such Presence IDs and presence attributes that do not
 *    exist yet in presence cache. Clients must be prepared
 *    also to handle the cases that attribute doesn't anymore
 *    exist in storage, if the notification was delivered due
 *    attribute clearing from storage.
 *
 * 2. Receive notifications from user own attribute changes.
 *    Notification from user own attributes is requested with
 *    zero length PresenceID and they are also delivered with
 *    zero length PresenceID. (See: KPEngUserOwnPresenceId from
 *    PEngPresenceEngineConsts2.h)
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngPresenceNotifier2 : public CBase
    {
        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngPresenceNotifier2 object.
         *
         * Instantiates CPEngPresenceNotifier2 object and connects it to
         * identified Presence Engine side NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for CPEngPresenceNotifier2
         * client side active objects. These active objects are used when
         * asynchronously delivering events to observers.
         *
         * @return New CPEngPresenceNotifier2 instance.
         */
        IMPORT_C static CPEngPresenceNotifier2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngPresenceNotifier2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngPresenceNotifier2();



    protected:

        /**
         * C++ constructor.
         */
        CPEngPresenceNotifier2();



        //-----------------------------------------------------------------------
    public: /* Basic Presence notifier */

        /**
         * Check if the notifier is active.
         *
         * @since 3.0
         * @return ETrue if the notifier is active. Else EFalse.
         */
        IMPORT_C TBool IsActive() const;


        /**
         * Starts observing presence changes.
         * Ignores duplicate presence attributes.
         *
         * @since 3.0
         * @param aPresenceID Presence ID which changes to track.
         * @param aTypes The attribute types to track. Attribute types
         *        known by Presence Engine are listed in
         *        PEngWVPresenceAttributes2.h.
         *
         * @return Result from observer start.
         * - KErrNone if notifier start succeeds.
         * - KErrNotSupported if attribute type isn't supported.
         * - KErrInUse if notifier already started.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Start( const TDesC& aPresenceID,
                             const TArray<TUint32>& aTypes );


        /**
         * Starts observing presence changes.
         * Ignores duplicate presence attributes and presence IDs.
         * Presence ID list can be empty.
         *
         * @since 3.0
         * @param aPresenceIDs Presence IDs which changes to track.
         * @param aTypes The attribute types to track. Attribute types
         *        known by Presence Engine are listed in
         *        PEngWVPresenceAttributes2.h.
         *
         * @return Result from observer start.
         * - KErrNone if notifier start succeeds.
         * - KErrNotSupported if attribute type isn't supported.
         * - KErrInUse if notifier already started.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Start( const MDesCArray& aPresenceIDs,
                             const TArray<TUint32>& aTypes );


        /**
         * Starts observing presence changes.
         * Ignores duplicate presence attributes and presence IDs.
         * Presence ID list can be empty.
         *
         * @since 3.0
         * @param aList The contact list from which to take
         * tracked Presence IDs.
         * @param aContactListView From which contact list
         * view to take observed contacts.
         * @param aTypes The attribute types to track. Attribute types
         *        known by Presence Engine are listed in
         *        PEngWVPresenceAttributes2.h.
         *
         * @return Result from observer start.
         * - KErrNone if notifier start succeeds.
         * - KErrNotSupported if attribute type isn't supported.
         * - KErrInUse if notifier already started.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Start( const MPEngContactList2& aList,
                             TPEngContactListView aContactListView,
                             const TArray<TUint32>& aTypes );


        /**
         * Stops observing presence changes.
         *
         * @since 3.0
         */
        IMPORT_C void Stop();



        //-----------------------------------------------------------------------
    public: /* Track list update */

        /**
         * Updates the list of observed presence IDs. Replaces
         * list of tracked Presence ID's with given list.
         * (Same effect than using separated Stop() and Start() calls
         * but is much more efficient.) Uses presence attributes given
         * in CPEngPresenceNotifier2::Start(). Ignores duplicate
         * presence IDs.
         *
         * @since 3.0
         * @param aPresenceIDs List of Presence IDs which changes to track.
         *
         * @return Result from observer start.
         * - KErrNone if notifier update succeeds.
         * - KErrNotReady if notifier not started.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Update( const MDesCArray& aPresenceIDs );



        /**
         * Updates the list of observed presence IDs. Replaces
         * list of tracked Presence ID's with given list.
         * (Same effect than using separated Stop() and Start() calls
         * but is much more efficient.) Uses presence attributes given
         * in CPEngPresenceNotifier2::Start(). Ignores duplicate
         * presence IDs.
         *
         * @since 3.0
         * @param aList The contact list from which to take
         *        tracked Presence IDs.
         * @param aContactListView From which contact list
         *        view to take observed contacts.
         *
         * @return Result from observer start.
         * - KErrNone if notifier update succeeds.
         * - KErrNotReady if notifier not started.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Update( const MPEngContactList2& aList,
                              TPEngContactListView aContactListView );




        //-----------------------------------------------------------------------
    public: /* Presence ID and attribute handling */




        /**
         * Adds Presence ID to the list of tracked presences.
         *
         * @since 3.0
         * @param aPresenceID Presence ID which changes to track.
         *        If Presence ID is already tracked, new requested
         *        attributes are added in the list of tracked attributes.
         * @param aTypes The attribute types to track. Attribute types
         *        known by Presence Engine are listed in
         *        PEngWVPresenceAttributes2.h. Ignores duplicate
         *        presence attributes.
         *
         * @return Result from adding Presence ID.
         * - KErrNone if Presence ID add succeeds.
         * - KErrNotSupported if attribute type isn't supported.
         * - KErrNotReady if notifier not started.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Add( const TDesC& aPresenceID,
                           const TArray<TUint32>& aTypes );


        /**
         * Removes Presence ID from the list of tracked presences.
         *
         * @since 3.0
         * @param aPresenceID Presence ID which tracking to stop.
         *
         * @return Result from removing Presence ID.
         * - KErrNone if Presence ID remove succeeds.
         * - KErrNotReady if notifier not started.
         * - KErrNotFound if Presence ID not tracked.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Remove( const TDesC& aPresenceID );




        /**
         * Removes Presence attribute from the list of tracked
         * attributes. Removes also such Presence IDs from
         * the tracked ones which do not have anymore tracked
         * attributes.
         *
         * @since 3.0
         * @param aType Attribute type which tracking to stop. Attribute
         *        types known by Presence Engine are listed in
         *        PEngWVPresenceAttributes2.h.
         *
         * @return Result from removing attribute.
         * - KErrNone if attribute remove succeeds.
         * - KErrNotSupported if attribute type isn't supported.
         * - KErrNotReady if notifier not started.
         * - KErrNotFound if attribute not tracked.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Remove( TUint32 aType );




        //-----------------------------------------------------------------------
    public: /* Tracked presence IDs */


        /**
         * Tracked Presence ID's.
         *
         * Gets the list of tracked Presence ID's.
         * Tracked Presence IDs can be iterated with:
         *  - CPEngTrackedPresenceIDs2::FirstTrackedPresenceID()
         *  - CPEngTrackedPresenceIDs2::NextTrackedPresenceID()
         *
         * List holds also information from last notified
         * changes. (Thus clients can access the notify information
         * also after notify callback has gone, without making a
         * copy from data. However, each new notification
         * overwrites the data from previous notification.)
         *
         * Last changed Presence ID's can be iterated with:
         *  - CPEngTrackedPresenceIDs::FirstChangedPresenceID()
         *  - CPEngTrackedPresenceIDs::NextChangedPresenceID()
         *
         * @since 3.0
         * @return Collection of tracked Presence ID's.
         */
        IMPORT_C const CPEngTrackedPresenceIDs2& TrackedPresenceIDs() const;
        IMPORT_C CPEngTrackedPresenceIDs2& TrackedPresenceIDs();




        //-----------------------------------------------------------------------
    public: /* Presence observers */


        /**
         * Registers observer to be notified from presence changes.
         *
         * Observers are notified in their registeration order.
         * (First registerd notified first.)
         *
         * @param aObserver The observer to be notified.
         * Panics if NULL pointer passed.
         * @return KErrNone is observer added succesfully.
         *         Else one of the system wide error codes.
         */
        IMPORT_C TInt AddObserver(
            MPEngPresenceObserver2& aObserver );



        /**
         * Unregisters presence observer.
         *
         * @param aObserver The observer to remove.
         * @return KErrNone is observer removed succesfully.
         *         KErrNotFound if the observer wasn't registered.
         */
        IMPORT_C TInt RemoveObserver(
            MPEngPresenceObserver2& aObserver );



        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngPresenceNotifier2Imp*     iImp;


    };

#endif  //__CPENGPRESENCENOTIFIER2_H

// End of File
