/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Data accessor for imps engine
* 
*/


#ifndef CImpsDataAccessor_H
#define CImpsDataAccessor_H

//  INCLUDES
#include "impsdataaccessorapi.h"
#include "impstdataaccessor.h"

/**
*  CImpsDataAccessor
*/
class CImpsDataAccessor : public CBase, public MImpsDataAccessor
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param is a pointer to aImpsFields class
        */
        IMPORT_C static CImpsDataAccessor* NewL( CImpsFields* aImpsFields );

        /**
        * Two-phased constructor.
        * @param is a pointer to aImpsFields class
        */
        IMPORT_C static CImpsDataAccessor* NewLC( CImpsFields* aImpsFields );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CImpsDataAccessor();

    public: // Functions from base classes

        /**
        * Accessor creates an instance of MImpsKey entity.
        * It is NOT deleted by Accessor! You must do that yourself.
        * @return MImpsKey new key
        */
        inline MImpsKey* NewKeyL();

        /**
        * descriptor value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        inline void StoreDescL( MImpsKey* aKey, TPtrC aValue );

        /**
        * descriptor8 value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        inline void StoreDesc8L( MImpsKey* aKey, TPtrC8 aValue );

        /**
        * Integer value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        inline void StoreIntegerL( MImpsKey* aKey, TInt aValue );

        /**
        * boolean value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        inline void StoreBooleanL( MImpsKey* aKey, TBool aValue );

        /**
        * key is a byte sequence, which defines how value is stored
        * empty value mutator
        * @param aKey Key
        */
        inline void StoreEmptyL( MImpsKey* aKey );

        /**
        * extension attribute mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aName Extension attribute name
        * @param aValue Extension attribute value
        */
        inline void StoreExtL( MImpsKey* aKey, TPtrC8 aName, TPtrC8 aValue );

        /**
        * descriptor value accessor
        * aBuf doesn't go out of scope even if we call this function again.
        * This way there is no need to copy the buffer
        * @param aKey Key for restore
        * @param aBuf OUT Pointer to TDesC object
        * @return ETrue if key is found, EFalse otherwise
        */
        inline TBool RestoreDescL( MImpsKey* aKey, TDesC*& aBuf );

        /**
        * descriptor value accessor
        * @param aKey Key for restore
        * @param aDes OUT Pointer to TDesC8 object
        * @return ETrue if key is found, EFalse otherwise
        */
        inline TBool RestoreDesc8L( MImpsKey* aKey, TDesC8*& aDes );

        /**
        * integer value accessor
        * @param aKey Key for restore
        * @param aInt OUT TInt object
        * @return ETrue if key is found, EFalse otherwise
        */
        inline TBool RestoreIntegerL( MImpsKey* aKey, TInt& aInt );

        /**
        * Boolean value accessor
        * @param aKey Key for restore
        * @param aBuf OUT Boolean object
        * @return ETrue if key is found, EFalse otherwise
        */
        inline TBool RestoreBooleanL( MImpsKey* aKey, TBool& aBool );

        /**
        * empty value accessor
        * @param aKey Key for restore
        * @return ETrue if key is found, EFalse otherwise
        */
        inline TBool RestoreEmptyL( MImpsKey* aKey );

        /**
        * extension attribute accessor
        * @param aKey Key for restore
        * @param aName OUT Pointer to TDesC8 object
        * @param aValue OUT Pointer to TDesC8 object
        * @return ETrue if key is found, EFalse otherwise
        */
        inline TBool RestoreExtL( MImpsKey* aKey,
                                  TDesC8*& aName, TDesC8*& aValue );

        /**
        * data type accessor
        * Leaves with KErrArgument if illegal key not in WV DTD.
        * return data type. Note that one possible value is "not supported".
        * @param aKey Key type
        * @return TImpsDataType Datatype
        */
        inline TImpsDataType KeyTypeL( MImpsKey* aKey );

        /**
        * Finds the alternative key
        * This function finds if the supplied key has a child in the
        * datastorage and returns the information of the element.
        * This is usefull with e.g. TransactionContent
        * @param aKey key in
        * @param aEnum Enum value out
        * @param aIndex index out
        * @param aType type out
        * @return ETrue if found, EFalse if not found
        */
        inline TBool RestoreAlternativeL( const MImpsKey* aKey,
                                          TInt& aEnum,
                                          TInt& aIndex,
                                          TImpsKeyType& aType ) const;

        /**
        * Checks if the key exists
        * This function finds if the supplied key exists as it self or with
        * children and returns ETrue or EFalse
        * @param aKey key in
        * @return ETrue if found, EFalse if not found
        */
        inline TBool CheckBranchExistenceL( const MImpsKey* aKey ) const;

        /**
        * Returns the number of transactions. This is used in MultiTrans
        * This function calculates the number of transaction elements which have
        * different index and returns it.
        * @return 0 if none found, positive for number of found
        */
        inline TInt NbrOfTransactionsL( ) const;

        /**
        * Gets the given transaction. If no transaction is found using the given
        * index, this function returns EFalse. ETrue if found.
        * The new transactions index is set to 0.
        * aFields members are also set accordingly.
        * @param aIndex Which transaction to get
        * @param aFields Fields object where the data is copied
        * If this parameter is NULL then the DataAccessors impsfiels
        * transactionID and messagetype are set from CImpsdata.
        * @return ETrue if found, EFalse if not found
        */
        inline TBool GetTransactionL( TInt aIndex, CImpsFields* aFields ) const;

        /**
        * Copy specific part of the message to the current message
        * @param aSource where to copy data elements
        * @param aKey which part of message is copied. NULL means that all data
        *        is copied.
        */
        void CopyDataL(
            MImpsDataAccessor& aSource,
            MImpsKey* aKey = NULL );

        /**
        * impsfields accessor
        * @return CImpsFields*
        */
        inline CImpsFields* GetImpsFields() const;


#ifdef _DEBUG
        void DumpToFileL( RFs& aFs, const TDesC& aFilename );
#endif

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CImpsDataAccessor();

        /**
        * Actual construction.
        * @param aImpsFields is pointer to message data storage class
        */
        void ConstructL( CImpsFields* aImpsFields );

    private:    // Data
        TImpsDataAccessor   iDataAccessor;

    };

#include "ImpsDataAccessor.inl"

#endif      // CImpsDataAccessor_H

// End of File
