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
* Description:  Notifier API to listen NWSessionSlot changes.
*
*/

// INCLUDE FILES
#include "CPEngNWSessionSlotNotifier2.h"
#include "CPEngNWSessionSlotNotifier2Imp.h"
#include "CPEngNWSessionSlotID2.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotNotifier2* CPEngNWSessionSlotNotifier2::NewL(
    TInt aPriority )
    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::NewL()" );

    CPEngNWSessionSlotNotifier2* self = new ( ELeave ) CPEngNWSessionSlotNotifier2;
    CleanupStack::PushL( self );
    self->iImp = CPEngNWSessionSlotNotifier2Imp::NewL( *self, aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotNotifier2* CPEngNWSessionSlotNotifier2::NewLC(
    TInt aPriority )
    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::NewLC()" );

    CPEngNWSessionSlotNotifier2* self = new ( ELeave ) CPEngNWSessionSlotNotifier2;
    CleanupStack::PushL( self );
    self->iImp = CPEngNWSessionSlotNotifier2Imp::NewL( *self, aPriority );
    return self;
    }



// Destructor
CPEngNWSessionSlotNotifier2::~CPEngNWSessionSlotNotifier2()
    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::~CPEngNWSessionSlotNotifier2()" );

    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::CPEngNWSessionSlotNotifier2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotNotifier2::CPEngNWSessionSlotNotifier2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::Start()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotNotifier2::Start()
    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::Start() - all events" );

    TRAPD( err, iImp->StartL( NULL, NULL ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::Start()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotNotifier2::Start(
    const CPEngNWSessionSlotID2& aMatchPattern )
    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::Start() - by slot ID match" );

    TRAPD( err, iImp->StartL( aMatchPattern.Implementation(), NULL ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::Start()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotNotifier2::Start(
    const TArray<TPEngNWSessionSlotEvent>& aEvents )

    {
    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlotNotifier2::Start() - by event match, %d events" ),
             aEvents.Count() );

    TRAPD( err, iImp->StartL( NULL, &aEvents ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::Stop()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotNotifier2::Stop()
    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::Stop()" );

    iImp->Stop();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::IsActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngNWSessionSlotNotifier2::IsActive() const
    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::IsActive()" );

    return iImp->IsActive();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::PushNWSessionSlotStateToObservers()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotNotifier2::PushNWSessionSlotStateToObservers(
    const CPEngNWSessionSlotID2& aMatchPattern )

    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::PushNWSessionSlotStateToObservers()" );

    return iImp->PushNWSessionSlotStateToObservers( aMatchPattern );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::GetNWSessionSlotState()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotNotifier2::GetNWSessionSlotState(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TPEngNWSessionSlotState& aState ) const
    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::GetNWSessionSlotState()" );

    return iImp->GetNWSessionSlotState( aNWSessionSlotID, aState );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::AddObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotNotifier2::AddObserver(
    MPEngNWSessionSlotObserver2& aObserver )

    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::AddObserver()" );

    return iImp->AddObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2::RemoveObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotNotifier2::RemoveObserver(
    MPEngNWSessionSlotObserver2& aObserver )

    {
    PENG_DP_TXT( "CPEngNWSessionSlotNotifier2::RemoveObserver()" );

    return iImp->RemoveObserver( aObserver );
    }


// End of File


