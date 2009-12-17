/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef MCABACKGROUNDOBSERVER_H
#define MCABACKGROUNDOBSERVER_H

// INCLUDES
#include "MCABackgroundInterface.h"
#include <e32std.h>

// CLASS DECLARATION

/**
*  Callback for background task status change notification.
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCABackgroundObserver
    {

    public:

        /**
        * Call if an event happend that match registered task and event mask.
        * @param aEventSource is the source of event
        * @param aStatus is the status of the background task
        * @param aSubTaskNumber is the number of subtask
        * @param aLeaveCode is the leave code of the task/subtask
        */
        virtual void HandleBackgroundEvent(
            MCABackgroundInterface::TCABackgroundTasks aEventSource,
            MCABackgroundInterface::TCABackgroundStatus aStatus,
            TInt aSubTaskNumber,
            TInt aLeaveCode
        ) = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCABackgroundObserver() {};
    };

#endif      // MCABACKGROUNDOBSERVER_H

// End of File
