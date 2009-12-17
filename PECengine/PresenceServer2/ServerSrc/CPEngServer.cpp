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
* Description:  Presence Server class
*
*/


//  Include Files
#include    <e32svr.h>
#include    <ecom/ecom.h>

#include    "CPEngServer.h"
#include    "CPEngSession.h"
#include    "CPEngSubSession.h"
#include    "TPEngServerCommon.h"
#include    "TPEngServerParams.h"
#include    "CPEngCSPSessManager.h"
#include    "CPengSessionSlotId.h"
#include    "CPEngActiveScheduler.h"

// Debug Prints
#include    "PresenceDebugPrint.h"

// CONSTANTS

// Static data for Capability check configuration
static const TInt KPEngServerRangeCount = 2;

/**
 * Ranges for the Request values
 * All requests will fall in one range
 */
static const TInt PEngServerRanges[ KPEngServerRangeCount ] =
    {
    0,  // Range from EMainSessShutdownServer to ESubSessFetchUpdateRqstResult
    EHighestServerRequest // range is from EHighestServerRequest request to KMaxInt
    };

/**
 * Element indexes for the defined ranges
 * we have only one range and for it is defined only one Element
 */
static const TUint8 KPEngServerElementsIndex[KPEngServerRangeCount] =
    {
    0, // First element in the element array will be applied for this range
    CPolicyServer::ENotSupported
    };

// Policy elements
static const CPolicyServer::TPolicyElement KPEngServerElements[] =
    {
        {
        _INIT_SECURITY_POLICY_C3( ECapabilityReadUserData,
        ECapabilityWriteUserData,
        ECapabilityNetworkServices ),
        CPolicyServer::EFailClient
        }
    };

