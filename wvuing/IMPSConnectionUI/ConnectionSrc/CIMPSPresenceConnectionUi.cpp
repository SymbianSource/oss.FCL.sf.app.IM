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
* Description:  Connection UI.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "cimpspresenceconnectionuing.h"
#include "CIMPSPresenceConnectionUiImp.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
EXPORT_C CIMPSPresenceConnectionUi* CIMPSPresenceConnectionUi::NewL(
    TIMPSConnectionClient aClient,
    CIMPSSAPSettingsStore* aSapStore )
    {
    CIMPSPresenceConnectionUi* self = CIMPSPresenceConnectionUi::NewLC( aClient, aSapStore );
    CleanupStack::Pop( self );  //self
    return self;
    }


// Two-phased constructor.
EXPORT_C CIMPSPresenceConnectionUi* CIMPSPresenceConnectionUi::NewLC(
    TIMPSConnectionClient aClient,
    CIMPSSAPSettingsStore* aSapStore )
    {
    CIMPSPresenceConnectionUi* self = new ( ELeave  ) CIMPSPresenceConnectionUi( aClient );
    CleanupStack::PushL( self );
    self->ConstructL( aSapStore );
    return self;
    }


// Destructor
EXPORT_C CIMPSPresenceConnectionUi::~CIMPSPresenceConnectionUi()
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::~CIMPSPresenceConnectionUi()" );
    delete iImp;
    }


// C++ constructor
CIMPSPresenceConnectionUi::CIMPSPresenceConnectionUi( TIMPSConnectionClient aClient )
        : iClient( aClient )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::CIMPSPresenceConnectionUi()" );
    }


// Symbian OS constructor
void CIMPSPresenceConnectionUi::ConstructL( CIMPSSAPSettingsStore* aSapStore )
    {
    iImp = CIMPSPresenceConnectionUiImp::NewL( iClient, aSapStore );
    }



// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::LoginL()
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSPresenceConnectionUi::LoginL( TIMPSConnectionClient aClient,
                                                 TIMPSLoginType aLoginType,
                                                 CPEngNWSessionSlotID2& aNWSessionSlotID,
                                                 MIMPSConnProcessObserver* aObserver,
                                                 CIMPSSAPSettings* aSap /*= NULL*/,
                                                 TBool aLastLoginSap /*= EFalse*/ )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::LoginL()" );

    // add two extra param if user want to login to last server only
    return iImp->LoginL( aClient,
                         aLoginType,
                         aNWSessionSlotID,
                         aObserver,
                         aSap,
                         aLastLoginSap );
    }



// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::LogoutL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSPresenceConnectionUi::LogoutL( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                                  MIMPSConnProcessObserver* aObserver )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::LogoutL()" );
    return iImp->LogoutL( aNWSessionSlotID,
                          aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::HandleApplicationExitL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSPresenceConnectionUi::HandleApplicationExitL(
    TIMPSConnectionClient aClient,
    TIMPSExitType aExitType,
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    MIMPSConnProcessObserver* aObserver )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::HandleApplicationExitL()" );
    return iImp->HandleApplicationExitL( aClient, aExitType, aNWSessionSlotID, aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::GetActiveNWSessionSlotIDL
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotID2* CIMPSPresenceConnectionUi::GetActiveNWSessionSlotIDL(
    TIMPSConnectionClient aClient )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::GetActiveNWSessionSlotIDs()" );
    return iImp->GetActiveNWSessionSlotIDL( aClient );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::GetLoggedInSapL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSPresenceConnectionUi::GetLoggedInSapL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    CIMPSSAPSettings& aSap )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::LoggedInSapL()" );
    return iImp->GetLoggedInSapL( aNWSessionSlotID, aSap );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::PureServiceStatusL
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotState CIMPSPresenceConnectionUi::PureServiceStatusL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::PureServiceStatusL()" );
    return iImp->PureServiceStatusL( aNWSessionSlotID );
    }



// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::CurrentConnectionOperationL
// -----------------------------------------------------------------------------
//
EXPORT_C TIMPSConnectionOperation CIMPSPresenceConnectionUi::CurrentConnectionOperationL(
    HBufC*& aAddtionalDesc )
    {
    IMPSCUI_DP_TXT( "CIMPSPresenceConnectionUi::CurrentConnectionOperationL()" );
    return iImp->CurrentConnectionOperationL( aAddtionalDesc );
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::LoggedInL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIMPSPresenceConnectionUi::LoggedInL( TIMPSConnectionClient aClient )
    {
    return iImp->LoggedInL( aClient );
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUi::DisplayDomainSelectionQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSPresenceConnectionUi::DisplayDomainSelectionQueryL(
    TDes& aSelectedDomain,
    CIMPSSAPSettings* aSap )
    {
    return iImp->DisplayDomainSelectionQueryL( aSelectedDomain, aSap );
    }

//  End of File




