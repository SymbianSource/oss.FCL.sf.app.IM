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
* Description:  Logging tools. Uniforms style to write debug data.
*
*/


/**
*     Logging tools. Uniforms style to write debug data to
*     screen using RDebug or to a file with RFileLogger.
*
*     Usage:
*     1.Configuring:
*
*       Logging and debug printing is configured with following macros
*           IMPSCUI_ENABLE_DEBUG_PRINT         (defining this enables debug printing)
*           IMPSCUI_DEBUG_OUTPUT_TO_FILE       (debug printing goes to file)
*
*       Debug printing can be configured on project level by defining desired
*       macros in .mmp file like this
*           //enable file logging
*           MACRO IMPSCUI_ENABLE_DEBUG_PRINT
*           MACRO IMPSCUI_DEBUG_OUTPUT_TO_FILE
*
*       You may also automate the debug printing to follow current build
*       variant like this:
*           #ifdef _DEBUG
*           MACRO IMPSCUI_ENABLE_DEBUG_PRINT
*           #endif // _DEBUG
*
*       When using debug printing to file, flogger.lib needs to be added in
*       mmp file
*           LIBRARY  flogger.lib
*       and following directory must be manually done before loggin
*       (no directory, no logs)  Epoc32\Wins\c\logs\IMPSCUI\
*
*
*     2.Printing:
*       // normal string                                    output:
*       IMPSCUI_DP( D_IMPSCUI_LIT( "Some text." ) );        >> IMPSCUI: Some text.
*       IMPSCUI_DP_TXT( "Some text.");                      >> IMPSCUI: Some text.
*
*       // string with variables
*       TInt index( 99 );
*       _LIT( KExample, "Example" );
*       IMPSCUI_DP( D_IMPSCUI_LIT( "Some text: %d" ), 100 );         >> IMPSCUI: Some text: 100
*       IMPSCUI_DP( D_IMPSCUI_LIT( "Some text: %d" ), index );       >> IMPSCUI: Some text: 99
*       IMPSCUI_DP( D_IMPSCUI_LIT( "Some text: %S" ), &KExample );   >> IMPSCUI: Some text: Example
*
*/
#ifndef __IMPSCUIDEBUGPRINT_H__
#define __IMPSCUIDEBUGPRINT_H__
#include "IMPSCommonUiProjectDefinitionMacros.h"


// Define macro below to enable the thread name in debug prints
#define IMPSCUI_DEBUG_STR_USE_THREAD_NAME


// Debug logging is enabled, you may enable debug printing in release builds also
#ifdef IMPSCUI_ENABLE_DEBUG_PRINT
// no include files if no debug printing --> faster compile time
// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <flogger.h>

#ifndef _DEBUG
// warn in release build!!
#if defined(__VC32__)
// Code scanner warning use of #pragma message (Id: 68) ignored
// because we want this message if debug printing is on in release build
#pragma message( "Warning: IMPSCommonUi Debug Printing is _ON_ in release build!" ) // CSI: 68 # See above
#else // __GCC32__
#warning "IMPSCommonUi Debug Printing is _ON_ in release build!"
#endif // __VC32__
#endif

/**
 * Depending if the build is UNICODE or not, define the
 * helper macros that insert IMPSCUI prefix.
 */
#ifdef _UNICODE
#define IMPSCUI_TOKEN_PASTING(s) L##s
#define IMPSCUI_TO_UNICODE(s) IMPSCUI_TOKEN_PASTING(s)
#define IMPSCUI_DEBUG_STR(m) IMPSCUI_TO_UNICODE("IMPSCUI: ") L##m
#else
#define IMPSCUI_DEBUG_STR "IMPSCUI: "
#endif // _UNICODE


#ifdef IMPSCUI_DEBUG_STR_USE_THREAD_NAME
/**
 * Helper macro for defining debug strings with plain debug text.
 * When we use thread name, we don't need "IMPSCUI:" prefix
 */
#define D_IMPSCUI_LIT(s) _L(s) // CSI: 78 # We need to use _L here
#else
/**
 * Helper macro for defining debug strings with "IMPSCUI:" prefix.
 */
#define D_IMPSCUI_LIT(s) TPtrC((const TText *) IMPSCUI_DEBUG_STR(s))
#endif // IMPSCUI_DEBUG_STR_USE_THREAD_NAME



/***************************************************************************
 * Debug prints to file
 ***************************************************************************/
#ifdef IMPSCUI_DEBUG_OUTPUT_TO_FILE
/**
 * Method to handle file writing
 */
_LIT( KIMPSCUIDebugOutputDir, "IMPSCUI" );
_LIT( KIMPSCUIDebugOutputFileName, "IMPSCUI.log" );
inline void IMPSCUIDebugWriteFormat( TRefByValue<const TDesC> aFmt, ... )
    {
    //Use RFileLogger to write log - no time and date
    RFileLogger logger;
    logger.Connect();
    //use date, use time
    logger.SetDateAndTime( EFalse, ETrue );
    logger.CreateLog( KIMPSCUIDebugOutputDir,
                      KIMPSCUIDebugOutputFileName,
                      EFileLoggingModeAppend );

    TBuf< 256 > buffer;
#ifdef IMPSCUI_DEBUG_STR_USE_THREAD_NAME
        {
        buffer.Append( _L( "[" ) ); // CSI: 78 # We need to use _L here
        buffer.Append( RThread().Name() );
        buffer.Append( _L( "] " ) ); // CSI: 78 # We need to use _L here
        }
#endif // IMPSCUI_DEBUG_STR_USE_THREAD_NAME

        {
        VA_LIST list;
        VA_START ( list, aFmt );
        TBuf< 300 > formatted;
        formatted.FormatList( aFmt, list );
        buffer.Append( formatted.Left( buffer.MaxLength() - buffer.Length() ) );
        }

    logger.Write( buffer );
    logger.CloseLog();
    logger.Close();
    }

