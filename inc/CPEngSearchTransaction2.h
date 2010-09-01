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
* Description:  Presence search transactions.
*
*/

#ifndef __CPENGSEARCHTRANSACTION2_H
#define __CPENGSEARCHTRANSACTION2_H


//  INCLUDES
#include <E32Base.h>



// FORWARD DECLARATIONS
class CPEngNWSessionSlotID2;
class CPEngSearchCriteria2;
class CPEngSearchTransaction2Imp;
class MPEngSearchTransactionObserver2;



/**
 * Transaction API for performing network search.
 *
 * With this API clients search user ids from network
 * server. Supports using either single or multiple search
 * criterias. Also supports continuing the search
 * after getting partial results.
 *
 * Search operations are asynchronous network operations,
 * and their end results are signalled to given operation
 * observer.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngSearchTransaction2 : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngSearchTransaction2 object.
         *
         * Instantiates CPEngSearchTransaction2 object and connects
         * it to identified Presence Engine NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for CPEngSearchTransaction2
         * client side active objects. These active objects are used when
         * asynchronously delivering events to observers.
         *
         * @return New CPEngSearchTransaction2 instance.
         */
        IMPORT_C static CPEngSearchTransaction2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngSearchTransaction2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngSearchTransaction2();



    private:

        CPEngSearchTransaction2();



        //-----------------------------------------------------------------------
    public: /* Network search transaction */



        /**
         * Tests is the search already active.
         *
         * @since 3.0
         * @return ETrue if operation is active. Else EFalse.
         */
        IMPORT_C TBool IsSearchFromNetworkActive() const;


        /**
         * Performs the network search.
         *
         * There can be just one active search operation at a time
         * per one CPEngSearchTransaction2 instance. However,
         * there can be simultaneously several search operations
         * going on within one NWSessionSlot with distinct
         * CPEngSearchTransaction2 objects.
         *
         * Takes ownership to passed search criteria(s). If method
         * returns error, the search criteria(s) ownership remains
         * on the client. In success sets the given model pointer to
         * NULL or resets the parameter criteria array.
         *
         * @since 3.0
         *
         * @param aCriteria The search criterias. If there are multiple
         * search criterias, logical AND operation is assumed between
         * the different criterias.
         * @param aSearchLimit Maximum number of search results that
         * can be received at a time.
         * @param aObserver The observer to notify from search events.
         *
         * @return Result from initiating search:
         *  - KErrNone if search succesfully initiated.
         *  - KErrInUse if search already on going from object.
         *  - KErrArgument if empty search criterias provided.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt SearchFromNetwork(
            CPEngSearchCriteria2*& aCriteria,
            TInt aSearchLimit,
            MPEngSearchTransactionObserver2& aObserver );


        IMPORT_C TInt SearchFromNetwork(
            RPointerArray< CPEngSearchCriteria2 >& aCriterias,
            TInt aSearchLimit,
            MPEngSearchTransactionObserver2& aObserver );

        /**
         * Continues the current search from given continuation index.
         *
         * Search can be continued only after starting one
         * with SearchFromNetwork() and that has returned
         * some results to clients by notifying the given observer.
         *
         * When continuing, further events are raported back to
         * original observer given in SearchFromNetwork().
         *
         * @since 3.0
         * @return Result from continuing the search:
         *  - KErrNone if search succesfully continued.
         *  - KErrInUse if search already on going.
         *  - KErrNotFound if no current search to continue.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt ContinueSearchFromNetwork( TInt aContinueIndex );


        /**
         * Stops the search.
         *
         * @since 3.0
         * @return Result from stopping the search.
         *  - KErrNone if search succesfully stopped.
         *  - KErrNotFound if no current search to stop.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt StopSearchFromNetwork();




        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngSearchTransaction2Imp*     iImp;


    };

#endif  //__CPENGSEARCHTRANSACTION2_H


//End of file




