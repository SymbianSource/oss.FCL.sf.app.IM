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
* Description:  Authorization Manager abstract interface
*
*/


#ifndef __MPENGAUTHORIZATIONMANAGER_H__
#define __MPENGAUTHORIZATIONMANAGER_H__


//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class	MPEngAuthorizationRequest;
class   MPEngAuthorizationStatus;
class	MPEngAuthorizationObserver;

// CLASS DECLARATION
/**
 * Authorization Manager abstract interface
 *
 */
class MPEngAuthorizationManager
    {

    public: // New functions

        /**
         *  Close reference
         *  Clean up stack support
         *
         *  @since 3.0
         */
        virtual void Close() = 0;

        /**
         *  Coutn of all authorizations
         *
         *  @since 3.0
         *  @return count of authorizations
         */
        virtual TInt AuthorizationsCount() const = 0;

        /**
         *  Get authorization request
         *
         *  @since 3.0
         *  @param aIndex index of the authorization request
         *  @return authorization request
         */
        virtual MPEngAuthorizationRequest& Authorization(
            TInt aIndex ) = 0;

        /**
         *  Coutn of pending authorizations request
         *  This also includes authorizations which were
         *  already responded, but server reissued them again
         *
         *  @since 3.0
         *  @return count of authorizations
         */
        virtual TInt PendingAuthorizationsCount()  const = 0;

        /**
         *  Get pending authorizations request
         *  Those authorizations also includes authorizations which were
         *  already responded, but server reissued them again
         *
         *  @since 3.0
         *  @param aIndex index of the authorization request
         *  @return authorization request
         */
        virtual MPEngAuthorizationRequest& PendingAuthorization(
            TInt aIndex ) = 0;

        /**
         *  Issued Authorization status count
         *  Authorization which were responded by the requesting user
         *  @since 3.0
         *  @return count
         */
        virtual TInt RespondedAuthorizationsCount() const = 0;

        /**
         *  Issued Authorization status
         *  Authorization which were responded by the requesting user
         *  @since 3.0
         *  @param index of the responded authorization
         *  @return responded authorization
         */
        virtual const MPEngAuthorizationStatus& RespondedAuthorization(
            TInt aIndex ) const = 0;


        /**
         *  Server denied authorizations count
         *  @since 3.0
         *  @return count
         */
        virtual TInt ServerDeniedAuthorizationsCount() const = 0;

        /**
         *  Server denied authorization
         *  @since 3.0
         *  @param aIndex index of denied authorization
         *  @return served denied authorization
         */
        virtual const MPEngAuthorizationStatus& ServerDeniedAuthorization(
            TInt aIndex ) const = 0;


        /**
         *  Remove all Authorization responses
         *  and release lock for editing.
         *
         *  Function removes all buffered authorization responses and
         *  release lock which was enabled when fisrt authorization
         *  response was stored.
         *
         *  @since 3.0
         */
        virtual void RemoveAllAuthorizationResponses() = 0;

        /**
         *  Buffer with all packed authorization responses.
         *  This buffer is intended to be sent to the Transaction
         *  server in order to publish buffered transactions.
         *  Ownership of the buffer is transfered to the client.
         *
         *  @since 3.0
         *  @return buffer with packed transaction responses.
         */
        virtual HBufC16* BufferedTransactionsLC() = 0;


    public: // Authorization Observerving from MPEngAuthorizationManager

        /**
         *  Register authorization observer.
         *
         *  @since 3.0
         *  @param aObserver authorization observer
         */
        virtual void RegisterAuthorizationObserverL(
            MPEngAuthorizationObserver& aObserver ) = 0;

        /**
         *  Unregister authorization observer.
         *
         *  @since 3.0
         *  @param aObserver authorization observer
         */
        virtual void UnRegisterAuthorizationObserver(
            MPEngAuthorizationObserver& aObserver ) = 0;

    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Protected destructor to prohibite deletion trough interface.
         */
        virtual ~MPEngAuthorizationManager() {};

    };
#endif      //  __MPEngAuthorizationManager_H__

//  End of File

