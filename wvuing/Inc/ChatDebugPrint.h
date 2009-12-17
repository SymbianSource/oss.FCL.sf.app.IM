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
* Description:  Debug print definitions
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
*           CHAT_ENABLE_DEBUG_PRINT         (defining this enables debug printing)
*           CHAT_DEBUG_OUTPUT_TO_FILE       (now debug printing goes to file)
*           CHAT_DEBUG_FILENAME "Some.log"  (file to store debug log)
*
*       Debug printing can be configured on project level by defining desired
*       macros in .mmp file like this
*           //enable file logging
*           MACRO CHAT_ENABLE_DEBUG_PRINT
*           MACRO CHAT_DEBUG_OUTPUT_TO_FILE
*
*       You may also automate the debug printing to follow current build
*       variant like this:
*           #ifdef _DEBUG
*           MACRO CHAT_ENABLE_DEBUG_PRINT
*           #endif // _DEBUG
*
*       The file to write debug log needs to be defined in file level.
*       (Defining it in mmp file causes errors to build procedure..)
*           #define CHAT_DEBUG_FILENAME "Example.log"
*
*       When using debug printing to file, flogger.lib needs to be added in
*       mmp file
*           LIBRARY  flogger.lib
*       and following directory must be manually done before loggin
*       (no directory, not logs) Epoc32\Wins\c\logs\CHAT\
*
*
*     2.Printing:
*
*       // normal string                                  output:
*       CHAT_DP( D_CHAT_LIT( "Some text." ) );                >> CHAT: Some text.
*       CHAT_DP( D_PLAIN_LIT( "Some text." ) );               >> Some text.
*       CHAT_DP_TXT( "Some text." );                          >> CHAT: Some text.
*
*       // string with variables
*       TInt index( 99 );
*       _LIT( KExample, "Example" );
*       CHAT_DP( D_CHAT_LIT( "Some text: %d" ), 100 );        >> CHAT: Some text: 100
*       CHAT_DP( D_CHAT_LIT( "Some text: %d" ), index );      >> CHAT: Some text: 99
*       CHAT_DP( D_CHAT_LIT( "Some text: %S" ), &KExample );  >> CHAT: Some text: Example
*
*       CHAT_DP( D_PLAIN_LIT( "Some text: %d" ), 100 );       >> Some text: 100
*       CHAT_DP( D_PLAIN_LIT( "Some text: %d" ), index );     >> Some text: 99
*       CHAT_DP( D_PLAIN_LIT( "Some text: %S" ), &KExample ); >> Some text: Example
*
*
*     3.Known issues:
*
*       - If you use macros from .mmp file remember to abld makefile <target> to
*         change flags from project.
*       - In statements like CHAT_DP( D_CHAT_LIT("Some text: %S"), &KExample );
*         parameters causes still some code to generated in release builds.
*         Propably it is best to #ifdef all debugprint blocks
*         with CHAT_ENABLE_DEBUG_PRINT statement.
*
* ============================================================================
*/


#ifndef __CHATDEBUGPRINT_H__
#define __CHATDEBUGPRINT_H__

// Debug logging is enabled, you may enable debug printing in release builds also
#ifdef CHAT_ENABLE_DEBUG_PRINT

// no include files if no debug printing --> faster compile time
// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <flogger.h>

#ifndef _DEBUG
// warn in release build
#if defined( __VC32__ )
#pragma message( "Warning: CHAT debug printing is on in release build!" ) // CSI: 68 # Warning for rel builds
#else // __GCC32__
#warning "CHAT debug printing is on in release build!"
#endif // __VC32__
#endif

/**
 * Depending if the build is UNICODE or not, define the
 * helper macros that insert CHAT prefix.
 */
#ifdef _UNICODE
#define CHAT_TOKEN_PASTING( s ) L##s
#define CHAT_TO_UNICODE( s ) CHAT_TOKEN_PASTING( s )
#define CHAT_DEBUG_STR( m ) CHAT_TO_UNICODE( "CHAT: " ) L##m
#else
#define CHAT_DEBUG_STR "CHAT: "
#endif // _UNICODE


/**
 * Helper macro for defining debug strings with plain debug text.
 */
#define D_PLAIN_LIT( s ) _L( s )    // CSI: 78 # Debug print


/**
 * Helper macro for defining debug strings with "CHAT:" prefix.
 */
#define D_CHAT_LIT( s ) TPtrC( ( const TText * ) CHAT_DEBUG_STR( s ) )



#ifdef CHAT_DEBUG_OUTPUT_TO_FILE

/**
 * Method to handle file writing
 */
