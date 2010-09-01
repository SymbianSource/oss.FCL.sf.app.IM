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
* Description: Class for handling the login issues.
*
*/



// INCLUDE FILES
#include <Cwvsapsettingsstore.h>
#include <Cwvsapsettings.h>
#include <imerrors.h>
#include "loginmngr.h"
#include "apiutils.h"
#include "imconnectionimpl.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLoginManager::CLoginManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLoginManager::CLoginManager( RImpsEng& aEngine, CImConnectionImpl& aConnection )
        :   CActive( EPriorityStandard ),
        iState( EReady ),
        iLoggedIn( EFalse ),
        iEngine( aEngine ),
        iEngineState( EImps_NOT_LOGGED ),
        iObserver( NULL ),
        iConnection( aConnection )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: Construction" ) );
#endif
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CLoginManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLoginManager::ConstructL()
    {
    User::LeaveIfError( iEngine.Connect() );
    // Register to the IMPS Access Client
    iAccessClient.RegisterL( iEngine, this );
    iAccessClient.RegisterErrorObserverL( *this );

    }

// -----------------------------------------------------------------------------
// CLoginManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLoginManager* CLoginManager::NewL( RImpsEng& aEngine,
                                    CImConnectionImpl& aConnection )
    {
    CLoginManager* self = new( ELeave ) CLoginManager( aEngine, aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CLoginManager::~CLoginManager()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: Destruction" ) );
#endif
    Cancel();
    iAccessClient.Unregister();
    TRAPD( err, iAccessClient.UnregisterErrorObserverL() );
    }


// -----------------------------------------------------------------------------
// CLoginManager::HandleLoginL
//
//
// -----------------------------------------------------------------------------
//
void CLoginManager::HandleLoginL(
    TInt /*aId*/,
    TImpsCspIdentifier& /*aCspId*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: HandleLogin" ) );
#endif

    iLoggedIn = ETrue;

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }


// -----------------------------------------------------------------------------
// CLoginManager::HandleLoginCancelL
//
// -----------------------------------------------------------------------------
//
void CLoginManager::HandleLoginCancelL(
    TInt aCancelledOpId,
    TImpsCspIdentifier& /*aCspId*/ )
    {

    }
// -----------------------------------------------------------------------------
// CLoginManager::HandleLoginStatusL
//
// -----------------------------------------------------------------------------
//
//void CLoginManager::HandleLoginStatusL(
//                                      TInt /*aId*/,
//                                      TInt aStatus,
//                                      TBool aIsLoggedIn )
//    {
//#ifdef _DEBUG
//    CImApiLogger::Log(_L("CLoginManager: HandleLoginStatus: %d"), aStatus);
//    CImApiLogger::Log(_L("CLoginManager: Is logged in: %d"), aIsLoggedIn);
//#endif
//    iLoggedIn = aIsLoggedIn;
//    TRequestStatus* status = &iStatus;
//    User::RequestComplete(status, aStatus);
//    }

// -----------------------------------------------------------------------------
// CLoginManager::HandleLogoutL
//
// -----------------------------------------------------------------------------
//
void CLoginManager::HandleLogoutL( TInt /*aId*/,
                                   TImpsCspIdentifier& aCspId )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: HandleLogoutL" ) );
#endif

    iLoggedIn = EFalse;

    }


// -----------------------------------------------------------------------------
// CLoginManager::HandleErrorL
//
// -----------------------------------------------------------------------------
//
void CLoginManager::HandleErrorL( TInt aStatus,
                                  TInt /*aOpId*/,
                                  const TDesC* /*aDescription*/,
                                  const CImpsDetailed* /*aDetailedRes*/,
                                  TImpsCspIdentifier& aCspId )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: Error received: %d" ), aStatus );
#endif
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aStatus );
    }

