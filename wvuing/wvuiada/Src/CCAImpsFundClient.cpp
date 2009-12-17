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
* Description:  Implements fundamental services interface.
*
*/



// INCLUDE FILES
#include    "CCAImpsFundClient.h"
#include	"TCAInterfaceSignaller.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAImpsFundClient::CCAImpsFundClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAImpsFundClient::CCAImpsFundClient( RImpsEng& aImpsEng )
        : iImpsEng( aImpsEng )
    {
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAImpsFundClient* CCAImpsFundClient::NewL( RImpsEng& aImpsEng )
    {
    CCAImpsFundClient* self = new( ELeave ) CCAImpsFundClient( aImpsEng );
    return self;
    }

// Destructor
CCAImpsFundClient::~CCAImpsFundClient()
    {
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::InterfaceL
// Returns registered WV Engine interface. Implements lazy initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RImpsFundClient2* CCAImpsFundClient::InterfaceL()
    {
    if ( iLazyInitialization )
        {
        // Connect to IMPS engine (server)
        TCAInterfaceSignaller::ConnectL();

        iClient.RegisterL( iImpsEng, iSearchHandler, iInviteHandler, KNullDesC,
                           ETrue, iPriority );
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
        }
    return &iClient;
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::RegisterL
// Stores values for later registration. Implements lazy initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFundClient::RegisterL( MImpsSearchHandler2* aSearchObs,
                                   MImpsInviteHandler2* aInviteObs, TInt aPriority /* = 0 */ )
    {
    iPriority = aPriority;
    iSearchHandler = aSearchObs;
    iInviteHandler = aInviteObs;
    iLazyInitialization = ETrue;
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::Unregister
// Unregisters if registered. Clears member variables.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFundClient::Unregister()
    {
    if ( iRegistered )
        {
        iClient.Unregister();
        }
    iLazyInitialization = ETrue;
    iRegistered = EFalse;
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::GetServicesL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFundClient::GetServicesL( TImpsServices& aServices )
    {
    InterfaceL()->GetServicesL( aServices );
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::SearchFirstL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsFundClient::SearchFirstL( const CSearchPairs& aPairs,
                                      TInt aSearchLimit )
    {
    return InterfaceL()->SearchFirstL( aPairs, aSearchLimit );
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::SearchNextL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsFundClient::SearchNextL( TInt aSearchID, TInt aIndex )
    {
    return InterfaceL()->SearchNextL( aSearchID, aIndex );
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::StopSearchL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsFundClient::StopSearchL( TInt aSearchID )
    {
    return InterfaceL()->StopSearchL( aSearchID );
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::InviteGroupRequestL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsFundClient::InviteGroupRequestL( const TDesC& aInviteID,
                                             const MDesCArray* aUserID, const MDesCArray* aScreenName,
                                             const MDesCArray* aGroupName, const TDesC& aInviteGroup,
                                             const TDesC& aOwnScreenName, const TDesC& aOwnGroupName,
                                             const TDesC& aInviteReason, const TInt aValidityPeriod )
    {
    return InterfaceL()->GroupInviteL( aInviteID,
                                       aUserID,
                                       aScreenName,
                                       aGroupName,
                                       aInviteGroup,
                                       aOwnScreenName,
                                       aOwnGroupName,
                                       aInviteReason,
                                       aValidityPeriod );
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::CancelGroupInviteRequestL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsFundClient::CancelGroupInviteRequestL( const TDesC& aInviteId,
                                                   const MDesCArray* aUserId, const MDesCArray* aScreenName,
                                                   const MDesCArray* aGroupName, const TDesC& aCancelReason,
                                                   const TDesC& aOwnScreenName, const TDesC& aOwnGroupName )
    {
    return InterfaceL()->CancelInviteL( aInviteId, aUserId, aScreenName,
                                        aGroupName, aCancelReason, aOwnScreenName, aOwnGroupName );
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::GroupInviteUserResponseL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsFundClient::GroupInviteUserResponseL( const TDesC& aInviteId,
                                                  TBool aAcceptance, const TDesC& aInviteResponse,
                                                  const TDesC& aOwnScreenName, const TDesC& aOwnGroupName )
    {
    return InterfaceL()->InviteResponseL( aInviteId, aAcceptance,
                                          aInviteResponse, aOwnScreenName, aOwnGroupName );
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::RegisterErrorObserverL
// Calls WV Engine if registered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFundClient::RegisterErrorObserverL( MImpsErrorHandler2 &aObs )
    {
    iErrorHandler = &aObs;
    if ( iRegistered )
        {
        InterfaceL()->RegisterErrorObserverL( *iErrorHandler );
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::UnregisterErrorObserverL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFundClient::UnregisterErrorObserverL()
    {
    iClient.UnregisterErrorObserverL();
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::RegisterStatusObserverL
// Calls WV Engine if registered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFundClient::RegisterStatusObserverL( MImpsStatusHandler2 *aObs )
    {
    iStatusHandler = aObs;
    if ( iRegistered )
        {
        InterfaceL()->RegisterStatusObserverL( iStatusHandler );
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::UnregisterStatusObserverL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFundClient::UnregisterStatusObserverL()
    {
    iClient.UnregisterStatusObserverL();
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::StatusHandler
// Returns current status handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MImpsStatusHandler2* CCAImpsFundClient::StatusHandler() const
    {
    return iStatusHandler;
    }

// -----------------------------------------------------------------------------
// CCAImpsFundClient::ErrorHandler
// Returns current error handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MImpsErrorHandler2* CCAImpsFundClient::ErrorHandler() const
    {
    return iErrorHandler;
    }

//  End of File
