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
* Description:  Observer API for presence watcher events
*
*/


#ifndef MCAWATCHEROBSERVER_H
#define MCAWATCHEROBSERVER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MCAStoredContact;

// CLASS DECLARATION

/**
 *  Interface for presence watcher observers.
 *
 *  @lib CAEngine.lib
 *  @since 1.2
 */
class MCAWatcherObserver
    {
    public:

        /**
         * Gets called when watched friend's presence status changes. This event
         * is restricted to only network originated events, so if user refreshes
         * the friends-list by himself, then this event will not raise.
         * @param aContact Contact that was changed
         */
        virtual void HandleWatcherEvent( const MCAStoredContact* aContact ) = 0;

    protected:

        /**
         * Destrutor.
         */
        virtual ~MCAWatcherObserver() {};
    };

#endif      // MCAWATCHEROBSERVER_H

// End of File
