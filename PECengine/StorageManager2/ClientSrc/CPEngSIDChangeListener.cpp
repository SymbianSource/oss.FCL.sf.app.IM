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
* Description:  Storage Id listener manager
*
*/



// INCLUDE FILES
#include "CPEngSIdChangeListener.h"
#include <e32std.h>

// Observer, SID containers
#include "CPEngSIdContainer.h"
#include "CPEngObserverContainer.h"
#include "MPEngSIDChangeObserver.h"

// Sub Client of the Storage Server
#include "RPEngStorageSubFolderClient.h"

// Watcher for the priority enumeration
#include "MPEngStorageManagerWatcher.h"

// packing tool for server messages
#include "PEngMessagePacker.h"

// debug prints
#include "PresenceDebugPrint.h"


// MACROS
// Check it there was error and roll back if there was error
#define IF_ERROR_ROLLBACK_LEAVE( aError )\
        {\
        TInt eC(  aError );\
        if ( eC != KErrNone )\
            {\
            RollBackSIdArray();\
            User::Leave( eC );\
            }\
        }

#define RETURN_IF_ERROR( aErrorCode )\
    TInt eC(  aErrorCode );\
    if ( eC != KErrNone )\
        {\
        iTransBuff.Close();\
        return eC;\
        }

// LOCAL CONSTANTS AND MACROS
const TInt KArrayGranularity = 5; // about 5 attributes pre WV ID

// Length of the 16bit descriptor needed for storing of one 32 bit Number
const TInt KDes16NumberLength = 2;

// ============================= LOCAL FUNCTIONS ===============================

