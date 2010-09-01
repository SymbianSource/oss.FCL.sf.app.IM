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
* Description:  Implementation of class CPEngStorageManager
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <s32mem.h>
#include "CPEngStorageManager.h"
#include "CPEngStoreEntry.h"
#include "PEngHashTool.h"
#include "PEngStorageGlobals.h"
#include "PEngStorageServerCommon.h"
#include "MPEngStorageManagerWatcher.h"
#include "CPEngSessionSlotId.h"

#include "PEngMessagePacker.h"

//  Debug prints
#include    "PresenceDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStorageManager::CPEngStoreEntry
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngStorageManager::CPEngStorageManager(
    MPEngStorageManagerWatcher& aWatcher )
        :   iAccessCount( 1 ), // init count to 1
        iStorageManagerWatcher( aWatcher ),
        iSessionSlot( aWatcher.SessionSlotId() )
    {
    // Increase handler count
    iStorageManagerWatcher.Open();      // CSI: 65 #
    }

// -----------------------------------------------------------------------------
// CPEngStorageManager::BaseConstructL
// Symbian 2nd phase Base constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngStorageManager::ConstructL(
    RPEngStorageClient& aStorageMainClient )
    {
    // connect to the Server with the Sub Session
    HBufC8* sessionName = iSessionSlot.PackLC();
    User::LeaveIfError( iSubFolderClient.Connect( aStorageMainClient, *sessionName ) );
    CleanupStack::PopAndDestroy(); // sessionName
    }

// -----------------------------------------------------------------------------
// CPEngStorageManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageManager* CPEngStorageManager::NewL(
    MPEngStorageManagerWatcher& aWatcher,
    RPEngStorageClient& aStorageMainClient )
    {
    CPEngStorageManager* self = CPEngStorageManager::NewLC( aWatcher, aStorageMainClient );

    CleanupStack::Pop( ); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CPEngStorageManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageManager* CPEngStorageManager::NewLC(
    MPEngStorageManagerWatcher& aWatcher,
    RPEngStorageClient& aStorageMainClient )
    {
    CPEngStorageManager* self =
        new ( ELeave ) CPEngStorageManager( aWatcher );

    CleanupClosePushL( *self );
    self->ConstructL( aStorageMainClient );

    return self;
    }

// Destructor
CPEngStorageManager::~CPEngStorageManager()
    {
    // Use same Panic as CObject is using for Reference counted objects
    __ASSERT_ALWAYS( iAccessCount == 0,
                     User::Panic( KPEngSMEuserCBasePanic,
                                  KPEngSMEuserCBasePanicReason ) );

    iSubFolderClient.Close();
    // close handle to watcher
    iStorageManagerWatcher.Close();
    }


// -----------------------------------------------------------------------------
// ==== Functions from the MPEngStorageManager base classes ====================
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngStorageManager::Open()
// -----------------------------------------------------------------------------
//
void CPEngStorageManager::Open()
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::Close()
// -----------------------------------------------------------------------------
//
void CPEngStorageManager::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 )
        {
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::StoreL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManager::StoreL( const CPEngStoreEntry& aStoreEntry )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::StoreL() [%S][%d]" ),
             &aStoreEntry.StorageId(), aStoreEntry.StorageType() );

    // Etrue to obey lock of the store entry
    StoreEntryL( aStoreEntry, ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::StoreExclusiveL()
// -----------------------------------------------------------------------------
//
void CPEngStorageManager::StoreExclusiveL( const CPEngStoreEntry& aStoreEntry )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::StoreExclusiveL() [%S][%d]" ),
             &aStoreEntry.StorageId(), aStoreEntry.StorageType() );


    // EFalse to obey lock of the store entry
    StoreEntryL( aStoreEntry, EFalse );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::RetrieveL()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManager::RetrieveL( CPEngStoreEntry& aStoreEntry,
                                     TBool aObeyVersion /* ETrue */ )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::RetrieveL() [%S][%d]" ),
             &aStoreEntry.StorageId(), aStoreEntry.StorageType() );

    // Get Entry size for all parts
    TPEngStorageType entryType ( aStoreEntry.StorageType() );
    // Initialize size with some room for version numbers
    TUint32 entrySize( KStoreVersionSize + aStoreEntry.EntrySize() );
    if ( entrySize <= 0 )
        {
        entrySize = KStoreEntryDefaultSize;
        }
    // do not believe and correct entry size
    entrySize += entrySize / KStoreEntryRetrieveOverlap;

    // create buffer with estimated size
    HBufC8* buffer = HBufC8::NewLC( entrySize );
    TPtr8 ptr = buffer->Des();

    // get Buffer from Server, positive response is size of needed buffer, it it's too small
    TInt err( 1 ); // to make at least one round in following loop
    while ( err > KErrNone )
        {
        err = iSubFolderClient.Retrieve( aStoreEntry.StorageId(),
                                         aStoreEntry.StorageType(),
                                         ptr );

        // check if we need to realloc buffer
        if ( err > KErrNone )
            {
            // realloc buffer and once more, be careful with Clean up stack
            CleanupStack::PopAndDestroy(); // buffer
            buffer = HBufC8::NewLC( err );
            ptr.Set( buffer->Des() );
            }
        }

    if ( err == KErrNotFound )
        {
        CleanupStack::PopAndDestroy(  ); // buffer
        // set store Version back to the Zero,so following store wont fail
        aStoreEntry.iEntryVersion = KErrNone;
        return KErrNotFound;
        }

    User::LeaveIfError( err );

    RDesReadStream drs( buffer->Des() );
    CleanupClosePushL( drs );

    // store version of the entry
    TInt32 retrievedVersion = drs.ReadInt32L();

    // check if internalizing is even necessary
    if ( aObeyVersion && ( retrievedVersion == aStoreEntry.iEntryVersion ) )
        {
        CleanupStack::PopAndDestroy( 2 );//buffer, drs
        return KErrNone;
        }

    entrySize = ptr.Size();

    // Internalize all parts of the store entry
    TPEngStorageType currentType( EPEngStorageBasicFirst );
    while ( !( currentType & EPEngStorageBasicLast ) )
        {
        if ( entryType & currentType )
            {
            TInt strmPartType = drs.ReadInt32L();
            TInt strmPartLength = drs.ReadInt32L();

            if ( currentType != strmPartType )
                {
                PENG_DP( D_PENG_LIT( "CPEngStorageManager::RetrieveL() [%S][%d] - type order mismatch: strmPartType %d currentType %d" ),
                         &aStoreEntry.StorageId(), aStoreEntry.StorageType(), strmPartType, currentType );
                User::Panic( KPEngSMPanic, EPEngSM_TypeOrderMismatch );
                }

            if ( strmPartLength != KErrNotFound )
                {
                MStreamBuf* rawStream = drs.Source();
                TStreamPos readStartPos = rawStream->TellL( MStreamBuf::ERead );

                aStoreEntry.InternalizeL( drs, currentType );

                TStreamPos readEndPos = rawStream->TellL( MStreamBuf::ERead );
                TInt readLength = readEndPos - readStartPos;

                // panic client if did not internalize properly
                if ( readLength != strmPartLength )
                    {
                    PENG_DP( D_PENG_LIT( "CPEngStorageManager::RetrieveL() [%S][%d] - internalize length mismatch: readLength %d strmPartLength %d" ),
                             &aStoreEntry.StorageId(), aStoreEntry.StorageType(), strmPartType, currentType );

                    User::Panic( KPEngSMPanic, EPEngSM_InternalizeLengthMismatch );
                    }
                }
            }

        currentType = static_cast<TPEngStorageType>( currentType << 1 ); // rotate once to left
        }

    // update version after successful internalize
    aStoreEntry.iEntryVersion = retrievedVersion;
    aStoreEntry.iSize = entrySize;
    CleanupStack::PopAndDestroy( 2 );//buffer, drs
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::Delete()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManager::Delete(
    const CPEngStoreEntry& aStoreEntry )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::Delete() [%S][%d] - By entry " ),
             &aStoreEntry.StorageId(), aStoreEntry.StorageType() );

    return iSubFolderClient.Delete( aStoreEntry.StorageId(), aStoreEntry.StorageType() );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::Delete()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManager::Delete( const TDesC& aStoreEntryId )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::Delete() [%S] - By entry id" ), &aStoreEntryId );
    // set as type everything
    return iSubFolderClient.Delete( aStoreEntryId , EPEngMixedPermanentCached );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::LockedL()
