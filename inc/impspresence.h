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
* Description: 
*     Presence list and subscription list objects for public WV Engine
*     Presence API.
*
*/



#ifndef CImpsPresenceList_H
#define CImpsPresenceList_H

//  INCLUDES
#include <e32base.h>
#include "ImpsConst.h"

// CONSTANTS

// DATA TYPES

// Owner type for presence
// Future version may support also list ids
enum TImpsPresOwner
    {
    EImpsUserOwner = 0
    };

// Supported default presence attribute values
enum TImpsPresAttributes
    {
    EImpsAttrComCap = 0,
    EImpsAttrOnlineStatus,
    EImpsAttrUserAvailability
    };

// CommCap Cap values
enum TImpsCap
    {
    EImpsCapIM = 0,
    EImpsCapCALL,
    EImpsCapSMS,
    EImpsCapMMS,
    EImpsCapEMAIL
    };

enum TImpsUserAvailability
    {
    EImpsUANotDefined = 0,  // PresenceValue missing
    EImpsAvailable, 
    EImpsNotAvailable,
    EImpsDiscreet
    };


#endif  
            
