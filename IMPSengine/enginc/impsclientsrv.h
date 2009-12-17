/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Definitions for engine's internal use.
*
*/


#ifndef IMPSCLIENTSRV_H
#define IMPSCLIENTSRV_H

// CONSTANTS

// Bit masks for extra data in client request
const TUint KImpsReqHandleNew  = 0x0001;
const TUint KImpsReqAnyContent = 0x0002;

// Negative IPC responses from the server are error codes.
// Positive values using 30 bits are message sizes.
// One bit is reserved to indicate that positive value contains
// operation id for failed client request due to OOM error.

// Bit mask to create server OOM error response
const TUint KImpsOOMReply = 0x40000000;
// Bit mask for server OOM error response to get opid
const TUint KImpsOOMReplyOpId = 0x3FFFFFFF;

// DATA TYPES

// opcodes used in message passing between SOS client and server
enum TImpsServRequest
    {
    // Client->Server requests and responses
    EImpsServNone = 0,
    EImpsServWVLogin,
    EImpsServWVLogout,
    EImpsServWVLogoutOne,
    EImpsServAssign,
    EImpsAccessRegister,    // single CSP support only
    EImpsAccessRegister2,   // Multi-csp support
    EImpsImRegister,
    EImpsGroupRegister,
    EImpsPresRegister,
    EImpsFundRegister,
    EImpsServWVSendOnly,    // IM message, entire message always
    EImpsServBlock,         // IM blocking request
    EImpsServGetBlocked,    // IM get bloked users list
    EImpsServGroup,         // Group message
    EImpsServPres,          // Presence message
    EImpsServFundSearch,    // Fundamental message
    EImpsServFundInvite,    // Fundamental message
    EImpsServPush,          // CIR
    EImpsServNextEvent,     // client asks more events
    EImpsServIsLogged,      // Ask if user is logged in
    EImpsServNbrSessions,   // Ask number of sessions
    EImpsServNbrSubSess,    // Ask number of subsessions
    EImpsServServices,      // Ask service tree
    EImpsServCloseSub,      // Close sub session, rel 2.1
    EImpsServDeleteSub,     // Delete subsession
    EImpsServPureRegister,
    EImpsServPureSend,
    EImpsServEventBody,     // client asks message body of the last event
    // extras
    EImpsServBuffSizeReq,   // ask maximum XML transaction content size
    // close, last valid request from client
    EImpsServCloseSession,  // shut down session
    // Server->Client event, no corresponding client request exist
    EImpsServStatusReg,     // register status observer
    EImpsServStatusUnreg,   // unregister status observer
    EImpsServDetailedReg,   // register detailed error observer
    EImpsServDetailedUnreg, // unregister detailed error observer
    EImpsServReactiveLogin,	// reactive authorization in use
    EImpsServCancelLogin,	// cancel ongoing login
    EImpsServCloseAll,		// close all sessions
    EImpsServCancelTrans,	// cancel transaction
    EImpsServSetExpiry,  	// set expiry time
    EImpsServCspVersion		// supported csp version
    };

// Data structure for IPC
struct SImpsEventData
    {
    TInt            iRequestType;    // TImpsServRequest
    TInt            iOpCode;
    TInt            iStatus;
    TBool           iMessageBody;   // packed message
    TInt            iMessageType;   // TImpsMessageType, response
    TInt            iReqMesType;    // TImpsMessageType, request
    TInt            iAux;           // auxiliary for small data
    };
#endif      // ?INCLUDE_H   

// End of File
