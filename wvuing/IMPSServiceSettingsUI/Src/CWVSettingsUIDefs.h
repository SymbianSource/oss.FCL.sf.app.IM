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
* Description:  Service Settings UI definitions
*
*/



#ifndef CWVSETTINGSDEFS_H
#define CWVSETTINGSDEFS_H

#include "CWVSettingsUI.hrh"
#include "WVSettingsSharedDataNG.h"
#include <ApEngineConsts.h>

_LIT( KPanicText, "WVSETTINGSVIEW" );
// KEmptyDesC is needed in listboxes as empty value
// KNullDesC cannot be used instead, because it messes
// up the layout
_LIT( KEmptyDesC, " " );
_LIT( KCompulsoryChar, "*" );
_LIT( KResourceName, "CWVSettingsUING.RSC" );
// The Variation resource file which contains the CommonUi's Feature/Variation flags
_LIT( KIMCUVariationResourceName, "IMConnectionUIVariationNG.RSC" );

_LIT( KBitmapFile, "apsettings.mbm" );

_LIT( KServerNameFormatTrailer, "\t" );
_LIT( KFirstIconFormat, "\t0" );
_LIT( KAt, "@" );

_LIT( KDot, "." );
_LIT( KTwoDots, ".." );
_LIT( KWV, "wv:" );
_LIT( KSlash, "/" );
_LIT( KPlus, "+" );
_LIT( KTabulator, "\t" );

_LIT( KAtEnc, "%40" );
_LIT( KSlashEnc, "%2F" );
_LIT( KPlusEnc, "%2B" );
_LIT( KTabulatorEnc, "%09" );
_LIT( KEmptyDesCEnc, "%20" );

_LIT( KWVSettingsDefaultConnStart, "12:00" );
_LIT( KWVSettingsDefaultConnEnd, "12:00" );
_LIT( KWVSettingsDefaultMemoryDrive, "C:\\" );

_LIT( KTimeFormat, "%02d:%02d" );

_LIT( KIMPSIconIndex, "\t\t0" );

_LIT( KModuleId, "IMApp" );
_LIT( KDirSeparator, "\\" );
_LIT( KRelativeBrandPath, "\\system\\data\\branding\\" );
_LIT( KBitmapFileServerList, "logo.mbm" );


const TInt KDefaultStartHour = 12;
const TInt KDefaultEndHour = 12;
const TInt KDefaultMinute = 0;
const TInt KTimeValueLength = 2;

const TInt KWVSettingsBitmapFileNameLength = 30;

const TInt KWVSettingsServerNameGeneratedPartMaxLength = 4;

const TInt KWVSettingsServerNameMaxLength = 50;
const TInt KWVSettingsServerSAPMaxLength = 50;
const TInt KWVSettingsServerUserNameMaxLength = 50;
const TInt KWVSettingsServerWVUserIdMaxLength = 50;
const TInt KWVSettingsServerPasswordMaxLength = 50;
const TInt KWVSettingsServerHttpProxyMaxLength = 50;
const TInt KWVSettingsServerURLMaxlength = 255;

const TInt KWVSettingsConnDaysTextMaxLength = 50;
const TInt KWVSettingsConnHoursTextMaxLength = 50;

// when all day bits are on then the integer value is 127
const TInt KWVSettingsConnDaysAllSelected = 127;

const TInt KWVSettingsDefaultIMLoginType = EWVSettingsChatLoginManual;

// Server memory size estimate for low disk handling
const TInt KWVSettingsServerSizeEstimate =
    KWVSettingsServerNameMaxLength +
    KWVSettingsServerSAPMaxLength +
    KWVSettingsServerUserNameMaxLength +
    KWVSettingsServerWVUserIdMaxLength +
    KWVSettingsServerPasswordMaxLength +
    KWVSettingsServerHttpProxyMaxLength +
    KWVSettingsServerURLMaxlength +
    KWVSettingsConnDaysTextMaxLength +
    KWVSettingsConnHoursTextMaxLength +
    50; // Add some buffer

const TInt KWVSettingsLeaveBase = -19000;

enum TWVSettingsViewLeaveCodes
    {
    EFriendsArrayMissingContactInterface = KWVSettingsLeaveBase - 1,
    EServersArrayMissingServerList = KWVSettingsLeaveBase - 2,
    EServersArrayReplacelingModelWithNULL = KWVSettingsLeaveBase - 3,
    EServerDialogUnknownItemType = KWVSettingsLeaveBase - 4,
    EIncorrectListBoxItemIndex = KWVSettingsLeaveBase - 5
    };

enum TWVSettingsDialogLoginSettings
    {
    EWVSettingsViewLoginPec,
    EWVSettingsViewLoginIm
    };

_LIT( KWVSettingsViewServerNameTrailerDecorationStart, "(" );
_LIT( KWVSettingsViewServerNameTrailerDecorationEnd, ")" );
const TUint KWVSettingsViewServerNameTrailerMinNumber = 1;   //start number --> (01)
const TUint KWVSettingsViewServerNameTrailerMaxNumber = 99;  //end number --> (99)
const TInt  KWVSettingsViewServerNameTrailerNumberWidth = 2; //how many numbers places is reserved
//for trailer number
//NOTE!! This must be big enough to hold
//KNameTrailerMaxNumber

// Backslash unicode
const TUint KBackSlash = 0x005C;


//WLAN APN can not be used from IM Application
//
//const TInt KWVSettingsViewBearerTypes = EApBearerTypeAllBearers;
//End
const TInt KWVSettingsViewBearerTypes = EApBearerTypeCSD | EApBearerTypeGPRS | EApBearerTypeHSCSD | EApBearerTypeCDMA | EApBearerTypeLAN | EApBearerTypeLANModem;

#endif      // CWVSETTINGSDEFS_H

// End of File
