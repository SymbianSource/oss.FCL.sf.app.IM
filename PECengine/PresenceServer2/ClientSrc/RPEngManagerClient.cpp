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
* Description:  Presence server main client
*
*/


//  INCLUDES
#include <f32file.h>

#include "RPEngManagerClient.h"

#include "PEngServerStarter.h"
#include "TPEngServerCommon.h"


// PEC Engine internal constants
#include "PEngInternalGlobalConsts.h"

// debug prints
#include "PresenceDebugPrint.h"

// CONSTANTS

/*
 * Number of message slots we allocated to this client.
 * There can be several Transaction and Session holder clients
 * per several sessions -> 50
 */
const TUint KMessageSlotCount = 50;


// MACROS
#define RETURN_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return KErrDisconnected;\
        }


// ============== NEW EXPORTED FUNCTIONS OF THE CLIENT====================

// -----------------------------------------------------------------------------
// RPEngManagerClient::RPEngManagerClient
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngManagerClient::RPEngManagerClient()
    {
    }

// Destructor
EXPORT_C RPEngManagerClient::~RPEngManagerClient()
    {
    }

// ============================ EXPORTED FUNCTIONS ===============================

// RPEngManagerClient::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngManagerClient::Connect()
    {
    PENG_DP_TXT( "RPEngManagerClient::Connect()" );

    TInt err = PEngServerStarter::ConnectServer( *this,
                                                 KServerName,
                                                 Version(),
                                                 KMessageSlotCount,
                                                 KServerExe );

    iConnected = ( err == KErrNone );
    return err;
    }

// -----------------------------------------------------------------------------
// RPEngManagerClient::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngManagerClient::Close( void )
    {
    PENG_DP( D_PENG_LIT( "RPEngManagerClient::Close()" ) );

    // all requests from this session are canceled on the server side
    RSessionBase::Close();
    iConnected = EFalse;


#if _BullseyeCoverage
    cov_write();
#endif
    }


// ======================= OTHER NOT-EXPORTED FUNCTIONS ========================

// -----------------------------------------------------------------------------
// RPEngManagerClient::Version
// Return version of the client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TVersion RPEngManagerClient::Version() const
    {
    return( TVersion( KClientVersionMajor, KClientVersionMinor, KClientVersionBuild ) );
    }


//  End of File
