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
* Description:  Implements WV engine access services interface
*
*/



// INCLUDE FILES
#include    "CCAImpsAccessClient.h"
#include    "CCAAdapterDll.h"
#include    "TCAInterfaceSignaller.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::CCAImpsAccessClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAImpsAccessClient::CCAImpsAccessClient( RImpsEng& aImpsEng )
        : iImpsEng( aImpsEng )
    {
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAImpsAccessClient* CCAImpsAccessClient::NewL( RImpsEng& aImpsEng )
    {
    CCAImpsAccessClient* self = new( ELeave ) CCAImpsAccessClient( aImpsEng );
    return self;
    }

// Destructor
CCAImpsAccessClient::~CCAImpsAccessClient()
    {
    delete iCIdle;
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::InterfaceL
// Returns registered WV Engine interface. Implements lazy initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RImpsAccessClient2* CCAImpsAccessClient::InterfaceL()
    {
    if ( iLazyInitialization )
        {
        iClient.RegisterL( iImpsEng, iAccessHandler, iPriority );
        iLazyInitialization = EFalse;
        iRegistered = ETrue;
        if ( iErrorHandler )
            {
            iClient.RegisterErrorObserverL( *iErrorHandler );
            }
        if ( iStatusHandler )
            {
            iClient.RegisterStatusObserverL( iStatusHandler );
            }
        // register all the client interfaces immediately, otherwise
        // client might miss events from network
        TCAInterfaceSignaller::RegisterAllL();
        }
    return &iClient;
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::RegisterL
// Stores values for later registration. Implements lazy initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsAccessClient::RegisterL( MImpsAccessHandler2* aObserver,
                                     TInt aPriority /* = 0  */ )
    {
    iPriority = aPriority;
    iAccessHandler = aObserver;
    iLazyInitialization = ETrue;
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::Unregister
// Unregisters if registered. Clears member variables.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsAccessClient::Unregister()
    {
    if ( iRegistered )
        {
        iClient.Unregister();
        }
    iAccessHandler = NULL;
    iLazyInitialization = ETrue;
    iRegistered = EFalse;
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::LoginL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsAccessClient::LoginL( TImpsCspIdentifier aCspId,
                                  const TDesC& aPsw,
                                  const TDesC& aClientId,
                                  TUint32 aAP,
                                  const TDesC* aKey1 /*= NULL*/,
                                  const TDesC* aKey2 /*= NULL*/ )
    {
    return InterfaceL()->ReactiveLoginL( aCspId, aPsw, aClientId, aAP, aKey1,
                                         aKey2 );
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::LogoutL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsAccessClient::LogoutL()
    {
    return InterfaceL()->LogoutL();
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::NumberOfSessionsL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsAccessClient::NumberOfSessionsL( TImpsCspIdentifier& aCspId )
    {
    return InterfaceL()->NumberOfSessionsL( aCspId );
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::GetServicesL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsAccessClient::GetServicesL( TImpsServices& aServices )
    {
    InterfaceL()->GetServicesL( aServices );
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::RegisterErrorObserverL
// Calls WV Engine if registered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsAccessClient::RegisterErrorObserverL( MImpsErrorHandler2& aObs )
    {
    iErrorHandler = &aObs;
    if ( iRegistered )
        {
        InterfaceL()->RegisterErrorObserverL( *iErrorHandler );
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::UnregisterErrorObserverL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsAccessClient::UnregisterErrorObserverL()
    {
    iErrorHandler = NULL;
    iClient.UnregisterErrorObserverL();
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::RegisterStatusObserverL
// Calls WV Engine if registered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsAccessClient::RegisterStatusObserverL( MImpsStatusHandler2* aObs )
    {
    iStatusHandler = aObs;
    if ( iRegistered )
        {
        InterfaceL()->RegisterStatusObserverL( iStatusHandler );
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::UnregisterStatusObserverL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsAccessClient::UnregisterStatusObserverL()
    {
    iClient.UnregisterStatusObserverL();
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::StatusHandler
// Returns current status handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MImpsStatusHandler2* CCAImpsAccessClient::StatusHandler() const
    {
    return iStatusHandler;
    }

// -----------------------------------------------------------------------------
// CCAImpsAccessClient::ErrorHandler
// Returns current error handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MImpsErrorHandler2* CCAImpsAccessClient::ErrorHandler() const
    {
    return iErrorHandler;
    }

//  End of File
