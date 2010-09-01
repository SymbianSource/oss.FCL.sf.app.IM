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
* Description:  Transaction status container interfaces.
*
*/

#ifndef __MPENGTRANSACTIONSTATUS2_H__
#define __MPENGTRANSACTIONSTATUS2_H__

//  INCLUDES
#include <E32Std.h>


//FORWARD DECLARATION
class MPEngDetailedResultEntryExtension2;
class MPEngTransactionStatusExtension2;
class MPEngTransactionStatusCopy2;



// CLASS DECLARATION
/**
 * Enumeration defining different description
 * texts inside detailed result.
 *
 * @since 3.0
 */
enum TPEngDetailedResultDescriptionText2
    {
    // The failed presence id
    EPEngDTPresenceID = 1,

    // The failed contact list id
    EPEngDTContactListID = 2,

    //Description of the error if present
    EPEngDTErrorDescription = 3
    };


/**
 * Enumeration defining different description
 * integers inside detailed result.
 *
 * @since 3.0
 */
enum TPEngDetailedResultDescriptionInteger2
    {
    //The failed attribute id
    EPEngDIAttributeID = 1
    };




/**
 * Detailed result entry.
 *
 * This interface offers access to one detailed
 * result entry.
 *
 * @since 3.0
 */
class MPEngDetailedResultEntry2
    {

    public:

        /**
         * Gets the transaction operation that produced
         * this detailed error.
         *
         * @since 3.0
         * @return Error operation.
         */
        virtual TInt Operation() const = 0;


        /**
         * Gets the error code for this detailed error.
         *
         * Might be error code from common Symbian OS
         * error codes, or error code from network transaction.
         *
         * @since 3.0
         * @return Error code.
         */
        virtual TInt Error() const = 0;



        /**
         * Gets descriptive text from detailed result entry.
         * If requested description text isn't set, returns
         * KErrNotFound, else returns KErrNone and aDescriptionText
         * is set to point description value.
         *
         * @since 3.0
         * @param aDescriptionText On the return contains requested
         * description text. If requested text isn't set,
         * contains on the return empty descriptor. Note:
         * No need to construct a local buffer for descriptor,
         * pass just TPtrC and on the return TPtrC is set to point
         * correct data.
         *
         * @param aDescriptionID Which description text to get.
         * @return If description isn't set: KErrNotFound.
         * Else KErrNone.
         */
        virtual TInt GetDetailedDesc( TPtrC& aDescriptionText,
                                      TPEngDetailedResultDescriptionText2 aDescriptionID ) const = 0;


        /**
         * Gets description integer from detailed result entry.
         * If requested description integer isn't set, returns
         * KErrNotFound, else returns KErrNone and aDescriptionInt
         * is set to contain description value.
         *
         * @since 3.0
         * @param aDescriptionInt On the return contains requested
         * description integer. If requested integer isn't set,
         * contains 0 on the return.
         * @param aDescriptionID Which description integer to get.
         * @return If description isn't set: KErrNotFound:
         * Else KErrNone.
         */
        virtual TInt GetDetailedInt( TUint32& aDescriptionInt,
                                     TPEngDetailedResultDescriptionInteger2 aDescriptionID ) const = 0;



    protected: //Extensions interface

        /**
         * Detailed result entry extension interface.
         *
         * @since 3.0
         * @return Detailed result entry extension.
         * Currently NULL.
         */
        virtual MPEngDetailedResultEntryExtension2* EntryExtension() = 0;



    protected:

        /**
         * Virtual destructor.
         *
         * Detailed result entries can't be destroyed
         * using this interface.
         */
        virtual ~MPEngDetailedResultEntry2() {};
    };





// CLASS DECLARATION
/**
 * Enumeration defining different description
 * texts inside MPEngTransactionStatus2 container.
 *
 * Reserved for future use. Currently no texts.
 *
 * @since 3.0
 */
