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
* Description: Implementation of the CImConnection interface (imconnection.h)
*
*/



// INCLUDE FILES
#include    "imconnectionimpl.h"
#include    "imerrors.h"
#include    "imclientimpl.h"
#include    "cntdbreader.h"
#include    "apiutils.h"
#include    <impserrors.h>
//#include    <centralrepository.h>
#include    <wvengineInternalCRKeys.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImConnectionImpl::CImConnectionImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImConnectionImpl::CImConnectionImpl():
        iCurrentOpId( 0 ),
        iState( EImNotLogged )
    {
    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::ConstructL( TDesC& aApplicationId )
    {
    User::LeaveIfError( iEngine.Connect() );
    iApplicationId = aApplicationId.AllocL();
    iContactReader = CContactDBReader::NewL();

    /*// Read the CSP version from CenRep
    TReal                   cspVersion; // the CSP version
    CRepository* repository = CRepository::NewLC(KCRUIDWVEngineVariation);
    repository->Get(KWVEngineCspVersion, cspVersion);
    CleanupStack::PopAndDestroy(); // repository
    if ( cspVersion < 1.2 )
        {
        // the CSP version is too low
        User::Leave(KErrNotSupported);
        }*/
    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImConnectionImpl* CImConnectionImpl::NewL( TDesC& aApplicationId )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: NewL %S" ), &aApplicationId );
#endif
    CImConnectionImpl* self = new( ELeave ) CImConnectionImpl();

    CleanupStack::PushL( self );
    self->ConstructL( aApplicationId );
    CleanupStack::Pop( );

    return self;
    }


// Destructor
CImConnectionImpl::~CImConnectionImpl()
    {
    delete iApplicationId;
    delete iContactReader;
    iEngine.Close();
    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::CreateImClientL
// Factory function to create CImClient object
// -----------------------------------------------------------------------------
//
MImClient* CImConnectionImpl::CreateImClientL()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: CreateImClientL" ) );
#endif

    return CImClient::NewL( *this );
    }


// -----------------------------------------------------------------------------
// CImConnectionImpl::RegisterObserverL
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::RegisterObserverL( MImConnectionObserver* aObserver )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: RegisterObserverL" ) );
#endif

    if ( iClientObserver )
        {
        User::Leave( KImApiErrAlreadyRegistered );
        }


    iAccessClient.RegisterL(
        iEngine,
        this );
    iAccessClient.RegisterErrorObserverL( *this );
    iAccessClient.RegisterStatusObserverL( this );

    iClientObserver = aObserver;

    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::UnregisterObserverL
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::UnregisterObserver()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: UnregisterObserver" ) );
#endif
    if ( iClientObserver != NULL )
        {
        iClientObserver = NULL;
        iAccessClient.Unregister();
        TInt err;
        TRAP( err, iAccessClient.UnregisterErrorObserverL() );
        TRAP( err, iAccessClient.UnregisterStatusObserverL() );
        }

    }


// -----------------------------------------------------------------------------
// CImConnectionImpl::LoginL
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::LoginL(
    const TDesC& aServer,
    const TDesC& aUserID,
    const TDesC& aPassword,
    const TUint32 aAP )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: LoginL" ) );
#endif

    switch ( iState )
        {
        case EImLoggingIn:
            User::Leave( KImApiErrLoginInProgress );
        case EImLoggingOut:
            User::Leave( KImApiErrLogoutInProgress );
        case EImCancelingLogin:
            User::Leave( KImApiErrCancelLoginInProgress );
        case EImLoggedIn:
            User::Leave( KImApiErrAlreadyLoggedIn );
        default: ; // EImNotLogged -> just continue
        }

    // check whether registered
    if ( !iClientObserver )
        {
        User::Leave( KImApiErrNotRegistered );
        }

    // We need to get the ClientID in from SAP Settings
    // some servers will not allow logging in without that
    const TDesC* clientId = NULL;
    clientId = SSapSettingsReader::ClientIdLC( aServer );

    iCurrentOpId = iAccessClient.LoginL(
                       TImpsCspIdentifier( aServer, aUserID ),
                       aPassword,
                       clientId ? *clientId : KNullDesC,
                       aAP,
                       NULL,
                       NULL
                   );

    if ( clientId != NULL )
        CleanupStack::PopAndDestroy( 1 ); // clientId

    SetState( EImLoggingIn );

    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::CancelLoginL
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::CancelLoginL()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: CancelLoginL" ) );
#endif
    switch ( iState )
        {
        case EImLoggedIn:
            User::Leave( KImApiErrAlreadyLoggedIn );
        case EImCancelingLogin:
            User::Leave( KImApiErrCancelLoginInProgress );
        case EImLoggingOut:
            User::Leave( KImApiErrLogoutInProgress );
        case EImNotLogged:
            User::Leave( KImApiErrNotLogged );
        default:; // EImLoggingIn -> just continue
        }

    iAccessClient.CancelLoginL( iCurrentOpId );
    SetState( EImCancelingLogin );
    }


