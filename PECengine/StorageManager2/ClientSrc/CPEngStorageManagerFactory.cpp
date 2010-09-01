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
* Description:  Factory of Storage Manager
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include	<s32mem.h>
#include	"CPEngStorageManagerFactory.h"
#include	"CPEngSessionSlotId.h"
#include	"CPEngStorageManagerWatcher.h"
#include	"CPEngStorageManager.h"
#include	"CPEngSessionSlotState.h"
#include	"CPEngSessionSlotEvent.h"
#include	"MPEngGlobalEventObserver.h"

// PEC Engine internal constants
#include "PEngPresenceEngineConsts2.h"


// CONSTANTS
// 50 is the number representing additional data size of global event
const TInt KGlobalEventSize = (	KPEngMaxAppIdLength
                                + KPEngMaxServiceAddressLength +
                                + KPEngMaxUserIdLength + 50 );

// application ID count , usually up to 20 IDs per Session Slot
const TInt KAppIdCount = 20;

// size of the applications IDs per one Session slot
const TInt KSessionStateSize = KPEngMaxAppIdLength * KAppIdCount;

// basic evaluation of the buffer for the global sessions slot states
// 5 as some reasonable number of sessions, should be more than 130
const TInt KGlobalSessionStatesSize =
    5 * ( KPEngMaxAppIdLength * KAppIdCount +
          KPEngMaxServiceAddressLength + KPEngMaxUserIdLength );

// 30% reserve for incoming data buffer
const TInt KBufferMultiplier = 13 / 10;

// MACROS
/**
 *  Reload Listener
 *  @since 3.0
 */
#define RELOAD_LISTENER( aStatus )\
    if( !IsActive() \
        && \
        ( aStatus != KErrNoMemory )\
        &&\
        ( aStatus != KErrCancel )\
        &&\
        ( aStatus != KErrServerTerminated ) \
      )\
        {\
        iStoreManagerClient.ReloadGlobalEventListener( iGlobalEventDes, iStatus );\
        SetActive();\
        }

/**
 *  Check scheduled deletion of the factory
 *  return TInt
 *  @since 3.0
 */
#define CHECK_OBJECT_DELETION_TINT()\
    if( !alive )\
        {\
        return KErrNone;\
        }

/**
 *  Check scheduled deletion of the factory
 *  Pop and destroy 1 item from clean up stack
 *  @since 3.0
 */
#define CHECK_OBJECT_DELETION_POPADESX()\
    if( !alive )\
        {\
        CleanupStack::PopAndDestroy();\
        return;\
        }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::CPEngStorageManagerFactory
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngStorageManagerFactory::CPEngStorageManagerFactory()
        : CActive( CActive::EPriorityStandard ),
        iGlobalEventDes( NULL, 1 ),
        // start with one session slot
        iAllStatesBuffSize( KGlobalSessionStatesSize )
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::ConstructL()
    {
    User::LeaveIfError( iStoreManagerClient.Connect() );
    // add active object to the active scheduler
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::InstanceLC
//
// -----------------------------------------------------------------------------
//
CPEngStorageManagerFactory* CPEngStorageManagerFactory::InstanceLC()
    {

    CPEngStorageManagerFactory* storeFactory = NULL;
    storeFactory = Instance();
    if ( !storeFactory )
        {
        // create notify factory
        storeFactory = new( ELeave ) CPEngStorageManagerFactory();
        CleanupClosePushL( *storeFactory );
        storeFactory->ConstructL();
        User::LeaveIfError( Dll::SetTls( storeFactory ) );
        }
    else
        {
        storeFactory->Open();                  // CSI: 65 #
        CleanupClosePushL( *storeFactory );
        }
    return storeFactory;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::InstanceLC
//
// -----------------------------------------------------------------------------
//
CPEngStorageManagerFactory* CPEngStorageManagerFactory::Instance()
    {
    return static_cast<CPEngStorageManagerFactory*> ( Dll::Tls() );
    }


// Destructor
CPEngStorageManagerFactory::~CPEngStorageManagerFactory()
    {
    // if we have not counter to zero, panic
    __ASSERT_ALWAYS( iAccessCount == 0,
                     User::Panic( KPEngSMEuserCBasePanic,
                                  KPEngSMEuserCBasePanicReason ) );

    Cancel();
    // check si scout was created and therefore needs to be stoped
    if ( iScoutCreated )
        {
        iStoreManagerClient.StopEventListening();
        }

    iObservers.Reset();
    iStorageWatchers.ResetAndDestroy();
    iStoreManagerClient.Close();
    Dll::SetTls( NULL );
    delete iGlobalEventBuff;

    if ( iAliveFlag )
        {
        ( *iAliveFlag ) = EFalse;
        }


#if _BullseyeCoverage
    cov_write();
#endif
    }


// =============================================================================
// =============== Reference Counting Support ==================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::Open()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::Open()
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::Close()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 ) // 0
        {
        // do not delete if we are still processing notifications
        delete this;
        }
    }


