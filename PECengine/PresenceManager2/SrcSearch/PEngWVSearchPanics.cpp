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
* Description: this class defines some panic functions
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "PEngWVSearchPanics.h"




// -----------------------------------------------------------------------------
// SearchPanicOrLeaveL()
//
//
//
// -----------------------------------------------------------------------------
GLDEF_C void SearchPanicOrLeaveL( TPEngSearchPanics aReason, TInt aLeaveCode )
    {
    // suppress the unreferenced variable warnings
    ( void ) aReason;
    ( void ) aLeaveCode;

#if defined(_DEBUG)
    SearchPanic( aReason );
#else
    User::Leave( aLeaveCode );
#endif
    }


// -----------------------------------------------------------------------------
// SearchPanic()
//
//
//
// -----------------------------------------------------------------------------
GLDEF_C void SearchPanic( TPEngSearchPanics aReason )
    {
    User::Panic( KPEngSearchPanicCategory, aReason );
    }






//  End of File



