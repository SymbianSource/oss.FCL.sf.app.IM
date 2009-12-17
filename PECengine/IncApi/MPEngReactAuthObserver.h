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
* Description:  Observer interface to receive reactive authorization notifications.
*
*/

#ifndef __MPENGREACTAUTHOBSERVER_H
#define __MPENGREACTAUTHOBSERVER_H

//  INCLUDES
#include <E32Std.h>


//FORWARD DECLARATIONS
class CPEngReactAuthNotifier;
class MPEngAuthorizationRequest;
class MPEngAuthorizationStatus;


// CLASS DECLARATION

/**
 * Observer interface to receive reactive authorization events.
 * Observer is registered to CPEngReactAuthNotifier.
 *
 * With this API clients can
 * 1. Receive notifications from new pending
 *    reactive authorization requests.
 *
 * 2. Receive notifications from network server reactive
 *    authorization status responses.
 *
 * @since 3.0
 */
class MPEngReactAuthObserver
    {
    public: //Observer methods

        /**
         * Reactive authorization request handler.
         *
         * Called by CPEngReactAuthNotifier to handle new reactive
         * authorization requests received from the network server.
         * State of the authorization requests is EPEngAuthPending.
         *
         * If this method leaves, error code is
         * reported back to HandleReactAuthError().
         *
         * @since 3.0
         * @param aNotifier The notifier source where notification
         *        is coming from. No ownership transferred.
         * @param aPendingAuthReqs The pending authorization requests.
         */
        virtual void HandlePendingAuthorizationReqL(
            CPEngReactAuthNotifier& aNotifier,
            TArray< MPEngAuthorizationRequest* >& aPendingAuthReqs ) = 0;


        /**
         * Reactive authorization status handler.
         *
         * Called by CPEngReactAuthNotifier to handle
         * reactive authorization status notifications
         * received from the network server.
         *
         * If this method leaves, error code is
         * reported back to HandleReactAuthError().
         *
         * @since 3.0
         * @param aNotifier The notifier source where notification
         *        is coming from. No ownership transferred.
         * @param aAuthStatuses The new authorization statuses.
         */
        virtual void HandlerAuthorizationStatusL(
            CPEngReactAuthNotifier& aNotifier,
            TArray< const MPEngAuthorizationStatus* > aAuthStatuses ) = 0;



        /**
         * Notification failure handler.
         *
         * @since 3.0
         *
         * @param aError The error from Presence Engine internal
         *        reactive authorization event handling
         *        (e.g. out of memory) or leave error from
         *        HandlePendingAuthorizationReqL() or
         *        HandlerAuthorizationStatusL().
         * @param aNotifier The notifier of which event handling failed.
         *        No ownership transferred.
         */
        virtual void HandleReactAuthError(
            TInt aError,
            CPEngReactAuthNotifier& aNotifier ) = 0;


    private: //Extensions

        /**
         * Reserved virtual table slots for future use.
         * Do not implement this.
         *
         * @since 3.0
         */
        virtual TInt ReactAuthObserverReserved() {
            return KErrNotSupported;
            }



    protected:  //Destructor

        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngReactAuthObserver() { }

    };



#endif  //__MPENGREACTAUTHOBSERVER_H


//End of file



