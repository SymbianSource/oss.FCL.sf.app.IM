/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CCARefreshTimer class handles the waiting functionality for
*                the storage manager to wait before calling observer in case of
*                multiple contacts adding.
*
*/


// INCLUDE FILES
#include "CCARefreshTimer.h"
#include "MCARefreshTimerObserver.h"

#include "ChatDebugPrint.h"

// CONSTANTS
const TInt KTimeToWaitBeforeRefresh( 1000000 ); // 1 seconds in microseconds

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCARefreshTimer::CCARefreshTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCARefreshTimer::CCARefreshTimer( MCARefreshTimerObserver* aObserver ) :
        CTimer( EPriorityIdle ),
        iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCARefreshTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCARefreshTimer::ConstructL()
    {
    // the base class must be constructed explicitely
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CCARefreshTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCARefreshTimer* CCARefreshTimer::NewL( MCARefreshTimerObserver* aObserver )
    {
    CCARefreshTimer* self = new( ELeave ) CCARefreshTimer( aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCARefreshTimer::~CCARefreshTimer()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CCARefreshTimer::Start
// -----------------------------------------------------------------------------

void CCARefreshTimer::Start()
    {
    CHAT_DP_FUNC_ENTER( "Start" );
    if ( IsActive() )
        {
        Cancel();
        }
    TTimeIntervalMicroSeconds32 waittime( KTimeToWaitBeforeRefresh );
    CTimer::After( waittime );
    CHAT_DP_FUNC_DONE( "Start" );
    }

// -----------------------------------------------------------------------------
// CCARefreshTimer::RunL
// Derived from CActive
// -----------------------------------------------------------------------------
//
void CCARefreshTimer::RunL()
    {
    CHAT_DP_FUNC_ENTER( "RunL" );
    if ( iObserver )
        {
        TInt status( iStatus.Int() );
        CHAT_DP( D_CHAT_LIT( "CCARefreshTimer::RunL status = %d" ), status );
        if ( status != KErrCancel )
            {
            iObserver->HandleTimeWaited( status );
            }
        }
    CHAT_DP_FUNC_DONE( "RunL" );
    }

// ---------------------------------------------------------
// CCARefreshTimer::RunError
// Derived from CActive
// ---------------------------------------------------------
//
TInt CCARefreshTimer::RunError( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT( "CCARefreshTimer::RunError [%d]" ), aError );

    if ( iObserver )
        {
        iObserver->HandleTimeWaited( aError );
        }

    return aError;
    }

//  End of File
