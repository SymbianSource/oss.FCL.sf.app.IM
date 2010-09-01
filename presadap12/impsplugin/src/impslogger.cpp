/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  logger implementation
*
*/

// INCLUDE FILES
#include "impslogger.h"
#include "IMPSConfig.h"
#include "impsdebugprint.h"

#include <E32Svr.h>
#include <E32Std.h>
#include <flogger.h>


/**
 * Handler used by logger to truncate the string
 * rather than panic in case of buffer overflow.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( TIMPSOverflowTruncate ) : public TDes16Overflow
    {
public:
    void Overflow( TDes16& /*aDes*/ ) {}
    };



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ImpsLogger::WriteLog()
// -----------------------------------------------------------------------------
//
//EXPORT_C void IMPSLogger::WriteLog( TRefByValue<const TDesC> aFmt,... )
void IMPSLogger::WriteLog( TRefByValue<const TDesC> aFmt, ... )
    {
    //Suppress unused formal parameter warning
    ( void )aFmt;

#ifdef IMPS_ENABLE_DEBUG_PRINT

    //Format the log line
    TBuf< KIMPSMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( RThread().Name() );
    buffer.Append( _L( "] " ) );            // CSI: 78 #

    TIMPSOverflowTruncate overflow;

    VA_LIST list;
    VA_START( list, aFmt );
    buffer.AppendFormatList( aFmt, list, &overflow );


#ifdef IMPS_DEBUG_OUTPUT_TO_FILE
    //Log goes to file
    RFileLogger logger;
    if ( logger.Connect() == KErrNone )
        {
        logger.SetDateAndTime( ETrue, ETrue );
        logger.CreateLog( KIMPSDebugOutputDir,
                          KIMPSDebugOutputFileName,
                          EFileLoggingModeAppend );

        logger.Write( buffer );
        logger.CloseLog();
        logger.Close();
        }

#else

    //Log goes to default debug output
    RDebug::Print( _L( "%S" ), &buffer );

#endif  //IMPS_DEBUG_OUTPUT_TO_FILE

#endif //IMPS_ENABLE_DEBUG_PRINT
    }


// End of File