// -----------------------------------------------------------------------------
//
TBool CPEngStorageManager::LockedL(
    const CPEngStoreEntry& aStoreEntry,
    TPengStorageLockPriority aPriority /*EStorageLockLevelBasic*/ ) const
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::LockedL() [%S][%d] - Priority %d" ),
             &aStoreEntry.StorageId(), aStoreEntry.StorageType(), aPriority );

    TInt ret( iSubFolderClient.StoreEntryLocked( aStoreEntry.StorageId(),
                                                 aStoreEntry.StorageType(),
                                                 aPriority ) );
    User::LeaveIfError( ret );
    return ( ret == 0 ? EFalse : ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::LockedL()
// -----------------------------------------------------------------------------
//
TBool CPEngStorageManager::Locked(
    const CPEngStoreEntry& aStoreEntry,
    TPengStorageLockPriority aPriority /*EStorageLockLevelBasic*/ ) const
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::Locked() [%S][%d] - Priority %d" ),
             &aStoreEntry.StorageId(), aStoreEntry.StorageType(), aPriority );

    TInt ret( iSubFolderClient.StoreEntryLocked( aStoreEntry.StorageId(),
                                                 aStoreEntry.StorageType(),
                                                 aPriority ) );
    return ( ret <= 0 ? EFalse : ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::Lock()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManager::Lock(
    const CPEngStoreEntry& aStoreEntry,
    TPengStorageLockPriority aPriority /*EStorageLockLevelBasic*/,
    TBool aCreateEntry /* = ETrue */ ) const
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::Locked() [%S][%d] - Priority %d" ),
             &aStoreEntry.StorageId(), aStoreEntry.StorageType(), aPriority );

    return iSubFolderClient.LockStoreEntry( aStoreEntry.StorageId(),
                                            aStoreEntry.StorageType(),
                                            aPriority,
                                            aCreateEntry );
    }

