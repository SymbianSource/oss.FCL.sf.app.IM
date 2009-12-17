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
* Description:  Presence reactive authorization transactions.
*
*/

#ifndef __CPENGREACTAUTHTRANSACTION_H
#define __CPENGREACTAUTHTRANSACTION_H

//  INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS
class MPEngReactAuth2;
class CPEngReactAuthTransactionImp;
class CPEngNWSessionSlotID2;
class MPEngReactAuthTransactionObserver;
class MPEngAuthorizationRespond;


// CLASS DECLARATION

/**
 * Transaction API for presence reactive authorizations.
 *
 * With this API clients can publish reactive authorization
 * responds to network.
 *
 * Reactive authorization transactions are asynchronous network
 * operations, and their end results are signalled to given operation
 * observer.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngReactAuthTransaction : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngReactAuthTransaction object.
         *
         * Instantiates CPEngReactAuthTransaction object and connects
         * it to identified Presence Engine NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for CPEngReactAuthTransaction
         * client side active objects. These active objects are used when
         * asynchronously delivering events to observers.
         *
         * @return New CPEngReactAuthTransaction instance.
         */
        IMPORT_C static CPEngReactAuthTransaction* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngReactAuthTransaction* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngReactAuthTransaction();



    private:

        CPEngReactAuthTransaction();



        //-----------------------------------------------------------------------
    public: /* Reactive authorization publish & synchronization */


        /**
         * Tests is the reactive authorization publish already active.
         *
         * @since 3.0
         * @return ETrue if operation is active. Else EFalse.
         */
        IMPORT_C TBool IsPublishReactAuthRespondsActive() const;


        /**
         * Publishes the reactive authorization responds to the network server.
         *
         * There can be just one active reactive authorization publish
         * operation at a time per one CPEngReactAuthTransaction instance.
         *
         * Published authorization responds is kept locked untill the publish
         * operation completes (either succesfully or with failure).
         *
         * Takes ownership to passed authorization responds. If method
         * returns error, authorization responds ownership remains on
         * the client. In success, sets the given authorization responds
         * pointer to NULL.
         *
         * Passed authorization responds must be loaded from same
         * NWSessionSlot as  CPEngReactAuthTransaction is connected.
         *
         * @since 3.0
         * @param aResponds The reactive authorization responds to publish.
         * @param aObserver The observer to notify from reactive
         * authorization publish events.
         * @return Result from initiating reactive authorization publish:
         *  - KErrNone if reactive authorization publish succesfully initiated.
         *  - KErrInUse if reactive authorization already on going from object.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt PublishReactAuthResponds(
            MPEngAuthorizationRespond*& aResponds,
            MPEngReactAuthTransactionObserver& aObserver );


        /**
         * Cancels the current reactive authorization publish operation.
         *
         * @since 3.0
         */
        IMPORT_C void CancelPublishReactAuthResponds();



        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngReactAuthTransactionImp*     iImp;

    };



#endif  //__CPENGREACTAUTHTRANSACTION_H

//End of file


