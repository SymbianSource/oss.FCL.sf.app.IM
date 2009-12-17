/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines central repository constants for service settings UI
*
*/



#ifndef IMPSSERVICESETTINGSUINGINTERNALCRKEYS_H
#define IMPSSERVICESETTINGSUINGINTERNALCRKEYS_H

//  INCLUDES
#include <E32STD.H>

// CONSTANTS
const TUid KWVSettingsCenRepUid = { 0x101F879C };

/**
* Presence application login type
* Values for the key defined in WVSettingsSharedDataNG.h:
* enum TWVSettingsPECLoginValues
*	{
*	EWVSettingsPECMinLoginType = 0, // This has to be the first value here
*	EWVSettingsPECLoginManual,
*	EWVSettingsPECLoginApplicationLaunch,
*	EWVSettingsPECLoginAutoAlways,
*	EWVSettingsPECLoginAutoInHomeNW,
*	EWVSettingsPECMaxLoginType // This has to be the last value here
*	};
*/
const TUint32 KIMPSCRPECLogin = 0x00000069;


/**
* Chat application login type
* Values for the key defined in WVSettingsSharedDataNG.h:
* enum TWVSettingsChatLoginValues
*	{
*	EWVSettingsChatMinLoginType = 0, // This has to be the first value here
*	EWVSettingsChatLoginManual,
*	EWVSettingsChatLoginApplicationLaunch,
*	EWVSettingsChatLoginAutoAlways,
*	EWVSettingsChatLoginAutoInHomeNW,
*	EWVSettingsChatMaxLoginType // This has to be the last value here
*	};
*/
const TUint32 KIMPSCRChatLogin = 0x00000065;

/**
* Presence application connection days scheduling setting
* Values for the key defined in WVSettingsSharedDataNG.h:
* enum TWVSettingsSchedulingDays
*	{
*	EWVSettingsNone = 0,
*	EWVSettingsMonday = 1,
*	EWVSettingsTuesday = 2,
*	EWVSettingsWednesday = 4,
*	EWVSettingsThursday = 8,
*	EWVSettingsFriday = 16,
*	EWVSettingsSaturday = 32,
*	EWVSettingsSunday = 64,
*	EWVSettingsAll = 127
*	};
*/
const TUint32 KIMPSCRPECConnDays = 0x0000006C;

/**
 * MultiDrive requirement String value.
 */
const TUint32 KIMPSCRIMDefaultMemoryDrive = 0x0000006D;

/**
 * Presence application connection start scheduling setting. String value.
 */
const TUint32 KIMPSCRPECConnStart = 0x0000006A;

/**
* Presence application connection end scheduling setting. String value.
*/
const TUint32 KIMPSCRPECConnEnd = 0x0000006B;


/**
* Chat application connection days scheduling setting
* Values for the key defined in WVSettingsSharedDataNG.h:
* enum TWVSettingsSchedulingDays
*	{
*	EWVSettingsNone = 0,
*	EWVSettingsMonday = 1,
*	EWVSettingsTuesday = 2,
*	EWVSettingsWednesday = 4,
*	EWVSettingsThursday = 8,
*	EWVSettingsFriday = 16,
*	EWVSettingsSaturday = 32,
*	EWVSettingsSunday = 64,
*	EWVSettingsAll = 127
*	};
*/
const TUint32 KIMPSCRIMConnDays = 0x00000068;

/**
 * Chat application connection start scheduling setting. String value.
 */
const TUint32 KIMPSCRIMConnStart = 0x00000066;

/**
 * Chat application connection end scheduling setting. String value.
 */
const TUint32 KIMPSCRIMConnEnd = 0x00000067;

#endif      // IMPSSERVICESETTINGSUINGINTERNALCRKEYS_H

// End of File
