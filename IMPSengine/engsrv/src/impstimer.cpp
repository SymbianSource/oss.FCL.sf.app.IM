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
* Description: Timers assemble for imps engine base function.
*
*               CImpsIdleTimer
*               Idle timer to lauch Poll requests in CSP protocol.
*               CImpsPDPIdleTimer
*               Idle timer to close idle PDP-context (IAP).
*               CImpsPDPOpenTimer
*               Timer to open idle PDP-context.
*               CImpsShutdownTimer
*               Idle timer to lauch keep-alive and Poll requests in CSP protocol.
*
*/


// INCLUDE FILES
#include    <e32std.h>
#include    <flogger.h>
#include    "impsutils.h"
#include    "impsTimer.h"
#include    "ImpsServer.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

const TInt KImpsUseAfterLimit = 240;

//**********************************
// CImpsBaseTimer
//**********************************
CImpsBaseTimer::CImpsBaseTimer(
    TInt aPriority )
        : CActive( aPriority ),
        iReset( EFalse )
    {
    // Add this to the scheduler
    ( void ) iTimer.CreateLocal();
    CActiveScheduler::Add( this );
    }

CImpsBaseTimer::~CImpsBaseTimer()
    {
    Cancel();
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CImpsBaseTimer::Start
// -----------------------------------------------------------------------------
void CImpsBaseTimer::Start( TInt aWaitSeconds )
    {
    iReset = EFalse;

    // Cancel is needed because of the timer may be reset.
    Cancel();

    if ( aWaitSeconds <= 0 )
        {
        return;
        }

    iSeconds = aWaitSeconds;

    // The At function caused a CUserbase-Panic 46 in very small
    // time values. 1-4 seconds. Now if the KeepAlive time
    // is smaller than UseAfterLimit, then we use the After function
    // If it is larger then use the At function
    // The reason not to use the After function for every situation is
    // that the TInt overflows after 35 minutes. 1000000*60*36 > 2^31
    if ( iSeconds <= KImpsUseAfterLimit )
        {
        iTimer.After( iStatus, iSeconds * 1000000 );
        }
    else
        {
        TTime myKeepAlive;
        myKeepAlive.HomeTime();
        myKeepAlive += TTimeIntervalSeconds( iSeconds );
        iTimer.At( iStatus, myKeepAlive );
        }
    iStatus = KRequestPending;
    SetActive();

    }

// -----------------------------------------------------------------------------
// CImpsBaseTimer::Stop
// -----------------------------------------------------------------------------
void CImpsBaseTimer::Stop( )
    {
    iReset = ETrue;
    Cancel();
    }

// -----------------------------------------------------------------------------
// CImpsBaseTimer::Reset
// -----------------------------------------------------------------------------
void CImpsBaseTimer::Reset( )
    {
    // Avoid continuos cancelling, therefore just set flag on
    iReset = ETrue;
    }

// -----------------------------------------------------------------------------
// CImpsBaseTimer::DoCancel
// -----------------------------------------------------------------------------
void CImpsBaseTimer::DoCancel()
    {
    iTimer.Cancel();
    }

//**********************************
// CImpsIdleTimer
//**********************************

CImpsIdleTimer::CImpsIdleTimer(
    MImpsCSPSession& aServer,
    TInt aPriority )
        : CImpsBaseTimer( aPriority ),
        iServer( aServer )
    {
    }

CImpsIdleTimer::~CImpsIdleTimer()
    {
    }

// -----------------------------------------------------------------------------
// CImpsIdleTimer::Start
// -----------------------------------------------------------------------------
void CImpsIdleTimer::Start( TInt aWaitSeconds )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsIdleTimer: Start sec=%d" ), aWaitSeconds );
#endif
    CImpsBaseTimer::Start( aWaitSeconds );
    }

// -----------------------------------------------------------------------------
// CImpsIdleTimer::Stop
// -----------------------------------------------------------------------------
void CImpsIdleTimer::Stop()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsIdleTimer: Stop" ) );
#endif
    CImpsBaseTimer::Stop();
    }

// -----------------------------------------------------------------------------
// CImpsIdleTimer::RunL
// -----------------------------------------------------------------------------
void CImpsIdleTimer::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "IdleTimer: RunL" ) );
#endif
    TInt err = KErrNone;
    TRAP( err, iServer.SendPollL() );
    Start( iSeconds );
    }


