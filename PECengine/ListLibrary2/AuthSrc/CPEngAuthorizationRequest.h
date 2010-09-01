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
* Description:  Container of the one reactive authorization request
*
*/

#ifndef __CPENGAUTHORIZATIONREQUEST_H__
#define __CPENGAUTHORIZATIONREQUEST_H__

//  INCLUDES
#include "CPEngAuthorizationItem.h"
#include "MPEngAuthorizationRequest.h"
#include <e32base.h>


// FORWARD DECLARATION
class MPEngAuthorizationManagerSet;


// CLASS DECLARATION
/**
 * Container of the one reactive authorization request
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngAuthorizationRequest : public CPEngAuthorizationItem,
            public MPEngAuthorizationRequest
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor,
         */
        static CPEngAuthorizationRequest* NewLC( const TDesC& aUserId,
                                                 TInt& aSize );

        /**
         * Two-phased constructor, constructs class from the Stream
         */
        static CPEngAuthorizationRequest* NewLC( RReadStream& aStream,
                                                 TInt& aSize );

        /**
         * Destructor.
         */
        virtual ~CPEngAuthorizationRequest();


    public: // From CPEngAuthorizationItem

        /**
         * Update local flag.
         * @see <CPEngAuthorizationItem.h>
         */
        void DoUpdateLocalFlags( const CPEngAuthorizationItem& aSource );



    public: // From MPEngAuthorizationRequest

        /**
         * User id of the user who requested authorization
         * @see <MPEngAuthorizationRequest.h>
         */
        const TDesC& UserId() const;


        /**
         * List of request attributes
         * @see <MPEngAuthorizationRequest.h>
         */
        const RArray<TUint32>& RequestedAttributes() const;


        /**
         * State of the authorization
         * @see <MPEngAuthorizationRequest.h>
         */
        MPEngAuthorizationRequest::TPEngAuthorizationRequestState AuthorizationState() const;


        /**
         * State of the authorization response
         * @see <MPEngAuthorizationRequest.h>
         */
        MPEngAuthorizationRequest::TPEngAuthorizationResponseType AuthorizationResponse() const;


        /**
         * List of authorized attributes
         * @see <MPEngAuthorizationRequest.h>
         */
        const RArray<TUint32>& AuthorizedAttributes() const;


        /**
         * Set authorization response
         * @see <MPEngAuthorizationRequest.h>
         */
        void SetAuthorizedAttributesL( TArray<TUint32>& aAttributes,
                                       TBool aKeepOldAuthorization = ETrue );

        void DenyAuthorizationL();

        void CancelAuthorizationL();



        /**
         * Removes buffered authorization response
         * @see <MPEngAuthorizationRequest.h>
         */
        void RemoveAuthorizationResponse();


        /**
         * Extension interface.
         * @see <MPEngAuthorizationRequest.h>
         */
        MPEngAuthorizationRequestExt* Extension() {
            return NULL;
            };
        const MPEngAuthorizationRequestExt* Extension() const {
            return NULL;
            };



    public: // Authorization update

        /**
         * Set Authorization manager pointer
         *
         * @since 3.0
         * @param aAuthManager authorization manager
         */
        void SetAuthorizationManager( MPEngAuthorizationManagerSet* aAuthManager );


        /**
         * Update authorization state
         *
         * @since 3.0
         * @param aState new state of authorization request
         */
        void SetAuthState( TPEngAuthorizationRequestState aState );


        /**
         * Update authorization response
         *
         * @since 3.0
         * @param aResponse new response of authorization request
         */
        void SetAuthResponse( TPEngAuthorizationResponseType aResponse );

        /**
         * Update authorized Attributes
         *
         * @since 3.0
         * @param aAuthorizedAttr authorized attributes
         */
        void UpdateAuthorizedAttributesL( const RArray<TUint32>& aAuthorizedAttr );


        /**
         * Set requested attributes for authorization
         *
         * @since 3.0
         * @param aRequestedAttr
         */
        void UpdateRequestedAttributesL( const RArray<TUint32>& aRequestedAttr );


        /**
         * Add Attribute to the requested attributes
         *
         * @since 3.0
         * @param aAttribute attribute to add to requested attributes
         */
        void AddRequestedAttributeL( TUint32 aAttribute );


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
        static TInt Compare( const CPEngAuthorizationRequest& aFirst,
                             const CPEngAuthorizationRequest& aSecond );


    private: // constructors

        /**
         * C++ constructor.
         */
        CPEngAuthorizationRequest( TInt& aSize );

        /**
         * Symbian constructor
         */
        void ConstructL( const TDesC& aUserId );


    private:    // Data

        /// OWN: State of authorization
        TPEngAuthorizationRequestState      iState;

        /// OWN: Authorization response type
        TPEngAuthorizationResponseType      iResponseType;

        /// OWN: Request Attributes
        RArray<TUint32>                     iRequestedAttr;

        /// OWN: Authorized Attributes
        RArray<TUint32>                     iAuthorizedAttr;

        /// REF: Authorization manager
        MPEngAuthorizationManagerSet*       iAuthManager;
    };

#endif      // __CPENGAUTHORIZATIONREQUEST_H__

// End of File

