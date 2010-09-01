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
* Description:  One Storage folder management
*
*/



// INCLUDE FILES
#include    <e32std.h>

#include <f32file.h>

#include    "CPEngStorageFolder.h"
#include    "PEngInternalGlobalConsts.h"

// Data entries
#include    "CPEngDataEntry.h"

// Request Handlers
#include    "CPEngRequestHandler.h"
#include    "CPEngHandlerListenSIDs.h"

#include    "CPEngSessionSlotState.h"
#include    "CPEngSessionSlotId.h"

// Clients message
#include    "RPEngMessage.h"

// Storages globals
#include    "PEngStorageGlobals.h"

// Hash tools
#include    "PEngHashTool.h"

#include    "TPEngStorageServerBTreeKeys.h"

//  Debug prints
#include    "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStorageFolder::CPEngStorageFolder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngStorageFolder::CPEngStorageFolder(
    RFs& aFs,
    const TDesC& aTempFolder,
    RBuf16& aCommBuff )
        : iFs( aFs ),
        iTempFolder( aTempFolder ),
        iBuffer( aCommBuff ),
        iDataEntryTree( EBtreeSecure )
    {

    }


// -----------------------------------------------------------------------------
// CPEngStorageFolder::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::ConstructL( )
    {
    iObservers = CObjectCon::NewL();
    // we need to put some number to the CObjectCon::iUniqueID so it takes ownerships
    iObservers->iUniqueID = reinterpret_cast<TInt>( this );

    iPool = CMemPagePool::NewL();
    iDataEntryTree.Connect( iPool, &iDataEntryTreeKey );
    //client must track if the tree is connected or not, if it is not connected,
    //you MUST NOT call for example ResetL, because it panics!!
    iTreeConnected = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngStorageFolder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageFolder* CPEngStorageFolder::NewL(
    RFs& aFs,
    const TDesC& aTempFolder,
    RBuf16& aCommBuff )
    {
    CPEngStorageFolder* self = NewLC( aFs,
                                      aTempFolder,
                                      aCommBuff );
    CleanupStack::Pop(); // self
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngStorageFolder::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageFolder* CPEngStorageFolder::NewLC(
    RFs& aFs,
    const TDesC& aTempFolder,
    RBuf16& aCommBuff )
    {
    CPEngStorageFolder* self = new( ELeave ) CPEngStorageFolder(
        aFs,
        aTempFolder,
        aCommBuff );
    CleanupClosePushL( *self );
    self->ConstructL( );
    return self;
    }


// Destructor
CPEngStorageFolder::~CPEngStorageFolder()
    {
    // clean tree, ignore error, since thread will be closed anyway
    TRAP_IGNORE( CleanTreeFromAllEntriesL() );

    delete iSessionState;
    delete iPool;

    if ( iObservers )
        {
        for ( TInt x( iObservers->Count() - 1 ) ; x >= 0  ; --x )
            {
            ( *iObservers )[ x ]->Close();
            }
        }

    delete iObservers;
    delete iSessionFolder;


#if _BullseyeCoverage
    cov_write();
#endif
    }



// =============================================================================
// =============== Functions from CObject base class ===========================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngStorageFolder::Close()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::Close ()
    {
    if ( iSessionState->SessionSlotState() == EPEngNWPresenceSessionClosed )
        {
        CObject::Close();
        }
    else
        {
        Dec();
        }
    }


// =============================================================================
// =============== Functions from MPEngStorageFolder ===========================
// =============================================================================
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngStorageFolder::DataEntryL()
// -----------------------------------------------------------------------------
//
CPEngDataEntry* CPEngStorageFolder::DataEntryL(
    const TDesC& aSId )
    {
    TBtreePos pos;
    if ( !iTreeConnected )
        {
        User::Leave( KErrNotReady );
        }

    CPEngDataEntry* entry = CheckBufferedDataEntries( aSId );
    if ( entry )
        {
        return entry;
        }

    if ( !iDataEntryTree.FindL( pos, const_cast<TDesC*> ( &aSId ) ) )
        {
        return NULL;
        }
    else
        {
        iDataEntryTree.ExtractAtL( pos, entry );
        }
    AddBufferedDataEntry( entry );
    return entry;
    }


// =============================================================================
// =============== New Public Functions/ Server used ===========================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngStorageFolder::SessionSlotState()
// -----------------------------------------------------------------------------
//
CPEngSessionSlotState& CPEngStorageFolder::SessionSlotState()
    {
    return *iSessionState;
    }


// -----------------------------------------------------------------------------
// CPEngStorageFolder::SetSessionSlotState()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::SetSessionSlotState( CPEngSessionSlotState& aState )
    {
    delete iSessionState;
    iSessionState = &aState;
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::SetSessionFolder()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::SetSessionFolder( HBufC& aFolder )
    {
    delete iSessionFolder;
    iSessionFolder = &aFolder;
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::CommiteStateUpdate()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::CommiteStateUpdate()
    {
    if ( iSessionState->SessionSlotState() != EPEngNWPresenceSessionClosed )
        {
        // nothing to do, session is active
        return;
        }
    // session is closed, check reference count and clean cached data
    // inform all observers about change
    NofifyAllAboutAll();
    if ( 0 == AccessCount() )
        {
        delete this;
        }
    else
        {
        TRAP_IGNORE( CleanTreeFromAllEntriesL() );
        }
    }

// =============================================================================
// =============== New public Functions / Store Engine =========================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngStorageFolder::WriteStoreEntry()
// -----------------------------------------------------------------------------
//
const CPEngDataEntry* CPEngStorageFolder::WriteStoreEntryL(
    const RPEngMessage& aMessage,
    TBool aNotifyActive,
    TUint32 aSessionId,
    TInt aSubSessionId )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageFolder::WriteStoreEntryL()" ) );
    // Etrue, that it should be created if it does not exists
    TBtreePos pos;
    CPEngDataEntry* storeEntry = GetStoreEntryL( aMessage, ETrue, pos );
    HBufC8* clientData = aMessage.ReadOneDescriptor8LC( KMessageSlot2 );

    return storeEntry->SetDataLX( clientData,
                                  static_cast<TPEngStorageType>( aMessage.Int1() ),
                                  aNotifyActive,
                                  aSessionId,
                                  aSubSessionId );
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::ReadStoreEntry()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::ReadStoreEntryL(
    const RPEngMessage& aMessage )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageFolder::ReadStoreEntryL()" ) );

    // get store entry
    // do not create if does not exists
    TBtreePos pos;
    CPEngDataEntry* storeEntry = GetStoreEntryL( aMessage, EFalse, pos );

    storeEntry->AssertStorageTypeL( static_cast<TPEngStorageType>( aMessage.Int1() ) );

    //  write data to the client side, trap error
    //  if not enough space on client side
    TInt e ( aMessage.WriteOneDescriptor( KMessageSlot2, storeEntry->DataL() ) );
    if ( e == KErrOverflow )
        {
        // complete message with the length
        aMessage.Complete( storeEntry->DataL().Length() );
        // return so message is not completed twice
        return;
        }

    // leave if any error
    User::LeaveIfError( e );
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::GetStoryLength()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::GetStoreEntryLengthL(
    const RPEngMessage& aMessage )
    {
    // do not create story entry if it does not exist
    TBtreePos pos;
    aMessage.Complete( GetStoreEntryL( aMessage, EFalse, pos )->DataL().Length() );
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::RemoveStoreEntry()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::RemoveStoreEntryL(
    const RPEngMessage& aMessage )
    {
    // load if exists, do not create
    TBtreePos pos;
    // we actually need position from tree, so true search
    CPEngDataEntry* storeEntry = GetStoreEntryL( aMessage, EFalse, pos, ETrue );
    storeEntry->DeleteStoreEntry();
    // remove store entry from tree
    iDataEntryTree.DeleteAtL( pos );
    // removed from tree, not delete instance
    delete storeEntry;
    // remove buffered pointers of deleted store entry
    if ( iBuffDataEntry0 == iBuffDataEntry1 )
        {
        iBuffDataEntry1 = NULL;
        }
    if ( iBuffDataEntry0 == iBuffDataEntry2 )
        {
        iBuffDataEntry2 = NULL;
        }
    iBuffDataEntry0 = NULL;
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::LockStoreEntry()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::LockStoreEntryL(
    const RPEngMessage& aMessage,
    TUint32 aSessionId,
    TInt aSubSessionId )
    {
    // load if exists, or create if does not exists
    TBtreePos pos;
    TInt priorityInt( aMessage.Int2() );
    TBool createEntry = priorityInt & KLockEntryCreateMask;
    CPEngDataEntry* storeEntry = GetStoreEntryL( aMessage, createEntry, pos );
    priorityInt &= ~KLockEntryCreateMask;
    TPengStorageLockPriority priority (
        static_cast<TPengStorageLockPriority> ( priorityInt ) );

    aMessage.Complete( storeEntry->IncreaseLockCountL( aSessionId,
                                                       aSubSessionId,
                                                       priority ) );
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::UnlockStoreEntry()
// -----------------------------------------------------------------------------
//
const CPEngDataEntry* CPEngStorageFolder::UnlockStoreEntryL(
    const RPEngMessage& aMessage,
    TBool aNotifyActive,
    TUint32 aSessionId,
    TInt aSubSessionId )
    {
    // load if exists, do not create if does not exists
    TBtreePos pos;
    CPEngDataEntry* storeEntry = GetStoreEntryL( aMessage, EFalse, pos );

    // check current lock count, so we avoid hasty notifications
    if ( !storeEntry->LockCount() )
        {
        // item is not locked at all, complete with zero
        aMessage.Complete( 0 );
        return NULL;
        }

    // release lock by one
    CPEngDataEntry* notifEntry = NULL;
    TInt lockCount( storeEntry->DecreaseLockCountL( aNotifyActive,
                                                    aSessionId,
                                                    aSubSessionId,
                                                    notifEntry ) );
    aMessage.Complete( lockCount );
    return NULL;
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::IsStoreEntryLocked()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::IsStoreEntryLockedL(
    const RPEngMessage& aMessage )
    {
    TBtreePos pos;
    CPEngDataEntry* volatile storeEntry = NULL;
    TRAPD( e, storeEntry = GetStoreEntryL( aMessage, EFalse, pos ) );
    if ( e == KErrNotFound )
        {
        // if does not exists in store, also not locked
        aMessage.Complete( KErrNone );
        return;
        }
    // other error leave
    User::LeaveIfError( e );
    aMessage.Complete( storeEntry->LockStatus(
                           static_cast<TPengStorageLockPriority>( aMessage.Int2() ) ) );
    }



// =============================================================================
// =============== New public Functions / Notify Engine ========================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngStorageFolder::StartListenSIDsChanges()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::ListenSIDsChangesL(
    const RPEngMessage& aMessage,
    TUint32 aSessionId,
    TInt aSubSessionId )
    {
    CPEngRequestHandler* reqHandler = FindRequestHandler( aSessionId, aSubSessionId,
                                                          EFolderSubSessListenSIDsChanges );
    if ( reqHandler )
        {
        // this handler already exists, not allowed by multiple
        User::Leave( KErrAlreadyExists );
        }

    // create new handler
    reqHandler = CPEngHandlerListenSIDs::NewLC( *this,
                                                aMessage,
                                                aSessionId,
                                                aSubSessionId );
    // store new handler
    iObservers->AddL( reqHandler );
    CleanupStack::Pop(); // newHandler
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::UpdateListenSIDsScout()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::UpdateListenSIDsScoutL(
    const RPEngMessage& aMessage,
    TUint32 aSessionId,
    TInt aSubSessionId )
    {
    CPEngRequestHandler* reqHandler = FindRequestHandler( aSessionId, aSubSessionId,
                                                          static_cast<TPEngStorageServerMessages> ( aMessage.Int0() ) );
    if ( !reqHandler )
        {
        // there is not such a request to be reloaded
        User::Leave( KErrArgument );
        }
    reqHandler->ReloadScoutWithNewMessageL( aMessage );
    }

void CPEngStorageFolder::FetchChangedIdsIndexesL(
    const RPEngMessage& aMessage,
    TUint32 aSessionId,
    TInt aSubSessionId )
    {
    CPEngRequestHandler* reqHandler = FindRequestHandler( aSessionId, aSubSessionId,
                                                          static_cast<TPEngStorageServerMessages> ( aMessage.Int0() ) );
    if ( !reqHandler )
        {
        // there is not such a request to be reloaded
        User::Leave( KErrArgument );
        }
    reqHandler->ReloadScoutWithNewMessageL( aMessage );
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::ReloadAsyncScout()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::ReloadAsyncScoutL(
    const RPEngMessage& aMessage,
    TUint32 aSessionId,
    TInt aSubSessionId )
    {
    CPEngRequestHandler* reqHandler = FindRequestHandler( aSessionId, aSubSessionId,
                                                          static_cast<TPEngStorageServerMessages> ( aMessage.Int0() ) );
    if ( !reqHandler )
        {
        // there is not such a request to be reloaded
        User::Leave( KErrArgument );
        }
    reqHandler->ReloadScoutWithNewMessageL( aMessage );
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::CancelSubSessionScouts()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::CancelSubSessionRequests(
    TUint32 aSessionId,
    TInt aSubSessionId )
    {
    for ( TInt x( iObservers->Count() - 1 ) ; x >= 0 ; --x )
        {
        CPEngRequestHandler* reqHandler =
            static_cast<CPEngRequestHandler*>( ( *iObservers )[ x ] );
        if (
            ( reqHandler->SessionId() == aSessionId )
            &&
            ( reqHandler->SubSessionId() == aSubSessionId )
        )
            {
            reqHandler->CancelRequestD();
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::CancelRequest()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::CancelRequest(
    const RPEngMessage& aMessage,
    TUint32  aSessionId,
    TInt aSubSessionId )
    {
    CPEngRequestHandler* reqHandler = FindRequestHandler( aSessionId, aSubSessionId,
                                                          static_cast<TPEngStorageServerMessages> ( aMessage.Int0() ) );
    // ignore if such a request does not exists
    if ( reqHandler )
        {
        reqHandler->CancelRequestD();
        }
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::NotifyChangedStorageIdL()
// -----------------------------------------------------------------------------
//
const CPEngDataEntry* CPEngStorageFolder::NotifyChangedStorageIdL(
    const RPEngMessage& aMessage,
    TBool aNotifyActive )
    {
    // find entry
    TBtreePos pos;
    CPEngDataEntry* storeEntry = GetStoreEntryL( aMessage, ETrue, pos );

    if ( aNotifyActive )
        {
        // better to go through array from end
        for ( TInt x( iObservers->Count() - 1 ) ; x >= 0 ; --x )
            {
            static_cast<CPEngHandlerListenSIDs*>
            ( ( *iObservers )[ x ] )->EntryUpdated( *storeEntry );
            }
        storeEntry = NULL;
        }

    //In other cases let the caller to do the notification delayed
    return storeEntry;
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::NotifySIDChanges()
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::NotifySIDChanges(
    const RPointerArray<CPEngDataEntry>& aEntries )
    {
    // better to go through array from end
    for ( TInt x( iObservers->Count() - 1 ) ; x >= 0 ; --x )
        {
        static_cast<CPEngHandlerListenSIDs*>
        ( ( *iObservers )[ x ] )->NotifyChangedSIDs( aEntries ) ;
        }
    }


// =============================================================================
// =============== New private Functions of the class ==========================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngStorageFolder::FindRequestHandler()
// Find Request Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngRequestHandler* CPEngStorageFolder::FindRequestHandler(
    TUint32 aSessionId,
    TInt aSubSession,
    TPEngStorageServerMessages aFunction )
    {
    TInt count( iObservers->Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        CPEngRequestHandler* reqHandler = static_cast<CPEngRequestHandler*>( ( *iObservers )[ x ] );
        if ( ( reqHandler->SessionId() == aSessionId )
             && ( reqHandler->SubSessionId() == aSubSession )
             && ( reqHandler->RequestFunction() == aFunction ) )
            {
            return reqHandler;
            }
        }
    return NULL;
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::GetStoreEntryL()
// Get Store Entry instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngDataEntry* CPEngStorageFolder::GetStoreEntryL(
    const RPEngMessage& aMessage,
    TBool aCreateIfNotPressent,
    TBtreePos& aPos,
    TBool aTrueSearch /* = EFalse */ )
    {
    // read Store ID and type from message
    aMessage.ReadOneDescriptorL( KMessageSlot0, iBuffer );
    PENG_DP( D_PENG_LIT( "CPEngStorageFolder::GetStoreEntryL():%S" ), &iBuffer );

    if ( !iTreeConnected )
        {
        User::Leave( KErrNotReady );
        }

    CPEngDataEntry* entry = NULL;
    if ( !aTrueSearch )
        {
        entry = CheckBufferedDataEntries( iBuffer );
        }

    if ( entry )
        {
        return entry;
        }

    if ( !iDataEntryTree.FindL( aPos, &iBuffer ) )
        {
        // not in the tree, try to load it, it will be also added to the tree
        entry = LoadStoreEntryL( iBuffer, aCreateIfNotPressent, aPos, aMessage );
        }
    else
        {
        iDataEntryTree.ExtractAtL( aPos, entry );
        }
    AddBufferedDataEntry( entry );
    return entry;
    }



// -----------------------------------------------------------------------------
// CPEngStorageFolder::LoadStoreEntryL()
// Load Store Entry from disc if it exists
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngDataEntry* CPEngStorageFolder::LoadStoreEntryL(
    TDesC& aKey,
    TBool aCreateIfNotPressent,
    TBtreePos& aPos,
    const RPEngMessage& aMessage )
    {
    // add it into the tree if entry will be created
    TPEngStorageType type( static_cast<TPEngStorageType>( aMessage.Int1() ) );

    CPEngDataEntry* entry = CPEngDataEntry::NewLC(
                                iFs,
                                iTempFolder,
                                aKey,
                                type );

    HBufC* path = NULL;
    if ( type & EPEngStorageBasicPermanent )
        {
        // create Hashed path for the file
        path = PEngHashTool::HashDescriptorL( aKey );
        CleanupStack::PushL( path );

        path = path->ReAllocL( path->Length() +
                               iSessionFolder->Length() );

        CleanupStack::Pop(); // new pointer was received, old buffer deleted
        // folder delimiter was already there
        path->Des().Insert( 0, *iSessionFolder );
        entry->SetHashedPath( path );
        }

    if ( !( entry->InitializeStoreEntryL() || aCreateIfNotPressent ) )
        {
        User::Leave( KErrNotFound );
        }

    // add entry to the tree
    if ( !iDataEntryTree.InsertL( aPos, entry ) )
        {
        User::Leave( KErrGeneral );
        }

    // notify SIDs observers about new added entry
    // better to go through array from end
    for ( TInt x( iObservers->Count() - 1 ) ; x >= 0 ; --x )
        {
        static_cast<CPEngHandlerListenSIDs*>
        ( ( *iObservers )[ x ] )->EntryCreatedL( *entry );
        }

    CleanupStack::Pop( entry );

    return entry;
    }

// -----------------------------------------------------------------------------
// CPEngStorageFolder::NofifyAllAboutAll()
// Notify observer that all observer SIds changed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::NofifyAllAboutAll()
    {
    // better to go through array from end
    for ( TInt x( iObservers->Count() - 1 ) ; x >= 0 ; --x )
        {
        static_cast<CPEngHandlerListenSIDs*>
        ( ( *iObservers )[ x ] )->NotifyAllSIdsChanged();
        }
    }

// -----------------------------------------------------------------------------
// CPEngStorageFolder::NofifyAllAboutAll()
// Look in buffered data entries for data entry with desired key
// -----------------------------------------------------------------------------
//
CPEngDataEntry* CPEngStorageFolder::CheckBufferedDataEntries(
    const TDesC& aKey )
    {
    if ( iBuffDataEntry0 && KErrNone == iBuffDataEntry0->Key().CompareF( aKey ) )
        {
        PENG_DP( D_PENG_LIT(
                     "CPEngStorageFolder::CheckBufferedDataEntries(0) Search skipped:%S" ),
                 &aKey );
        return iBuffDataEntry0;
        }

    if ( iBuffDataEntry1 && KErrNone == iBuffDataEntry1->Key().CompareF( aKey ) )
        {
        PENG_DP( D_PENG_LIT(
                     "CPEngStorageFolder::CheckBufferedDataEntries(1) Search skipped:%S" ),
                 &aKey );
        CPEngDataEntry* entry = iBuffDataEntry1;
        iBuffDataEntry1 = iBuffDataEntry0;
        iBuffDataEntry0 = entry;
        return entry;
        }

    if ( iBuffDataEntry2 && KErrNone == iBuffDataEntry2->Key().CompareF( aKey ) )
        {
        PENG_DP( D_PENG_LIT(
                     "CPEngStorageFolder::CheckBufferedDataEntries(2) Search skipped:%S" ),
                 &aKey );
        AddBufferedDataEntry( iBuffDataEntry2 );
        return iBuffDataEntry0;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPEngStorageFolder::AddBufferedDataEntry()
// Add buffered data entry pointer
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::AddBufferedDataEntry(
    CPEngDataEntry* aDataEntry )
    {
    iBuffDataEntry2 = iBuffDataEntry1;
    iBuffDataEntry1 = iBuffDataEntry0;
    iBuffDataEntry0 = aDataEntry;
    }


// -----------------------------------------------------------------------------
// CPEngStorageFolder::CleanTreeFromAllEntriesL()
// Clean B tree from all entries
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageFolder::CleanTreeFromAllEntriesL()
    {
    if ( !iTreeConnected )
        {
        User::Leave( KErrNotReady );
        }
    TBtreeMark pos;
    TBool b = iDataEntryTree.ResetL( pos );
    iBuffDataEntry0 = NULL;
    iBuffDataEntry1 = NULL;
    iBuffDataEntry2 = NULL;
    while ( b )
        {
        CPEngDataEntry* entry;
        iDataEntryTree.ExtractAtL( pos, entry );
        if ( entry )
            {
            delete entry;
            }
        b = iDataEntryTree.NextL( pos );
        }
    iDataEntryTree.ClearL();
    }


//  End of File
