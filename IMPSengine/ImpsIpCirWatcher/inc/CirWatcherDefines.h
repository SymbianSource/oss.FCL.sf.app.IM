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
* Description: CIR CONSTANTS
*
*
*/


#ifndef __CIRWATCHERDEFINES__
#define __CIRWATCHERDEFINES__

// INCLUDES
#include <e32base.h>

//TCP CIR CONSTANTS
_LIT( KTcpLogDir,                               "ImpsTcpWatcher" );
_LIT( KTcpWatcherLogFile,                       "TcpWatcher.txt" );
_LIT( KHELOMessage,                             "HELO " );
_LIT( KMessageTermination,                      "\r\n" );
_LIT8( KImpsPingData,                           "PING\r\n" );
_LIT8( KImpsPingResponseOK,                     "OK" );
_LIT8( KImpsPingResponseWVCI,                   "WVCI" );
const TInt KTcpLogBufferMaxSize                 = 2000;

//UDP CIR CONSTANTS
const TInt KUdpLogBufferMaxSize                 = 2000;
_LIT( KUdpLogDir,                               "ImpsUdpWatcher" );
_LIT( KUdpWatcherLogFile,                       "UdpWatcher.txt" );

//FORWARD DECLARATIONS
class MImpsCirReceiver;
class MImpsConnectionManager;

#endif

// End of File
