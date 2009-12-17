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
* Description:  Macros to be used with IMPSCUI to define debug versions.
*
*/

#ifndef  __IMPSCUIPROJECTDEFINITIONMACROS_H
#define  __IMPSCUIPROJECTDEFINITIONMACROS_H

#if defined(DEBUG)
#define IMPSCUI_ENABLE_DEBUG_PRINT
#endif

#if !defined(__WINS__) & defined(IMPSCUI_ENABLE_DEBUG_PRINT)
#define IMPSCUI_DEBUG_OUTPUT_TO_FILE
#endif

#endif /* __IMPSCUIPROJECTDEFINITIONMACROS_H */
