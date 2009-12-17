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
* Description:  Implements instant messaging services interface.
*
*/



// INCLUDE FILES
#include    "CCAImpsImClient.h"
#include	"TCAInterfaceSignaller.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAImpsImClient::CCAImpsImClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAImpsImClient::CCAImpsImClient( RImpsEng& aImpsEng )
        : iImpsEng( aImpsEng )
    {
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAImpsImClient* CCAImpsImClient::NewL( RImpsEng& aImpsEng )
    {
    CCAImpsImClient* self = new( ELeave ) CCAImpsImClient( aImpsEng );
    return self;
    }


// Destructor
CCAImpsImClient::~CCAImpsImClient()
    {
    }


// -----------------------------------------------------------------------------
// CCAImpsImClient::InterfaceL
// Returns registered WV Engine interface. Implements lazy initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RImpsImClient2* CCAImpsImClient::InterfaceL()
    {
    if ( iLazyInitialization )
        {
        // Connect to IMPS engine (server)
        TCAInterfaceSignaller::ConnectL();

        iClient.RegisterL( iImpsEng, iImHandler, iBlockingHandler, KNullDesC,
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
// CCAImpsImClient::RegisterL
// Stores values for later registration. Implements lazy initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsImClient::RegisterL( MImpsImHandler2* aImpsObserver,
                                 MImpsBlockingHandler2* aBlockingHandler, TUid aApplicationId,
                                 TInt aPriority /* = 0 */ )
    {
    iPriority = aPriority;
    iApplicationId = aApplicationId;
    iImHandler = aImpsObserver;
    iBlockingHandler = aBlockingHandler;
    iLazyInitialization = ETrue;
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::Unregister
// Unregisters if registered. Clears member variables.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsImClient::Unregister()
    {
    if ( iRegistered )
        {
        iClient.Unregister();
        }
    iLazyInitialization = ETrue;
    iRegistered = EFalse;
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::GetServicesL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsImClient::GetServicesL( TImpsServices& aServices )
    {
    InterfaceL()->GetServicesL( aServices );
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::SendTextMessageL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsImClient::SendTextMessageL( const TDesC* aSenderSn,
                                        const MDesCArray* aUserIds,
                                        const TDesC* aGroupId,
                                        const MDesCArray* aScreenNames,
                                        const TDesC& aContent,
                                        TBool aDeliveryReportWanted )
    {
    return InterfaceL()->SendTextMessageL(  aSenderSn,
                                            aUserIds,
                                            aGroupId,
                                            aScreenNames,
                                            aContent,
                                            aDeliveryReportWanted );
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::SendContentMessageL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsImClient::SendContentMessageL( const TDesC* aSenderSn,
                                           const MDesCArray* aUserIds,
                                           const TDesC* aGroupId,
                                           const MDesCArray* aScreenNames,
                                           const TDesC& aContentType,
                                           const TDesC8& aContent,
                                           TBool aDeliveryReportWanted )
    {
    return InterfaceL()->SendContentMessageL(  aSenderSn,
                                               aUserIds,
                                               aGroupId,
                                               aScreenNames,
                                               aContentType,
                                               aContent,
                                               aDeliveryReportWanted );
    }
// -----------------------------------------------------------------------------
// CCAImpsImClient::BlockEntityRequestL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsImClient::BlockEntityRequestL( const CDesCArray* aBlockEntity,
                                           const CDesCArray* aUnBlockEntity,
                                           TBool aBlockedListInUse,
                                           const CDesCArray* aGrantedEntityList,
                                           const CDesCArray* aUnGrantEntity,
                                           TBool aGrantedListInUse )
    {
    return InterfaceL()->BlockEntityRequestL( aBlockEntity,
                                              aUnBlockEntity,
                                              aBlockedListInUse,
                                              aGrantedEntityList,
                                              aUnGrantEntity,
                                              aGrantedListInUse );
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::GetBlockedListRequestL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsImClient::GetBlockedListRequestL()
    {
    return InterfaceL()->GetBlockedListRequestL();
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::RegisterErrorObserverL
// Calls WV Engine if registered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsImClient::RegisterErrorObserverL( MImpsErrorHandler2 &aObs )
    {
    iErrorHandler = &aObs;
    if ( iRegistered )
        {
        InterfaceL()->RegisterErrorObserverL( *iErrorHandler );
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::RegisterStatusObserverL
// Calls WV Engine if registered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsImClient::RegisterStatusObserverL( MImpsStatusHandler2* aObs )
    {
    iStatusHandler = aObs;
    if ( iRegistered )
        {
        InterfaceL()->RegisterStatusObserverL( iStatusHandler );
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::UnregisterErrorObserverL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsImClient::UnregisterErrorObserverL()
    {
    iClient.UnregisterErrorObserverL();
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::MaxTransactionContentLengthL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsImClient::MaxTransactionContentLengthL( )
    {
    return iClient.MaxTransactionContentLengthL();
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::UnregisterStatusObserverL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsImClient::UnregisterStatusObserverL()
    {
    iClient.UnregisterStatusObserverL();
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::StatusHandler
// Returns current status handler pointer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MImpsStatusHandler2* CCAImpsImClient::StatusHandler() const
    {
    return iStatusHandler;
    }

// -----------------------------------------------------------------------------
// CCAImpsImClient::ErrorHandler
// Returns current error handler pointer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MImpsErrorHandler2* CCAImpsImClient::ErrorHandler() const
    {
    return iErrorHandler;
    }

//  End of File
