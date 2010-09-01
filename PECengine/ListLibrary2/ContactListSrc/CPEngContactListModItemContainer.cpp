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
* Description:  Container of the one contact list item.
*
*/

// INCLUDE FILES
#include    "PEngContactIdsTools.h"
#include    "CPEngContactListModItemContainer.h"
#include    "MPEngContactListModStore.h"
#include    "CPEngStoreEntry.h"
#include    "CPEngContactListProperty.h"
#include    "PEngListLibraryPanics.h"
#include    <e32std.h>


// CONSTANTS
const TInt KIntStreamBytes = 4;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::CPEngContactListModItemContainer()
// -----------------------------------------------------------------------------
//
CPEngContactListModItemContainer::CPEngContactListModItemContainer(
    MPEngContactListModStore& aStoreEntry )
        : iStoreEntry( aStoreEntry ),
        iAccessCount( 1 ), // initial access count
        iSize( aStoreEntry.StoreSizeCount() ),
        iFresh( EFalse )
    {
    // Initial size of numbers stored for managing the item
    iSize += KIntStreamBytes * 5;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::ConstructL(
    const TDesC& aContactId,
    const TDesC* aNickName )
    {
    iContactId = aContactId.AllocL();
    iSize += aContactId.Length();
    if ( aNickName )
        {
        iNickName = aNickName->AllocL();
        iSize += aNickName->Length();
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::ConstructNewNickL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::ConstructNewNickL(
    const TDesC& aContactId,
    const TDesC& aNewNickName )
    {
    iContactId = aContactId.AllocL();
    iSize +=  aContactId.Length();
    iNewNickName = aNewNickName.AllocL();
    iSize +=  aNewNickName.Length();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListModItemContainer* CPEngContactListModItemContainer::NewLC(
    MPEngContactListModStore& aStoreEntry,
    const TDesC& aContactId,
    const TDesC* aNickName )
    {
    CPEngContactListModItemContainer* self =
        new( ELeave ) CPEngContactListModItemContainer( aStoreEntry );

    CleanupClosePushL( *self );
    self->ConstructL( aContactId, aNickName );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::NewNewNickLC()
// -----------------------------------------------------------------------------
//
CPEngContactListModItemContainer* CPEngContactListModItemContainer::NewNewNickLC(
    MPEngContactListModStore& aStoreEntry,
    const TDesC& aContactId,
    const TDesC& aNewNickName )
    {
    CPEngContactListModItemContainer* self =
        new( ELeave ) CPEngContactListModItemContainer( aStoreEntry );

    CleanupClosePushL( *self );
    self->ConstructNewNickL( aContactId, aNewNickName );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListModItemContainer* CPEngContactListModItemContainer::NewLC(
    MPEngContactListModStore& aStoreEntry,
    RReadStream& aStream )
    {
    CPEngContactListModItemContainer* self =
        new( ELeave ) CPEngContactListModItemContainer( aStoreEntry );

    CleanupClosePushL( *self );
    self->InternalizeL( aStream, EPEngStorageBasicPermanent );

    return self;
    }


// Destructor
CPEngContactListModItemContainer::~CPEngContactListModItemContainer()
    {
    // panic if object is still accessed
    __ASSERT_DEBUG( iAccessCount == 0 , Panic( ERefAccessedObjectDeleted ) );

    // decrease size since object is deleted
    iSize -= KIntStreamBytes * 3;
    if ( iContactId )
        {
        iSize += iContactId->Length();
        }

    delete iContactId;
    delete iNickName;
    delete iNewNickName;

    iProperties.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::Id()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListModItemContainer::Id() const
    {
    return *iContactId;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::NickName()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListModItemContainer::NickName() const
    {
    if ( iNewNickName )
        {
        return *iNewNickName;
        }

    if ( iNickName )
        {
        return *iNickName;
        }

    return KNullDesC;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::UpdateNickNameL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::UpdateNickNameL( const TDesC& aNickName )
    {
    TInt size( iNewNickName ? iNewNickName->Length() : KIntStreamBytes * 2 );
    delete iNewNickName;
    iNewNickName = NULL;

    if ( !iNickName || ( KErrNone != iNickName->Compare( aNickName ) ) )
        {
        iNewNickName = aNickName.AllocL();
        size -= iNewNickName->Length();
        }

    iSize -= size;
    iStoreEntry.StoreEntryL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModItemContainer::GetProperty( TUint aName,
                                                    TUint aLevel,
                                                    TInt& aValue ) const

    {
    return PEngPropertyManager::GetProperty( iProperties,
                                             aName,
                                             aLevel,
                                             aValue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModItemContainer::GetProperty( TUint aName,
                                                    TUint aLevel,
                                                    TPtrC8& aValue ) const
    {
    return PEngPropertyManager::GetProperty( iProperties,
                                             aName,
                                             aLevel,
                                             aValue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModItemContainer::GetProperty( TUint aName,
                                                    TUint aLevel,
                                                    TPtrC16& aValue ) const
    {
    return PEngPropertyManager::GetProperty( iProperties,
                                             aName,
                                             aLevel,
                                             aValue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::Property()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModItemContainer::Property( TUint aName,
                                                 TUint aLevel ) const
    {
    return PEngPropertyManager::PropertyOrZero( iProperties,
                                                aName,
                                                aLevel );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::SetPropertyL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::SetPropertyL( TUint aName,
                                                     TUint aLevel,
                                                     TInt aValue )
    {
    PEngPropertyManager::SetPropertyL( iProperties,
                                       aName,
                                       aLevel,
                                       aValue,
                                       iSize );
    iStoreEntry.StoreEntryL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::SetPropertyL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::SetPropertyL( TUint aName,
                                                     TUint aLevel,
                                                     const TDesC8& aValue )
    {
    PEngPropertyManager::SetPropertyL( iProperties,
                                       aName,
                                       aLevel,
                                       aValue,
                                       iSize );
    iStoreEntry.StoreEntryL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::SetPropertyL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::SetPropertyL( TUint aName,
                                                     TUint aLevel,
                                                     const TDesC16& aValue )
    {
    PEngPropertyManager::SetPropertyL( iProperties,
                                       aName,
                                       aLevel,
                                       aValue,
                                       iSize );
    iStoreEntry.StoreEntryL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::DeletePropertyL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::DeletePropertyL( TUint aName,
                                                        TUint aLevel )
    {
    if ( PEngPropertyManager::DeletePropertyL(
             iProperties,
             aName,
             aLevel,
             iSize ) )
        {
        iStoreEntry.StoreEntryL();
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::GetPermanentValue()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModItemContainer::GetPermanentValue( const TDesC& /*aKey*/,
                                                          TPtrC8& /*aValue*/ ) const
    {
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::GetCachedValue()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModItemContainer::GetCachedValue( const TDesC& /*aKey*/,
                                                       TPtrC8& /*aValue*/ ) const
    {
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::SetPermanentValueL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::SetPermanentValueL( const TDesC& /*aKey*/,
                                                           const TDesC8& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::SetCachedValueL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::SetCachedValueL( const TDesC& /*aKey*/,
                                                        const TDesC8& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::DeletePermanentValueL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::DeletePermanentValueL( const TDesC& /*aKey*/ )
    {
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::DeleteCachedValueL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::DeleteCachedValueL( const TDesC& /*aKey*/ )
    {
    User::Leave( KErrNotSupported );
    }


// =============================================================================
// ===============Functions of main class ======================================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::Compare()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModItemContainer::Compare(
    const CPEngContactListModItemContainer& aFirst,
    const CPEngContactListModItemContainer& aSecond )
    {
    return NContactIdsTools::CompareContactIds( *( aFirst.iContactId ),
                                                *( aSecond.iContactId ) );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::Open()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::Open( TInt aRefType )
    {
    iAccessCount++;
    iRefDefinitions |= aRefType;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::CloseRef()
// -----------------------------------------------------------------------------
CPEngContactListModItemContainer* CPEngContactListModItemContainer::CloseRef(
    TInt aRefType /* 0 */ )
    {
    iRefDefinitions &= ~aRefType;

    // access count will be decreased by 1, get pointer
    CPEngContactListModItemContainer* self = ( iAccessCount - 1 ) ? this : NULL;
    Close();

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::RefActive()
// -----------------------------------------------------------------------------
TBool CPEngContactListModItemContainer::RefActive( TInt aRefType /* 0 */ )
    {
    return iRefDefinitions & aRefType;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::Close()
// -----------------------------------------------------------------------------
void CPEngContactListModItemContainer::Close()
    {
    --iAccessCount;

    if ( !iAccessCount )
        {
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::ExternalizeL( RWriteStream& aStream,
                                                     TInt aType ) const
    {
    switch ( aType )
        {
        case EPEngStorageBasicPermanent:
            {
            // reference definitions
            aStream.WriteInt32L( iRefDefinitions );
            aStream.WriteInt32L( iServerIndex );

            // WV Id
            aStream.WriteInt32L( iContactId->Length() );
            aStream << *iContactId;

            // nickname
            if ( iNickName )
                {
                aStream.WriteInt32L( iNickName->Length() );
                aStream << *iNickName;
                }
            else
                {
                // no nick name, save -1 to signal NULL pointer
                aStream.WriteInt32L( -1 );
                }

            //Fresh status
            aStream.WriteInt32L( iFresh );

            // Detailed properties
            PEngPropertyManager::ExternalizePropertiesL( iProperties,
                                                         aStream,
                                                         aType );
            break;
            }


        case EPEngStorageBasicCached:
            {
            // the new nickname
            if ( iNewNickName )
                {
                aStream.WriteInt32L( iNewNickName->Length() );
                aStream << *iNewNickName;
                }
            else
                {
                // no new nickname, save  -1 to signal NULL pointer
                aStream.WriteInt32L( -1 );
                }


            // Detailed properties
            PEngPropertyManager::ExternalizePropertiesL( iProperties,
                                                         aStream,
                                                         aType );

            break;
            }

        default:
            {
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::InternalizeL( RReadStream& aStream,
                                                     TInt aType )
    {
    switch ( aType )
        {
        case EPEngStorageBasicPermanent:
            {
            iProperties.ResetAndDestroy();

            iRefDefinitions = aStream.ReadInt32L();
            iServerIndex = aStream.ReadInt32L();

            // first WV Id
            TInt size( 2 * KIntStreamBytes );
            TInt length( aStream.ReadInt32L() );
            iContactId = HBufC::NewL( aStream, length );
            size += iContactId->Length();

            // read nickname
            length = aStream.ReadInt32L();
            size += KIntStreamBytes;
            if ( length != -1 )  // just to signal NULL pointer
                {
                iNickName = HBufC::NewL( aStream, length );
                size += iNickName->Length();
                }
            else
                {
                iNickName = NULL;
                }


            //Fresh status
            iFresh = aStream.ReadInt32L();


            // Detailed properties
            PEngPropertyManager::InternalizePropertiesL( iProperties,
                                                         aStream,
                                                         size );

            iSize += size;
            TInt& entrySize = iStoreEntry.StoreSizeCount();
            entrySize += size;
            break;
            }


        case EPEngStorageBasicCached:
            {
            // always 4 numbers stored in the store beside other data
            TInt size ( 4 * KIntStreamBytes );

            // New nickname
            TInt length ( aStream.ReadInt32L() );
            if ( length != -1 )  // just to signal NULL pointer
                {
                delete iNewNickName;
                iNewNickName = NULL;
                iNewNickName = HBufC::NewL( aStream, length );
                size += iNewNickName->Length();
                }
            else
                {
                delete iNewNickName;
                iNewNickName = NULL;
                }

            // Detailed properties
            PEngPropertyManager::InternalizePropertiesL( iProperties,
                                                         aStream,
                                                         size );

            iSize += size;
            TInt& entrySize = iStoreEntry.StoreSizeCount();
            entrySize += size;
            break;
            }

        default:
            {
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::UpdateContactId()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::UpdateContactId( HBufC* aId )  // CSI: 60 #
    {
    __ASSERT_DEBUG( aId, Panic( ENullParameter ) );

    iSize -= iContactId->Length();
    delete iContactId;
    iContactId = aId;
    iSize += iContactId->Length();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::NewNick()
// -----------------------------------------------------------------------------
//
const HBufC* CPEngContactListModItemContainer::NewNick() const
    {
    return iNewNickName;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::CurrecntNick()
// -----------------------------------------------------------------------------
//
const HBufC* CPEngContactListModItemContainer::CurrentNick() const
    {
    return iNickName;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::SetCurrentNickname()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::SetCurrentNickname( HBufC* aNickName ) // CSI: 60 #
    {
    if ( iNickName )
        {
        iSize -= iNickName->Length();
        delete iNickName;
        }

    iNickName = aNickName;

    if ( iNickName )
        {
        iSize += iNickName->Length();
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::AdoptCurrentNickAsNew()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::AdoptCurrentNickAsNew()
    {
    if ( iNewNickName )
        {
        iSize -= iNewNickName->Length();
        delete iNewNickName;
        }
    iNewNickName = iNickName;
    iNickName = NULL;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::RollBackNickname()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::RollBackNickname()
    {
    if ( iNewNickName )
        {
        iSize -= iNewNickName->Length();
        delete iNewNickName;
        }

    iNewNickName = NULL;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::CommitNickName()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::CommitNickName()
    {
    if ( iNickName )
        {
        iSize -= iNickName->Length();
        delete iNickName;
        }

    iNickName = iNewNickName;
    iNewNickName = NULL;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::ServerIndex()
// -----------------------------------------------------------------------------
TInt CPEngContactListModItemContainer::ServerIndex() const
    {
    return iServerIndex;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::SetServerIndex()
// -----------------------------------------------------------------------------
void CPEngContactListModItemContainer::SetServerIndex( TInt aIndex )
    {
    iServerIndex = aIndex;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::IsFreshContact()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListModItemContainer::IsFreshContact()
    {
    return iFresh;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModItemContainer::SetFreshContact()
// -----------------------------------------------------------------------------
//
void CPEngContactListModItemContainer::SetFreshContact( TBool aFresh )
    {
    iFresh = aFresh;
    }


//  End of File

