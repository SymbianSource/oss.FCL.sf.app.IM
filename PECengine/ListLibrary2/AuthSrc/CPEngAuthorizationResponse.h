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
* Description:  Container of one reactive authorization response
*
*/

#ifndef __CPENGAUTHORIZATIONRESPONSE_H__
#define __CPENGAUTHORIZATIONRESPONSE_H__


//  INCLUDES
#include <e32base.h>
#include "CPEngAuthorizationItem.h"
#include "MPEngAuthorizationStatus.h"


// CLASS DECLARATION
/**
 * Container of one reactive authorization response
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngAuthorizationResponse : public CPEngAuthorizationItem,
            public MPEngAuthorizationStatus
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor,
         */
        static CPEngAuthorizationResponse* NewLC( const TDesC& aUserId,
                                                  TInt& aSize );

        /**
         * Two-phased constructor, constructs class from the stream
         */
        static CPEngAuthorizationResponse* NewLC( RReadStream& aStream,
                                                  TInt& aSize );

        /**
         * Destructor.
         */
        virtual ~CPEngAuthorizationResponse();



    public: // from CPEngAuthorizationItem

        /**
         * Update local flag
         * @see <CPEngAuthorizationItem.h>
         */
        void DoUpdateLocalFlags( const CPEngAuthorizationItem& aSource );



    public: // From MPEngAuthorizationStatus

        /**
         * User id of the user authorization was issued to
         * @since 3.0
         * @see <MPEngAuthorizationStatus.h>
         */
        const TDesC& UserId() const;


        /**
         * Status of the authorization
         * @since 3.0
         * @see <MPEngAuthorizationStatus.h>
         */
        TInt AuthorizationStatus() const;


        /**
         * List of authorized attributes
         * @since 3.0
         * @see <MPEngAuthorizationStatus.h>
         */
        const RArray<TUint32>& AuthorizedAttributes() const;

        /**
         * Contact item extension interface.
         * @since 3.0
         * @see <MPEngAuthorizationStatus.h>
         */
        MPEngAuthorizationStatusExt* Extension() {
            return NULL;
            };
        const MPEngAuthorizationStatusExt* Extension() const {
            return NULL;
            };


    public: // New functions

        /**
         * Set Response type
         *
         * @since 3.0
         * @param aResponseType type of the response
         */
        void SetResponseType( TInt aResponseType );


        /**
         * Set attributes to be authorized
         * If already authorized attributes are passed, they are included
         * as well then, in order to keep their authorization
         *
         * @since 3.0
         * @param aAttributes attributes to be authorized
         */
        void SetAttributesToAuthorizeL(
            const TArray<TUint32>& aAttributes,
            const RArray<TUint32>* aAuthorizedAttributes );

        /**
         * Add Attribute to the authorized attributes
         *
         * @since 3.0
         * @param aAttribute attribute to add to authorized attributes
         */
        void AddAttributeL( TUint32 aAttribute );


        /**
         * Externalize to the given stream
         * @since 3.0
         * @param aStream write stream
         */
        void ExternalizeL( RWriteStream& aStream ) const ;


        /**
         * Internalize from the given stream
         * @since 3.0
         * @param aStream read stream
         */
        void InternalizeL( RReadStream& aStream );


    public: // Function for the

        /**
         * Compare function toe use with RPointerArray.
         *
         * Two instances are compared based on the TDesC.CompareF function
         * of the requesting Id
         *
         * @since 3.0
         * @param aFirst fist class for comparison
         * @param aSecond second class for comparison
         * @return -1 if fist is smaller,
         *         +1 is second is smaller,
         *         0 if they are same
         */
        static TInt Compare( const CPEngAuthorizationResponse& aFirst,
                             const CPEngAuthorizationResponse& aSecond );

    private: // constructors

        /**
         * C++ constructor.
         */
        CPEngAuthorizationResponse( TInt& aSize );

        /**
         * Symbian constructor.
         */
        void ConstructL( const TDesC& aUserId );


    private:    // Data

        /// OWN: Response type
        TInt                      iResponseType;

        /// OWN: Attributes to be authorized
        RArray<TUint32>           iAttributes;

    };

#endif      // __CPENGAUTHORIZATIONRESPONSE_H__

// End of File

