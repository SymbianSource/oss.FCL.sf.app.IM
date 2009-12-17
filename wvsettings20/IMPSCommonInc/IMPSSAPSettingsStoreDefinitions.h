/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common definitions for IMPSSAPSettingsStore.
*
*/


#ifndef IMPSSAPSETTINGSSTOREDEFINITIONS_H
#define IMPSSAPSETTINGSSTOREDEFINITIONS_H

// INCLUDES
#include <e32base.h>

// DATATYPES

// Settings that have fixed identifier.
enum TFixedSAPSetting
    {
    ESAPAddress = 0,
    ESAPName,
    ESAPPort,
    ESAPUserId,
    ESAPUserPassword,
    EHTTPProxyAddress,
    EHTTPProxyPort,
    EHTTPProxyUserId,
    EHTTPProxyUserPassword,
    EProtection,
    EAccessPoint,
    EClientId,
    EHighLevelServices,
    EServerAuthenticationName,
    EServerAuthenticationPassword,
    EServerAcceptedContentType,
    EAuthorizationMode
    };

// Key-Value pairs
enum TKeyValuePairs
    {
    EKeyValuePairBase = 256
    };

// CONSTS

// The actual encryption key for passwords comes when
// we XOR the IMEI with key defined in headers.
// This is the defined key, and must be 24 bytes long!
// As secure as any other string that could be defined here.
_LIT8( KSAPPwdDefinedKey, "SSS_20&%qwerty!+mbnzxcvb" );

// Fake IMEI for the cases we're unable to get the
// real serial number from the device.
_LIT( KSAPFakeIMEI, "012345678912345" );

// Base Id ranges for settings
const TUint32 KSAPSettingsPECBaseId = 0x30; // PEC SAP settings base id
//bits: 11110 0000000000 0000000000000000
const TUint32 KSAPSettingsIMBaseId = 0x31; // IM SAP settings base id
//bits: 11111 0000000000 0000000000000000

// Offsets for bitwise operations
const TUint32 KBaseOffset = 26; //Base offset
const TUint32 KSAPOffset = 16; //SAP offset

// Masks for bitwise operations
//
//Mask for finding Key-value pairs
//bits: 111111 1111111111 1111111100000000
const TUint32 KSAPPairsMask = 0xFFFFFF00;

//Mask for finding SAP Uids
//bits: 000000 1111111111 0000000000000000
const TUint32 KSAPUidMask = 0x03FF0000;

//Mask for finding SAP type
//bits: 000000 0011111111 1111111111111111
const TUint32 KSAPTypeMask = 0x00FFFFFF;

//Mask for getting SAP type
//bits: 111111 0000000000 0000000000000000
const TUint32 KSAPBaseMask = 0xFC000000;

//Mask for populating either IM or PEC SAPs
//bits: 111111 0000000000 1111111111111111
const TUint32 KSAPPopulateGroupMask = 0xFC00FFFF;

//Mask for populating all SAPs
//bits: 000000 0000000000 1111111111111111
const TUint32 KSAPPopulateAllMask = 0x0000FFFF;

//Mask for checking SAP existence
const TUint32 KSAPExistsMask = 0xFFFFFFFF;

// Id of the default PEC SAP setting.
//bits: 110000 0000000000 0000000000001110
const TUint32 KDefaultPECSAPId = 0xC000000E;

// Id of the default IM SAP setting.
//bits: 110001 0000000000 0000000000001111
const TUint32 KDefaultIMSAPId = 0xC400000F;

// Id for the highest SAP Uid
//bits: 110010 0000000000 0000000000001111
const TUint32 KSAPHighestUid = 0xC800000F;

// Id of the first storable SAP.
const TUint32 KFirstSAPId = 0x00000001;
// Id of the last storable SAP. (10 bits)
const TUint32 KLastSAPId = 0x000003FF;


//Id of the encryption key in key repository
const TUint32 KEncryptionKeyId = 0x00000001;

// Maximum length of the encryption key
const TInt KSAPPwdKeyMaxLength = 50;

// field separator for flat key-valuepair structure
_LIT( KKeyValuePairFieldSeparator, "||" );

// Maximum length of TInt textual representation
const TInt KTIntMaxLengthTextual = 10;

#endif	// IMPSSAPSETTINGSSTOREDEFINITIONS_H