/**
 * Actual debug printters.
 * Output to log file.
 */
#define IMPSCUI_DP IMPSCUIDebugWriteFormat
#define IMPSCUI_DP_TXT(s) IMPSCUIDebugWriteFormat( D_IMPSCUI_LIT(s) )




/***************************************************************************
 * Debug prints to debug output
 ***************************************************************************/
#else
inline void IMPSCUIDebugWriteFormat( TRefByValue<const TDesC> aFmt, ... )
    {
    TBuf< 256 > buffer;
#ifdef IMPSCUI_DEBUG_STR_USE_THREAD_NAME
        {
        buffer.Append( _L( "[" ) ); // CSI: 78 # We need to use _L here
        buffer.Append( RThread().Name() );
        buffer.Append( _L( "] " ) ); // CSI: 78 # We need to use _L here
        }
#endif // IMPSCUI_DEBUG_STR_USE_THREAD_NAME

        {
        VA_LIST list;
        VA_START ( list, aFmt );
        TBuf< 300 > formatted;
        formatted.FormatList( aFmt, list );
        buffer.Append( formatted.Left( buffer.MaxLength() - buffer.Length() ) );
        }
    //prevent crashing if buffer has format directives
    RDebug::Print( _L( "%S" ), &buffer );
    }

/**
 * Actual debug printters.
 * Output to debugger output.
 */
#define IMPSCUI_DP IMPSCUIDebugWriteFormat
#define IMPSCUI_DP_TXT(s) IMPSCUIDebugWriteFormat( D_IMPSCUI_LIT(s) )
#endif



#else   //IMPSCUI_ENABLE_DEBUG_PRINT

/**
 *
 * Empty implementations for non-debug printing build versions.
 *
 */

/**
 * Dummy struct for checking that all IMPSCUI_DP's define string
 * literals using space-saving D_IMPSCUI_LIT.
 */
struct TIMPSCUIEmptyDebugString { };

/**
 * Macro for defining debug-only literal strings (empty release version)
 */
#define D_IMPSCUI_LIT(s) TIMPSCUIEmptyDebugString()

/**
 * Macro for empty debug print function
 */
#define IMPSCUI_DP_TXT(s) IMPSCUI_DP( D_IMPSCUI_LIT(s) )


/// Empty debug print function for release builds.
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString )
    {
    }

template<class T1>
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString, T1 )
    {
    }

template<class T1, class T2>
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString, T1, T2 )
    {
    }

template<class T1, class T2, class T3>
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString, T1, T2, T3 )
    {
    }

template<class T1, class T2, class T3, class T4>
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString, T1, T2, T3, T4 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5>
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString, T1, T2, T3, T4, T5 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6>
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString, T1, T2, T3, T4, T5, T6 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString, T1, T2, T3, T4, T5, T6, T7 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void IMPSCUI_DP( TIMPSCUIEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8 )
    {
    }


#endif  // IMPSCUI_ENABLE_DEBUG_PRINT

/* Some general wrappers to IMPSCUI_DP for convenience.
 *
 * Since these just wrap IMPSCUI_DP, they work transparently: the macros write
 * stuff to debug output or to a file, whichever you are using,
 * you don't have to care.
 *
 * Since IMPSCUI_DP is either defined or empty inline, these won't
 * matter in either build (no, there is no noticeable penalty in compile time)
 *
 * There are three types of wrappers, output format is
 *
 * "filename:linenumber method - enter" when entering function
 * "filename:linenumber method - message" when inside function
 * "filename:linenumber method - done" when exiting function
 *
 * Example:
 * TInt CSomeClass::SomeMethod()
 * {
 *      IMPSCUI_DP_FUNC_ENTER("SomeMethod");
 *
 *      TInt i = 41;
 *
 *      IMPSCUI_DP_FUNC_DP("SomeMethod", "Doing intensive calculations");
 *
 *      i++;
 *
 *      IMPSCUI_DP_FUNC_DONE("SomeMethod");
 *  }
 *
 * You have to provide the method name yourself since the __FUNCTION__
 * preprocessor macro is not understood.
 */

// workaround 8-bit string to 16-bit unicode
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

// when entering a function
#define IMPSCUI_DP_FUNC_ENTER(method) \
    IMPSCUI_DP( D_IMPSCUI_LIT("%s:%d %s - enter"), __WFILE__, __LINE__, L ## method );

// debug print
#define IMPSCUI_DP_FUNC_DP(method,msg) \
    IMPSCUI_DP( D_IMPSCUI_LIT("%s:%d %s - %s"), __WFILE__, __LINE__, L ## method, L ## msg );

// when exiting a function
#define IMPSCUI_DP_FUNC_DONE(method) \
    IMPSCUI_DP( D_IMPSCUI_LIT("%s:%d %s - done"), __WFILE__, __LINE__, L ## method );

#endif  // __IMPSCUIDEBUGPRINT_H__

//  End of File
