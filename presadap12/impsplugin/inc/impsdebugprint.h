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
* Description:  imps protocol adaptation .
*
*/

#ifndef __IMPSDEBUGPRINT_H__
#define __IMPSDEBUGPRINT_H__

//  INCLUDES
#include    "impsconfig.h"


// -----------------------------------------------------------------------------
// Debug logging is enabled
// -----------------------------------------------------------------------------
#ifdef IMPS_ENABLE_DEBUG_PRINT

// INCLUDES
#include    "impslogger.h"

// CONSTANTS
_LIT( KIMPSDebugOutputDir, "IMPS" );
_LIT( KIMPSDebugOutputFileName, "IMPS2.log" );
const TInt KIMPSMaxLogLineLength = 250;


// TARGET WARNING
#ifndef _DEBUG
#if defined(__VC32__)
#pragma message( "Warning: IMPS Protocol Adapt debug printing _ON_" )	// CSI: 68 #
#else // __GCC32__
#warning "IMPS Protocol Adapt printing _ON_"
#endif // __VC32__
#endif


// DEBUG PRINT INDIRECTION
#define D_IMPS_LIT(s) _L(s)		// CSI: 78 #
#define IMPS_DP IMPSLogger::WriteLog
#define IMPS_DP_TXT( s ) IMPSLogger::WriteLog( D_IMPS_LIT( s ) )

// DEBUG PRINTING IF FOR STATEMENTS
#define IMPS_IF_DP( statement ) statement


// TOKEN PASTING HELPERS FOR DEBUG PRINTING
#define IMPS_STRINGIZE(l) L#l
#define IMPS_TOKEN_PASTING(s) L##s




//-----------------------------------------------------------------------------
// Empty implementations for non-debug printing builds.
//-----------------------------------------------------------------------------
#else
// EMPTY DEBUG PRINTING IF
#define IMPS_IF_DP( statement )

// DUMMY PARAMETER STRUCT
struct TIMPSEmptyDebugString { };


// DEBUG PRINT INDIRECTION
#define D_IMPS_LIT(s) TIMPSEmptyDebugString()
#define IMPS_DP_TXT(s) IMPS_DP( D_IMPS_LIT(s) )


// EMPTY DEBUG PRINT FUNCTIONS
inline void IMPS_DP( TIMPSEmptyDebugString )
    {
    }

template<class T1>
inline void IMPS_DP( TIMPSEmptyDebugString, T1 )
    {
    }

template<class T1, class T2>
inline void IMPS_DP( TIMPSEmptyDebugString, T1, T2 )
    {
    }

template<class T1, class T2, class T3>
inline void IMPS_DP( TIMPSEmptyDebugString, T1, T2, T3 )
    {
    }

template<class T1, class T2, class T3, class T4>
inline void IMPS_DP( TIMPSEmptyDebugString, T1, T2, T3, T4 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5>
inline void IMPS_DP( TIMPSEmptyDebugString, T1, T2, T3, T4, T5 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6>
inline void IMPS_DP( TIMPSEmptyDebugString, T1, T2, T3, T4, T5, T6 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void IMPS_DP( TIMPSEmptyDebugString, T1, T2, T3, T4, T5, T6, T7 )
    {
    }

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void IMPS_DP( TIMPSEmptyDebugString, T1, T2, T3, T4, T5, T6, T7, T8 )
    {
    }

#endif  // IMPS_ENABLE_DEBUG_PRINT


#endif  // __IMPSDEBUGPRINT_H__

//  End of File
