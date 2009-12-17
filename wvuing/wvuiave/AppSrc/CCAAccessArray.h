/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares array that contains user data
 *
*/


#ifndef CCAACCESSARRAY_H
#define CCAACCESSARRAY_H

//  INCLUDES
#include <bamdesca.h>               //MDesCArray
#include <badesca.h>


// FORWARD DECLARATIONS
class MCAGroupPropertiesPC;

// CLASS DECLARATION

/**
 *  This array class binds three different arrays to one array.
 *  MdcaPoint is overridden so that it is possible to use listbox tabulators
 *  @lib chatng.app
 *  @since 3.2
 */
class CCAAccessArray : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aGroupPC Handle to groups process component
         * @param aUserList List of users
         */
        static CCAAccessArray* NewL( MCAGroupPropertiesPC* aGroupPC,
                                     CDesCArray& aUserList );

        /**
         * Destructor.
         */
        virtual ~CCAAccessArray();

    public: // New functions

        /**
         * This method appends items to access array
         * @param aPtr Descriptor that contains WVIDs
         */
        void AppendL( const TDesC& aPtr );

        /**
         * This method returns descriptor from array that contains not
         * formatted data
         * @param aIndex Index of the item
         * @return Unformatted data descriptor
         */
        TPtrC RawData( TInt aIndex ) const;

        /**
         * This method deletes from the access array
         * @param aIndex Index of item that will be removed from array
         * @param aCount The count of items that are deleted starting from index
         */
        void Delete( TInt aIndex, TInt aCount );

        /**
         * This method resets the access array
         */
        void Reset();

    public: // Functions from MDesCArray

        /**
         * From MDesCArray returns the count of the items
         * @return number of items that are in accessarray
         */
        TInt MdcaCount() const;

        /**
         * From MDesCArray returns descriptor for given index
         * @return Formatted descriptor (\t) for given index
         */
        TPtrC MdcaPoint( TInt aIndex ) const;

    private:

        /**
         * Default constructor
         * @param aGroupPC Handle to groups process component interface
         * @param aUserList List of users
         */
        CCAAccessArray( MCAGroupPropertiesPC* aGroupPropsDialogPC, CDesCArray& aUserList );

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

    private:    // Data

        //Group PC interface. Doesn't own
        MCAGroupPropertiesPC* iGroupPropertiesPC;

        //Array that contains user ids. Doesn't own
        CDesCArray& iAccessList;

        //Buffer for line. Owns
        HBufC* iLine;
    };

#endif      // CCAACCESSARRAY_H

// End of File

