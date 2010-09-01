/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Defines the base line for WV engine errors
*              KImpsGeneralError  Imps_ERROR_BASE
*
*               The error codes retuned by WV engine are either: 
*               a) General Symbian OS error code defined in e32std.h
*               b) WV engine internal error code defined in impserror.h, range is
*                  KImpsGeneralError ... (KImpsGeneralError - 99).
*               c) CSP error code translated in form  
*                  KImpsGeneralError - <CSP-error-code>
*
*/


#ifndef IMPSERRORS_H
#define IMPSERRORS_H

// the error range is -34000 -> -34999
#define Imps_ERROR_BASE -34000


// error codes must be defined as enum so that they can be
// included by a source file that uses the error codes as ints

// All errors are define relative to Imps_ERROR_BASE
enum
    {
    // Unspecified error, used when no defined error matches.
    KImpsGeneralError = Imps_ERROR_BASE,
    // Terminal is already logged in SAP
    KImpsErrorAlreadyLogged = Imps_ERROR_BASE - 1,
    // Terminal has not yet logged in SAP
    KImpsErrorNotLogged = Imps_ERROR_BASE - 2,
    // Client not registered
    KImpsErrorNotRegistered = Imps_ERROR_BASE - 3,
    // Unauthorized request in SAP
    KImpsErrorUnauthorized = Imps_ERROR_BASE - 4,
    // General configuration error
    KImpsErrorConfig = Imps_ERROR_BASE - 5,
    // CSP Client Capabilities negotiation fails
    KImpsErrorCapabilities = Imps_ERROR_BASE - 6,
    // CSP Service negotiation fails
    KImpsErrorServices = Imps_ERROR_BASE - 7,
    // IAP connection closed or not found in CommDb
    KImpsErrorNoIAP = Imps_ERROR_BASE - 9,

    // message is too big to be sent - cannot allocate buffer
    KImpsErrorMessageTooBig = Imps_ERROR_BASE - 10,
    // message type invalid - cannot encode
    KImpsErrorUnknownMessageType = Imps_ERROR_BASE - 11,

    // This is not an actual error but a fake for online status changes
    KImpsNoIapStatus = Imps_ERROR_BASE - 12,
    KImpsOnlineStatus = Imps_ERROR_BASE - 13,
    KImpsOfflineStatus = Imps_ERROR_BASE - 14,
    KImpsNotLoggedStatus = Imps_ERROR_BASE - 15,

    // Logout not successful
    KImpsErrorOthersLogged = Imps_ERROR_BASE - 16,

    // Terminal off-line mode (flight mode)
    KImpsErrorTerminalOffLine = Imps_ERROR_BASE - 17,
    
    // Mapping of HTTP error codes. Rough categories only
    KImpsErrorHTTPConfiguration = Imps_ERROR_BASE - 18, // 1XX, 3XX
    KImpsErrorHTTPNotFound = Imps_ERROR_BASE - 19,      // 4XX
    KImpsErrorHTTPServerDown = Imps_ERROR_BASE - 20,    // 5XX

    // Errors from the transport adapter
    KImpsErrorSessionNotOpen = Imps_ERROR_BASE - 23,
    KImpsErrorBearerSuspended = Imps_ERROR_BASE - 24,
    KImpsErrorAlreadyClosed = Imps_ERROR_BASE - 25,
    KImpsErrorOffLine = Imps_ERROR_BASE - 26,
    KImpsErrorAllConnSlotsInUse = Imps_ERROR_BASE - 27,

    // Errors generated by illegal XML response
    KImpsErrorTID = Imps_ERROR_BASE - 40,
    KImpsErrorSID = Imps_ERROR_BASE - 41,
    KImpsErrorCID = Imps_ERROR_BASE - 42,
    KImpsErrorEncode = Imps_ERROR_BASE - 43,
    KImpsErrorDecode = Imps_ERROR_BASE - 44,
    KImpsErrorResponseStatus = Imps_ERROR_BASE - 45,
    KImpsErrorCSPVersion = Imps_ERROR_BASE - 46,
    KImpsErrorValidate = Imps_ERROR_BASE - 47,

    // Errors generated by illegal keys
    KImpsErrorKeyEmpty = Imps_ERROR_BASE - 50,
    KImpsErrorKeyIndexTooLarge = Imps_ERROR_BASE - 51,
    KImpsErrorKeyIndexInvalid = Imps_ERROR_BASE - 52,

    // New
    KImpsErrorShuttingDown = Imps_ERROR_BASE - 55,
    KImpsErrorAlreadyInUse = Imps_ERROR_BASE - 56


    };

#endif      // ImpsERRORS_H   
            
