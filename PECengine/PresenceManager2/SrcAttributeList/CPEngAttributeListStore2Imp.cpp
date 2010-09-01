/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Store API to retrieve attribute lists.
*
*/

// INCLUDE FILES
#include "CPEngAttributeListStore2Imp.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngSessionSlotId.h"
#include "CPEngNWSessionSlotStorageProxy.h"

#include "MPEngAttributeListManager.h"
#include "PEngListLibraryFactory.h"

#include <CPEngNWSessionSlotID2.h>



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeListStore2Imp* CPEngAttributeListStore2Imp::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeListStore2Imp* self =
        new ( ELeave ) CPEngAttributeListStore2Imp( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngAttributeListStore2Imp::~CPEngAttributeListStore2Imp()
    {
    if ( iAttrListManager )
        {
        iAttrListManager->Close();
        }

    delete iUsedSlot;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::CPEngAttributeListStore2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeListStore2Imp::CPEngAttributeListStore2Imp( TInt aPriority )
        : iCActivePriority( aPriority )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListStore2Imp::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iAttrListManager = PEngListLibraryFactory::AttributeListsManagerL(
                           iUsedSlot->BaseId() );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::CreateEmptyAttributeListL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListStore2Imp::CreateEmptyAttributeListL()
    {
    return iAttrListManager->CreateEmptyAttributeListL();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::GetDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListStore2Imp::GetDefaultAttributeListL(
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    return iAttrListManager->GetDefaultAttributeListL( aAttrListsSet );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::SetAsDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListStore2Imp::SetAsDefaultAttributeListL(
    MPEngAttributeList2& aAttributeList )
    {
    iAttrListManager->SetAsDefaultAttributeListL( aAttributeList );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::DeleteDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListStore2Imp::DeleteDefaultAttributeListL()
    {
    iAttrListManager->DeleteDefaultAttributeListL();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::GetAttributeListForContactListL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListStore2Imp::GetAttributeListForContactListL(
    const TDesC& aContactListName,
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    return iAttrListManager->GetAttributeListForContactListL( aContactListName,
                                                              aAttrListsSet );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::AttachAttributeListToContactListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListStore2Imp::AttachAttributeListToContactListL(
    const TDesC& aContactListName,
    MPEngAttributeList2& aAttributeList )
    {
    iAttrListManager->AttachAttributeListToContactListL( aContactListName,
                                                         aAttributeList );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::DeleteAttributeListFromContactListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListStore2Imp::DeleteAttributeListFromContactListL(
    const TDesC& aContactListName )
    {
    iAttrListManager->DeleteAttributeListFromContactListL( aContactListName );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::GetAttributeListForUserL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListStore2Imp::GetAttributeListForUserL(
    const TDesC& aPresenceID,
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    return iAttrListManager->GetAttributeListForUserL( aPresenceID,
                                                       aAttrListsSet );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::AttachAttributeListToUserL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListStore2Imp::AttachAttributeListToUserL(
    const TDesC& aPresenceID,
    MPEngAttributeList2& aList )
    {
    iAttrListManager->AttachAttributeListToUserL( aPresenceID,
                                                  aList );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2Imp::DeleteAttributeListFromUserL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListStore2Imp::DeleteAttributeListFromUserL(
    const TDesC& aPresenceID )
    {
    iAttrListManager->DeleteAttributeListFromUserL( aPresenceID );
    }


// End of File



