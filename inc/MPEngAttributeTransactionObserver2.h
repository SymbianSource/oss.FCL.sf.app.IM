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
* Description:  Observer interface to listen attribute transaction completion.
*
*/

#ifndef __MPENGATTRIBUTETRANSACTIONOBSERVER2_H
#define __MPENGATTRIBUTETRANSACTIONOBSERVER2_H

//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS
class CPEngAttributeTransaction2;
class MPEngTransactionStatus2;


// CLASS DECLARATION

/**
 * Observer interface to receive presence attribute
 * transaction completion events.
 *
 * @since 3.0
 */
class MPEngAttributeTransactionObserver2
    {
    public: //Observer methods


        /**
         * Transaction complete handler.
         *
         * Called by CPEngAttributeTransaction2 to handle attribute
         * transaction complete. If this method leaves, error code is
         * reported back to HandleAttributeTransactionError().
         *
         * @since 3.0
         * @param aStatus Transaction status container holding result from
         *        transaction. Container ownership remains on the notfier.
         * @param aTransaction The transaction object where notification
         *        is coming from. No ownership transferred.
         * @param aTransactionOperation ID of the completed transaction.
         *        Transaction operation IDs are listed in
         *        PEngPresenceEngineConsts2.h TPEngTransactionOperation
         *        enumeration. Client can use transaction ID to figure
         *        out what operation completed.
         */
        virtual void HandleAttributeTransactionCompleteL(
            MPEngTransactionStatus2& aStatus,
            CPEngAttributeTransaction2& aTransaction,
            TInt aTransactionOperation ) = 0;



        /**
         * Failure handler.
         *
         * Called to handle leave errors from
         * HandleAttributeTransactionCompleteL().
         *
         * @since 3.0
         * @param aError The leave error from transaction complete handling.
         * @param aTransaction The transaction object which event
         *        handling failed.
         * @param aTransactionOperation ID of the transaction operation.
         *        which event handling failed. Transaction operation
         *        IDs are listed in PEngPresenceEngineConsts2.h
         *        TPEngTransactionOperation enumeration.
         */
        virtual void HandleAttributeTransactionError(
            TInt aError,
            CPEngAttributeTransaction2& aTransaction,
            TInt aTransactionOperation ) = 0;


    private: //Extensions

        /**
         * Reserved virtual table slots for future use.
         * Do not implement this.
         *
         * @since 3.0
         */
        virtual TInt AttributeTransactionReserved() {
            return KErrNotSupported;
            }



    protected:  //Destructor

        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngAttributeTransactionObserver2() { }

    };



#endif //__MPENGATTRIBUTETRANSACTIONOBSERVER2_H


//End of file

