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
* Description:  Interface for group pc observer for chat/message error events
*
*/


#ifndef MCAGROUPOBSERVERPC_H
#define MCAGROUPOBSERVERPC_H

//INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
 *  Group observer interface for chat and message error events to the UI.
 *  Signalling goes from the Engine -> UI.
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAGroupObserverPC
    {
    public:

        /**
         * Handle events.
         * @param aWvid - WVID of the sender
         * @param aContactId - Sender identification
         */
        virtual void HandleChatEvent( const TDesC& aWvid = KNullDesC,
                                      const TDesC& aContactId = KNullDesC  ) = 0;


        /**
         * Handle events.
         * @param aNick, Identification of Recipient
         * @param aError, Message Error Code
         */
        virtual void HandleMessageError( TInt aError,
                                         const TDesC& aNick = KNullDesC ) = 0;

    protected:
        /**
         * Destructor.
         */
        virtual ~MCAGroupObserverPC() {};
    };

#endif      // MCAGROUPCHATOBSERVERPC_H

// End of File
