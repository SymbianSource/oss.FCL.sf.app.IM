/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
* timer class for imps handler.
*
*/


// INCLUDE FILES
#include    <e32std.h>
#include    <flogger.h>
#include    "impsutils.h"
#include    "impshandler.h"
#include    "impshandlertimer.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

const TInt KWaitTime = 5;

// -----------------------------------------------------------------------------
// CImpsHandlerTimer::Start
// -----------------------------------------------------------------------------
CImpsHandlerTimer::CImpsHandlerTimer(
    TInt aPriority,
    CImpsHandler2& aHandler ) 
    : CActive( aPriority ),
      iHandler( aHandler )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandlerTimer: Constructor"));
#endif    
    // Add this to the scheduler
    (void) iTimer.CreateLocal();
    CActiveScheduler::Add(this);
    }

CImpsHandlerTimer::~CImpsHandlerTimer()
    {
    Cancel();
    iTimer.Close();
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandlerTimer: Destructor"));
#endif        
    }

// -----------------------------------------------------------------------------
// CImpsHandlerTimer::Start
// -----------------------------------------------------------------------------
void CImpsHandlerTimer::Start( )
    {
    if ( IsActive() )
    	{
    	return;
    	}
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandlerTimer: Start"));
#endif
    iTimer.After( iStatus, KWaitTime * 1000000 );
    iStatus = KRequestPending;
    SetActive();
    }

// -----------------------------------------------------------------------------
// CImpsHandlerTimer::RunL
// -----------------------------------------------------------------------------
void CImpsHandlerTimer::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandlerTimer: RunL, calling EventHandled"));
#endif  
    iHandler.StartRun();  
    iHandler.EventHandled();
    }
    
// -----------------------------------------------------------------------------
// CImpsHandlerTimer::DoCancel
// -----------------------------------------------------------------------------
void CImpsHandlerTimer::DoCancel()
    {
    iTimer.Cancel();
    }

//  End of File  




