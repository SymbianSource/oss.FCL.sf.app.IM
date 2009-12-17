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
* Description:  Implementation of class CPEngPlgSrv
*
*/


//  Include Files
#include "CPEngPlgSrv.h"
#include "CPEngPlgSess.h"
#include "PresenceDebugPrint.h"
#include "TPEngServerParams.h"
#include "CPEngPluginHolder.h"
#include "CPEngPluginInterface2.h"
#include <e32svr.h>
#include <s32file.h>
#include <sysutil.h>
#include <ecom/ecom.h>

// simple wrapper class with array deletion on close
class RImplInfoPtrArray2 : public RImplInfoPtrArray
    {
    public:
        void Close()
            {
            ResetAndDestroy();
            }
    };

// Static data for Capability check configuration
// Two ranges: one for allowed, second for disallowed requests
static const TInt KPEngPluginServerRangeCount = 2;

/**
 * Ranges for the Request values
 * All requests will fall in one range
 */
static const TInt PEngPluginServerRanges[KPEngPluginServerRangeCount] =
    {
    // Range from EPEngPlgShutdownServer to EPEngPlgServerLastRequest
    0,
    // range is from EPEngPlgServerLastRequest request to end
    EPEngPlgServerLastRequest
    };

/**
 * Element indexes for the defined ranges
 * we have only one range and for it is defined only one Element
 */
static const TUint8 KPEngPluginServerElementsIndex[KPEngPluginServerRangeCount] =
    {
    // First element in the element array will be applied for this range
    0,
    // the second element will be used for the second range
    CPolicyServer::ENotSupported
    };

// Policy elements
static const CPolicyServer::TPolicyElement KPEngPluginServerElements[] =
    {
        {
        _INIT_SECURITY_POLICY_C3( ECapabilityReadUserData,
        ECapabilityWriteUserData,
        ECapabilityNetworkServices ),
        CPolicyServer::EFailClient
        }
    };

// and the Policy array is the following
static const CPolicyServer::TPolicy KPEngPluginServerPolicy =
    {
    // The index into Elements,that is used to check a connection attempt
    0,
    // Number of ranges in the iRanges array
    KPEngPluginServerRangeCount,
    // A pointer to an array of ordered ranges of request numbers
    PEngPluginServerRanges,
    // A pointer to an array of TUint8 values specifying
    // the appropriate action to take for each range in iRanges
    KPEngPluginServerElementsIndex,
    // A pointer to an array of distinct policy elements
    KPEngPluginServerElements
    };

// CONSTRUCTION
// Static constructor, leaves pointer to cleanup-stack
CPEngPlgSrv* CPEngPlgSrv::NewLC( const TDesC& aServerName, TInt aPriority )
    {
    CPEngPlgSrv* self = new( ELeave ) CPEngPlgSrv( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aServerName );
    return self;
    }

// Destructor (virtual by CBase)
CPEngPlgSrv::~CPEngPlgSrv()
    {
    iPlugins.ResetAndDestroy();
    REComSession::FinalClose();
    }

// Default constructor, protected
CPEngPlgSrv::CPEngPlgSrv( TInt aPriority )
        : CPolicyServer( aPriority, KPEngPluginServerPolicy ),
        iOnlineState( EPEngPlgSvrStateUnknown )
    {
    }

// Second phase construct
void CPEngPlgSrv::ConstructL( const TDesC& aServerName )
    {
    iServerName.Copy( aServerName );
    StartL( aServerName );
    }


TInt CPEngPlgSrv::ExecuteServerL( TPEngServerParams& aParams )
    {
    __UHEAP_MARK;

    //Renaming must be done as early as possible
    aParams.RenameMainThread( KPEngPluginServerExe );

    TInt res( KErrNone );

    // start scheduler and server
    CActiveScheduler* pA = new( ELeave )CActiveScheduler;
    CleanupStack::PushL( pA );
    CActiveScheduler::Install( pA );

    //If exe server, call RunServerL directly.
    TRAP( res, RunServerL( aParams ) );

    CActiveScheduler::Install( NULL );
    CleanupStack::PopAndDestroy();//pA

    __UHEAP_MARKEND;
    return res;
    }

void CPEngPlgSrv::StopServer()
    {
    CActiveScheduler::Stop(); // CSI: 4 #
    }


