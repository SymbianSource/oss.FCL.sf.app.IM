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
* Description:  Used for testing and connection of the Admin sub clients
*
*/


// INCLUDE FILES
#include "RPEngStorageAdminClient.h"
#include <eikdll.h>
#include <f32file.h>

#include "PEngServerStarter.h"
#include "PEngStorageServerCommon.h"
#include "TPEngStorageServerMessages.h"
#include "PEngMessagePacker.h"

// PEC Engine internal constants
#include "PEngInternalGlobalConsts.h"

#include "PresenceDebugPrint.h"


// CONSTANTS
// each session can have several subsession, while only one subsession
// is using asynchronouse requests, 20 slots shall be enough to cover
const TUint KMessageSlotCount = 20;

// MACROS
#define RETURN_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return KErrDisconnected;\
        }


// ============================ EXPORTED FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPEngStorageAdminClient::RPEngStorageAdminClient
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngStorageAdminClient::RPEngStorageAdminClient()
    {
    }


// Destructor
EXPORT_C RPEngStorageAdminClient::~RPEngStorageAdminClient()
    {
    }


// -----------------------------------------------------------------------------
// RPEngStorageAdminClient::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngStorageAdminClient::Connect()
    {
    PENG_DP_TXT( "RPEngStorageAdminClient::Connect()" );

    //Connects this client to the server.
    //Launches new server process if one not yet running.
    TInt err = PEngServerStarter::ConnectServer( *this,
                                                 KStorageServerName,
                                                 Version(),
                                                 KMessageSlotCount,
                                                 KStorageServerExe );

    iConnected = ( err == KErrNone );
    return err;
    }


// -----------------------------------------------------------------------------
// RPEngStorageAdminClient::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngStorageAdminClient::Close()
    {
    PENG_DP_TXT( "RPEngStorageAdminClient::Close()" );

    // all requests from this session are canceled on the server side
    RSessionBase::Close();
    iConnected = EFalse;
    }


// =============================================================================
// ----------------------- Testing support -------------------------------------
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngStorageAdminClient::WipeSession
// Wipe session folder from the storage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngStorageAdminClient::WipeSession(
    const TDesC8& aSessionId )
    {
    PENG_DP_TXT( "RPEngStorageAdminClient::WipeSession()" );

    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aSessionId );
    return SendReceive( EMainSessWipeSessionFolder, messArgs );
    }


// =============================================================================
// ------------ NEW PRIVATE FUNCTIONS ------------------------------------------
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngStorageAdminClient::Version
// -----------------------------------------------------------------------------
//
TVersion RPEngStorageAdminClient::Version() const
    {
    return( TVersion( KClientVersionMajor,
                      KClientVersionMinor,
                      KClientVersionBuild ) );
    }

//  End of File




















