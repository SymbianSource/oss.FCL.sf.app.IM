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
#include "mimpssapobserver.h"
#include "TIMPSObserverTypePair.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TIMPSObserverTypePair::TIMPSObserverTypePair()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

TIMPSObserverTypePair::TIMPSObserverTypePair( MIMPSSAPObserver* aObs,
                                              TIMPSAccessGroup aGroup )
        : iObs( aObs ), iGroup( aGroup )
    {
    }

// -----------------------------------------------------------------------------
// TIMPSObserverTypePair::Type()
// -----------------------------------------------------------------------------

TIMPSAccessGroup TIMPSObserverTypePair::Group()
    {
    return iGroup;
    }

// -----------------------------------------------------------------------------
// TIMPSObserverTypePair::Observer()
// -----------------------------------------------------------------------------

MIMPSSAPObserver* TIMPSObserverTypePair::Observer()
    {
    return iObs;
    }

