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
* Description:  Presence contact list transactions.
*
*/

#ifndef __CPENGCONTACTLISTTRANSACTION2_H
#define __CPENGCONTACTLISTTRANSACTION2_H

//  INCLUDES
#include <E32Base.h>
#include <BamDescA.h>


// FORWARD DECLARATIONS
class MPEngContactList2;
class CPEngContactListTransaction2Imp;
class CPEngNWSessionSlotID2;
class MPEngContactListTransactionObserver2;



// CLASS DECLARATION

/**
 * Transaction API for presence contact lists.
 *
 * With this API clients can publish contact
 * lists to network.
 *
 * Contact list publish operations are asynchronous network operations,
 * and their end results are signalled to given operation observer.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngContactListTransaction2 : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngContactListTransaction2 object.
         *
         * Instantiates CPEngContactListTransaction2 object and connects
         * it to identified Presence Engine NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for CPEngContactListTransaction2
         * client side active objects. These active objects are used when
         * asynchronously delivering events to observers.
         *
         * @return New CPEngContactListTransaction2 instance.
         */
        IMPORT_C static CPEngContactListTransaction2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngContactListTransaction2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngContactListTransaction2();



    private:

        CPEngContactListTransaction2();



        //-----------------------------------------------------------------------
    public: /* Contact lists base synchronization */


        /**
         * Tests is the contact list base synchronize already active.
         *
         * @since 3.0
         * @return ETrue if operation is active. Else EFalse.
         */
        IMPORT_C TBool IsBaseSynchronizeContactListsActive() const;



        /**
         * Base synchronizes list of contact lists with network server.
         *
         * There can be just one active base synchronization
         * operation at a time per one CPEngContactListTransaction2 instance.
         *
         * @since 3.0
         * @param aObserver The observer to notify from contact list
         * 					base synchronization events.
         * 		  aNeedToSubscribe ETrue if the lists need to be unsubscribed
         * 					during base synchronization.
         * @return Result from initiating contact base synchronization:
         *  - KErrNone if contact list base synchronization succesfully initiated.
         *  - KErrInUse if base synchronization already on going.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt BaseSynchronizeContactLists(
            MPEngContactListTransactionObserver2& aObserver );

        IMPORT_C TInt BaseSynchronizeContactLists(
            MPEngContactListTransactionObserver2& aObserver,
            TBool aNeedToUnsubscribe );

        /**
         * Cancels the contact list base synchronize.
         *
         * @since 3.0
         */
        IMPORT_C void CancelBaseSynchronizeContactLists();





        //-----------------------------------------------------------------------
    public: /* Contact list publish & synchronization */


        /**
         * Tests is the contact list publish or
         * synchronize already active.
         *
         * @since 3.0
         * @return ETrue if operation is active. Else EFalse.
         */
        IMPORT_C TBool IsContactListPubSyncActive() const;



        /**
         * Synchronizes the contact list with the network server.
         *
         * There can be just one active contact list publish or
         * synchronization  operation at a time per one
         * CPEngContactListTransaction2 instance.
         *
         * @since 3.0
         * @param aContactListName The contact list(s) to synchronize.
         * @param aObserver The observer to notify from contact list
         * synchronize events.
         * @return Result from initiating contact list synchronize:
         *  - KErrNone if contact list synchronize succesfully initiated.
         *  - KErrNotfound if requested contact list not found.
         *  - KErrInUse if contact synchronize already on going from object.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt SynchronizeContactList(
            const TDesC& aContactListName,
            MPEngContactListTransactionObserver2& aObserver );

        IMPORT_C TInt SynchronizeContactLists(
            const MDesCArray& aContactListNames,
            MPEngContactListTransactionObserver2& aObserver );


        /**
         * Publishes the contact list to the network server.
         *
         * There can be just one active contact list publish or
         * synchronization  operation at a time per one
         * CPEngContactListTransaction2 instance.
         *
         * @since 3.0
         * @param aContactListName The contact list(s) to publish.
         * @param aObserver The observer to notify from contact list
         * publish events.
         * @return Result from initiating contact list publish:
         *  - KErrNone if contact list publish succesfully initiated.
         *  - KErrNotfound if requested contact list not found.
         *  - KErrInUse if contact publish already on going from object.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt PublishContactList(
            const TDesC& aContactListName,
            MPEngContactListTransactionObserver2& aObserver );

        IMPORT_C TInt PublishContactLists(
            const MDesCArray& aContactListNames,
            MPEngContactListTransactionObserver2& aObserver );



        /**
         * Cancels the current contact list publish or synchronize
         * operation.
         *
         * @since 3.0
         */
        IMPORT_C void CancelContactListPubSync();




        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngContactListTransaction2Imp*     iImp;

    };



#endif  //__CPENGCONTACTLISTTRANSACTION2_H

//End of file


