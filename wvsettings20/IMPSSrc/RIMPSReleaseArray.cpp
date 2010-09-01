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
* Description:  Array utility class for cleanup management in central
				 repository transactions.
*
*/


//  INCLUDES
#include <e32base.h>
#include "RIMPSReleaseArray.h"


// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// RIMPSReleaseArray::Close()
// Destroys all items stored in array and closes it
// -----------------------------------------------------------------------------
//

void RIMPSReleaseArray::Close()
    {
    ResetAndDestroy();
    RPointerArray::Close();
    }

