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
* Description:  imlaunchertest test module.
*
*/



// INCLUDE FILES
#include    <imlauncherplugin.h>
#include    "imlaunchertestclass.h"
//#include    "logutils.h"

//#include <uids.h>
#include "uids.h"


// Default start wait time in sec
static const TInt KDefaultTimeOut = 100;
//const TUid KTestAppId = {0x101FB0CE};


#include <flogger.h>

// CONSTANTS
_LIT( KDebugOutputDir, "DummyApp" );
_LIT( KDebugOutputFileName, "DummyApp.log" );
const TInt KMaxLogLineLength = 250;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImLaunchertestclass::CImLaunchertestclass
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImLaunchertestclass::CImLaunchertestclass()
        :   CActive( EPriorityStandard ),
        iPlugin( NULL ),
        iReturnValue( KErrNone )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CImLauncher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImLaunchertestclass::ConstructL()
    {

    //initialize the timer class
    //iTimer = CImTestShutDownTimer::NewL(this, EPriorityStandard);

    //CActiveScheduler.Add(this);
    }

// -----------------------------------------------------------------------------
// CImLaunchertestclass::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImLaunchertestclass* CImLaunchertestclass::NewL()
    {
    CImLaunchertestclass* self = new( ELeave ) CImLaunchertestclass;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
CImLaunchertestclass::~CImLaunchertestclass()
    {
    Cancel();

    if ( iPlugin ) // extra security check
        {
        delete iPlugin;
        }
    }

// -----------------------------------------------------------------------------
// CImLauncher::RunL
// -----------------------------------------------------------------------------
//
void CImLaunchertestclass::RunL()
    {

    iReturnValue = iStatus.Int();
    // We're finished

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "iReturnValue = %d" ) );
    buffer.Append( _L( "]" ) );            // CSI: 78 #

    //Log goes to file
    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CImLauncher::DoCancel
// -----------------------------------------------------------------------------
//
void CImLaunchertestclass::DoCancel()
    {

    //Issue the cancel request.
    if ( iPlugin ) // extra security check
        {
        iPlugin->CancelStartApplication();

        delete iPlugin;
        iPlugin = NULL;

        }

    }


// -----------------------------------------------------------------------------
// CImLauncher::RunError
// Handles the RunL leave errors
// -----------------------------------------------------------------------------
//
TInt CImLaunchertestclass::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CImLaunchertestclass::StartL()
// Loads the application launcher plugin and waits for the completition
// -----------------------------------------------------------------------------
TInt CImLaunchertestclass::StartL()
    {

    //iCanceled = EFalse; // This is the start application test case
    //Already initialized earlier

    //Initialize aSap and aUserId

    const TDesC aSap = KNullDesC;
    const TDesC aUserId = KNullDesC;


    //TBuf8<50> aApplicationId( _L8("0x") );

    //TUint32 newuid = (KTestAppUid.iUid);
    //aApplicationId.AppendNum(newuid,EHex);

    // ECom loading
    //iPlugin = CImLauncherPlugin::NewL((TDesC8 &)aApplicationId);
    iPlugin = CImLauncherPlugin::NewL( KTestAppUid );

    // Make a request
    iStatus = KRequestPending;
    SetActive();

    iPlugin->StartApplicationL( iStatus, aSap, aUserId );

    //Started the timer, in case no User::RequestComplete is received
    //from StartApplicationL, the timer will expire and the control of
    //the test case comes back to this place.

    //iTimer->After(KDefaultTimeOut * 1000000);

    CActiveScheduler::Start();

    // if the request failed return the fail code to thread starter

    delete iPlugin;
    iPlugin = NULL;

    return 0;

    }

// -----------------------------------------------------------------------------
// ThreadFunction()
// Static function. Installs the CleanupsStack and ActiveScheduler
// then initiates the application launching.
// -----------------------------------------------------------------------------
TInt CImLaunchertestclass::ThreadFunction()
    {

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "ThreadFunction called" ) );
    buffer.Append( _L( "] " ) );

    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    TInt err;

    TRAP( err, CImLaunchertestclass::StartL() );
    REComSession::FinalClose();

    return err;
    }


TInt CImLaunchertestclass::CancelApplication()
    {

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "CancelApplication called" ) );
    buffer.Append( _L( "] " ) );

    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    TInt err;

    //Get the handle to the launched process

    //Issue the cancel request.
    if ( iPlugin ) // extra security check
        {
        iPlugin->CancelStartApplication();

        delete iPlugin;
        iPlugin = NULL;
        }

    REComSession::FinalClose();

    return 0;
    }

TInt CImLaunchertestclass::StartPluginL()
    {
    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "StartPluginL called" ) );
    buffer.Append( _L( "] " ) );

    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    iCanceled = ETrue; //This is the cancel test case

    //Launch the application

    const TDesC aSap = KNullDesC;
    const TDesC aUserId = KNullDesC;

    iPlugin = CImLauncherPlugin::NewL( KTestAppUid );


    // Make a request
    iStatus = KRequestPending;
    SetActive();

    iPlugin->StartApplicationL( iStatus, aSap, aUserId );

    CActiveScheduler::Start();//Launch the Application

    return 0;
    }