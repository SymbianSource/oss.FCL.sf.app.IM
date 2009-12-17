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
* Description:  Presence Engine specific CenRep keys.
*
*/

#ifndef __PRESENCEENGINEINTERNALCRKEYS_H
#define __PRESENCEENGINEINTERNALCRKEYS_H

// Presence Engine Variation API
// Presence Engine variation keys.
const TUid KCRUIDPresenceEngineVariation = {0x101F8792};



// Integer flag indicating whether Presence Engine supports
// Infolink presence attribute or not.
const TUint32 KPEngVariationInfoLinkSupported = 0x00000001;
enum TPEngVariationInfoLinkSupported
    {
    EPEngVariationInfoLinkNotSupported = 0,
    EPEngVariationInfoLinkSupported = 1
    };



#endif      // __PRESENCEENGINEINTERNALCRKEYS_H


//  End of File


