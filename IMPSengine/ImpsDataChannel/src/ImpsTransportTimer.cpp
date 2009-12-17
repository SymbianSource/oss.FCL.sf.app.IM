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
* Description: Timer for Imps Transport.
*
*
*/


// INCLUDES
#include "ImpsTransportTimer.h"
#include "ImpsTimingRequester.h"
#include "HttpTransportadapter.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CImpsTransportTimer::CImpsTransportTimer( MImpsTransportTimerCallback* aCallback,
                                          TImpsTimingRequester* aRequester ) :
        CActive( 0 ),
        iRequester( aRequester ),
        iCallback( aCallback )

    {
    }

// ---------------------------------------------------------
// CImpsTransportTimer::ConstructL()
//
// ---------------------------------------------------------
//
void CImpsTransportTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CImpsTransportTimer::NewL
//
// ---------------------------------------------------------
//
CImpsTransportTimer* CImpsTransportTimer::NewL( MImpsTransportTimerCallback* aCallback,
                                                TImpsTimingRequester* aRequester )
    {
    CImpsTransportTimer* self = new ( ELeave ) CImpsTransportTimer( aCallback, aRequester );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CImpsTransportTimer::~CImpsTransportTimer()
//
// ---------------------------------------------------------
//
CImpsTransportTimer::~CImpsTransportTimer()
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsTransportTimer::~CImpsTransportTimer(), destructor called." ) );
#endif
    Cancel();
    iTimer.Close();
    }


// ---------------------------------------------------------
// CImpsTransportTimer::ActivateTimer
//
// ---------------------------------------------------------
//
void CImpsTransportTimer::ActivateTimer( TTimeIntervalMicroSeconds32 aDelay )
    {
    __ASSERT_ALWAYS( !IsActive(), User::Panic( _L( "CImpsTransportTimer" ), 1 ) );
    iTimer.After( iStatus, aDelay );
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsTransportTimer activated." ) );
#endif
    SetActive();
    }

// ---------------------------------------------------------
// CImpsTransportTimer::RunL
//
// ---------------------------------------------------------
//
void CImpsTransportTimer::RunL()
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsTransportTimer::RunL()." ) );
#endif
    iCallback->HandleTransportTimerEventL( iRequester, iStatus.Int() );
    }

// ---------------------------------------------------------
// CImpsTransportTimer::DoCancel
//
// ---------------------------------------------------------
//
void CImpsTransportTimer::DoCancel()
    {
    iTimer.Cancel();
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsTransportTimer cancelled." ) );
#endif
    }

//  End of File

