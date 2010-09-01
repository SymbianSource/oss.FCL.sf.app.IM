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


#ifndef __MCACONVERSATIONOBSERVERPC_H__
#define __MCACONVERSATIONOBSERVERPC_H__

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
class MCAConversationObserverPC
    {
    public: // Interface
        /**
         * Handle chat events. - MCAConversationObserverPC
         * @param aWvid - WVID
         * @param aContactId - Contact identification
         */
        virtual void HandleChatEvent( 	const TDesC& aWvid,
                                       const TDesC& aContactId ) = 0;

        /**
         * Handle events - MCAConversationObserverPC
         * @param aError
         * @param aInfo, Event occurred because of message
         * @param aMsgContentType - Content of the message
         */
        virtual void HandleMessageError( TInt aError,
                                         const TDesC& aInfo,
                                         TEnumsPC::TContentType aMsgContentType ) = 0;

        /**
         * Handle events - MCAConversationObserverPC
         * @param aContactId The contact which was affected (add/delete/change)
         * this function is called when there is any change w.r.t contacts
         *
         */
        virtual void HandleContactChange( const TDesC& aContactId ) = 0;


        /**
         * Destructor
         */
        virtual ~MCAConversationObserverPC() {}
    };

#endif      // __MCACONVERSATIONOBSERVERPC_H__

// End of File
