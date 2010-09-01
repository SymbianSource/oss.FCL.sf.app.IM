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
* Description:  Flow controller for messages
*
*/



#ifndef MCAMESSAGEFLOWHANDLERPC_H
#define MCAMESSAGEFLOWHANDLERPC_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class MCAMessageContainer;
class MCAConversationManagerPC;
class MCARecordedChatsPC;

// CLASS DECLARATION

/**
*  Flow controller for messages
*
*  @lib wvuiprocessng.lib
*  @since Series 60 3.2
*/
class MCAMessageFlowHandlerPC
    {

    public:

        /**
         * Should we fetch new messages or not
         * @param aFetch ETrue if this chat is in foreground and should
         *               receive messages, EFalse otherwise.
         */
        virtual void FetchMessages( TBool aFetch ) = 0;


        /**
         * Destructor.
         */
        virtual ~MCAMessageFlowHandlerPC() {};

    };

#endif      // MCAMESSAGEFLOWHANDLERPC_H

// End of File