TInt CompareObserversPointers( const MPEngSIDChangeObserver* aObserver1,
                               const MPEngSIDChangeObserver* aObserver2 )
    {
    if ( aObserver1 == aObserver2 )
        {
        return KErrNone;
        }
    if ( aObserver1 < aObserver2 )
        {
        return -1;
        }
    return 1;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::CPEngSIdChangeListener
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngSIdChangeListener::CPEngSIdChangeListener(
    RPEngStorageSubFolderClient& aNotifyClient )
        : CActive( CActive::EPriorityStandard ),
        iNotifyClient( aNotifyClient ),
        iTransBuffSize( 2 ), // 2 as init for count
        iObservers( KArrayGranularity ),
        iSIdContainers( KArrayGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::ConstructL()
    {
    // add active object to the active scheduler
    CActiveScheduler::Add( this );
    iAnswerBuff.CreateL( KDes16NumberLength );
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSIdChangeListener* CPEngSIdChangeListener::NewL(
    RPEngStorageSubFolderClient& aNotifyClient )
    {
    CPEngSIdChangeListener* self = NewLC( aNotifyClient );

    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSIdChangeListener* CPEngSIdChangeListener::NewLC(
    RPEngStorageSubFolderClient& aNotifyClient )
    {
    CPEngSIdChangeListener* self = new( ELeave )
    CPEngSIdChangeListener( aNotifyClient );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
CPEngSIdChangeListener::~CPEngSIdChangeListener()
    {
    Cancel();
    if ( iListenerRunning )
        {
        iNotifyClient.StopListeningSIdsChanges();
        }
    iTransBuff.Close();
    iAnswerBuff.Close();
    iObservers.ResetAndDestroy();
    iSIdContainers.ResetAndDestroy();
    if ( iAliveFlag )
        {
        ( *iAliveFlag ) = EFalse;
        }
    }


// =============================================================================
// new functions of the CPEngSIdChangeListener
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::RegisterObserverL()
// -----------------------------------------------------------------------------
//
TBool CPEngSIdChangeListener::RegisterObserverL(
    const MDesCArray& aNewSIds,
    MPEngSIDChangeObserver& aSIdChangeObserver,
    TInt aPriority,
    TBool aKeepOldIds )
    {
    TInt index( 0 );
    if ( KErrNotFound == FindObserver( aSIdChangeObserver, index ) )
        {
        return AddNewObserverL( aNewSIds,
                                aSIdChangeObserver,
                                aPriority );
        }
    // update array
    RPointerArray<MPEngSIdContainer> sidContainers;
    CleanupClosePushL( sidContainers );
    CPEngObserverContainer* container = iObservers[ index ];
    RPointerArray<MPEngSIdContainer> containersBackUp;
    CleanupClosePushL( containersBackUp );

    TBool update( EFalse );
    TRAPD( e, update = UpdateEngagedSIdsArrayL( aNewSIds,
                                                sidContainers ) );

    IF_ERROR_ROLLBACK_LEAVE( e );
    TInt err ( container->UpdateSIdsContainers( sidContainers,
                                                containersBackUp,
                                                aKeepOldIds ) );
    IF_ERROR_ROLLBACK_LEAVE( err );
    if ( update )
        {
        err = UpdateListenerToServer();
        if ( err != KErrNone )
            {
            // this will go fine, since there is room in array to roll back
            // put array to old state
            container->UpdateSIdsContainers( containersBackUp, EFalse );
            RollBackSIdArray();
            User::Leave( err );
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // containersBackUp, sidContainers
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::RemoveSIdsFromObserver()
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::RemoveSIdsFromObserver(
    const MDesCArray& aSIdsToRemove,
    const MPEngSIDChangeObserver& aSIdChangeObserver )
    {
    TInt index( 0 );
    if ( KErrNotFound == FindObserver( aSIdChangeObserver, index ) )
        {
        return;
        }
    iObservers[ index ]->RemoveSIdsFromSIdsObserver( aSIdsToRemove );
    if ( UpdateRequired() )
        {
        // ignore error, array does not have to be rebuilded if it failed
        UpdateListenerToServer();
        }
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::RestartObserver()
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::RestartObserver(
    const MPEngSIDChangeObserver& aSIdChangeObserver )
    {
    TInt index( 0 );
    if ( KErrNotFound == FindObserver( aSIdChangeObserver, index ) )
        {
        return;
        }
    iObservers[ index ]->RestartObserver();
    if ( UpdateRequired() )
        {
        // ignore error, array does not have to be rebuilded if it failed
        UpdateListenerToServer();
        }
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::UnRegisterObserver()
// -----------------------------------------------------------------------------
//
TBool CPEngSIdChangeListener::UnRegisterObserver(
    const MPEngSIDChangeObserver& aSIdChangeObserver )
    {
    TInt index( 0 );
    if ( KErrNotFound == FindObserver( aSIdChangeObserver, index ) )
        {
        return EFalse;
        }

    delete iObservers[ index ];
    iObservers.Remove( index );

    if ( UpdateRequired() )
        {
        // ignore error, array does not have to be rebuilded if it failed
        UpdateListenerToServer();
        }
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::UpdateSuspendStateOfObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngSIdChangeListener::UpdateSuspendStateOfObserver(
    const MPEngSIDChangeObserver& aSIdChangeObserver,
    const TBool aSuspendState )
    {
    TInt index( 0 );
    if ( KErrNotFound == FindObserver( aSIdChangeObserver, index ) )
        {
        return KErrNotFound;
        }
    iObservers[ index ]->SetSuspendMode( aSuspendState );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::AnyActiveListener()
// -----------------------------------------------------------------------------
//
TBool CPEngSIdChangeListener::AnyActiveListener()
    {
    return ( iObservers.Count() != 0 );
    }


// -----------------------------------------------------------------------------
// -------- Function from the CActive base class -------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::RunL()
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::RunL()
    {
    //check the indexes
    TInt status ( iStatus.Int() );
    User::LeaveIfError( status );
    PEngMessagePacker::ResetTIntArray( iAnswerBuff );
    User::LeaveIfError( iNotifyClient.FetchChangedSIdsIndexes( iAnswerBuff ) );
    TInt count( PEngMessagePacker::TIntArrayCount( iAnswerBuff ) );
    TInt sidsCount( iSIdContainers.Count() );

    // collect what has changed
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        TInt index = PEngMessagePacker::TIntFromArray( iAnswerBuff, x );
        // test end of array . The array is filled with -1s
        if ( index == KErrNotFound )
            {
            break;
            }
        if ( ( index >= sidsCount ) || ( index < 0 ) )
            {
            User::Leave( KErrTotalLossOfPrecision );
            //invalid index, what to DO!?
            }
        // mark change
        iSIdContainers[ index ]->NotifySIdChangeL();
        }

    TBool alive( ETrue );
    iAliveFlag = &alive;

    InformObserverAboutChange();

    if ( !alive )
        {
        return;
        }
    iAliveFlag = NULL;

    User::LeaveIfError( iNotifyClient.ReloadListenSIdsChangesScout(
                            iStatus ) );
    SetActive();
    }


// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::RunError()
// -----------------------------------------------------------------------------
//
TInt CPEngSIdChangeListener::RunError(
    TInt aError )
    {
    // notify about change if something was marked before leave;
    InformObserverAboutChange();

    // reload listener if there was not any critical error code,
    // if error is KErrCancel reload if there still some id observed
    if ( ( aError != KErrNoMemory )
         &&
         ( ( aError != KErrCancel ) || iListenerRunning )
         &&
         ( aError != KErrServerTerminated )
       )
        {
        iAnswerBuff.Zero();
        aError = iNotifyClient.ReloadListenSIdsChangesScout(
                     iStatus );
        if ( KErrNone == aError )
            {
            SetActive();
            return KErrNone;
            }
        }

    TBool alive( ETrue );
    iAliveFlag = &alive;

    InformObserverAboutError( aError );

    if ( !alive )
        {
        return KErrNone;
        }
    iAliveFlag = NULL;

    iNotifyClient.StopListeningSIdsChanges();
    iListenerRunning = EFalse;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::DoCancel()
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::DoCancel()
    {
    iNotifyClient.StopListeningSIdsChanges();
    iListenerRunning = EFalse;
    }

// -----------------------------------------------------------------------------
// -------- New private Function of the CPEngSIdChangeListener -----------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::AddNewObserverL()
// New observer is added to the storage id listener, also all its store ids
// are registered for observing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEngSIdChangeListener::AddNewObserverL(
    const MDesCArray& aSIds,
    MPEngSIDChangeObserver& aChangeObserver,
    TInt aPriority )
    {
    // check if there is already same listener
    TInt index( 0 );
    if ( KErrNone == FindObserver( aChangeObserver, index ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    // update array
    RPointerArray<MPEngSIdContainer> sidContainers;
    CleanupClosePushL( sidContainers );
    CPEngObserverContainer* newContainer =
        CPEngObserverContainer::NewLC( aChangeObserver,
                                       aPriority );

    TBool update( EFalse );
    TRAPD( e, update = UpdateEngagedSIdsArrayL( aSIds,
                                                sidContainers ) );
    IF_ERROR_ROLLBACK_LEAVE( e );
    User::LeaveIfError( newContainer->UpdateSIdsContainers( sidContainers,
                                                            ETrue ) );

    User::LeaveIfError( AllocateSlotForObserverContainer() );
    if ( update )
        {
        TInt err ( UpdateListenerToServer() );
        IF_ERROR_ROLLBACK_LEAVE( err );
        }
    // this should pass since we allocated slot before
    iObservers.InsertL( newContainer, index );
    CleanupStack::Pop(); // newContainer

    CleanupStack::PopAndDestroy(); // sidContainers
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::UpdateEngagedSIdsArrayL()
// Check if passed SIDs are presented in the array of engaged SIDs
// Transfer array to the server
// array allocation  is also updated
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEngSIdChangeListener::UpdateEngagedSIdsArrayL(
    const MDesCArray& aNewSIds,
    RPointerArray<MPEngSIdContainer>& aSIdContainers )
    {
    TInt newElementCount( 0 );
    // run it from the end, might speed up in cases or ordered arrays
    for ( TInt x ( aNewSIds.MdcaCount() - 1 ) ; x >= 0 ; --x )
        {
        TInt index( KErrNotFound );
        if ( KErrNotFound == FindSIdContainer( aNewSIds.MdcaPoint( x ), index ) )
            {
            TPtrC newSId( aNewSIds.MdcaPoint( x ) );
            CPEngSIdContainer* newSIDContainer =
                CPEngSIdContainer::NewLC( newSId );
            iSIdContainers.InsertL( newSIDContainer, index );
            CleanupStack::Pop(); // newSIDContainer
            newElementCount++;
            // allocate one slot in transfer array and server answer array
            iTransBuffSize += ( newSId.Length() + 2 );
            }
        // increase slot allocation, // just one observer
        User::LeaveIfError( iSIdContainers[ index ]->AllocateSlots( 1 ) );
        aSIdContainers.AppendL( iSIdContainers[ index ] );
        }
    if ( newElementCount )
        {
        newElementCount <<= 1;
        iAnswerBuff.ReAllocL( iAnswerBuff.MaxLength() + newElementCount );
        }
    return newElementCount;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::RebuildTransferArray
// Rebuild transfer array to the presence server
// It is rebuilt from the  array of engaged SIDs.
// This function should not fail, if proper functions
// were used for appending to the Engaged SIds array
// -----------------------------------------------------------------------------
//
TInt CPEngSIdChangeListener::RebuildTransferArray()
    {
    RETURN_IF_ERROR( iTransBuff.ReAlloc( iTransBuffSize ) );
    iTransBuff.Zero();

    TInt count ( iSIdContainers.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        CPEngSIdContainer* container = iSIdContainers[ x ];
        // include SID only if there is observer
        if ( container->ObserverCount() != KErrNone )
            {
            PEngMessagePacker::AppendToDesArray( iTransBuff, container->SId() );
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::UpdateListenerToServer
// Update Set of SIDs to the Presence Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngSIdChangeListener::UpdateListenerToServer()
    {
    // rebuild array
    RETURN_IF_ERROR( RebuildTransferArray() );
    // update data to server
    if ( PEngMessagePacker::DesArrayCount( iTransBuff )  == 0 )
        {
        iNotifyClient.StopListeningSIdsChanges();
        iListenerRunning = EFalse;
        CommitSIdArray();
        }
    else if  ( iListenerRunning )
        {
        RETURN_IF_ERROR( iNotifyClient.UpdateListenSIdsChangesScout(
                             iTransBuff ) );
        CommitSIdArray();
        }
    else
        {
        RETURN_IF_ERROR( iNotifyClient.StartListeningSIdsChanges(
                             iTransBuff ) );
        CommitSIdArray();
        iListenerRunning = ETrue;
        if ( !IsActive() )
            {
            RETURN_IF_ERROR( iNotifyClient.ReloadListenSIdsChangesScout(
                                 iStatus ) );
            SetActive();
            }
        }
    iTransBuff.Close();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::CommitSIdArray
// Commit array of SIDs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::CommitSIdArray()
    {
    for ( TInt x ( iSIdContainers.Count() - 1 ) ; x >= 0 ; x-- )
        {
        if ( iSIdContainers[ x ]->ObserverCount() == 0 )
            {
            delete iSIdContainers[ x ];
            iSIdContainers.Remove( x );
            }
        else
            {
            iSIdContainers[ x ]->SetSIdPublished();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::RollBackSIdArray
// Roll back array of SIDs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::RollBackSIdArray()
    {
    for ( TInt x ( iSIdContainers.Count() - 1 ) ; x >= 0 ; x-- )
        {
        if ( ! iSIdContainers[x]->SIdPublished() )
            {
            delete iSIdContainers[ x ];
            iSIdContainers.Remove( x );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::FindSIdContainer
// Search in Array in array of Engaged SIDs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngSIdChangeListener::FindSIdContainer(
    const TDesC& aSId,
    TInt& aIndex ) const
    {
    TInt l( 0 );
    TInt r( iSIdContainers.Count() );
    TInt ret( KErrNotFound );
    while ( r > l )
        {
        TInt inx = ( l + r ) >> 1;
        TInt k ( aSId.CompareF( iSIdContainers[ inx ]->SId() ) );
        if ( k == 0 )
            {
            aIndex = inx;
            return KErrNone;
            }
        else if ( k > 0 )
            l = inx + 1;
        else
            r = inx;
        }
    aIndex = r;
    return ret;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::FindObserver
// Search in Array of Observers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngSIdChangeListener::FindObserver(
    const MPEngSIDChangeObserver& aObserver,
    TInt& aIndex ) const
    {
    TInt l( 0 );
    TInt r( iObservers.Count() );
    TInt ret( KErrNotFound );
    while ( r > l )
        {
        TInt inx = ( l + r ) >> 1;
        TInt k ( CompareObserversPointers( &aObserver, iObservers[ inx ]->Observer() ) );
        if ( k == 0 )
            {
            aIndex = inx;
            return KErrNone;
            }
        else if ( k > 0 )
            l = inx + 1;
        else
            r = inx;
        }
    aIndex = r;
    return ret;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::UpdateRequired
// Check array of engaged SIDs if there is a need to do update
// to the server what is listened
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEngSIdChangeListener::UpdateRequired()
    {
    TInt count( iSIdContainers.Count() );
    for ( TInt i( 0 ) ; i < count ; i++ )
        {
        if (
            ! iSIdContainers[ i ]->SIdPublished()
            ||
            ( KErrNone ==  iSIdContainers[ i ]->ObserverCount() )
        )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::InformObserverAboutChange()
// Call all observers and inform about SID change if needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::InformObserverAboutChange()
    {
    TInt priority ( MPEngStorageManagerWatcher::EPEngObserverInternal );
    while ( priority
            <=
            MPEngStorageManagerWatcher::EPEngObserverLowPriority
          )
        {
        TInt count ( iObservers.Count() );
        for ( TInt i( count - 1 ) ; i >= 0 ; i-- )
            {
            iObservers[ i ]->NotifySIdsChanges( priority );
            // since as result of notification, observer can
            }
        priority++;
        }
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::InformObserverAboutError
// Call all observers and inform about error in listening
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSIdChangeListener::InformObserverAboutError(
    TInt aErroCode )
    {
    TInt count ( iObservers.Count() );
    for ( TInt i ( count - 1 ) ; i >= 0 ; i-- )
        {
        iObservers[ i ]->NotifySIdListenError( aErroCode );
        }
    }

// -----------------------------------------------------------------------------
// CPEngSIdChangeListener::InformObserverAboutError
// Allocate one slot for the observer container in the array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngSIdChangeListener::AllocateSlotForObserverContainer()
    {
    TInt err ( iObservers.Append( NULL ) );
    if ( err != KErrNone )
        {
        return err;
        }
    iObservers.Remove( iObservers.Count() - 1 );
    return err;
    }

//  End of File
