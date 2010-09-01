/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server starter definitions.
*
*/

#ifndef __PENGSERVERSTARTERDEFS_H__
#define __PENGSERVERSTARTERDEFS_H__

//  INCLUDES
#include <E32STD.H>



// CONSTANTS
_LIT( KPEngLaunchMutexNameExtension, "[launchMutex]" );
const TInt KPEngDefaultMinHeapSize = 0x1000; // 4KB
const TInt KPEngDefaultMaxHeapSize = 0x1000000; // 16MB
const TInt KPEngSrvConnTries = 7;
const TInt KPEngSrvConnRetryWait = 500; //MicroSeconds => 0.0005s
_LIT( KServerNameExtDll, "dll" );
_LIT( KServerNameExtExe, "exe" );


#endif      //  __PENGSERVERSTARTERDEFS_H__


// End of file

