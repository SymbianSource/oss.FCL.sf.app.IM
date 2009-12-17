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
* Description:  List Library dll interface
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "PEngListLibraryFactory.h"
#include "CPEngListLibFactory.h"
#include "CPEngAttributeListManager.h"
#include "CPEngContactListManager.h"
#include "CPEngContactListTransactionManager.h"
#include "CPEngAttributeListTransactionManager.h"

#include "CPEngAuthorizationManager.h"
#include "CPEngAuthorizationTransFactory.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngListLibraryFactory::AttributeListsManagerL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAttributeListManager* PEngListLibraryFactory::AttributeListsManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngListLibFactory* f = CPEngListLibFactory::InstanceLC();
    MPEngAttributeListManager* m = f->AttrListManagerL( aSessionSlot );
    CleanupStack::PopAndDestroy( f );
    return m;
    }


// -----------------------------------------------------------------------------
// PEngListLibraryFactory::ContactListManagerL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngContactListManager* PEngListLibraryFactory::ContactListManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngListLibFactory* f = CPEngListLibFactory::InstanceLC();
    MPEngContactListManager* m = f->CntListManagerL( aSessionSlot );
    CleanupStack::PopAndDestroy( f );
    return m;
    }


// -----------------------------------------------------------------------------
// PEngListLibraryFactory::AttributeListsTransactionManagerL
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngTransactionFactory* PEngListLibraryFactory::AttributeListsTransactionManagerLC(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngListLibFactory* f = CPEngListLibFactory::InstanceLC();
    CPEngAttributeListTransactionManager* m = f->AttrListTransManagerL( aSessionSlot );
    CleanupStack::PopAndDestroy( f );
    CleanupClosePushL( *m );
    return m;
    }


// -----------------------------------------------------------------------------
// PEngListLibraryFactory::ContactListTransactionManagerL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngTransactionFactory* PEngListLibraryFactory::ContactListTransactionManagerLC(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngListLibFactory* f = CPEngListLibFactory::InstanceLC();
    CPEngContactListTransactionManager* m = f->CntListTransManagerL( aSessionSlot );
    CleanupStack::PopAndDestroy( f );
    CleanupClosePushL( *m );
    return m;
    }


// -----------------------------------------------------------------------------
// PEngListLibraryFactory::ContactListTransactionManagerL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAuthorizationManager* PEngListLibraryFactory::AuthorizationManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngListLibFactory* f = CPEngListLibFactory::InstanceLC();
    CPEngAuthorizationManager* m = f->AuthorizationManagerL( aSessionSlot );
    CleanupStack::PopAndDestroy( f );
    return m;
    }


// -----------------------------------------------------------------------------
// PEngListLibraryFactory::ContactListTransactionManagerL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngTransactionFactory* PEngListLibraryFactory::AuthorizationTransactionManagerLC(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngListLibFactory* f = CPEngListLibFactory::InstanceLC();

    CPEngAuthorizationTransFactory* m = f->AuthorizationTransManagerL( aSessionSlot );
    CleanupStack::PopAndDestroy();
    CleanupClosePushL( *m );
    return m;
    }

//  End of File



