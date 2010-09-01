/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: IMPS plugin connection manager
*
*/


#include <e32std.h>
#include <e32base.h>
#include <ximpobjectfactory.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpstatus.h>

#include "CImpsPluginConnectionManager.h"
#include "PImpsAdapWVCspVersion.h"

#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"

#include "CImpsPluginLoginRequest.h"
#include "CImpsPluginLogoutRequest.h"





// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::CImpsPluginConnectionManager
// ---------------------------------------------------------------------------
//
CImpsPluginConnectionManager::CImpsPluginConnectionManager()
        : iIapId( 0 )
    {
    IMPS_DP( D_IMPS_LIT( "::CImpsPluginConnectionManager() Start" ) );
    IMPS_DP( D_IMPS_LIT( "::CImpsPluginConnectionManager() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::ConstructL
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::ConstructL()
    {

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::ConstructL() Start" ) );

    User::LeaveIfError( iImpsClient.Connect() );

    iAccessHandler = CImpsPluginAccessHandler::NewL( iImpsClient );

    iNotifier = CPresenceNotificationHandler::NewL( *this );

    // set the default version to 1.2
    SetVersion( EWVCspV12 );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::ConstructL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::ManufactureDataHandlerL
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::ManufactureDataHandlerL()
    {

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::ManufactureDataHandlerL() Start" ) );
    if ( !iIsDataHandlerReady )
        {
        iPureHandler = CImpsPluginPureDataHandler::NewL( iImpsClient, *iAccessHandler );
        iIsDataHandlerReady = ETrue;
        iNotifier->StartListeningL();
        }
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::IsDataHandlerCreatedL
// ---------------------------------------------------------------------------
//
TBool CImpsPluginConnectionManager::IsDataHandlerCreatedL()
    {

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::IsDataHandlerCreatedL() Start-End" ) );

    return iIsDataHandlerReady;
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::NewL()
// ---------------------------------------------------------------------------
//
CImpsPluginConnectionManager* CImpsPluginConnectionManager::NewL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::NewL() Start" ) );

    CImpsPluginConnectionManager* self = new( ELeave )
    CImpsPluginConnectionManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::NewL() End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::~CImpsPluginConnectionManager
// ---------------------------------------------------------------------------
//
CImpsPluginConnectionManager::~CImpsPluginConnectionManager()
    {
    IMPS_DP( D_IMPS_LIT( "::~CImpsPluginConnectionManager() Start" ) );

    delete iDisconnectStatusObj;

    delete iServerAddress;
    delete iUserName;
    delete iPassword;
    delete iLoginRequest;
    delete iLogoutRequest;
    delete iNotifier;
    delete iPureHandler;
    delete iAccessHandler;
    // before destory cancel each of the active objects request
    const TInt c = iRequestArray.Count() - 1;
    for ( TInt i = c; i >= 0; i-- )
        {
        delete iRequestArray[ i ];
        }
    iPresenceAuthGrantReqList.Reset();
    iRequestArray.Reset();

    iImpsClient.Close();

    IMPS_DP( D_IMPS_LIT( "::~CImpsPluginConnectionManager() End" ) );
    }

CPresenceNotificationHandler* CImpsPluginConnectionManager::PresNotifier()
    {
    return iNotifier;
    }
// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::IapId
// ---------------------------------------------------------------------------
//
TInt32 CImpsPluginConnectionManager::IapId()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::IapId() Start-End" ) );
    return iIapId;
    }
// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::ServerAddress
// ---------------------------------------------------------------------------
//
TDesC& CImpsPluginConnectionManager::ServerAddress()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::ServerAddress() Start-End" ) );
    return *iServerAddress;
    }


// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::Username
// ---------------------------------------------------------------------------
//
TDesC& CImpsPluginConnectionManager::UserName()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::UserName() Start-End" ) );
    return *iUserName;
    }


// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::Password
// ---------------------------------------------------------------------------
//
TDesC& CImpsPluginConnectionManager::Password()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::Password() Start-End" ) );
    return *iPassword;
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::HandleToHost
// ---------------------------------------------------------------------------
//
MXIMPProtocolConnectionHost& CImpsPluginConnectionManager::HandleToHost()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::HandleToHost() Start-End" ) );
    return *iHostHandle;
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::AccessHandler
// ---------------------------------------------------------------------------
//
CImpsPluginAccessHandler& CImpsPluginConnectionManager::AccessHandler()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::AccessHandler() Start-End" ) );
    return *iAccessHandler;
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::DataHandler
// ---------------------------------------------------------------------------
//
CImpsPluginPureDataHandler& CImpsPluginConnectionManager::DataHandler()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::DataHandler() Start-End" ) );
    return *iPureHandler;
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::SetHost
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::SetHost( MXIMPProtocolConnectionHost& aHost )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::SetHost() Start" ) );
    iHostHandle = &aHost;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::SetHost() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::SetConnectionParamsL
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::SetConnectionParamsL( const TDesC& aServerAddress,
                                                         const TDesC& aUserName,
                                                         const TDesC& aPassword,
                                                         const TInt32 aIapId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::SetConnectionParamsL() Start" ) );
    // first store the login parameters
    HBufC* tmpBuf = aServerAddress.AllocL();
    delete iServerAddress;
    iServerAddress = tmpBuf;

    tmpBuf = aUserName.AllocL();
    delete iUserName;
    iUserName = tmpBuf;

    tmpBuf = aPassword.AllocL();
    delete iPassword;
    iPassword = tmpBuf;

    iIapId = aIapId;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::SetConnectionParamsL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::InitializeStatusL
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::InitializeStatusL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::InitializeStatusL() Start" ) );

    MXIMPObjectFactory& objFactory = iHostHandle->ObjectFactory();
    iDisconnectStatusObj = objFactory.NewStatusLC();

    CleanupStack::Pop( iDisconnectStatusObj ); //iDisconnectStatusObj

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::InitializeStatusL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::StatusChangedL
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::StatusChangedL(
    TPEngNWSessionSlotState aNewState,
    TPEngNWSessionSlotEvent aNewEvent )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::StatusChangedL() Start" ) );

    IMPSPRTPLUGIN_UNUSED_PARAM( aNewState );
    IMPSPRTPLUGIN_UNUSED_PARAM( aNewEvent );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::StatusChangedL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::ReserveSpaceForOneMoreRequestL
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::ReserveSpaceForOneMoreRequestL( )
    {
    IMPS_DP( D_IMPS_LIT( "::ReserveSpaceForOneMoreRequestL() Start" ) );

    TInt count = iRequestArray.Count();

    iRequestArray.Reserve( count + 1 );

    IMPS_DP( D_IMPS_LIT( "::ReserveSpaceForOneMoreRequestL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::AddRequest
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::AddRequest( CActive* aRequestObject )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::AddRequest() Start" ) );

    TInt count = iRequestArray.Count();

    TInt error = iRequestArray.Append( aRequestObject );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::AddRequest() End (%d)" ), error );
    }


// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::Remove
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::Remove( CActive* aRequestObject )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::Remove() Start" ) );

    TInt index = iRequestArray.Find( aRequestObject );
    if ( index != KErrNotFound )
        {
        iRequestArray.Remove( index );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::Remove() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::GetVersion
// ---------------------------------------------------------------------------
//
TImpsAdapWVCspVersion CImpsPluginConnectionManager::GetVersion()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::GetVersion() Start-End" ) );
    return iCSPVersion;
    }

// ---------------------------------------------------------------------------
// CImpsPluginConnectionManager::Remove
// ---------------------------------------------------------------------------
//
void CImpsPluginConnectionManager::SetVersion( TImpsAdapWVCspVersion aVersion )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginConnectionManager::SetVersion Start-End" ) );

    if ( aVersion == EWVCspV12 )
        {
        iCSPVersion = EWVCspV12;
        }
    else if ( aVersion == EWVCspV13 )
        {
        iCSPVersion = EWVCspV13;
        }
    else if ( aVersion == EWVCspV11 )
        {
        iCSPVersion = EWVCspV11;
        }
    }

RArray< HBufC >* CImpsPluginConnectionManager::PresenceAuthGrantReqList()
    {
    return &iPresenceAuthGrantReqList;
    }




// End of File