//**********************************
// CImpsPDPIdleTimer
//**********************************

CImpsPDPIdleTimer::CImpsPDPIdleTimer(
    MImpsCSPSession& aServer,
    TInt aPriority )
        : CImpsBaseTimer( aPriority ),
        iServer( aServer )
    {
    }

CImpsPDPIdleTimer::~CImpsPDPIdleTimer()
    {
    }

// -----------------------------------------------------------------------------
// CImpsPDPIdleTimer::Reset
// -----------------------------------------------------------------------------
void CImpsPDPIdleTimer::Reset( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPDPIdleTimer: Reset" ) );
#endif
    // Reset re-starts this timer only if already runnig
    if ( IsActive() )
        {
        CImpsBaseTimer::Start( iSeconds );
        }
    }

// -----------------------------------------------------------------------------
// CImpsPDPIdleTimer::Start
// -----------------------------------------------------------------------------
void CImpsPDPIdleTimer::Start( TInt aWaitSeconds )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPDPIdleTimer: Start sec =%d" ), aWaitSeconds );
#endif
    CImpsBaseTimer::Start( aWaitSeconds );
    }

// -----------------------------------------------------------------------------
// CImpsPDPIdleTimer::Stop
// -----------------------------------------------------------------------------
void CImpsPDPIdleTimer::Stop()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPDPIdleTimer: Stop" ) );
#endif
    CImpsBaseTimer::Stop();
    }

// -----------------------------------------------------------------------------
// CImpsPDPIdleTimer::RunL
// -----------------------------------------------------------------------------
void CImpsPDPIdleTimer::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "PDPIdleTimer: RunL" ) );
#endif
    // This timer only calls ClosePDP()
    if ( iStatus.Int() != KErrCancel )
        {
        iServer.ClosePDP();
        }
    iReset = EFalse;
    Start( iSeconds );
    }

//**********************************
// CImpsPDPOpenTimer
//**********************************

CImpsPDPOpenTimer::CImpsPDPOpenTimer(
    MImpsCSPSession& aServer,
    TInt aPriority )
        : CImpsBaseTimer( aPriority ),
        iServer( aServer )
    {
    }

CImpsPDPOpenTimer::~CImpsPDPOpenTimer()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPDPOpenTimer: DELETE" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CImpsPDPOpenTimer::Start
// -----------------------------------------------------------------------------
void CImpsPDPOpenTimer::Start( TInt aWaitSeconds )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPDPOpenTimer: Start sec=%d" ), aWaitSeconds );
#endif
    CImpsBaseTimer::Start( aWaitSeconds );
    }
// -----------------------------------------------------------------------------
// CImpsPDPOpenTimer::Stop
// -----------------------------------------------------------------------------
void CImpsPDPOpenTimer::Stop()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPDPOpenTimer: Stop" ) );
#endif
    CImpsBaseTimer::Stop();
    }

// -----------------------------------------------------------------------------
// CImpsPDPOpenTimer::RunL
// -----------------------------------------------------------------------------
void CImpsPDPOpenTimer::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPDPOpenTimer: RunL" ) );
#endif
    if ( iStatus.Int() != KErrCancel )
        {
        TInt ret = iServer.OpenPDP();
        // if already open then do not start
        if ( ret == KErrAlreadyExists )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsPDPOpenTimer: not started anymore ***" ) );
#endif
            return;
            }
        }
    Start( iSeconds );
    }

//**********************************
// CImpsShutdownTimer
//**********************************

CImpsShutdownTimer::CImpsShutdownTimer(
    CImpsServer& aServer,
    TInt aPriority )
        : CImpsBaseTimer( aPriority ),
        iServer( aServer )
    {
    }

CImpsShutdownTimer::~CImpsShutdownTimer()
    {
    iStatus = KErrNone;
    }

// -----------------------------------------------------------------------------
// CImpsShutdownTimer::RunL
// -----------------------------------------------------------------------------
void CImpsShutdownTimer::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "ShutDownTimer: RunL" ) );
#endif
    if ( iStatus != KErrNone )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "ShutDownTimer: RunL ignored" ) );
