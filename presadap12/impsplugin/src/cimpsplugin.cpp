/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS Protocol implementation for Presence Framework
*
*/


#include <badesca.h>
#include <ecom/implementationproxy.h>

#include <ximpserviceinfo.h>
#include <ximpbase.h>
#include <ximpidentity.h>
#include <presenceinfo.h>
#include <ximpprotocolconnectionhost.h>
#include <FeatMgr.h>
#include <bldvariant.hrh>
#include "CImpsPlugin.h"
#include "ImpsDebugPrint.h"
#include "CImpsConnection.h"
#include "impsplugin12uid.h"
#include "ximpcontextclientinfo.h"

_LIT( KImpsPlugin, "CImpsPlugin" );

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Key value pair table to identify correct constructor
// function for the requested interface.
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImps12PluginAdaptationUid3, CImpsPlugin::NewL )
    };


// ---------------------------------------------------------------------------
// Exported function to return the implementation proxy table
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    IMPS_DP( D_IMPS_LIT( "ImplementationGroupProxy() Start" ) );

    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    IMPS_DP( D_IMPS_LIT( "ImplementationGroupProxy() End" ) );

    return ImplementationTable;
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::CImpsPlugin()
// ---------------------------------------------------------------------------
//
CImpsPlugin::CImpsPlugin()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::CImpsPlugin() Start-End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::ConstructL()
// ---------------------------------------------------------------------------
//
void CImpsPlugin::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::ConstructL() Start-End" ) );

    FeatureManager::InitializeLibL();

    if ( FeatureManager::FeatureSupported( KFeatureIdPresence ) &&
         FeatureManager::FeatureSupported( KFeatureIdPresenceFramework ) &&
         FeatureManager::FeatureSupported( KFeatureIdIm )  )
        {
        //construct the plugin
        }
    else
        {
        IMPS_DP( D_IMPS_LIT( "CImpsPlugin::ConstructL() Feature Not Supported" ) );
        User::Leave( KErrNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::ConstructL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::NewLC()
// ---------------------------------------------------------------------------
//
CImpsPlugin* CImpsPlugin::NewLC()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::NewLC() Start" ) );

    CImpsPlugin* self = CImpsPlugin::NewL();
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::NewLC() End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::NewL()
// ---------------------------------------------------------------------------
//
CImpsPlugin* CImpsPlugin::NewL()
    {
    IMPS_DP( D_IMPS_LIT( "=============================START=========================================" ) );
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::NewL() Start" ) );

    CImpsPlugin* self = new( ELeave ) CImpsPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::NewL() End :Address of %d" ), &self );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::CImpsPlugin()
// ---------------------------------------------------------------------------
//
CImpsPlugin::~CImpsPlugin()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::~CImpsPlugin() Start" ) );
    FeatureManager::UnInitializeLib();

    iConnections.ResetAndDestroy();

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::~CImpsPlugin() End" ) );
    IMPS_DP( D_IMPS_LIT( "=============================END=========================================" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::PrimeHost()
// ---------------------------------------------------------------------------
//
void CImpsPlugin::PrimeHost( MXIMPProtocolPluginHost& aHost )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::PrimeHost() Start" ) );

    iHost = &aHost;

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::PrimeHost() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::AcquireConnectionL()
// ---------------------------------------------------------------------------
//
MXIMPProtocolConnection& CImpsPlugin::AcquireConnectionL(
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& aClientCtxInfo )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::AcquireConnectionL() Start" ) );

    CImpsConnection* connection = CImpsConnection::NewL( aServiceInfo, aClientCtxInfo );

    IMPS_DP( D_IMPS_LIT( " CImpsPlugin::AcquireConnectionL:service address::%S" ), &( aServiceInfo.ServiceAddress() ) );
    IMPS_DP( D_IMPS_LIT( " CImpsPlugin::AcquireConnectionL:user ID::%S" ), &( aServiceInfo.UserId() ) );
    IMPS_DP( D_IMPS_LIT( " CImpsPlugin::AcquireConnectionL:user password::%S" ), &( aServiceInfo.Password() ) );
    IMPS_DP( D_IMPS_LIT( " CImpsPlugin::AcquireConnectionL:Client ID::%S" ), &( aClientCtxInfo.ClientId().Identity() ) );
    CleanupStack::PushL( connection );
    iConnections.AppendL( connection );
    CleanupStack::Pop( connection );

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::AcquireConnectionL() End" ) );
    return *connection;
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::ReleaseConnection()
// ---------------------------------------------------------------------------
//
void CImpsPlugin::ReleaseConnection( MXIMPProtocolConnection& aConnection )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::ReleaseConnection() Start" ) );

    TInt connectionsCount = iConnections.Count();

    for ( TInt i( connectionsCount - 1 ); i >= 0; i-- )
        {
        MXIMPProtocolConnection* tmp = iConnections[i];
        if ( tmp == &aConnection )
            {
            delete iConnections[i];
            iConnections.Remove( i );
            }
        }

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::ReleaseConnection() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CImpsPlugin::GetInterface( TInt32 aInterfaceId,
                                 TIfGetOps aOptions  )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::GetInterface() Start" ) );

    if ( aInterfaceId == MXIMPProtocolPlugin::KInterfaceId )
        {
        MXIMPProtocolPlugin* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )

        {
        User::Panic( KImpsPlugin, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CImpsPlugin::GetInterface( TInt32 aInterfaceId,
                                       TIfGetOps aOptions ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::GetInterface() const Start" ) );

    if ( aInterfaceId == MXIMPProtocolPlugin::KInterfaceId )
        {
        const MXIMPProtocolPlugin* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )

        {
        User::Panic( KImpsPlugin, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::GetInterface() const End" ) );

    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsPlugin::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CImpsPlugin::GetInterfaceId() const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPlugin::GetInterface() without parameters Start-End" ) );
    return MXIMPProtocolPlugin::KInterfaceId;
    }


// End of file

