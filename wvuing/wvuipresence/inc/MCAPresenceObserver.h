/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer API for presence events
*
*/


#ifndef MCAPRESENCEOBSERVER_H
#define MCAPRESENCEOBSERVER_H

// INCLUDES
#include <e32std.h>
#include "MCAPresence.h"

// CLASS DECLARATION

/**
*  Class that wishes to receive events about changed presence flags
*  in friends-list.
*
*
*  @lib CAPresence.lib
*  @since 1.2
*/
class MCAPresenceObserver
    {

    public:

        /**
        * Gets called when presence state of a friend/friends
        * from friends-list changes. When client receives multiple updates
        * at once, this will still get called only once.
        */
        virtual void HandlePresenceChange() = 0;

        /**
        * Gets called when presence subscription failes. This is used when subscription
        * request has originated from somewhere else than UI itself.
        * @param aWVId UserID of failed subscription.
        */
        virtual void HandleSubscriptionError(
            const TDesC& aWVId,
            const TInt   aErrorCode ) = 0;

        /**
        * Gets called when own presence status changes
        */
        virtual void HandleOwnStatusChange( MCAPresence::TPresenceStatus aStatus ) = 0;


    protected:

        /**
        * Destructor.
        */
        virtual ~MCAPresenceObserver() {};
    };

#endif      // MCAPRESENCEOBSERVER_H

// End of File
