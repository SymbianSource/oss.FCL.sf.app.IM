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
* Description:  Observer interface to handle message container changes.
*
*/


#ifndef __MCAMESSAGEOBSERVER_H__
#define __MCAMESSAGEOBSERVER_H__

//  INCLUDES
#include <e32def.h>

//	FORWARD CLASS DECLERATIONS

// CLASS DECLARATION

/**
 *  Observer interface to handle message container changes.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessageObserver
    {

    public: // Definitions

        /**
         * Events for observer to handle.
         */
        enum TMessageEventType
            {
            ENewMessage,
            EMessageChanged,
            EMessageDeleted,
            EChatDeleted
            };

    public: // Interface

        /**
         * Handle events.
         * @param aEvent Event to handle.
         * @param aIndex, Index of message which is affected by event.
         */
        virtual void HandleMessageEvent( TMessageEventType aEvent, TInt aIndex = -1 ) = 0;

    protected: // For protection.

        /**
         * Destructor for protection
         */
        virtual ~MCAMessageObserver() {}
    };

#endif      // __MCAMESSAGEOBSERVER_H__

// End of File
