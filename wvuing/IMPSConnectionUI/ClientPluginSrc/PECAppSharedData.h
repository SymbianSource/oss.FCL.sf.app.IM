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
* Description:  Defines shared data constants for presence application.
*
*/



#ifndef PECAPPSHAREDDATA_H
#define PECAPPSHAREDDATA_H

//  INCLUDES

// CONSTANTS


// DATA TYPES

// Publishing values
enum TPECAppPublishingValues
    {
    EPECDopPrivateAndPublic = 0,
    EPECDopOnlyPrivate,
    EPECDopOnlyPublic,
    EPECDopOff
    };

// Presence for new viewers values
enum TPECAppPFNVValues
    {
    EPECPFNVPublic = 0,
    EPECPFNVPrivate
    };

// Depends on profile values
enum TPECAppDOPValues
    {
    EPECDOPOff = 0,
    EPECDOPAvailAndPrivMsg,
    EPECDOPOnlyAvail,
    EPECDOPOnlyPrivMsg
    };

// Login values

enum TPECAppLoginValues
    {
    EPECLoginAutoInHomeNW = 0,
    EPECLoginAutoAlways,
    EPECLoginManual,
    EPECDoManualLoginWithStateChangeQuery
    };


#endif      // PECAPPSHAREDDATA_H

// End of File
