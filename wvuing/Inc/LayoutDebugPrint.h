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
* Description:  Layout debug print definitions
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
*           LAYOUT_ENABLE_DEBUG_PRINT         (defining this enables debug printing)
*           LAYOUT_DEBUG_OUTPUT_TO_FILE       (now debug printing goes to file)
*           LAYOUT_DEBUG_FILENAME "Some.log"  (file to store debug log)
*
*       Debug printing can be configured on project level by defining desired
*       macros in .mmp file like this
*           //enable file logging
*           MACRO LAYOUT_ENABLE_DEBUG_PRINT
*           MACRO LAYOUT_DEBUG_OUTPUT_TO_FILE
*
*       You may also automate the debug printing to follow current build
*       variant like this:
*           #ifdef _DEBUG
*           MACRO LAYOUT_ENABLE_DEBUG_PRINT
*           #endif // _DEBUG
*
*       The file to write debug log needs to be defined in file level.
*       (Defining it in mmp file causes errors to build procedure..)
*           #define LAYOUT_DEBUG_FILENAME "Example.log"
*
*       When using debug printing to file, flogger.lib needs to be added in
*       mmp file
*           LIBRARY  flogger.lib
*       and following directory must be manually done before loggin
*       (no directory, not logs)  Epoc32\Wins\c\logs\LAYOUT\
*
*
*     2.Printing:
*
*       // normal string                                  output:
*       LAYOUT_DP( D_LAYOUT_LIT("Some text.") );                 >> LAYOUT: Some text.
*       LAYOUT_DP( D_PLAIN_LIT("Some text.") );               >> Some text.
*       LAYOUT_DP_TXT("Some text.");                          >> LAYOUT: Some text.
*
*       // string with variables
*       TInt index( 99 );
*       _LIT( KExample, "Example" );
*       LAYOUT_DP( D_LAYOUT_LIT("Some text: %d"), 100 );         >> LAYOUT: Some text: 100
*       LAYOUT_DP( D_LAYOUT_LIT("Some text: %d"), index );       >> LAYOUT: Some text: 99
*       LAYOUT_DP( D_LAYOUT_LIT("Some text: %S"), &KExample );   >> LAYOUT: Some text: Example
*
*       LAYOUT_DP( D_PLAIN_LIT("Some text: %d"), 100 );       >> Some text: 100
*       LAYOUT_DP( D_PLAIN_LIT("Some text: %d"), index );     >> Some text: 99
*       LAYOUT_DP( D_PLAIN_LIT("Some text: %S"), &KExample ); >> Some text: Example
*
*
*     3.Known issues:
*
*       - If you use macros from .mmp file remember to abld makefile <target> to
*         change flags from project.
*       - In statements like LAYOUT_DP( D_LAYOUT_LIT("Some text: %S"), &KExample );
*         parameters causes still some code to generated in release builds.
*         Propably it is best to #ifdef all debugprint blocks
*         with LAYOUT_ENABLE_DEBUG_PRINT statement.
*
* ============================================================================
*/


#ifndef __LAYOUTDEBUGPRINT_H__
#define __LAYOUTDEBUGPRINT_H__

// Debug logging is enabled, you may enable debug printing in release builds also
#ifdef LAYOUT_ENABLE_DEBUG_PRINT

// no include files if no debug printing --> faster compile time
// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <flogger.h>

#ifndef _DEBUG
// warn in release build!!
#if defined(__VC32__)
#pragma message( "Warning: LAYOUT debug printing is on in release build!" ) // CSI: 68 # Warning for rel builds
#else // __GCC32__
#warning "LAYOUT debug printing is on in release build!"
#endif // __VC32__
#endif

/**
 * Depending if the build is UNICODE or not, define the
 * helper macros that insert LAYOUT prefix.
 */
#ifdef _UNICODE
#ifndef LAYOUT_DEBUG_OUTPUT_TO_FILE
#define LAYOUT_TOKEN_PASTING( s ) L##s
#define LAYOUT_TO_UNICODE( s ) LAYOUT_TOKEN_PASTING( s )
#define LAYOUT_DEBUG_STR( m ) LAYOUT_TO_UNICODE( "LAYOUT: " ) L##m
#else
#define LAYOUT_TOKEN_PASTING( s ) L##s
#define LAYOUT_TO_UNICODE( s ) LAYOUT_TOKEN_PASTING( s )
#define LAYOUT_DEBUG_STR( m ) L##m
#endif
#else
#define LAYOUT_DEBUG_STR "LAYOUT: "
#endif // _UNICODE


/**
 * Helper macro for defining debug strings with plain debug text.
 */
#define D_LPLAIN_LIT( s ) _L( s )   // CSI: 78 # Debug print


/**
 * Helper macro for defining debug strings with "LAYOUT:" prefix.
 */
#define D_LAYOUT_LIT( s ) TPtrC( ( const TText * ) LAYOUT_DEBUG_STR( s ) )

#ifdef LAYOUT_DEBUG_OUTPUT_TO_FILE

/**
 * Method to handle file writing
 */
