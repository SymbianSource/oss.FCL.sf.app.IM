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
* Description: imlaunchertest test module.
*
*/

#include <e32base.h>
#include <e32std.h>		//RTimer
#include "timertool.h"

static const TInt KDefaultTimeOut = 10;

#include <flogger.h>

#include "timertool.h"

// CONSTANTS
_LIT( KDebugOutputDir, "DummyApp" );
_LIT( KDebugOutputFileName, "DummyApp.log" );
const TInt KMaxLogLineLength = 250;


CTimerTool* CTimerTool::NewL()
    {
    CTimerTool* iSelf = new ( ELeave ) CTimerTool();
    iSelf->ConstructL();
    return iSelf;
    }

CTimerTool::CTimerTool() : CActive( EPriorityStandard )//, iObserver(aObserver)
    {
    }

void CTimerTool::ConstructL()
    {

    error = KErrNone;
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

CTimerTool::~CTimerTool()
    {
    Deque();
    iTimer.Close();

    //Anumeha - check if u need this

    //iTimer = NULL;
    //delete iTimer;
    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "desctructor of CTimerTool" ) );
    buffer.Append( _L( "] " ) );            // CSI: 78 #

    //Log goes to file
    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );



    }

void CTimerTool::Start( TTimeIntervalMicroSeconds32 aInterval )
    {

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "Start on the timer is called" ) );
    buffer.Append( _L( "] " ) );            // CSI: 78 #

    //Log goes to file
    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    iTimer.After( iStatus, aInterval );

    //iStatus = KRequestPending;
    SetActive();

    //Start the timer
    //User::After(aInterval);

    //Start the active scheduler
    //CActiveScheduler::Start();

    }

//void CTimerTool::Stop()
TInt CTimerTool::Stop()
    {
    //Cancel();

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "Stop on the timer has been called" ) );
    buffer.Append( _L( "] " ) );            // CSI: 78 #

    //Log goes to file
    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    TRAP( error, iTimer.Cancel() );
    CActiveScheduler::Stop();

    return error;
    }

void CTimerTool::DoCancel()
    {
    iTimer.Cancel();
    }

void CTimerTool::RunL()
    {

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "RunL for the timer is called" ) );
    buffer.Append( _L( "] " ) );            // CSI: 78 #

    //Log goes to file
    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    //iReturnValue = iStatus.Int();

    /* if (error == KErrCancel)
        {
    	iReturnValue = KErrCancel;
    	CActiveScheduler::Stop();
        }
    else
        {
    	iReturnValue = KErrTimedOut;
    	//Restart the timer, keeping in loop
    	iTimer.Start(KDefaultTimeOut * 1000000);
        } */

    //Check the User::RequestComplete error code.
    switch ( iStatus.Int() )

        {
        case KErrCancel:
            //User called Cancel, do nothing.

            buffer.Append( _L( "[" ) );             // CSI: 78 #
            buffer.Append( _L( "KErrCancel for the timer" ) );
            buffer.Append( _L( "] " ) );            // CSI: 78 #

            //Log goes to file
            RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );
            break;

        case KErrNone:
        case KErrTimedOut:


            buffer.Append( _L( "[" ) );             // CSI: 78 #
            buffer.Append( _L( "KErrTimedOut for the timer" ) );
            buffer.Append( _L( "] " ) );            // CSI: 78 #

            //Log goes to file
            RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );
            // keep the timer running;

            Start( KDefaultTimeOut * 1000000 );
            //Start functiom in the CTimerTool class

            break;

        default:
            buffer.Append( _L( "[" ) );             // CSI: 78 #
            buffer.Append( _L( "default case for the timer" ) );
            buffer.Append( _L( "] " ) );            // CSI: 78 #

            //Log goes to file
            RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

            break;
        }
    }