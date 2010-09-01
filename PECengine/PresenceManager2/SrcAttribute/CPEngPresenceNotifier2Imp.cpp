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
* Description:  Notifier implementation to listen presence changes.
*
*/

// INCLUDE FILES
#include "CPEngPresenceNotifier2Imp.h"
#include "CPEngTrackedPresenceIDCollection.h"
#include "CPEngTrackedPresenceIDEntry.h"
#include "CPEngNWSessionSlotID2.h"

#include "TPEngMDesCArrayAdapter.h"
#include "TPEngGenArrayAdapter.h"
#include "GenObserverNotifyMediators.h"

#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngPresenceAttrManager.h"
#include "PEngAttrLibFactory.h"

#include "MPEngStorageManagerWatcher.h"
#include "PEngStorageManager.h"

#include <CPEngPresenceNotifier2.h>
#include <CPEngTrackedPresenceIDs2.h>
#include <MPEngPresenceObserver2.h>
#include <MPEngPresenceAttrModel2.h>
#include "PresenceDebugPrint.h"
#include "PEngWVPresenceAttributes2.h"




// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngPresenceNotifier2Imp* CPEngPresenceNotifier2Imp::NewL(
    CPEngPresenceNotifier2& aInterface,
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngPresenceNotifier2Imp* self =
        new ( ELeave ) CPEngPresenceNotifier2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngPresenceNotifier2Imp::~CPEngPresenceNotifier2Imp()
    {
    iDying = ETrue;

    Stop();


    if ( iWatcher )
        {
        iWatcher->UnregisterListenSIDsObserver( *this );
        iWatcher->Close();
        }

    if ( iAttrManager )
        {
        iAttrManager->Close();
        }

    iObsArray.Close();
    delete iUsedSlot;
    delete iTrackedPresenceIDs;
    iTrackedAttributes.Reset();
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::CPEngPresenceNotifier2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngPresenceNotifier2Imp::CPEngPresenceNotifier2Imp(
    CPEngPresenceNotifier2& aInterface,
    TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iWatcher = PEngStorageManager::GetStorageManagerWatcherL( iUsedSlot->BaseId() );
    iAttrManager = PEngAttrLibFactory::AttributeManagerInstanceL( iUsedSlot->BaseId() );


    iTrackedPresenceIDs = CPEngTrackedPresenceIDCollection::NewL();
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::IsActive()
// -----------------------------------------------------------------------------
//
TBool CPEngPresenceNotifier2Imp::IsActive() const
    {
    return iStarted;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::Start()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceNotifier2Imp::Start( const MDesCArray& aPresenceIDs,
                                       const TArray<TUint32>& aTypes )
    {
    //sanity checks to encapsulate notifier behaviour
    if ( iDying )
        {
        //if dying, the notifier restart is silently ignored
        //because notifier is evidently shutting down
        return KErrNone;
        }

    if ( iStarted )
        {
        return KErrInUse;
        }


    CDesCArray* sidsList = NULL;
    TRAPD( err,
        {
        //Generate SIDs
        //Generating ignores duplicate attributes and presence IDs
        GenerateSidsL( sidsList, aPresenceIDs );


        //Add the ID & attributes to notify collection
        //Adding ignores duplicate attributes and presence IDs
        iTrackedPresenceIDs->AddTrackedIdsL( aPresenceIDs, aTypes );


        //Cache tracked attribute types
        //Adding ignores duplicate attributes
        AddTrackedAttributesL( aTypes );


        //and register for SID change events
        iWatcher->RegisterListenSIDsObserverL(
            *sidsList,
            *this,
            MPEngStorageManagerWatcher::EPEngObserverNormalPriority,
            EFalse );
        } );

    delete sidsList;

    if ( err == KErrNone )
        {
        iStarted = ETrue;
        }

    else
        {
        //Cancel the notification requests
        iTrackedPresenceIDs->RemoveAllTrackedIds();
        iWatcher->UnregisterListenSIDsObserver( *this );
        iTrackedAttributes.Reset();
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::Stop()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::Stop()
    {
    //if not running, nothing to do
    if ( !iStarted )
        {
        return;
        }


    // The state must be set to stopped before actual stopping to
    // prevent unintentional event leaks.
    iStarted = EFalse;

    //Cancel the notification requests
    iWatcher->RestartSIDsObserver( *this );

    //Reset cached arrays
    iTrackedPresenceIDs->RemoveAllTrackedIds();
    iTrackedAttributes.Reset();
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::Update()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceNotifier2Imp::Update( const MDesCArray& aPresenceIDs,
                                        const TArray<TUint32>* aTypes,
                                        TBool aKeepOldTrackeds )
    {
    if ( !iStarted )
        {
        return KErrNotReady;
        }

    TArray<TUint32> trackedAttributesLocal = iTrackedAttributes.Array();
    if ( !aTypes )
        {
        aTypes = &trackedAttributesLocal;
        }


    CDesCArray* sidsList = NULL;
    TRAPD( err, UpdateWatcherL( sidsList,
                                aPresenceIDs,
                                *aTypes,
                                aKeepOldTrackeds ) );

    //If there isn't sidsList, the add has failed totally
    //and there isn't a need to tidy the list or watcher
    if ( ( err != KErrNone ) && sidsList )
        {
        // if aKeepOldTrackeds was not active, no need to clean tracked ids
        if ( aKeepOldTrackeds )
            {
            iTrackedPresenceIDs->RemoveTrackedIds( aPresenceIDs, *aTypes );
            }
        iWatcher->RemoveSIDsFromSIDsObserver( *sidsList, *this );
        }

    delete sidsList;
    return err;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::Remove()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceNotifier2Imp::Remove( const TDesC& aPresenceID )
    {
    if ( !iStarted )
        {
        return KErrNotReady;
        }


    const CPEngTrackedPresenceIDEntry* trackedID =
        iTrackedPresenceIDs->FindTrackedPresenceID( aPresenceID );
    if ( !trackedID )
        {
        return KErrNotFound;
        }


    TPEngMDesCArrayAdapter presenceIdAdapter( aPresenceID );
    CDesCArray* sidsList = NULL;
    TRAPD( err,
           GenerateSidsL( sidsList, presenceIdAdapter ) );
    if ( err != KErrNone )
        {
        delete sidsList;
        return err;
        }


    //No exceptions allowed below...
    iTrackedPresenceIDs->RemoveTrackedIds( presenceIdAdapter );
    iWatcher->RemoveSIDsFromSIDsObserver( *sidsList, *this );
    delete sidsList;

    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::Remove()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceNotifier2Imp::Remove( TUint32 aType )
    {
    if ( !iStarted )
        {
        return KErrNotReady;
        }


    //Gather first a filtered list of presence ID's that have
    //attribute as tracked
    iTrackedPresenceIDs->ResetFilteredList();
    const TInt trackedCount = iTrackedPresenceIDs->TrackedPresenceIDsCount();
    for ( TInt trackedIx = 0; trackedIx < trackedCount; trackedIx++ )
        {
        const CPEngTrackedPresenceIDEntry& trackedID =
            iTrackedPresenceIDs->TrackedPresenceID( trackedIx );

        if ( trackedID.IsOnlyTrackedAttribute( aType ) )
            {
            iTrackedPresenceIDs->IncludeToFilteredList( trackedIx );
            }
        }

    //Gather list of SIDs to unregister
    CDesCArray* sidsList = NULL;

    const MDesCArray& filteredList = iTrackedPresenceIDs->FilteredList();
    TPEngGenArrayAdapter< TUint32 > attrTypeAdapter( aType );
    //if( filteredList.MdcaCount() > 0 )
        {
        TRAPD( err,
               GenerateSidsL( sidsList,
                              filteredList ) );
        if ( err != KErrNone )
            {
            return err;
            }
        }


    //List of to be removed SIDs gathered -> do the remove
    //No exceptions allowed below...

    iTrackedPresenceIDs->RemoveTrackedIds( attrTypeAdapter.Array() );
    iWatcher->RemoveSIDsFromSIDsObserver( *sidsList, *this );
    delete sidsList;

    if ( RemoveTrackedAttributes( attrTypeAdapter.Array() ) )
        {
        return KErrNone;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::TrackedPresenceIDs()
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDs2& CPEngPresenceNotifier2Imp::TrackedPresenceIDs()
    {
    return iTrackedPresenceIDs->Interface();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::AddObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceNotifier2Imp::AddObserver( MPEngPresenceObserver2& aObserver )
    {
    return iObsArray.AddObserver( &aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::RemoveObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceNotifier2Imp::RemoveObserver( MPEngPresenceObserver2& aObserver )
    {
    return iObsArray.RemoveObserver( &aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::HandleSIDsChangeL()
// From MPEngSIDChangeObserver
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::HandleSIDsChangeL( CPtrCArray& aChangedSIDs )
    {
    //if not running, don't notify the observers
    if ( !iStarted )
        {
        return;
        }

    iTrackedPresenceIDs->ResetPresenceChangeMarks();
    const TInt sidCount( aChangedSIDs.MdcaCount() );

    PENG_DP( D_PENG_LIT( "CPEngPresenceNotifier2Imp::HandleSIDsChangeL() - %d attribute SIDs" ),
             sidCount );

    for ( TInt sidIx( 0 ); sidIx < sidCount; sidIx++ )
        {
        TPtrC sid = aChangedSIDs.MdcaPoint( sidIx );
        TUint32 attribute = KPEngNullAttributeType;
        TPtrC presenceId( NULL, 0 );
        TInt err = iAttrManager->ResolveStoreId( sid, attribute, presenceId );

        if ( err == KErrNone )
            {
            iTrackedPresenceIDs->MarkPresenceChange( presenceId );
            }
        }

    MediateNotify( KErrNone );
    }




// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::HandleSIDNotifyError()
// From MPEngSIDChangeObserver
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::HandleSIDNotifyError( TInt aError )
    {
    //if not running, don't notify the observers
    if ( !iStarted )
        {
        return;
        }

    iTrackedPresenceIDs->ResetPresenceChangeMarks();
    MediateNotify( aError );
    }





// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::GenerateSidsL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::GenerateSidsL( CDesCArray*& aGeneratedSids,
                                               const MDesCArray& aPresenceIDs )
    {
    aGeneratedSids = NULL;

    //Make sure that array granurality never gets to zero
    TInt granurality = aPresenceIDs.MdcaCount();
    if ( granurality == 0 )
        {
        granurality++;
        }

    CDesCArrayFlat* sids = new ( ELeave ) CDesCArrayFlat( granurality );
    CleanupStack::PushL( sids );

    const TInt presenceIdCount( aPresenceIDs.MdcaCount() );
    for ( TInt pIx( 0 ); pIx < presenceIdCount; pIx++ )
        {
        TPtrC presenceID( aPresenceIDs.MdcaPoint( pIx ) );

        // only one attribute to be observed
        HBufC* theSid = iAttrManager->GenerateStoreIdL( KUidPrAttrOnlineStatus, presenceID );
        CleanupStack::PushL( theSid );

        //This checks against duplicate PresenceIDs
        //==> Ignore duplicates
        TRAPD( err, sids->InsertIsqL( *theSid ) );
        if ( err != KErrAlreadyExists )
            {
            User::LeaveIfError( err );
            }

        CleanupStack::PopAndDestroy( theSid );
        }

    CleanupStack::Pop( sids );
    aGeneratedSids = sids;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::AddTrackedAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::AddTrackedAttributesL( const TArray<TUint32>& aTypes )
    {
    const TInt count = aTypes.Count();
    for ( TInt ix = 0; ix < count; ix++ )
        {
        TRAPD( err, iTrackedAttributes.InsertInSignedKeyOrderL( aTypes[ ix ] ) );
        if ( err != KErrAlreadyExists )
            {
            User::LeaveIfError( err );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::RemoveTrackedAttributes()
// -----------------------------------------------------------------------------
//
TBool CPEngPresenceNotifier2Imp::RemoveTrackedAttributes( const TArray<TUint32>& aTypes )
    {
    TBool anyRemoved = EFalse;

    const TInt count = aTypes.Count();
    for ( TInt ix = 0; ix < count; ix++ )
        {
        TInt index = iTrackedAttributes.FindInSignedKeyOrder( aTypes[ ix ] );
        if ( index != KErrNotFound )
            {
            iTrackedAttributes.Remove( index );
            anyRemoved = ETrue;
            }
        }

    return anyRemoved;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::MediateNotify()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::MediateNotify( TInt aError )
    {
    TPresenceNotifyMediator eventMediator( iInterface,
                                           *iTrackedPresenceIDs );
    if ( aError == KErrNone )
        {
        iObsArray.NotifyObservers( eventMediator );
        }
    else
        {
        iObsArray.NotifyErrorObservers( eventMediator, aError );
        }
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::UpdateWatcherL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::UpdateWatcherL(
    CDesCArray*& aSidsList,
    const MDesCArray& aPresenceIDs,
    const TArray<TUint32>& aTypes,
    TBool aKeepOldTrackeds )
    {
    GenerateSidsL( aSidsList, aPresenceIDs );

    //Update the ID & attributes to notify array
    // if we should not keep old ones, use temp tracked presence ids
    CPEngTrackedPresenceIDCollection* trackedIds = NULL;
    if ( !aKeepOldTrackeds )
        {
        trackedIds = CPEngTrackedPresenceIDCollection::NewL();
        CleanupStack::PushL( trackedIds );
        trackedIds->AddTrackedIdsL( aPresenceIDs, aTypes );
        }
    else
        {
        iTrackedPresenceIDs->AddTrackedIdsL( aPresenceIDs, aTypes );
        }

    //And update SID change request
    //aKeepOldTrackeds determines is this add or update
    iWatcher->RegisterListenSIDsObserverL( *aSidsList,
                                           *this,
                                           MPEngStorageManagerWatcher::EPEngObserverNormalPriority,
                                           aKeepOldTrackeds );

    if ( trackedIds )
        {
        delete iTrackedPresenceIDs;
        iTrackedPresenceIDs = trackedIds;
        CleanupStack::Pop( trackedIds );
        }
    }


// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::TPresenceNotifyMediator::TPresenceNotifyMediator()
// -----------------------------------------------------------------------------
//
CPEngPresenceNotifier2Imp::TPresenceNotifyMediator::TPresenceNotifyMediator(
    CPEngPresenceNotifier2& aNotifier,
    CPEngTrackedPresenceIDCollection& aPresenceIds )
        : iNotifier( aNotifier ),
        iPresenceIds( aPresenceIds )
    {
    }

// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::TPresenceNotifyMediator::MediateNotifyL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::TPresenceNotifyMediator::MediateNotifyL(
    MPEngPresenceObserver2& aObserverToNotify )
    {
    iPresenceIds.ResetTrackedEntriesIterators();
    iPresenceIds.Interface().ResetTrackedIterator();
    iPresenceIds.Interface().ResetChangedIterator();

    aObserverToNotify.HandlePresenceChangeL( iNotifier, iPresenceIds.Interface() );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceNotifier2Imp::TPresenceNotifyMediator::MediateNotifyError()
// -----------------------------------------------------------------------------
//
void CPEngPresenceNotifier2Imp::TPresenceNotifyMediator::MediateNotifyError(
    MPEngPresenceObserver2& aObserverToNotify,
    TInt aLeaveError )
    {
    aObserverToNotify.HandlePresenceError( aLeaveError, iNotifier );
    }



// End of File



