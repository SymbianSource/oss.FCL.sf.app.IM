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
* Description:  Common constants definition for Presence server and its client
*
*/


#ifndef __TPENGSERVERCOMMON_H__
#define __TPENGSERVERCOMMON_H__

//  CONSTANTS
_LIT( KServerName, "PEngTransactionServer" );

_LIT( KSessionName, "DPEngTransactionServerSession" );

//Client's version number.
const TInt KClientVersionMajor = 1;
const TInt KClientVersionMinor = 1;
const TInt KClientVersionBuild = 1;

const CActive::TPriority KServerPriority = CActive::EPriorityStandard;
const TThreadPriority KServerThreadPriority = EPriorityNormal;



/**
* Base name for Presence Server executable.
* Executable drive, folder and filename extension are determined
* by the PEngServerStarter during the compile / runtime.
*
* This has to match component base name in component mmp file.
*/
_LIT( KServerExe, "PENGSERVER2" );



enum TPEngServerPanic
    {
    EClientMissing = 1,

    ESchedulerError,

    EStateMachineInitFailed,

    EPECBadDescriptor,

    EPECBadRequest
    };

#endif      //  __TPENGSERVERCOMMON_H__

