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
* Description:  Implements group services interface.
*
*/



// INCLUDE FILES
#include    "CCAImpsGroupClient.h"
#include	"TCAInterfaceSignaller.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::CCAImpsGroupClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAImpsGroupClient::CCAImpsGroupClient( RImpsEng& aImpsEng )
        : iImpsEng( aImpsEng )
    {
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAImpsGroupClient* CCAImpsGroupClient::NewL( RImpsEng& aImpsEng )
    {
    CCAImpsGroupClient* self = new ( ELeave ) CCAImpsGroupClient( aImpsEng );
    return self;
    }


// Destructor
CCAImpsGroupClient::~CCAImpsGroupClient()
    {
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::InterfaceL
// Returns registered WV Engine interface. Implements lazy initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RImpsGroupClient2* CCAImpsGroupClient::InterfaceL()
    {
    if ( iLazyInitialization )
        {
        // Connect to IMPS engine (server)
        TCAInterfaceSignaller::ConnectL();

        iClient.RegisterL( iImpsEng, iGroupHandler, KNullDesC, ETrue,
                           iPriority );
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
// CCAImpsGroupClient::RegisterL
// Stores values for later registration. Implements lazy initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsGroupClient::RegisterL( MImpsGroupHandler2* aObserver,
                                    TUid aApplicationId, TInt aPriority /* = 0  */ )
    {
    iPriority = aPriority;
    iApplicationId = aApplicationId;
    iGroupHandler = aObserver;
    iLazyInitialization = ETrue;
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::Unregister
// Unregisters if registered. Clears member variables.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsGroupClient::Unregister()
    {
    if ( iRegistered )
        {
        iClient.Unregister();
        }
    iLazyInitialization = ETrue;
    iRegistered = EFalse;
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::AddMembersL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::AddMembersL( const TDesC& aGroupId,
                                      const MDesCArray& aUserList, const MDesCArray& aScreenNameList )
    {
    return InterfaceL()->AddMembersL( aGroupId, aUserList, aScreenNameList );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::CheckSubscriptionL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::CheckSubscriptionL( const TDesC& aGroupId )
    {
    return InterfaceL()->CheckSubscriptionL( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::CreateGroupL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::CreateGroupL( const TDesC& aGroupId,
                                       const CImpsCommonGroupProps* aProperties, const TDesC& aScreenName,
                                       TBool aJoinGroup, TBool aSubscribeNotification /* = EFalse  */ )
    {
    return InterfaceL()->CreateGroupL( aGroupId, aProperties, aScreenName,
                                       aJoinGroup, aSubscribeNotification /* = EFalse  */ );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::DeleteGroupL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::DeleteGroupL( const TDesC& aGroupId )
    {
    return InterfaceL()->DeleteGroupL( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::GroupMembersL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::GroupMembersL( const TDesC& aGroupId )
    {
    return InterfaceL()->GroupMembersL( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::GroupPropertiesL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::GroupPropertiesL( const TDesC& aGroupId )
    {
    return InterfaceL()->GroupPropertiesL( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::JoinGroupL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::JoinGroupL(
    const TDesC& aGroupId,
    const TDesC& aScreenName,
    TBool aUsers,
    TBool aSubscribeNotification /* = EFalse  */ )
    {
    return InterfaceL()->JoinGroupL( aGroupId, aScreenName, aUsers,
                                     aSubscribeNotification /* = EFalse  */ );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::LeaveGroupL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::LeaveGroupL( const TDesC& aGroupId )
    {
    return InterfaceL()->LeaveGroupL( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::ModifyMembersAccessL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::ModifyMembersAccessL( const TDesC& aGroupId,
                                               const MDesCArray* aAdminList, const MDesCArray* aModerList,
                                               const MDesCArray* aOrdinaryList )
    {
    return InterfaceL()->ModifyMembersAccessL( aGroupId, aAdminList, aModerList,
                                               aOrdinaryList );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::RemoveMembersL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::RemoveMembersL( const TDesC& aGroupId,
                                         const MDesCArray& aUserList )
    {
    return InterfaceL()->RemoveMembersL( aGroupId, aUserList );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::SetGroupPropertiesL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::SetGroupPropertiesL( const TDesC& aGroupId,
                                              const CImpsCommonGroupProps* aGroupProps,
                                              const CImpsPrivateGroupProps* aOwnProps )
    {
    return InterfaceL()->SetGroupPropertiesL( aGroupId, aGroupProps, aOwnProps );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::SetRejectListL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::SetRejectListL( const TDesC& aGroupId,
                                         const MDesCArray* aRejectedList, const MDesCArray* aEnabledList )
    {
    return InterfaceL()->SetRejectListL( aGroupId, aRejectedList, aEnabledList );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::SubscribeL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::SubscribeL( const TDesC& aGroupId )
    {
    return InterfaceL()->SubscribeL( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::UnsubscribeL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAImpsGroupClient::UnsubscribeL( const TDesC& aGroupId )
    {
    return InterfaceL()->UnsubscribeL( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::GetServicesL
// Forwards the call to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsGroupClient::GetServicesL( TImpsServices& aServices )
    {
    InterfaceL()->GetServicesL( aServices );
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::RegisterErrorObserverL
// Calls WV Engine if registered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsGroupClient::RegisterErrorObserverL( MImpsErrorHandler2 &aObs )
    {
    iErrorHandler = &aObs;
    if ( iRegistered )
        {
        InterfaceL()->RegisterErrorObserverL( *iErrorHandler );
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::UnregisterErrorObserverL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsGroupClient::UnregisterErrorObserverL()
    {
    iClient.UnregisterErrorObserverL();
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::RegisterStatusObserverL
// Calls WV Engine if registered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsGroupClient::RegisterStatusObserverL( MImpsStatusHandler2 *aObs )
    {
    iStatusHandler = aObs;
    if ( iRegistered )
        {
        InterfaceL()->RegisterStatusObserverL( iStatusHandler );
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::UnregisterStatusObserverL
// Calls WV Engine always.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsGroupClient::UnregisterStatusObserverL()
    {
    iClient.UnregisterStatusObserverL();
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::StatusHandler
// Returns current status handler pointer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MImpsStatusHandler2* CCAImpsGroupClient::StatusHandler() const
    {
    return iStatusHandler;
    }

// -----------------------------------------------------------------------------
// CCAImpsGroupClient::ErrorHandler
// Returns current error handler pointer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MImpsErrorHandler2* CCAImpsGroupClient::ErrorHandler() const
    {
    return iErrorHandler;
    }

//  End of File
