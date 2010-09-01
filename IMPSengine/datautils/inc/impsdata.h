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
* Description: 
*     Interface for CImpsData
*
*/


#ifndef IMPS_CIMPSDATA_H
#define IMPS_CIMPSDATA_H

//  INCLUDES
#include "impsdataaccessorapi.h"
#include "impskey.h"

#ifdef _DEBUG
#include    <f32file.h>
#endif


// FORWARD DECLARATIONS
class CImpsKey;

// CLASS DECLARATION

/**
*  CImpsElement the key and corresponding value
*
*/
class CImpsElement : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor
        */
        static CImpsElement* NewL( );

        /**
        * Destructor.
        */
        virtual ~CImpsElement();

    public: // New functions

        /**
        * SetStringL
        * @param Key in
        * @param string to store
        * @return void
        */
        void SetStringL( const CImpsKey* aKey, const TDesC& aBuf );

        /**
        * SetString8L
        * @param Key in
        * @param string to store
        * @return void
        */
        void SetString8L( const CImpsKey* aKey, const TDesC8& aBuf );

        /**
        * SetIntL
        * @param Key in
        * @param int to store
        * @return void
        */
        void SetIntL( const CImpsKey* aKey, TInt aInt );

        /**
        * SetBooleanL
        * @param Key in
        * @param boolean to store
        * @return void
        */
        void SetBooleanL( const CImpsKey* aKey, TBool aBool );

        /**
        * SetEmptyL
        * @param Key in
        * @return void
        */
        void SetEmptyL( const CImpsKey* aKey );

        /**
        * GetKey
        * @return Key
        */
        inline const CImpsKey* GetKey(  ) const;

        /**
        * GetKey
        * @return Key
        */
        inline CImpsKey* GetKey(  );

        /**
        * GetInt
        * @return int
        */
        inline TInt GetInt(  ) const;

        /**
        * GetString
        * @return string
        */
        inline TDesC* GetString(  ) const;

        /**
        * GetString8
        * @return string
        */
        inline TDesC8* GetString8(  ) const;

        /**
        * GetBoolean
        * @return boolean
        */
        inline TBool GetBoolean(  ) const;

        /**
        * IsEmpty
        * @return true if empty
        */
        inline TBool IsEmpty(  ) const;

        /**
        * GetType
        * @return type of the value
        */
        inline TImpsDataType GetType(  ) const;

        /**
        * Size
        * @return the size the data used by this object
        */
        TInt Size( ) const;

        /**
        * Pack
        * @param start pointer
        * @param end pointer
        * @return KErrNone if ok. error status otherwise
        */
        TInt Pack( const TUint8*& aPtrStart ) const;

        /**
        * UnPackL
        * @param Pointer with stream to unpack
        */
        void UnPackL( TUint8*& aPtr );

#ifdef _DEBUG
        /**
        * DumpToFile
        * @param file where to dump
        */
        void DumpToFileL( RFile& aFile ) const;