void CPEngPlgSrv::RunServerL( TPEngServerParams& aParams )
    {
    PENG_DP( D_PENG_LIT( "CPEngPlgSrv::RunServerL()" ) );

    //One instance of server must be allocated here.
    CPEngPlgSrv* server = CPEngPlgSrv::NewLC( aParams.ServerName(),
                                              CActive::EPriorityStandard );

    aParams.Signal();
    // start fielding requests from clients
    //Thread is ended when CActiveScheduler::Stop is called.
    server->StartServer();

    CleanupStack::PopAndDestroy();//server
    // finished
    }

void CPEngPlgSrv::LoadByTypeL( const TDesC8& aPluginType )
    {
    PENG_DP( D_PENG_LIT( "CPEngPlgSrv::LoadByTypeL" ) );

    RImplInfoPtrArray2 plugins;
    CleanupClosePushL( plugins );

    TEComResolverParams params;
    params.SetDataType( aPluginType );
    params.SetWildcardMatch( EFalse );
    REComSession::ListImplementationsL( KPEngPluginInterfaceUid, params, plugins );

    const TInt count = plugins.Count();

    for ( TInt i( 0 ); i < count; i++ )
        {
        TUid uid = plugins[i]->ImplementationUid();

        CPEngPluginHolder* plug = CPEngPluginHolder::NewLC();
        plug->LoadPluginL( uid );
        iPlugins.AppendL( plug );

        CleanupStack::Pop();//plug
        }

    CleanupStack::PopAndDestroy();  // plugins

    PENG_DP( D_PENG_LIT( "CPEngPlgSrv::LoadByTypeL ends" ) );
    }

void CPEngPlgSrv::UnloadOnlinePluginsL()
    {
    PENG_DP( D_PENG_LIT( "CPEngPlgSrv::UnloadOnlinePluginsL" ) );
    RImplInfoPtrArray2 plugins;
    CleanupClosePushL( plugins );

    TEComResolverParams params;
    params.SetDataType( KPEngOnlineType );
    params.SetWildcardMatch( EFalse );
    REComSession::ListImplementationsL( KPEngPluginInterfaceUid, params, plugins );

    const TInt implCount = plugins.Count();
    for ( TInt ii( implCount - 1 ); ii >= 0; ii-- )
        {
        const TInt loadedCount = iPlugins.Count();
        for ( TInt jj( loadedCount - 1 ); jj >= 0; jj-- )
            {
            if ( plugins[ii]->ImplementationUid() == iPlugins[jj]->Uid() )
                {
                delete iPlugins[ jj ];
                iPlugins.Remove( jj );
                }
            }
        }

    CleanupStack::PopAndDestroy();  // plugins

    PENG_DP( D_PENG_LIT( "CPEngPlgSrv::UnloadOnlinePluginsL ends" ) );
    }

void CPEngPlgSrv::UnloadAllL()
    {
    iPlugins.ResetAndDestroy();
    }

void CPEngPlgSrv::SetStateL( TPEngPlgOnlineState aNewState )
    {
    if ( iOnlineState < EPEngPlgSvrStateOnline &&
         aNewState == EPEngPlgSvrStateOnline )
        {
        // we went online, load online plugins
        LoadByTypeL( KPEngOnlineType );
        }
    if ( aNewState == EPEngPlgSvrStateOffline &&
         iOnlineState == EPEngPlgSvrStateOnline )
        {
        // we went offline, unload and load offline plugins
        UnloadOnlinePluginsL();
        }
    if ( aNewState == EPEngPlgSvrStateOffline &&
         iOnlineState == EPEngPlgSvrStateUnknown )
        {
        // the server was first started and we are offline
        LoadByTypeL( KPEngOfflineType );
        }
    if ( aNewState == EPEngPlgSvrStateUnknown )
        {
        // this shouldn't happen in normal situations
        UnloadAllL();
        }

    iOnlineState = aNewState;
    }

TInt CPEngPlgSrv::PluginCount() const
    {
    return iPlugins.Count();
    }

TInt CPEngPlgSrv::Plugin( TInt aIndex ) const
    {
    return iPlugins[aIndex]->Uid().iUid;
    }

void CPEngPlgSrv::StartServer()
    {
    CActiveScheduler::Start(); // CSI: 3 #
    }

CSession2* CPEngPlgSrv::NewSessionL( const TVersion &aVersion, const RMessage2& /*aMessage*/ ) const
    {
    if ( !User::QueryVersionSupported( aVersion, TVersion( KRequiredVersionMajor, KRequiredVersionMinor, KRequiredVersionBuild ) ) )
        {
        User::Leave( KErrNotSupported );
        }

    return CPEngPlgSess::NewL( const_cast<CPEngPlgSrv&>( *this ) );
    };

//  End of File

