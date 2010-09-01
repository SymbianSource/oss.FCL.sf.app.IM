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
* Description:  Presence Engine logging tools.
*
*/

#ifndef __PENGDEBUGPRINT_H__
#define __PENGDEBUGPRINT_H__

//  INCLUDES
#include    "PEngConfig.h"


// -----------------------------------------------------------------------------
// Debug logging is enabled
// -----------------------------------------------------------------------------
#ifdef PENG_ENABLE_DEBUG_PRINT

// INCLUDES
#include    "PEngLogger.h"

// CONSTANTS
_LIT( KPENGDebugOutputDir, "PENG" );
_LIT( KPENGDebugOutputFileName, "PEng2.log" );
const TInt KPEngMaxLogLineLength = 250;


// TARGET WARNING
#ifndef _DEBUG
#if defined(__VC32__)
#pragma message( "Warning: Presence Engine debug printing _ON_" )	// CSI: 68 #
#else // __GCC32__
#warning "Presence Engine debug printing _ON_"
#endif // __VC32__
#endif


// DEBUG PRINT INDIRECTION
#define D_PENG_LIT(s) _L(s)		// CSI: 78 #
#define PENG_DP PEngLogger::WriteLog
#define PENG_DP_TXT( s ) PEngLogger::WriteLog( D_PENG_LIT( s ) )

// DEBUG PRINTING IF FOR STATEMENTS
#define PENG_IF_DP( statement ) statement


// TOKEN PASTING HELPERS FOR DEBUG PRINTING
#define PENG_STRINGIZE(l) L#l
#define PENG_TOKEN_PASTING(s) L##s




//-----------------------------------------------------------------------------
// Empty implementations for non-debug printing builds.
//-----------------------------------------------------------------------------
#else
// EMPTY DEBUG PRINTING IF
#define PENG_IF_DP( statement )

// DUMMY PARAMETER STRUCT
struct TPEngEmptyDebugString { };


// DEBUG PRINT INDIRECTION
#define D_PENG_LIT(s) TPEngEmptyDebugString()
#define PENG_DP_TXT(s) PENG_DP( D_PENG_LIT(s) )


// EMPTY DEBUG PRINT FUNCTIONS
inline void PENG_DP( TPEngEmptyDebugString )
    {
    }

template<class T1>
inline void PENG_DP( TPEngEmptyDebugString, T1 )
    {
    }

template<class T1, class T2>
inline void PENG_DP( TPEngEmptyDebugString, T1, T2 )
    {
    }

template<class T1, class T2, class T3>
inline void PENG_DP( TPEngEmptyDebugString, T1, T2, T3 )
    {
    }

template<class T1, class T2, class T3, class T4>
inline void PENG_DP( TPEngEmptyDebugString, T1, T2, T3, T4 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5>
inline void PENG_DP( TPEngEmptyDebugString, T1, T2, T3, T4, T5 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6>
inline void PENG_DP( TPEngEmptyDebugString, T1, T2, T3, T4, T5, T6 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void PENG_DP( TPEngEmptyDebugString, T1, T2, T3, T4, T5, T6, T7 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void PENG_DP( TPEngEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8 )
    {
    }

#endif  // PENG_ENABLE_DEBUG_PRINT


#endif  // __PENGDEBUGPRINT_H__

//  End of File
