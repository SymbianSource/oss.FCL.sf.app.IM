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
* Description: Utils for im launcher log.
*
*/



// INCLUDE FILES
#include "logutils.h"

// CONSTANTS
const TInt KLogBufferLength = 256;
_LIT( KLogDir, "impsc" );
_LIT( KLogFile, "launcher.txt" );

void CImApiLogger::Log( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList( aFmt, list );

//#ifdef _USE_FLOGGER
    // Write to log file
    RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, buf );
//#else
//#endif

    }

//  End of File