// -----------------------------------------------------------------------------
// CImConnectionImpl::LoginL
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::LogoutL()
    {
    switch ( iState )
        {
        case EImLoggingIn:
            User::Leave( KImApiErrNotLogged );
        case EImCancelingLogin:
            User::Leave( KImApiErrCancelLoginInProgress );
        case EImLoggingOut:
            User::Leave( KImApiErrLogoutInProgress );
        case EImNotLogged:
            User::Leave( KImApiErrNotLogged );
        default:; // EImLoggedIn -> just continue
        }

    iAccessClient.LogoutL();
    SetState( EImLoggingOut );
    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::ImConnectionStatus
// -----------------------------------------------------------------------------
//
TImConnectionStatus CImConnectionImpl::ImConnectionStatus()
    {
    return ( iState == EImLoggedIn ) ? ELoggedIn : ENotLoggedIn;
    }
// -----------------------------------------------------------------------------
// CLoginManager::HandleLoginL
//
//
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::HandleLoginL(
    TInt /*aId*/,
    TImpsCspIdentifier& /*aCspId*/ )
    {
    // Called when login has finished
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: HandleLoginL" ) );
#endif
    SetState( EImLoggedIn );
    if ( iClientObserver )
        {
        iClientObserver->HandleLoginL( KErrNone );
        }


    }


// -----------------------------------------------------------------------------
// CLoginManager::HandleLoginCancelL
//
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::HandleLoginCancelL(
    TInt /*aCancelledOpId*/,
    TImpsCspIdentifier& /*aCspId*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: HandleLoginCancelL" ) );
#endif
    SetState( EImNotLogged );
    if ( iClientObserver )
        {
        iClientObserver->HandleCancelLoginL( KErrNone );
        }

    }

// -----------------------------------------------------------------------------
// CLoginManager::HandleLogoutL
//
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::HandleLogoutL( TInt /*aId*/,
                                       TImpsCspIdentifier& /*aCspId*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: HandleLogoutL" ) );
#endif
    SetState( EImNotLogged );
    if ( iClientObserver )
        {
        iClientObserver->HandleLogoutL( KErrNone );
        }

    }


// -----------------------------------------------------------------------------
// CLoginManager::HandleErrorL
//
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::HandleErrorL( TInt aStatus,
                                      TInt /*aOpId*/,
                                      const TDesC* /*aDescription*/,
                                      const CImpsDetailed* /*aDetailedRes*/,
                                      TImpsCspIdentifier& /*aCspId*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImConnectionImpl: Error received: %d in state: %d" ), aStatus, iState );
#endif
    if ( iClientObserver )
        {
        TInt apiErrorCode = ConvertImpsEngineErrorCode( aStatus );
        // distribute the error to the correct handler
        switch ( iState )
            {
            case EImLoggingIn:
                SetState( EImNotLogged );
                iClientObserver->HandleLoginL( apiErrorCode );
                break;
            case EImCancelingLogin:
                iClientObserver->HandleCancelLoginL( apiErrorCode );
                // here the state change is tricky, it is better to wait the
                // status change
                break;
            case EImLoggingOut:
                SetState( EImNotLogged );
                iClientObserver->HandleLogoutL( apiErrorCode );
                break;
            default:
                ;
            }
        }

    }

// -----------------------------------------------------------------------------
// CLoginManager::HandleStatusChangeL
//
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::HandleStatusChangeL(
    TImpsServiceStatus aStatus,
    TImpsCspIdentifier& /*aCspId*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: Status changed: %d" ), aStatus );
#endif

    // change our status according to the engine
    // this is a good opportunity to synch or state to engine's
    switch ( aStatus )
        {
        case EImps_NOT_LOGGED:
            SetState( EImNotLogged );
            break;
        case EImps_ON_LINE:
            SetState( EImLoggedIn );
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLoginManager::HandleNbrSessionsL
//
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::HandleNbrSessionsL( TInt /*aId*/,
                                            TInt /*aNbr*/,
                                            TImpsCspIdentifier& /*aCspId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::CreateImStatusFetcherL
// Factory function CIMStatusFetcherImpl object
// -----------------------------------------------------------------------------
//
RImpsEng& CImConnectionImpl::ImpsEngine()
    {
    return iEngine;
    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::ContactDBReader
// -----------------------------------------------------------------------------
//
CContactDBReader& CImConnectionImpl::ContactDBReader()
    {
    return *iContactReader;
    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::ClientId
// -----------------------------------------------------------------------------
//
const TDesC& CImConnectionImpl::ApplicationId()
    {
    return *iApplicationId;
    }

// -----------------------------------------------------------------------------
// CImConnectionImpl::ConvertImpsEngineErrorCode
// Converts the IMPS engine error code to an API error code
// -----------------------------------------------------------------------------
//
TInt CImConnectionImpl::ConvertImpsEngineErrorCode( TInt aErrorCode )
    {
    // Do not try to convert KErrNone
    if ( aErrorCode == KErrNone )
        {
        return KErrNone;
        }

    TInt returnValue;
    switch ( aErrorCode )
        {
        case KImpsErrorNotLogged:
            returnValue = KImApiErrNotLogged;
            break;

        case Imps_ERROR_BASE-409: // Wrong password
            returnValue = KImApiErrWrongPassword;
            break;

        case Imps_ERROR_BASE-531: // User does not exist
            returnValue = KImApiErrInvalidUserId;
            break;

        case Imps_ERROR_BASE-201: // Partial success
            returnValue = KImApiErrPartialSuccess;
            break;
        default: // KImpsErrorOthersLogged
            // KImpsErrorShuttingDown,
            // KImpsErrorBearerSuspended
            returnValue = KImApiGeneralError;
        }
    return returnValue;
    }


// -----------------------------------------------------------------------------
// CImConnectionImpl::SetState
// Changes the internal state of the object
// -----------------------------------------------------------------------------
//
void CImConnectionImpl::SetState( TConnectionState aNewState )
    {
    iState = aNewState;
    }


//  End of File
