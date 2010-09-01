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

#ifndef __PENGSTORAGESERVERCOMMON_H__
#define __PENGSTORAGESERVERCOMMON_H__

//  INCLUDES
#include <e32base.h>



//  CONSTANTS

//Client's version number. Major, Minor, Build

/**
 * though store version has length shorted, it also used to
 * ensure certain minimal size of buffer => 10
 */
const TInt KStoreVersionSize = 20;

/**
 * Tolerance of the size of the store entry is 10% (100 /10)
 */
const TInt KStoreEntrySizeTolerance = 10;

/**
 * overlapp of the Store entry size for retrieving is 20% (100/5)
 */
const TInt KStoreEntryRetrieveOverlap = 5;

/**
 * Id store entry provides senseless size, this size is use instead
 */
const TInt KStoreEntryDefaultSize = 250;


/**
 * Numbers are stored as 16 bits - 2 bytes
 */
const TInt KNumberSize = 4;

const TInt KClientVersionMajor = 1;
const TInt KClientVersionMinor = 0;
const TInt KClientVersionBuild = 0;
const TInt KRequiredVersionMajor = 1;
const TInt KRequiredVersionMinor = 0;
const TInt KRequiredVersionBuild = 0;

const CActive::TPriority KServerPriority = CActive::EPriorityStandard;
const TThreadPriority KServerThreadPriority = EPriorityNormal;


const TInt32 KServerCloseTimeOut = 5000000; // (5 seconds)

_LIT( KStorageServerName, "PEngStorageServer" );
_LIT( KStorageSessionName, "DPBStorageServerSession" );

/**
 * Base name for Storage Server executable.
 * Executable drive, folder and filename extension are determined
 * by the PEngServerStarter during the compile / runtime.
 *
 * This has to match component base name in component mmp file.
 */
_LIT( KStorageServerExe, "PENGSTORSERV" );



//  DATA TYPES

enum TPEngStorageServerPanic
    {
    EClientMissing = 1,
    ESchedulerError,
    EErrorInBTree
    };

enum TPEngStoragClientPanic
    {
    ESSBadRequest = 1024,
    ESSBadDescriptor = 1025
    };

#endif      //  __PENGSTORAGESERVERCOMMON_H__

