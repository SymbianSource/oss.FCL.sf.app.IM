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
* Description:  Observer interface to be implemented by UI components
*				 for callbacks from the PC on background events
*
*/



#ifndef MCASEARCHBACKGROUNDOBSERVERPC_H
#define MCASEARCHBACKGROUNDOBSERVERPC_H


// INCLUDES
#include <e32std.h>
#include "TEnumsPC.h"



// CLASS DECLARATION
/**
 *  Interface for observer
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */

class MCASearchBackgroundObserverPC
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


        /**
         * Virtual destructor
         */
        virtual ~MCASearchBackgroundObserverPC() {};
    };

#endif      // MCASEARCHBACKGROUNDOBSERVERPC_H

// End of File
