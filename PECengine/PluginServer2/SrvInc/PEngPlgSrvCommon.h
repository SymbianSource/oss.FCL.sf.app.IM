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
* Description:  See class description below.
*
*/


#ifndef __CPENGPLGSRVCOMMON_H__
#define __CPENGPLGSRVCOMMON_H__

//  INCLUDES
#include <e32base.h>

//Client's version number. Major, Minor, Build
const TInt KClientVersionMajor = 1;
const TInt KClientVersionMinor = 0;
const TInt KClientVersionBuild = 0;
const TInt KRequiredVersionMajor = 1;
const TInt KRequiredVersionMinor = 0;
const TInt KRequiredVersionBuild = 0;

/**
 * Base name for Storage Server executable.
 * Executable drive, folder and filename extension are determined
 * by the PEngServerStarter during the compile / runtime.
 *
 * This has to match component base name in component mmp file.
 */
_LIT( KPEngPluginServerExe, "PEngPlgServ2" );

_LIT8(  KPEngOfflineType, "PECOffline" );
_LIT8(  KPEngOnlineType, "PECOnline" );

//  DATA TYPES
enum TPEngCacheServerMessages
    {
    EPEngPlgShutdownServer = 1,	    // no parameters
    EPEngPlgSetStateOffline,        // no parameters
    EPEngPlgSetStateOnline,         // no parameters
    EPEngPlgPluginCount,		    // no parameters, return >= 0 == count, return < 0 == error
    EPEngPlgPluginUid,              // parameter 0 is for the plugin index
    // return >= 0 == uid, return < 0 == error
    EPEngPlgServerLastRequest       // keep this as the last one for policy setting
    };

enum TPEngCacheServerPanic
    {
    EClientMissing = 1,
    ESchedulerError,
    };

enum TPEngCacheClientPanic
    {
    EBadRequest = 1024
    };

enum TPEngPlgOnlineState
    {
    EPEngPlgSvrStateUnknown,
    EPEngPlgSvrStateOffline,
    EPEngPlgSvrStateOnline
    };

#endif      //  __CPENGPLGSRVCOMMON_H__

