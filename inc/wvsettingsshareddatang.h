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
* Description:  Defines shared data constants for service settings UI
*
*/



#ifndef WVSETTINGSSHAREDDATANG_H
#define WVSETTINGSSHAREDDATANG_H

//  INCLUDES
#include <E32STD.H>
#include "ChatNGCommonUIDs.h"
// CONSTANTS
const TUid KWVSettingsViewUid = { 0x10282E14 };
const TUid KUidChatNGClient = {KWVUIAVEEXEUIDS2};


// DATA TYPES

// Login values

enum TWVSettingsChatLoginValues
    {
    EWVSettingsChatMinLoginType = 0, // This has to be the first value here
    EWVSettingsChatLoginManual,
    EWVSettingsChatLoginApplicationLaunch,
    EWVSettingsChatLoginAutoAlways,
    EWVSettingsChatLoginAutoInHomeNW,
    EWVSettingsChatMaxLoginType // This has to be the last value here
    };

enum TWVSettingsSchedulingDays
    {
    EWVSettingsNone = 0,
    EWVSettingsMonday = 1,
    EWVSettingsTuesday = 2,
    EWVSettingsWednesday = 4,
    EWVSettingsThursday = 8,
    EWVSettingsFriday = 16,
    EWVSettingsSaturday = 32,
    EWVSettingsSunday = 64,
    EWVSettingsAll = 127
    };


#endif      // WVSETTINGSSHAREDDATANG_H

// End of File
