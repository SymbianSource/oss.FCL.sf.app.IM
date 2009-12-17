/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  it is used to update session slot state and signal new
*                session slot events
*
*/


// INCLUDE FILES
#include "RPEngStorageAdminSubClient.h"

#include "RPEngStorageAdminClient.h"
#include "TPEngStorageServerMessages.h"

#include "PresenceDebugPrint.h"

// PEC Engine internal constants
#include "PEngInternalGlobalConsts.h"


// MACROS
#define RETURN_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return KErrDisconnected;\
        }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPEngStorageAdminSubClient::RPEngStorageAdminSubClient
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngStorageAdminSubClient::RPEngStorageAdminSubClient( )
    {
    }


// Destructor
EXPORT_C RPEngStorageAdminSubClient::~RPEngStorageAdminSubClient()
    {
    }


// =============================================================================
// ============================ Close/Connect FUNCTIONS ========================
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngStorageAdminSubClient::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngStorageAdminSubClient::Connect(
    RPEngStorageAdminClient& aAdminClient,
    const TDesC8& aSessionName )
    {
    PENG_DP( D_PENG_LIT( "RPEngStorageAdminSubClient::Connect()" ) );

    TIpcArgs messArguments;
    messArguments.Set( KMessageSlot0, &aSessionName );

    TInt err( RSubSessionBase::CreateSubSession(
                  aAdminClient,
                  EMainSessCreateAdminSubSession,
                  messArguments ) );

    iConnected = ( err == KErrNone );
    return err;
    }


// -----------------------------------------------------------------------------
// RPEngStorageAdminSubClient::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngStorageAdminSubClient::Close()
    {
    PENG_DP( D_PENG_LIT( "RPEngStorageAdminSubClient::Close()" ) );

    // all requests from this sub-session are canceled by framework
    CloseSubSession( EMainSessCloseSubSession );
    iConnected = EFalse;
    }


// =============================================================================
// ------------------------ NEW MEMBER FUNCTIONS -------------------------------
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngStorageAdminSubClient::NotifyGlobalEvent()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngStorageAdminSubClient::NotifyEvent(
    const TDesC8& aEvent )
    {
    PENG_DP( D_PENG_LIT( "RPEngStorageAdminSubClient::NotifyEvent()" ) );

    RETURN_IF_NOT_CONNECTED();

    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aEvent );
    return SendReceive( EAdminSubSessNotifyNewEvent, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageAdminSubClient::NotifyError()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngStorageAdminSubClient::NotifyError(
    TInt aError )
    {
    PENG_DP( D_PENG_LIT( "RPEngStorageAdminSubClient::NotifyError()" ) );

    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, aError );
    return Send( EAdminSubSessNotifyError, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageAdminSubClient::CheckAppIdRegistration()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngStorageAdminSubClient::CheckAppIdRegistration(
    const TDesC& aAppId )
    {
    RETURN_IF_NOT_CONNECTED();

    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aAppId );
    return SendReceive( EAdminSubSessCheckAppIdRegistration, messArgs );
    }

//  End of File




















