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
* Description: Implements the CIMStatusFetcher and the MImStatusList interface.
*
*/



// INCLUDE FILES
#include "imstatusimpl.h"
#include "apiutils.h"
#include "imconnectionimpl.h"
#include "cntdbreader.h"
#include "impserrors.h"
#include "imerrors.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::CIMStatusFetcherImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMStatusFetcherImpl::CIMStatusFetcherImpl( CImConnectionImpl* aImConnection )
        :   iImConnection( aImConnection )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CIMStatusFetcherImpl: Construction" ) );
#endif

    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMStatusFetcherImpl::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMStatusFetcherImpl* CIMStatusFetcherImpl::NewL( CImConnectionImpl* aImConnection )
    {
    CIMStatusFetcherImpl* self = new( ELeave ) CIMStatusFetcherImpl( aImConnection );
    CleanupStack::PushL( self );

    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::~CIMStatusFetcherImpl
// Destructor
// -----------------------------------------------------------------------------
//
CIMStatusFetcherImpl::~CIMStatusFetcherImpl()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CIMStatusFetcherImpl: Destruction" ) );
#endif

    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::RegisterL
// Starts the registration sequence
// -----------------------------------------------------------------------------
//
TInt CIMStatusFetcherImpl::RegisterObserverL( MImStatusObserver* aObserver )
    {

#ifdef _DEBUG
    CImApiLogger::Log( _L( "CIMStatusFetcherImpl: RegisterL" ) );
#endif

    if ( iClientObserver )
        {
        return KImApiErrAllreadyRegistered;
        }

    iClientObserver = aObserver;
    iPresenceClient.RegisterL(
        ImpsEngine(),
        ImpsPresenceHandler(),
        ClientId(),
        ETrue,
        0 );

    iPresenceClient.RegisterErrorObserverL( ImpsErrorHandler() );

    return KErrNone;


    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::UnregisterL
// Unregisters the client from the WV engine
// -----------------------------------------------------------------------------
//
TInt CIMStatusFetcherImpl::UnregisterObserverL()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CIMStatusFetcherImpl: UnregisterL" ) );
#endif

    iPresenceClient.Unregister();
    iPresenceClient.UnregisterErrorObserverL();

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::GetOnlineStatusL
// Fetches the online status of the WV users given by WV user IDs in async way
// -----------------------------------------------------------------------------
//
TInt CIMStatusFetcherImpl::GetOnlineStatusL( const CDesCArray& aUserIds )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CIMStatusFetcherImpl: GetOnlineStatusL" ) );
#endif
    iPresenceClient.GetPresenceL( aUserIds );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::GetOnlineStatusL
// Fetches the online status of the WV users given by contact IDs in async way
// -----------------------------------------------------------------------------
//
TInt CIMStatusFetcherImpl::GetOnlineStatusL( const CContactIdArray& aContactIds )
    {

#ifdef _DEBUG
    CImApiLogger::Log( _L( "CIMStatusFetcherImpl: GetOnlineStatusL" ) );
    CImApiLogger::Log( _L( "    Item count: %d" ), aContactIds.Count() );
#endif

    CDesCArraySeg* userIds = new ( ELeave ) CDesCArraySeg( 1 );
    CleanupStack::PushL( userIds );
    iImConnection->ContactDBReader().ReadWVIdsL( aContactIds, *userIds );

#ifdef _DEBUG
    for ( TInt i( 0 ); i < userIds->MdcaCount(); ++i )
        {
        TPtrC tmp = ( *userIds )[i];
        CImApiLogger::Log( _L( "    User ID: %S" ), &tmp );
        }
#endif

    TInt opId( 0 );

    TRAPD( err, opId = iPresenceClient.GetPresenceL( *userIds ) );
    CleanupStack::PopAndDestroy(); // >> userIds


    if ( err != KErrNone )
        {
        return opId;
        }

    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::HandleCompleteL
// Implementation of MImpsPresenceHandler::HandleCompleteL
// -----------------------------------------------------------------------------
//
void CIMStatusFetcherImpl::HandleCompleteL( TInt /*aStatus*/,
                                            TInt /*aOpId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::HandleNewPresenceL
// Implementation of MImpsPresenceHandler::HandleNewPresenceL
// Creates the CIMStatusList object with the IM statuses
// -----------------------------------------------------------------------------
//
void CIMStatusFetcherImpl::HandleNewPresenceL( TInt aStatus,
                                               TInt aOpId,
                                               CImpsPresenceList& aPresList )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CIMStatusFetcherImpl: HandleNewPresenceL" ) );
#endif

    CIMStatusListImpl* statusList = CIMStatusListImpl::NewL( aPresList );
    CleanupStack::PushL( statusList );

    if ( iClientObserver )
        {
        TRAPD( err, iClientObserver->HandleGetOnlineStatusL(
                   aOpId,
                   aStatus,
                   statusList ) );
        }

    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::HandleErrorL
// Implementation of MImpsPresenceHandler::HandleErrorL
// -----------------------------------------------------------------------------
//
void CIMStatusFetcherImpl::HandleErrorL( TInt aStatus,
                                         TInt aOpId,
                                         const TDesC* /*aDescription*/,
                                         const CImpsDetailed* /*aDetailedRes*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CIMStatusFetcherImpl: Error received: %d" ), aStatus );
#endif

    TInt apiError;

    switch ( aStatus )
        {
        case KImpsErrorNotLogged:
            apiError = KImApiErrNotLogged;
            break;

        default:
            apiError = KImApiGeneralError;
        }


    if ( iClientObserver )
        {
        TRAPD( err, iClientObserver->HandleGetOnlineStatusL(
                   aOpId,
                   apiError,
                   NULL ) );
        }
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::ImpsPresenceHandler
//
// -----------------------------------------------------------------------------
//
MImpsPresenceHandler* CIMStatusFetcherImpl::ImpsPresenceHandler()
    {
    return this;
    }


// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::ImpsErrorHandler
//
// -----------------------------------------------------------------------------
//
MImpsErrorHandler& CIMStatusFetcherImpl::ImpsErrorHandler()
    {
    return *this;
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::ImpsPresenceClient
//
// -----------------------------------------------------------------------------
//
RImpsPresenceClient& CIMStatusFetcherImpl::ImpsPresenceClient()
    {
    return iPresenceClient;
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::ImpsEngine
//
// -----------------------------------------------------------------------------
//
RImpsEng& CIMStatusFetcherImpl::ImpsEngine()
    {
    return iImConnection->ImpsEngine();
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::ClientId
//
// -----------------------------------------------------------------------------
//
TDesC& CIMStatusFetcherImpl::ClientId()
    {
    return *iClientId;
    }

// -----------------------------------------------------------------------------
// CIMStatusFetcherImpl::ClientObserver
//
// -----------------------------------------------------------------------------
//
MImStatusObserver* CIMStatusFetcherImpl::ClientObserver()
    {
    return iClientObserver;
    }


//  End of File
