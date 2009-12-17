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
* Description:  Observer interface to handle chat container changes.
*
*/


#ifndef __MCACHATOBSERVER_H__
#define __MCACHATOBSERVER_H__

//  INCLUDES
#include <e32def.h>

//	FORWARD CLASS DECLERATIONS
class MCAMessage;

// CLASS DECLARATION

/**
 *  Observer interface to handle messages container changes.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAChatObserver
    {

    public: // Definitions

        /**
         * Events for observer to handle.
         */
        enum TChatEventType
            {
            EGroupListChanged,
            EChatListChanged,
            ESendListChanged,
            EUnreadCountChanged, // Happens with new message and when messages are read.
            EMemoryLow,
            EEventNotSpecified
            };

    public: // Interface

        /**
         * Handle events.
         * @param aEvent Event to handle.
         * @param aMessage, Event occurred because of message
         */
        virtual void HandleChatEvent( TChatEventType aEvent, MCAMessage* aMessage = NULL ) = 0;

    protected: // For protection.

        /**
         * Destructor
         */
        virtual ~MCAChatObserver() {}
    };

#endif      // __MCACHATOBSERVER_H__

// End of File
