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
* Description:  Observer interface to receive presence change notifications.
*
*/

#ifndef __MPENGAUTHORIZATIONOBSERVER_H
#define __MPENGAUTHORIZATIONOBSERVER_H

//  INCLUDES
#include <E32Std.h>


//FORWARD DECLARATIONS
class MPEngAuthorizationRequest;
class MPEngAuthorizationStatus;



// CLASS DECLARATION

/**
 * Observer interface to receive Authorization notifications.
 *
 * @since 3.0
 */
class MPEngAuthorizationObserver
    {

    public: //Observer methods

        /**
         *  Authorization handler
         *
         *  Called when new reactive authorization request was
         *  received from the Server
         *  State of the such a authorization requeste is EPEngAuthPending
         *  If this method leaves, error code is
         *  reported back to HandlePresenceError().
         *
         *  @since 3.0
         *  @param aPendingAuthorization pending authorization
         *  @param aNotifier The notifier source where notification
         *                   is coming from. No ownership transferred.
         */
        virtual void HandleNewPendingAuthorization(
            MPEngAuthorizationRequest& aNewPendingAuthorization ) = 0;



        virtual void HandlerNewAuthorizationStatus(
            const MPEngAuthorizationStatus& aNewAuthorizationStatus ) = 0;

        /**
         *  Handle Authorization engine update
         *
         *  Called when some athorization was updated.
         *
         *  @since 3.0
         *  @param aNotifier The notifier source where notification
         *                   is coming from. No ownership transferred.
         */
        virtual void HandleAuthorizationEngineUpdate() = 0;


        /**
         * Notification failure handler.
         *
         * @since 3.0
         *
         * @param aError The error resulting from Presence Engine internal
         *        presence change event handling (e.g. out of memory) or
         *         leave error from HandlePresenceChangeL().
         * @param aNotifier The notifier of which event handling failed.
         *        No ownership transferred.
         */
        virtual void HandleAuthorizationEngineError(
            TInt aError ) = 0;

    protected:  //Destructor

        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngAuthorizationObserver() { }


    };

#endif //__MPENGAUTHORIZATIONOBSERVER_H


// End of File


