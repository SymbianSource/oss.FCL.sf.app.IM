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
* Description: IMPS SAP Settings Store panic handler
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "IMPSSAPSettingsStorePanics.h"

/**
* IMPSSAPSettingsStore panic handler.
* @param aReason The reason for panic.
*
* @since 3.0
*/
GLDEF_C void Panic( TIMPSSAPSettingsStorePanicReason aReason )
    {
    User::Panic( KSSSPanic, aReason );
    }


//  End of File



