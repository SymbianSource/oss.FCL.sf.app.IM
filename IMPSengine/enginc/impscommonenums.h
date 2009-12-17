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
* Description: 
*     This file contains all the common enums between different files
*
*/



#ifndef __IMPSCOMMONENUMS_H
#define __IMPSCOMMONENUMS_H


// INCLUDE

// DATA TYPES

// CSP session type
enum TImpsSessionType
    {
    EImpsUndefSes = 0,  // UNDEF
    EImpsInband,        // INBAND
    EImpsOutband        // OUTBAND
    };

enum TImpsTrasactionMode
    {
    EImpsRequest = 0,    // REQUEST
    EImpsResponse        // RESPONSE
    };



// CSP Client-ID type
enum TImpsClientIDType
    {
    EImpsCliUndef = 0,  // undef
    EImpsCliURL,        // URL
    EImpsCliMSISDN      // MSISDN
    };

// Digest schema used in hash.
enum TImpsDigestSchema
    {
    EImpsPWD = 0,
    EImpsSHA,
    EImpsMD4,
    EImpsMD5,
    EImpsMD6
    };

// One of the following MOBILE_PHONE | COMPUTER| PDA | CLI | OTHER
enum TImpsClientType
    {
    EImpsMOBILE_PHONE = 0,
    EImpsCOMPUTER,
    EImpsPDA,
    EImpsCLI,
    EImpsOTHER
    };

// The delivery method setting. Notify/Get or Push.
enum TImpsInitialDeliveryMethod
    {
    EImpsN = 0,
    EImpsP
    };

// Bearer that the client supports.
enum TImpsSupportedBearer
    {
    EImpsSMS = 0,
    EImpsWSP,
    EImpsHTTP,
    EImpsHTTPS
    };
// Communiction Intitiation Request method that the client supports.
enum TImpsSupportedCIRMethod
    {
    EImpsWAPSMS = 0,
    EImpsWAPUDP,
    EImpsSUDP,
    EImpsSTCP
    };

// Indicates the type of the invitation.
enum TImpsInviteType
    {
    EImpsIM = 0,
    EImpsGR,
    EImpsPR,
    EImpsSC
    };
// The type of the subscription request. It is Get, Set, or Unset.
enum TImpsSubscribeType
    {
    EImpsG = 0,
    EImpsS,
    EImpsU
    };

// Indicates the transfer encoding used on the content.
enum TImpsContentEncoding
    {
    EImpsNone = 0,
    EImpsBASE64
    };

// Indicates the csp version used in messages.
enum TImpsCspVersion
    {
    EImpsCspVersion11 = 0,
    EImpsCspVersion12
    };


// CSP 1.2 additions
enum TImpsWatcherStatus
    {
    EImpsCURRENT_SUBSCRIBER = 0,
    EImpsFORMER_SUBSCRIBER,
    EImpsPRESENCE_ACCESS
    };

enum TImpsReactiveAuthState
    {
    EImpsGRANTED = 0,
    EImpsDENIED,
    EImpsPENDING
    };


#endif      // __IMPSCOMMONENUMS_H

// End of File
