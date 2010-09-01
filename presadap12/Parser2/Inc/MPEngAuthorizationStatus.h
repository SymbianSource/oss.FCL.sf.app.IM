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
* Description:  Reactive authorization status interface.
*
*/

#ifndef __MPENGAUTHORIZATIONSTATUS_H
#define __MPENGAUTHORIZATIONSTATUS_H


//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS
class MPEngAuthorizationStatusExt;



// CLASS DECLARATION
/**
 * Interface for reactive authorization status.
 *
 * @since 3.0
 */
class MPEngAuthorizationStatus
    {
    public: // Authorization statuses

        /**
         * Reactive authorization statuses.
         *
         * @since 3.0
         */
        enum TPEngAuthorizationStatusType
            {
            //Authorization status is undefined
            EPEngAuthUndefined      = 0x00000,

            //Authorization was accepted
            EPEngAuthAccepted       = 0x00001,

            //Authorization was denied
            EPEngAuthDenied         = 0x00002,
            };


    public: // Access to authorization status details

        /**
         * Identification of user.
         *
         * Gets identification of user.
         *
         * @since 3.0
         * @return Id of the requesting user.
         */
        virtual const TDesC& UserId() const = 0;


        /**
         * Status of the authorization.
         *
         * Gets the status of authorization.
         *
         * @since 3.0
         * @return Status of of the authorization.
         */
        virtual TInt AuthorizationStatus() const = 0;


        /**
         * List of authorized attributes
         *
         * Gets the list of authorized attributes.
         *
         * @since 3.0
         * @return List of authorized attributes.
         */
        virtual const RArray<TUint32>& AuthorizedAttributes() const = 0;



    private: //Extensions interfaces

        /**
         * Reactive authorization status extension interface.
         *
         * @since 3.0
         * @return Reactive authorization status extension.
         */
        virtual MPEngAuthorizationStatusExt* Extension() = 0;
        virtual const MPEngAuthorizationStatusExt* Extension() const = 0;


    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Protected destructor to prohibits deletion
         * through the interface.
         */
        virtual ~MPEngAuthorizationStatus() {};

    };
#endif      //  __MPENGAUTHORIZATIONSTATUS_H


//  End of File

