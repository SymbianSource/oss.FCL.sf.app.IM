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
* Description:  Logger implementation
*
*/

// INCLUDE FILES
#include "PEngLogger.h"
#include "PEngConfig.h"
#include "PresenceDebugPrint.h"

#include <E32Svr.h>
#include <E32Std.h>
#include <flogger.h>


/**
 * Handler used by logger to truncate the string
 * rather than panic in case of buffer overflow.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( TPEngOverflowTruncate ) : public TDes16Overflow
    {
public:
    void Overflow( TDes16& /*aDes*/ ) {}
    };



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngLogger::WriteLog()
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngLogger::WriteLog( TRefByValue<const TDesC> aFmt, ... )
    {
    //Suppress unused formal parameter warning
    ( void )aFmt;

#ifdef PENG_ENABLE_DEBUG_PRINT

    //Format the log line
    TBuf< KPEngMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( RThread().Name() );
    buffer.Append( _L( "] " ) );            // CSI: 78 #

    TPEngOverflowTruncate overflow;

    VA_LIST list;
    VA_START( list, aFmt );
    buffer.AppendFormatList( aFmt, list, &overflow );


#ifdef PENG_DEBUG_OUTPUT_TO_FILE
    //Log goes to file
    RFileLogger logger;
    if ( logger.Connect() == KErrNone )
        {
        logger.SetDateAndTime( EFalse, ETrue );
        logger.CreateLog( KPENGDebugOutputDir,
                          KPENGDebugOutputFileName,
                          EFileLoggingModeAppend );

        logger.Write( buffer );
        logger.CloseLog();
        logger.Close();
        }

#else

    //Log goes to default debug output
    RDebug::Print( _L( "%S" ), &buffer );

#endif  //PENG_DEBUG_OUTPUT_TO_FILE

#endif //PENG_ENABLE_DEBUG_PRINT
    }


// End of File


