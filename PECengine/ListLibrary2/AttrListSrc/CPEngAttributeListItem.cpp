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
* Description:  Container of one attribute list item.
*
*/

// INCLUDE FILES
#include "CPEngAttributeListItem.h"
#include "PEngListLibraryPanics.h"
#include <e32std.h>
#include <s32strm.h>


//Default granurality for attribute list element members
const TInt KAttributeListElementGranurality = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeListItem::CPEngAttributeListItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeListItem::CPEngAttributeListItem()
        : iCurrentlyDefault( EFalse ),
        iNewDefault( EFalse ),
        iListOfAttributes( KAttributeListElementGranurality ),
        iListOfContactLists( KAttributeListElementGranurality ),
        iListOfNewContactLists( KAttributeListElementGranurality ),
        iListOfContactIDs( KAttributeListElementGranurality ),
        iListOfNewContactIDs( KAttributeListElementGranurality ),
        // calculated initial size for store, 8*4
        iSizeOfEntry( 32 )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::ConstructL( const RArray<TUint32>& aAttributes )
    {
    // copy attributes
    TInt count ( aAttributes.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        // we can use append since passed array is already in order
        iListOfAttributes.AppendL( aAttributes[x] );

        }
    iSizeOfEntry += count * 4; // 4 bytes for each attribute ID
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::ConstructL(
    const CPEngAttributeListItem& aAttributeList  )
    {
    iCurrentlyDefault = aAttributeList.CurrentlyDefault();
    iNewDefault = aAttributeList.NewDefault();
    CopyTIntArrayL( iListOfAttributes, aAttributeList.PresenceAttributes() );
    CopyDesArrayL( iListOfContactLists, aAttributeList.ArrayOfContacts( ECurrentContactLists ) );
    CopyDesArrayL( iListOfNewContactLists, aAttributeList.ArrayOfContacts( ENewContactLists ) );
    CopyDesArrayL( iListOfContactIDs, aAttributeList.ArrayOfContacts( ECurrentContactIDs ) );
    CopyDesArrayL( iListOfNewContactIDs, aAttributeList.ArrayOfContacts( ENewContactIDs ) );
    iSizeOfEntry = aAttributeList.SizeOfEntry();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::NewL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListItem* CPEngAttributeListItem::NewL( RReadStream& aStream )
    {
    CPEngAttributeListItem* self = NewLC( aStream );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAttributeListItem* CPEngAttributeListItem::NewLC(
    RReadStream& aStream )
    {
    CPEngAttributeListItem* self = new( ELeave ) CPEngAttributeListItem;

    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::NewL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListItem* CPEngAttributeListItem::NewL(
    const RArray<TUint32>& aAttributes )
    {
    CPEngAttributeListItem* self = NewLC( aAttributes );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeListItem* CPEngAttributeListItem::NewLC(
    const RArray<TUint32>& aAttributes )
    {
    CPEngAttributeListItem* self = new( ELeave ) CPEngAttributeListItem;

    CleanupStack::PushL( self );
    self->ConstructL( aAttributes );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::NewL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListItem* CPEngAttributeListItem::NewL(
    const CPEngAttributeListItem& aAttributeList )
    {
    CPEngAttributeListItem* self = new( ELeave ) CPEngAttributeListItem;

    CleanupStack::PushL( self );
    self->ConstructL( aAttributeList );
    CleanupStack::Pop();

    return self;
    }



//   Destructor
CPEngAttributeListItem::~CPEngAttributeListItem()
    {
    iListOfAttributes.Reset();
    iListOfContactLists.Reset();
    iListOfNewContactLists.Reset();
    iListOfContactIDs.Reset();
    iListOfNewContactIDs.Reset();
    }

// -----------------------------------------------------------------------------
// CPEngAttributeListItem::AddContactToListL()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListItem::AddContactToListL( const TDesC& aContact,
                                                TPEngAttrListItemArrays aArray )
    {
    CDesCArray& array = Array( aArray );
    TInt err( array.InsertIsqL( aContact, ECmpFolded ) );
    iSizeOfEntry += aContact.Size();
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::DeleteContactFromList()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListItem::DeleteContactFromList( const TDesC& aContact,
                                                    TPEngAttrListItemArrays aArray )
    {
    CDesCArray& array = Array( aArray );
    TInt err ( DeleteItemFromArray( array, aContact ) );
    if ( err != KErrNone )
        {
        return err;
        }

    return ContactsCount();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::ArrayOfContacts()
// -----------------------------------------------------------------------------
//
const CDesCArray& CPEngAttributeListItem::ArrayOfContacts(
    TPEngAttrListItemArrays aArray ) const
    {
    // this cast is here to reuse defined function,
    //it will return anyway const reference
    return  const_cast<CPEngAttributeListItem*>( this )->Array( aArray );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::PresenceAttributes()
// -----------------------------------------------------------------------------
//
const RArray<TUint32>& CPEngAttributeListItem::PresenceAttributes() const
    {
    return iListOfAttributes;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::UpdatePresenceAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::UpdatePresenceAttributesL(
    const RArray<TUint32>& aNewPresenceAttributes )
    {
    // 4 bytes each attribute
    iSizeOfEntry -= 4 * iListOfAttributes.Count();
    iListOfAttributes.Reset();

    TInt count( aNewPresenceAttributes.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        User::LeaveIfError( iListOfAttributes.InsertInSignedKeyOrder( aNewPresenceAttributes[ x ] ) );
        }

    // 4 bytes each attribute
    iSizeOfEntry += 4 * iListOfAttributes.Count();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::CommitContactListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::CommitContactListL( const TDesC& aContact )
    {
    CommitContactL( iListOfContactLists, iListOfNewContactLists, aContact );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::CommitContactIdL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::CommitContactIdL( const TDesC& aContact )
    {
    CommitContactL( iListOfContactIDs, iListOfNewContactIDs, aContact );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::CurrentlyDefault()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListItem::CurrentlyDefault() const
    {
    return iCurrentlyDefault;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::NewDefault()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListItem::NewDefault() const
    {
    return iNewDefault;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::SetNewDefault()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListItem::SetNewDefault( TBool aNewDefault )
    {
    iNewDefault = aNewDefault;
    return ContactsCount();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::CommitDefault()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::CommitDefault()
    {
    // was it suppose to be new default
    iCurrentlyDefault = iNewDefault;
    iNewDefault =  EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::Synchronized()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListItem::Synchronized() const
    {
    return iSynchronized;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::SetSynchronization()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::SetSynchronization( TBool aSync )
    {
    iSynchronized = aSync;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( iCurrentlyDefault );
    aStream.WriteInt32L( iNewDefault );
    aStream.WriteInt32L( iSynchronized );

    TInt count ( iListOfAttributes.Count() );
    aStream.WriteInt32L( count );

    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        aStream.WriteInt32L( iListOfAttributes[x] );
        }

    ExternalizeDesArrayToStreamL( iListOfContactLists, aStream );

    ExternalizeDesArrayToStreamL( iListOfContactIDs, aStream );

    ExternalizeDesArrayToStreamL( iListOfNewContactLists, aStream );

    ExternalizeDesArrayToStreamL( iListOfNewContactIDs, aStream );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::InternalizeL( RReadStream& aStream )
    {
    iCurrentlyDefault = aStream.ReadInt32L();
    iNewDefault = aStream.ReadInt32L();
    iSynchronized = aStream.ReadInt32L();
    iSizeOfEntry = 12; // already those 3 flags readed before


    TInt count ( aStream.ReadInt32L() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        iListOfAttributes.AppendL( aStream.ReadInt32L() );
        }
    iSizeOfEntry += 4 * ( 1 + count );

    iSizeOfEntry += InternalizeDesArrayFromStreamL( iListOfContactLists,
                                                    aStream );

    iSizeOfEntry += InternalizeDesArrayFromStreamL( iListOfContactIDs,
                                                    aStream );

    iSizeOfEntry += InternalizeDesArrayFromStreamL( iListOfNewContactLists,
                                                    aStream );

    iSizeOfEntry += InternalizeDesArrayFromStreamL( iListOfNewContactIDs,
                                                    aStream );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::SizeOfEntry()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListItem::SizeOfEntry() const
    {
    return iSizeOfEntry;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::ContactsCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListItem::ContactsCount() const
    {
    // Returns number of contact which are attached to the attribute list
    // If attribute list is set as default, it is considered like another
    // contact would be attached to the attribute list
    TInt count = iListOfContactLists.Count() +
                 iListOfNewContactLists.Count() +
                 iListOfContactIDs.Count() +
                 iListOfNewContactIDs.Count() +
                 iCurrentlyDefault +
                 iNewDefault;
    return count;
    }

// -----------------------------------------------------------------------------
// CPEngAttributeListItem::Array()
// -----------------------------------------------------------------------------
//
CDesCArray& CPEngAttributeListItem::Array( TPEngAttrListItemArrays aArray )
    {
    switch ( aArray )
        {
        case ENewContactLists:
            {
            return iListOfNewContactLists;
            }

        case ENewContactIDs:
            {
            return iListOfNewContactIDs;
            }

        case ECurrentContactLists:
            {
            return iListOfContactLists;
            }

        case ECurrentContactIDs:
            {
            return iListOfContactIDs;
            }

        default:
            {
            // by default work with list of new contact lists
            return iListOfNewContactLists;
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngAttributeListItem::CommitContactL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::CommitContactL( CDesCArray& aCurrentArray,
                                             CDesCArray& aNewArray,
                                             const TDesC& aContact )
    {
    // Try to find item from New list, if it is find update Current
    // contact list
    TInt position( 0 );
    if ( KErrNone == aNewArray.FindIsq( aContact, position, ECmpFolded ) )
        {
        // it was found in new, add it to the current one
        // trap, since it can leave it is already in list
        TRAPD( err, aCurrentArray.InsertIsqL( aContact, ECmpFolded ) );
        if ( err == KErrNoMemory ) // if it was no memory leave again
            {
            User::Leave( KErrNoMemory );
            }
        // now remove it from the new contacts
        aNewArray.Delete( position );
        // size does not change here since it was added and removed
        return;
        }

    // it was not found in the new contacts, so try to delete it from current
    // if it does not exist, just then it is just fine, ignore it
    DeleteItemFromArray( aCurrentArray, aContact );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::DeleteItemFromArray()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListItem::DeleteItemFromArray( CDesCArray& aArray,
                                                  const TDesC& aItem )
    {
    TInt posToDelete( 0 );
    if ( KErrNone == aArray.FindIsq( aItem, posToDelete, ECmpFolded ) )
        {
        aArray.Delete( posToDelete );
        iSizeOfEntry -= aItem.Size();
        return KErrNone;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::CopyDesArrayL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::CopyDesArrayL( CDesCArray& aArrayTarget,
                                            const CDesCArray& aArraySource )
    {
    TInt count ( aArraySource.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        aArrayTarget.AppendL( aArraySource.MdcaPoint( x ) );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::CopyTIntArrayL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::CopyTIntArrayL( RArray<TUint32>& aArrayTarget,
                                             const RArray<TUint32>& aArraySource )
    {
    TInt count ( aArraySource.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        aArrayTarget.AppendL( aArraySource[x] );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListItem::ExternalizeDesArrayToStreamL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListItem::ExternalizeDesArrayToStreamL( CDesCArray& aArray,
                                                           RWriteStream& aStream )
    {
    TInt count( aArray.MdcaCount() );
    aStream.WriteInt32L( count );
    for ( TInt ii( 0 ); ii < count; ii++ )
        {
        aStream << aArray.MdcaPoint( ii );
        }
    }

// -----------------------------------------------------------------------------
// CPEngAttributeListItem::InternalizeDesArrayFromStreamL()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListItem::InternalizeDesArrayFromStreamL( CDesCArray& aArray,
                                                             RReadStream& aStream )
    {
    //internalize item and insert it in array
    // reset the content of the array
    aArray.Reset();
    TInt sizeOfEntry( 0 );
    TInt count ( aStream.ReadInt32L() );
    // internalize array
    for ( TInt x ( 0 ); x < count; x++ )
        {
        // 100 should enough for contact list names
        HBufC* item = HBufC::NewLC( aStream, 100  );
        aArray.InsertIsqL( *item, ECmpFolded );
        sizeOfEntry += item->Size();
        CleanupStack::PopAndDestroy(); // item
        }

    return sizeOfEntry + 4; // 4 == the count
    }




//  End of File