// -----------------------------------------------------------------------------
// CLoginManager::HandleStatusChangeL
//
// -----------------------------------------------------------------------------
//
void CLoginManager::HandleStatusChangeL( TImpsServiceStatus aStatus )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: Status changed: %d" ), aStatus );
#endif
    iEngineState = aStatus;

    switch ( aStatus )
        {
        case EImps_NOT_LOGGED:
            iLoggedIn = EFalse;
            break;
        case EImps_ON_LINE:
            iLoggedIn = ETrue;
            break;

        case EImps_OFF_LINE:
        case EImps_NO_IAP:
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLoginManager::HandleNbrSessionsL
//
// -----------------------------------------------------------------------------
//
void CLoginManager::HandleNbrSessionsL( TInt /*aId*/,
                                        TInt /*aNbr*/,
                                        TImpsCspIdentifier& /*aCspId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CLoginManager::StartRegistration
//
// -----------------------------------------------------------------------------
//
TInt CLoginManager::StartLogin( MLoginObserver* aObserver )
    {
//#ifdef _DEBUG
//    CImApiLogger::Log(_L("CLoginManager: Login started"));
//#endif
//
//    if ( IsActive() )
//        {
//        return KImApiErrConnectingInProgress;
//        }
//    iObserver = aObserver;
//    iState = EInitializing;
//    iStatus = KRequestPending;
//    SetActive();
//    TRequestStatus* status = &iStatus;
//    User::RequestComplete(status, KErrNone);
    return KErrNone;
    }
//
// -----------------------------------------------------------------------------
// CLoginManager::IsLoggedIn()
//
// -----------------------------------------------------------------------------
//
TBool CLoginManager::IsLoggedIn() const
    {
    return iLoggedIn;
    }

// -----------------------------------------------------------------------------
// CLoginManager::EngineStatus()
//
// -----------------------------------------------------------------------------
//
TImpsServiceStatus CLoginManager::EngineStatus() const
    {
    return iEngineState;
    }

// -----------------------------------------------------------------------------
// CLoginManager::CancelLogin()
//
// -----------------------------------------------------------------------------
//

void CLoginManager::CancelLogin( )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: Login canceled" ) );
#endif
    Cancel();
    }

// -----------------------------------------------------------------------------
// CLoginManager::RunL
//
// -----------------------------------------------------------------------------
//
void CLoginManager::RunL()
    {

    User::LeaveIfError( iStatus.Int() );

    switch ( iState )
        {
        case EInitializing:
            {
#ifdef _DEBUG
            CImApiLogger::Log( _L( "CLoginManager: Calling IsLoggedIn()" ) );
#endif
//            iAccessClient.IsLoggedInL();
            iState = ELoggingIn;
            iStatus = KRequestPending;
            SetActive();
            }
        break;

        case ELoggingIn:
            if ( !iLoggedIn )
                { // Do Login
                // Get the default login params from SAP settings
                CWVSAPSettingsStore* store = CWVSAPSettingsStore::NewLC(); // <<<
                CWVSAPSettings* sapSettings = CWVSAPSettings::NewLC();     // <<<
                store->GetDefaultL( *sapSettings );
                const TDesC& server = sapSettings->SAPAddress();
                const TDesC& user = sapSettings->SAPUserId();
                const TDesC& passwd = sapSettings->SAPUserPassword();
                const TDesC& clientId = sapSettings->ClientId();
                TUint32 myAP = sapSettings->AccessPoint();
//                const TDesC& proxy = sapSettings->HTTPProxyAddress();
                const TDesC& key1 = sapSettings->SAPSpecificString1();
                const TDesC& key2 = sapSettings->SAPSpecificString2();

#ifdef _DEBUG
                CImApiLogger::Log( _L( "CLoginManager: Logging in:" ) );
                CImApiLogger::Log( _L( "    Server: %S" ), &server );
                CImApiLogger::Log( _L( "    User: %S" ), &user );
                CImApiLogger::Log( _L( "    Password: %S" ), &passwd );
//                CImApiLogger::Log(_L("    ClientID: %S"),iClientID);
                CImApiLogger::Log( _L( "    IAP: %d" ), myAP );
//                CImApiLogger::Log(_L("    Proxy: %S"),&proxy);

#endif

#ifdef _DEBUG
                CImApiLogger::Log( _L( "CLoginManager: Logging in" ) );
#endif
//                iAccessClient.LoginL(
//                    server,
//                    user,
//                    passwd,
//                    // because of AOL we need the client ID from SAP settings
//                    // iConnection.ClientId(),
//                    clientId,
//                    myAP,
//                    //proxy, Not used!
//                    KNullDesC,
//                    &key1,
//                    &key2);

                iState = EFinishing;
                iStatus = KRequestPending;
                SetActive();

                CleanupStack::PopAndDestroy( 2 );   // >>> sapSettings, store
                }
            else
                {
                // already logged in -> just finish
#ifdef _DEBUG
                CImApiLogger::Log( _L( "CLoginManager: Already logged in." ) );
#endif
                iState = EFinishing;
                iStatus = KRequestPending;
                SetActive();
                TRequestStatus* s = &iStatus;
                User::RequestComplete( s, KErrNone );

                }

            break;

        case EFinishing:
            NotifyObserver( KErrNone );
            iState = EReady;
            break;

        case EReady:
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLoginManager::RunError
//
// Returns: <KErrNone>:
// -----------------------------------------------------------------------------
//
TInt CLoginManager::RunError( TInt aError )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CLoginManager: RunL leaves: %d" ), aError );
#endif
    NotifyObserver( aError );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CLoginManager::DoCancel
//
// -----------------------------------------------------------------------------
//
void CLoginManager::DoCancel()
    {
    iState = EInitializing;
    }

// -----------------------------------------------------------------------------
// CLoginManager::NotifyObservers
//
// -----------------------------------------------------------------------------
//
void CLoginManager::NotifyObserver( TInt aError )
    {
    iObserver->HandleLogin( aError );
    }

//  End of File
