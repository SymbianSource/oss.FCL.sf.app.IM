/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Reactive authorization request interface.
*
*/

#ifndef __MPENGAUTHORIZATIONREQUEST_H
#define __MPENGAUTHORIZATIONREQUEST_H

//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS
class MPEngAuthorizationRequestExt;


// CLASS DECLARATION
/**
 * Interface for reactive authorization request.
 *
 * @since 3.0
 */
class MPEngAuthorizationRequest
    {
    public: // Authorization Request State

        /**
         * Reactive authorization request state.
         *
         * @since 3.0
         */
        enum TPEngAuthorizationRequestState
            {
            // Authorization request is waiting for answer
            EPEngAuthPending        = 0x00001,

            // Authorization request was already answered
            EPEngAuthAnswered       = 0x00002
            };


        /**
         * Reactive authorization responses.
         *
         * @since 3.0
         */
        enum TPEngAuthorizationResponseType
            {
            //Authorization isn't yet responded
            EPEngAuthUndefined      = 0x00000,

            //Authorization was accepted
            EPEngAuthAccepted       = 0x00001,

            //Authorization was denied
            EPEngAuthDenied         = 0x00002,

            //Authorization was canceled
            EPEngAuthCanceled       = 0x00003
            };



    public: // Access to authorization request details

        /**
         * Identification of requesting user.
         *
         * Gets identification of requesting user.
         *
         * @since 3.0
         * @return Id of the requesting user.
         */
        virtual const TDesC& UserId() const = 0;


        /**
         * List of requested attributes.
         *
         * Gets list of requested attributes.
         *
         * @since 3.0
         * @return Array of requested attributes.
         */
        virtual const RArray<TUint32>& RequestedAttributes() const = 0;


        /**
         * State of the authorization request.
         *
         * Gets state of the authorization request.
         *
         * @since 3.0
         * @return State of the authorization request.
         */
        virtual TPEngAuthorizationRequestState AuthorizationState() const = 0;


        /**
         * State of the authorization response
         *
         * Gets state of the authorization response.
         *
         * If authorization response state is EPEngAuthUndefined,
         * it means that there has not been yet any response to this
         * authorization request.
         *
         * If authorization request was reissued by networ server,
         * authorization is put again in EPEngAuthPending state,
         * and response state remains in last authorization response.
         *
         * @since 3.0
         * @return state of the authorization
         */
        virtual TPEngAuthorizationResponseType AuthorizationResponse() const = 0;


        /**
         * List of authorized attributes.
         *
         * Gets the list of currently authorized attributes.
         *
         * @since 3.0
         * @return The list of currently authorized attributes.
         */
        virtual const RArray<TUint32>& AuthorizedAttributes() const = 0;



    public: //Authorization response handling

        /**
         * Authorizes the request with given attribute set.
         *
         * Authorizes the request with attribute set.
         * Pending authorization request can be:
         *  - Authorized with SetAuthorizedAttributesL()
         *  - Denied with DenyAuthorizationL()
         *
         * Already responded authorization request can be:
         *  - Reauthorized: SetAuthorizedAttributesL()
         *    New authorization overwrites previous response.
         *  - Canceled: CancelAuthorizationL()
         *
         * Prior client can perform response handling operations,
         * it needs to have open MPEngAuthorizationRespond object.
         * Individual authorization responses are buffered to
         * MPEngAuthorizationRespond object from which thay are
         * published to network at once.
         *
         * @since 3.0
         * @param aAttributes Attributes to be authorized.
         * @param aKeepOldAuthorization If ETrue, then
         *        already authorized attributes are kept.
         *        If EFalse, new attribute set overwrites
         *        previous attribute set.
         */
        virtual void SetAuthorizedAttributesL(
            TArray<TUint32>& aAttributes,
            TBool aKeepOldAuthorization = ETrue ) = 0;


        /**
         * Denies authorization request.
         *
         * @since 3.0
         * @see SetAuthorizedAttributesL()
         */
        virtual void DenyAuthorizationL() = 0;


        /**
         * Cancels existing authorization.
         *
         * @since 3.0
         * @see SetAuthorizedAttributesL()
         */
        virtual void CancelAuthorizationL() = 0;


        /**
         * Undo the authorization response.
         *
         * Undo the authorization response.
         * Removes response from among other
         * buffered responses.
         *
         * @since 3.0
         */
        virtual void RemoveAuthorizationResponse() = 0;



    private: //Extensions interfaces

        /**
         * Reactive authorization request extension interface.
         *
         * @since 3.0
         * @return Reactive authorization request extension.
         */
        virtual MPEngAuthorizationRequestExt* Extension() = 0;
        virtual const MPEngAuthorizationRequestExt* Extension() const = 0;



    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Protected destructor to prohibits deletion
         * through the interface.
         */
        virtual ~MPEngAuthorizationRequest() {};

    };
#endif      //  __MPENGAUTHORIZATIONREQUEST_H

//  End of File

