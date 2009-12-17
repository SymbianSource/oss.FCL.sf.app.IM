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
* Description:  API to manage NWSessionSlots.
*
*/

// INCLUDE FILES
#include "CPEngNWSessionSlotManager2.h"
#include "CPEngNWSessionSlotManager2Imp.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotManager2* CPEngNWSessionSlotManager2::NewL( TInt aPriority )
    {
    PENG_DP_TXT( "CPEngNWSessionSlotManager2::NewL()" );

    CPEngNWSessionSlotManager2* self = new ( ELeave ) CPEngNWSessionSlotManager2;
    CleanupStack::PushL( self );
    self->iImp = CPEngNWSessionSlotManager2Imp::NewL( *self, aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotManager2* CPEngNWSessionSlotManager2::NewLC( TInt aPriority )
    {
    PENG_DP_TXT( "CPEngNWSessionSlotManager2::NewLC()" );

    CPEngNWSessionSlotManager2* self = new ( ELeave ) CPEngNWSessionSlotManager2;
    CleanupStack::PushL( self );
    self->iImp = CPEngNWSessionSlotManager2Imp::NewL( *self, aPriority );
    return self;
    }



// Destructor
CPEngNWSessionSlotManager2::~CPEngNWSessionSlotManager2()
    {
    PENG_DP_TXT( "CPEngNWSessionSlotManager2::~CPEngNWSessionSlotManager2()" );

    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2::CPEngNWSessionSlotManager2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotManager2::CPEngNWSessionSlotManager2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2::CreateNWSessionSlot()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotManager2::CreateNWSessionSlot(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    PENG_DP_TXT( "CPEngNWSessionSlotManager2::CreateNWSessionSlot()" );

    return iImp->CreateNWSessionSlot( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2::RemoveNWSessionSlot()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotManager2::RemoveNWSessionSlot(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    PENG_DP_TXT( "CPEngNWSessionSlotManager2::RemoveNWSessionSlot()" );

    return iImp->RemoveNWSessionSlot( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2::GetNWSessionSlots()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotManager2::GetNWSessionSlots(
    RPointerArray<CPEngNWSessionSlotID2>& aNWSessionSlotIDs ) const
    {
    PENG_DP_TXT( "CPEngNWSessionSlotManager2::GetNWSessionSlots() - requesting all" );

    TRAPD( err, iImp->GetNWSessionSlotsL( aNWSessionSlotIDs, NULL, NULL ) )

    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlotManager2::GetNWSessionSlots() - Found: %d, Status: %d" ),
             aNWSessionSlotIDs.Count(), err );

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2::GetNWSessionSlots()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotManager2::GetNWSessionSlots(
    RPointerArray< CPEngNWSessionSlotID2 >& aNWSessionSlotIDs,
    const CPEngNWSessionSlotID2& aMatchPattern,
    TPEngNWSessionSlotState aMatchState ) const
    {
    /*
    PENG_IF_DP(
        {
        TDebugSlotIdBuf slotId;
        TDebugSlotStateBuf slotState;
        DebugObjectDump::ToBuf( slotId, aMatchPattern );
        DebugObjectDump::ToBuf( slotState, aMatchState );
        PENG_DP( D_PENG_LIT( "CPEngNWSessionSlotManager2::GetNWSessionSlots() - Filters: [%S] [%S]" ),
                              &slotId, &slotState );
        });
    */
    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlotManager2::GetNWSessionSlots() - By filter: %d" ),
             aMatchState );


    TRAPD( err, iImp->GetNWSessionSlotsL( aNWSessionSlotIDs, &aMatchPattern, &aMatchState ) );


    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlotManager2::GetNWSessionSlots() - Found: %d, Status: %d" ),
             aNWSessionSlotIDs.Count(), err );

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2::ExistsNWSessionSlot()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotManager2::ExistsNWSessionSlot(
    const CPEngNWSessionSlotID2& aNWSessionSlotID ) const
    {
    PENG_DP_TXT( "CPEngNWSessionSlotManager2::ExistsNWSessionSlot()" );

    TRAPD( err, iImp->ExistsNWSessionSlotL( aNWSessionSlotID ) );

    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlotManager2::ExistsNWSessionSlot() - Status: %d" ),
             err );

    return err;
    }



// End of File


