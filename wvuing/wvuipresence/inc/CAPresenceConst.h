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
* Description:  Constans for IM Presence
*
*/



#ifndef CAPRESENCECONST_H
#define CAPRESENCECONST_H

#include <e32std.h>

const TInt KUndefined = -1;
const TInt KCAPresenceErrorBase = -20000;

_LIT( KIMPresencePanicCategory, "IMPresence" );

enum TIMPresenceErrorCodes
    {
    ECANoSettingsAPI = KCAPresenceErrorBase - 1,
    ECAArraysNotSameSize = KCAPresenceErrorBase - 2,
    ECAAttributeDecodeFailed = KCAPresenceErrorBase - 3
    };

#endif      // CAPRESENCECONST_H

// End of File
