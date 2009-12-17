/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPSSAPSettingsStore panics
*
*/

#ifndef __IMPSSAPSETTINGSSTOREPANICS_H__
#define __IMPSSAPSETTINGSSTOREPANICS_H__

//  INCLUDES
#include <e32std.h>

// DATA TYPES
enum TIMPSSAPSettingsStorePanicReason
    {
    EIMPSSetStoreUnknownObjectVersionInternalize = 0,
    EIMPSSetStoreUnknownObjectVersionExternalize,
    EIMPSInvalidAccessGroup,
    EIMPSInvalidKeyValue
    };


// FUNCTION PROTOTYPES
/**
 * IMPSSAPSettingsStore panic handler.
 * Panics current user thread with category "IMPSSAP Store" and
 * reason number from TIMPSSAPSettingsStorePanicReason.
 * @param aReason The reason of panic.
 *
 * @since 1.2
 */
GLREF_C void Panic( TIMPSSAPSettingsStorePanicReason aReason );


_LIT( KSSSPanic, "IMPSSAP Store" );


#endif  //__IMPSSAPSETTINGSSTOREPANICS_H__

//  End of File

