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
* Description:  CIdle based background services
*
*/


#ifndef MCABACKGROUNDINTERFACE_H
#define MCABACKGROUNDINTERFACE_H

// INCLUDES
#include <badesca.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class MCABackgroundObserver;
class MCABackgroundTask;

// CLASS DECLARATION

/**
*	CIdle based background services.  .
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCABackgroundInterface
    {
    public:
        enum TCABackgroundTasks
            {
            EGroupFetch = 1,
            EPresence = 2,
            EWhisperSync = 4,
            //used just in 2.1
            EBlockGrantListFetch = 8
            };

        enum TCABackgroundStatus
            {
            //add or removed
            EUnknown = 0
            //enqueued and CIdle has been started
            , EIdleStarted = 1
            //CIdle is working
            , EStarting = 2
            //CActiveSchedulerWait has been started
            , EWaiting = 4
            //task or subtask has been completed
            , ECompleted = 8
            //task or subtask has been cancelled due to network loss or exit
            , ECancelled = 16
            //task or subtask has been failed and left
            , EFailed = 32
            };

    public:
        /**
        * Registers background task
        * @param aBackgroundTask pointer of instance
        * @param aTaskID is id of task
        * @param aSubTasks is number of subtask, positive
        * @return KErrNone on success, system wide error code otherwise
        */
        virtual TInt RegisterBackgroundTask(
            MCABackgroundTask* aBackgroundTask,
            TCABackgroundTasks aTaskID,
            TInt aSubTasks = 1
        ) = 0;
        /**
        * Registers observer for background task
        * @param aBackgaBackgroundObserver pointer of instance
        * @param aTaskMask is an AND mask for task requested,
        * @param aEventMask is an AND mask for events requested,
        * @return KErrNone on success, system wide error code otherwise
        */
        virtual TInt RegisterBackgroundObserver(
            MCABackgroundObserver* aBackgroundObserver,
            TInt aTaskMask,
            TInt aEventMask
        ) = 0;
        /**
        * Removes observer from the queue
        * @param aBackgaBackgroundObserver pointer of instance to remove
        */
        virtual TInt UnRegisterBackgroundObserver(
            MCABackgroundObserver* aBackgroundObserver
        ) = 0;

        /**
        * @param aTaskID is an id of a background task
        * @retrun status of the background task or KErrNotFound
        */
        virtual TInt BackgroundTaskStatus(
            TCABackgroundTasks aTaskID
        ) = 0;
    protected:

        /**
        * Destructor.
        */
        virtual ~MCABackgroundInterface() {};

    };

#endif      // MCABACKGROUNDINTERFACE_H

// End of File
