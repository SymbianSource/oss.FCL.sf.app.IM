/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Alive manager for IMPS engine.
*
*
*/


// INCLUDE FILES
#include    <s32std.h>
#include    "impsalivemanager.h"
#include    "impsutils.h"
#include    "impscspsessionapi.h"
#include    "impstimer.h"
#include    "impserrors.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CImpsAliveManager::CImpsAliveManager
// ---------------------------------------------------------
//
CImpsAliveManager::CImpsAliveManager( MImpsCSPSession& aServer )
        : CActive( 0 ),
        iState( EImpsAliveIdle ),
        iServer( aServer ),
        iTimer( NULL ),
        iScheduled( EFalse ),
        iTid( KNullDesC ),
        iSeconds( 0 )
    {
    // Add this to the scheduler
    CActiveScheduler::Add( this );
    }

// Two-phased constructor.
CImpsAliveManager* CImpsAliveManager::NewL( MImpsCSPSession& aServer )
    {
    CImpsAliveManager* self = new ( ELeave ) CImpsAliveManager( aServer );

    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( );

    return self;
    }


// ---------------------------------------------------------
// CImpsAliveManager::ConstructL
// ---------------------------------------------------------
//
void CImpsAliveManager::ConstructL()
    {
    iTimer = new ( ELeave ) CImpsAliveTimer( *this, CActive::EPriorityStandard );
    }

// ---------------------------------------------------------
// CImpsAliveManager::~CImpsAliveManager
// ---------------------------------------------------------
//
CImpsAliveManager::~CImpsAliveManager(  )
    {
    Cancel();
    delete iTimer;
    }

// ---------------------------------------------------------
// CImpsAliveManager::StartTimer
// Server core calls this always when receving KeepAliveResp.
// ---------------------------------------------------------
//
void CImpsAliveManager::StartTimer( TInt aInterval )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: StartTimer" ) );
#endif
    iState = EImpsAliveActive;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: iState = %d " ), iState );
#endif
    iFailCount = 0;
    iTimerFail = 0;
    // Some time needed to open PDP, that's why 60 seconds decreased
    TInt myInterval = ( aInterval / 2 ) - 60;
    if ( myInterval < 60 )
        {
        myInterval = aInterval / 2;
        }
    iSeconds = myInterval;
    iTimer->Start( iSeconds );
    }

// ---------------------------------------------------------
// CImpsAliveManager::StopTimer
// ---------------------------------------------------------
//
void CImpsAliveManager::StopTimer(  )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: StopTimer" ) );
#endif
    iTimer->Stop();
    iState = EImpsAliveIdle;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: iState = %d " ), iState );
#endif
    }

// ---------------------------------------------------------
// CImpsAliveManager::CheckResp
// ---------------------------------------------------------
//
void CImpsAliveManager::CheckResp( TInt aCode )
    {
    // Session id check should already be done in engine core.
    // Any normal transport response resets the timer
    if ( iState != EImpsAlivePending )
        {
        iTimer->Start( iSeconds );
        return;
        }

    // Something is received within the session.
    // We are not interested to get the particular response for
    // KeepAliveReq transaction.
    if ( aCode !=  600 &&
         aCode !=  601 &&
         aCode !=  604 )
        {
        iState = EImpsAliveActive;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "AliveManager: CheckResp resets iState = %d " ), iState );
#endif
        iFailCount = 0;
        iTimerFail = 0;
        return;
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "AliveManager: CheckResp 60x" ) );
#endif
        // Just stop timer here. DoLogout is called later in engine core,
        // after the possible response event is created for the client.
        StopTimer();
        }
    }

// ---------------------------------------------------------
// CImpsAliveManager::CheckError
// ---------------------------------------------------------
//
void CImpsAliveManager::CheckError( const TDesC& aTid )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: CheckError %d" ),
                            iState );
#endif
    if ( iState != EImpsAlivePending )
        {
        return;
        }
    // Open PDP errors come here without TID and then error handling is required.
    if ( aTid.Length() && iTid.Compare( aTid ) )
        {
        // This was not KeepAliveReguest transaction
        return;
        }
    iState = EImpsAliveActive;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: iState = %d " ), iState );
#endif
    DoHandleError( );
    }

// ---------------------------------------------------------
// CImpsAliveManager::SendKeepAlive
// ---------------------------------------------------------
//
void CImpsAliveManager::SendKeepAlive(
    TBool aSchedule )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: SendKeepAlive sch=%d state=%d fc=%d tf=%d" ),
                            aSchedule, iState, iFailCount, iTimerFail );
#endif

    if ( iState == EImpsAliveActive )
        {
        iState = EImpsAlivePending;
        iScheduled = aSchedule;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "AliveManager: iState = %d " ), iState );
#endif

        TRAPD( err, ( iTid = iServer.SendAliveL( ) ) );
        if ( err )
            {
            // Error received immediately and
            // since going back to active state.
            iState = EImpsAliveActive;
            DoHandleError( );
            }
        }
    else if ( iState == EImpsAliveIdle )
        {
        return;
        }
    else // if ( iState == EImpsAlivePending )
        {
        if ( aSchedule )
            {
            // Previous request not complete!
            iTimerFail++;
            iFailCount = 0;
            if ( iTimerFail > KImpsMaxAliveScheduleErr )
                {
                // current CSP session seems to be lost!
                DoSessionClose();
                return;
                }
            }
        TRAPD( err, ( iTid = iServer.SendAliveL( ) ) );
        if ( err )
            {
            DoHandleError( );
            }
        }
    }

