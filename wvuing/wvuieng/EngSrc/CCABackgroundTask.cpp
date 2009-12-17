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
* Description:  This is encapsulation of a background task, part of CCARequestMapper
*
*/


// INCLUDE FILES
#include "CCABackgroundTask.h"			// Own header
#include "MCABackgroundCallback.h"
#include "MCABackgroundTask.h"

#include <e32base.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCABackgroundTask::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCABackgroundTask* CCABackgroundTask::NewL(
    MCABackgroundCallback* aParent
    , MCABackgroundTask* aBackgroundTask
    , MCABackgroundInterface::TCABackgroundTasks aTask
    , TInt aSubTasks
)
    {

    CCABackgroundTask* self = CCABackgroundTask::NewLC( aParent
                                                        , aBackgroundTask
                                                        , aTask
                                                        , aSubTasks
                                                      );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCABackgroundTask* CCABackgroundTask::NewLC(
    MCABackgroundCallback* aParent
    , MCABackgroundTask* aBackgroundTask
    , MCABackgroundInterface::TCABackgroundTasks aTask
    , TInt aSubTasks
)
    {
    if ( aSubTasks <= 0 )
        {
        User::Leave( KErrArgument );
        }
    CCABackgroundTask* self = new( ELeave ) CCABackgroundTask( aParent
                                                               , aBackgroundTask
                                                               , aTask
                                                               , aSubTasks
                                                             );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// -----------------------------------------------------------------------------
// CCABackgroundTask::~CCABackgroundTask
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCABackgroundTask::~CCABackgroundTask()
    {
    if ( iIdle )
        {
        iIdle->Cancel();
        delete iIdle;
        }
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::Start
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABackgroundTask::Start()
    {
    if ( !iIdle->IsActive() )
        {
        iIdle->Start( TCallBack( BackgroundTask, this ) );
        }
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::Cancel
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABackgroundTask::Cancel()
    {
    iIdle->Cancel();
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::Identity
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCABackgroundTask::Identity( const CCABackgroundTask& aA
                                   , const CCABackgroundTask& aB
                                 )
    {
    return ( aA.iTask == aB.iTask );
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::SubTasks
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCABackgroundTask::SubTasks()
    {
    return iSubTasks;
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::TaskState
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCABackgroundInterface::TCABackgroundStatus CCABackgroundTask::TaskState()
    {
    return iTaskState;
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::SetTaskState
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABackgroundTask::SetTaskState(
    MCABackgroundInterface::TCABackgroundStatus aTaskState )
    {
    iTaskState = aTaskState;
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::More
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCABackgroundTask::More()
    {
    if ( iSubTasks > 1 )
        {
        iSubTasks--;
        return ETrue;
        }
    iSubTasks = 0;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::TaskId
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCABackgroundInterface::TCABackgroundTasks CCABackgroundTask::TaskId()
    {
    return iTask;
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::BackgroundTask
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCABackgroundTask* CCABackgroundTask::BackgroundTask()
    {
    return iBackgroundTask;
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::ExecuteTaskL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABackgroundTask::ExecuteTaskL()
    {
    iBackgroundTask->HandleBackgroundTaskL( iSubTasks );
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::Pending
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCABackgroundTask::Pending()
    {
    return iIdle->IsActive();
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::CCABackgroundTask
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCABackgroundTask::CCABackgroundTask(
    MCABackgroundCallback* aParent
    , MCABackgroundTask* aBackgroundTask
    , MCABackgroundInterface::TCABackgroundTasks aTask
    , TInt aSubTasks
) :
        iParent( aParent ),
        iBackgroundTask( aBackgroundTask ),
        iTask( aTask ),
        iSubTasks( aSubTasks ),
        iTaskState( MCABackgroundInterface::EUnknown )
    {
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCABackgroundTask::ConstructL()
    {
    iIdle = CIdle::NewL( CActive::EPriorityIdle );
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::BackgroundTask
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCABackgroundTask::BackgroundTask( TAny *aInstance )
    {
    return static_cast<CCABackgroundTask*>( aInstance )->DoBackgroundTask();
    }

// -----------------------------------------------------------------------------
// CCABackgroundTask::DoBackgroundTask
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCABackgroundTask::DoBackgroundTask()
    {
    return iParent->HandleBackgroundTask( iTask );
    }

//  End of File
