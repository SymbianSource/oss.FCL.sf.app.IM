/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Logging tools. Uniforms style to write debug data to
*                screen using RDebug or to a file with RFileLogger.
*
*/



/*
*
*  Description:
*
*     Logging tools. Uniforms style to write debug data to
*     screen using RDebug or to a file with RFileLogger.
*
*     Usage:
*     1.Configuring:
*
*       Logging and debug printing is configured with following macros
*           SSS_ENABLE_DEBUG_PRINT         (defining this enables debug printing)
*           SSS_DEBUG_OUTPUT_TO_FILE       (now debug printing goes to file)
*           SSS_DEBUG_FILENAME "Some.log"  (file to store debug log)
*
*       Debug printing can be configured on project level by defining desired
*       macros in .mmp file like this
*           //enable file logging
*           MACRO SSS_ENABLE_DEBUG_PRINT
*           MACRO SSS_DEBUG_OUTPUT_TO_FILE
*
*       You may also automate the debug printing to follow current build
*       variant like this:
*           #ifdef _DEBUG
*           MACRO SSS_ENABLE_DEBUG_PRINT
*           #endif // _DEBUG
*
*       The file to write debug log needs to be defined in file level.
*       (Defining it in mmp file causes errors to build procedure..)
*           #define SSS_DEBUG_FILENAME "Example.log"
*
*       When using debug printing to file, flogger.lib needs to be added in
*       mmp file
*           LIBRARY  flogger.lib
*       and following directory must be manually done before loggin
*       (no directory, not logs).
*           Epoc32\Wins\c\logs\SSS\
*
*
*     2.Printing:
*
*       // normal string                                  output:
*       SSS_DP( D_SSS_LIT("Some text.") );                 >> SSS: Some text.
*       SSS_DP( D_PLAIN_LIT("Some text.") );               >> Some text.
*       SSS_DP_TXT("Some text.");                          >> SSS: Some text.
*
*       // string with variables
*       TInt index( 99 );
*       _LIT( KExample, "Example" );
*       SSS_DP( D_SSS_LIT("Some text: %d"), 100 );         >> SSS: Some text: 100
*       SSS_DP( D_SSS_LIT("Some text: %d"), index );       >> SSS: Some text: 99
*       SSS_DP( D_SSS_LIT("Some text: %S"), &KExample );   >> SSS: Some text: Example
*
*       SSS_DP( D_PLAIN_LIT("Some text: %d"), 100 );       >> Some text: 100
*       SSS_DP( D_PLAIN_LIT("Some text: %d"), index );     >> Some text: 99
*       SSS_DP( D_PLAIN_LIT("Some text: %S"), &KExample ); >> Some text: Example
*/


#ifndef __SSSDEBUGPRINT_H__
#define __SSSDEBUGPRINT_H__
#include "IMPSSAPSettingsStoreDebugDefinitionMacros.h"


// Debug logging is enabled, you may enable debug printing in release builds also
#ifdef SSS_ENABLE_DEBUG_PRINT

// no include files if no debug printing --> faster compile time
// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <flogger.h>

#ifndef _DEBUG
// warn in release build!!
#if defined(__VC32__)
#pragma message( "Warning: SSS debug printing is on in release build!" ) // CSI: 68 #
#else // __GCC32__
#warning "SSS debug printing is on in release build!"
#endif // __VC32__
#endif

/**
 * Depending if the build is UNICODE or not, define the
 * helper macros that insert SSS prefix.
 */
#ifdef _UNICODE
#define SSS_TOKEN_PASTING(s) L##s
#define SSS_TO_UNICODE(s) SSS_TOKEN_PASTING(s)
#define SSS_DEBUG_STR(m) SSS_TO_UNICODE("SSS: ") L##m
#else
#define SSS_DEBUG_STR "SSS: "
#endif // _UNICODE


/**
 * Helper macro for defining debug strings with plain debug text.
 */
#define D_PLAIN_LIT(s) _L(s)	// CSI: 78 #


/**
 * Helper macro for defining debug strings with "SSS:" prefix.
 */
#define D_SSS_LIT(s) TPtrC((const TText *) SSS_DEBUG_STR(s))



#ifdef SSS_DEBUG_OUTPUT_TO_FILE

/**
 * Method to handle file writing
 */
