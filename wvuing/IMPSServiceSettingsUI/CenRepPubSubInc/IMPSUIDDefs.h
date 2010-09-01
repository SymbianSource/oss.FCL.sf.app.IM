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
* Description:  Some constants for usage of system notifier pub&sub implementation
*
*/

#ifndef __IMPSUIDDEFSPUBSUB_H
#define __IMPSUIDDEFSPUBSUB_H

#include <e32property.h>

const TUid KIMPSServSettUid = 		  { 0x101F879C };
const TUid KIMPSConnUiPermanentUid =  { 0x101F879B };
// this is now actually the always online manager server UID
const TUid KIMPSConnUiTemporaryUid =  {KUidSystemCategoryValue};
const TUid KIMPSChatClientKeyUid =    { 0x1028289C };
const TUid KIMPSProfileEngineKeyUid = { 0x101F8798 };
const TUid KIMPSPresenceKeyUid =      { 0x101F8796 };
// this is now actually the always online manager server UID
const TUid KIMPSServSettNotifyUid =   {KUidSystemCategoryValue};

#endif      // __IMPSUIDDEFSPUBSUB_H

// End of File
