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
* Description:  Declares group list array
 *
*/


#ifndef CCAGROUPLISTARRAYPC_H
#define CCAGROUPLISTARRAYPC_H

//  INCLUDES
#include "impsbuilddefinitions.h"

#include <bamdesca.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class MCAStoredGroup;

// CLASS DECLARATION

/**
 *  Group List Array
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */
class CCAGroupListArrayPC : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @return New instance of chat list array
         */
        static CCAGroupListArrayPC* NewL();

        /**
         * Destructor.
         */
        ~CCAGroupListArrayPC();

    public: // Functions from base classes

        /**
         * From MDesCArray Returns the number of descriptor elements in a descriptor array
         * @return The number of descriptor elements in a descriptor array
         */
        TInt MdcaCount() const;

        /**
         * From MDesCArray Indexes into a descriptor array
         * @param aIndex The position of the descriptor element within a
         *		  descriptor array. The position is relative to zero; i.e. zero
         *        implies the first descriptor element in a descriptor array.
         * @return A 16 bit non-modifiable pointer descriptor representing the
         *		   descriptor element located at position aIndex within a
         *		   descriptor array.
         */
        TPtrC MdcaPoint( TInt aIndex ) const;

    public: // New functions

        /**
         * Returns a reference to the element located at the specified
         * position within the array.
         * @param aIndex The position of an element within array
         * @return pointer to the element located at position given position
         */
        MCAStoredGroup* operator [] ( TInt aIndex ) const;

        /**
         * Sorts the array
         * @since 2.1
         */
        void Sort();

        /**
         * Adds an item to array
         * @since 2.1
         * @param aGroup Added group
         */
        void Add( const MCAStoredGroup* aGroup );

        /**
         * Remove an item from given index
         * @since 2.1
         * @paran aIndex Index of removed item
         */
        void Remove( TInt aIndex );

        /**
         * Returns the reference of wrapped array
         * @return Array that contains pointers to MCAStoredGroup
         */
        RPointerArray<MCAStoredGroup>& Array();

        /**
         * Returns an index of given group or KErrNotFound
         * @since 2.1
         * @param aGroupId Group to find
         * @return KErrNone if found
         */
        TInt Find( const TDesC& aGroupId );


        /**
         * Returns the reference of wrapped array
         * @param aIndex of the item
         * @return const ptr to MCAStoredGroup
         */
        MCAStoredGroup* GroupAt( TInt aIndex ) const;

    private:

        /**
         * C++ default constructor.
         * @param aGroupArray Array of group datas
         */
        CCAGroupListArrayPC();

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

    private:    // Data

        //owns
        HBufC* iLine;

        //owns. Array MCAStoredGroup
        RPointerArray<MCAStoredGroup> iGroupArray;


    };

#endif      // CCAGROUPLISTARRAYPC_H   

// End of File
