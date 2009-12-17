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
* Description:  Authorization engine interface
*
*/

#ifndef __MPENGAUTHORIZATIONENGINE_H__
#define __MPENGAUTHORIZATIONENGINE_H__


//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class   CPEngAuthorizationResponse;
class   CPEngAuthorizationRequest;


// CLASS DECLARATION
/**
 * Authorization engine abstract API
 */
class MPEngAuthorizationEngine
    {

    public: // New functions

        /**
         * Update Reactive authorization state
         * Store new Authorization state in the authorization engine
         *
         * @since 3.0
         * @param aAuthResponse aAuthResponse response to take details from
         * @param aPartialAcceptance if only part of the attributes can be accepted
         *        this settigs depends on the capabililty of the server
         */
        virtual void UpdateAuthorizationStateL(
            CPEngAuthorizationResponse& aAuthResponse,
            TBool aPartialAcceptance ) = 0;

        /**
         * Removes authorization request.
         */
        virtual void RemoveAuthorizationRequestL(
            CPEngAuthorizationResponse& aAuthResponse ) = 0;

        /**
         * Add Authorization request.
         * Parameter aAuthRequest must be on clean up stack before
         * calling this method and it will be removed
         *
         * @since 3.0
         * @param aAuthRequest authorization request to add
         */
        virtual void AddAuthRequestLX( CPEngAuthorizationRequest* aAuthRequest ) = 0;


        /**
         * Add Authorization status
         * Parameter aAuthStatus must be on clean up stack before calling this method
         * and it will be removed
         *
         * @since 3.0
         * @param aAuthStatus authorization status to add
         */
        virtual void AddAuthStatusLX( CPEngAuthorizationResponse* aAuthStatus ) = 0;


        /**
         * Engine store size counter
         *
         * @since 3.0
         * @return size counter
         */
        virtual TInt& SizeCounter() = 0;


        /**
         * Lock for Network publishing
         *
         * @since 3.0
         */
        virtual void LockForNetworkPublishing() = 0;


        /**
         * Unlock from Network publishing
         *
         * @since 3.0
         */
        virtual void UnLockForNetworkPublishing() = 0;


    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Protected to prohibite deletion through interface.
         */
        virtual ~MPEngAuthorizationEngine() {};

    };
#endif      //  __MPENGAUTHORIZATIONENGINE_H__

//  End of File

