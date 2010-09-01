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
* Description:  NWSessionSlot notifier API implementation
*
*/

// INCLUDE FILES
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngNWSessionSlotNotifier2Imp.h"
#include "CPEngNWSessionSlotEventEntry.h"
#include "GenObserverNotifyMediators.h"

#include "PEngStorageManager.h"
#include "MPEngStorageManagerWatcher.h"
#include "CPEngSessionSlotEvent.h"
#include "CPEngSessionSlotState.h"
#include "PresenceDebugPrint.h"




#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlotNotifier2.h>
#include <CPEngNWSessionSlotEvent2.h>
#include <MPEngNWSessionSlotObserver2.h>



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotNotifier2Imp* CPEngNWSessionSlotNotifier2Imp::NewL(
    CPEngNWSessionSlotNotifier2& aInterface,
    TInt aPriority )
    {
    CPEngNWSessionSlotNotifier2Imp* self =
        new ( ELeave ) CPEngNWSessionSlotNotifier2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngNWSessionSlotNotifier2Imp::~CPEngNWSessionSlotNotifier2Imp()
    {
    iDying = ETrue;
    Stop();

    DeleteFilters();
    iObsArray.Close();
    delete iEventEntry;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::CPEngNWSessionSlotNotifier2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotNotifier2Imp::CPEngNWSessionSlotNotifier2Imp( CPEngNWSessionSlotNotifier2& aInterface,
                                                                TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotNotifier2Imp::ConstructL()
    {
    iEventEntry = CPEngNWSessionSlotEvent2Copy::NewL();
    }




// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::StartL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotNotifier2Imp::StartL(
    const CPEngNWSessionSlotID2Imp* aSlotIdFilter,
    const TArray<TPEngNWSessionSlotEvent>* aEventFilter )
    {
    //Pre checks
    if ( iStarted )
        {
        User::Leave( KErrInUse );
        }


    //Clear possibly existing filters
    DeleteFilters();

    //And initialize new ones
    if ( aSlotIdFilter )
        {
        iSlotIdFilter = aSlotIdFilter->CloneL();
        }

    if ( aEventFilter )
        {
        const TInt eventsCount = aEventFilter->Count();
        if ( eventsCount == 0 )
            {
            User::Leave( KErrArgument );
            }

        iEventFilter = new ( ELeave ) RArray<TPEngNWSessionSlotEvent>;
        for ( TInt ii = 0; ii < eventsCount; ii++ )
            {
            iEventFilter->InsertInSignedKeyOrderL( ( *aEventFilter )[ ii ] );
            }
        }

    //Register for events
    PEngStorageManager::RegisterGlobalEventObserverL( *this );
    iStarted = ETrue;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::Stop()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotNotifier2Imp::Stop()
    {
    if ( !iStarted )
        {
        return;
        }

    PEngStorageManager::UnregisterGlobalObserver( *this );
    iStarted = EFalse;

    DeleteFilters();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::IsActive()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlotNotifier2Imp::IsActive() const
    {
    return iStarted;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::PushNWSessionSlotStateToObservers()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotNotifier2Imp::PushNWSessionSlotStateToObservers(
    const CPEngNWSessionSlotID2& aMatchPattern )

    {
    if ( iSynchNotifyRunning || iObsArray.NotifyLoopRunning() )
        {
        return KErrInUse;
        }

    iSynchNotifyRunning = ETrue;
    TRAPD( err, DoNotifyNWSessionSlotStatesL( *aMatchPattern.Implementation() ) );
    iSynchNotifyRunning = EFalse;

    return err;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::GetNWSessionSlotState()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotNotifier2Imp::GetNWSessionSlotState(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TPEngNWSessionSlotState& aState ) const
    {
    TRAPD( err,
        {
        CPEngSessionSlotState* state = CPEngSessionSlotState::NewLC();
        PEngStorageManager::SessionSlotStateL( aNWSessionSlotID.Implementation()->BasePart(),
                                               *state );

        aState = state->AppState( aNWSessionSlotID.AppId() );
        CleanupStack::PopAndDestroy( state );
        } );


    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::AddObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotNotifier2Imp::AddObserver(
    MPEngNWSessionSlotObserver2& aObserver )

    {
    //Observer array takes care from possible NULL pointers
    return iObsArray.AddObserver( &aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::RemoveObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotNotifier2Imp::RemoveObserver(
    MPEngNWSessionSlotObserver2& aObserver )

    {
    return iObsArray.RemoveObserver( &aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::HandleNewSessionSlotEventL()
// From MPEngGlobalEventObserver
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotNotifier2Imp::HandleNewSessionSlotEventL(
    const CPEngSessionSlotEvent& aSessionEvent )
    {
    if ( iDying )
        {
        return;
        }

    iEventEntry->Implementation()->SetFullEventReferenceL( aSessionEvent );
    TBool eventAccepted = IsEventAccepted( iEventEntry->NWSessionSlotID(),
                                           iEventEntry->Event() );

    if ( eventAccepted )
        {
        MediateNotify( KErrNone, *iEventEntry );
        }

    iEventEntry->Implementation()->ResetReferences();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::HandleObserverError()
// From MPEngGlobalEventObserver
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotNotifier2Imp::HandleObserverError( TInt aError )
    {
    if ( iDying )
        {
        return;
        }

    iEventEntry->Implementation()->ResetReferences();
    MediateNotify( aError, *iEventEntry );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::DoNotifyNWSessionSlotStatesL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotNotifier2Imp::DoNotifyNWSessionSlotStatesL(
    const CPEngNWSessionSlotID2Imp& aSlotIdMatcher )
    {
    //this might be called during the event notify loop
    //==> use here temp objects so the original data isn't messed up

    RObjectArray< CPEngSessionSlotState > ssStates;
    CleanupClosePushL( ssStates );

    CPEngNWSessionSlotEvent2Copy* eventEntry = CPEngNWSessionSlotEvent2Copy::NewL();
    CleanupStack::PushL( eventEntry );

    //Get list of existing slot states
    PEngStorageManager::AllSessionSlotsStatesL( ssStates );

    TInt matchCount = 0;

    //Generate client side events from requested NWSessionSlots
    const TInt ssStatesCount = ssStates.Count();
    for ( TInt ii = 0; ii < ssStatesCount; ii++ )
        {
        const CPEngSessionSlotState& ssState = *ssStates[ ii ];
        CPEngSessionSlotId& ssBaseId = ssState.SessionSlotId();

        TBool baseMatch = ( aSlotIdMatcher.MatchBasePart( ssBaseId ) == KErrNone );
        if ( baseMatch )
            {
            const MDesC16Array& ssApplications = ssState.RegisteredApplications();
            const TInt appCount = ssApplications.MdcaCount();
            for ( TInt jj = 0; jj < appCount; jj++ )
                {
                TPtrC appId = ssApplications.MdcaPoint( jj );
                TBool appMatch = ( aSlotIdMatcher.MatchAppIdPart( appId ) == KErrNone );
                if ( appMatch )
                    {
                    matchCount++;
                    eventEntry->Implementation()->SetPseudoEventReferenceL(
                        ssBaseId,
                        appId,
                        ssState.AppState( appId ),
                        ssState.SessionSlotState() );
                    MediateNotify( KErrNone, *eventEntry );
                    eventEntry->Implementation()->ResetReferences();
                    }
                }
            }
        }

    if ( matchCount == 0 )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( 2 ); //ssStates, eventEntry
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::DeleteFilters()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotNotifier2Imp::DeleteFilters()
    {
    delete iSlotIdFilter;
    iSlotIdFilter = NULL;

    if ( iEventFilter )
        {
        iEventFilter->Reset();
        }

    delete iEventFilter;
    iEventFilter = NULL;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::IsEventAccepted()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlotNotifier2Imp::IsEventAccepted(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TPEngNWSessionSlotEvent aEvent ) const
    {
    TBool accepted = ETrue;

    if ( iSlotIdFilter )
        {
        if ( iSlotIdFilter->MatchFullId( *aNWSessionSlotID.Implementation() ) == KErrNotFound )
            {
            accepted = EFalse;
            }
        }


    if ( iEventFilter && accepted )
        {
        if ( iEventFilter->Find( aEvent ) == KErrNotFound )
            {
            accepted = EFalse;
            }
        }


    return accepted;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotNotifier2Imp::MediateNotify()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotNotifier2Imp::MediateNotify( TInt aError,
                                                    CPEngNWSessionSlotEvent2& aEvent )
    {
    TGenNotifyMediator2 < MPEngNWSessionSlotObserver2,
    CPEngNWSessionSlotNotifier2&,
    CPEngNWSessionSlotEvent2& >
    eventMediator( &MPEngNWSessionSlotObserver2::HandleNWSessionSlotChangeL,
                   &MPEngNWSessionSlotObserver2::HandleNWSessionSlotError,
                   iInterface,
                   aEvent );

    if ( aError == KErrNone )
        {
        PENG_DP( D_PENG_LIT( "CPEngNWSessionSlotNotifier2 - Handle event: %d" ),
                 aEvent.Event() );

        iObsArray.NotifyObservers( eventMediator );
        }
    else
        {
        PENG_DP( D_PENG_LIT( "CPEngNWSessionSlotNotifier2 - Handle event error: %d" ),
                 aError );

        iObsArray.NotifyErrorObservers( eventMediator, aError );
        }
    }




// End of File


