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
* Description:  Advanced transaction status container interfaces.
*
*/

#ifndef __MPENGADVTRANSACTIONSTATUS2_H__
#define __MPENGADVTRANSACTIONSTATUS2_H__

//  INCLUDES
#include <E32Base.h>
#include "MPEngTransactionStatus2.h"


// CLASS DECLARATION
/**
 * Advanced transaction status container interface.
 * Transaction status contains errors from presence
 * transactions with the network SAP.
 *
 * Advanced interface allows to add new detailed results.
 *
 * @since 3.0
 */
class MPEngAdvTransactionStatus2 : public MPEngTransactionStatusCopy2
    {
    public: // Setter functions

        /**
         * Sets the transaction result status.
         *
         * If transaction went ok, without any
         * errors, transaction result status is KErrNone.
         * Else a error code from PresenceErrors.h
         *
         * @since 3.0
         * @param aStatus The overall error code.
         */
        virtual void SetStatus( TInt aStatus ) = 0;


        /**
         * Adds a new detailed result entry.
         *
         * Stores a new detailed result entry inside
         * the transaction status object.
         *
         * @since 3.0
         *
         * @param aOperationID The ID of operation producing
         *                    this detailed result entry.
         * @param aErrorCode Error code.
         * @param aAttributeTypeID Possible attribute type ID.
         *                         Pass NULL if no attribute type ID.
         * @param aPresenceID Possible presence ID.
         *                    Pass NULL if no presence ID.
         * @param aContactListID Possible contact list ID.
         *                       Pass NULL if no contact list ID.
         * @aDescription Possible description of the error.
         *               Pass NULL if no description.
         */
        virtual void AddDetailedResultL( TInt aOperationID,
                                         TInt aErrorCode,
                                         const TUint32* aAttributeTypeID,
                                         const TDesC* aPresenceID,
                                         const TDesC* aContactListID,
                                         const TDesC* aDescription ) = 0;

        /**
         * Adds a new data result entry.
         *
         * Stores a new data result entry inside
         * the transaction status object.
         *
         * @since 3.0
         *
         * @param aOperationID The ID of operation producing
         * this data  result entry.
         * @param aDataResult The data result. Function takes the ownership
         * of given buffer only in full success. Buffer may not be NULL.
         */
        virtual void AddDataResultL( TInt aOperationID,
                                     HBufC16* aDataResult ) = 0;



    public: // Getter functions


        /**
         * Getter for data result.
         *
         * Gets a "first" data result entry of asked operation ID.
         * If there isn't any data result of asked operation, returns
         * KErrNotFound. Else returns KErrNone.
         * Subsequent detailed results can be obtained by calling
         * GetNextDetailedResultByOperation().
         *
         * @since 3.0
         * @param aOperationID Defines which transaction operation type to get.
         * @param aDataResult On the return contains first found data result.
         * If no data result is found, contains on the return empty descriptor.
         * Note: No need to construct a local buffer for descriptor,
         * pass just TPtrC and on the return TPtrC is set to point
         * correct data.
         *
         * @return Status of getting the data result.
         * KErrNone if a data result of the asked operation is found.
         * Else KErrNotFound.
         */
        virtual TInt GetFirstDataResultByOperation( TInt aOperationID,
                                                    TPtrC16& aDataResult ) = 0;


        /**
         * Getter for data result.
         *
         * Gets a "next" data result entry of asked operation.
         * (Operation criteria is set by calling first GetFirstDataResultByOperation().
         * Once setted operation criteria stays in use untill new criteria is set by
         * GetFirstDataResultByOperation().)
         *
         * If there isn't any more data results of required operation,
         * returns KErrNotFound. Else returns KErrNone.
         *
         * If operation criteria hasn't been set previously, panics with
         * "PEngTransStat", 3.
         *
         * @since 3.0
         * @param aDataResult On the return contains first found data result.
         * If no data result is found, contains on the return empty descriptor.
         * Note: No need to construct a local buffer for descriptor,
         * pass just TPtrC and on the return TPtrC is set to point
         * correct data.
         *
         * @return Status of getting the data result.
         * KErrNone if a data result of the asked operation is found.
         * Else KErrNotFound.
         */
        virtual TInt GetNextDataResultByOperation( TPtrC16& aDataResult ) = 0;




    public:  //Destructor


        /**
         * Virtual inline destructor.
         *
         * Transaction status objects can be
         * destroyed using this interface.
         */
        virtual ~MPEngAdvTransactionStatus2() {};

    };

#endif      //  __MPENGADVTRANSACTIONSTATUS2_H__

//  End of File

