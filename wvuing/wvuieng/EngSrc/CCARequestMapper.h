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
* Description:  This is the storage of asynchronous imps requests and
*                background tasks.
*
*/


#ifndef CCAREQUESTMAPPER_H
#define CCAREQUESTMAPPER_H

// INCLUDES

#include "PublicEngineDefinitions.h"
#include "MCABackgroundInterface.h"
#include "MCABackgroundCallback.h"
#include "MCABackGroundTaskObserver.h"
#include <ImpsDetailed.h>

// FORWARD DECLARATIONS
class CCARequest;
class TBackgroundNotifier;
class CCABackgroundTask;

// CLASS DECLARATION

/**
*  Maps requests to responses. Used e.g. CreateGroupL.
*  Handles background tasks.
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCARequestMapper : public CBase
            , public MCABackgroundInterface
            , public MCABackgroundCallback
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCARequestMapper* NewL();

        /**
        * Destructor.
        */
        virtual ~CCARequestMapper();

    public: // From MCABackgroundInterface

        /**
        * @see MCABackgroundInterface
        */
        TInt RegisterBackgroundTask(
            MCABackgroundTask* aBackgroundTask,
            TCABackgroundTasks aTaskID,
            TInt aSubTasks = 1
        );
        /**
        * @see MCABackgroundInterface
        */
        TInt RegisterBackgroundObserver(
            MCABackgroundObserver* BackgroundObserver,
            TInt aTaskMask,
            TInt aEventMask
        );
        /**
        * @see MCABackgroundInterface
        */
        TInt BackgroundTaskStatus(
            TCABackgroundTasks aTaskID
        );

        /**
        * @see MCABackgroundInterface
        */
        TInt UnRegisterBackgroundObserver(
            MCABackgroundObserver* aBackgroundObserver
        );

    private: // From MCABackgroundCallback

        /**
         * @see MCABackgroundCallback::HandleBackgroundTask
         */
        TInt HandleBackgroundTask(
            MCABackgroundInterface::TCABackgroundTasks aTask );

    public: // New functions
        /**
        *
        * @param aState specifies the new network state.
        */
        void HandleNetworkStateChange( TNetworkState aState );

        /**
        * Creates request object and starts waiting if wanted.
        * CCARequestMapper owns the instance of created request, so remember to
        * call RemoveRequest/RemoveRequestAndLeaveIfErrorL when request is no
        * longer needed!!!
        * @since 1.2
        * @param aOpId Operation Id of request made
        * @param aWait Start asynchronous waiting
        * @param aDestroyAfterComplete Can the request be destroyed after it
        * has completed
        * @param aCallBack Callback function to be executed after request is
        *                  completed
        * @return Pointer to request object
        */
        CCARequest* CreateRequestL( TInt aOpId,
                                    TBool aWait = ETrue,
                                    TBool aDestroyAfterComplete = EFalse,
                                    TCallBack aCallBack = NULL );

        /**
        * Finds request according to OpId
        * @since 1.2
        * @param aOpId Operation Id of request
        * @return Pointer to request object
        */
        const CCARequest* FindRequest( TInt aOpId );

        /**
        * Removes request from queue
        * @since 1.2
        * @param aRequest Request to be removed
        */
        void RemoveRequest( CCARequest* aRequest );

        /**
        * Removes request from queue, and leaves if request's error code
        * differs from KErrNone
        * @since 1.2
        * @param aRequest Request to be removed
        */
        void RemoveRequestAndLeaveIfErrorL( CCARequest* aRequest );

        /**
        * Handles responses to requests, so remember to call this from your
        * code if you want request to be processed!!!
        * @since 1.2
        * @param aOpId Operation Id of response
        * @param aErrorCode Response's error-code
        */
        void HandleRequest( TInt aOpId, TInt aErrorCode );

        /**
        * Cancels all pending requests
        */
        void CancelAllRequests();

        /**
        * Number of waiting requests
        * @return Number of waiting requests
        */
        TInt WaitCount();

        /**
        * Cancels all pending requests
        */
        void ReportWaitStart();

        /**
        * Cancels all pending requests
        */
        void ReportWaitStop();

        /**
        * Register to receive notifications related to
        * start and completetion of background task
        * @param MCABackGroundTaskObserver- object
        */
        void RegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver );

        /**
         * UnRegister from receiving notifications related to
         * start and completetion of backgroundtask
         */
        void UnRegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver );

        /**
         * To know whether Background task has started
         * @return TBool
         */
        TBool IsBackgroundTaskPending();

        /**
         * Notify about the state
         * start and completetion of backgroundtask
         */
        void NotifyBackgroundTaskObservers( );

    private:

        /**
        * C++ default constructor.
        */
        CCARequestMapper();

        /**
         * Finds request for given operation id
         * @param aOpId is operation id
         */
        CCARequest* DoFindRequest( TInt aOpId );

        /**
         * Notifies background observers
         * @param aTaskPosition index of the task in iBackgroundTasks
         * @param aStatus new status to publis to observers
         * @param aSubTaksNumber numer of the current subtask
         * @param aLeaveCode leave code if the subtask if is there any,
         *                   KErrNone otherwise
         */
        void NotifyBackgroundObservers(
            TInt aTaskPosition,
            TCABackgroundStatus aStatus,
            TInt aSubTaskNumber,
            TInt aLeaveCode = 0
        );

        /**
        * Registers background task
        * @param aBackgroundTask pointer of instance
        * @param aTaskID is id of task
        * @param aSubTasks is number of subtask, positive
        * @return KErrNone on success, system wide error code otherwise
        */
        TInt RegisterBackgroundTaskL(
            MCABackgroundTask* aBackgroundTask,
            TCABackgroundTasks aTaskID,
            TInt aSubTasks = 1
        );


    private:    // Data
        RPointerArray<CCARequest> iRequestArray;
        TInt iWaitCount;

        //do own
        TBool iBackgroundProcessing;

        RArray<TBackgroundNotifier> iBackgroundObservers;
        RPointerArray<CCABackgroundTask> iBackgroundTasks;

        // for background task
        RPointerArray<MCABackGroundTaskObserver> iBackGroundTaskObserver;

    };

/**
*  Encapsulates information about a background observer
*  Internal, used by CCARequestMapper
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class TBackgroundNotifier
    {
    public: // New functions

        /**
        * Constructor
        * @param aBackgroundObserver pointer to background observer
        * @param aObserverTaskMask task mask ( bitmap )
        * @param aObserverEventMask event mask ( bitmap )
        */
        TBackgroundNotifier( MCABackgroundObserver* aBackgroundObserver
                             , TInt aObserverTaskMask = 0
                                                        , TInt aObserverEventMask = 0
                           );
        /**
        * Equality relation on TBackgroundNotifier
        * this is required for RArray<TBackgroundNotifier>::Find()
        *
        * @param aA for comapre
        * @param aB for compare
        * @retrun ETrue if iObservers are the same, EFalse otherwise
        */

        static TBool Identity( const TBackgroundNotifier& aA
                               , const TBackgroundNotifier& aB
                             );

    public:
        //does not own
        MCABackgroundObserver* iObserver;
        //for binary and operation,
        // see MCABackgroundInterface::TCABackgroundTasks
        TInt iTaskMask;
        //for binary and operation,
        // see MCABackgroundInterface::TCABackgroundStatus
        TInt iEventMask;
    };

#endif      // CCAREQUESTMAPPER_H

// End of File
