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
* Description:  Observer interface to listen attribute list transaction completion.
*
*/

#ifndef __MPENGATTRIBUTELISTTRANSACTIONOBSERVER2_H
#define __MPENGATTRIBUTELISTTRANSACTIONOBSERVER2_H

//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS
class CPEngAttributeListTransaction2;
class MPEngTransactionStatus2;



// CLASS DECLARATION

/**
 * Observer interface to receive presence attribute list
 * transaction completion events.
 *
 * @since 3.0
 */
class MPEngAttributeListTransactionObserver2
    {
    public: //Observer methods


        /**
         * Transaction complete handler.
         *
         * Called by CPEngAttributeListTransaction2 to handle attribute list
         * transaction complete. If this method leaves, error code is
         * reported back to HandleAttributeListTransactionError().
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
        virtual void HandleAttributeListTransactionCompleteL(
            MPEngTransactionStatus2& aStatus,
            CPEngAttributeListTransaction2& aTransaction,
            TInt aTransactionOperation ) = 0;


        /**
         * Failure handler.
         *
         * Called to handle leave errors from
         * HandleAttributeListTransactionCompleteL().
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
        virtual void HandleAttributeListTransactionError(
            TInt aError,
            CPEngAttributeListTransaction2& aTransaction,
            TInt aTransactionOperation ) = 0;



    private: //Extensions

        /**
         * Reserved virtual table slots for future use.
         * Do not implement this.
         *
         * @since 3.0
         */
        virtual TInt AttributeListTransactionReserved() {
            return KErrNotSupported;
            }



    protected:  //Destructor

        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngAttributeListTransactionObserver2() { }

    };



#endif //__MPENGATTRIBUTELISTTRANSACTIONOBSERVER2_H


//End of file




