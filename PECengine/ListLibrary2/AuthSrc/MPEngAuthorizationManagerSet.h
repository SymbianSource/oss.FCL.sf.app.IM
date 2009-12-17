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
* Description:  Authorization Manager Set abstract interface
*
*/

#ifndef __MPENGAUTHORIZATIONMANAGERSET_H__
#define __MPENGAUTHORIZATIONMANAGERSET_H__


//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class   CPEngAuthorizationResponse;


// CLASS DECLARATION
/**
 * Authorization Manager Set abstract interface
 */
class MPEngAuthorizationManagerSet
    {

    public: // New functions

        /**
         * Set authorization response
         *
         * @since 3.0
         * @param aUserid user Id
         * @param aAttributes attributes to be authorized
         * @param aKeepOldAuthorization already authorized attributes
         *        are kept authorized
         */
        virtual void SetAuthorizedAttributesL(
            const CPEngAuthorizationRequest& aAuthRequest,
            TArray<TUint32>& aAttributes,
            TBool aKeepOldAuthorization ) = 0;

        virtual void DenyAuthorizationL( const TDesC& aUserId ) = 0;

        virtual void CancelAuthorizationL( const TDesC& aUserId ) = 0;


        /**
         * Removes buffered authorization response
         * If this was last buffered authorization request
         * also lock for editing of authorization responses
         * is released.
         *
         * @since 3.0
         * @param aUserid user Id
         */
        virtual void RemoveAuthorizationResponse( const TDesC& aUserId ) = 0;


    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Protected to prohibite deletion through interface.
         */
        virtual ~MPEngAuthorizationManagerSet() {};

    };

#endif      //  __MPENGAUTHORIZATIONMANAGERSET_H__

//  End of File