inline void SSSDebugWriteFormat( TRefByValue<const TDesC> aFmt, ... )
    {
    _LIT( KDir, "SSS" );
#ifdef SSS_DEBUG_FILENAME
    const static TLitC < sizeof( SSS_DEBUG_FILENAME ) > KName = {sizeof( SSS_DEBUG_FILENAME ) - 1, SSS_TO_UNICODE( SSS_DEBUG_FILENAME ) };
#else
    _LIT( KName, "SSSDebug.log" );
#endif // SSS_DEBUG_FILENAME

    //Use RFileLogger to write log - no time and date
    RFileLogger logger;
    logger.Connect();
    //use date, use time
    logger.SetDateAndTime( EFalse, ETrue );
    logger.CreateLog( KDir, KName, EFileLoggingModeAppend );

    // take the ellipsis parameters
    VA_LIST args;
    VA_START( args, aFmt );
    logger.WriteFormat( aFmt, args );
    VA_END( args );
    logger.CloseLog();
    logger.Close();
    }

/**
 * Actual debug printters.
 * Output to log file.
 */
#define SSS_DP SSSDebugWriteFormat
#define SSS_DP_TXT(s) SSSDebugWriteFormat( D_SSS_LIT(s) )



#else
/**
 * Debug prints to debug output
 */
inline void SSSDebugWriteFormat( TRefByValue<const TDesC> aFmt, ... )
    {
    TBuf< 256 > buffer;
    _LIT( KOpenSqBr, "[" );
    _LIT( KCloseSqBr, "] " );

    buffer.Append( KOpenSqBr );

#if defined(__WINS__)
    buffer.Append( RThread().Name() );
#else
    buffer.Append( RProcess().Name() );
#endif

    buffer.Append( KCloseSqBr );
        {
        VA_LIST list;
        VA_START ( list, aFmt );
        TBuf< 300 > formatted;
        formatted.FormatList( aFmt, list );
        buffer.Append( formatted.Left( buffer.MaxLength() - buffer.Length() ) );
        }

    RDebug::Print( _L( "%S" ), &buffer ); //handle format directives correctly
    }

/**
 * Actual debug printters.
 * Output to debugger output.
 */
#define SSS_DP SSSDebugWriteFormat
#define SSS_DP_TXT(s) SSSDebugWriteFormat( D_SSS_LIT(s) )
#endif


#else   //SSS_ENABLE_DEBUG_PRINT

/**
 *
 * Empty implementations for non-debug printing build versions.
 *
 */

/**
 * Dummy struct for checking that all SSS_DP's define string
 * literals using space-saving D_PLAIN_LIT or D_SSS_LIT.
 */
struct TSSSEmptyDebugString { };

/**
 * Macro for defining debug-only literal strings (empty release version)
 */
#define D_PLAIN_LIT(s) TSSSEmptyDebugString()

/**
 * Macro for defining debug-only literal strings (empty release version)
 */
#define D_SSS_LIT(s) TSSSEmptyDebugString()

/**
 * Macro for empty debug print function
 */
#define SSS_DP_TXT(s) SSS_DP( D_SSS_LIT(s) )


/// Empty debug print function for release builds.
inline void SSS_DP( TSSSEmptyDebugString )
    {
    }

template<class T1>
inline void SSS_DP( TSSSEmptyDebugString, T1 )
    {
    }

template<class T1, class T2>
inline void SSS_DP( TSSSEmptyDebugString, T1, T2 )
    {
    }

template<class T1, class T2, class T3>
inline void SSS_DP( TSSSEmptyDebugString, T1, T2, T3 )
    {
    }

template<class T1, class T2, class T3, class T4>
inline void SSS_DP( TSSSEmptyDebugString, T1, T2, T3, T4 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5>
inline void SSS_DP( TSSSEmptyDebugString, T1, T2, T3, T4, T5 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6>
inline void SSS_DP( TSSSEmptyDebugString, T1, T2, T3, T4, T5, T6 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void SSS_DP( TSSSEmptyDebugString, T1, T2, T3, T4, T5, T6, T7 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void SSS_DP( TSSSEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8 )
    {
    }


#endif  // SSS_ENABLE_DEBUG_PRINT
#endif  // __SSSDEBUGPRINT_H__


//  End of File
