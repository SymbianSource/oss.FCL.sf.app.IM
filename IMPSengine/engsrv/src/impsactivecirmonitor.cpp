/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Im circle Manager.
*
*
*/


// INCLUDE FILES
#include    <e32std.h>
#include    "impsactivecirmonitor.h"
#include    "impsutils.h"
#include    <msgconnmanagerapi.h>


// CONSTANTS

// TCP/IP CIR ping interval in seconds
const TInt KImpsPingInt = 900;

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

//**********************************
// CImpsCirManager
//**********************************
CImpsCirManager::CImpsCirManager(
    MImpsCSPSession& aServer,
    TInt aPriority )
        : CActive( aPriority ),
        iServer( aServer ),
        iState( EImpsCirOffline ),
        iCanceled( EFalse ),
        iManager( NULL ),
        iTCPWatcher( NULL ),
        iUDPWatcher( NULL ),
        iChannels ( 0 ),
        iReqChannels ( 0 )
    {
    // Add this to the scheduler
    CActiveScheduler::Add( this );
    }

// EPOC default constructor can leave.
void CImpsCirManager::ConstructL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: Constructor rel200517.1+" ) );
#endif
    }

// Two-phased constructor.
CImpsCirManager* CImpsCirManager::NewL(
    MImpsCSPSession& aServer,
    TInt aPriority )
    {
    CImpsCirManager* self = new ( ELeave ) CImpsCirManager(
        aServer, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CImpsCirManager::~CImpsCirManager()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: Destructor" ) );
#endif
    Cancel();
    if ( iTCPWatcher )
        {
        delete iTCPWatcher;
        iTCPWatcher = NULL;
        }
    if ( iUDPWatcher )
        {
        iUDPWatcher->Destroy();
        iUDPWatcher = NULL;
        }

    // REcomSession::FinalClose() is not called here but in the engine core
    // just before active acheduler is stopped.

    }  //lint !e1740 iManager is not deleted here, it's only reference

// ---------------------------------------------------------
// CImpsCirManager::OpenChL
// ---------------------------------------------------------
//
void CImpsCirManager::OpenChL(
    TImpsCirChannels aChannels,
    MMsgConnManager& aManager,
    TUint aServerport,
    const TDesC& aServerAddress,
    const TDesC8& aCSPSessionID,
    TUint aUDPPort )
    {
    if  ( ( iState == EImpsCirOnline ) || ( aChannels == 0 ) )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsCirManager: OpenChL ignored" ) );
#endif
        iServer.CirChOpened();
        // Nothing to open
        return;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: OpenChL" ) );
#endif

    iManager = &aManager;
    iReqChannels = aChannels;
    iUDPPort = aUDPPort;

    // Start TCP watcher first if required
    if ( ( aChannels & KImpsCirTcp ) && ~( iChannels & KImpsCirTcp ) )
        {
        DoOpenTCPL( aServerport, aServerAddress,
                    aCSPSessionID );
        // Another watcher started in RunL if needed
        }
    // UPD start here if TCP not required
    else if ( ( aChannels & KImpsCirUdp ) && ~( iChannels & KImpsCirUdp ) )
        {
        DoOpenUDP( iUDPPort );
        }
    }

// ---------------------------------------------------------
// CImpsCirManager::DoOpenTCPL
// ---------------------------------------------------------
//
void CImpsCirManager::DoOpenTCPL(
    TUint aServerport,
    const TDesC& aServerAddress,
    const TDesC8& aCSPSessionID )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: DoOpenTCPL" ) );
#endif
    // If opening fails then this method leaves. TCP is tried to open
    // so CSP session gets information about failure.
    // Start watchers, TCP watcher first
    iTCPWatcher = CImpsTcpCirWatcher::NewL( KImpsPingInt, *this, *iManager );
    iState = EImpsCirOpeningTcp;
    ActivateMe();
    iTCPWatcher->StartTCPWatcherL( iStatus, aServerport, aServerAddress, aCSPSessionID );
    }

// ---------------------------------------------------------
// CImpsCirManager::DoOpenUDP
// ---------------------------------------------------------
//
void CImpsCirManager::DoOpenUDP(
    TUint aUDPPort )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: DoOpenUDP" ) );
#endif
    // UPD watcher registeration
    TInt err = KErrNone;
    TRAP( err, iUDPWatcher = CImpsUdpCirWatcher::NewL() );
    if ( err )
        {
        // UDP open erros are reported to csp session asynchronously via RunL
        DoHandleError( err, EImpsUdpCir );
        }
    TRAP( err, iUDPWatcher->RegisterCirReceiverL( *this, aUDPPort ) );
    if ( err )
        {
        // UDP open erros are reported to csp session asynchronously via RunL
        DoHandleError( err, EImpsUdpCir );
        }
    else
        {
        iChannels = iChannels | KImpsCirUdp;
        }
    }

// ---------------------------------------------------------
// CImpsCirManager::CloseCh
// ---------------------------------------------------------
//
void CImpsCirManager::CloseCh(
    TImpsCirChannels aChannels )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: CloseCh" ) );
