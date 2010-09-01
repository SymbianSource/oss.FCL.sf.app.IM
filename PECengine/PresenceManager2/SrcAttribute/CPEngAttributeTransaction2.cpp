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
* Description:  Presence attribute transactions.
*
*/

// INCLUDE FILES
#include "CPEngAttributeTransaction2.h"
#include "CPEngAttributeTransaction2Imp.h"
#include "TPEngMDesCArrayAdapter.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngAttributeTransaction2* CPEngAttributeTransaction2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeTransaction2* self = CPEngAttributeTransaction2::NewLC(
                                           aNWSessionSlotID,
                                           aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngAttributeTransaction2* CPEngAttributeTransaction2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeTransaction2* self = new ( ELeave ) CPEngAttributeTransaction2;
    CleanupStack::PushL( self );
    self->iImp = CPEngAttributeTransaction2Imp::NewL( *self,
                                                      aPriority,
                                                      aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngAttributeTransaction2::~CPEngAttributeTransaction2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::CPEngAttributeTransaction2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeTransaction2::CPEngAttributeTransaction2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::IsPublishOwnAttributesActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngAttributeTransaction2::IsPublishOwnAttributesActive() const
    {
    return iImp->IsPublishOwnAttributesActive();
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::PublishAndUnLockOwnAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeTransaction2::PublishAndUnLockOwnAttribute(
    MPEngPresenceAttrModel2*& aModel,
    MPEngAttributeTransactionObserver2& aObserver )
    {
    return iImp->PublishAndUnLockOwnAttribute( aModel,
                                               aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::PublishAndUnLockOwnAttributes()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeTransaction2::PublishAndUnLockOwnAttributes(
    RPointerArray<MPEngPresenceAttrModel2>& aModels,
    MPEngAttributeTransactionObserver2& aObserver )
    {
    return iImp->PublishAndUnLockOwnAttributes( aModels,
                                                aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::CancelPublishOwnAttributes()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeTransaction2::CancelPublishOwnAttributes()
    {
    iImp->CancelPublishOwnAttributes();
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::IsFetchAttributesActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngAttributeTransaction2::IsFetchAttributesActive() const
    {
    return iImp->IsFetchAttributesActive();
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::FetchAttributesToCache()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeTransaction2::FetchAttributesToCache(
    const TDesC& aPresenceID,
    const TArray<TUint32>& aTypes,
    MPEngAttributeTransactionObserver2& aObserver,
    TInt aOptions )
    {
    TPEngMDesCArrayAdapter adapter( aPresenceID );
    return iImp->FetchAttributes( adapter,
                                  aTypes,
                                  aObserver,
                                  aOptions,
                                  EFalse );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::FetchAttributesToCache()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeTransaction2::FetchAttributesToCache(
    const MDesCArray& aPresenceIDs,
    const TArray<TUint32>& aTypes,
    MPEngAttributeTransactionObserver2& aObserver,
    TInt aOptions )
    {
    return iImp->FetchAttributes( aPresenceIDs,
                                  aTypes,
                                  aObserver,
                                  aOptions,
                                  EFalse );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::FetchAttributesToObjects()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeTransaction2::FetchAttributesToObjects(
    const TDesC& aPresenceID,
    const TArray<TUint32>& aTypes,
    MPEngAttributeTransactionObserver2& aObserver )
    {
    TPEngMDesCArrayAdapter adapter( aPresenceID );
    return iImp->FetchAttributes( adapter,
                                  aTypes,
                                  aObserver,
                                  KErrNone,  //No options
                                  ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::CancelFetchAttributes()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeTransaction2::CancelFetchAttributes()
    {
    iImp->CancelFetchAttributes();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2::GetFetchedAttributes()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeTransaction2::GetFetchedAttributes(
    RPointerArray< MPEngPresenceAttrModel2>& aModels )
    {
    return iImp->GetFetchedAttributes( aModels );
    }



// End of File


