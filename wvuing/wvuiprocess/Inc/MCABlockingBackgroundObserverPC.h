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
* Description:  Callback for background task status change notification
*
*/


#ifndef MCABLOCKINGBACKGROUNDOBSERVERPC_H
#define MCABLOCKINGBACKGROUNDOBSERVERPC_H

// INCLUDES
#include "MCABlockingPC.h"
#include "TEnumsPC.h"
#include <e32std.h>

// CLASS DECLARATION

/**
*  Callback for background task status change notification from UI
*
*  @lib wvuiprocessng.lib
*  @since 3.2
*/
class MCABlockingBackgroundObserverPC
    {

    public:

        /**
        * Call if an event happend that match registered task and event mask.
        * @param aEventSource is the source of event
        * @param aStatus is the status of the background task
        * @param aSubTaskNumber is the number of subtask
        * @param aLeaveCode is the leave code of the task/subtask
        */
        virtual void HandleBackgroundEventPC(
            TEnumsPC::TCABackgroundTasks aEventSource,
            TEnumsPC::TCABackgroundStatus aStatus,
            TInt aSubTaskNumber,
            TInt aLeaveCode
        ) = 0;

    public:

        /**
        * Destructor.
        */
        virtual ~MCABlockingBackgroundObserverPC() {};
    };

#endif      // MCABLOCKINGBACKGROUNDOBSERVERPC_H

// End of File
