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
* Description: IM Internal CR Keys
*
*/
#ifndef IMNGINTERNALCRKEYS_H
#define IMNGINTERNALCRKEYS_H

// #include <IMSDKCRKeys.h>

// IM Internal API

const TUid KCRUidIM = {0x1028289C};

// Authorization level of presence (EAll = 0, EFriends = 1,  ENobodyAll = 2)
const TUint32 KIMAuthPresence  = 0x00000004;

// IM Internal variation API
const TUid KCRUidIMVariation = {0x1028289D};

const TUid KCRUidIMNG = {0x1028289C};


// Tone settings from IM application (Enabled/disabled)
// If setting is disabled, tone is selected from Profiles else
// settings is set from IM application and Profiles hides IM tone setting item
const TUint32 KIMFeatBrandableTone = 0x00000001;

#endif      // IMNGINTERNALCRKEYS_H
