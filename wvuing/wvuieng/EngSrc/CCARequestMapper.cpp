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
* Description:  This is the storage of asynchronous imps requests.
*
*/

// INCLUDE FILES
#include "CCARequestMapper.h"			// Own header
#include "CCARequest.h"
#include "ChatDebugPrint.h"
#include "PrivateEngineDefinitions.h"

#include "MCABackgroundTask.h"
#include "MCABackgroundObserver.h"
#include "CCABackgroundTask.h"

#include <e32base.h>
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCARequestMapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCARequestMapper* CCARequestMapper::NewL()
    {
    CCARequestMapper* self = new( ELeave ) CCARequestMapper;

    return self;
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::~CCARequestMapper
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCARequestMapper::~CCARequestMapper()
    {
    CHAT_DP( D_CHAT_LIT( "RequestMapper destroying (%d requests pending)." ),
             iRequestArray.Count() );

    iRequestArray.ResetAndDestroy();
    iRequestArray.Close();

    iBackgroundTasks.ResetAndDestroy();
    iBackgroundTasks.Close();
    iBackgroundObservers.Reset();
    iBackgroundObservers.Close();
    iBackGroundTaskObserver.Reset();
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::RegisterBackgroundTask
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARequestMapper::RegisterBackgroundTask(
    MCABackgroundTask* aBackgroundTask
    , TCABackgroundTasks aTaskID
    , TInt aSubTasks
)
    {
    if ( !aBackgroundTask )
        {
        return KErrArgument;
        }

    TInt err( KErrNone );
    TRAPD( leave, err = RegisterBackgroundTaskL(
                            aBackgroundTask
                            , aTaskID
                            , aSubTasks
                        ) );
    switch ( leave )
        {
        case KErrNone:
            {
            return err;
            }
        case KErrNoMemory: // flowtrough
        case KErrDiskFull:
            {
            CActiveScheduler::Current()->Error( leave );
            return leave;
            }
        default:
            {
            return leave;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::RegisterBackgroundObserver
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARequestMapper::RegisterBackgroundObserver(
    MCABackgroundObserver* aBackgroundObserver
    , TInt aTaskMask
    , TInt aEventMask
)
    {
    CHAT_DP( D_CHAT_LIT(
                 "RequestMapper::RegisterBackgroundObserverL new observer %d %d" ),
             aTaskMask, aEventMask );

    TIdentityRelation<TBackgroundNotifier>
    identity( TBackgroundNotifier::Identity );


    TInt position ( iBackgroundObservers.Find( TBackgroundNotifier(
                                                   aBackgroundObserver ), identity ) );

    TInt err( KErrNone );
    if ( position == KErrNotFound )
        {
        err = iBackgroundObservers.Append( TBackgroundNotifier(
                                               aBackgroundObserver
                                               , aTaskMask
                                               , aEventMask
                                           ) );
        }
    else
        {
        iBackgroundObservers[ position ].iTaskMask = aTaskMask;

        iBackgroundObservers[ position ].iEventMask = aEventMask;

        }

    return err;
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::BackgroundTaskStatus
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARequestMapper::BackgroundTaskStatus(
    TCABackgroundTasks aTaskID
)
    {
    CHAT_DP( D_CHAT_LIT( "CCARequestMapper::BackgroundTaskStatus() task %d" ),
             aTaskID );

    const TInt limit( iBackgroundTasks.Count() );
    for ( TInt i( 0 ); i < limit; i++ )
        {
        CCABackgroundTask* task = iBackgroundTasks[ i ];

        if ( task->TaskId() == aTaskID )
            {
            CHAT_DP( D_CHAT_LIT( "CCARequestMapper::BackgroundTaskStatus() task \
                                 %d status:%d" )
                     , aTaskID, task->TaskState() );
            return task->TaskState();
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::UnRegisterBackgroundObserver
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARequestMapper::UnRegisterBackgroundObserver(
    MCABackgroundObserver* aBackgroundObserver
)
    {
    CHAT_DP_TXT( "RequestMapper::UnRegisterBackgroundObserver" );

    TIdentityRelation<TBackgroundNotifier>
    identity( TBackgroundNotifier::Identity );

    TInt position ( iBackgroundObservers.Find( TBackgroundNotifier(
                                                   aBackgroundObserver )
                                               , identity ) );

    if ( position == KErrNotFound )
        {
        return position;
        }
    else
        {
        iBackgroundObservers.Remove( position );
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::HandleBackgroundTask
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARequestMapper::HandleBackgroundTask(
    MCABackgroundInterface::TCABackgroundTasks aTask )
    {

    TInt position( KErrNotFound );

    TInt limit( iBackgroundTasks.Count() );
    TInt index( 0 );

    while ( index < limit && position == KErrNotFound )
        {
        if ( iBackgroundTasks[ index ]->TaskId() == aTask )
            {
            position = index;
            }
        index++;
        }

    __ASSERT_ALWAYS( position != KErrNotFound , User::Panic(
                         KCAEnginePanicCategory, EBackgroundTaskNotFound ) );

    if ( iBackgroundTasks[ position ]->TaskState() &
         (
             MCABackgroundInterface::ECancelled |
             MCABackgroundInterface::EUnknown
         ) )
        {
        //task cancelled nothing to do

        return EFalse;
        }
    NotifyBackgroundObservers(
        position,
        EStarting,
        iBackgroundTasks[ position ]->SubTasks()
    );
    TRAPD( leave , iBackgroundTasks[ position ]->ExecuteTaskL() );
    CHAT_DP( D_CHAT_LIT( "CCARequestMapper::HandleBackgroundTask() task %d, \
	                      subtask %d left with %d" ),
             aTask, 0, leave );
    if ( leave )
        {
        NotifyBackgroundObservers(
            position,
            MCABackgroundInterface::EFailed,
            iBackgroundTasks[ position ]->SubTasks(),
            leave
        );
        }
    else
        {
        NotifyBackgroundObservers(
            position,
            MCABackgroundInterface::ECompleted,
            iBackgroundTasks[ position ]->SubTasks()
        );
        }

    NotifyBackgroundTaskObservers();

    return iBackgroundTasks[ position ]->More();
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::HandleNetworkStateChange
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARequestMapper::HandleNetworkStateChange( TNetworkState aState )
    {
    CHAT_DP( D_CHAT_LIT( "CCARequestMapper::HandleNetworkStateChange state %d" ),
             aState );
    if ( aState != ELoggedIn )
        {
        //Cancelling all penging request
        CancelAllRequests();

        //Cancelling all background task
        TInt limit( iBackgroundTasks.Count() );
        for ( TInt i( 0 ); i < limit; i++ )
            {
            CCABackgroundTask* task = iBackgroundTasks[ i ];
            task->Cancel();

            if ( task->TaskState() & ( MCABackgroundInterface::EIdleStarted
                                       | MCABackgroundInterface::EStarting
                                       | MCABackgroundInterface::EWaiting ) )
                {
                NotifyBackgroundObservers(
                    i
                    , MCABackgroundInterface::EUnknown
                    , task->SubTasks()
                );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::CreateRequestL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCARequest* CCARequestMapper::CreateRequestL( TInt aOpId,
                                              TBool aWait,
                                              TBool aDestroyAfterComplete,
                                              TCallBack aCallBack )
    {
    CHAT_DP( D_CHAT_LIT( "RequestMapper::CreateRequestL (OpId: %d, Wait: %d, \
	                      aDestroyAfterComplete: %d, aCallBack: %d)" ),
             aOpId, aWait, aDestroyAfterComplete, aCallBack.iFunction );

    CCARequest* request = CCARequest::NewL( aOpId,
                                            aDestroyAfterComplete,
                                            aCallBack );
    request->SetRequestMapper( this );

    TInt err( iRequestArray.Append( request ) );

    if ( err != KErrNone )
        {
        CHAT_DP( D_CHAT_LIT( "RequestMapper::CreateRequestL failed with \
		                      error code %d!!!" ), err );
        delete request;
        User::Leave( err );
        }

    if ( aWait )
        {
        CHAT_DP( D_CHAT_LIT( "Wait starting...request type is %d" ),
                 request->RequestType() );

        request->StartWait();
        }

    return request;
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::FindRequest
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CCARequest* CCARequestMapper::FindRequest( TInt aOpId )
    {
    return DoFindRequest( aOpId );
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::RemoveRequest
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARequestMapper::RemoveRequest( CCARequest* aRequest )
    {
    TInt index( iRequestArray.Find( aRequest ) );
    CHAT_DP( D_CHAT_LIT( "RequestMapper::RemoveRequest (Index: %d)" ), index );

    if ( index != KErrNotFound )
        {
        delete aRequest;
        iRequestArray.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::RemoveRequestAndLeaveIfErrorL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARequestMapper::RemoveRequestAndLeaveIfErrorL( CCARequest* aRequest )
    {
    if ( aRequest )
        {
        TInt errorCode( aRequest->ErrorCode() );
        RemoveRequest( aRequest );

        User::LeaveIfError( errorCode );
        }
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::HandleRequest
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARequestMapper::HandleRequest( TInt aOpId, TInt aErrorCode )
    {
    CHAT_DP( D_CHAT_LIT( "RequestMapper::HandleRequest (OpId: %d, Error: %d)" ),
             aOpId, aErrorCode );

    CCARequest* request = DoFindRequest( aOpId );

    //signal OOM
    if ( aErrorCode == KErrNoMemory || aErrorCode == KErrDiskFull )
        {
        CActiveScheduler::Current()->Error( aErrorCode );
        }

    // If we have the request...
    if ( request )
        {
        // Set error code
        request->SetErrorCode( aErrorCode );
        // Call callback function if one is defined
        request->ExecuteCallBackFunction();
        // Stop wait if needed
        request->StopWaitIfNeeded();
        // Destroy object if marked so
        if ( request->DestroyAfterComplete() )
            {
            RemoveRequest( request );
            }
        }
    CHAT_DP_TXT( "RequestMapper::HandleRequest over" );
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::CancelAllRequests
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARequestMapper::CancelAllRequests()
    {
    CHAT_DP( D_CHAT_LIT( "RequestMapper::CancelAllRequests (%d)" ),
             iRequestArray.Count() );

    CCARequest* request = NULL;

    for ( TInt i( 0 ); i < iRequestArray.Count(); ++i )
        {
        request = iRequestArray[i];
        request->SetErrorCode( EOperationCancelled );
        request->StopWaitIfNeeded();
        }

    }

// -----------------------------------------------------------------------------
// CCARequestMapper::WaitCount
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARequestMapper::WaitCount()
    {
    return iWaitCount;
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::ReportWaitStart
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARequestMapper::ReportWaitStart()
    {
    iWaitCount++;
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::ReportWaitStop
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARequestMapper::ReportWaitStop()
    {
    iWaitCount--;
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::CCARequestMapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCARequestMapper::CCARequestMapper()
    {
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::DoFindRequest
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCARequest* CCARequestMapper::DoFindRequest( TInt aOpId )
    {
    CHAT_DP_TXT( "CCARequestMapper::DoFindRequest" );
    TInt indexCount( iRequestArray.Count() );

    for ( TInt index( 0 ); index < indexCount; index ++ )
        {
        if ( iRequestArray[index]->IdMatches( aOpId ) )
            {
            CHAT_DP_TXT( "CCARequestMapper::DoFindRequest over +" );
            return iRequestArray[index];
            }
        }
    CHAT_DP_TXT( "CCARequestMapper::DoFindRequest over -" );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::NotifyBackgroundObservers
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARequestMapper::NotifyBackgroundObservers(
    TInt aTaskPosition,
    TCABackgroundStatus aStatus,
    TInt aSubTaskNumber,
    TInt aLeaveCode
)
    {
    CHAT_DP( D_CHAT_LIT( "CCARequestMapper::NotifyBackgroundObservers() \
	                      %d %d %d %d" ),
             aTaskPosition, aStatus, aSubTaskNumber, aLeaveCode );

    TInt limit( iBackgroundObservers.Count() );
    CCABackgroundTask* task = iBackgroundTasks[ aTaskPosition ];
    task->SetTaskState( aStatus );

    for ( TInt i( 0 ); i < limit; i++ )
        {
        if ( ( task->TaskId() & iBackgroundObservers[ i ].iTaskMask ) &&
             ( aStatus & iBackgroundObservers[ i ].iEventMask ) )
            {
            iBackgroundObservers[ i ].iObserver->HandleBackgroundEvent(
                task->TaskId()
                , aStatus
                , aSubTaskNumber
                , aLeaveCode
            );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCARequestMapper::RegisterBackgroundTaskL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARequestMapper::RegisterBackgroundTaskL(
    MCABackgroundTask* aBackgroundTask
    , TCABackgroundTasks aTaskID
    , TInt aSubTasks
)
    {

    CCABackgroundTask* newTask = CCABackgroundTask::NewLC(
                                     this
                                     , aBackgroundTask
                                     , aTaskID
                                     , aSubTasks
                                 );
    TIdentityRelation<CCABackgroundTask>
    identity( CCABackgroundTask::Identity );

    TInt position ( iBackgroundTasks.Find( newTask, identity ) );

    if ( position != KErrNotFound )
        {
        CCABackgroundTask* toRemove = iBackgroundTasks[ position ];
        iBackgroundTasks.Remove( position );
        delete toRemove;
        }

    TInt err( KErrNone );
    err = iBackgroundTasks.Append( newTask );
    if ( err )
        {
        CleanupStack::PopAndDestroy( newTask );
        return err;
        }
    else
        {
        CleanupStack::Pop( newTask );
        }

    position = iBackgroundTasks.Count() - 1;

    newTask->Start();

    newTask->SetTaskState( MCABackgroundInterface::EIdleStarted );

    NotifyBackgroundObservers(
        position,
        EIdleStarted,
        newTask->SubTasks()
    );
    return err; //KErrNone
    }

// -----------------------------------------------------------------------------
// TBackgroundNotifier::TBackgroundNotifier
// constructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBackgroundNotifier::TBackgroundNotifier( MCABackgroundObserver* aObserver
                                          , TInt aTaskMask
                                          , TInt aEventMask
                                        ) :
        iObserver( aObserver )
        , iTaskMask( aTaskMask )
        , iEventMask( aEventMask )
    {
    }

// -----------------------------------------------------------------------------
// TBackgroundNotifier::Identity
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool TBackgroundNotifier::Identity( const TBackgroundNotifier& aA
                                     , const TBackgroundNotifier& aB
                                   )
    {
    return ( aA.iObserver == aB.iObserver );
    }


// -----------------------------------------------------------------------------
// TBackgroundNotifier::IsBackgroundTaskPending
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CCARequestMapper::IsBackgroundTaskPending()
    {
    const TInt limit( iBackgroundTasks.Count() );

    for ( TInt i( 0 ); i < limit; i++ )
        {
        CCABackgroundTask* task = iBackgroundTasks[ i ];

        if ( task->TaskId() == EGroupFetch )
            {

            MCABackgroundInterface::TCABackgroundStatus taskState = task->TaskState();

            if ( taskState == EIdleStarted || taskState == EStarting || taskState == EWaiting )
                {

                return ETrue;
                }
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCAEngine::RegisterGroupSyncObserver()
// ---------------------------------------------------------
//
void CCARequestMapper::RegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver )
    {
    ASSERT( aObserver );
    TInt status( iBackGroundTaskObserver.Find( aObserver ) );
    if ( status == KErrNotFound )
        {
        status = iBackGroundTaskObserver.Append( aObserver ) ;
        }
    }

// ---------------------------------------------------------
// CCAEngine::UnRegisterGroupSyncObserver()
// ---------------------------------------------------------
//
void CCARequestMapper::UnRegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver )
    {
    ASSERT( aObserver );
    const TInt status( iBackGroundTaskObserver.Find( aObserver ) );
    if ( status != KErrNotFound )
        {
        iBackGroundTaskObserver.Remove( status );
        iBackGroundTaskObserver.Compress();
        }
    }

// ---------------------------------------------------------
// CCAEngine::NotifyGroupSyncObservers()
// ---------------------------------------------------------
//
void CCARequestMapper::NotifyBackgroundTaskObservers()
    {
    TInt count = iBackGroundTaskObserver.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        TRAP_IGNORE ( iBackGroundTaskObserver[i]->HandleBackGroundTaskCompleteL( IsBackgroundTaskPending() ) );
        }
    }

//  End of File
