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
* Description: Header file for data accessor
* 
*/


#ifndef TImpsDataAccessor_H
#define TImpsDataAccessor_H

//  INCLUDES
#include "impsdataaccessorapi.h"

// DATA TYPES
// Data types for the utility class
enum TAccessorDataType
    {
    EStringType = 0,
    EString8Type,
    EIntegerType,
    EBooleanType,
    EEmptyType,
    EExtensionType
    };



// FORWARD DECLARATIONS
class CImpsFields;
class CImpsKey;

// CLASS DECLARATION

// This is a utility class for accessor
class TAccessorData
    {
    public:
        TDesC*            iValuePointer;
        TDesC8*           iValuePointer8;
        TPtrC             iValue;
        TPtrC8            iValue8;
        TInt              iValueInt;
        TBool             iValueBool;
        TAccessorDataType iType;
    };


/**
*  TImpsDataAccessor
*/
class TImpsDataAccessor : public CBase, public MImpsDataAccessor
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * If this constructor is used, the member variable must be
        * set by using method SetImpsFields( CImpsFields* aImpsFields )
        */
        IMPORT_C TImpsDataAccessor( );

        /**
        * C++ default constructor.
        */
        IMPORT_C TImpsDataAccessor( CImpsFields* aImpsFields );

        /**
        * Set the member variable
        */
        IMPORT_C void SetImpsFields( CImpsFields* aImpsFields );

        /**
        * Get the member variable
        */
        IMPORT_C CImpsFields* GetImpsFields( ) const;

        /**
        * Destructor.
        */
        IMPORT_C virtual ~TImpsDataAccessor();

    public: // Functions from base classes

        /**
        * Accessor creates an instance of MImpsKey entity.
        * It is NOT deleted by Accessor! You must do that yourself.
        * @return MImpsKey new key
        */
        IMPORT_C MImpsKey* NewKeyL();

        /**
        * descriptor value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        IMPORT_C void StoreDescL( MImpsKey* aKey, TPtrC aValue );

        /**
        * descriptor8 value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        IMPORT_C void StoreDesc8L( MImpsKey* aKey, TPtrC8 aValue );

        /**
        * Integer value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        IMPORT_C void StoreIntegerL( MImpsKey* aKey, TInt aValue );

        /**
        * boolean value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        IMPORT_C void StoreBooleanL( MImpsKey* aKey, TBool aValue );

        /**
        * key is a byte sequence, which defines how value is stored
        * empty value mutator
        * @param aKey Key
        */
        IMPORT_C void StoreEmptyL( MImpsKey* aKey );

        /**
        * extension attribute mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aName Extension attribute name
        * @param aValue Extension attribute value
        */
        IMPORT_C void StoreExtL( MImpsKey* aKey, TPtrC8 aName, TPtrC8 aValue );

        /**
        * descriptor value accessor
        * aBuf doesn't go out of scope even if we call this function again.
        * This way there is no need to copy the buffer
        * @param aKey Key for restore
        * @param aBuf OUT Pointer to TDesC object
        * @return EFalse if key is not found
        *         ETrue if key is found
        */
        IMPORT_C TBool RestoreDescL( MImpsKey* aKey, TDesC*& aBuf );

        /**
        * descriptor value accessor
        * This way there is no need to copy the buffer
        * @param aKey Key for restore
        * @param aDes OUT Pointer to TDesC8 object
        * @return EFalse if key is not found
        *         ETrue if key is found
        */
        IMPORT_C TBool RestoreDesc8L( MImpsKey* aKey, TDesC8*& aDes );

        /**
        * integer value accessor
        * @param aKey Key for restore
        * @param aInt OUT
        * @return EFalse if key is not found
        *         ETrue if key is found
        */
        IMPORT_C TBool RestoreIntegerL( MImpsKey* aKey, TInt& aInt );

        /**
        * Boolean value accessor
        * @param aKey Key for restore
        * @param aBool OUT Boolean object
        * @return EFalse if key is not found
        *         ETrue if key is found
        */
        IMPORT_C TBool RestoreBooleanL( MImpsKey* aKey, TBool& aBool );

        /**
        * empty value accessor
        * @param aKey Key for restore
        * @return EFalse if key is not found
        *         ETrue if key is found
        */
        IMPORT_C TBool RestoreEmptyL( MImpsKey* aKey );

        /**
        * extension attribute accessor
        * @param aKey Key for restore
        * @param aName OUT Pointer to TDesC8 object
        * @param aValue OUT Pointer to TDesC8 object
        * @return EFalse if key is not found
        *         ETrue if key is found
        */
        IMPORT_C TBool RestoreExtL( MImpsKey* aKey,
                                    TDesC8*& aName, TDesC8*& aValue );

        /**
        * data type accessor
        * Leaves with KErrArgument if illegal key not in WV DTD.
        * return data type. Note that one possible value is "not supported".
        * @param aKey Key type
        * @return TImpsDataType Datatype
        */
        IMPORT_C TImpsDataType KeyTypeL( MImpsKey* aKey );

        /**
        * Finds the alternative key
        * This function finds if the supplied key has
        * a child in the datastorage
        * and returns the information of the element.
        * This is usefull with e.g. TransactionContent
        * @param aKey key in
        * @param aEnum Enum value out
        * @param aIndex index out
        * @param aType type out
        * @return ETrue if found, EFalse if not found
        */
        IMPORT_C TBool RestoreAlternativeL( const MImpsKey* aKey,
                                            TInt& aEnum, TInt& aIndex, TImpsKeyType& aType ) const;

        /**
        * Checks if the key exists
        * This function finds if the supplied key exists
        * as it self or with children
        * and returns ETrue or EFalse
        * @param aKey key in
        * @return ETrue if found, EFalse if not found
        */
        IMPORT_C TBool CheckBranchExistenceL( const MImpsKey* aKey ) const;

        /**
        * Returns the number of transactions. This is used in MultiTrans
        * This function calculates the number of transaction elements which
        * have different index and returns it.
        * @return 0 if none found, positive for number of found
        */
        IMPORT_C TInt NbrOfTransactionsL( ) const;

        /**
        * Gets the given transaction. If no transaction is found using the
        * given index, this function returns EFalse. ETrue if found.
        * The new transactions index is set to 0.
        * aFields members are also set accordingly.
        * @param aIndex Which transaction to get
        * @param aFields Fields object where the data is copied
        * If this parameter is NULL then the DataAccessors impsfiels
        * transactionID and messagetype are set from CImpsdata.
        * @return ETrue if found, EFalse if not found
        */
        IMPORT_C TBool GetTransactionL( TInt aIndex,
                                        CImpsFields* aFields ) const;

        /**
        * Copy specific part of the message to the current message
        * @param aSource where to copy data elements
        * @param aKey which part of message is copied. NULL means that all data
        *        is copied.
        */
        IMPORT_C void CopyDataL(
            MImpsDataAccessor& aSource,
            MImpsKey* aKey );

        void DumpToFileL( RFs& aFs, const TDesC& aFilename );
    private:


        /**
        * Main storing function
        * @param Key to store
        */
        void DoStoreL( const CImpsKey* aKey );

        /**
        * Store headers
        * @param Key to store
        */
        void StoreHeadersL( const CImpsKey* aKey );

        /**
        * Main restoring function
        * @param Key to search
        * @return ETrue if found, EFalse otherwise
        */
        TBool DoRestoreL( const CImpsKey* aKey );

        /**
        * Restore headers
        * @param Key to search
        * @return ETrue if found, EFalse otherwise
        */
        TBool RestoreHeadersL( const CImpsKey* aKey );

        // By default, prohibit copy constructor
        TImpsDataAccessor( const TImpsDataAccessor& );

        // Prohibit assigment operator
        TImpsDataAccessor& operator= ( const TImpsDataAccessor& );

    private:    // Data
        CImpsFields*      iImpsFields;
        TAccessorData     iData;
    };

#endif      // TImpsDataAccessor_H

// End of File
