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
* Description:  plugin implementation for imlauncher
*
*/


#include "imlaunchertestplugin.h"
#include "apacmdln.h"
#include "apgcli.h"
#include <flogger.h>
#include    <aknpopup.h>
#include	<aknnotedialog.h>

#include <e32base.h>
#include <ecom/implementationproxy.h>

#include <e32std.h>
#include <apacmdln.h>


//#include <testdummyapplication.h>

//forward class declaration
class CApaCommandLine;

//#define LDUMMYNAME_APP_EXE  "Launcher_Console.exe"

#include <flogger.h>

// CONSTANTS
_LIT( KDebugOutputDir, "DummyApp" );
_LIT( KDebugOutputFileName, "DummyApp.log" );
const TInt KMaxLogLineLength = 250;

_LIT( KExecutableName, "timertool.exe" );

//#define KIMLAUNCHERINTERFACEUID_TMP             0x101FB0CC

/** DLL Entry point */
TInt E32Dll( /* TDllReason aReason*/ )
    {
    return( KErrNone );
    }

const TImplementationProxy ImplementationTable[] =
    {
    //IMPLEMENTATION_PROXY_ENTRY(KIMLAUNCHERINTERFACEUID_TMP, CTestPlugin::NewL )
    //IMPLEMENTATION_PROXY_ENTRY(KImLauncherPluginUid, CTestPlugin::NewL )
    //Anumeha
    //IMPLEMENTATION_PROXY_ENTRY({0x101FB0CC},CTestPlugin::NewL )

    //IMPLEMENTATION_PROXY_ENTRY({0x101FB0CD}, CTestPlugin::NewL)
    IMPLEMENTATION_PROXY_ENTRY( {0x101FB0CF}, CTestPlugin::NewL )
    //should match the implementation Uid in the RSS file.
    };


// ---------------------------------------------------------------------------
// Exported function to return the implementation proxy table
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// Construction and destruction functions

CTestPlugin* CTestPlugin::NewL()
    {
    CTestPlugin* self = new ( ELeave ) CTestPlugin;

    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop();
    return self;
    }

CTestPlugin::CTestPlugin()
    {
    // See ConstructL() for initialisation completion.
    }

void CTestPlugin::ConstructL()
    {
//nothing to do, no data to initialize
    }



// Implementation of StartApplicationL
void CTestPlugin::StartApplicationL( TRequestStatus& aStatus, const TDesC& aSap,
                                     const TDesC& aUserId )
    {

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "StartApplicationL called" ) );
    buffer.Append( _L( "] " ) );

    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );


    //This picks the timertool.exe from the Z: drive.
    //_LIT (SERVER, "\\sys\\bin\\timertool.exe"); -

    //Try picking the timertool.exe from the C:drive.
    _LIT ( SERVER, "C:\\sys\\bin\\timertool.exe" );

    RProcess p;
    TInt r = p.Create( SERVER, KNullDesC );
    if ( r != KErrNone )
        {
        TBuf< KMaxLogLineLength > buffer;
        buffer.Append( _L( "[" ) );             // CSI: 78 #
        buffer.Append( _L( "error creating timertool.exe" ) );
        buffer.Append( _L( "] " ) );

        RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );


        User::RequestComplete( iStatus, r );
        }
    else
        {
        TBuf< KMaxLogLineLength > buffer;
        buffer.Append( _L( "[" ) );             // CSI: 78 #
        buffer.Append( _L( "timertool.exe created successfully" ) );
        buffer.Append( _L( "] " ) );

        RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );


        iStatus = &aStatus;

        TRequestStatus stat;
        p.Rendezvous( stat );


        if ( stat != KRequestPending )
            p.Kill( 0 );		// abort startup
        else
            p.Resume();	// logon OK - start the server


        User::WaitForRequest( stat );		// wait for start or death

        // Check the exit type.
        // We can't use the 'exit reason' because if the server panicked this
        // is set to the panic 'reason' (which may be '0' and cannot thus be distinguished
        // from KErrNone)

        r = p.ExitType();

        if ( EExitPanic == r )
            r = KErrGeneral;
        else
            r = stat.Int();


        p.Close(); //closes the process handle and not the process.

        User::RequestComplete( iStatus, r );


        }


    /*
    TRequestStatus start;
    TServerStart starter(start);

    #ifdef __WINS__ // On the Windows emulator the process is a DLL
    RLibrary lib;
    lib.Load(KBinaryName, BinaryUid);
    TLibraryFunction export1 = lib.Lookup(1);
    TThreadFunction threadFunction = reinterpret_cast (export1());
    TName name(KBinaryName);
    // Randomness ensures a unique thread name
    name.AppendNum(Math::Random(), EHex);

    // Now create the server thread
    const TInt KMinHeapSize=0x1000;
    const TInt KMaxHeapSize=0x1000000;
    RThread thread;
    thread.Create(name, threadFunction, KDefaultStackSize, &starter, &lib, NULL, KMinHeapSize, KMaxHeapSize, EOwnerProcess);
    lib.Close();
    #else // Phone hardware
    RProcess process;
    process.Create(KBinaryName,starter.AsCommand(), BinaryUid);
    #endif

    */

    }

void CTestPlugin::CancelStartApplication()
    {
    //Format the log line
    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "Cancel is called from plugin" ) );
    buffer.Append( _L( "] " ) );            // CSI: 78 #

    //Log goes to file
    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    /* First, you need to find the process
    On Symbian 9, process name is in the following format:

    <name>[<UID3>]<instance number>where:
    <name> - the name of executable or the name of Console - so remember it can be changed;

    <UID3> - UID3 of executable, in lowercase HEX, 8 digits, always the same;

    <instance number> - instance number, 4 digits, starting from 0001.

    For example name of kernel process is: ekern.exe[100041af]0001 */

    TFindProcess processFinder( _L( "*[101fb0ce]*" ) ); // by UID3

    TFullName result;

    RProcess processHandle;
    while ( processFinder.Next( result ) == KErrNone )
        {
        TInt err;
        TRAP ( err, processHandle.Open ( processFinder, EOwnerThread ) );

        //Kernel opened the process successfully
        //Call Terminate on that process,to check the Cancel API.

        //Calling the processHandle.Terminate.

        if ( err == KErrNone )
            {

            TBuf< KMaxLogLineLength > buffer;
            buffer.Append( _L( "[" ) );             // CSI: 78 #
            buffer.Append( _L( "got the valid process handle, will call terminate on the process now" ) );
            buffer.Append( _L( "] " ) );            // CSI: 78 #

            //Log goes to file
            RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

            processHandle.Terminate( KErrCancel );
            processHandle.Close();
            }

        }

    }

CTestPlugin::~CTestPlugin()
    {

    }
