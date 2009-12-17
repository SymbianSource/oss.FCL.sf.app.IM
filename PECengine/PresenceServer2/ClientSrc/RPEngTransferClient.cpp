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
* Description:   Sub-session of the Presence Server
*  It is used to update date of the PEC engine to network
*
*/


//  INCLUDES
#include "RPEngTransferClient.h"

#include "RPEngManagerClient.h"

#include "TPEngServerMessages.h"

// Global constants of the PEC engine
#include "PEngInternalGlobalConsts.h"

#include "PresenceDebugPrint.h"


// MACROS
#define RETURN_ERR_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return KErrDisconnected;\
        }

#define RETURN_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return;\
        }

// ============================ EXPORTED FUNCTIONS =============================

// -----------------------------------------------------------------------------
// RPEngTransferClient::RPEngClientNotifyApi
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngTransferClient::RPEngTransferClient()
    {
    }

// Destructor
EXPORT_C RPEngTransferClient::~RPEngTransferClient()
    {
    }

// ============================ CONNECT FUNCTIONS ==============================
// -----------------------------------------------------------------------------
// RPEngTransferClient::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngTransferClient::Connect(
    const TDesC8& aSessSlotId ,
    const TDesC& aAppId,
    RPEngManagerClient& aManagerClient )
    {
    PENG_DP_TXT( "RPEngTransferClient::Connect()" );

    TIpcArgs messArguments;
    messArguments.Set( KMessageSlot0, &aSessSlotId );
    messArguments.Set( KMessageSlot1, &aAppId );

    TInt err( RSubSessionBase::CreateSubSession( aManagerClient,
                                                 EMainSessCreateTransactionSubSession,
                                                 messArguments ) );

    iConnected = ( err == KErrNone );
    return err;
    }


// -----------------------------------------------------------------------------
// RPEngTransferClient::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngTransferClient::Close()
    {
    PENG_DP_TXT( "RPEngTransferClient::Close()" );

    // all requests from this sub-session are canceled by framework
    CloseSubSession( EMainSessCloseSubSession );
    iConnected = EFalse;
    }


// ======================= TRANSFER EXPORTED FUNCTIONS ===========================

// -----------------------------------------------------------------------------
// RPEngTransferClient::UpdateData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngTransferClient::UpdateData(
    TInt aOperation,
    const TDesC& aData,
    TRequestStatus& aStatus )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, aOperation );
    messArgs.Set( KMessageSlot1, &aData );
    // now when we have still free slots, pass length of data
    // since some operation simply do not pass data
    messArgs.Set( KMessageSlot3, aData.Length() );
    SendReceive( ESubSessUpdateData, messArgs, aStatus );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RPEngTransferClient::UpdateData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngTransferClient::FetchLastRequestResult(
    TDes8& aRequestResult )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aRequestResult );
    // store there also max length, for information
    messArgs.Set( KMessageSlot1, aRequestResult.MaxLength() );
    return SendReceive( ESubSessFetchUpdateRqstResult, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngTransferClient::UpdateData
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngTransferClient::CancelUpdateData()
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArg;
    messArg.Set( KMessageSlot0, ESubSessUpdateData );
    SendReceive( ESubSessCancelRequest, messArg );
    }

//  End of File





















