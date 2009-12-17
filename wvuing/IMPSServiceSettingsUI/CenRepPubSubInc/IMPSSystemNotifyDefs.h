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

#ifndef __IMPSSYSTEMNOTIFYDEFSPUBSUB_H
#define __IMPSSYSTEMNOTIFYDEFSPUBSUB_H

#include <SaCls.h>


#include <ctsydomainpskeys.h>

const TUid KIMPSSystemUidCurrentCall = {0x101F8787};

enum TIMPSSystemCurrentCall
    {
    EIMPSSystemCallNone = EPSCTsyCallTypeNone,
    EIMPSSystemCallVoice = EPSCTsyCallTypeCSVoice,
    EIMPSSystemCallFax = EPSCTsyCallTypeFax,
    EIMPSSystemCallData = EPSCTsyCallTypeData,
    EIMPSSystemCallAlerting = EPSCTsyCallStateAlerting,
    EIMPSSystemCallRinging = EPSCTsyCallStateRinging,
    EIMPSSystemCallAlternating = EPSCTsyCallStateAnswering, // answering why?
    EIMPSSystemCallDialling = EPSCTsyCallStateDialling,
    EIMPSSystemCallAnswering = EPSCTsyCallStateAnswering,
    EIMPSSystemCallDisconnecting = EPSCTsyCallStateDisconnecting
    };


const TUid KIMPSSystemUidNetworkStatus = {KUidNetworkStatusValue};

enum TIMPSSystemNetworkStatus
    {
    EIMPSSystemNetworkAvailable = ESANetworkAvailable,
    EIMPSSystemNetworkUnAvailable = ESANetworkUnAvailable
    };

#endif      // __IMPSSHAREDDATADEFSPUBSUB_H

// End of File
