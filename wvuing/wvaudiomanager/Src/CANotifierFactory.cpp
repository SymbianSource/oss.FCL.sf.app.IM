/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Publish and Subscribe factory method implementation
*
*/



// INCLUDE FILES
#include    "CANotifierFactory.h"
#include    "MCANotifierInterface.h"
#include    "CCANotifierImpl.h"


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CANotifierFactory::CreateNotifierL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C MCANotifierInterface* CANotifierFactory::CreateNotifierL( MCANotifierKeyObserver* aObserver, const TUid aUid )
    {
    return CCANotifierImpl::NewL( aObserver, aUid );
    }

//  End of File

