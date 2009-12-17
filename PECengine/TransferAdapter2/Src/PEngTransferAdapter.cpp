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
* Description:  Get access handler and pureda data handler
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "PEngTransferAdapter.h"
#include    "CPEngTransAdapFactory.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngTransferAdapter::AccessHandler()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAccessHandler* PEngTransferAdapter::AccessHandlerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngTransAdapFactory* factory = CPEngTransAdapFactory::InstanceLC();
    MPEngAccessHandler* handler = factory->AccessHandlerL( aSessionSlot );
    CleanupStack::PopAndDestroy(); // factory
    return handler;
    }


// -----------------------------------------------------------------------------
// PEngTransferAdapter::PureDataHandler()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngPureDataHandler* PEngTransferAdapter::PureDataHandlerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngTransAdapFactory* factory = CPEngTransAdapFactory::InstanceLC();
    MPEngPureDataHandler* handler = factory->PureDataHandlerL( aSessionSlot );
    CleanupStack::PopAndDestroy(); // factory
    return handler;
    }

//  End of File
