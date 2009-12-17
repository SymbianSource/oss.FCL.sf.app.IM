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
* Description:  Generic thread safe server starter.
*
*/


//  INCLUDE FILES
#include "PEngServerStarter.h"
#include "PEngServerStarterDefs.h"
#include "TPEngServerParams.h"
#include "PresenceDebugPrint.h"

#include <E32STD.H>
#include <f32file.h>
#include <data_caging_path_literals.hrh>


_LIT( KPEngPathDelimiter, "\\" );


/**
 * RSessionBase accessor to give to the PEngServerStarter
 * access to RSessionBase::CreateSession().
 *
 * @since 2.6
 */
class RPEngSessionBaseAccessor : public RSessionBase
    {
    public: // Constructor

        /**
         * C++ constructor.
         *
         * @since 2.6
         */
        inline RPEngSessionBaseAccessor()
            {
            }

    public: // New functions


        /**
         * Calls the RSessionBase::CreateSession().
         *
         * @since 2.6
         * @param aServer See RSessionBase::CreateSession().
         * @param aVersion See RSessionBase::CreateSession().
         * @param aAsyncMessageSlots See RSessionBase::CreateSession().
         * @return See RSessionBase::CreateSession().
         */
        inline TInt CreateSession( const TDesC& aServer,
                                   const TVersion& aVersion,
                                   TInt aAsyncMessageSlots )
            {
            return RSessionBase::CreateSession( aServer,
                                                aVersion,
                                                aAsyncMessageSlots );
            }
    };





// ============================= LOCAL FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// GenerateFullServerExe()
// Local helper.
// Generates full server executable name and path.
//
//
//
// Param: aServerName - A plain server name for which to generate the fullname.
// Param: aFullServerPath - On the function return contains full
//        server path and name.
// -----------------------------------------------------------------------------
//
void GenerateFullServerExe( const TDesC& aServerName, TFileName& aFullServerPath )
    {
        {
        //Get drive (C:)
        TFileName dllPath;
        Dll::FileName( dllPath );
        aFullServerPath.Copy( TParsePtrC( dllPath ).Drive() );
        }


    //Get path (\Xxxx\Xxxx)
    aFullServerPath.Append( KDC_PROGRAMS_DIR );

    //Fix the path delimeter if missing from path
    TPtrC pathDelim = aFullServerPath.Right( KPEngPathDelimiter().Length() );
    if ( pathDelim != KPEngPathDelimiter )
        {
        aFullServerPath.Append( KPEngPathDelimiter );
        }


    //Server name + extension (aServer.EXT)
    aFullServerPath.Append( aServerName );
    aFullServerPath.Append( KExtDelimiter );

    aFullServerPath.Append( KServerNameExtExe );

    PENG_DP( D_PENG_LIT( "GenerateFullServerExe() [%S]" ), &aFullServerPath );
    }




// -----------------------------------------------------------------------------
// ProcessRunning()
// Local helper.
// Checks is there server process started from the given
// exe file (exe or dll).
//
//
// Param: aFullServerExe - Server exe which running status to check.
//        NOTE!! In THUMB the process name is the name portion of the
//        filename from which the executable is loaded. In WINS the
//        thread name is similarly the name portion of the dll filename
//        (See CreateWinsThread() below) ==> thus here must be used
//        also the exe/dll as parameter.
//
// Return: KErrNotFound - no matching running process found.
//         KErrNone - one matching running process found.
//         KErrGeneral - more than one running process found.
// -----------------------------------------------------------------------------
//
TInt ProcessRunning( const TDesC& aFullServerExe )
    {
    TFindProcess find;
    RProcess process;


    //Initialize the find
    TFullName name( TParsePtrC( aFullServerExe ).Name() );
    name.Append( KMatchAny );
    find.Find( name );


    PENG_DP( D_PENG_LIT( "ProcessRunning() [%S]" ), &name );

    //loop through all of matching processes
    TInt runningCount = 0;
    while ( find.Next( name ) == KErrNone )
        {
        TInt error = process.Open( find );
        if ( error != KErrNone )
            {
            //if can't open, the process is .. not .. valid
            PENG_DP( D_PENG_LIT( "ProcessRunning() - Couldn't open process [%S], error[%d]" ), &name, error );
            continue;
            }

        TExitType exitType = process.ExitType();
        process.Close();

        PENG_DP( D_PENG_LIT( "ProcessRunning() - Server process %d found [%S], ExitType[%d] ==> Running[%d]" ),
                 runningCount, &name, exitType, ( exitType == EExitPending ) );

        //check if the processes is running
        if ( exitType == EExitPending )
            {
            runningCount++;
            }
        }


    PENG_DP( D_PENG_LIT( "ProcessRunning() [%d] matches found" ), runningCount );
    switch ( runningCount )
        {
        case 0: //No server running
            {
            return KErrNotFound;
            }

        case 1: //One server running
            {
            return KErrNone;
            }

        default:
            {
            //More than one server instance running
            return KErrGeneral;
            }
        }
    }


