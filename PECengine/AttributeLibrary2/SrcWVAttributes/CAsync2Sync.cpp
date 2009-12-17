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
* Description:  Async to sync.
*
*/

// INCLUDE FILES
#include <E32Base.h>
#include "CAsync2Sync.h"
#include "PresenceDebugPrint.h"


// -----------------------------------------------------------------------------
// CAsync2Sync::NewLC()
// -----------------------------------------------------------------------------
//
CAsync2Sync* CAsync2Sync::NewLC()
    {
    CAsync2Sync* self = new ( ELeave ) CAsync2Sync;
    CleanupStack::PushL( self );
    return self;
    }


// Destructor
CAsync2Sync::~CAsync2Sync()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CAsync2Sync::CAsync2Sync()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAsync2Sync::CAsync2Sync()
        : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    Prepare();
    }


// -----------------------------------------------------------------------------
// CAsync2Sync::Prepare()
// -----------------------------------------------------------------------------
//
void CAsync2Sync::Prepare()
    {
    Cancel();
    iStatus = KErrNone;
    }


// -----------------------------------------------------------------------------
// CAsync2Sync::WaitToComplete()
// -----------------------------------------------------------------------------
//
void CAsync2Sync::WaitToComplete()
    {
    if ( iStatus == KRequestPending )
        {
        //some asynchronous operation really started
        //==> wait it to complete
        SetActive();

        PENG_DP( D_PENG_LIT( "CAsync2Sync::WaitToComplete() Waiting..." ) );
        iWait.Start();      // CSI: 10 #
        PENG_DP( D_PENG_LIT( "CAsync2Sync::WaitToComplete() Wait done [%d]" ), iStatus.Int() );
        }
    else
        {
        //No  request pending
        PENG_DP( D_PENG_LIT( "CAsync2Sync::WaitToComplete() No pending request to wait" ) );
        }
    }


// -----------------------------------------------------------------------------
// CAsync2Sync::RequestStatus()
// -----------------------------------------------------------------------------
//
TRequestStatus& CAsync2Sync::RequestStatus()
    {
    return iStatus;
    }


// -----------------------------------------------------------------------------
// CAsync2Sync::RunL()
// -----------------------------------------------------------------------------
//
void CAsync2Sync::RunL()
    {
    iWait.AsyncStop();
    }


// -----------------------------------------------------------------------------
// CAsync2Sync::RunError()
// -----------------------------------------------------------------------------
//
TInt CAsync2Sync::RunError( TInt aError )
    {
    iWait.AsyncStop();
    iStatus = aError;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CAsync2Sync::DoCancel()
// -----------------------------------------------------------------------------
//
void CAsync2Sync::DoCancel()
    {
    iStatus = KErrCancel;
    }


// End of file.


