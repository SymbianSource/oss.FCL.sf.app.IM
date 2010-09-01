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


#ifndef MCALISTINITOBSERVER_H
#define MCALISTINITOBSERVER_H

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
class MCAListInitObserver
    {

    public:

        /**
        * Gets called when presence state of a friend/friends
        * from friends-list changes. When client receives multiple updates
        * at once, this will still get called only once.
        */
        virtual void HandleListInitChange( TBool aCompleted ) = 0;


    protected:

        /**
        * Destructor.
        */
        virtual ~MCAListInitObserver() {};
    };

#endif      // MCALISTINITOBSERVER_H

// End of File
