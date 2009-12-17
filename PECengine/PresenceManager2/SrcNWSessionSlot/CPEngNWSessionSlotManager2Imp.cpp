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
* Description:  API implementation to manage NWSessionSlots.
*
*/

// INCLUDE FILES
#include <CPEngNWSessionSlotManager2.h>

#include "CPEngNWSessionSlotManager2Imp.h"
#include "CPEngNWSessionSlotID2.h"
#include "CPEngNWSessionSlotID2Imp.h"


#include "PEngStorageManager.h"
#include "CPEngSessionSlotState.h"
#include "CPEngSessionSlotId.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotManager2Imp* CPEngNWSessionSlotManager2Imp::NewL(
    CPEngNWSessionSlotManager2& aInterface,
    TInt aPriority )
    {
    CPEngNWSessionSlotManager2Imp* self =
        new ( ELeave ) CPEngNWSessionSlotManager2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngNWSessionSlotManager2Imp::~CPEngNWSessionSlotManager2Imp()
    {
#if _BullseyeCoverage
    cov_write();
#endif
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2Imp::CPEngNWSessionSlotManager2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotManager2Imp::CPEngNWSessionSlotManager2Imp( CPEngNWSessionSlotManager2& aInterface,
                                                              TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotManager2Imp::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2Imp::CreateNWSessionSlot()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotManager2Imp::CreateNWSessionSlot(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    const CPEngNWSessionSlotID2Imp* slotIDImp = aNWSessionSlotID.Implementation();
    if ( !slotIDImp->IsFullyDefined() )
        {
        return KErrArgument;
        }

    TRAPD( err, PEngStorageManager::CreateSessionFolderL( slotIDImp->BasePart(),
                                                          slotIDImp->AppId() ) );

    return err;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2Imp::RemoveNWSessionSlot()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotManager2Imp::RemoveNWSessionSlot(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    const CPEngNWSessionSlotID2Imp* slotIDImp = aNWSessionSlotID.Implementation();
    if ( !slotIDImp->IsFullyDefined() )
        {
        return KErrArgument;
        }

    TRAPD( err, PEngStorageManager::RemoveSessionFolderL( slotIDImp->BasePart(),
                                                          slotIDImp->AppId() ) );

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2Imp::GetNWSessionSlotsL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotManager2Imp::GetNWSessionSlotsL(
    RPointerArray<CPEngNWSessionSlotID2>& aNWSessionSlotIDs,
    const CPEngNWSessionSlotID2* aMatchPattern,
    TPEngNWSessionSlotState* aMatchState ) const
    {
    //Get list of existing slot states
    RObjectArray< CPEngSessionSlotState > allSlots;
    CleanupClosePushL( allSlots );
    PEngStorageManager::AllSessionSlotsStatesL( allSlots );


    //Generate client side slotId's from existing and matching
    //NWSessionSlots
    TInt allSlotsCount = allSlots.Count();
    for ( TInt ix = 0; ix < allSlotsCount; ix++ )
        {
        const CPEngSessionSlotState* handledSlot = allSlots[ ix ];
        CPEngSessionSlotId& ssBaseId = handledSlot->SessionSlotId();

        const MDesC16Array& regApps = handledSlot->RegisteredApplications();
        const MDesC16Array& activeApps = handledSlot->ActiveApplications();
        DoGenerateMatchingSlotIdsL( aNWSessionSlotIDs,
                                    ssBaseId,
                                    regApps,
                                    activeApps,
                                    aMatchPattern,
                                    aMatchState );
        }

    CleanupStack::PopAndDestroy(); //slotStates
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2Imp::ExistsNWSessionSlotL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotManager2Imp::ExistsNWSessionSlotL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID ) const
    {
    const CPEngNWSessionSlotID2Imp* slotIDImp = aNWSessionSlotID.Implementation();
    if ( !slotIDImp->IsFullyDefined() )
        {
        User::Leave( KErrArgument );
        }

    //Try get the wanted slot
    const CPEngSessionSlotId& wantedBaseId = aNWSessionSlotID.Implementation()->BasePart();
    CPEngSessionSlotState* wantedSlotState = CPEngSessionSlotState::NewLC();
    PEngStorageManager::SessionSlotStateL( wantedBaseId, *wantedSlotState );


    //Check if base & AppId's match to given match pattern
    TInt  matchStatus = KErrNotFound;
    if ( wantedSlotState->SessionSlotId().Match( wantedBaseId ) == KErrNone )
        {
        const MDesC16Array& apps = wantedSlotState->RegisteredApplications();

        TInt appsCount = apps.MdcaCount();
        for ( TInt ii = 0; ii < appsCount; ii++ )
            {
            TPtrC16 appId( apps.MdcaPoint( ii ) );
            matchStatus = aNWSessionSlotID.Implementation()->MatchAppIdPart( appId );
            if ( matchStatus == KErrNone )
                {
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( wantedSlotState );
    User::LeaveIfError( matchStatus );
    }




// -----------------------------------------------------------------------------
// IsIdMatch()
// -----------------------------------------------------------------------------
//
TBool IsIdMatch( const CPEngNWSessionSlotID2* aSlotIdMatcher,
                 const CPEngNWSessionSlotID2& aSlotId )
    {
    TBool idMatch = ETrue;

    if ( aSlotIdMatcher )
        {
        idMatch = ( aSlotId.MatchFullId( *aSlotIdMatcher ) == KErrNone );
        }

    return idMatch;
    }


// -----------------------------------------------------------------------------
// HasApp()
// -----------------------------------------------------------------------------
//
TBool HasApp( const MDesC16Array& aActiveApps, const TDesC& aAppId )
    {
    const TInt activeAppCount = aActiveApps.MdcaCount();
    for ( TInt ii = 0; ii < activeAppCount; ii++ )
        {
        TPtrC app = aActiveApps.MdcaPoint( ii );
        if ( app == aAppId )
            {
            return ETrue;
            }
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// IsStateMatch()
// -----------------------------------------------------------------------------
//
TBool IsStateMatch( const TPEngNWSessionSlotState* aSlotStateMatcher,
                    const MDesC16Array& aActiveApps,
                    const CPEngNWSessionSlotID2& aSlotId )
    {
    TBool stateMatch = ETrue;

    if ( aSlotStateMatcher )
        {
        if ( *aSlotStateMatcher == EPEngNWPresenceSessionClosed )
            {
            //only closed apps wanted -> app id may not exist in aActiveApps
            stateMatch = !HasApp( aActiveApps, aSlotId.AppId() );
            }

        if ( *aSlotStateMatcher == EPEngNWPresenceSessionOpen )
            {
            //only open apps wanted -> app id must be found from aActiveApps
            stateMatch = HasApp( aActiveApps, aSlotId.AppId() );
            }
        }

    return stateMatch;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotManager2Imp::DoGenerateMatchingSlotIdsL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotManager2Imp::DoGenerateMatchingSlotIdsL(
    RPointerArray<CPEngNWSessionSlotID2>& aNWSessionSlotIDs,
    CPEngSessionSlotId& aSessionSlotBaseId,
    const MDesC16Array& aRegApps,
    const MDesC16Array& aActiveApps,
    const CPEngNWSessionSlotID2* aSlotIdMatcher,
    TPEngNWSessionSlotState* aSlotStateMatcher ) const
    {
    const TInt regAppCount = aRegApps.MdcaCount();

    for ( TInt ii = 0; ii < regAppCount; ii++ )
        {
        CPEngNWSessionSlotID2* slotId = CPEngNWSessionSlotID2::NewLC();
        CPEngNWSessionSlotID2Imp* slotIdImp = slotId->Implementation();

        slotIdImp->SetAllL( aSessionSlotBaseId,
                            aRegApps.MdcaPoint( ii ) );

        TBool idMatch = IsIdMatch( aSlotIdMatcher, *slotId );
        TBool stateMatch = IsStateMatch( aSlotStateMatcher, aActiveApps, *slotId );

        if ( idMatch && stateMatch )
            {
            User::LeaveIfError( aNWSessionSlotIDs.Append( slotId ) );
            CleanupStack::Pop( slotId );
            }
        else
            {
            CleanupStack::PopAndDestroy( slotId );
            }
        }
    }



// End of File