// ---------------------------------------------------------
// CImpsAliveManager::DoHandleError
// ---------------------------------------------------------
//
void CImpsAliveManager::DoHandleError( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: DoHandleError" ) );
#endif
    if ( IsActive() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "AliveManager: DoHandleError was active" ) );
#endif
        return;
        }
    TRequestStatus* s = &iStatus;
    iFailCount++;
    if ( iFailCount > KImpsMaxAliveRetryOnce )
        {
        if ( !iScheduled )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "AliveManager: DoHandleError !scheduled" ) );
#endif
            return;
            }
        iTimerFail++;
        iFailCount = 0;
        if ( iTimerFail > KImpsMaxAliveScheduleErr )
            {
            // current CSP session seems to be lost!
            // Yield control to scheduler
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "AliveManager: DoHandleError NOK stop" ) );
#endif
            iStatus = KRequestPending;
            SetActive();
            User::RequestComplete( s, KImpsNotLoggedStatus );
            }
        // else not try more until scheduled by the timer
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "AliveManager: DoHandleError OK retry" ) );
#endif
        // Tell CSP session to send KeepAlive when GPRS resumes
        iServer.SendAliveInResume();
        // Yield control to scheduler and retry in RunL
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( s, KErrNone );
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: DoHandleError END" ) );
#endif
    }

// ---------------------------------------------------------
// CImpsAliveManager::DoSessionClose
// ---------------------------------------------------------
//
void CImpsAliveManager::DoSessionClose( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: DoSessionClose" ) );
#endif
    iTimer->Stop();
    iState = EImpsAliveIdle;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: iState = %d " ), iState );
#endif
    iServer.DoLogout();
    }

// ---------------------------------------------------------
// CImpsAliveManager::DoCancel
// ---------------------------------------------------------
//
void CImpsAliveManager::DoCancel()
    {
    iTimer->Stop();
    iState = EImpsAliveIdle;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: iState = %d " ), iState );
#endif
    }

// ---------------------------------------------------------
// CImpsAliveManager::RunL
// ---------------------------------------------------------
//
void CImpsAliveManager::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: RunL %d" ), iState );
#endif
    if ( iState == EImpsAliveIdle )
        {
        // Cancelled
        return;
        }
    if ( iStatus == KImpsNotLoggedStatus )
        {
        // current CSP session is lost
        DoSessionClose();
        }
    else
        {
        // Retry
        SendKeepAlive( iScheduled );
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveManager: RunL ends" ) );
#endif
    }

//**********************************
// CImpsAliveTimer
//**********************************

CImpsAliveTimer::CImpsAliveTimer(
    CImpsAliveManager& aServer,
    TInt aPriority )
        : CActive( aPriority ),
        iMgr( aServer ),
        iReset( EFalse ),
        iCanceled( EFalse )
    {
    // Add this to the scheduler
    ( void ) iTimer.CreateLocal();
    CActiveScheduler::Add( this );
    }

CImpsAliveTimer::~CImpsAliveTimer()
    {
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------
// CImpsAliveTimer::Start
// ---------------------------------------------------------
//
void CImpsAliveTimer::Start( TInt aWaitSeconds )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveTimer: Start %d sec" ), aWaitSeconds );
#endif
    iReset = EFalse;
    if ( aWaitSeconds <= 0 )
        {
        return;
        }
    // Cancel is needed because of the timer may be reset.
    Cancel();
    iSeconds = aWaitSeconds;

    // The At function caused a CUserbase-Panic 46 in very small
    // time values. 1-4 seconds. Now if the KeepAlive time
    // is smaller than UseAfterLimit, then we use the After function
    // If it is larger then use the At function
    // The reason not to use the After function for every situation is
    // that the TInt overflows after 35 minutes. 1000000*60*36 > 2^31
    if ( iSeconds < KUseAfterLimit )
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

// ---------------------------------------------------------
// CImpsAliveTimer::Stop
// ---------------------------------------------------------
//
void CImpsAliveTimer::Stop( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveTimer: Stop" ) );
#endif
    iReset = ETrue;
    Cancel();
    // The following is needed because of the timer may just
    // have triggered and thus it is not active, but the request is
    // waiting in the scheduler and coming to RunL later.
    iCanceled = ETrue;
    }

// ---------------------------------------------------------
// CImpsAliveTimer::DoCancel
// ---------------------------------------------------------
//
void CImpsAliveTimer::DoCancel()
    {
    iTimer.Cancel();
    }

// ---------------------------------------------------------
// CImpsAliveTimer::RunL
// ---------------------------------------------------------
//
void CImpsAliveTimer::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "AliveTimer: RunL" ) );
#endif
    if ( iCanceled )
        {
        return;
        }
    if ( !iReset )
        {
        iMgr.SendKeepAlive( ETrue );
        }
    iReset = EFalse;
    Start( iSeconds );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File

