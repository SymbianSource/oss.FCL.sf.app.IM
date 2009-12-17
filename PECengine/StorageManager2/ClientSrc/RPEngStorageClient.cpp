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
* Description:  Storage main client
*
*/


// INCLUDE FILES
#include "RPEngStorageClient.h"
#include <eikdll.h>
#include <f32file.h>

#include "PEngServerStarter.h"
#include "PEngStorageServerCommon.h"
#include "TPEngStorageServerMessages.h"

// PEC Engine internal constants
#include "PEngPresenceEngineConsts2.h"
#include "PEngInternalGlobalConsts.h"

#include "PresenceDebugPrint.h"


// CONSTANTS
// each session can have several sub-session, while only one sub-session
// is using asynchronous requests, 20 slots shall be enough to cover
const TUint KMessageSlotCount = 20;


// MACROS
#define RETURN_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return KErrDisconnected;\
        }

#define RETURN_IF_NULL( desBuff )\
    if ( !desBuff )\
        {\
        return KErrNoMemory;\
        }

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// RPEngStorageClient::RPEngStorageClient
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RPEngStorageClient::RPEngStorageClient()
    {
    }

// Destructor
RPEngStorageClient::~RPEngStorageClient()
    {
    }

// =============================================================================
// =============Function of the client for Connection managing =================
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngStorageClient::Connect
// -----------------------------------------------------------------------------
//
TInt RPEngStorageClient::Connect()
    {
    PENG_DP_TXT( "RPEngStorageClient::Connect()" );

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
// RPEngStorageClient::Close
// -----------------------------------------------------------------------------
//
void RPEngStorageClient::Close()
    {
    PENG_DP_TXT( "RPEngStorageClient::Close()" );

    // all requests from this session are canceled on the server side
    RSessionBase::Close();
    iConnected = EFalse;
    }

// =============================================================================
// ============= New Function of the client  ===================================
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngStorageClient::SessionSlotState()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageClient::SessionSlotState(
    const TDesC8& aSessionName,
    HBufC8*& aSessionState,
    TInt aInitSize )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aSessionName );
    aSessionState = HBufC8::New( aInitSize );
    RETURN_IF_NULL( aSessionState );
    TPtr8 sessState ( aSessionState->Des() );
    messArgs.Set( KMessageSlot1, &sessState );
    TInt err( SendReceive( EMainSessGetSessionState, messArgs ) );
    // if error is positive, then buffer was not big enough
    if ( err > KErrNone )
        {
        // realloc buffer, looks complicated, but safe, avoiding realloc
        delete aSessionState;
        aSessionState = NULL;
        aSessionState = HBufC8::New( err );
        RETURN_IF_NULL( aSessionState );
        sessState.Set( aSessionState->Des() );
        messArgs.Set( KMessageSlot1, &sessState );
        err = SendReceive( EMainSessGetSessionState, messArgs );
        }
    // if there was error erase buffer
    if ( err < KErrNone )
        {
        delete aSessionState;
        aSessionState = NULL;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// RPEngStorageClient::AllSessionSlotsStates()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageClient::AllSessionSlotsStates(
    HBufC8*& aSessionSlotsBuffer,
    TInt aInitSize )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    aSessionSlotsBuffer = HBufC8::New( aInitSize );
    RETURN_IF_NULL( aSessionSlotsBuffer );
    TPtr8 sessStates ( aSessionSlotsBuffer->Des() );
    messArgs.Set( KMessageSlot0, &sessStates );
    TInt err( SendReceive( EMainSessGetAllSessionStates, messArgs ) );
    // if error is positive, then buffer was not big enough
    if ( err > KErrNone )
        {
        // realloc buffer, looks complicated, but safe, avoiding realloc
        delete aSessionSlotsBuffer;
        aSessionSlotsBuffer = NULL;
        aSessionSlotsBuffer = HBufC8::New( err );
        RETURN_IF_NULL( aSessionSlotsBuffer );
        sessStates.Set( aSessionSlotsBuffer->Des() );
        messArgs.Set( KMessageSlot1, &sessStates );
        err = SendReceive( EMainSessGetAllSessionStates, messArgs );
        }
    // if there was error erase buffer
    if ( err < KErrNone )
        {
        delete aSessionSlotsBuffer;
        aSessionSlotsBuffer = NULL;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// RPEngStorageClient::ListenGlobalEvents()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageClient::ListenGlobalEvents()
    {
    RETURN_IF_NOT_CONNECTED();
    return SendReceive( EMainSessListenGlobalEvents );
    }

// -----------------------------------------------------------------------------
// RPEngStorageClient::ReloadGlobalEventListener()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageClient::ReloadGlobalEventListener(
    TDes8& aGlobalEventBuffer,
    TRequestStatus& aStatus )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, EMainSessListenGlobalEvents );
    messArgs.Set( KMessageSlot1, &aGlobalEventBuffer );
    SendReceive( EMainSessReloadAsynchronousScout, messArgs, aStatus );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RPEngStorageClient::StopEventListening()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageClient::StopEventListening()
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, EMainSessListenGlobalEvents );
    return SendReceive( EMainSessCancelRequest, messArgs );
    }

// -----------------------------------------------------------------------------
// RPEngStorageClient::CreateSessionFolder()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageClient::CreateSessionFolder(
    const TDesC8& aSessionSlot,
    const TDesC16& aApplicationId )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aSessionSlot );
    messArgs.Set( KMessageSlot1, &aApplicationId );

    return SendReceive( EMainSessCreateSessionFolder, messArgs );
    }

// -----------------------------------------------------------------------------
// RPEngStorageClient::RemoveSessionFolder()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageClient::RemoveSessionFolder(
    const TDesC8& aSessionSlot,
    const TDesC16& aApplicationId )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aSessionSlot );
    messArgs.Set( KMessageSlot1, &aApplicationId );
    return SendReceive( EMainSessRemoveSessionFolder, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageClient::Version()
// -----------------------------------------------------------------------------
//
TVersion RPEngStorageClient::Version() const
    {
    return( TVersion( KClientVersionMajor,
                      KClientVersionMinor,
                      KClientVersionBuild ) );
    }

//  End of File

