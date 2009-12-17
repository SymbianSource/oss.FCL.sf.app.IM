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
#include <ximpcontextclientinfo.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpprotocolconnection.h>
#include <protocolpresencefeatures.h>


#include "CImpsConnection.h"
#include "CImpsPluginConnectionManager.h"
#include "impsdebugprint.h"
#include "PImpsAdapWVCspVersion.h"

#include "cimpsprotocolpresencefeatures.h"
#include "CImpsPluginLogoutRequest.h"
#include "CImpsPluginLoginRequest.h"

#include "ximpserviceinfo.h"


_LIT( KImpsConnection, "CImpsConnection" );
_LIT8( KConnectionFeacture, "imps/connection " );


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CImpsConnection::CImpsConnection()
// ---------------------------------------------------------------------------
//
CImpsConnection::CImpsConnection()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::CImpsConnection() Start-End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsConnection::ConstructL()
// ---------------------------------------------------------------------------
//
void CImpsConnection::ConstructL(
    const MXIMPServiceInfo&  aServiceInfo ,
    const MXIMPContextClientInfo& aClientCtxInfo )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::ConstructL() Start" ) );

    IMPSPRTPLUGIN_UNUSED_PARAM( aClientCtxInfo );

    iConnMan = CImpsPluginConnectionManager::NewL();
    iConnMan->SetConnectionParamsL( aServiceInfo.ServiceAddress(),
                                    aServiceInfo.UserId(),
                                    aServiceInfo.Password(),
                                    aServiceInfo.IapId() );

    iPresFeatures = CImpsProtocolPresenceFeatures::NewL( *iConnMan );

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::ConstructL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsConnection::NewL()
// ---------------------------------------------------------------------------
//
CImpsConnection* CImpsConnection::NewL(
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& aClientCtxInfo )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::NewL() Start" ) );

    CImpsConnection* self = new( ELeave ) CImpsConnection;
    CleanupStack::PushL( self );
    self->ConstructL( aServiceInfo, aClientCtxInfo );
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::NewL() End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsConnection::~CImpsConnection()
// ---------------------------------------------------------------------------
//
CImpsConnection::~CImpsConnection()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::~CImpsConnection() Start" ) );

    delete iConnMan;
    delete iPresFeatures;

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::~CImpsConnection() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsConnection::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CImpsConnection::GetInterface( TInt32 aInterfaceId,
                                     TIfGetOps aOptions )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::GetInterface() Start" ) );
    if ( aInterfaceId == MXIMPProtocolConnection::KInterfaceId )
        {
        MXIMPProtocolConnection* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )

        {
        User::Panic( KImpsConnection, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsConnection::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CImpsConnection::GetInterface( TInt32 aInterfaceId,
                                           TIfGetOps aOptions ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::GetInterface() const Start" ) );

    if ( aInterfaceId == MXIMPProtocolConnection::KInterfaceId )
        {
        const MXIMPProtocolConnection* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KImpsConnection, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::GetInterface() const End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsConnection::NewInstanceLC()
// ---------------------------------------------------------------------------
//
TInt32 CImpsConnection::GetInterfaceId() const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::GetInterfaceId() Start-End" ) );
    return MXIMPProtocolConnection::KInterfaceId;
    }


// ---------------------------------------------------------------------------
// CImpsConnection::PrimeHost()
// ---------------------------------------------------------------------------
//
void CImpsConnection::PrimeHost( MXIMPProtocolConnectionHost& aHost )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::PrimeHost() Start" ) );

    iConnMan->SetHost( aHost );

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::PrimeHost() Start" ) );
    }


// ---------------------------------------------------------------------------
// CImpsConnection::OpenSessionL()
// ---------------------------------------------------------------------------
//
void CImpsConnection::OpenSessionL(
    const MXIMPContextClientInfo& aClientCtxInfo,
    TXIMPRequestId aOpId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::OpenSessionL() Start" ) );

    IMPSPRTPLUGIN_UNUSED_PARAM( aClientCtxInfo );

    iConnMan->ReserveSpaceForOneMoreRequestL();

    CImpsPluginLoginRequest* req =
        CImpsPluginLoginRequest::NewLC( *iConnMan, aOpId );
    CleanupStack::Check( req );

    req->IssueLoginRequestL();

    iConnMan->AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::OpenSessionL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsConnection::OpenSessionL()
// ---------------------------------------------------------------------------
//
void CImpsConnection::OpenSessionL( const TInt& aSettingsId,
                                    TXIMPRequestId aReqId )
    {
    // not supported
    }


// ---------------------------------------------------------------------------
// CImpsConnection::CloseSession()
// ---------------------------------------------------------------------------
//
void CImpsConnection::CloseSession(
    const MXIMPContextClientInfo& aClientCtxInfo,
    TXIMPRequestId aOpId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsConnection::CloseSession() Start" ) );

    IMPSPRTPLUGIN_UNUSED_PARAM( aClientCtxInfo );

    iConnMan->ReserveSpaceForOneMoreRequestL();

    CImpsPluginLogoutRequest* req =
        CImpsPluginLogoutRequest::NewLC( *iConnMan, aOpId );
    CleanupStack::Check( req );

    req->IssueLogoutRequestL();

    iConnMan->AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::CloseSession() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsConnection::ReOpenSessionsL()
// ---------------------------------------------------------------------------
//
void CImpsConnection::ReOpenSessionsL( TXIMPRequestId aOpId )
    {
    IMPS_DP_TXT( "CImpsConnection::ReOpenSessionsL() Start" );

    iConnMan->ReserveSpaceForOneMoreRequestL();

    CImpsPluginLoginRequest* req =
        CImpsPluginLoginRequest::NewLC( *iConnMan, aOpId );
    CleanupStack::Check( req );

    req->IssueLoginRequestL();

    iConnMan->AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "CImpsConnection::ReOpenSessionsL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsConnection::GetSupportedFeaturesL()
// ---------------------------------------------------------------------------
//
void CImpsConnection::GetSupportedFeaturesL( CDesC8Array& aFeatures ) const
    {
    IMPS_DP_TXT( "CImpsConnection::GetSupportedFeaturesL() Start End" );
    // first empty the whole array
    aFeatures.Reset();

    // then add our features there
    aFeatures.AppendL( KConnectionFeacture );
    IMPS_DP_TXT( "CImpsConnection::GetSupportedFeaturesL" );
    }
// ---------------------------------------------------------------------------
// CImpsConnection::ProtocolPresenceFeatures()
// ---------------------------------------------------------------------------
//
MProtocolPresenceFeatures& CImpsConnection::ProtocolPresenceFeatures()
    {
    IMPS_DP_TXT( "CImpsConnection::ProtocolPresenceFeatures() Start End" );
    return *iPresFeatures;

    // Fix warning: #111-D: statement is unreachable
    // IMPS_DP_TXT("CImpsConnection::ProtocolPresenceFeatures");
    }

// ---------------------------------------------------------------------------
// CImpsConnection::GetProtocolInterface
// ---------------------------------------------------------------------------
//
TAny* CImpsConnection::GetProtocolInterface( TInt /*aInterfaceId*/ )
    {
    IMPS_DP_TXT( "CImpsConnection::GetProtocolInterface() Start End" );
    // not supported
    return NULL;
    }

// End of file

