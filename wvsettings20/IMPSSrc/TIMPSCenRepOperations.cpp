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
* Description:  SAP Observer type pair container.
*
*/


//  INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include "TIMPSCenRepOperations.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TIMPSCenRepOperations::TIMPSCenRepOperations()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

TIMPSCenRepOperations::TIMPSCenRepOperations( TUint32 aUid,
                                              CRepository& aRepository )
        : iRepository( aRepository ), iMaskBase( aUid )
    {
    }




