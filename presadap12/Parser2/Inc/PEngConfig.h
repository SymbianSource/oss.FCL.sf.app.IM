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
* Description:  Compiletime definitions for Presence Engine.
*
*/

#ifndef  __PENGCONFIG_H
#define  __PENGCONFIG_H


//Other macros
#undef PENG_ENABLE_DEBUG_PRINT

#if defined(DEBUG)
#define PENG_ENABLE_DEBUG_PRINT
#endif
#if defined(_DEBUG)
#define PENG_ENABLE_DEBUG_PRINT
#endif

#if defined(__WINS__)
#undef PENG_DEBUG_OUTPUT_TO_FILE
#endif

#if defined(__THUMB__) && defined(PENG_ENABLE_DEBUG_PRINT)
#define PENG_DEBUG_OUTPUT_TO_FILE
#endif

#endif // __PENGCONFIG_H