static const CPolicyServer::TPolicy KPEngServerPolicy =
    {
    // The index into Elements,that is used to check a connection attempt

    0,
    // Number of ranges in the iRanges array
    KPEngServerRangeCount,

    // A pointer to an array of ordered ranges of request numbers
    PEngServerRanges,

    // A pointer to an array of TUint8 values specifying
    // the appropriate action to take for each range in iRanges
    KPEngServerElementsIndex,

    // A pointer to an array of distinct policy elements
    KPEngServerElements
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngServer::CPEngServer
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngServer::CPEngServer(
    TInt aPriority )
        : CPolicyServer( aPriority, KPEngServerPolicy ),
        iRunning( EFalse ),
        iSessionsManagers( 1 ) // usually only one session
    {
    }

// -----------------------------------------------------------------------------
// CPEngServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngServer::ConstructL()
    {
    StartL( KServerName );
    User::LeaveIfError( iStoreAdmin.Connect() );
    iContainer = CObjectCon::NewL();
    // we need to assign some unique ID to the container, to own the object
    // stored in it
    iContainer->iUniqueID = 1;
    }

// -----------------------------------------------------------------------------
// CPEngServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngServer* CPEngServer::NewL(
    TInt aPriority )
    {
    CPEngServer* self = CPEngServer::NewLC( aPriority );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CPEngServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngServer* CPEngServer::NewLC(
    TInt aPriority )
    {
    CPEngServer* self = new( ELeave ) CPEngServer( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
CPEngServer::~CPEngServer()
    {
    PENG_DP( D_PENG_LIT( "CPEngServer::~CPEngServer()" ) );
    delete iContainer;
    iStoreAdmin.Close();
    iSessionsManagers.ResetAndDestroy();
    REComSession::FinalClose();
    }


// =============================================================================
// =============== Functions from base class ===================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngServer::ExecuteServerL()
// -----------------------------------------------------------------------------
//
TInt CPEngServer::ExecuteServerL(
    TPEngServerParams& aParams )
    {
    //Renaming must be done as early as possible
    aParams.RenameMainThread( KServerName );

    TInt res( KErrNone );

    // start scheduler and server
    CPEngActiveScheduler* pA = new( ELeave )CPEngActiveScheduler;
    CleanupStack::PushL( pA );
    CPEngActiveScheduler::Install( pA );

    //If exe server, call RunServerL directly.
    TRAP( res, RunServerL( aParams ) );

    CPEngActiveScheduler::Install( NULL );
    CleanupStack::PopAndDestroy();//pA
    return res;
    }


// -----------------------------------------------------------------------------
// CPEngServer::StopServer()
// -----------------------------------------------------------------------------
//
void CPEngServer::StopServer()
    {
    if ( iRunning )
        {
        CPEngActiveScheduler::Stop();
        }
    iRunning = EFalse;
    }

// -----------------------------------------------------------------------------
// CPEngServer::RunServerL()
// -----------------------------------------------------------------------------
//
void CPEngServer::RunServerL(
    TPEngServerParams& aParams )
    {
    PENG_DP( D_PENG_LIT( "CPEngServer::RunServerL()" ) );

    //One instance of server must be allocated here.
    CPEngServer* server = CPEngServer::NewLC( KServerPriority );

    //must die if can't signal client
    aParams.Signal( );
    // start fielding requests from clients
    //Thread is ended when CPEngActiveScheduler::Stop is called.
    server->StartServer();


    CleanupStack::PopAndDestroy();//server
    // finished
    PENG_DP( D_PENG_LIT( "CPEngServer::RunServerL() - End" ) );
    }

// =============================================================================
// =============== Functions from the MPEngServer class =================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngServer::SessionCreated()
// -----------------------------------------------------------------------------
//
void CPEngServer::SessionCreated()
    {
    iSessCount++;
    }

// -----------------------------------------------------------------------------
// CPEngServer::SessionDied()
// -----------------------------------------------------------------------------
//
void CPEngServer::SessionDied()
    {
    iSessCount--;
    CheckServerAccess();
    }

// -----------------------------------------------------------------------------
// CPEngServer::AddSubSessionL()
// -----------------------------------------------------------------------------
//
void CPEngServer::AddSubSessionL(
    CPEngSubSession& aSubSession )
    {
    iContainer->AddL( &aSubSession );
    }


// -----------------------------------------------------------------------------
// CPEngServer::RemoveSubSessionL()
// -----------------------------------------------------------------------------
//
void CPEngServer::RemoveSubSessionL(
    CPEngSubSession& aSubSession )
    {
    iContainer->Remove( &aSubSession );
    }


// -----------------------------------------------------------------------------
// CPEngServer::CSPSessionManagerL()
// -----------------------------------------------------------------------------
//
CPEngCSPSessManager* CPEngServer::CSPSessionManagerLC(
    CPEngSessionSlotId& aSessionId )
    {
    // try to find session slot manager
    TInt count( iSessionsManagers.Count() );
    for ( TInt x( 0 ) ; x < count  ; ++x )
        {
        if ( KErrNone == iSessionsManagers[ x ]->SessionId().Match(
                 aSessionId ) )
            {
            // session manager exists, return it
            CPEngCSPSessManager* sessManager = iSessionsManagers[ x ];
            sessManager->Open(); // CSI: 65 #
            CleanupClosePushL( *sessManager );
            return sessManager;
            }
        }
    // Session Manager does not exist, create it
    CPEngCSPSessManager* sessManager = CPEngCSPSessManager::NewLC(
                                           *this,
                                           aSessionId,
                                           iStoreAdmin );
    iSessionsManagers.AppendL( sessManager );
    return sessManager;
    }


// -----------------------------------------------------------------------------
// CPEngServer::RemoveStateMachine()
// -----------------------------------------------------------------------------
//
void CPEngServer::RemoveCSPSessManager(
    CPEngCSPSessManager* aManager )
    {
    TInt index(  iSessionsManagers.Find( aManager ) );
    if ( index != KErrNotFound )
        {
        iSessionsManagers.Remove( index );
        }
    CheckServerAccess();
    }

// -----------------------------------------------------------------------------
// CPEngServer::SessionOpened()
// -----------------------------------------------------------------------------
//
void CPEngServer::SessionOpened()
    {
    iOpenedSessions++;
    }

// -----------------------------------------------------------------------------
// CPEngServer::SessionClosed()
// -----------------------------------------------------------------------------
//
void CPEngServer::SessionClosed()
    {
    iOpenedSessions--;
    }

// =============================================================================
// =============== New private Functions of the main class =====================
// =============================================================================


// =============================================================================
// =============== Functions from the CServer2 class ===========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngServer::NewSessionL()
// -----------------------------------------------------------------------------
//
CSession2* CPEngServer::NewSessionL(
    const TVersion &aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
    if ( !User::QueryVersionSupported( aVersion,
                                       TVersion( KClientVersionMajor,
                                                 KClientVersionMinor,
                                                 KClientVersionBuild ) ) )
        {
        User::Leave( KErrNotSupported );
        }

    return CPEngSession::NewL( const_cast<CPEngServer&>( *this ) );
    }

// =============================================================================
// =============== New Private methods from class ==============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngServer::StartServer()
// Start Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngServer::StartServer()
    {
    iRunning = ETrue;
    PENG_DP( D_PENG_LIT( "PresenceServer::Started" ) );
    CPEngActiveScheduler::Start();
    PENG_DP( D_PENG_LIT( "PresenceServer::Stopped" ) );
    }

// -----------------------------------------------------------------------------
// CPEngServer::CheckServerAccess()
// Check server access count
// it also check is there is some active session
// if server is no more need, it is stoped
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngServer::CheckServerAccess()
    {
    if ( ( iSessCount + iSessionsManagers.Count() ) == 0 )
        {
        // stop server
        StopServer();
        }
    }

//  End of File
