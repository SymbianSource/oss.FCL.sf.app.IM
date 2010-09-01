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
* Description:  Presence attribute list transactions.
*
*/

#ifndef __CPENGATTRIBUTELISTTRANSACTION2_H
#define __CPENGATTRIBUTELISTTRANSACTION2_H


//  INCLUDES
#include <E32Base.h>


//FORWARD DECLARATIONS
class CPEngAttributeListTransaction2Imp;
class CPEngNWSessionSlotID2;
class MPEngAttributeListTransactionObserver2;




/**
 * Transaction API for presence attribute lists.
 *
 * With this API clients can publish attribute
 * lists to network.
 *
 * Attribute list publish is asynchronous network operation,
 * and its end results are signalled to given operation observer.
 *
 @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngAttributeListTransaction2 : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngAttributeListTransaction2 object.
         *
         * Instantiates CPEngAttributeListTransaction2 object and connects
         * it to identified Presence Engine NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for CPEngAttributeListTransaction2
         * client side active objects. These active objects are used when
         * asynchronously delivering events to observers.
         *
         * @return New CPEngAttributeListTransaction2 instance.
         */
        IMPORT_C static CPEngAttributeListTransaction2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngAttributeListTransaction2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngAttributeListTransaction2();



    private:
        CPEngAttributeListTransaction2();





        //-----------------------------------------------------------------------
    public: /* Attribute list publish */


        /**
         * Tests is the publish already active.
         *
         * @since 3.0
         * @return ETrue if operation is active. Else EFalse.
         */
        IMPORT_C TBool IsPublishAttributeListsActive() const;



        /**
         * Publishes the attribute lists to the network server.
         *
         * There can be just one active publish operation at a time
         * per one CPEngAttributeListTransaction2 instance.
         *
         * @since 3.0
         * @param aObserver The observer to notify from attribute list
         * publish events.
         *
         * @return Result from initiating attribute list publish:
         *  - KErrNone if attribute list publish succesfully initiated.
         *  - KErrInUse if attribute publish already on going from object.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt PublishAttributeLists(
            MPEngAttributeListTransactionObserver2& aObserver );



        /**
         * Cancels the attribute list publish.
         *
         * @since 3.0
         */
        IMPORT_C void CancelPublishAttributeLists();




        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngAttributeListTransaction2Imp*     iImp;


    };

#endif //__CPENGATTRIBUTELISTTRANSACTION2_H



//End of file


