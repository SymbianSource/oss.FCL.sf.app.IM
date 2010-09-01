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
* Description:  Compiletime definitions for imps protocol adaptations,
*
*/

#ifndef  __IMPSCONFIG_H__
#define  __IMPSCONFIG_H__


#define IMPSPRTPLUGIN_UNUSED_PARAM(p) (void) p



//#define IMPS_ENABLE_DEBUG_PRINT
//#define IMPS_DEBUG_OUTPUT_TO_FILE



//Other macros
//#undef IMPS_ENABLE_DEBUG_PRINT

#if defined(DEBUG)
#define IMPS_ENABLE_DEBUG_PRINT
#endif
#if defined(_DEBUG)
#define IMPS_ENABLE_DEBUG_PRINT
#endif

#if defined(__WINSCW__) && defined(IMPS_ENABLE_DEBUG_PRINT)
#define IMPS_DEBUG_OUTPUT_TO_FILE
#endif

#if defined(__ARMV5__) && defined(IMPS_ENABLE_DEBUG_PRINT)
#define IMPS_DEBUG_OUTPUT_TO_FILE
#endif


#endif // __IMPSCONFIG_H__
