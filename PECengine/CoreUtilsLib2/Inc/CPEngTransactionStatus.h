/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Transaction status implementation.
*
*/

#ifndef __CPENGTRANSACTIONSTATUS_H__
#define __CPENGTRANSACTIONSTATUS_H__

//  INCLUDES
#include <E32Base.h>
#include "MPEngAdvTransactionStatus2.h"


//FORWARD DECLARATION
class CPEngDetailedResultEntry;
class CPEngDataResultEntry;
class RReadStream;
class RWriteStream;



// CLASS DECLARATION

/**
 * Transaction status implementation.
 *
 * @lib
 * @since 3.0
 */
class CPEngTransactionStatus : public CBase, public MPEngAdvTransactionStatus2
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CPEngTransactionStatus* NewL();



        /**
         * Destructor.
         */
        virtual ~CPEngTransactionStatus();



    private: // Constructors.

        /**
         * C++ default constructor.
         */
        CPEngTransactionStatus();





    public: // Functions from MPEngTransactionStatus

        /**
         * From MPEngTransactionStatus
         */
        TInt Status() const;

        TInt DetailedResultCount() const;
        const MPEngDetailedResultEntry2& DetailedResult( TInt aIndex ) const;

        TInt GetFirstDetailedResultByOperation( TInt aOperationID,
                                                const MPEngDetailedResultEntry2*& aDetailedResult );
        TInt GetNextDetailedResultByOperation( const MPEngDetailedResultEntry2*& aDetailedResult );


        TInt GetDesc( TPtrC& aDescriptionText,
                      TPEngTransStatusDescriptionText2 aDescriptionID ) const;


        TInt GetInt( TUint32& aDescriptionInt,
                     TPEngTransStatusDescriptionInteger2 aDescriptionID ) const;


        MPEngTransactionStatusCopy2* CloneLC() const;


        MPEngTransactionStatusExtension2* StatusExtension();



    public: // Functions from MPEngTransactionStatusCopy

        /**
         * From MPEngTransactionStatusCopy
         */
        void Close();


    public: // Functions from MPEngAdvTransactionStatus

        /**
         * From MPEngAdvTransactionStatus
         */
        void SetStatus( TInt aStatus );
        void AddDetailedResultL( TInt aOperationID,
                                 TInt aErrorCode,
                                 const TUint32* aAttributeTypeID,
                                 const TDesC* aPresenceID,
                                 const TDesC* aContactListID,
                                 const TDesC* aDescription );

        void AddDataResultL( TInt aOperationID,
                             HBufC16* aDataResult );

        TInt GetFirstDataResultByOperation( TInt aOperationID,
                                            TPtrC16& aDataResult );
        TInt GetNextDataResultByOperation( TPtrC16& aDataResult );



    public: // New functions


        /**
         * Resets transaction status object contents.
         *
         * Status will be set to KErrNone and
         * all contained detailed entries are cleared.
         *
         * @since 3.0
         */
        IMPORT_C void Reset();



        /**
         * Imports transaction status data from given object.
         *
         * Merges transaction status from given source container
         * to called object. In merging following rules are used:
         *
         * 1. All detailed results from the source container
         *    are moved as such to the end of existing ones.
         *
         * 2. Transaction status code is handled following way:
         *
         * Status code in           Status code                 Destination container
         * destination container    in source container   ===>  end state
         * KErrNone                 KErrNone                    KErrNone
         * KErrNone                 KErrCode1                   KErrCode1
         * KErrCode1                KErrNone                    KErrCode1
         * KErrCode1                KErrCode1                   KErrCode1
         * KErrCode1                KErrCode2                   KErrCode1
         *
         * ===> First found error is kept in importing container.
         *
         * NOTE: Import succeeds always. Takes the ownership of
         * parameter container.
         *
         * @since 3.0
         * @param aContainerToImport The container to merge.
         * Takes the ownership.
         */
        IMPORT_C void ImportStatusFrom( CPEngTransactionStatus& aSource );



        /**
         * Creates a new deep copy from transaction status object.
         *
         * Creates a deep copy from transaction status object and
         * returns it ownership to client. Created object is placed
         * on the CleanupStack.
         *
         * This copy holds also internal data results.
         * CloneLC() for external clients doesn't copy data results.
         *
         * Only data is copied. Cached settings like criterias
         *
         * @since 3.0
         * @return New copy from CPEngTransactionStatus.
         * Ownership of created object is returned to calling client.
         */
        IMPORT_C CPEngTransactionStatus* FullCloneLC() const;


        /**
         * Packs the transaction status and possible data results
         * as one HBufC8.
         *
         * @since 3.0
         * @return Transaction result package.
         *         Buffer ownership is returned to caller.
         */
        IMPORT_C HBufC8* PackResultsL() const;


        /**
         * Extracts transaction results from package.
         *
         * Extracts the transaction status and possible data results
         * from package.
         *
         * @since 3.0
         * @param aResultPkg Transaction result package.
         */
        IMPORT_C void UnpackResultsL( const TDesC8& aResultPkg );





    private: // Helper functions.


        /**
         * Gets next by required operation type.
         */
        TInt LocateNextDetailByOperation( const MPEngDetailedResultEntry2*& aDetailedResult );
        TInt LocateNextDataByOperation( TPtrC16& aDataResult );

        /**
         * Merges imported containers status code to local one.
         * See MPEngTransactionStatus for merging rules.
         */
        void MergeStatusCodes( const CPEngTransactionStatus& aSource );


        /**
         * Approximates the needed stream length needed to
         * externalize the object data.
         *
         * @since 3.0
         * @return Approximation of needed space in bytes.
         */
        TInt ExternalizeSize() const;


        /**
         * Externalizes objects state to given write stream.
         *
         * @since 3.0
         */
        void ExternalizeL( RWriteStream& aStream ) const;


        /**
         * Internalizes this objects state from given
         * read stream. Before reading, previous
         * contents are reseted.
         *
         * @since 3.0
         */
        void InternalizeL( RReadStream& aStream );


        /**
         * Clones transaction container base data.
         * e.g. status code and detailed results
         * visible to external clients.
         *
         * @since 3.0
         * @return New cloned CPEngTransactionStatus instance.
         */
        CPEngTransactionStatus* BaseCloneLC() const;


        /**
         * Panics with given reason.
         */
        static void Panic( TInt aReason );



    private:    // Data


        //OWN: Detailed results
        TSglQue<CPEngDetailedResultEntry>  iDetailedResultsQ;

        //OWN: Detailed results iterator
        TSglQueIter<CPEngDetailedResultEntry> iDetailedResultsQIter;

        //OWN: Count of detailed results
        TInt    iDetailedResultsCount;

        //OWN: Operation ID for detailed results filtered access
        TInt    iDetResCurrentOperationId;

        //OWN: Current index for  detailed results filtered access
        TInt    iDetResCurrentIndex;




        //OWN: Data results
        TSglQue<CPEngDataResultEntry>  iDataResultsQ;

        //OWN: Data results iterator
        TSglQueIter<CPEngDataResultEntry> iDataResultsQIter;

        //OWN: Count of data results
        TInt    iDataResultsCount;

        //OWN: Operation ID for for data results filtered access
        TInt    iDataResCurrentOperationId;

        //OWN: Current index for data results filtered access
        TInt    iDataResCurrentIndex;




        //OWN: Transaction result code, owned
        TInt    iStatusCode;


    };


#endif      //  __CPENGTRANSACTIONSTATUS_H__

// End of File


