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
* Description:  Reactive authorization responds interface.
*
*/

#ifndef __MPENGAUTHORIZATIONRESPOND_H
#define __MPENGAUTHORIZATIONRESPOND_H

//  INCLUDES
#include <E32Std.h>

// FORWARD DECLARATIONS
class MPEngAuthorizationRespondExt;
class MPEngAuthorizationRespondAdvanced;


// CLASS DECLARATION
/**
 * Interface for reactive authorization responds.
 *
 * @since 3.0
 */
class MPEngAuthorizationRespond
    {
    private: //Extensions interfaces

        /**
         * Authorization responds extension interface.
         *
         * @since 3.0
         * @return authorization responds extension.
         */
        virtual MPEngAuthorizationRespondExt* Extension() = 0;
        virtual const MPEngAuthorizationRespondExt* Extension() const = 0;



    public: //Presence Engine internal interface access

        /**
         * Presence Engine internal reactive authorization
         * responds interface
         *
         * @since 3.0
         * @return Authorization responds internal interface.
         */
        virtual const MPEngAuthorizationRespondAdvanced* Advanced() const = 0;
        virtual MPEngAuthorizationRespondAdvanced* Advanced() = 0;



    public: //Cleanup support & destructor

        /**
         * Support for authorization responds cleanup.
         *
         * When this method is called, concrete authorization
         * respond object is deleted and all its runtime
         * resources are freed.
         * ==> Authorization respond can be pushed on the
         * CleanupStack using CleanupClosePushL().
         *
         * @since 3.0
         */
        virtual void Close() = 0;


        /**
         * Inline virtual destructor. Concrete authorization
         * respond object can be deleted through
         * this interface.
         *
         * @since 3.0
         */
        virtual ~MPEngAuthorizationRespond() {};

    };
#endif      //  __MPENGAUTHORIZATIONRESPOND_H

//  End of File

