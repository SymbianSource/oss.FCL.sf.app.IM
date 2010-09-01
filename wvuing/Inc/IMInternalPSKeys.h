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
* Description: IM internal PS keys
*
*/
#ifndef IMINTERNALPSKEYS_H
#define IMINTERNALPSKEYS_H

#include <e32property.h>
#include "ChatNGCommonUIDs.h"
// IM Internal SP API

/// PS UId for IM
const TUid KPSUidIMEngine = {0x101F8865};

/// Service address. String
const TUint32 KIMServiceAddress  = 0x00000001;

/// PS UID for IM application
const TUid KPSUidIMUI = { KWVUIAVEEXEUIDS2 };

/// Unread messages. Integer
const TUint32 KIMUnreadMsgKey    = 0x00000001;
_LIT_SECURITY_POLICY_PASS( KIMUnreadMsgReadPolicy );
_LIT_SECURITY_POLICY_PASS( KIMUnreadMsgWritePolicy );

#endif      // IMINTERNALPSKEYS_H
