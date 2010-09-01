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
* Description:  Presence contact list transactions.
*
*/

// INCLUDE FILES
#include "CPEngContactListTransaction2.h"
#include "CPEngContactListTransaction2Imp.h"
#include "TPEngMDesCArrayAdapter.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngContactListTransaction2* CPEngContactListTransaction2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngContactListTransaction2* self = CPEngContactListTransaction2::NewLC(
                                             aNWSessionSlotID,
                                             aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngContactListTransaction2* CPEngContactListTransaction2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngContactListTransaction2* self = new ( ELeave ) CPEngContactListTransaction2;
    CleanupStack::PushL( self );
    self->iImp = CPEngContactListTransaction2Imp::NewL( *self,
                                                        aPriority,
                                                        aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngContactListTransaction2::~CPEngContactListTransaction2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::CPEngContactListTransaction2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngContactListTransaction2::CPEngContactListTransaction2()
    {
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::IsBaseSynchronizeContactListsActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngContactListTransaction2::IsBaseSynchronizeContactListsActive() const
    {
    return iImp->IsBaseSynchronizeContactListsActive();
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::BaseSynchronizeContactLists()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListTransaction2::BaseSynchronizeContactLists(
    MPEngContactListTransactionObserver2& aObserver )
    {
    return iImp->BaseSynchronizeContactLists( aObserver, ETrue );
    }

EXPORT_C TInt CPEngContactListTransaction2::BaseSynchronizeContactLists(
    MPEngContactListTransactionObserver2& aObserver,
    TBool aNeedToUnsubscribe )
    {
    return iImp->BaseSynchronizeContactLists( aObserver, aNeedToUnsubscribe );
    }

// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::CancelBaseSynchronizeContactLists()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngContactListTransaction2::CancelBaseSynchronizeContactLists()
    {
    iImp->CancelBaseSynchronizeContactLists();
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::IsContactListPubSyncActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngContactListTransaction2::IsContactListPubSyncActive() const
    {
    return iImp->IsContactListPubSyncActive();
    }




// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::SynchronizeContactList()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListTransaction2::SynchronizeContactList(
    const TDesC& aContactListName,
    MPEngContactListTransactionObserver2& aObserver )

    {
    TPEngMDesCArrayAdapter adapter( aContactListName );
    return iImp->SynchronizeContactLists( adapter, aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::SynchronizeContactLists()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListTransaction2::SynchronizeContactLists(
    const MDesCArray& aContactListNames,
    MPEngContactListTransactionObserver2& aObserver )
    {
    return iImp->SynchronizeContactLists( aContactListNames, aObserver );
    }




// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::PublishContactList()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListTransaction2::PublishContactList(
    const TDesC& aContactListName,
    MPEngContactListTransactionObserver2& aObserver )

    {
    TPEngMDesCArrayAdapter adapter( aContactListName );
    return iImp->PublishContactLists( adapter, aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::PublishContactLists()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListTransaction2::PublishContactLists(
    const MDesCArray& aContactListNames,
    MPEngContactListTransactionObserver2& aObserver )
    {
    return iImp->PublishContactLists( aContactListNames, aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2::CancelContactListPubSync()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngContactListTransaction2::CancelContactListPubSync()
    {
    iImp->CancelContactListPubSync();
    }


// End of File