#endif

    // If active then have to wait
    Cancel();

    if ( !( aChannels & iChannels ) ||
         iState == EImpsCirOffline ||
         ( !iTCPWatcher && !iUDPWatcher ) )
        {
        // Nothing to close
        return;
        }

    // Channels that required to be closed
    iReqChannels = aChannels;

    // Close TCP
    if ( ( iReqChannels & KImpsCirTcp ) &&
         ( iChannels & KImpsCirTcp ) )
        {
        iTCPWatcher->StopTCPWatcher();
        iChannels = iChannels & ~KImpsCirTcp;
        }
    // Close UDP
    if ( ( iReqChannels & KImpsCirUdp ) &&
         ( iChannels & KImpsCirUdp ) )
        {
        TInt err = KErrNone;
        TRAP( err, iUDPWatcher->UnregisterCirReceiverL( *this ) );
        iChannels = iChannels & ~KImpsCirUdp;
        }

    // Destructor handles the rest of the shut down activies

    iState = EImpsCirOffline;
    }

// ---------------------------------------------------------
// CImpsCirManager::CirReceivedL
// ---------------------------------------------------------
//
void CImpsCirManager::CirReceivedL(
    const TDesC8& aMessage,
    const TImpsSupportedCirTypes aCirType )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: CirReceivedL type=%d" ), aCirType );
#endif

    // notice: This is not very clear how to handle multiple IP-CIR channels
    // Better keep on listening them
    /*
    if ( ( aCirType == EImpsTcpCir ) && ( iChannels & KImpsCirUdp ) )
        {
        // Stop UDP if another CIR channel in use
        iUDPWatcher->StopUDPWatcher();
        }
    else if ( ( aCirType == EImpsUdpCir ) && ( iChannels & KImpsCirTcp ) )
        {
        // Stop TCP if another CIR channel in use
        iTCPWatcher->StopTCPWatcher();
        }
        */

    iServer.CirMessageL( aMessage );

    }

// ---------------------------------------------------------
// CImpsCirManager::HandleErrorL
// ---------------------------------------------------------
//
void CImpsCirManager::HandleErrorL(
    const TInt aReceiverError,
    const TImpsSupportedCirTypes aCirType )
    {
    DoHandleError( aReceiverError, aCirType );
    }

// ---------------------------------------------------------
// CImpsCirManager::DoHandleError
// ---------------------------------------------------------
//
void CImpsCirManager::DoHandleError(
    const TInt aReceiverError,
    const TImpsSupportedCirTypes aCirType )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: DoHandleError %d type=%d" ),
                            aReceiverError, aCirType );
#endif

    // We suppose that this is not called if this is active, but
    // better to check anyway. Then do the following actions.
    // - Yield to ifself with an error code.
    // - In RunL stop channels
    // - Then call iServer.CirChError() that destroys us.

    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        iCanceled = EFalse;
        TRequestStatus* s = &iStatus;
        User::RequestComplete( s, aReceiverError );
        }
    }

// ---------------------------------------------------------
// CImpsCirManager::RunL
// ---------------------------------------------------------
//
void CImpsCirManager::RunL( )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: RunL %d" ), iStatus.Int() );
#endif

    if ( iCanceled )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsCirManager: RunL ignored" ) );
#endif
        iCanceled = EFalse;
        if ( iChannels )
            {
            iState = EImpsCirOnline;
            }
        else
            {
            iState = EImpsCirOffline;
            }
        return;
        }

    // Error handling, common for both open methods and HandleErrorL methods.
    // CSP session entity will call destrcutor, so no need to do else than
    // close watcher
    if ( iStatus.Int() != KErrNone )
        {
        if ( iState == EImpsCirOnline )
            {
            if ( iChannels & KImpsCirTcp )
                {
                iTCPWatcher->StopTCPWatcher();
                }
            if ( iChannels & KImpsCirUdp )
                {
                TInt err = KErrNone;
                TRAP( err, iUDPWatcher->UnregisterCirReceiverL( *this ) );
                }
            iState = EImpsCirOffline;
            }
        iServer.CirChError();
        return;
        }

    // asynchronous TCP CIR opening completed
    if ( iState == EImpsCirOpeningTcp )
        {
        iChannels = iChannels | KImpsCirTcp;
        // if UDP were supported
        if ( ( iReqChannels & KImpsCirUdp ) &&
             !( iChannels & KImpsCirUdp ) )
            {
            // load UDP listener synchronously
            DoOpenUDP( iUDPPort );
            }
        }

    // Everything is OK and opened
    iState = EImpsCirOnline;
    iServer.CirChOpened();
    }

// ---------------------------------------------------------
// CImpsCirManager::DoCancel
// ---------------------------------------------------------
//
void CImpsCirManager::DoCancel()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCirManager: DoCancel" ) );
#endif
    // This just waits IAP connection operation to end.
    iCanceled = ETrue;
    if ( iState == EImpsCirOpeningTcp )
        {
        // This returns KErrCancel if an operation is not ready
        iTCPWatcher->StopTCPWatcher();
        }
    }

// ---------------------------------------------------------
// CImpsCirManager::ActivateMe
// ---------------------------------------------------------
//
TInt CImpsCirManager::ActivateMe(  )
    {
    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        iCanceled = EFalse;
        return KErrNone;
        }
    else
        {
        return KErrNotReady;
        }
    }



//  End of File

