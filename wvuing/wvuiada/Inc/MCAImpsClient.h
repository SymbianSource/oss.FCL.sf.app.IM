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
* Description:  Abstract interface for RImpsClient.
*
*/



#ifndef MCAIMPSCLIENT_H
#define MCAIMPSCLIENT_H

//  INCLUDES
#include <ImpsClient.h>

// CLASS DECLARATION

/**
*  Abstract base class.
*  Same interface as RImpsClient
*
*  @lib CAAdapter.lib
*  @since 1.2
*/
class MCAImpsClient
    {
    public: // New functions

        // INITILIZATION AND CONNECT

        /**
        * Registers the service status change observer.
        * Ignored if API has already registered an observer
        * This can be called after the actual registeration of the
        * particular client has been made (RegisterL).
        * @since 3.0
        * @param aObs status observer implementation. Ownership NOT transferred.
        */
        virtual void RegisterStatusObserverL( MImpsStatusHandler2* aObs ) = 0;

        /**
        * Unregisters the service status change observer.
        * Ignored if API does not have registered observer.
        * This method is not necessary if unregister of the particular
        * client type has been executed (Unregister).
        * @since 1.2
        */
        virtual void UnregisterStatusObserverL( ) = 0;

        /**
        * Available services accessor
        * Notice that there are no services in NOT_LOGGED state, i.e.
        * before login has been executed successfully from any client.
        * @since 1.2
        * @param aServices Service tree having
        * all supported features and functions.
        */
        virtual void GetServicesL( TImpsServices& aServices ) = 0;

        /**
        * Register an advanced error observer.
        * If this is not registered then normal observers are called in
        * derived client interfaces, but they do not provide other
        * information then status code. This detailed error
        * message observer provides also textual descriton and
        * detailed error list in case of partial success.
        * @since 3.0
        * @param aObs error event observer implementation. Ownership NOT transferred.
        */
        virtual void RegisterErrorObserverL( MImpsErrorHandler2& aObs ) = 0;

        /**
        * Unregisters an advanced error observer.
        * Ignored if API does not have error observer.
        * This method is not necessary if unregister of the particular
        * client type has been executed (Unregister).
        * @since 1.2
        */
        virtual void UnregisterErrorObserverL( ) = 0;


        /**
        * Internal use only.
        * Error observer accessor
        * @since 1.2
        * @return error observer pointer. Ownership NOT transferred.
        */
        virtual MImpsErrorHandler2* ErrorHandler() const = 0;

        /**
        * Internal use only.
        * Status observer accessor. This is missing from WV Engine. Introduced for consistency.
        * @since 1.2
        * @return Status observer pointer. Ownership NOT transferred.
        */
        virtual MImpsStatusHandler2* StatusHandler() const = 0;

    protected:  // New functions

        /**
        * Destructor.
        */
        virtual ~MCAImpsClient() { };
    };

#endif      // MCAIMPSCLIENT_H

// End of File
