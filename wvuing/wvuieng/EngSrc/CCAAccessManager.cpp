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
* Description:  Network operations for chat access handling.
 *
*/


// INCLUDE FILES
#include "ChatDebugPrint.h"

#include "CCAAccessManager.h"
#include "MCAImpsAccessClient.h"
#include "CCAAdapterDll.h"
#include "MCAImpsFactory.h"
#include "CCARequest.h"
#include "CCARequestMapper.h"


// ================= MEMBER FUNCTIONS =======================
// Symbian OS default constructor can leave.
void CCAAccessManager::ConstructL()
    {
    CHAT_DP_TXT( "CCAAccessManager::ConstructL, CreateAccessClientL" );
    iImpsAccessClient = CreateImpsFactoryL()->CreateAccessClientL();

    iImpsAccessClient->RegisterL( this, CActive::EPriorityIdle );
    iImpsAccessClient->RegisterErrorObserverL( *this );
    }

// Two-phased constructor.
CCAAccessManager* CCAAccessManager::NewL( CCARequestMapper& aRequestMapper )
    {
    CCAAccessManager* self = new ( ELeave ) CCAAccessManager( aRequestMapper );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCAAccessManager::~CCAAccessManager()
    {
    if ( iImpsAccessClient )
        {
        iImpsAccessClient->Unregister();
        }
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAAccessManager::CCAAccessManager( CCARequestMapper& aRequestMapper )
        : iRequestMapper( aRequestMapper )
    {
    }

void CCAAccessManager::LoginL(   TImpsCspIdentifier& aCspId,
                                 const TDesC& aPsw,
                                 const TDesC& aClientId,
                                 TUint32 aAP,
                                 const TDesC* aKey1/* = NULL*/,
                                 const TDesC* aKey2/* = NULL*/ )
    {
    CHAT_DP_FUNC_ENTER( "CCAAccessManager::LoginL" );
    TInt opId = iImpsAccessClient->LoginL( aCspId, aPsw, aClientId,
                                           aAP, aKey1, aKey2 );
    CHAT_DP( D_CHAT_LIT( "CCAAccessManager::LoginL - opid=%d" ), opId );
    TInt error = ExecuteRequestL( opId, ELoginRequest );
    CHAT_DP( D_CHAT_LIT( "CCAAccessManager::LoginL - opid=%d, status=%d" ),
             opId, error );
    CHAT_DP_FUNC_DONE( "CCAAccessManager::LoginL" );
    }

void CCAAccessManager::LogoutL()
    {
    CHAT_DP_FUNC_ENTER( "CCAAccessManager::LogoutL" );
    TInt opId = iImpsAccessClient->LogoutL();
    TInt error = ExecuteRequestL( opId, ELogoutRequest );
    CHAT_DP( D_CHAT_LIT( "CCAAccessManager::LogoutL - opid=%d, status=%d" ),
             opId, error );
    CHAT_DP_FUNC_DONE( "CCAAccessManager::LogoutL" );
    }

// --------------------------------------------------------
// CCAGroupManager::HandleErrorL
// ---------------------------------------------------------
//
void CCAAccessManager::HandleErrorL(
    TInt aStatus,
    TInt aOpId,
    const TDesC* /* aDescription */,
    const CImpsDetailed* /* aDetailedRes */,
    TImpsCspIdentifier& /*aCspId*/ )
    {
    CHAT_DP_FUNC_ENTER( "HandleErrorL" );
    CHAT_DP( D_CHAT_LIT( "CCAAccessManager::HandleErrorL - opid=%d, status=%d" ),
             aOpId, aStatus );
    iRequestMapper.HandleRequest( aOpId, aStatus );
    CHAT_DP_FUNC_DONE( "HandleErrorL" );
    }

void CCAAccessManager::HandleLoginL( TInt aId,
                                     TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAAccessManager::HandleLoginL - opid=%d" ), aId );
    iRequestMapper.HandleRequest( aId, KErrNone );
    }

void CCAAccessManager::HandleLogoutL( TInt aId,
                                      TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAAccessManager::HandleLogoutL - opid=%d" ), aId );
    iRequestMapper.HandleRequest( aId, KErrNone );
    }

void CCAAccessManager::HandleLoginCancelL( TInt aCancelledOpId,
                                           TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAAccessManager::HandleLoginCancelL - opid=%d" ),
             aCancelledOpId );
    iRequestMapper.HandleRequest( aCancelledOpId, KErrCancel );
    }

void CCAAccessManager::HandleNbrSessionsL( TInt aId,
                                           TInt /* aNbr */,
                                           TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAAccessManager::HandleNbrSessionsL - opid=%d" ),
             aId );
    iRequestMapper.HandleRequest( aId, KErrNone );
    }

// ---------------------------------------------------------
// CCAGroupWrapper::ExecuteRequest
// ---------------------------------------------------------
//
TInt CCAAccessManager::ExecuteRequestL( TInt aOperationId,
                                        TOperationRequests aRequestType,
                                        MDesCArray* aAdditionalData )
    {
    // creating request
    CCARequest* request = iRequestMapper.CreateRequestL( aOperationId, EFalse );
    CCARequestMapper& requestMapper = iRequestMapper;

    // setting type
    request->SetRequestType( aRequestType );

    // setting additional data if exists
    if ( aAdditionalData )
        {
        request->SetAdditionalData( *aAdditionalData );
        }

    // Now when the request type and additional data is set we can
    // activate the synchronous wait
    if ( aRequestType != ELogoutRequest )
        {
        request->StartWait();
        }

    TInt retVal( request->ErrorCode() );
    requestMapper.RemoveRequest( request );

    return retVal;
    }

//  End of File
