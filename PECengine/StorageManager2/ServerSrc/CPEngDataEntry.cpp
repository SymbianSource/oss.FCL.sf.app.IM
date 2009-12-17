/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements one store entry of the storage server
*
*/


// INCLUDE FILES
#include "CPEngDataEntry.h"
#include <e32svr.h>
#include <s32mem.h>
#include <f32file.h>

#include "MPEngHandlerListenSIDs.h"
#include "PEngStorageGlobals.h"
#include "PEngStorageServerCommon.h"
#include "PresenceDebugPrint.h"



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEngDataEntry::CPEngDataEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngDataEntry::CPEngDataEntry(
    RFs& aFs,
    const TDesC& aTempFolder,
    TPEngStorageType aType )
        : iFs( aFs ),
        iTempFolder( aTempFolder ),
        // Lock ignore bit is always set to one
        // for local copy of type
        iType(
            static_cast<TPEngStorageType>
            ( aType | EPEngStorageLockIgnored ) )
    {
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::BaseConstructL(
    const TDesC& aKey )
    {
    iKey = aKey.AllocL();
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngDataEntry* CPEngDataEntry::NewL(
    RFs& aFs,
    const TDesC& aTempFolder,
    const TDesC& aKey,
    TPEngStorageType aType )
    {
    CPEngDataEntry* self = NewLC( aFs, aTempFolder, aKey, aType );

    CleanupStack::Pop(); // self

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngDataEntry* CPEngDataEntry::NewLC(
    RFs& aFs,
    const TDesC& aTempFolder,
    const TDesC& aKey,
    TPEngStorageType aType )
    {
    CPEngDataEntry* self = new( ELeave ) CPEngDataEntry( aFs,
                                                         aTempFolder,
                                                         aType );

    CleanupStack::PushL( self );
    self->BaseConstructL( aKey );

    return self;
    }


// Destructor
CPEngDataEntry::~CPEngDataEntry()
    {
    // notify observers about delete
    TInt count( iObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iObservers[ x ]->EntryDeleted( *this );
        }
    iObservers.Reset();
    delete iData;
    delete iKey;
    delete iHashedPath;
    }


// =============================================================================
// =============== New Functions of the base class =============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngDataEntry::DataL()
// -----------------------------------------------------------------------------
//
const TDesC8& CPEngDataEntry::DataL() const
    {
    if ( !iData )
        {
        User::Leave( KErrNotFound );
        }
    return *iData;
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::Key()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngDataEntry::Key() const
    {
    // key cannot be NULL since it has been created at construction
    return *iKey;
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::KeyBuffer()
// -----------------------------------------------------------------------------
//
HBufC** CPEngDataEntry::KeyBuffer()
    {
    return &iKey;
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::LockStatus()
// -----------------------------------------------------------------------------
//
TBool  CPEngDataEntry::LockStatus(
    TPengStorageLockPriority aPriority ) const
    {

    if ( aPriority > iLockPriority )
        {
        return 0;
        }
    return iLockCount;
    }

// -----------------------------------------------------------------------------
// CPEngDataEntry::LockCount()
// -----------------------------------------------------------------------------
//
TInt CPEngDataEntry::LockCount() const
    {
    return iLockCount;
    }

// -----------------------------------------------------------------------------
// CPEngDataEntry::IncrementLockCountL()
// -----------------------------------------------------------------------------
//
TInt CPEngDataEntry::IncreaseLockCountL(
    TInt32 aSessionID,
    TInt aSubSessionID,
    TPengStorageLockPriority aPriority )
    {
    // check the priority first
    if ( aPriority > iLockPriority )
        {
        // update the lock in the way it will replace old lock
        iLockSession = aSessionID;
        iLockSubSession = aSubSessionID;
        iLockPriority = aPriority;
        // 1 as locked once
        iLockCount = 1;
        return iLockCount;
        }
    // priority is not higher, continue
    CheckLockSubSessionL( aSessionID, aSubSessionID );
    if ( iLockCount == 0 )
        {
        iLockSession = aSessionID;
        iLockSubSession = aSubSessionID;
        iLockPriority = aPriority;
        }
    // increase lock count
    iLockCount++;
    return iLockCount;
    }



// -----------------------------------------------------------------------------
// CPEngDataEntry::DecreaseLockCountL()
// -----------------------------------------------------------------------------
//
TInt CPEngDataEntry::DecreaseLockCountL(
    TBool aNotifyActive,
    TInt32 aSessionID,
    TInt aSubSessionID,
    CPEngDataEntry*& aNotifEntry )
    {
    // check Sub Session validity
    CheckLockSubSessionL( aSessionID, aSubSessionID );
    // decrease lock count
    iLockCount--;

    // notify if lock count reached zero
    if ( iLockCount == 0 )
        {
        if ( aNotifyActive )
            {
            NotifyUpdate();
            }
        else
            {
            aNotifEntry = this;
            }
        }

    // Check that lock count is not negative
    iLockCount = ( iLockCount < 0 ? 0 : iLockCount );
    return iLockCount;
    }



// -----------------------------------------------------------------------------
// CPEngDataEntry::Version()
// -----------------------------------------------------------------------------
//
TInt  CPEngDataEntry::Version() const
    {
    return iVersion;
    }



// -----------------------------------------------------------------------------
// CPEngDataEntry::LastModification()
// -----------------------------------------------------------------------------
//
const TTime&  CPEngDataEntry::LastModification() const
    {
    return iModified;
    }



// -----------------------------------------------------------------------------
// CPEngDataEntry::AssertStorageTypeL()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::AssertStorageTypeL( TPEngStorageType aType ) const
    {
    // check type, if it matches, store lock is not included in type check
    if ( ( aType | EPEngStorageLockIgnored ) != iType )
        {
        PENG_DP( D_PENG_LIT( "CPEngDataEntry::AssertStorageTypeL() - type missmatch: aType %d, iType %d" ),
                 aType, iType );

        User::Leave( KErrArgument );
        }
    }



// =============================================================================
// ===== New Functions of the base class  with default implementation===========
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngDataEntry::SetDataLX()
// -----------------------------------------------------------------------------
//
const CPEngDataEntry* CPEngDataEntry::SetDataLX( HBufC8* aNewData,
                                                 TPEngStorageType aType,
                                                 TBool aNotifyActive,
                                                 TInt32 aSessionID,
                                                 TInt aSubSessionID )
    {
    //Verify storage type match
    AssertStorageTypeL( aType );

    // shall lock be obeyed
    if ( !( aType & EPEngStorageLockIgnored ) )
        {
        CheckLockSubSessionL( aSessionID, aSubSessionID );
        }

    CheckVersionNumberL( *aNewData, aType );
    if ( aType & EPEngStorageBasicPermanent )
        {
        // Update first permanent version
        UpdatePermanentPartL( *aNewData );
        }

    delete iData;
    iData = aNewData;

    CleanupStack::Pop(); // aNewData
    UpdateVersionNumber();
    if ( aNotifyActive )
        {
        NotifyUpdate();
        return NULL;
        }
    else
        {
        return this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::DeleteStoreEntry()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::DeleteStoreEntry()
    {
    if ( !( iType & EPEngStorageBasicPermanent ) )
        {
        // nothing to do
        return;
        }
    // delete file from disc, ignore if does not exists
    if ( iHashedPath )
        {
        iFs.Delete( *iHashedPath );
        }
    }



// -----------------------------------------------------------------------------
// CPEngDataEntry::InitializeStoreEntryL()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::SetHashedPath( HBufC* aPath )      // CSI: 60 #
    {
    delete iHashedPath;
    iHashedPath = aPath;
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::InitializeStoreEntryL()
// -----------------------------------------------------------------------------
//
TBool CPEngDataEntry::InitializeStoreEntryL()
    {
    delete iData;
    iData = NULL;
    iVersion = KErrNone;
    iLockCount = EFalse;

    TInt entrySize = KStoreVersionSize;
    HBufC8* permanentData = NULL;

    if ( iType & EPEngStorageBasicPermanent )
        {
        permanentData = PermanentFromFileOrNullL();
        }


    if ( permanentData )
        {
        CleanupStack::PushL( permanentData );
        entrySize += permanentData->Size();

        // permanent data loaded => update version to number 1
        iVersion = ETrue;
        }
    else
        {
        // permanent data not loaded, file does not exists or type
        // does even have permanent part.
        return EFalse;
        }


    //Setup write stream
    CBufFlat* buf = CBufFlat::NewL( entrySize );
    CleanupStack::PushL( buf );

    RBufWriteStream ws;
    CleanupClosePushL( ws );
    ws.Open( *buf );                    // CSI: 65 #

    // Write current version of the entry
    ws.WriteInt32L( iVersion );


    // Go through all types and init ones needed by this entry
    TPEngStorageType currentType( EPEngStorageBasicFirst );
    while ( !( currentType & EPEngStorageBasicLast ) )
        {
        if ( iType & currentType )
            {
            //Write empty part information
            ws.WriteInt32L( currentType );
            ws.WriteInt32L( KErrNotFound );

            //If currently handling permanent part and having permanent data,
            //insert permanent part and update length holder
            if ( ( currentType & EPEngStorageBasicPermanent ) && permanentData )
                {

                MStreamBuf* rawStream = ws.Sink();

                TStreamPos typeStartPos = rawStream->TellL( MStreamBuf::EWrite );
                ws.WriteL( *permanentData );
                TStreamPos typeEndPos = rawStream->TellL( MStreamBuf::EWrite );

                TInt typeLength = typeEndPos - typeStartPos;

                typeStartPos -= 4; //length holder is TInt32 (4 bytes), just before the actual data
                rawStream->SeekL( MStreamBuf::EWrite, typeStartPos );
                ws.WriteInt32L( typeLength );
                rawStream->SeekL( MStreamBuf::EWrite, typeEndPos );
                }
            }

        currentType = static_cast<TPEngStorageType>( currentType << 1 ); // rotate once to left
        }

    iData = buf->Ptr( 0 ).AllocL();
    CleanupStack::PopAndDestroy( 2, buf );


    if ( permanentData )
        {
        CleanupStack::PopAndDestroy( permanentData );
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// =============================================================================
// =============== New Protected Functions of the base class ===================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngDataEntry::AddObserverL()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::AddObserverL(
    MPEngHandlerListenSIDs& aObserver )
    {
    TInt err( iObservers.InsertInAddressOrder( &aObserver ) );
    err = err == KErrAlreadyExists ? KErrNone : err;
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CPEngDataEntry::RemoveObserver()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::RemoveObserver(
    MPEngHandlerListenSIDs& aObserver )
    {
    TInt index( iObservers.FindInAddressOrder( &aObserver ) );
    if ( KErrNotFound != index )
        {
        iObservers.Remove( index );
        }
    }

// =============================================================================
// =============== New Protected Functions of the base class ===================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngDataEntry::CheckVersionNumberL()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::CheckVersionNumberL(
    const TDesC8& aNewData,
    TPEngStorageType aType )
    {
    if ( !( aType & EPEngStorageFlagVersionChecked ) )
        {
        return;
        }


    // check version
    RDesReadStream drs( aNewData );
    CleanupClosePushL( drs );
    TInt clVersion = drs.ReadInt32L();
    CleanupStack::PopAndDestroy(); //drs


    // client version will be by 1 ahead, increased by manager
    clVersion--;
    if ( ( iVersion != clVersion ) )
        {
        // leave with error, Session will complete rest
        User::Leave( KErrAccessDenied );
        }
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::CheckLockSubSessionL()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::CheckLockSubSessionL(
    TInt32 aSessionID,
    TInt aSubSessionID )
    {
    if ( ( iLockCount > 0 )
         && ( ( aSessionID != iLockSession )
              || ( aSubSessionID != iLockSubSession ) ) )
        {
        User::Leave( KErrLocked );
        }
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::UpdateVersionNumber()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::UpdateVersionNumber()
    {
    iVersion = iVersion % KMaxTInt + 1;
    }



// =============================================================================
// =============== New Private Functions of the base class =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngDataEntry::UpdatePermanentPartL()
// Updates permanent part on the disc
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::UpdatePermanentPartL( const TDesC8& aNewData )
    {
    RDesReadStream drs( aNewData );
    CleanupClosePushL( drs );

    drs.ReadInt32L(); // store version of the entry
    TInt permanentDataLength = KErrNotFound;

    //Try locate permanent part
    TrySeekPermanentPartL( drs, permanentDataLength );

    //Store permanent part if having one
    if ( permanentDataLength > 0 )
        {
        HBufC8* permanentDataBuf = HBufC8::NewLC( permanentDataLength );
        TPtr8 permanentData( permanentDataBuf->Des() );
        drs.ReadL( permanentData, permanentDataLength );


        // open temp file, write date into it and rename it to the correct file
        RFile temp;
        TFileName fName;
        User::LeaveIfError( temp.Temp( iFs,
                                       iTempFolder,
                                       fName,
                                       EFileShareExclusive ) );
        CleanupClosePushL( temp );
        temp.Write( permanentData );
        TInt err( iFs.Delete( *iHashedPath ) );
        err =  err == KErrNotFound ? KErrNone : err;
        User::LeaveIfError( err );
        User::LeaveIfError( temp.Flush() );
        CleanupStack::PopAndDestroy(); // temp
        User::LeaveIfError( iFs.Rename( fName, * iHashedPath ) ) ;

        CleanupStack::PopAndDestroy( permanentDataBuf );
        }

    CleanupStack::PopAndDestroy(); //drs
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::TrySeekPermanentPartL()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::TrySeekPermanentPartL( RReadStream& aStream,
                                            TInt& aPermanentDataLength ) const
    {
    TPEngStorageType currentType( EPEngStorageBasicFirst );
    while ( !( currentType & EPEngStorageBasicLast ) )
        {
        TInt partType = aStream.ReadInt32L();
        if ( partType & EPEngStorageBasicPermanent )
            {
            // quite from loop, we have it
            aPermanentDataLength = aStream.ReadInt32L();
            break;
            }
        else
            {
            //proceed to next part
            TInt partLength = aStream.ReadInt32L();
            aStream.ReadL( partLength );
            }

        currentType = static_cast<TPEngStorageType>( currentType << 1 ); // rotate once to left
        }
    }


// -----------------------------------------------------------------------------
// CPEngDataEntry::PermanentFromFileOrNullL()
// -----------------------------------------------------------------------------
//
HBufC8* CPEngDataEntry::PermanentFromFileOrNullL() const
    {
    // if there is no hashed path, return NULL
    if ( !iHashedPath )
        {
        return NULL;
        }

    HBufC8* permanentData = NULL;
    RFile file;

    TInt err ( file.Open( iFs, *iHashedPath, EFileRead ) );
    if ( err == KErrNotFound )
        {
        permanentData = NULL;
        }

    else
        {
        User::LeaveIfError( err );
        CleanupClosePushL( file );
        TInt size;
        User::LeaveIfError( file.Size( size ) );

        permanentData = HBufC8::NewLC( size );
        TPtr8 permanentPtr = permanentData->Des();
        User::LeaveIfError( file.Read( permanentPtr ) );

        CleanupStack::Pop( permanentData );
        CleanupStack::PopAndDestroy(); //file
        }

    return permanentData;
    }

// -----------------------------------------------------------------------------
// CPEngDataEntry::NotifyUpdate()
// -----------------------------------------------------------------------------
//
void CPEngDataEntry::NotifyUpdate()
    {
    TInt count( iObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iObservers[ x ]->EntryUpdated( *this );
        }
    }


//  End of File
