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
* Description:  Store API to retrieve contact list models.
*
*/

// INCLUDE FILES
#include "CPEngContactListStore2.h"
#include "CPEngContactListStore2Imp.h"
#include "MPEngContactListProperties.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngContactListStore2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngContactListStore2* CPEngContactListStore2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngContactListStore2* self = CPEngContactListStore2::NewLC( aNWSessionSlotID,
                                                                  aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngContactListStore2* CPEngContactListStore2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngContactListStore2* self = new ( ELeave ) CPEngContactListStore2;
    CleanupStack::PushL( self );
    self->iImp = CPEngContactListStore2Imp::NewL( aNWSessionSlotID,
                                                  aPriority );
    return self;
    }



// Destructor
CPEngContactListStore2::~CPEngContactListStore2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngContactListStore2::CPEngContactListStore2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngContactListStore2::CPEngContactListStore2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::AllContactListsL()
// -----------------------------------------------------------------------------
//
EXPORT_C const MDesCArray& CPEngContactListStore2::AllContactListsL() const
    {
    return iImp->AllContactListsL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::SynchronizedContactListsL()
// -----------------------------------------------------------------------------
//
EXPORT_C const MDesCArray& CPEngContactListStore2::SynchronizedContactListsL() const
    {
    return iImp->SynchronizedContactListsL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::ContactListL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngContactList2& CPEngContactListStore2::ContactListL( const TDesC& aContactList )
    {
    return iImp->ContactListL( aContactList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::CreateContactListL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngContactListProperties& CPEngContactListStore2::CreateContactListL(
    const TDesC& aContactList,
    TPEngContactListBaseSettings& aBaseSettings )
    {
    return iImp->CreateContactListL( aContactList, aBaseSettings );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::DeleteContactListL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngContactListStore2::DeleteContactListL( const TDesC& aContactList )
    {
    iImp->DeleteContactListL( aContactList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::ChangeContactListTypeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngContactListStore2::ChangeContactListTypeL( const TDesC& aContactList,
                                                              TPEngContactListType aContactListType )
    {
    iImp->ChangeContactListTypeL( aContactList, aContactListType );
    }

// -----------------------------------------------------------------------------
// CPEngContactListStore2::ContactListPropertiesL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngContactListProperties& CPEngContactListStore2::ContactListPropertiesL(
    const TDesC& aContactList )
    {
    return iImp->ContactListPropertiesL( aContactList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::SetAsDefaultContactListL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngContactListStore2::SetAsDefaultContactListL(
    const TDesC& aContactList )
    {
    iImp->SetAsDefaultContactListL( aContactList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::ReleaseContactList()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListStore2::ReleaseContactList( const TDesC& aContactList )
    {
    return iImp->ReleaseContactList( aContactList );
    }



// -----------------------------------------------------------------------------
// CPEngContactListStore2::ActiveContactListsL()
// Deprecated.
// -----------------------------------------------------------------------------
//
EXPORT_C const MDesCArray& CPEngContactListStore2::ActiveContactListsL() const
    {
    return iImp->DummyList();
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::NativeContactListsL()
// Deprecated.
// -----------------------------------------------------------------------------
//
EXPORT_C const MDesCArray& CPEngContactListStore2::NativeContactListsL() const
    {
    return iImp->DummyList();
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::NativeContactListsL()
// Deprecated.
// -----------------------------------------------------------------------------
//
EXPORT_C const MDesCArray& CPEngContactListStore2::UnknownServerContactListsL() const
    {
    return iImp->DummyList();
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2::ReleaseUnknownContactListFromStore()
// Deprecated.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListStore2::ReleaseUnknownContactListFromStore(
    const TDesC& /*aContactList*/ )
    {
    return KErrNotSupported;
    }


// End of File


