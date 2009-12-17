/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __MCACHATOBSERVERPC_H__
#define __MCACHATOBSERVERPC_H__

//  INCLUDES
#include <e32def.h>
#include "TEnumsPC.h"

// CLASS DECLARATION

/**
 *  Observer interface to handle messages container changes.
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */
class MCAChatObserverPC
    {
    public: // Interface
        /**
         * Handle chat events. - MCAConversationObserverPC
         * @param aWvid - WVID
         * @param aContactId - Contact identification
         * @param aIndex - The index of the conversation in the local store of PC
         * @param aChatInList - if new chat then ETrue
         * @param aMessage - The message text
         */
        virtual void HandleOpenChatsEvent( const TEnumsPC::TChange aEvent,
                                           const TEnumsPC::TItem aTabItemType,
                                           const TDesC& aWvid,
                                           const TDesC& aIdentification,
                                           const TInt aIndex ) = 0;


        /**
         * Destructor
         */
        virtual ~MCAChatObserverPC() {}
    };

#endif      // __MCACHATOBSERVERPC_H__

// End of File
