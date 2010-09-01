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
* Description:  Macros to be used with IMPSSAPSettingsStore to define debug printing mode.
*
*/

#ifndef  __IMPSSAPSETTINGSSTOREDEBUGDEFINITIONMACROS_H
#define  __IMPSSAPSETTINGSSTOREDEBUGDEFINITIONMACROS_H

// MACROS

//enable debug printing in debug builds
#if defined (_DEBUG)
#define SSS_ENABLE_DEBUG_PRINT
#endif      // _DEBUG

//When needing debug output to file, define
//SSS_DEBUG_OUTPUT_TO_FILE line below
//#define SSS_DEBUG_OUTPUT_TO_FILE


#endif // __IMPSSAPSETTINGSSTOREDEBUGDEFINITIONMACROS_H
//  End of File


