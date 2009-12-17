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
* Description:  Access handler of the network session
*
*/

// INCLUDE FILES
#include    "CPEngSessionManager.h"

#include	"CPEngSessionSlotId.h"
#include	"MPEngTransAdapFactory.h"
#include	"CPEngAccessHandler.h"
#include	"CPEngPureDataHandler.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSessionManager::CPEngSessionManager
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngSessionManager::CPEngSessionManager(
    MPEngTransAdapFactory& aFactory )
        : iFactory( aFactory )
    {
    iFactory.OpenSessionCount();
    }

// -----------------------------------------------------------------------------
// CPEngSessionManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngSessionManager::ConstructL(
    const CPEngSessionSlotId& aSesssId )
    {
    iSessionId = aSesssId.CloneL();
    User::LeaveIfError( iImpsClient.Connect() );
    }


// -----------------------------------------------------------------------------
// CPEngSessionManager::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSessionManager* CPEngSessionManager::NewLC(
    MPEngTransAdapFactory& aFactory,
    const CPEngSessionSlotId& aSesssId )
    {
    CPEngSessionManager* self = new ( ELeave ) CPEngSessionManager( aFactory );
    CleanupClosePushL( *self );
    self->ConstructL( aSesssId );
    return self;
    }


// Destructor
CPEngSessionManager::~CPEngSessionManager()
    {
    iFactory.CloseSession( this );
    delete iSessionId;
    iImpsClient.Close();
    delete iAccessHandler;
    delete iPureDataHandler;
    }


// =============================================================================
// =============== New Functions of the MPEngSessionManager ====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSessionManager::CloseAccessHandler
// -----------------------------------------------------------------------------
//
void CPEngSessionManager::CloseAccessHandler( )
    {
    iAccessHandler = NULL;
    Close();
    }


// -----------------------------------------------------------------------------
// CPEngSessionManager::ClosePureDataHandler
// -----------------------------------------------------------------------------
//
void CPEngSessionManager::ClosePureDataHandler()
    {
    iPureDataHandler = NULL;
    Close();
    }


// -----------------------------------------------------------------------------
// CPEngSessionManager::OpenRef()
// -----------------------------------------------------------------------------
//
void CPEngSessionManager::OpenRef()
    {
    Open();
    }


// =============================================================================
// =============== New Functions of the main class =============================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngSessionManager::AccessHandler
// -----------------------------------------------------------------------------
//
MPEngAccessHandler* CPEngSessionManager::AccessHandlerL()
    {
    if ( iAccessHandler )
        {
        iAccessHandler->Open();         // CSI: 15,65 #
        }
    else
        {
        iAccessHandler = CPEngAccessHandler::NewL( iImpsClient,
                                                   *this,
                                                   *iSessionId );
        }
    return iAccessHandler;
    }


// -----------------------------------------------------------------------------
// CPEngSessionManager::PureDataHandler
// -----------------------------------------------------------------------------
//
MPEngPureDataHandler* CPEngSessionManager::PureDataHandlerL()
    {
    if ( iPureDataHandler )
        {
        iPureDataHandler->Open();       // CSI: 15,65 #
        }
    else
        {
        AccessHandlerL();
        CleanupClosePushL( *iAccessHandler );
        iPureDataHandler = CPEngPureDataHandler::NewL( iImpsClient,
                                                       *this,
                                                       *iAccessHandler,
                                                       *iSessionId );
        CleanupStack::PopAndDestroy(); // iAccessHandler
        }
    return iPureDataHandler;
    }


// -----------------------------------------------------------------------------
// CPEngSessionManager::SessionId
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngSessionManager::SessionId() const
    {
    return *iSessionId;
    }

// End of File


