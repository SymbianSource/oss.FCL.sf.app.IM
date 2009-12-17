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
#include "CPEngAttributeListStore2.h"
#include "CPEngAttributeListStore2Imp.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngAttributeListStore2* CPEngAttributeListStore2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeListStore2* self = CPEngAttributeListStore2::NewLC( aNWSessionSlotID,
                                                                      aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngAttributeListStore2* CPEngAttributeListStore2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeListStore2* self = new ( ELeave ) CPEngAttributeListStore2;
    CleanupStack::PushL( self );
    self->iImp = CPEngAttributeListStore2Imp::NewL( aNWSessionSlotID,
                                                    aPriority );
    return self;
    }



// Destructor
CPEngAttributeListStore2::~CPEngAttributeListStore2()
    {
    delete iImp;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::CPEngAttributeListStore2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeListStore2::CPEngAttributeListStore2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::CreateEmptyAttributeListL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAttributeList2* CPEngAttributeListStore2::CreateEmptyAttributeListL()
    {
    return iImp->CreateEmptyAttributeListL();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::GetDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAttributeList2* CPEngAttributeListStore2::GetDefaultAttributeListL(
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    return iImp->GetDefaultAttributeListL( aAttrListsSet );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::SetAsDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeListStore2::SetAsDefaultAttributeListL(
    MPEngAttributeList2& aAttributeList )
    {
    iImp->SetAsDefaultAttributeListL( aAttributeList );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::DeleteDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeListStore2::DeleteDefaultAttributeListL()
    {
    iImp->DeleteDefaultAttributeListL();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::GetAttributeListForContactListL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAttributeList2* CPEngAttributeListStore2::GetAttributeListForContactListL(
    const TDesC& aContactListName,
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    return iImp->GetAttributeListForContactListL( aContactListName, aAttrListsSet );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::AttachAttributeListToContactListL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeListStore2::AttachAttributeListToContactListL(
    const TDesC& aContactListName,
    MPEngAttributeList2& aAttributeList )
    {
    iImp->AttachAttributeListToContactListL( aContactListName, aAttributeList );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::DeleteAttributeListFromContactListL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeListStore2::DeleteAttributeListFromContactListL(
    const TDesC& aContactListName )
    {
    iImp->DeleteAttributeListFromContactListL( aContactListName );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::GetAttributeListForUserL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAttributeList2* CPEngAttributeListStore2::GetAttributeListForUserL(
    const TDesC& aPresenceID,
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    return iImp->GetAttributeListForUserL( aPresenceID, aAttrListsSet );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::AttachAttributeListToUserL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeListStore2::AttachAttributeListToUserL(
    const TDesC& aPresenceID,
    MPEngAttributeList2& aList )
    {
    iImp->AttachAttributeListToUserL( aPresenceID, aList );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListStore2::DeleteAttributeListFromUserL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeListStore2::DeleteAttributeListFromUserL(
    const TDesC& aPresenceID )
    {
    iImp->DeleteAttributeListFromUserL( aPresenceID );
    }



// End of File


