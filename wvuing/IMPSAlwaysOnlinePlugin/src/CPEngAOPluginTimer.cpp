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
* Description:  This class implements the waiting for next login retry feature
*				 for the Always Online Plugin
*
*/


// INCLUDE FILES
#include "CPEngAOPluginTimer.h"
#include "MPEngAOWaitObserver.h"

#include "IMPSCommonUiDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAOPluginTimer::CPEngAOPluginTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAOPluginTimer::CPEngAOPluginTimer() : CTimer( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPEngAOPluginTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAOPluginTimer::ConstructL()
    {
    // the base class must be constructed explicitely
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CPEngAOPluginTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAOPluginTimer* CPEngAOPluginTimer::NewL()
    {
    CPEngAOPluginTimer* self = new( ELeave ) CPEngAOPluginTimer;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CPEngAOPluginTimer::~CPEngAOPluginTimer()
    {
    Cancel();
    }

void CPEngAOPluginTimer::SetObserver( MPEngAOWaitObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CPEngAOPluginTimer::At
// Derived from CTimer
// -----------------------------------------------------------------------------
//
void CPEngAOPluginTimer::At( const TTime& aTime )
    {
    iTimerStarted.HomeTime();
    iExpectedLaunchTime = aTime;
    CTimer::At( aTime );
    }

// -----------------------------------------------------------------------------
// CPEngAOPluginTimer::RunL
// Derived from CActive
// -----------------------------------------------------------------------------
//
void CPEngAOPluginTimer::RunL()
    {
    IMPSCUI_DP_FUNC_ENTER( "RunL" );
    if ( iObserver )
        {
        TInt status( iStatus.Int() );
        IMPSCUI_DP( D_IMPSCUI_LIT( "CPEngAOPluginTimer::RunL status = %d" ), status );
        if ( status < KErrNone )
            {
            if ( status == KErrAbort )
                {
                TTime curTime;
                curTime.HomeTime();

                // if current time is between start time and
                // expected launch time then just restarting timer to
                // expected launch time to avoid unnecessary timer
                // early launching
                if ( ( curTime > iTimerStarted ) &&
                     ( curTime < iExpectedLaunchTime ) )
                    {
                    IMPSCUI_DP_TXT( "Timer stopped prematurely with error KErrAbort(-39)" );
                    IMPSCUI_DP_TXT( "Restarting timer to expected launch time" );
                    TDateTime launchTime;
                    launchTime = iExpectedLaunchTime.DateTime();
                    // +1 added to day and month to get debug print human readable form
                    IMPSCUI_DP( D_IMPSCUI_LIT( "year [%d] " ), launchTime.Year() );
                    IMPSCUI_DP( D_IMPSCUI_LIT( "day [%d], month [%d]" ), launchTime.Day() + 1,
                                launchTime.Month() + 1 );
                    IMPSCUI_DP( D_IMPSCUI_LIT( "hour [%d], minu [%d]" ), launchTime.Hour(),
                                launchTime.Minute() );
                    At( iExpectedLaunchTime );
                    // returning because error handled in this timer itself
                    IMPSCUI_DP_FUNC_DONE( "RunL" );
                    return;
                    }
                }
            iObserver->HandleTimeWaitedError( status );
            }
        else
            {
            iObserver->HandleTimeWaited();
            }
        }
    IMPSCUI_DP_FUNC_DONE( "RunL" );
    }

// ---------------------------------------------------------
// CPEngAOPluginTimer::RunError
// Derived from CActive
// ---------------------------------------------------------
//
TInt CPEngAOPluginTimer::RunError( TInt aError )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CPEngAOPluginTimer::RunError [%d]" ), aError );

    if ( iObserver )
        {
        iObserver->HandleTimeWaitedError( aError );
        }

    return aError;
    }

//  End of File
