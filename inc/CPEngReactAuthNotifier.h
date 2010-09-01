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
* Description:  Notifier API to listen presence reactive authorization changes.
*
*/

#ifndef __CPENGREACTAUTHNOTIFIER_H
#define __CPENGREACTAUTHNOTIFIER_H


//  INCLUDES
#include <E32Base.h>
#include <BamDescA.h>


//FORWARD DECLARATIONS
class MPEngReactAuthObserver;
class CPEngReactAuthNotifierImp;
class CPEngNWSessionSlotID2;



// CLASS DECLARATION

/**
 * Notifier API for presence reactive authorization events.
 *
 * With this API clients can receive notifications from
 * changes in presence reactive authorizations.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngReactAuthNotifier : public CBase
    {
        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngReactAuthNotifier object.
         *
         * Instantiates CPEngReactAuthNotifier object and connects it to
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
        IMPORT_C static CPEngReactAuthNotifier* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngReactAuthNotifier* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngReactAuthNotifier();



    protected:

        CPEngReactAuthNotifier();




        //-----------------------------------------------------------------------
    public: /* Basic reactive authorization notifier */

        /**
         * Check if the notifier is active.
         *
         * @since 3.0
         * @return ETrue if the notifier is active. Else EFalse.
         */
        IMPORT_C TBool IsActive() const;


        /**
         * Starts notifying reactive authorization events.
         *
         * @since 3.0
         * @param aOptions Reserved for future notify options.
         * @return Result from observer start.
         * - KErrNone if notifier start succeeds.
         * - KErrInUse if notifier already started.
         * - Else one of system wide error codes.
         */
        IMPORT_C TInt Start( TInt aOptions = 0 );


        /**
         * Stops observing reactive authorization changes.
         *
         * @since 3.0
         */
        IMPORT_C void Stop();



        //-----------------------------------------------------------------------
    public: /* reactive authorization observers */


        /**
         * Registers observer to be notified from reactive authorization changes.
         *
         * Observers are notified in their registeration order.
         * (First registered is notified first.)
         *
         * @param aObserver The observer to be notified.
         * @return KErrNone is observer added succesfully.
         *         Else one of the system wide error codes.
         */
        IMPORT_C TInt AddObserver(
            MPEngReactAuthObserver& aObserver );



        /**
         * Unregisters reactive authorization observer.
         *
         * @param aObserver The observer to remove.
         * @return KErrNone is observer removed succesfully.
         *         KErrNotFound if the observer wasn't registered.
         */
        IMPORT_C TInt RemoveObserver(
            MPEngReactAuthObserver& aObserver );




        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngReactAuthNotifierImp*     iImp;

    };

#endif  //__CPENGREACTAUTHNOTIFIER_H

// End of File



