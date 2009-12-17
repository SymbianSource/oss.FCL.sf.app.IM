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
* Description: Timer for Tcp CirWatcher.
*
*
*/


// INCLUDES
#include "TcpCirWatcher.h"
#include "TcpCirWatcherTimer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor may NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CTcpCirWatcherTimer::CTcpCirWatcherTimer( MTcpCirWatcherTimerCallback* aCallback ) :
        CActive( 0 ),
        iCallback( aCallback )

    {
    }

// ---------------------------------------------------------
// Symbian OS constructor may leave.
//
// ---------------------------------------------------------
//
void CTcpCirWatcherTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// Two-phased constructor.
//
// ---------------------------------------------------------
//
CTcpCirWatcherTimer* CTcpCirWatcherTimer::NewL( MTcpCirWatcherTimerCallback* aCallback )
    {
    CTcpCirWatcherTimer* self = new ( ELeave ) CTcpCirWatcherTimer( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CTcpCirWatcherTimer::~CTcpCirWatcherTimer()
    {
#ifdef _DEBUG
    CTcpCirWatcher::WriteToLog( _L8( "CTcpCirWatcherTimer::~CTcpCirWatcherTimer(), destructor called." ) );
#endif
    Cancel();
    iTimer.Close();
    }


// ---------------------------------------------------------
// CTcpCirWatcherTimer::ActivateTimer
//
// ---------------------------------------------------------
//
void CTcpCirWatcherTimer::ActivateTimer( const TTimeIntervalMicroSeconds32 aDelay,
                                         const TInt aCurrentTask )
    {
    __ASSERT_ALWAYS( !IsActive(), User::Panic( _L( "CTcpCirWatcherTimer" ), 1 ) );
    iCurrentTask = aCurrentTask;
    iTimer.After( iStatus, aDelay );
#ifdef _DEBUG
    CTcpCirWatcher::WriteToLog( _L8( "CTcpCirWatcherTimer activated." ) );
#endif
    SetActive();
    }

// ---------------------------------------------------------
// CTcpCirWatcherTimer::RunL()
//
// ---------------------------------------------------------
//
void CTcpCirWatcherTimer::RunL()
    {
#ifdef _DEBUG
    CTcpCirWatcher::WriteToLog( _L8( "CTcpCirWatcherTimer::RunL()." ) );
#endif
    iCallback->HandleTimerEventL( iCurrentTask );
    iCurrentTask = 0;
    }

// ---------------------------------------------------------
// CTcpCirWatcherTimer::DoCancel()
// ---------------------------------------------------------
//
void CTcpCirWatcherTimer::DoCancel()
    {
    iTimer.Cancel();
    iCurrentTask = 0;
#ifdef _DEBUG
    CTcpCirWatcher::WriteToLog( _L8( "CTcpCirWatcherTimer::DoCancel()" ) );
#endif
    }

//  End of File

