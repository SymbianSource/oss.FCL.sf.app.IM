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
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngContactListNotifier2Imp.h"
#include "CPEngNWSessionSlotEventEntry.h"
#include "GenObserverNotifyMediators.h"

#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngStorageManagerWatcher.h"
#include "PEngStorageManager.h"
#include "TPEngMDesCArrayAdapter.h"

#include <CPEngContactListNotifier2.h>
#include <MPEngContactListObserver2.h>




// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContactListNotifier2Imp* CPEngContactListNotifier2Imp::NewL(
    CPEngContactListNotifier2& aInterface,
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngContactListNotifier2Imp* self =
        new ( ELeave ) CPEngContactListNotifier2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngContactListNotifier2Imp::~CPEngContactListNotifier2Imp()
    {
    iDying = ETrue;
    Stop();

    if ( iWatcher )
        {
        iWatcher->UnregisterListenSIDsObserver( *this );
        iWatcher->Close();
        }

    iObsArray.Close();
    delete iUsedSlot;
    }



// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::CPEngContactListNotifier2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngContactListNotifier2Imp::CPEngContactListNotifier2Imp(
    CPEngContactListNotifier2& aInterface,
    TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }



// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListNotifier2Imp::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iWatcher = PEngStorageManager::GetStorageManagerWatcherL( iUsedSlot->BaseId() );
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::Start()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListNotifier2Imp::Start( const MDesCArray& aContactListNames )
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

    // EFalse as not keeping previous observing Ids
    TRAPD( err,
        {
        // and register for SID change events
        iWatcher->RegisterListenSIDsObserverL(
            aContactListNames,
            *this,
            MPEngStorageManagerWatcher::EPEngObserverNormalPriority,
            EFalse );
        } );

    if ( err == KErrNone )
        {
        iStarted = ETrue;
        }

    else
        {
        //Cancel the notification requests
        iWatcher->UnregisterListenSIDsObserver( *this );
        }

    return err;
    }




// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::Stop()
// -----------------------------------------------------------------------------
//
void CPEngContactListNotifier2Imp::Stop()
    {
    // we cannot actually unregister all SIDs/possibly can be implemented in Sotrage
    // anyway, this will cause to ignore all possible notifications
    if ( !iStarted )
        {
        return;
        }
    iWatcher->RestartSIDsObserver( *this );
    iStarted = EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::IsActive()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListNotifier2Imp::IsActive() const
    {
    return iStarted;
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::Add()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListNotifier2Imp::Add( const TDesC& aContactListName )

    {
    if ( !iStarted )
        {
        return KErrNotReady;
        }

    TPEngMDesCArrayAdapter adapter( aContactListName );
    TRAPD( err,
        {
        //Update SID change request
        iWatcher->RegisterListenSIDsObserverL(
            adapter,
            *this,
            MPEngStorageManagerWatcher::EPEngObserverNormalPriority,
            ETrue );
        } );

    //If in error, tidy the watcher
    if ( err != KErrNone  )
        {
        iWatcher->RemoveSIDsFromSIDsObserver( adapter, *this );
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::Remove()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListNotifier2Imp::Remove( const TDesC& aContactListName )

    {
    if ( !iStarted )
        {
        return KErrNotReady;
        }

    //No exceptions allowed below...
    TPEngMDesCArrayAdapter adapter( aContactListName );
    iWatcher->RemoveSIDsFromSIDsObserver( adapter, *this );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::AddObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListNotifier2Imp::AddObserver(
    MPEngContactListObserver2& aObserver )

    {
    return iObsArray.AddObserver( &aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::RemoveObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListNotifier2Imp::RemoveObserver(
    MPEngContactListObserver2& aObserver )

    {
    return iObsArray.RemoveObserver( &aObserver );
    }




// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::HandleSIDsChangeL()
// From MPEngSIDChangeObserver
// -----------------------------------------------------------------------------
//
void CPEngContactListNotifier2Imp::HandleSIDsChangeL( CPtrCArray& aChangedSIDs )
    {
    //if not running, don't notify the observers
    if ( !iStarted )
        {
        return;
        }

    const TInt sidCount = aChangedSIDs.MdcaCount();
    for ( TInt ix = 0; ix < sidCount; ix++ )
        {
        MediateNotify( KErrNone, aChangedSIDs.MdcaPoint( ix ) );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::HandleSIDNotifyError()
// From MPEngSIDChangeObserver
// -----------------------------------------------------------------------------
//
void CPEngContactListNotifier2Imp::HandleSIDNotifyError( TInt aError )
    {
    //if not running, don't notify the observers
    if ( !iStarted )
        {
        return;
        }

    MediateNotify( aError, KNullDesC );
    }


// -----------------------------------------------------------------------------
// CPEngContactListNotifier2Imp::MediateNotify()
// -----------------------------------------------------------------------------
//
void CPEngContactListNotifier2Imp::MediateNotify( TInt aError,
                                                  const TDesC& aContactListName )
    {
    TGenNotifyMediator2 < MPEngContactListObserver2,
    CPEngContactListNotifier2&,
    const TDesC& >
    eventMediator( &MPEngContactListObserver2::HandleContactListChangeL,
                   &MPEngContactListObserver2::HandleContactListError,
                   iInterface,
                   aContactListName );

    if ( aError == KErrNone )
        {
        iObsArray.NotifyObservers( eventMediator );
        }
    else
        {
        iObsArray.NotifyErrorObservers( eventMediator, aError );
        }
    }



// End of File


