/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for Chat observer
*
*/


#ifndef MCALOGINREFRESHOBSERVERPC_H
#define MCALOGINREFRESHOBSERVERPC_H


//INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**

 *  CCAConversationContainer implemented
 *
 *  @lib wvprocessng.lib
 *  @since 3.2
 */
class MCALoginRefreshObserverPC
    {
    public:

        /**
         * Gets called when invite event occurs.
         * @param aEvent Type of event.
         * @param aUserId userid of the sender
         * @param aMessage invite message
         */
        virtual void HandleLoginRefreshCompleteL() = 0;


    public:
        /**
         * Destructor.
         */
        virtual ~MCALoginRefreshObserverPC() {};
    };

#endif      // MCALOGINREFRESHOBSERVERPC_H

// End of File
