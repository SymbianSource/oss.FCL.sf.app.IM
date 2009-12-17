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
* Description: Client application plugin loader.
*
*/



// INCLUDE FILES
#include    <imlauncherplugin.h>
#include    "imlauncher.h"
#include    "logutils.h"


// Default start wait time in sec
static const TInt KDefaultTimeOut = 60;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImLauncher::CImLauncher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImLauncher::CImLauncher()
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
void CImLauncher::ConstructL()
    {
    iTimer = CImShutDownTimer::NewL( this, EPriorityStandard );
    }

// -----------------------------------------------------------------------------
// CImLauncher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImLauncher* CImLauncher::NewL()
    {
    CImLauncher* self = new( ELeave ) CImLauncher;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CImLauncher::~CImLauncher()
    {
    Cancel();
    delete iTimer;
    if ( iPlugin ) // extra security check
        {
        delete iPlugin;
        }
    }


// -----------------------------------------------------------------------------
// CImLauncher::Panic
// Panics the thread
// -----------------------------------------------------------------------------
//
void CImLauncher::Panic( TLauncherPanic aPanicCode )
    {
    User::Panic( KLauncherPanic, aPanicCode );
    }

// -----------------------------------------------------------------------------
// CImLauncher::RunL
// -----------------------------------------------------------------------------
//
void CImLauncher::RunL()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "Application start status: %d" ), iStatus.Int() );
#endif
    iReturnValue = iStatus.Int();
    // We're finished
    iTimer->Cancel();
    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CImLauncher::DoCancel
// -----------------------------------------------------------------------------
//
void CImLauncher::DoCancel()
    {
    if ( iPlugin ) // extra security check
        {
        iPlugin->CancelStartApplication();
        }
    iTimer->Cancel();
    }

// -----------------------------------------------------------------------------
// CImLauncher::DoShutDown
// -----------------------------------------------------------------------------
//
void CImLauncher::TimedOut()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "Application starting timed out" ) );
#endif
    Cancel();
    iReturnValue = KErrTimedOut;

    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CImLauncher::RunError
// Handles the RunL leave errors
// -----------------------------------------------------------------------------
//
TInt CImLauncher::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CImLauncher::StartL()
// Loads the application launcher plugin and waits for the completition
// -----------------------------------------------------------------------------
void CImLauncher::StartL( TImpsLaunchParams& aThreadParams )
    {

    /*
     * Convert the 16bit descriptor to 8bit.
     * This should not be a problem as ClientID should not contain
     * non ASCII chars.
     * If there will be a problem the ECom default resolver should be changed!
     */

    TBuf8<KImpsMaxClientId> applicationId;
    if ( aThreadParams.iApplicationId.Length() > KImpsMaxClientId )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        applicationId.Copy( aThreadParams.iApplicationId );
        }

#ifdef _DEBUG
    // NOTICE: this accepts only 16bit descriptor to log :-(
    CImApiLogger::Log( _L( "Loading 3rd party launcher plugin: %S" ), &aThreadParams.iApplicationId );
#endif
    // ECom loading
    iPlugin = CImLauncherPlugin::NewL( applicationId );

#ifdef _DEBUG
    CImApiLogger::Log( _L( "Loading successfull" ) );
#endif

    // Make a request
    iStatus = KRequestPending;
    SetActive();
    iPlugin->StartApplicationL( iStatus, aThreadParams.iSAP, aThreadParams.iUserId );


#ifdef _DEBUG
    CImApiLogger::Log( _L( "Timer started: %d sec" ), KDefaultTimeOut );
#endif
    iTimer->After( KDefaultTimeOut * 1000000 );

    CActiveScheduler::Start();

    // if the request failed return the fail code to thread starter
    User::LeaveIfError( iReturnValue );

    delete iPlugin;
    iPlugin = NULL;

    }

// -----------------------------------------------------------------------------
// ThreadFunction()
// Static function. Installs the CleanupsStack and ActiveScheduler
// then initiates the application launching.
// -----------------------------------------------------------------------------
TInt CImLauncher::ThreadFunction( TImpsLaunchParams& aThreadParams )
    {

#ifdef _DEBUG
    CImApiLogger::Log( _L( "ThreadFunction start" ) );
    __UHEAP_MARK;
#endif

    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( cleanupStack == NULL )
        {
        Panic( ECreateTrapCleanup );
        }

    // Construct active scheduler
    CActiveScheduler* activeScheduler = new CActiveScheduler;
    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    CImLauncher* launcher = NULL;
    TRAPD( err,  launcher = CImLauncher::NewL() );
    if ( err == KErrNone )
        {
        TRAP( err, launcher->StartL( aThreadParams ) );
        }

    delete launcher;
    delete activeScheduler;
    delete cleanupStack;
    cleanupStack = NULL;
    REComSession::FinalClose();

#ifdef _DEBUG
    CImApiLogger::Log( _L( "ThreadFunction end %d" ), err );
    __UHEAP_MARKEND;
    CImApiLogger::Log( _L( "MEM TEST OK" ) );
#endif

    return err;
    }


/*
 * Main function
 */
TInt E32Main()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "EXE Started" ) );
#endif
//
    TBuf < sizeof( TImpsLaunchParams ) > paramsDes;

    User::CommandLine( paramsDes );

    TImpsLaunchParams params;
    params.Set( paramsDes );

    return CImLauncher::ThreadFunction( params );
    }


//**********************************
// CImShutDownTimer
//**********************************

// -----------------------------------------------------------------------------
// CImShutDownTimer::CImShutDownTimer()
// Constructor
// -----------------------------------------------------------------------------
CImShutDownTimer::CImShutDownTimer( CImLauncher* aLauncher, TInt aPriority )
        : CTimer( aPriority ),
        iLauncher( aLauncher )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CImShutDownTimer::ConstructL()
// 2nd phase Constructor
// -----------------------------------------------------------------------------
void CImShutDownTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CImShutDownTimer::NewL()
// Factory function
// -----------------------------------------------------------------------------
CImShutDownTimer* CImShutDownTimer::NewL( CImLauncher* aLauncher, TInt aPriority )
    {
    CImShutDownTimer* self = new( ELeave ) CImShutDownTimer( aLauncher, aPriority );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CImShutDownTimer::~CImShutDownTimer()
// Destructor
// -----------------------------------------------------------------------------
CImShutDownTimer::~CImShutDownTimer()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CImShutDownTimer::RunL()
// Signals the timeout to the timer user
// -----------------------------------------------------------------------------
void CImShutDownTimer::RunL()
    {
    iLauncher->TimedOut();
    }

//  End of File
