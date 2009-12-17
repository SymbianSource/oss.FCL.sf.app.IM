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
* Description: IM Private CR Keys
*
*/
#ifndef IMPRIVATECRKEYS_H
#define IMPRIVATECRKEYS_H

#include "IMNGInternalCRKeys.h"

// First login done or not done (Etrue/EFalse)
const TUint32 KIMFirstLogin        = 0x00000000;

// Default screen name in use or not in use (ETrue/EFalse)
const TUint32 KIMScreenNameInUse   = 0x00000001;

// Presence update automatic or not (ETrue/EFalse)
// default value 1
const TUint32 KIMAutoPresence      = 0x00000002;

// History shown or not shown (ETrue/EFalse)
const TUint32 KIMShowHistory       = 0x00000003;

// Authorization level of receiving instant messages (All, Friends, None)
const TUint32 KIMAuthIM            = 0x00000005;

// Authorization level of receiving invites (All, Friends, None)
const TUint32 KIMAuthInvite        = 0x00000006;

// Speed of message flow (range from 1 to 10)
// default value 5
const TUint32 KIMMsgFlow           = 0x00000007;

// Order of friends list (by presence, by alphabetic)
const TUint32 KIMOrderFriends      = 0x00000008;

// Default screen name
// default value ""
const TUint32 KIMDefScreenName     = 0x00000009;

// Own user id
// default value ""
const TUint32 KIMOwnUserId         = 0x0000000a;

// Own alias name
// default value ""
const TUint32 KIMAliasName         = 0x0000000b;

// Tone file name
// default value "z:\\Nokia\\Sounds\\Digital\\Chat alert.mid"
const TUint32 KIMAlertTone         = 0x0000000c;

// Show offline contacts (ETrue/EFalse)
const TUint32 KIMOfflineContacts   = 0x0000000d;

// Status messages for online status.
const TUint32 KIMStatusMsgOnline    = 0x0000000e;

// Status messages for away status.
const TUint32 KIMStatusMsgAway      = 0x0000000f;

// Status messages for busy status.
const TUint32 KIMStatusMsgBusy      = 0x00000010;

// Start of the Variation Flags in Cenrep
const TUint32 KIMStartVariationID 	= 0x00000011;

// Start of the Variation Flags in Cenrep
//const TUint32 KIMCUStartVariationID 	= 0x00000054;
const TUint32 KIMCUStartVariationID 	= 0x00000058;

//Local variation API
//Local variation key. Enumerations can be found from Inc\IMVariant.hrh
const TUint32 KIMVariationKey = 0x00000000;

#endif      // IMPRIVATECRKEYS_H