inline void CHATDebugWriteFormat( TRefByValue<const TDesC> aFmt, ... )
    {
    _LIT( KDir, "Chat" );
#ifdef CHAT_DEBUG_FILENAME
    const static TLitC < sizeof( CHAT_DEBUG_FILENAME ) > KName = {
        sizeof( CHAT_DEBUG_FILENAME ) - 1,
        CHAT_TO_UNICODE( CHAT_DEBUG_FILENAME )
        };
#else
    _LIT( KName, "ChatDebug.log" );
#endif // CHAT_DEBUG_FILENAME

    // take the ellipsis parameters
    VA_LIST args;
    VA_START( args, aFmt );
    RFileLogger::WriteFormat( KDir, KName, EFileLoggingModeAppend,
                              aFmt, args );
    VA_END( args );
    }

/**
 * Actual debug printters.
 * Output to log file.
 */
#define CHAT_DP CHATDebugWriteFormat
#define CHAT_DP_TXT( s ) CHATDebugWriteFormat( D_CHAT_LIT( s ) )



#else
/**
 * Actual debug printters.
 * Output to debugger output.
 */
#define CHAT_DP RDebug::Print
#define CHAT_DP_TXT( s ) RDebug::Print( D_CHAT_LIT( s ) )
#endif


#else   //CHAT_ENABLE_DEBUG_PRINT

/**
 *
 * Empty implementations for non-debug printing build versions.
 *
 */

/**
 * Dummy struct for checking that all CHAT_DP's define string
 * literals using space-saving D_PLAIN_LIT or D_CHAT_LIT.
 */
struct TCHATEmptyDebugString { };

/**
 * Macro for defining debug-only literal strings (empty release version)
 */
#define D_PLAIN_LIT( s ) TCHATEmptyDebugString()

/**
 * Macro for defining debug-only literal strings (empty release version)
 */
#define D_CHAT_LIT( s ) TCHATEmptyDebugString()

/**
 * Macro for empty debug print function
 */
#define CHAT_DP_TXT( s ) CHAT_DP( D_CHAT_LIT( s ) )


/// Empty debug print function for release builds.
inline void CHAT_DP( TCHATEmptyDebugString )
    {
    }

template< class T1 >
inline void CHAT_DP( TCHATEmptyDebugString, T1 )
    {
    }

template< class T1, class T2 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2 )
    {
    }

template< class T1, class T2, class T3 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3 )
    {
    }

template< class T1, class T2, class T3, class T4 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3, T4 )
    {
    }

template< class T1, class T2, class T3, class T4, class T5 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3, T4, T5 )
    {
    }

template< class T1, class T2, class T3, class T4, class T5, class T6 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3, T4, T5, T6 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6,
class T7 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3, T4, T5, T6, T7 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6,
class T7, class T8 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6,
class T7, class T8, class T9 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8,
                     T9 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6,
class T7, class T8, class T9, class T10 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8,
                     T9, T10 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6,
class T7, class T8, class T9, class T10, class T11 >
inline void CHAT_DP( TCHATEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8,
                     T9, T10, T11 )
    {
    }

#endif  // CHAT_ENABLE_DEBUG_PRINT

/* Some general wrappers to CHAT_DP for convenience.
 *
 * Since these just wrap CHAT_DP, they work transparently: the macros write
 * stuff to debug output or to a file, whichever you are using,
 * you don't have to care.
 *
 * Since CHAT_DP is either defined or empty inline, these won't
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
 *      CHAT_DP_FUNC_ENTER( "SomeMethod" );
 *
 *      TInt i = 41;
 *
 *      CHAT_DP_FUNC_DP( "SomeMethod", "Doing intensive calculations" );
 *
 *      i++;
 *
 *      CHAT_DP_FUNC_DONE( "SomeMethod" );
 *  }
 *
 * You have to provide the method name yourself since the __FUNCTION__
 * preprocessor macro is not understood.
 */

#include "ChatDebugMacros.h" 	// WFILE etc.

// when entering a function
#define CHAT_DP_FUNC_ENTER( method ) \
    CHAT_DP( D_CHAT_LIT("%s:%d %s - enter" ), __WFILE__, __LINE__, L ## method );

// debug print
#define CHAT_DP_FUNC_DP( method,msg ) \
    CHAT_DP( D_CHAT_LIT( "%s:%d %s - %s" ), __WFILE__, __LINE__, L ## method, L ## msg );

// when exiting a function
#define CHAT_DP_FUNC_DONE(method) \
    CHAT_DP( D_CHAT_LIT( "%s:%d %s - done" ), __WFILE__, __LINE__, L ## method );

#endif  // __CHATDEBUGPRINT_H__


//  End of File
