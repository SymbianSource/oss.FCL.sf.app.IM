/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
//#include <e32base.h>
//#include <e32cons.h>

#include "testdummyapplication.h"
#include "timertool.h"

#include <flogger.h>

// CONSTANTS
_LIT( KDebugOutputDir, "DummyApp" );
_LIT( KDebugOutputFileName, "DummyApp.log" );
const TInt KMaxLogLineLength = 250;

//Forward declarations
//class CTimerTool;

//Macros
static const TInt KDefaultTime = 2;


//TInt CDummyApplication::ThreadStart()
TInt E32Main()
    {

    //__UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack

    TInt err = KErrNone;

    RProcess::Rendezvous( err );

    TRAPD( error, doExampleL() ); // now do the example

    delete cleanup; // destroy clean-up stack

    //__UHEAP_MARKEND;

    return error;
    }


// do the example
//void CDummyApplication::doExampleL()
TInt doExampleL()
    {

    TBuf< KMaxLogLineLength > buffer;

    CActiveScheduler* scheduler = new( ELeave )CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    RProcess p;
    TInt error = KErrNone;

    TRequestStatus rendezvousStatus;
    p.Rendezvous( rendezvousStatus );

    if ( rendezvousStatus == KErrNone )
        {
        p.Resume();
        User::WaitForRequest( rendezvousStatus );               // CSI: 94 #
        error = rendezvousStatus.Int();

        //Print the rendevous status for the process created.

        buffer.Append( _L( "[" ) );             // CSI: 78 #
        buffer.Append( _L( "Process created: rendezvousStatus == KErrNone" ) );
        buffer.Append( _L( "] " ) );

        RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

        }

    //The exe has been created, let the exe write into a log file

    //Format the log line

    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "DummyApplication is running" ) );
    buffer.Append( _L( "] " ) );            // CSI: 78 #

    //Log goes to file
    //RFileLogger logger;

    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    CTimerTool* ilooptimer = CTimerTool::NewL();

    //Start the loop timer
    ilooptimer->Start( KDefaultTime*100000 );

    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy();//scheduler
    return error;
    }

