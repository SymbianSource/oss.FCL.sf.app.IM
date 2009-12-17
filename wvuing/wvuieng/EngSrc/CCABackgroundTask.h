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
* Description:  This is encapsulation of a background task,
*                part of CCARequestMapper
*
*/


#ifndef CCABACKGROUNDTASK_H
#define CCABACKGROUNDTASK_H

// INCLUDES
#include "MCABackgroundInterface.h"

#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MCABackgroundCallback;

/**
*  Encapsulates a background task. This is a part of CCARequestMapper.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCABackgroundTask : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Static constructor.
        * @param aParent instance to call back with CIdle
        * @param aBackgroundTask background task to encapsulate
        * @param aTask id of the background task
        * @param aSubTasks number of iterations - greater than zero
        *	it leaves with KErrArgument if aSubTasks <= 0
        * @see MCABackgroundInterface::TCABackgroundTasks
        */
        static CCABackgroundTask* NewL(
            MCABackgroundCallback* aParent
            , MCABackgroundTask* aBackgroundTask
            , MCABackgroundInterface::TCABackgroundTasks aTask
            , TInt aSubTasks
        );
        /**
        * Static constructor.
        * @param aParent instance to call back with CIdle
        * @param aBackgroundTask background task to encapsulate
        * @param aTask id of the background task
        * @param aSubTasks number of iterations - greater than zero
        *	it leaves with KErrArgument if aSubTasks <= 0
        * @see MCABackgroundInterface::TCABackgroundTasks
        */
        static CCABackgroundTask* NewLC(
            MCABackgroundCallback* aParent
            , MCABackgroundTask* aBackgroundTask
            , MCABackgroundInterface::TCABackgroundTasks aTask
            , TInt aSubTasks
        );
        /**
        * Destructor.
        */
        virtual ~CCABackgroundTask();

    public: // New functions

        /**
        * Starts background execution ( CIdle ).
        */
        void Start();

        /**
        * Cancels background execution ( CIdle ).
        */
        void Cancel();

        /**
        * Identity relation for RArray Find(...)
        * @param aA for comparison
        * @param aB for comparison
        * @return ETrue if task ids are the same, EFalse otherwise
        */
        static TBool Identity( const CCABackgroundTask& aA
                               , const CCABackgroundTask& aB
                             );
        /**
        * @return the number of subtask/iteration left
        */
        TInt SubTasks();

        /**
        * @return the last execution state
        * @see MCABackgroundInterface::TCABackgroundStatus
        */
        MCABackgroundInterface::TCABackgroundStatus TaskState();

        /**
        * Sets State of this background task
        * @param aTaskState new state to set
        * @see MCABackgroundInterface::TCABackgroundStatus
        */
        void SetTaskState(
            MCABackgroundInterface::TCABackgroundStatus aTaskState );

        /**
        * Is more iteration left.
        * It detracts number of iterations by 1
        * @return ETrue if there is any after detraction( SubTasks() >0 ),
        */
        TBool More();

        /**
        * @return id of the background task
        * @see MCABackgroundInterface::TCABackgroundTasks
        */
        MCABackgroundInterface::TCABackgroundTasks TaskId();

        /**
        * @return pointer to the background task
        * no ownership transfer
        */
        MCABackgroundTask* BackgroundTask();

        /**
        * Executes background task.
        */
        void ExecuteTaskL();

        /**
        * @return ETrue if this task is active.
        */
        TBool Pending();

    private:

        /**
        * C++ default constructor.
        * @param aParent instance to call back with CIdle
        * @param aBackgroundTask background task to encapsulate
        * @param aTask id if the background task
        * @param aSubTasks number of iterations - greater than zero
        *	it leaves with KErrArgument if aSubTasks <= 0
        * @see MCABackgroundInterface::TCABackgroundTasks
        */
        CCABackgroundTask(
            MCABackgroundCallback* aParent
            , MCABackgroundTask* aBackgroundTask
            , MCABackgroundInterface::TCABackgroundTasks aTask
            , TInt aSubTasks
        );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * CIdle-kind of function for background task implementation
        * @param aInstance Instance of this class
        * @return Need for recall
        */
        static TInt BackgroundTask( TAny *aInstance );

        /**
        * Called by BackgroundTask().
        * Its calling instace given as aParent with aTask on construction.
        */
        TInt DoBackgroundTask();

    private:    // Data
        // Doesn't own
        MCABackgroundCallback* iParent;
        MCABackgroundTask* iBackgroundTask;

        //do own
        MCABackgroundInterface::TCABackgroundTasks iTask;
        TInt iSubTasks;
        MCABackgroundInterface::TCABackgroundStatus iTaskState;
        //Owns
        CIdle* iIdle;


    };

#endif      // CCABACKGROUNDTASK_H

// End of File