inline void LAYOUTDebugWriteFormat( TRefByValue< const TDesC > aFmt, ... )
    {
    _LIT( KDir, "Layout" );
#ifdef LAYOUT_DEBUG_FILENAME
    const static TLitC < sizeof( LAYOUT_DEBUG_FILENAME ) > KName = {
        sizeof( LAYOUT_DEBUG_FILENAME ) - 1, LAYOUT_TO_UNICODE(
            LAYOUT_DEBUG_FILENAME )
        };
#else
    _LIT( KName, "LayoutDebug.log" );
#endif // LAYOUT_DEBUG_FILENAME

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
#define LAYOUT_DP LAYOUTDebugWriteFormat
#define LAYOUT_DP_TXT( s ) LAYOUTDebugWriteFormat( D_LAYOUT_LIT( s ) )



#else
/**
 * Actual debug printters.
 * Output to debugger output.
 */
#define LAYOUT_DP RDebug::Print
#define LAYOUT_DP_TXT( s ) RDebug::Print( D_LAYOUT_LIT( s ) )
#endif


#else   //LAYOUT_ENABLE_DEBUG_PRINT

/**
 *
 * Empty implementations for non-debug printing build versions.
 *
 */

/**
 * Dummy struct for checking that all LAYOUT_DP's define string
 * literals using space-saving D_PLAIN_LIT or D_LAYOUT_LIT.
 */
struct TLAYOUTEmptyDebugString { };

/**
 * Macro for defining debug-only literal strings (empty release version)
 */
#define D_LPLAIN_LIT( s ) TLAYOUTEmptyDebugString()

/**
 * Macro for defining debug-only literal strings (empty release version)
 */
#define D_LAYOUT_LIT( s ) TLAYOUTEmptyDebugString()

/**
 * Macro for empty debug print function
 */
#define LAYOUT_DP_TXT( s ) LAYOUT_DP( D_LAYOUT_LIT( s ) )


/// Empty debug print function for release builds.
inline void LAYOUT_DP( TLAYOUTEmptyDebugString )
    {
    }

template< class T1 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1 )
    {
    }

template< class T1, class T2 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2 )
    {
    }

template< class T1, class T2, class T3 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3 )
    {
    }

template< class T1, class T2, class T3, class T4 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3, T4 )
    {
    }

template< class T1, class T2, class T3, class T4, class T5 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3, T4, T5 )
    {
    }

template< class T1, class T2, class T3, class T4, class T5, class T6 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3, T4, T5, T6 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3, T4, T5, T6, T7 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6, class T7,
class T8 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6, class T7,
class T8, class T9 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8, T9 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6, class T7,
class T8, class T9, class T10 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8, T9,
                       T10 )
    {
    }

template < class T1, class T2, class T3, class T4, class T5, class T6, class T7,
class T8, class T9, class T10, class T11 >
inline void LAYOUT_DP( TLAYOUTEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8, T9,
                       T10, T11 )
    {
    }

#endif  // LAYOUT_ENABLE_DEBUG_PRINT

/* Some general wrappers to LAYOUT_DP for convenience.
 *
 * Since these just wrap LAYOUT_DP, they work transparently: the macros write
 * stuff to debug output or to a file, whichever you are using,
 * you don't have to care.
 *
 * Since LAYOUT_DP is either defined or empty inline, these won't
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
 *      LAYOUT_DP_FUNC_ENTER("SomeMethod");
 *
 *      TInt i = 41;
 *
 *      LAYOUT_DP_FUNC_DP("SomeMethod", "Doing intensive calculations");
 *
 *      i++;
 *
 *      LAYOUT_DP_FUNC_DONE("SomeMethod");
 *  }
 *
 * You have to provide the method name yourself since the __FUNCTION__
 * preprocessor macro is not understood.
 */

#include "ChatDebugMacros.h"	// WFILE etc.

// when entering a function
#define LAYOUT_DP_FUNC_ENTER( method ) \
    LAYOUT_DP( D_LAYOUT_LIT( "%s:%d %s - enter" ), __WFILE__, __LINE__, L ## method );

// debug print
#define LAYOUT_DP_FUNC_DP( method,msg ) \
    LAYOUT_DP( D_LAYOUT_LIT( "%s:%d %s - %s" ), __WFILE__, __LINE__, L ## method, L ## msg );

// when exiting a function
#define LAYOUT_DP_FUNC_DONE( method ) \
    LAYOUT_DP( D_LAYOUT_LIT( "%s:%d %s - done" ), __WFILE__, __LINE__, L ## method );

// Helpper Method to write rect's value
#ifdef LAYOUT_ENABLE_DEBUG_PRINT
#define LAYOUT_DP_RECT( name, rect ) \
    LAYOUT_DP( \
               D_LAYOUT_LIT( "%s: %d,%d;%d,%d (%dx%d)" ), \
               L ## name, \
               rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY, \
               rect.Width(), rect.Height() );

#else //LAYOUT_ENABLE_DEBUG_PRINT
#define LAYOUT_DP_RECT( name, rect ) \
    LAYOUT_DP_TXT( "LAYOUT_DP_RECT" );

#endif //LAYOUT_ENABLE_DEBUG_PRINT

#endif  // __LAYOUTDEBUGPRINT_H__


//  End of File
