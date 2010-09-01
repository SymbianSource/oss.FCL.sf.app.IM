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
* Description:  Notifier API to listen presence changes.
*
*/

// INCLUDE FILES
#include "CPEngPresenceNotifier2.h"
#include "CPEngPresenceNotifier2Imp.h"
#include "TPEngMDesCArrayAdapter.h"
#include "TPEngCntLstMDesCArrayAdapter.h"


// MACORS
#define RETURN_IF_NOT_VALID_CNT_LST_VIEW( aContactListView ) \
    if( ( aContactListView != EPEngCntListLocalView ) &&  \
        ( aContactListView != EPEngCntListNetworkView ) ) \
        {   \
        return KErrArgument; \
        }   \
     

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngPresenceNotifier2* CPEngPresenceNotifier2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngPresenceNotifier2* self = CPEngPresenceNotifier2::NewLC(
                                       aNWSessionSlotID,
                                       aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngPresenceNotifier2* CPEngPresenceNotifier2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngPresenceNotifier2* self = new ( ELeave ) CPEngPresenceNotifier2;
    CleanupStack::PushL( self );
    self->iImp = CPEngPresenceNotifier2Imp::NewL( *self,
                                                  aPriority,
                                                  aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngPresenceNotifier2::~CPEngPresenceNotifier2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::CPEngPresenceNotifier2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngPresenceNotifier2::CPEngPresenceNotifier2()
    {
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::IsActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngPresenceNotifier2::IsActive() const
    {
    return iImp->IsActive();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Start()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::Start(
    const TDesC& aPresenceID,
    const TArray<TUint32>& aTypes )
    {
    TPEngMDesCArrayAdapter adapter( aPresenceID );
    return Start( adapter, aTypes );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Start()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::Start(
    const MDesCArray& aPresenceIDs,
    const TArray<TUint32>& aTypes )
    {
    return iImp->Start( aPresenceIDs, aTypes );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Start()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::Start(
    const MPEngContactList2& aList,
    TPEngContactListView aContactListView,
    const TArray<TUint32>& aTypes )
    {
    RETURN_IF_NOT_VALID_CNT_LST_VIEW( aContactListView );

    TPEngCntLstMDesCArrayAdapter cntLstAdapter( aList, aContactListView );
    return iImp->Start( cntLstAdapter, aTypes );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Stop()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngPresenceNotifier2::Stop()
    {
    iImp->Stop();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Update()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::Update( const MDesCArray& aPresenceIDs )
    {
    return iImp->Update( aPresenceIDs, NULL, EFalse );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Update()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::Update( const MPEngContactList2& aList,
                                              TPEngContactListView aContactListView )
    {
    RETURN_IF_NOT_VALID_CNT_LST_VIEW( aContactListView );

    TPEngCntLstMDesCArrayAdapter cntLstAdapter( aList, aContactListView );
    return iImp->Update( cntLstAdapter, NULL, EFalse );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Add()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::Add(
    const TDesC& aPresenceID,
    const TArray<TUint32>& aTypes )
    {
    TPEngMDesCArrayAdapter presenceIdAdapter( aPresenceID );
    return iImp->Update( presenceIdAdapter, &aTypes, ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Remove()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::Remove(
    const TDesC& aPresenceID )

    {
    return iImp->Remove( aPresenceID );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::Remove()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::Remove(
    TUint32 aType )
    {
    return iImp->Remove( aType );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::TrackedPresenceIDs()
// -----------------------------------------------------------------------------
//
EXPORT_C const CPEngTrackedPresenceIDs2& CPEngPresenceNotifier2::
TrackedPresenceIDs() const
    {
    return iImp->TrackedPresenceIDs();
    }


EXPORT_C CPEngTrackedPresenceIDs2& CPEngPresenceNotifier2::
TrackedPresenceIDs()
    {
    return iImp->TrackedPresenceIDs();
    }




// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::AddObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::AddObserver(
    MPEngPresenceObserver2& aObserver )
    {
    return iImp->AddObserver( aObserver );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2::RemoveObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngPresenceNotifier2::RemoveObserver(
    MPEngPresenceObserver2& aObserver )
    {
    return iImp->RemoveObserver( aObserver );
    }


// End of File


