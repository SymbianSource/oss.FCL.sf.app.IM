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
*     Interface for CImpsKey
*
*/


#ifndef IMPS_CIMPSKEY_H
#define IMPS_CIMPSKEY_H

//  INCLUDES
#include "impsdataaccessorapi.h"


// DATA TYPES
enum TImpsKeyOptions
    {
    EPartialKey = 0,
    EWholeKey,
    EPartialKeyLonger
    };

// FORWARD DECLARATIONS
#ifdef _DEBUG
class RFile;
#endif

// CLASS DECLARATION

//
// Currently index is limited to 128 possible values.
// If this value is exceeded the implementation must be redesigned
//

/**
*  CImpsKey class
*  This class is used for identifying different elements
*/
class CImpsKey: public MImpsKey, public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsKey* NewL( );

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsKey* NewLC( );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CImpsKey();

    public: // New functions

        /**
        * Add new elem to the key
        * @param aElemvalue Element value
        * @param aKeyType  Key type
        * @return void
        */
        void AddL( const TInt aElemvalue , enum TImpsKeyType aKeyType = EImpsKeyTypeIM );

        /**
        * Count the elements in the key
        * @return TInt Number of elements in key
        */
        inline TInt Count( ) const;

        /**
        * Pop the last element from the key
        * If too much is tried the the function leaves
        * @param aCount how many items to pop. default is 1
        * @return void
        */
        void PopL( TInt aCount = 1 );

        /**
        * Get the requested element from the key
        * @param aIndex Which element
        * @param aKeyType Return keytype of element
        * @return TInt
        */
        TInt GetElementL( const TInt aIndex, TImpsKeyType& aKeyType ) const;

        /**
        * Replaces the last element in the key
        * @param aElemvalue  New element value
        * @param aKeyType keytype of element
        * @return void
        */
        void ReplaceLastL( const TInt aElemvalue , enum TImpsKeyType aKeyType = EImpsKeyTypeIM );

        /**
        * Resets the key to empty key
        * @return void
        */
        void Reset( );

        /**
        * Equal operator
        * @param aKey to compare with
        * @return 1 = true, 0 is false
        */
        TInt Compare( MImpsKey &aKey );

        /**
        * Copy operator
        * @return copied key
        */
        CImpsKey* CopyL( ) const;

        /**
        * Check Key
        * @param aKey Key to be checked
        * @param aOptions Checking options
        * @return ETrue if checked ok
        */
        TBool CheckKey( const CImpsKey *aKey, TImpsKeyOptions aOptions ) const;

        /**
        * Equal operator
        * @param key to compare with
        * @return 1 = true, 0 is false
        */
        inline TInt operator==( const CImpsKey &aKey ) const;

        /**
        * Size
        * @return size of the key
        */
        TInt Size( ) const;

        /**
        * Pack
        * @param start pointer
        * @return error or KErrNone in success
        */
        TInt Pack( const TUint8*& aPtrStart ) const;

        /**
        * UnPackL
        * @param Pointer with stream to unpack
        */
        void UnPackL( TUint8*& aPtr );

        /**
        * Destructor
        */
        void Destroy( );

#ifdef _DEBUG
        /**
        * DumpToFile
        * @param file where to dump
        */
        void DumpToFileL( RFile& file ) const;
#endif

        /**
        * SetElementIndex
        * This function changes last elements index with the given index.
        * If Key size is 0 then this function doesn't do anything.
        * @param aCount is which element
        * @param aNewIndex new index for the element in the key
        */
        void SetElementIndex( TInt aCount, TInt aNewIndex );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CImpsKey();

        /**
        * Symbian OS second phase construct
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsKey( const CImpsKey& );
        // Prohibit assigment operator
        CImpsKey& operator= ( const CImpsKey& );

    private:    // Data
        HBufC16* iBuffer;
        TInt    iCount; // Number of items in the buffer
    };

#include "ImpsKey.inl"


#endif
// End of File
