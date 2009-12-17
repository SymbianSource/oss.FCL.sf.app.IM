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
* Description:  Helper methods for touch support
*
*/


#include <aknutils.h>

#include "catouchutils.h"
#include "impsbuilddefinitions.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CATouchUtils::PenEnabled
// ---------------------------------------------------------------------------
//
TBool CATouchUtils::PenEnabled()
    {
#ifndef RD_30_DISABLE_TOUCH
    return AknLayoutUtils::PenEnabled();

#else
    // In 3.0 mode we don't have pen support
    return EFalse;

#endif //RD_30_DISABLE_TOUCH  

    }


// End of File