// -----------------------------------------------------------------------------
// CPEngStorageManager::Unlock()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManager::Unlock( const CPEngStoreEntry& aStoreEntry ) const
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::Unlock() store id: %S, StoreTypeL %d " ),
             &aStoreEntry.StorageId(), aStoreEntry.StorageType() );
    return iSubFolderClient.UnlockStoreEntry( aStoreEntry.StorageId(), aStoreEntry.StorageType() );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::NotifyChangedSId()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManager::NotifyChangedSId( const TDesC& aChangedSId,
                                            TPEngStorageType aSidStorageType )
    {
    return iSubFolderClient.NotifyChangedSIdBlind( aChangedSId, aSidStorageType );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::BufferServerSideNotifications()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManager::BufferServerSideNotifications( TInt aCount )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::BufferServerSideNotifications() - Count %d" ),
             aCount );
    return iSubFolderClient.BufferServerSideNotifications( aCount );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::ReleaseServerSideBuffering()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageManager::ReleaseServerSideBuffering( TInt aCount )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageManager::ReleaseServerSideBuffering() - count %d" ),
             aCount );
    return iSubFolderClient.ReleaseServerSideBuffering( aCount );
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::StorageManagerWatcher()
// -----------------------------------------------------------------------------
//
MPEngStorageManagerWatcher& CPEngStorageManager::StorageManagerWatcher()
    {
    return iStorageManagerWatcher;
    }


// -----------------------------------------------------------------------------
// CPEngStorageManager::SessionSlotId()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngStorageManager::SessionSlotId() const
    {
    return iSessionSlot;
    }


// =============================================================================
// =============== New private Functions of the class ==========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageManager::BaseStoragePath
// Stores Store Entry given as an argument.
// Based on the passed parameter is obeyed lock or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageManager::StoreEntryL( const CPEngStoreEntry& aStoreEntry,
                                       TBool aObeyLock )
    {
    // count how much space wee need for storing
    TUint32 entrySize( KStoreVersionSize + aStoreEntry.EntrySize() );
    // do not believe and correct entry size
    entrySize += entrySize / KStoreEntrySizeTolerance;

    // get buffer for transfer
    CBufFlat* buf = CBufFlat::NewL( entrySize );
    CleanupStack::PushL( buf );
    RBufWriteStream ws;
    CleanupClosePushL( ws );
    ws.Open( *buf );                                // CSI: 65 #

    // get all parts of the store entry, and Externalize them
    TPEngStorageType entryType ( aStoreEntry.StorageType() );
    TPEngStorageType currentType( EPEngStorageBasicFirst );

    // Write current version of the entry, increased by one, for server info
    ws.WriteInt32L( aStoreEntry.iEntryVersion + 1 );

    // Write actual data
    while ( !( currentType & EPEngStorageBasicLast ) )
        {
        if ( entryType & currentType )
            {

            //Write first type and type length "holder"
            ws.WriteInt32L( currentType );
            ws.WriteInt32L( KErrNotFound );

            MStreamBuf* rawStream = ws.Sink();

            //Do the write and cache locally the write start and points
            TStreamPos typeStartPos = rawStream->TellL( MStreamBuf::EWrite );
            aStoreEntry.ExternalizeL( ws, currentType );
            TStreamPos typeEndPos = rawStream->TellL( MStreamBuf::EWrite );

            //Caculate write length
            TInt typeLength = typeEndPos - typeStartPos;

            //Patch the length holder
            //-4 below: Length holder is TInt32 (4 bytes) just before the actual data
            typeStartPos -= 4;
            rawStream->SeekL( MStreamBuf::EWrite, typeStartPos );
            ws.WriteInt32L( typeLength );
            rawStream->SeekL( MStreamBuf::EWrite, typeEndPos );
            }

        // rotate once to left
        currentType = static_cast<TPEngStorageType>( currentType << 1 );
        }

    buf->Compress();
    entrySize = buf->Size();
    // update store type, depends if lock shall be obeyed
    entryType = static_cast<TPEngStorageType>
                ( aObeyLock ?  entryType : ( entryType | EPEngStorageLockIgnored ) );

    // store data to the server
    User::LeaveIfError( iSubFolderClient.Store( aStoreEntry.StorageId(),
                                                entryType , buf->Ptr( 0 ) ) );

    // update version after successful update by 1
    ( const_cast<CPEngStoreEntry&> ( aStoreEntry ) ).iEntryVersion =
        aStoreEntry.iEntryVersion  % KMaxTInt + 1;

    ( const_cast<CPEngStoreEntry&> ( aStoreEntry ) ).iSize = entrySize;

    CleanupStack::PopAndDestroy( 2 );// ws & buffer
    }

//  End of File

