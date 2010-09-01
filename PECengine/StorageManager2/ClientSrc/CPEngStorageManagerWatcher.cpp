/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It is used to liste Storage Ids and session state
*				 changes
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngStorageManagerWatcher.h"

#include	"CPEngSessionSlotId.h"

#include	"CPEngSIdChangeListener.h"
#include	"MPEngStorageFactory.h"

#include	"PEngMessagePacker.h"
#include	"CPEngStorageManager.h"
#include	"MPEngGlobalEventObserver.h"

#include	"PresenceDebugPrint.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::CPEngStorageManagerWatcher
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngStorageManagerWatcher::CPEngStorageManagerWatcher(
    MPEngStorageFactory& aStorageFactory )
        : iStorageManagerFactory( aStorageFactory )
    {
    iAccessCount++;
    iStorageManagerFactory.OpenWatcher();
    }

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::ConstructL(
    const CPEngSessionSlotId& aSessionSlot,
    RPEngStorageClient& aStorageClient )
    {
    iSessionSlotId = aSessionSlot.CloneL();
    HBufC8* sessionName = aSessionSlot.PackLC();
    User::LeaveIfError( iNotifyClient.Connect( aStorageClient,
                                               *sessionName ) );
    CleanupStack::PopAndDestroy(); // sessionName
    }

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageManagerWatcher* CPEngStorageManagerWatcher::NewL(
    const CPEngSessionSlotId& aSessionSlot,
    RPEngStorageClient& aStorageClient,
    MPEngStorageFactory& aStorageFactory )
    {
    CPEngStorageManagerWatcher* self = NewLC(	aSessionSlot,
                                              aStorageClient,
                                              aStorageFactory );

    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageManagerWatcher* CPEngStorageManagerWatcher::NewLC(
    const CPEngSessionSlotId& aSessionSlot,
    RPEngStorageClient& aStorageClient,
    MPEngStorageFactory& aStorageFactory )
    {
    CPEngStorageManagerWatcher* self = new( ELeave )
    CPEngStorageManagerWatcher( aStorageFactory );
    // reference counted
    CleanupClosePushL( *self );
    self->ConstructL( aSessionSlot, aStorageClient );
    return self;
    }


// Destructor
CPEngStorageManagerWatcher::~CPEngStorageManagerWatcher()
    {
    // if we have not counter to zero, panic
    __ASSERT_ALWAYS( iAccessCount == 0,
                     User::Panic( KPEngSMEuserCBasePanic,
                                  KPEngSMEuserCBasePanicReason ) );

    // close has canceled all async requests
    delete iStorageIdListener;
    delete iSessionSlotId;
    iEventObservers.Reset();
    iNotifyClient.Close();

    iStorageManagerFactory.CloseStorageWatcher( this );
    // signal to the Storage factory that this Watcher is dying
    }



// =============================================================================
// =============== New public function of the class ============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::NotifyGlobalEvent()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::NotifyGlobalEvent(
    const CPEngSessionSlotEvent& aNewEvent )
    {
    // notify observers
    TInt count ( iEventObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        TRAPD( e, iEventObservers[ x ]->HandleNewSessionSlotEventL(
                   aNewEvent ) );
        if ( e != KErrNone )
            {
            iEventObservers[ x ]->HandleObserverError( e );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::NotifySessionSlotRemovalL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::NotifyListenerError(
    TInt aError )
    {
    // notify observers
    TInt count ( iEventObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        iEventObservers[ x ]->HandleObserverError( aError );
        }
    }


// =============================================================================
// ============ Functions of the MPEngStorageManagerWatcher class===============
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::Open()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::Open()
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::RegisterListenEventObserverL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 ) // 0
        {
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::CreateStorageManagerL()
// -----------------------------------------------------------------------------
//
MPEngStorageManager* CPEngStorageManagerWatcher::CreateStorageManagerL()
    {
    return CPEngStorageManager::NewL(
               *this,
               iStorageManagerFactory.MainClient() );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::RegisterListenEventObserverL()
// -----------------------------------------------------------------------------
//
TPEngNWSessionSlotState CPEngStorageManagerWatcher::RegisterListenEventObserverL(
    MPEngGlobalEventObserver& aEventObserver )
    {
    User::LeaveIfError( iEventObservers.InsertInAddressOrder(
                            &aEventObserver ) );
    // increase reference counter
    this->Open();                       // CSI: 65 #

    return iSessionState;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::UnregisterListenEventObserver()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::UnregisterListenEventObserver(
    MPEngGlobalEventObserver& aEventObserver )
    {
    TInt err ( iEventObservers.FindInAddressOrder( &aEventObserver ) );
    if ( err != KErrNotFound )
        {
        iEventObservers.Remove( err );
        this->Close();
        // decrease reference counter
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::CurrentStateOfPresenceServerL()
// -----------------------------------------------------------------------------
//
TPEngNWSessionSlotState CPEngStorageManagerWatcher::CurrentStateOfSessionSlot()
    {
    return iSessionState;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::RegisterListenSIDsObserverL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::RegisterListenSIDsObserverL(
    const MDesCArray& aInterestedSIds,
    MPEngSIDChangeObserver& aSIdChangeObserver,
    TPEngObserverPriority aPriority,
    TBool aKeepOldIds )
    {
    LoadSIdListenerL();
    // ETrue if listener was existing -> decrease ref count
    if ( iStorageIdListener->RegisterObserverL( aInterestedSIds,
                                                aSIdChangeObserver,
                                                aPriority,
                                                aKeepOldIds ) )
        {
        // increase reference counter
        this->Open();               // CSI: 65 #
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::RemoveSIDsFromSIDsObserverL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::RemoveSIDsFromSIDsObserver(
    const MDesCArray& aSIdsToRemove,
    const MPEngSIDChangeObserver& aSIdChangeObserver )
    {
    if ( iStorageIdListener )
        {
        iStorageIdListener->RemoveSIdsFromObserver(
            aSIdsToRemove,
            aSIdChangeObserver );
        }
    }

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::RestartSIDsObserver()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::RestartSIDsObserver(
    const MPEngSIDChangeObserver& aSIdChangeObserver )
    {
    if ( iStorageIdListener )
        {
        iStorageIdListener->RestartObserver(
            aSIdChangeObserver );
        }
    }

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::UnregisterListenSIDsObserver()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::UnregisterListenSIDsObserver(
    const MPEngSIDChangeObserver& aSIDChangeObserver )
    {
    if ( iStorageIdListener
         &&
         ( iStorageIdListener->UnRegisterObserver( aSIDChangeObserver ) )
       )
        {
        CleanStorageIdListener();
        this->Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::SuspendListenSIDsObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManagerWatcher::SuspendListenSIDsObserver(
    const MPEngSIDChangeObserver& aSIdChangeObserver )
    {
    if ( iStorageIdListener )
        {
        return iStorageIdListener->UpdateSuspendStateOfObserver(
                   aSIdChangeObserver,
                   ETrue );
        }
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::ResumeListenSIDsObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManagerWatcher::ResumeListenSIDsObserver(
    const MPEngSIDChangeObserver& aSIdChangeObserver )
    {
    if (	iStorageIdListener )
        {
        return iStorageIdListener->UpdateSuspendStateOfObserver(
                   aSIdChangeObserver,
                   EFalse );
        }
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::SessionSlotId()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngStorageManagerWatcher::SessionSlotId() const
    {
    return *iSessionSlotId;
    }


// -----------------------------------------------------------------------------
// Private functions of the CPEngStorageManagerWatcher
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::CleanStorageIdListener()
// Check if there is still some SId listener
// and deletes it if it's not needed
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::CleanStorageIdListener()
    {
    if ( !iStorageIdListener->AnyActiveListener() )
        {
        delete iStorageIdListener;
        iStorageIdListener = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CPEngStorageManagerWatcher::LoadSIdListenerL()
// Get Storage ID listener,
// if it does not exists, it constructed
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerWatcher::LoadSIdListenerL()
    {
    if ( !iStorageIdListener )
        {
        iStorageIdListener = CPEngSIdChangeListener::NewL( iNotifyClient );
        }
    }


//  End of File
