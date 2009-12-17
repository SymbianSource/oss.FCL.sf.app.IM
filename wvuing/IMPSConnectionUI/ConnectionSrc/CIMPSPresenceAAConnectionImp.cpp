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
* Description:  AA connection implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsStore.h>
#include <CPEngNWSessionSlotManager2.h>
#include <CPEngNWSessionSlotID2.h>
#include <sysutil.h>
#include "CIMPSPresenceAAConnectionImp.h"
#include "MCnUiConnectionHandler.h"
#include "CCnUiBaseControlContext.h"
#include "IMPSCommonUiDebugPrint.h"



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CIMPSPresenceAAConnectionImp* CIMPSPresenceAAConnectionImp::NewL(
    CIMPSSAPSettingsStore* aSapStore )
    {
    CIMPSPresenceAAConnectionImp* self = new ( ELeave ) CIMPSPresenceAAConnectionImp( aSapStore );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self

    return self;
    }


// Destructor
CIMPSPresenceAAConnectionImp::~CIMPSPresenceAAConnectionImp()
    {
    delete iConnHandler;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSPresenceAAConnectionImp::CIMPSPresenceAAConnectionImp( CIMPSSAPSettingsStore* aSapStore )
        : iSapStoreProxy( aSapStore )
    {
    }


// Symbian OS default constructor can leave.
void CIMPSPresenceAAConnectionImp::ConstructL()
    {
    iConnHandler = CreateConnHandlerL( iSapStoreProxy );
    }



// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnectionImp::LoginL()
// -----------------------------------------------------------------------------
//
TInt CIMPSPresenceAAConnectionImp::LoginL( TIMPSConnectionClient aClient,
                                           TBool aShowDetailedError /* = EFalse */ )
    {
    // Check disk space
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( NULL, 0 ) )
        {
        // Don't show any own notes here
        User::Leave( KErrDiskFull );
        }

    TInt retStatus = KErrNone;
    CCnUiBaseControlContext* cc = CCnUiBaseControlContext::NewLC( aClient,
                                                                  iSapStoreProxy,
                                                                  *iConnHandler );

    retStatus = cc->DoAALoginL( aShowDetailedError );

    CleanupStack::PopAndDestroy( cc ); //cc
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSPresenceAAConnectionImp::LoginL( %d ), retStatus=%d" ), aClient, retStatus );
    return retStatus;
    }



// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnectionImp::LogoutL()
// -----------------------------------------------------------------------------
//
TInt CIMPSPresenceAAConnectionImp::LogoutL( TIMPSConnectionClient aClient,
                                            TBool aIsScheduled /* EFalse */ )
    {
    CCnUiBaseControlContext* cc = CCnUiBaseControlContext::NewLC( aClient,
                                                                  iSapStoreProxy,
                                                                  *iConnHandler );
    cc->DoAALogoutL( aIsScheduled );
    CleanupStack::PopAndDestroy( cc ); //cc

    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSPresenceAAConnectionImp::LogoutL( %d )" ), aClient );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnectionImp::LoggedInL()
// -----------------------------------------------------------------------------
//
TBool CIMPSPresenceAAConnectionImp::LoggedInL( TIMPSConnectionClient aClient )
    {
    return iConnHandler->TheClientLoggedInL( aClient );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnectionImp::PureServiceStatusL()
// -----------------------------------------------------------------------------
//
TPEngNWSessionSlotState CIMPSPresenceAAConnectionImp::PureServiceStatusL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    return iConnHandler->ServiceStatusL( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnectionImp::DefaultServerNameL()
// -----------------------------------------------------------------------------
//
HBufC* CIMPSPresenceAAConnectionImp::DefaultServerNameL( TIMPSConnectionClient aClient )
    {
    CIMPSSAPSettings* sapSettings = CIMPSSAPSettings::NewLC();

    TIMPSAccessGroup accessGroup;
    if ( aClient == EIMPSConnClientPEC )
        {
        accessGroup = EIMPSPECAccessGroup;
        }
    else
        {
        accessGroup = EIMPSIMAccessGroup;
        }

    // get the default sap
    iSapStoreProxy.SapStoreL().GetDefaultL( sapSettings, accessGroup );

    // get the sap name
    HBufC* sapName = sapSettings->SAPName().AllocL();

    CleanupStack::PopAndDestroy( sapSettings );

    return sapName;
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnectionImp::GetActiveNWSessionSlotIDL()
//
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2* CIMPSPresenceAAConnectionImp::GetActiveNWSessionSlotIDL(
    TIMPSConnectionClient aClient )
    {
    CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();

    CPEngNWSessionSlotID2* pattern = CPEngNWSessionSlotID2::NewLC();
    pattern->SetServiceAddressMatchAnyL();
    pattern->SetUserIdMatchAnyL();
    if ( aClient == EIMPSConnClientPEC )
        {
        pattern->SetAppIdL( KPEngAppIdPEC );
        }
    else
        {
        pattern->SetAppIdL( KPEngAppIdIM );
        }

    RPointerArray< CPEngNWSessionSlotID2 > array;
    TInt error( slotManager->GetNWSessionSlots( array,
                                                *pattern,
                                                EPEngNWPresenceSessionOpen ) );

    if ( error != KErrNone )
        {
        array.ResetAndDestroy();
        User::Leave( error );
        }
    CleanupStack::PopAndDestroy( 2, slotManager ); // pattern, slotManager
    CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &array ) );

    CPEngNWSessionSlotID2* slotID = NULL;

    if ( array.Count() > 0 )
        {
        // only one active slot per application at the moment
        CPEngNWSessionSlotID2* tempSlotID = array[ 0 ];

        slotID = tempSlotID->CloneL();
        }
    else
        {
        // not found
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy(); // array
    return slotID;
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceAAConnectionImp::DestroyCloseModelArray()
//
// -----------------------------------------------------------------------------
//
void CIMPSPresenceAAConnectionImp::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast< RPointerArray< CPEngNWSessionSlotID2 >* >( aObject )->ResetAndDestroy();
    }

//  End of File