// -----------------------------------------------------------------------------
// DoLaunchServer()
// Local helper.
// Launches the server process and waits it startup.
//
// Param: aFullServerExe - The dll / exe from which to launch the server process.
//        aServerName - The server name to identify the server
//        aParam1 & 2 - Client given parameters to give to created process.
//
// Return: System standard error code.
//
// -----------------------------------------------------------------------------
//
TInt DoLaunchServer( const TDesC& aFullServerExe,
                     const TDesC& aServerName,
                     TInt aParam1,
                     TInt aParam2 )
    {
    PENG_DP( D_PENG_LIT( "DoLaunchServer() [%S] as [%S]" ), &aFullServerExe, &aServerName );

    TInt error( KErrNone );
    TPEngServerParams startParams( aServerName, aParam1, aParam2 );


    //Create thread / process according the platform
    RProcess process;
    error = process.Create( aFullServerExe, startParams.AsCommandLine() );


    if ( error != KErrNone )
        {
        return error;
        }


    //and execute the process and wait it's startup
    TRequestStatus rendezvousStatus;
    process.Rendezvous( rendezvousStatus );

    PENG_DP( D_PENG_LIT( "DoLaunchServer() - Waiting for startup or die..." ) );
    process.Resume();
    User::WaitForRequest( rendezvousStatus );               // CSI: 94 #
    error = rendezvousStatus.Int();

    if ( ( error == KErrNone ) &&
         ( process.ExitType() == EExitPending ) )
        {
        //Startup signalled from process ==> server successfully started
        PENG_DP( D_PENG_LIT( "DoLaunchServer() - Server started" ) );
        }

    else
        {
        //Something failed in server startup
        TExitCategoryName exitCategory = KNullDesC();
        exitCategory = process.ExitCategory();
        PENG_DP( D_PENG_LIT( "DoLaunchServer() - Startup failed: ExitReason[%S, %d], Error[%d]" ),
                 &exitCategory, process.ExitReason(), error );

        if ( error == KErrNone )
            {
            error = KErrServerTerminated;
            }
        }

    process.Close();

    return error;
    }




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngServerStarter::LaunchServer()
// Public member function for clients to launch the server process.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngServerStarter::LaunchServer( const TDesC& aServerExeBaseName,
                                               const TDesC& aServerName,
                                               TInt aParam1,
                                               TInt aParam2 )
    {
    PENG_DP( D_PENG_LIT( "PEngServerStarter::LaunchServer() [%S]" ), &aServerExeBaseName );

    RMutex launchMutex;

        {
        // Dynamic mutex name used to allow code share.
        TName launchMutexName( TParsePtrC( aServerExeBaseName ).Name() );
        launchMutexName.Append( KPEngLaunchMutexNameExtension );

        // Open or Create mutex to serialize to access to server startup code.
        // Way below is race condition safe.
        TInt error( KErrNotFound );
        while ( error == KErrNotFound )
            {
            error = launchMutex.CreateGlobal( launchMutexName );
            if ( error != KErrAlreadyExists )
                {
                break;
                }
            error = launchMutex.OpenGlobal( launchMutexName );
            }

        if ( error != KErrNone )
            {
            return error;
            }
        }


    //Determine the drive for executable
    TFileName fullServerExe;
    GenerateFullServerExe( aServerExeBaseName, fullServerExe );

    TInt error;
    launchMutex.Wait();
        {
        //Serialized section
        error = ProcessRunning( fullServerExe );
        if ( error == KErrNotFound )
            {
            //server not running
            error = DoLaunchServer( fullServerExe,
                                    aServerName,
                                    aParam1,
                                    aParam2 );
            }
        }

    launchMutex.Signal();
    launchMutex.Close();

    PENG_DP( D_PENG_LIT( "PEngServerStarter::LaunchServer() done[%d]" ), error );
    return error;
    }



// -----------------------------------------------------------------------------
// PEngServerStarter::ConnectServer()
// Public member function for clients to connect to server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngServerStarter::ConnectServer( RSessionBase& aSession,
                                                const TDesC& aServerName,
                                                const TVersion& aVersion,
                                                TInt aAsyncMessageSlots,
                                                const TDesC& aServerExeBaseName,
                                                TInt aParam1,
                                                TInt aParam2 )
    {
    if ( aSession.Handle() != KNullHandle )
        {
        return KErrInUse;
        }

    TInt err = KErrGeneral;
    TInt wait = KPEngSrvConnRetryWait;
    for ( TInt tries = 0 ; tries < KPEngSrvConnTries ; tries++ )
        {
        RPEngSessionBaseAccessor acc;
        err = acc.CreateSession( aServerName, aVersion, aAsyncMessageSlots );
        aSession = acc; //session ownership is now on client

        if ( err == KErrNone ||
             ( err != KErrNotFound && err != KErrServerTerminated ) )
            {
            break; // connected ok or something else than missing server
            }

        // if server not found, try to connect more times
        if ( ( err == KErrNotFound ) && ( tries < 4 ) )
            {
            continue;
            }

        err = LaunchServer( aServerExeBaseName, aServerName, aParam1, aParam2 );
        if ( err != KErrNone )
            {
            break; //launch failed
            }


        if ( tries > 0 )
            {
            //2nd or subsequent try - qive some time for server to startup
            PENG_DP( D_PENG_LIT( "PEngServerStarter::ConnectServer() - giving time for startup [%d]" ), wait );
            User::After( wait );    // CSI: 92 #
            wait = wait + wait;     //On next round wait longer
            }
        }


    PENG_DP( D_PENG_LIT( "PEngServerStarter::ConnectServer( %d )" ), err );
    return err;
    }



// End of file

