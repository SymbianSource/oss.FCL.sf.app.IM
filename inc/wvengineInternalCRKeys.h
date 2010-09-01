/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: define some const value
*
*/

#ifndef WVENGINEINTERNALCRKEYS_H
#define WVENGINEINTERNALCRKEYS_H

const TUid KCRUIDWVEngineVariation = {0x101FB0DA}; 

// Defines the OMA IMPS standard number that is supported
const TUint32 KWVEngineCspVersion   = 0x00000001;

// Defines the timeout in minutes how long the pdp-context is kept open after 
// last network operation. The CSP session is kept active and the pdp-context 
// is reopened for keep-alive messages. Value 0 means the pdp-context is never 
// actively dropped within CSP-session. 
const TUint32 KWVEngineDropContextWhileIdle = 0x00000003;

// Defines whether 4-way login or 2-way login is used
const TUint32 KWVEngineLoginMethod = 0x00000005;

// Initial poll time in seconds
const TUint32 KWVEnginePollTime = 0x00000010;

// Maximum IM message size
const TUint32 KWVEngineMaxMessageSize = 0x00000011;

// How many seconds is the CSP-session kept alive during idle time. 
// This is the value the client proposes in the login phase, the WV server 
// defines what is the value that is actually used.
const TUint32 KWVEngineDefaultKeepAliveTime = 0x00000012;

// Access service request expiry time in seconds in WV engine API.
const TUint32 KWVEngineACExpiryTime = 0x00000021;

// IM service request expiry time in seconds in WV engine API.
const TUint32 KWVEngineIMExpiryTime = 0x00000022;

// Group service request expiry time in seconds in WV engine API.
const TUint32 KWVEngineGRExpiryTime = 0x00000023;

// Presence service request expiry time in seconds in WV engine API.
const TUint32 KWVEnginePRExpiryTime = 0x00000024;

// Fundamental service request expiry time in seconds in WV engine API.
const TUint32 KWVEngineFUExpiryTime = 0x00000025;

// Regular polling time frequence in seconds in CIR mode. WV Engine goes to 
// CIR mode after receiving first CIR message and sends Poll messages when 
// requested but this enables timer based polling in CIR mode too. 
// Value 0 means timer based polling is totally stopped in CIR mode.
const TUint32 KWVEngineCIRModePollTime = 0x00000031;

// Number of accepted transactions in incoming message
const TUint32 KWVEngineMultiTrans = 0x00000032;

#endif      // WVENGINEINTERNALCRKEYS_H

