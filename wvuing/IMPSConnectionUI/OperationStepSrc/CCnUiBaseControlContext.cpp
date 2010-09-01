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
* Description:  Basic control context.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlotManager2.h>
#include <CPEngNWSessionSlot2.h>
#include <PEngPresenceEngineConsts2.h>

#include "CCnUiBaseControlContext.h"
#include "CCnUiSapStoreProxy.h"
#include "CCnUiCntrlStepDriver.h"

#include "MCnUiClientPlugin.h"
#include "MCnUiSignaller.h"
#include "MCnUiConnModeHandler.h"
#include "MCnUiGlobalNotificationUiFacade.h"

#include "CCnUiAALoginCntrlStep.h"
#include "CCnUiAALogoutCntrlStep.h"

#include "IMPSCommonUiDebugPrint.h"
#include "impspresenceconnectionuiconstsng.h"


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiBaseControlContext* CCnUiBaseControlContext::NewLC( TIMPSConnectionClient aClient,
                                                         CCnUiSapStoreProxy& aSapStoreProxy,
                                                         MCnUiConnectionHandler& aConnHandler )
    {
    CCnUiBaseControlContext* self = new ( ELeave ) CCnUiBaseControlContext( aClient,
                                                                            aSapStoreProxy,
                                                                            aConnHandler );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
CCnUiBaseControlContext::~CCnUiBaseControlContext()
    {
    //static resources
    delete iSignaller;
    delete iConnModeHandler;

    //dynamic resources
    delete iPluginPEC;
    delete iPluginIM;
    delete iGNUI;

    //finally, release the SAP store, if it was needed & created during this
    //operation
    iSapStoreProxy.ReleaseOwnedSapStore();

    delete iAASessionSlotID;
    delete iAASessionSlot;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiBaseControlContext::CCnUiBaseControlContext( TIMPSConnectionClient aClient,
                                                  CCnUiSapStoreProxy& aSapStoreProxy,
                                                  MCnUiConnectionHandler& aConnHandler )
        : iControlledClient( aClient ),
        iSapStoreProxy( aSapStoreProxy ),
        iConnHandler( aConnHandler )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiBaseControlContext::ConstructL()
    {
    //static resources
    iSignaller = CreateConnUiSignallerL();

    iConnModeHandler = CreateConnModeHandlerL();

    //dynamic resources (e.g. client plug-in's)
    //are lazy initialized on the fly when needed
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::DoAALoginL()
// -----------------------------------------------------------------------------
//
TInt CCnUiBaseControlContext::DoAALoginL( TBool aShowDetailedError )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiBaseControlContext::DoAALoginL( %d )" ), aShowDetailedError );
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();
    CCnUiCntrlStepDriver* driver = CCnUiCntrlStepDriver::NewLC();

    CPEngNWSessionSlotID2* tempID = CPEngNWSessionSlotID2::NewL();
    delete iAASessionSlotID;
    iAASessionSlotID = tempID;

    driver->AppendStepL( CCnUiAALoginCntrlStep::NewLC( *this,
                                                       *sap,
                                                       aShowDetailedError,
                                                       *iAASessionSlotID ) );
    CleanupStack::Pop(); // CCnUiStepAALoginControl instance

    //and run
    TInt status = driver->ExecuteL();
    CleanupStack::PopAndDestroy( 2 ); //driver & sap
    if ( status == KErrNone )
        {
        CPEngNWSessionSlot2* tempSlot = CPEngNWSessionSlot2::NewL( *iAASessionSlotID );
        delete iAASessionSlot;
        iAASessionSlot = tempSlot;
        iAASessionSlot->OpenNWPresenceSessionOwnership();
        }
    return status;
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::DoAALogoutL()
// -----------------------------------------------------------------------------
//
TInt CCnUiBaseControlContext::DoAALogoutL( TBool aIsScheduled )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiBaseControlContext::DoAALogoutL( %d )" ), aIsScheduled );
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();
    CCnUiCntrlStepDriver* driver = CCnUiCntrlStepDriver::NewLC();

    CPEngNWSessionSlotID2* tempSlot = GetActiveNWSessionSlotIDL( iControlledClient );
    delete iAASessionSlotID;
    iAASessionSlotID = tempSlot;

    driver->AppendStepL( CCnUiAALogoutCntrlStep::NewLC( *this,
                                                        *sap,
                                                        aIsScheduled,
                                                        *iAASessionSlotID ) );
    CleanupStack::Pop(); // CCnUiAALogoutCntrlStep instance

    //and run
    TInt status = driver->ExecuteL();
    CleanupStack::PopAndDestroy( 2 ); //driver & sap
    if ( status == KErrNone )
        {
        if ( iAASessionSlot )
            {
            iAASessionSlot->CloseNWPresenceSessionOwnership();
            }
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::ConnHandler()
// -----------------------------------------------------------------------------
//
MCnUiConnectionHandler& CCnUiBaseControlContext::ConnHandler()
    {
    return iConnHandler;
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::Signaller()
// -----------------------------------------------------------------------------
//
MCnUiSignaller& CCnUiBaseControlContext::Signaller()
    {
    return *iSignaller;
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::ConnModeHandler()
// -----------------------------------------------------------------------------
//
MCnUiConnModeHandler& CCnUiBaseControlContext::ConnModeHandler()
    {
    return *iConnModeHandler;
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::ControlledClient()
// -----------------------------------------------------------------------------
//
TIMPSConnectionClient CCnUiBaseControlContext::ControlledClient()
    {
    return iControlledClient;
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::SapStoreL()
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsStore& CCnUiBaseControlContext::SapStoreL()
    {
    return iSapStoreProxy.SapStoreL();
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::ClientPluginL()
// -----------------------------------------------------------------------------
//
MCnUiClientPlugin& CCnUiBaseControlContext::ClientPluginL(
    TIMPSConnectionClient aClient,
    CPEngNWSessionSlotID2& aNWSessionSlotID,
    TBool aRefreshPlugin /* = EFalse */ )
    {
    MCnUiClientPlugin* matchedPlugin = NULL;

    switch ( aClient )
        {
        case EIMPSConnClientPEC:
            {
            if ( !iPluginPEC )
                {
                iPluginPEC = CreateClientPluginL( EIMPSConnClientPEC, aNWSessionSlotID );
                }
            matchedPlugin = iPluginPEC;
            break;
            }

        case EIMPSConnClientIM:
            {
            if ( !iPluginIM || aRefreshPlugin )
                {
                MCnUiClientPlugin* tempPlugin =
                    CreateClientPluginL( EIMPSConnClientIM, aNWSessionSlotID );
                delete iPluginIM;
                iPluginIM = tempPlugin;
                }
            matchedPlugin = iPluginIM;
            break;
            }

        default:
            {
            User::Leave( KErrUnknown );
            break;
            }
        }

    return *matchedPlugin;
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::GlobalNotificationUiL()
// -----------------------------------------------------------------------------
//
MCnUiGlobalNotificationUiFacade& CCnUiBaseControlContext::GlobalNotificationUiL()
    {
    if ( !iGNUI )
        {
        iGNUI = CreateGlobalNotificationUiFacadeL();
        }

    return *iGNUI;
    }


// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::GetActiveNWSessionSlotIDL()
//
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2* CCnUiBaseControlContext::GetActiveNWSessionSlotIDL(
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
    User::LeaveIfError( slotManager->GetNWSessionSlots( array,
                                                        *pattern,
                                                        EPEngNWPresenceSessionOpen ) );
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
    CleanupStack::PopAndDestroy();
    return slotID;
    }

// -----------------------------------------------------------------------------
// CCnUiBaseControlContext::DestroyCloseModelArray()
//
// -----------------------------------------------------------------------------
//
void CCnUiBaseControlContext::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast< RPointerArray< CPEngNWSessionSlotID2 >* >( aObject )->ResetAndDestroy();
    }

//  End of File


