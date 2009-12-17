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
* Description:  Observer interface to handle error notifications
*
*/


#ifndef __MCAMESSAGEERROROBSERVER_H__
#define __MCAMESSAGEERROROBSERVER_H__

//  INCLUDES
#include <e32std.h>

//	FORWARD CLASS DECLERATIONS
class MCAMessage;

// CLASS DECLARATION

/**
 *  Observer interface to handle error notifications
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessageErrorObserver
    {
    public: // Interface

        /**
         * Handle events.
         * @param aMessage, Event occurred because of message
         */
        virtual void HandleMessageError( TInt aError, MCAMessage* aMessage ) = 0;

    protected: // For protection.

        /**
         * Destructor
         */
        virtual ~MCAMessageErrorObserver() {}
    };

#endif      // __MCAMESSAGEERROROBSERVER_H__

// End of File
