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
* Description:  AA connection.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "cimpspresenceaaconnectionng.h"
#include "CIMPSPresenceAAConnectionImp.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
EXPORT_C CIMPSPresenceAAConnection* CIMPSPresenceAAConnection::NewL(
    TIMPSConnectionClient aClient,
    CIMPSSAPSettingsStore* aSapStore )
    {
    CIMPSPresenceAAConnection* self = CIMPSPresenceAAConnection::NewLC( aClient, aSapStore );
    CleanupStack::Pop( self );  //self
    return self;
    }


// Two-phased constructor.
EXPORT_C CIMPSPresenceAAConnection* CIMPSPresenceAAConnection::NewLC(
    TIMPSConnectionClient aClient,
    CIMPSSAPSettingsStore* aSapStore )
    {
    CIMPSPresenceAAConnection* self = new ( ELeave  ) CIMPSPresenceAAConnection;
    CleanupStack::PushL( self );
    self->ConstructL( aClient, aSapStore );
    return self;
    }


// Destructor
EXPORT_C CIMPSPresenceAAConnection::~CIMPSPresenceAAConnection()
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceAAConnection::~CIMPSPresenceAAConnection()" );
    delete iImp;
    }


// C++ constructor
CIMPSPresenceAAConnection::CIMPSPresenceAAConnection()
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceAAConnection::CIMPSPresenceAAConnection()" );
    }


// Symbian OS constructor
void CIMPSPresenceAAConnection::ConstructL( TIMPSConnectionClient aClient,
                                            CIMPSSAPSettingsStore* aSapStore )
    {
    // set the client information to the TLS
    User::LeaveIfError( Dll::SetTls( reinterpret_cast<TAny*>( aClient ) ) );
    iImp = CIMPSPresenceAAConnectionImp::NewL( aSapStore );
    }



// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnection::LoginL()
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSPresenceAAConnection::LoginL( TIMPSConnectionClient aClient,
                                                 TBool aShowDetailedError /* = EFalse */ )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceAAConnection::LoginL()" );
    return iImp->LoginL( aClient, aShowDetailedError );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnection::LogoutL()
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSPresenceAAConnection::LogoutL( TIMPSConnectionClient aClient,
                                                  TBool aIsScheduled /* = EFalse */ )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceAAConnection::LogoutL()" );
    iImp->LogoutL( aClient, aIsScheduled );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnection::LoggedInL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIMPSPresenceAAConnection::LoggedInL( TIMPSConnectionClient aClient )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceAAConnection::LoggedInL()" );
    return iImp->LoggedInL( aClient );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnection::PureServiceStatusL
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotState CIMPSPresenceAAConnection::PureServiceStatusL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceAAConnection::PureServiceStatusL()" );
    return iImp->PureServiceStatusL( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnection::DefaultServerNameL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CIMPSPresenceAAConnection::DefaultServerNameL( TIMPSConnectionClient aClient )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceAAConnection::DefaultServerNameL()" );
    return iImp->DefaultServerNameL( aClient );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnection::DefaultServerNameL
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotID2* CIMPSPresenceAAConnection::GetActiveNWSessionSlotIDL(
    TIMPSConnectionClient aClient )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceAAConnection::DefaultServerNameL()" );
    return iImp->GetActiveNWSessionSlotIDL( aClient );
    }
//  End of File




