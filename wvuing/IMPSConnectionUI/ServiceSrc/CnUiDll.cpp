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
* Description:  Connection UI dll implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include "CnUiPanics.h"

EXPORT_C void Reserved_1() {}

/**
 * Connection UI panic handler.
 * @param aReason The reason for panic.
 *
 * @since 1.2
 */
GLDEF_C void CnUiPanic( TIMPSConnectionUiPanicReason aReason )
    {
    User::Panic( KIMPSConnUi_PanicCat, aReason );
    }


/**
 * Connection UI panic & leave handler.
 * Ugly ifdefs to suppress unused formal
 * parameter warnings.
 * @param aPanicReason The reason for panic.
 * @param aLeaveCode The reason for leave.
 * @since 1.2
 */
#if defined(_DEBUG)
GLDEF_C void CnUiPanicOrLeaveL( TIMPSConnectionUiPanicReason aReason,  TInt /*aLeaveCode*/ )
#else
GLDEF_C void CnUiPanicOrLeaveL( TIMPSConnectionUiPanicReason /*aReason*/, TInt aLeaveCode )
#endif
    {
#if defined(_DEBUG)
    //in debug builds panic with asked reason
    User::Panic( KIMPSConnUi_PanicCat, aReason );
#else
    //in release builds leave
    User::Leave( aLeaveCode );
#endif
    }


//  End of File