#endif
        return;
        }
    if ( !iReset )
        {
        iServer.DoShutDown();
        }
    // It is extremely important that after the previous call this
    // method does nothing since DoShutDown actully deletes the
    // current entity if this class!
    }

// -----------------------------------------------------------------------------
// CImpsShutdownTimer::Stop
// -----------------------------------------------------------------------------
void CImpsShutdownTimer::Stop()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "ShutDownTimer: Stop" ) );
#endif
    CImpsBaseTimer::Stop();
    }

// -----------------------------------------------------------------------------
// CImpsShutdownTimer::Start
// -----------------------------------------------------------------------------
void CImpsShutdownTimer::Start( TInt aWaitSeconds )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "ShutDownTimer: Start sec=%d" ), aWaitSeconds );
#endif

    Cancel();

    if ( !aWaitSeconds )
        {
        // Do the shut down ASAP
        iStatus = KRequestPending;
        SetActive();
        iReset = EFalse;
        TRequestStatus* s = &iStatus;
        User::RequestComplete( s, KErrNone );
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "ShutDownTimer: RequestComplete" ) );
#endif
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "ShutDownTimer: BaseTimer:Start begins" ) );
#endif
        CImpsBaseTimer::Start( aWaitSeconds );
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "ShutDownTimer: Start ends" ) );
#endif
    }

//**********************************
// CImpsExpiryTimer
//**********************************

CImpsExpiryTimer::CImpsExpiryTimer(
    CImpsServer& aServer,
    TImpsEventType aType,
    TInt aPriority )
        : CImpsBaseTimer( aPriority ),
        iServer( aServer ),
        iType ( aType )
    {
    }

CImpsExpiryTimer::~CImpsExpiryTimer()
    {
    }

// -----------------------------------------------------------------------------
// CImpsExpiryTimer::Start
// -----------------------------------------------------------------------------
void CImpsExpiryTimer::Start( TInt aWaitSeconds )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsExpiryTimer: Start sec=%d" ), aWaitSeconds );
#endif
    CImpsBaseTimer::Start( aWaitSeconds );
    }

// -----------------------------------------------------------------------------
// CImpsExpiryTimer::Stop
// -----------------------------------------------------------------------------
void CImpsExpiryTimer::Stop()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsExpiryTimer: Stop" ) );
#endif
    CImpsBaseTimer::Stop();
    }

// -----------------------------------------------------------------------------
// CImpsExpiryTimer::RunL
// -----------------------------------------------------------------------------
void CImpsExpiryTimer::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "ExpiryTimer: RunL" ) );
#endif
    if ( iStatus == KErrNone )
        {
        iServer.CheckExpiryL( iType );
        iTimer.After( iStatus, iSeconds * 1000000 );
        SetActive();
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "ExpiryTimer: RunL ignored" ) );
#endif
        }
    }

//**********************************
// CImpsSendQueued
//**********************************

CImpsSendQueued::CImpsSendQueued(
    MImpsCSPSession& aServer,
    TInt aPriority )
        : CActive( aPriority ),
        iServer( aServer ),
        iCanceled ( EFalse )
    {
    // Add this to the scheduler
    CActiveScheduler::Add( this );
    }

CImpsSendQueued::~CImpsSendQueued()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CImpsSendQueued::Send
// -----------------------------------------------------------------------------
void CImpsSendQueued::Send(  )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendQueued: Send" ) );
#endif
    // This yields the control to the server thread active scheduler
    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        iCanceled = EFalse;
        TRequestStatus* s = &iStatus;
        User::RequestComplete( s, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CImpsSendQueued::DoCancel
// -----------------------------------------------------------------------------
void CImpsSendQueued::DoCancel()
    {
    iCanceled = ETrue;
    }

// -----------------------------------------------------------------------------
// CImpsSendQueued::RunL
// -----------------------------------------------------------------------------
void CImpsSendQueued::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendQueued: RunL" ) );
#endif
    if ( iCanceled || iStatus.Int() != KErrNone )
        {
        return;
        }
    // Trigger a server thread to send a queued message
    TInt errx = KErrNone;
    TRAP( errx, iServer.DoSendAllQueuedL() );
    }


//  End of File
