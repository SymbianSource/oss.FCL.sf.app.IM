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
* Description:  Presence search transactions observer.
*
*/

#ifndef __MPENGSEARCHTRANSACTIONOBSERVER2_H
#define __MPENGSEARCHTRANSACTIONOBSERVER2_H

//  INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS
class CPEngSearchTransaction2;
class CPEngSearchResult2;
class MPEngTransactionStatus2;


// CLASS DECLARATION

/**
 * Observer interface to receive search
 * transaction events.
 *
 * @since 3.0
 */
class MPEngSearchTransactionObserver2
    {
    public: //Observer methods

        /**
         * Search transaction event handler.
         *
         * Called by CPEngSearchTransaction2 to handle search
         * transaction events. If this method leaves, error code is
         * reported back to HandleSearchTransactionNotifyError().
         *
         * @since 3.0
         * @param aStatus Transaction status container holding result from
         *        transaction. Container ownership remains on the notfier.
         * @param aTransaction The transaction object where notification
         *        is coming from. No ownership transferred.
         * @param aSearchResult The search result object holding the
         *        result details. No ownership transferred.
         * @param aTransactionOperation ID of the completed transaction
         *        Transaction operation IDs are listed in
         *        PEngPresenceEngineConsts2.h TPEngTransactionOperation
         *        enumeration. Client can use transaction ID to figure
         *        out what operation completed.
         */
        virtual void HandleSearchTransactionCompleteL(
            MPEngTransactionStatus2& aStatus,
            CPEngSearchTransaction2& aTransaction,
            CPEngSearchResult2& aSearchResult,
            TInt aTransactionOperation ) = 0;


        /**
         * Failure handler.
         *
         * Called to handle leave errors from
         * HandleSearchTransactionCompleteL().
         *
         * @since 3.0
         * @param aError The leave error from transaction complete handling.
         * @param aTransaction The transaction object which event
         *        handling failed.
         * @param aTransactionOperation ID of the transaction operation
         *        which event handling failed. Transaction operation
         *        IDs are listed in PEngPresenceEngineConsts2.h
         *        TPEngTransactionOperation enumeration.
         */
        virtual void HandleSearchTransactionError(
            TInt aError,
            CPEngSearchTransaction2& aTransaction,
            TInt aTransactionOperation ) = 0;


    private: //Extensions

        /**
         * Reserved virtual table slots for future use.
         * Do not implement this.
         *
         * @since 3.0
         */
        virtual TInt SearchTransactionReserved() {
            return KErrNotSupported;
            }



    protected:  //Destructor

        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngSearchTransactionObserver2() { }

    };



#endif  //__MPENGSEARCHTRANSACTIONOBSERVER2_H



//End of file




