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
* Description:  This class is used to store/retrieve plugin implementation
*		 specific shareddata keys and enumerations.
*
*/


// INCLUDE FILES
#include "timpsaokeys.h"
#include "IMPSUIDDefs.h"

#include <WVSettingsSharedDataNG.h>
#include <impspresenceconnectionuiconstsng.h>

//CONSTANTS

// -----------------------------------------------------------------------------
// TIMPSAOKeys::TIMPSAOKeys
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TIMPSAOKeys::TIMPSAOKeys()
    {
    iMinLoginTypeEnum = EWVSettingsChatMinLoginType;
    iLoginManualEnum = EWVSettingsChatLoginManual;
    iLoginApplicationLaunchEnum = EWVSettingsChatLoginApplicationLaunch;
    iLoginAutoAlwaysEnum = EWVSettingsChatLoginAutoAlways;
    iLoginAutoInHomeNWEnum = EWVSettingsChatLoginAutoInHomeNW;
    iMaxLoginTypeEnum = EWVSettingsChatMaxLoginType;
    iLoginTypeKey = EIMPSSharedKeysIMLogin;
    iConnDaysKey = EIMPSSharedKeysIMConnDays;
    iConnStartKey = EIMPSSharedKeysIMConnStart;
    iConnEndKey = EIMPSSharedKeysIMConnEnd;
    iScheduleChangedKey = EIMPSSharedKeysServSettScheduleChangedIM;
    iLoginTypeChangedKey = EIMPSSharedKeysServSettLoginTypeChangedIM;
    }


// ---------------------------------------------------------
// TIMPSAOKeys::MinLoginType
//
// ---------------------------------------------------------
//
TInt TIMPSAOKeys::MinLoginTypeEnum()
    {
    return iMinLoginTypeEnum;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::LoginManualEnum
//
// ---------------------------------------------------------
//
TInt TIMPSAOKeys::LoginManualEnum()
    {
    return iLoginManualEnum;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::LoginApplicationLaunchEnum
//
// ---------------------------------------------------------
//
TInt TIMPSAOKeys::LoginApplicationLaunchEnum()
    {
    return iLoginApplicationLaunchEnum;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::LoginAutoAlwaysEnum
//
// ---------------------------------------------------------
//
TInt TIMPSAOKeys::LoginAutoAlwaysEnum()
    {
    return iLoginAutoAlwaysEnum;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::LoginAutoInHomeNWEnum
//
// ---------------------------------------------------------
//
TInt TIMPSAOKeys::LoginAutoInHomeNWEnum()
    {
    return iLoginAutoInHomeNWEnum;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::MaxLoginType
//
// ---------------------------------------------------------
//
TInt TIMPSAOKeys::MaxLoginTypeEnum()
    {
    return iMaxLoginTypeEnum;
    }


// ---------------------------------------------------------
// TIMPSAOKeys::LoginTypeKey
//
// ---------------------------------------------------------
//
const TIMPSSharedKeys& TIMPSAOKeys::LoginTypeKey()
    {
    return iLoginTypeKey;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::ConnDaysKey
//
// ---------------------------------------------------------
//
const TIMPSSharedKeys& TIMPSAOKeys::ConnDaysKey()
    {
    return iConnDaysKey;
    }


// ---------------------------------------------------------
// TIMPSAOKeys::ConnStartKey
//
// ---------------------------------------------------------
//
const TIMPSSharedKeys& TIMPSAOKeys::ConnStartKey()
    {
    return iConnStartKey;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::ConnEndKey
//
// ---------------------------------------------------------
//
const TIMPSSharedKeys& TIMPSAOKeys::ConnEndKey()
    {
    return iConnEndKey;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::LoginTypeChangedKey
//
// ---------------------------------------------------------
//
const TIMPSSharedKeys& TIMPSAOKeys::LoginTypeChangedKey()
    {
    return iLoginTypeChangedKey;
    }

// ---------------------------------------------------------
// TIMPSAOKeys::ScheduleChangedKey
//
// ---------------------------------------------------------
//
const TIMPSSharedKeys& TIMPSAOKeys::ScheduleChangedKey()
    {
    return iScheduleChangedKey;
    }
