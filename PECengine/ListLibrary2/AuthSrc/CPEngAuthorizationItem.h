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
* Description:  Container of one reactive authorization item
*
*/

#ifndef __CPENGAUTHORIZATIONITEM_H__
#define __CPENGAUTHORIZATIONITEM_H__


//  INCLUDES
#include <e32base.h>
#include "MPEngAuthorizationRequest.h"


// FORWARD DECLARATION
class   RWriteStream;
class   RReadStream;


// CLASS DECLARATION
/**
 * Container of the one reactive authorization item
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngAuthorizationItem : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CPEngAuthorizationItem();


    public: // New public functions

        /**
         * User id of the user who requested authorization
         *
         * @since 3.0
         * @return User id
         */
        const TDesC& Id() const;


        /**
         * Get local flag value
         * @since 3.0
         * @return Local flag value
         */
        TInt LocalFlags() const;


        /**
         * Set local flag value
         * @since 3.0
         * @param Local flag value
         */
        void SetLocalFlags( TInt aLocalFlag );


        /**
         * Template function for derived classes to
         * update local flags
         *
         * @since 3.0
         * @param aSource source item
         */
        virtual void DoUpdateLocalFlags(
            const CPEngAuthorizationItem& aSource ) = 0;


    protected: // Streaming Support

        /**
         * Externalizes to the given stream
         * @since 3.0
         * @param aStream write stream
         */
        void ExternalizeL( RWriteStream& aStream ) const ;

        /**
         * Internalizes from the given stream
         * @since 3.0
         * @param aStream read stream
         */
        void InternalizeL( RReadStream& aStream );


        /**
         * Externalize TUint array
         *
         * @since 3.0
         * @param aStream write stream
         * @param aArray array
         */
        static void ExternalizeArrayL( RWriteStream& aStream,
                                       const RArray<TUint32>& aArray );

        /**
         * Internalize TUint array
         *
         * @since 3.0
         * @param aStream read stream
         * @param aArray array
         */
        static void InternalizeArrayL( RReadStream& aStream,
                                       RArray<TUint32>& aArray,
                                       TInt& aSize );

        /**
         * Size of TUint aArray for stream.
         *
         * @since 3.0
         * @param aArray array
         * @return size of array
         */
        static TInt SizeOfArray( const RArray<TUint32>& aArray );


        /**
         * Add Attribute to the array
         *
         * @since 3.0
         * @param aArray array of attributes to add atribute
         * @param aAttribute attribute to add to requested attributes
         * @rturn size of which array size was increased
         */
        static TInt AddAttributeToArrayL( RArray<TUint32>& aArray,
                                          TUint32 aAttribute );


        /**
         * Copy TUint array.
         *
         * @since 3.0
         * @param aSource source array
         * @param aTarget target array
         * @return difference in the size of array
         */
        static TInt CopyArrayContentL( RArray<TUint32>& aTarget,
                                       const TArray<TUint32>& aSource );


    protected: // constructors

        /**
         * C++ constructor.
         */
        CPEngAuthorizationItem( TInt& aSize );

        /**
         * Symbian constructor
         */
        void ConstructL( const TDesC& aUserId );


    protected: // data, used by derived classes

        /// REF: Size of the item
        TInt&                               iSize;

        /// OWN: TInt local cached flag
        TUint32                             iLocalFlags;

    private:    // Data

        /// OWN: Requesting contact Id
        HBufC*                              iUserId;
    };

#endif      // __CPENGAUTHORIZATIONITEM_H__

// End of File

