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
*     Definitions of wbxml message tokens 
*     for the WirelessVillage OTA settings parser.
*
*/


#ifndef WvSettingsDef_H
#define WvSettingsDef_H

#include <e32def.h>
#include <e32std.h>

// *********      GENERAL WBXML TOKENS       *********
// WBXML global tokens - applicable to all messages
// See WAP Binary XML Content Format, 16-Jun-99, p16.  
// These and our message specific tokens define our DTD

#define KWV_STR_TERM				0x00	// Basically the NULL terminator character for a string
#define KWV_END_TAG					0x01    // Code for the end of an element
#define KWV_STR_I					0x03    // indicates the start of an inline string,
											// terminated by '\0', OR STR_TERM
#define KWV_TAG_CONTENT				0x40	// Bit 6 of the tag is set when a element has content
#define KWV_TAG_ATTRIBUTES			0x80	// Bit 7 of tag is set when the element has attributes
#define KWV_TAG_ATTRIBUTES_CONTENT	0xc0	// Bits 6 & 7 are set - attributes and content

// **********    PARSER SPECIFIC CODE LEVEL TAG TOKENS    **********
// WBXML tokens defined in Over The Air Settings Specification
// version 7.x
#define K_WVCOMMON					0x05 
#define K_WVHTTP					0x06 
#define K_WVOTAVERSION				0x07 
#define K_WVPASSWORD				0x08 
#define K_WVPREFERREDPROTOCOL		0x09 
#define K_WVPROXY					0x0a 
#define K_WVPROXYADDRESS			0x0b 
#define K_WVPROXYPASSWORD			0x0c 
#define K_WVPROXYPORT				0x0d 
#define K_WVPROXYUSER				0x0e 
#define K_WVREFNAME					0x0f 
#define K_WVSAPFOURWAYSUPPORTED		0x10 
#define K_WVSAPMOBILENUMBER			0x11 
#define K_WVSAPURI					0x12 
#define K_WVSETTINGS				0x13 
#define K_WVSETTINGSNAME			0x14 
#define K_WVSMS						0x15 
#define K_WVSMSMOBILENUMBER			0x16 
#define K_WVSUPPORTEDVERSION		0x17 
#define K_WVUSERID					0x18 


// Labels for use in xml element creation
_LIT(KWVCommon, "WVCommon");
_LIT(KWVHTTP, "WVHTTP");
_LIT(KWVOTAVersion, "WVOTAVersion");
_LIT(KWVPassword, "WVPassword");
_LIT(KWVPreferredProtocol, "WVPreferredProtocol");
_LIT(KWVProxy, "WVProxy");
_LIT(KWVProxyAddress, "WVProxyAddress");
_LIT(KWVProxyPassword, "WVProxyPassword");
_LIT(KWVProxyPort, "WVProxyPort");
_LIT(KWVProxyUser, "WVProxyUser");
_LIT(KWVRefName, "WVRefName");
_LIT(KWVSAPFourWaySupported, "WVSAPFourWaySupported");
_LIT(KWVSAPMobileNumber, "WVSAPMobileNumber");
_LIT(KWVSAPURI, "WVSAPURI");
_LIT(KWVSettings, "WVSettings");
_LIT(KWVSettingsName, "WVSettingsName");
_LIT(KWVSMS, "WVSMS");
_LIT(KWVSMSCMobileNumber, "WVSMSCMobileNumber");
_LIT(KWVSupportedVersion, "WVSupportedVersion");
_LIT(KWVUserID, "WVUserID");


#endif