enum TPEngTransStatusDescriptionText2
    {
    };


/**
 * Enumeration defining different description
 * integers inside MPEngTransactionStatus2 container.
 *
 * Reserved for future use. Currently no integers.
 *
 * @since 3.0
 */
enum TPEngTransStatusDescriptionInteger2
    {
    };



// CLASS DECLARATION
/**
 * Transaction status container interface. Transaction
 * status contains errors from presence transaction
 * with the network SAP.
 *
 * Transaction error might be something like
 * partial failure when adding contacts to
 * contacts lists or it might be OOM error coming
 * from initiating or handling the transaction.
 *
 * @since 3.0
 */
class MPEngTransactionStatus2
    {
    public: // New functions

        /**
         * Transaction result status.
         *
         * If transaction went ok, without any
         * errors, status is KErrNone.
         *
         * If error code is something else than
         * KErrNone, there might be detailed
         * result entry giving the more detailed
         * information from the error.
         *
         * @since 3.0
         * @return The error code.
         */
        virtual TInt Status() const = 0;


        /**
         * Getter to transaction detailed result count.
         *
         * If transaction was partially successful, there will
         * be one or more detailed results, which describe what went
         * wrong in transaction. If the transaction was succesful,
         * there isn't any detailed results.
         *
         * Detailed results are presented by MPEngDetailedResultEntry2.
         *
         * @since 3.0
         * @return The detailed result count.
         */
        virtual TInt DetailedResultCount() const = 0;


        /**
         * Getter for detailed result object.
         *
         * Gets a detailed result entry. Client may not assume anything
         * about the ordering of returned detailed entries.
         *
         * @since 3.0
         * @param aIndex Defines which detailed result element to access
         *        within the transaction status. The index is relative to
         *        zero. Zero implies the first detailed result entry.
         *        If passed index is out of bounds, panics with
         *        "PEngTransStat", 1.
         *
         * @return Asked detailed result. Lifetime of returned detailed
         *         result entry is same as originating MPEngTransactionStatus2 object.
         *         No ownership transfered.
         */
        virtual const MPEngDetailedResultEntry2& DetailedResult( TInt aIndex ) const = 0;



        /**
         * Getter for detailed result object.
         *
         * Gets a pointer to "first" detailed result entry of asked operation ID.
         * If there isn't any detailed result of asked operation, returns
         * KErrNotFound. Else returns KErrNone.
         * Subsequent detailed results can be obtained by calling
         * GetNextDetailedResultByOperation().
         *
         * @since 3.0
         * @param aTransactionOperation By which transaction operation type
         *        to get detailed entries. Transaction operation IDs are listed in
         *        PEngPresenceEngineConsts2.h. TPEngTransactionOperation enumeration
         *        defines transaction IDs for presence data transactions.
         *        TPEngNWSessionSlotOperations enumeration defines IDs for NWSessionSlot
         *        operations.
         *
         * @param aDetailedResult Pointer to first found detailed result entry
         *        of asked operation type is returned here. Lifetime of
         *        returned detailed result entry is same as originating
         *        MPEngTransactionStatus2 object. If no detailed result of asked
         *        operation isn't found, NULL is returned. No ownership transfered.
         *
         * @return Status of getting the detailed result. KErrNone if a
         *         detailed result of the asked operation is found. Else KErrNotFound.
         */
        virtual TInt GetFirstDetailedResultByOperation( TInt aTransactionOperation,
                                                        const MPEngDetailedResultEntry2*& aDetailedResult ) = 0;


        /**
         * Getter for detailed result object.
         *
         * Gets a pointer to "next" detailed result entry
         * of asked operation. (Operation criteria is set by calling
         * first GetFirstDetailedResultByOperation(). Once setted operation
         * criteria stays in use untill new criteria is set by
         * GetFirstDetailedResultByOperation().)
         *
         * If there isn't any more detailed results of required operation,
         * returns KErrNotFound. Else returns KErrNone.
         *
         * If operation criteria hasn't been set previously, panics with
         * "PEngTransStat", 2.
         *
         * @since 3.0
         * @param aDetailedResult Pointer to next found detailed result entry
         *        of asked operation is returned here. Lifetime of returned detailed
         *        result entry is same as originating MPEngTransactionStatus2 object.
         *        If no more detailed results of asked operation isn't found,
         *        NULL is returned.
         *
         * @return Status of getting the detailed result. KErrNone if a
         *         detailed result of the asked operation is found. Else KErrNotFound.
         */
        virtual TInt GetNextDetailedResultByOperation( const MPEngDetailedResultEntry2*& aDetailedResult ) = 0;





    public: // Access to description fields

        /**
         * Gets descriptive text from transaction status.
         * If requested description text isn't set, returns
         * KErrNotFound, else returns KErrNone and aDescriptionText
         * is set to point description value.
         *
         * @since 3.0
         * @param aDescriptionText On the return contains requested
         * description text. If requested text isn't set, contains on
         * the return empty descriptor.
         * @param aDescriptionID Which description text to get.
         * @return If description isn't set KErrNotFound.
         * Else KErrNone.
         */
        virtual TInt GetDesc( TPtrC& aDescriptionText,
                              TPEngTransStatusDescriptionText2 aDescriptionID ) const = 0;


        /**
         * Gets descriptive integer value from transaction status.
         * If requested description integer isn't set, returns
         * KErrNotFound, else returns KErrNone and aDescriptionInt
         * is set to contain description value.
         *
         * @since 3.0
         * @param aDescriptionInt On the return contains requested
         * description integer. If requested integer isn't set,
         * contains on the return 0.
         * @param aDescriptionID Which description integer to get.
         * @return If description isn't set KErrNotFound.
         * Else KErrNone.
         */
        virtual TInt GetInt( TUint32& aDescriptionInt,
                             TPEngTransStatusDescriptionInteger2 aDescriptionID ) const = 0;





    public: // Cloning support

        /**
         * Creates a new deep copy from transaction status object.
         *
         * Creates a deep copy from transaction status object and
         * returns it ownership to client. Created object is placed
         * on the CleanupStack.
         *
         * Created copy is derived from MPEngTransactionStatus2Copy
         * interface to allow client to delete created object.
         *
         * Only data is copied. Cached settings like criterias
         * used by GetFirstDetailedResultByOperation() and
         * GetNextDetailedResultByOperation() are not copied.
         *
         * @since 3.0
         * @return New copy from MPEngTransactionStatus2.
         * Ownership of created object is returned to calling client.
         */
        virtual MPEngTransactionStatusCopy2* CloneLC() const = 0;



        /**
          * Transaction status extension interface.
          *
          * @since 3.0
          * @return Transaction status extension.
          * Currently NULL.
          */
        virtual MPEngTransactionStatusExtension2* StatusExtension() = 0;




    protected:

        /**
         * Virtual inline destructor.
         *
         * Concrete transaction status can't be
         * destroyed using this interface.
         */
        virtual ~MPEngTransactionStatus2() {};

    };



/**
 * This interface offers possibility to client to
 * control cloned transaction status object lifetime.
 *
 * @since 3.0
 */
class MPEngTransactionStatusCopy2 : public MPEngTransactionStatus2
    {
    public:
        /**
         * Cleanup support.
         *
         * Cloned transaction status objects can be
         * pushed on the CleanupStack using
         * CleanupClosePushL().
         *
         * @since 3.0
         */
        virtual void Close() = 0;


    public:

        /**
         * Virtual destructor.
         *
         * Transaction status copies can be destroyed
         * using this interface.
         */
        virtual ~MPEngTransactionStatusCopy2() {};
    };



#endif      //  __MPENGTRANSACTIONSTATUS2_H__

//  End of File


