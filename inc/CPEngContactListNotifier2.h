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
* Description:  Notifier API to listen presence contact list changes.
*
*/

#ifndef __CPENGCONTACTLISTNOTIFIER2_H
#define __CPENGCONTACTLISTNOTIFIER2_H


//  INCLUDES
#include <E32Base.h>
#include <BamDescA.h>


//FORWARD DECLARATIONS
class MPEngContactListObserver2;
class MPEngContactList2;
class CPEngContactListNotifier2Imp;
class CPEngNWSessionSlotID2;



// CLASS DECLARATION

/**
 * Notifier API for presence contact list changes.
 *
 * With this API clients can
 * 1. Receive notifications from changes in presence
 *    contact lists contents (users added or removed).
 *
 * 2. Receive notifications from changes in presence
 *    contact lists settings.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngContactListNotifier2 : public CBase
    {
        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngContactListNotifier2 object.
         *
         * Instantiates CPEngContactListNotifier2 object and connects it to
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
        IMPORT_C static CPEngContactListNotifier2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngContactListNotifier2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngContactListNotifier2();



    protected:

        CPEngContactListNotifier2();




        //-----------------------------------------------------------------------
    public: /* Basic contact list notifier */

        /**
         * Check if the notifier is active.
         *
         * @since 3.0
         * @return ETrue if the notifier is active. Else EFalse.
         */
        IMPORT_C TBool IsActive() const;



        /**
         * Starts observing contact list changes.
         *
         * @since 3.0
         * @param aContactListName Contact list(s) which changes to track.
         * @return Result from observer start.
         * - KErrNone if notifier start succeeds.
         * - KErrInUse if notifier already started.
         * - KErrAlreadyExist if duplicate contact lists given.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Start( const TDesC& aContactListName );
        IMPORT_C TInt Start( const MDesCArray& aContactListNames );


        /**
         * Stops observing contact list changes.
         *
         * @since 3.0
         */
        IMPORT_C void Stop();





        //-----------------------------------------------------------------------
    public: /* Advanced notifier features */


        /**
         * Adds contact list to the list of tracked contact lists.
         *
         * @since 3.0
         * @param aContactListName Contact list which changes to track.
         *
         * @return Result from adding Contact list.
         * - KErrNone if contact list add succeeds.
         * - KErrNotReady if notifier not started.
         * - KErrAlreadyExist if duplicate contact list.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Add( const TDesC& aContactListName );


        /**
         * Removes contact list from the list of tracked contact lists.
         *
         * If the last contact list is removed, stops the notifier.
         *
         * @since 3.0
         * @param aContactListName Contact list which tracking to stop.
         *
         * @return Result from removing Presence ID.
         * - KErrNone if contact list remove succeeds.
         * - KErrNotReady if notifier not started.
         * - KErrNotFound if contact list not tracked.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Remove( const TDesC& aContactListName );





        //-----------------------------------------------------------------------
    public: /* Contact list observers */


        /**
         * Registers observer to be notified from contact list changes.
         *
         * Observers are notified in their registeration order.
         * (First registerd notified first.)
         *
         * @param aObserver The observer to be notified.
         * @return KErrNone is observer added succesfully.
         *         Else one of the system wide error codes.
         */
        IMPORT_C TInt AddObserver(
            MPEngContactListObserver2& aObserver );



        /**
         * Unregisters contact list observer.
         *
         * @param aObserver The observer to remove.
         * @return KErrNone is observer removed succesfully.
         *         KErrNotFound if the observer wasn't registered.
         */
        IMPORT_C TInt RemoveObserver(
            MPEngContactListObserver2& aObserver );




        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngContactListNotifier2Imp*     iImp;

    };



#endif  //__CPENGCONTACTLISTNOTIFIER2_H

// End of File



