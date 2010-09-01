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
* Description:  Notifier API to listen presence contact list changes.
*
*/

// INCLUDE FILES
#include "CPEngContactListNotifier2.h"
#include "CPEngContactListNotifier2Imp.h"
#include "TPEngMDesCArrayAdapter.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngContactListNotifier2* CPEngContactListNotifier2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngContactListNotifier2* self = CPEngContactListNotifier2::NewLC(
                                          aNWSessionSlotID,
                                          aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngContactListNotifier2* CPEngContactListNotifier2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngContactListNotifier2* self = new ( ELeave ) CPEngContactListNotifier2;
    CleanupStack::PushL( self );
    self->iImp = CPEngContactListNotifier2Imp::NewL( *self,
                                                     aPriority,
                                                     aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngContactListNotifier2::~CPEngContactListNotifier2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::CPEngContactListNotifier2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngContactListNotifier2::CPEngContactListNotifier2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::Start()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListNotifier2::Start( const TDesC& aContactListName )
    {
    TPEngMDesCArrayAdapter adapter( aContactListName );
    return iImp->Start( adapter );
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::Start()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListNotifier2::Start(
    const MDesCArray& aContactListNames )
    {
    return iImp->Start( aContactListNames );
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::Stop()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngContactListNotifier2::Stop()
    {
    iImp->Stop();
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::IsActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngContactListNotifier2::IsActive() const
    {
    return iImp->IsActive();
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::Add()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListNotifier2::Add( const TDesC& aContactListName )

    {
    return iImp->Add( aContactListName );
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::Remove()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListNotifier2::Remove( const TDesC& aContactListName )

    {
    return iImp->Remove( aContactListName );
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::AddObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListNotifier2::AddObserver(
    MPEngContactListObserver2& aObserver )

    {
    return iImp->AddObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2::RemoveObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngContactListNotifier2::RemoveObserver(
    MPEngContactListObserver2& aObserver )

    {
    return iImp->RemoveObserver( aObserver );
    }


// End of File