// =============================================================================
// ========= Implementation of Virtual functions of CActive ====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::RunL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::RunL()
    {
    // Check the state, if the Global event fit in the buffer
    TInt status ( iStatus.Int() );
    if ( status > KErrNone )
        {
        // buffer was not big enough, set the new size
        iGlobalEventBuff = iGlobalEventBuff->ReAllocL( status );
        iGlobalEventDes.Set( iGlobalEventBuff->Des() );
        RELOAD_LISTENER( iStatus.Int() );
        return;
        }
    User::LeaveIfError( status );

    // decode event from buffer
    CPEngSessionSlotEvent* newEvent = CPEngSessionSlotEvent::NewLC();
    // unpack the buffer into the Session Event
    newEvent->UnpackEventL( *iGlobalEventBuff );

    TBool alive( ETrue );
    iAliveFlag = &alive;

    // now notify all who is interested in the event
    TInt count ( iObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        TRAPD( e,
               iObservers[ x ]->HandleNewSessionSlotEventL( *newEvent ) );
        if ( e != KErrNone )
            {
            iObservers[ x ]->HandleObserverError( e );
            }
        }

    CHECK_OBJECT_DELETION_POPADESX();

    // now notify Watcher which is connected to this
    count = iStorageWatchers.Count();
    const CPEngSessionSlotId& slotId =
        newEvent->SessionSlotIndentification();
    for ( TInt i( 0 ) ; i < count ; i++ )
        {
        if (
            KErrNone == iStorageWatchers[ i ]->SessionSlotId().Match(
                slotId )
        )
            {
            iStorageWatchers[ i ]->NotifyGlobalEvent( *newEvent );
            // leave loop, only one watcher can match
            break;
            }
        }

    CHECK_OBJECT_DELETION_POPADESX();
    iAliveFlag = NULL;

    CleanupStack::PopAndDestroy(); // newEvent
    // continue listening
    RELOAD_LISTENER( iStatus.Int() );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::RunError()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManagerFactory::RunError( TInt aError )
    {
    // notify error to the listeners

    TBool alive( ETrue );
    iAliveFlag = &alive;

    TInt count ( iObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        iObservers[ x ]->HandleObserverError( aError );
        TInt count ( iStorageWatchers.Count() );
        for ( TInt i ( 0 ) ; i < count ; i++ )
            {
            iStorageWatchers[ i ]->NotifyListenerError( aError );
            }
        }

    CHECK_OBJECT_DELETION_TINT();
    iAliveFlag = NULL;

    RELOAD_LISTENER( aError );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::DoCancel()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::DoCancel()
    {
    iStoreManagerClient.StopEventListening();
    iScoutCreated = EFalse;
    }


// =============================================================================
// =============== Functions from MPEngStorageFactory class ====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::CloseStorageWatcher()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::CloseStorageWatcher(
    CPEngStorageManagerWatcher* aStorageWatcher )
    {
    TInt index( iStorageWatchers.Find( aStorageWatcher ) );
    if ( index != KErrNotFound )
        {
        iStorageWatchers.Remove( index );
        }
    Close();
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::OpenWatcher()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::OpenWatcher()
    {
    Open();
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::MainClient()
// -----------------------------------------------------------------------------
//
RPEngStorageClient& CPEngStorageManagerFactory::MainClient()
    {
    return iStoreManagerClient;
    }


// =============================================================================
// =============== Functions from base  class ==================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::StorageManagerL()
// -----------------------------------------------------------------------------
//
CPEngStorageManager* CPEngStorageManagerFactory::StorageManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    // get watcher first, handle of the watcher is increased by one there
    CPEngStorageManagerWatcher*  watcher =
        StorageManagerWatcherL( aSessionSlot );
    CleanupClosePushL( *watcher );
    // now create Storage Manager
    CPEngStorageManager* newStoreManager =
        CPEngStorageManager::NewL( *watcher,
                                   iStoreManagerClient );

    // handle count was increased in the CPEngStorageManager::NewL
    CleanupStack::PopAndDestroy( );  //
    return newStoreManager;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::StorageManagerWatcherL()
// -----------------------------------------------------------------------------
//
CPEngStorageManagerWatcher* CPEngStorageManagerFactory::
StorageManagerWatcherL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    TInt count ( iStorageWatchers.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        if ( KErrNone ==
             iStorageWatchers[ x ]->SessionSlotId().Match(
                 aSessionSlot ) )
            {
            iStorageWatchers[ x ]->Open();
            return iStorageWatchers[ x ];
            }
        }
    // watcher does not exist yet, create it
    CPEngStorageManagerWatcher* newWatcher =
        CPEngStorageManagerWatcher::NewLC( aSessionSlot,
                                           iStoreManagerClient,
                                           *this );
    User::LeaveIfError( iStorageWatchers.Append( newWatcher ) );
    CleanupStack::Pop(); // newWatcher
    return newWatcher;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::CreateSessionFolderL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::CreateSessionFolderL(
    const CPEngSessionSlotId& aSessionSlot,
    const TDesC& aApplicationId )
    {
    HBufC8* sessionName = aSessionSlot.PackLC();
    User::LeaveIfError( iStoreManagerClient.CreateSessionFolder(
                            *sessionName,
                            aApplicationId ) );
    CleanupStack::PopAndDestroy(); // sessionName
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::RemoveSessionFolderL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::RemoveSessionFolderL(
    const CPEngSessionSlotId& aSessionSlot,
    const TDesC& aApplicationId )
    {
    HBufC8* sessionName = aSessionSlot.PackLC();
    User::LeaveIfError( iStoreManagerClient.RemoveSessionFolder(
                            *sessionName,
                            aApplicationId ) );
    CleanupStack::PopAndDestroy(); // sessionName
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::SessionSlotStateL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::SessionSlotStateL(
    const CPEngSessionSlotId& aSessionSlot,
    CPEngSessionSlotState& aSessionSlotState )
    {
    CPEngSessionSlotId* id = aSessionSlot.CloneLC();
    aSessionSlotState.SetSessionSlotId( *id, ETrue );
    CleanupStack::Pop(); // id

    HBufC8* ssId = aSessionSlotState.SessionSlotId().PackLC();
    HBufC8* sessionStateBuf = NULL;


    User::LeaveIfError( iStoreManagerClient.SessionSlotState(
                            *ssId,
                            sessionStateBuf,
                            KSessionStateSize ) );

    CleanupStack::PushL( sessionStateBuf );
    aSessionSlotState.UnpackDataL( *sessionStateBuf, EPureState );
    CleanupStack::PopAndDestroy( 2 ); // sessionState, ssId
    }



// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::AllSessionSlotsStatesL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::AllSessionSlotsStatesL(
    RObjectArray<CPEngSessionSlotState>& aSessionSlots )
    {
    // clean passed array
    aSessionSlots.ResetAndDestroy();
    // try the size of the GLobal even, times session
    HBufC8* allSessionsBuff = NULL;
    // if there is error, des buffer will be cleaned
    User::LeaveIfError( iStoreManagerClient.AllSessionSlotsStates(
                            allSessionsBuff,
                            iAllStatesBuffSize * KBufferMultiplier ) );

    iAllStatesBuffSize = allSessionsBuff->Length();
    CleanupStack::PushL( allSessionsBuff );
    // unpack the buffer into the Session states
    RDesReadStream drs( allSessionsBuff->Des() );
    CleanupClosePushL( drs );
    // get count of the sessions
    TInt32 count ( drs.ReadInt32L() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        CPEngSessionSlotState* newState = CPEngSessionSlotState::NewLC();
        newState->InternalizeL( drs, EWholeState );
        aSessionSlots.AppendL( newState );
        CleanupStack::Pop( ); // newState
        }
    CleanupStack::PopAndDestroy( 2 ); // drs, allSessionsBuff
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::RegisterGlobalEventListenerL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::RegisterGlobalEventObserverL(
    MPEngGlobalEventObserver& aGlobalObserver )
    {
    // is it first observer, do we need to start observing
    if ( iObservers.Count() == 0 )
        {
        StartGlobalEventListenerL();
        }
    iObservers.InsertInAddressOrderL( &aGlobalObserver );
    Open();
    }


// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::UnregisterGlobalEventListener()
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::UnregisterGlobalObserver(
    MPEngGlobalEventObserver& aGlobalObserver )
    {
    TInt err ( iObservers.FindInAddressOrder( &aGlobalObserver ) );
    if ( err != KErrNotFound )
        {
        iObservers.Remove( err );
        if ( iObservers.Count() == 0 )
            {
            Cancel();
            }
        Close();
        }
    }


// =============================================================================
// =============== New Private Functions from main  class ======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageManagerFactory::UnregisterGlobalEventListener()
// Start global event listener
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageManagerFactory::StartGlobalEventListenerL()
    {
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }

    if ( !iScoutCreated )
        {
        delete iGlobalEventBuff;
        iGlobalEventBuff = NULL;
        iGlobalEventBuff = HBufC8::NewL( KGlobalEventSize );
        iGlobalEventDes.Set( iGlobalEventBuff->Des() );

        TInt err ( iStoreManagerClient.ListenGlobalEvents() );
        if ( err < KErrNone )
            {
            User::Leave( err );
            }
        iScoutCreated = ETrue;
        }
    iStoreManagerClient.ReloadGlobalEventListener( iGlobalEventDes,
                                                   iStatus );
    SetActive();
    }

//  End of File