#endif

        /**
        * CopyL
        */
        CImpsElement* CopyL( ) const;

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CImpsElement();

        /**
        * Actual construction.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsElement( const CImpsElement& );
        // Prohibit assigment operator
        CImpsElement& operator= ( const CImpsElement& );

    private:            // Data
        CImpsKey*       iKey;
        TInt            iInt;
        TBool           iBool;
        HBufC*          iString;
        HBufC8*         iString8;
        TImpsDataType   iValueType;
        TBool           iIsEmpty; // If the element doesn't contain any values
    };


// DATA TYPES
typedef CArrayPtrFlat<CImpsElement> CImpsElements;

/**
*  CImpsData contains a vector of CImpsElements
*  Container for CImpsElements
*/
class CImpsData : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImpsData* NewL();

        /**
        * Destructor.
        */
        virtual ~CImpsData();

    public: // New functions

        /**
        * Stores new key/value pair to the datastore
        * @param aKey Key to save with the value
        * @param aBuff String value to store
        * @return void
        */
        void StoreStringL( const CImpsKey *aKey, const TDesC& aBuff );

        /**
        * Stores new key/value pair to the datastore
        * @param aKey Key to save with the value
        * @param aBuff String value to store
        * @return void
        */
        void StoreString8L( const CImpsKey *aKey, const TDesC8& aBuff );

        /**
        * Stores new key/value pair to the datastore
        * @param aKey Key to save with the value
        * @param aInt Interger value to store
        * @return void
        */
        void StoreIntL( const CImpsKey *aKey, TInt aInt );

        /**
        * Stores new key/value pair to the datastore
        * @param aKey Key to save with the value
        * @param aBool Boolean value to store
        * @return void
        */
        void StoreBooleanL( const CImpsKey *aKey, TBool aBool );

        /**
        * Stores new key and empty value pair to the datastore
        * @param aKey Key to save with the value
        * @return void
        */
        void StoreEmptyL( const CImpsKey *aKey );

        /**
        * Restores value from the store found by key
        * @param aKey Key used for selecting from the store
        * @param aBuff buffer found
        * @return true if found, false if not
        */
        TBool RestoreString( const CImpsKey *aKey, TDesC*& aBuff ) const;

        /**
        * Restores value from the store found by key
        * @param aKey Key used for selecting from the store
        * @param aBuff buffer found
        * @return true if found, false if not
        */
        TBool RestoreString8( const CImpsKey *aKey, TDesC8*& aBuff );

        /**
        * Restores value from the store found by key
        * @param aKey Key used for selecting from the store
        * @param aInt integer found
        * @return true if found, false if not
        */
        TBool RestoreInt( const CImpsKey *aKey, TInt& aInt );

        /**
        * Restores value from the store found by key
        * @param aKey Key used for selecting from the store
        * @param aBool Boolean found
        * @return true if found, false if not
        */
        TBool RestoreBoolean( const CImpsKey *aKey, TBool& aBoolean );

        /**
        * Restores value from the store found by key
        * @param aKey Key used for selecting from the store
        * @return true if found, false if not
        */
        TBool RestoreEmpty( const CImpsKey *aKey );

        /**
        * Checks if the key exists in the datastore. Options
        * are partial
        * wholekey
        * @param aKey Key used for finding from the store
        * @param aOpt Find mode (partial or whole)
        * @return CImpsKey if found, NULL if not
        */
        const CImpsKey* CheckKey( const CImpsKey *aKey, TImpsKeyOptions aOpt = EPartialKey ) const;

        /**
        * Finds the element from the list based on a key(no partial key, full key)
        * @param aKey Key used for finding from the store
        * @param type of the key.
        * @return address of the element, NULL if not found
        */
        const CImpsElement* Find( const CImpsKey *aKey, TImpsDataType aType ) const;

        /**
        * Resets the array to empty array and deletes all elemenst
        */
        inline void Reset( );

        /**
        * Size
        * @return the size the data used by this object
        */
        TInt Size( ) const;

        /**
        * Count
        * @return the number of elements in the list
        */
        inline TInt Count( ) const;

        /**
        * Pack
        * @param start pointer
        * @param end pointer
        * @return KErrNone if ok. error status otherwise
        */
        TInt Pack( const TUint8*& aPtrStart ) const;

        /**
        * UnPackL
        * @param Pointer with stream to unpack
        */
        void UnPackL( TUint8*& aPtr );
#ifdef _DEBUG
        /**
        * DumpToFileL
        * @param Filename with path where to dump the object
        */
        void DumpToFileL( RFs& aSession, const TDesC& aFileName ) const;
#endif
        /**
        * Copy
        * @param All elements which have the key in the beginning are copied
        * If aKey is NULL everything is copied
        * @param Source CImpsData
        */
        void CopyL( const CImpsKey* aKey, const CImpsData* aSource );

        /**
        * SetKeyIndex
        * This function changes all the entries in this data storage
        * which have this key. The index of the last key is changed.
        * @param Partial key ( All found keys are changed )
        * Cannot be NULL
        * @param New index for the key element
        */
        void SetKeyIndex( const CImpsKey* aKey, TInt aNewIndex );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CImpsData();

        /**
        * Symbian OS second phase construction
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsData( const CImpsData& );
        // Prohibit assigment operator
        CImpsData& operator= ( const CImpsData& );

    private:    // Data
        CImpsElements*   iElements;

    };

#include "ImpsData.inl"

#endif      // IMPS_CIMPSDATA_H

// End of File
