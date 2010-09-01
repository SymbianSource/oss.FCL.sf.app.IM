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
* Description:  Attribute list manager
*
*/

// INCLUDE FILES
#include    <e32std.h>
#include "CPEngAttributeListManager.h"
#include "CPEngAttributeListItem.h"
#include "CPEngAttributeListModel.h"

#include "PEngListLibTools.h"

#include "MPEngStorageManager.h"
#include "PEngStorageGlobals.h"
#include "PEngStorageManager.h"
#include "MPEngListLibFactory.h"
#include "CPEngSessionSlotId.h"

#include "PEngAttrLibFactory.h"
#include "MPEngPresenceAttrManager.h"


// CONSTANTS
// flag of the default attribute list + 3 counts, of 3 arrays
const TInt KIntEntrySize( 16 );

// storage ID of the attribute lists
_LIT( KAttributeListsStoreID, "AttributeListsStore" );

//Default granurality for ID lists
const TInt KAttributeListIdGranurality = 3;

const TInt KWVIDMaxSize = 50;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeListManager::CPEngAttributeListManager()
// -----------------------------------------------------------------------------
//
CPEngAttributeListManager::CPEngAttributeListManager(
    MPEngListLibFactory& aFactory )
        : CPEngStoreEntry( EPEngMixedPermanentVersionCon ),
        iAccessCount( 1 ), // init count to 1
        iFactory( aFactory ),
        iAttributeLists( KAttributeListIdGranurality ),
        iDeleteAttrListContactIDs( KAttributeListIdGranurality ),
        iDeleteAttrListContactLists( KAttributeListIdGranurality ),
        iDeleteDefaultAttrList( EFalse )
    {
    iSize = KIntEntrySize;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::ConstructL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    iSessionId = aSessionSlot.CloneL();
    MPEngStorageManager* storageManager =
        PEngStorageManager::GetStorageManagerL( *iSessionId );
    CleanupClosePushL( *storageManager );
    CPEngStoreEntry::BaseConstructL( *storageManager );
    CleanupStack::PopAndDestroy();

    iAttributeManager = PEngAttrLibFactory::AttributeManagerInstanceL(
                            *iSessionId );
    iStorageManager->RetrieveL( *this );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::NewL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListManager* CPEngAttributeListManager::NewL(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngAttributeListManager* self = NewLC( aFactory, aSessionSlot );
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAttributeListManager* CPEngAttributeListManager::NewLC(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngAttributeListManager* self =
        new( ELeave ) CPEngAttributeListManager( aFactory );

    CleanupClosePushL( *self );
    self->ConstructL( aSessionSlot );

    return self;
    }


//   Destructor
CPEngAttributeListManager::~CPEngAttributeListManager()
    {
    delete iSessionId;
    if ( iAttributeManager )
        {
        iAttributeManager->Close();
        }

    iAttributeLists.ResetAndDestroy();
    iDeleteAttrListContactIDs.Reset();
    iDeleteAttrListContactLists.Reset();
    }



// =============================================================================
// =============== From MPEngAttributeListManager ==============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAttributeListManager::SessionId()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngAttributeListManager::SessionId() const
    {
    return *iSessionId;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::Open()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::Open()
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::Close()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 )
        {
        iFactory.RemoveAttrListManager( this );
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::CreateEmptyAttributeListL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListManager::CreateEmptyAttributeListL()
    {
    return CPEngAttributeListModel::NewL( iAttributeManager );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::GetDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListManager::GetDefaultAttributeListL(
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    switch ( aAttrListsSet )
        {
        case EPEngNetworkAttributeLists:
            {
            TInt index ( FindCurrentDefaultAttributeList() );
            if ( index == KErrNotFound )
                {
                User::Leave( KErrNotFound );
                }

            // get list of attributes and create model with it
            return CPEngAttributeListModel::NewL( iAttributeManager,
                                                  iAttributeLists[index]->PresenceAttributes() );
            }

        case EPEngLocalAttributeLists:
            {
            TInt index ( FindNewDefaultAttributeList() );
            if ( index == KErrNotFound )
                {
                index = FindCurrentDefaultAttributeList();
                if ( index == KErrNotFound )
                    {
                    User::Leave( KErrNotFound );
                    }
                }

            // get list of attributes and create model with it
            return CPEngAttributeListModel::NewL( iAttributeManager,
                                                  iAttributeLists[index]->PresenceAttributes() );
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }


    // just for compiler
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::SetAsDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::SetAsDefaultAttributeListL(
    MPEngAttributeList2& aAttrList )
    {
    // set delete default attribute list to EFalse
    iDeleteDefaultAttrList = EFalse;
    CPEngAttributeListModel* model =
        static_cast< CPEngAttributeListModel* >( &aAttrList );
    // first check if there is not any new default flag already and remove it
    TInt index ( FindNewDefaultAttributeList() );
    if ( index != KErrNotFound )
        {
        // set new default to zero and if attribute lists is no more needed
        // delete it (0 == zero contacts in attr list)
        if ( 0 == iAttributeLists[ index ]->SetNewDefault( EFalse ) )
            {
            // attr. list empty delete it
            iSize -= iAttributeLists[ index ]->SizeOfEntry();
            delete iAttributeLists[ index ];
            iAttributeLists.Remove( index );
            }
        }

    // now find new one and set it, if it does not exist, create it
    index = FindAttributeList( model->PresenceAttributes() );
    if ( index != KErrNotFound )
        {
        // check if it is not already default, if not then set flag for new
        // otherwise there is no need to do anything
        if ( !iAttributeLists[ index ]->CurrentlyDefault() )
            {
            // it exists, just set the flag
            iAttributeLists[ index ]->SetNewDefault( ETrue );
            }
        }
    else
        {
        // it does not exist, create it and insert it in the array
        // with parameter new Default on
        CPEngAttributeListItem* newListItem =
            CPEngAttributeListItem::NewLC( model->PresenceAttributes() );

        // set it as new Default
        newListItem->SetNewDefault( ETrue );
        iAttributeLists.AppendL( newListItem );
        CleanupStack::Pop(); // newListItem
        iSize += newListItem->SizeOfEntry() ;
        }


    // store changes
    StoreChangesL();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::DeleteDefaultAttributeListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::DeleteDefaultAttributeListL()
    {
    // check if there is somewhere new default attribute list
    TInt err ( FindNewDefaultAttributeList() );
    if ( err != KErrNotFound )
        {
        // set new default to zero and if attribute lists is no more needed
        // delete it (0 == zero contacts in attr list)
        if ( 0 == iAttributeLists[ err ]->SetNewDefault( EFalse ) )
            {
            // attr. list empty delete it
            delete iAttributeLists[ err ];
            iAttributeLists.Remove( err );
            }
        }

    // set flag to delete default attr list
    iDeleteDefaultAttrList = ETrue;
    StoreChangesL();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::GetAttributeListForUserL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListManager::GetAttributeListForUserL(
    const TDesC&  aContactId,
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    return GetAttributeListForContactL(
               aContactId,
               aAttrListsSet,
               CPEngAttributeListItem::ECurrentContactIDs,
               CPEngAttributeListItem::ENewContactIDs );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::GetAttributeListForContactListL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListManager::GetAttributeListForContactListL(
    const TDesC& aContactList,
    TPEngAttributeListsSet2 aAttrListsSet )
    {
    return GetAttributeListForContactL(
               aContactList,
               aAttrListsSet,
               CPEngAttributeListItem::ECurrentContactLists,
               CPEngAttributeListItem::ENewContactLists );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::AttachAttributeListToUserL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::AttachAttributeListToUserL(
    const TDesC& aContactId,
    MPEngAttributeList2& aList )
    {
    AttachAttributeListToContactL(
        aContactId,
        aList,
        CPEngAttributeListItem::ECurrentContactIDs,
        CPEngAttributeListItem::ENewContactIDs,
        iDeleteAttrListContactIDs );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::AttachAttributeListToContactListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::AttachAttributeListToContactListL(
    const TDesC& aContactList,
    MPEngAttributeList2& aList )
    {
    AttachAttributeListToContactL(
        aContactList,
        aList,
        CPEngAttributeListItem::ECurrentContactLists,
        CPEngAttributeListItem::ENewContactLists,
        iDeleteAttrListContactLists );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::DeleteAttributeListFromUserL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::DeleteAttributeListFromUserL(
    const TDesC& aContactId )
    {
    DeleteAttributeListFromContactL(
        aContactId,
        CPEngAttributeListItem::ENewContactIDs,
        iDeleteAttrListContactIDs );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::DeleteAttributeListFromContactListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::DeleteAttributeListFromContactListL(
    const TDesC& aContactList )
    {
    DeleteAttributeListFromContactL(
        aContactList,
        CPEngAttributeListItem::ENewContactLists,
        iDeleteAttrListContactLists );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeListManager::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::ExternalizeL( RWriteStream& aStream,
                                              TPEngStorageType aStorageType ) const
    {
    if ( !( aStorageType & StorageType() ) )
        {
        User::Leave( KErrGeneral );
        }

    // write delete default list flag
    aStream.WriteInt32L( iDeleteDefaultAttrList );
    aStream.WriteInt32L( iSynchronized );


    // externalize list of to be delete wv IDs's attribute lists
    TInt count ( iDeleteAttrListContactIDs.Count() );
    aStream.WriteInt32L( count );
    for ( TInt i( 0 ) ; i < count ; i++ )
        {
        aStream << iDeleteAttrListContactIDs.MdcaPoint( i );
        }


    // externalize list of to be deleted contact lists's attribute lists
    count = iDeleteAttrListContactLists.Count();
    aStream.WriteInt32L( count );
    for ( TInt ii( 0 ) ; ii < count ; ii++ )
        {
        aStream << iDeleteAttrListContactLists.MdcaPoint( ii );
        }


    // externalize attribute lists
    count = iAttributeLists.Count();
    aStream.WriteInt32L( count );
    for ( TInt x ( 0 ) ;  x < count ; x++ )
        {
        iAttributeLists[x]->ExternalizeL( aStream );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::InternalizeL( RReadStream& aStream,
                                              TPEngStorageType aStorageType )
    {
    if ( !( aStorageType & StorageType() ) )
        {
        User::Leave( KErrGeneral );
        }


    iSize = KIntEntrySize;
    iDeleteDefaultAttrList =  aStream.ReadInt32L();

    // read synchronize flag
    // member value isn't updated from stored data
    // ==> full sync is performed each time when attribute lists
    //     are used for the first time during the session
    //     (if member variable would be updated from persisted data, sync would hapen
    //      only when atttribute lists are updated for the first time to account.)
    aStream.ReadInt32L();


    // internalize list of to be delete wv IDs's attribute lists
    TInt count ( aStream.ReadInt32L() );
    iDeleteAttrListContactIDs.Reset();
    for ( TInt i( 0 ) ; i < count ; i++ )
        {
        // use max size as double of KWVIDMaxSize
        HBufC16* newContact = HBufC16::NewLC( aStream, 2 * KWVIDMaxSize );
        iDeleteAttrListContactIDs.InsertIsqL( *newContact, ECmpFolded );
        iSize += newContact->Size();
        CleanupStack::PopAndDestroy(); // newContact
        }


    // internalize list of to be deleted contact lists's attribute lists
    count = aStream.ReadInt32L();
    iDeleteAttrListContactLists.Reset();
    for ( TInt ii( 0 ) ; ii < count ; ii++ )
        {
        // use max size as double of KWVIDMaxSize
        HBufC16* newContact = HBufC16::NewLC( aStream, 2 * KWVIDMaxSize );
        iDeleteAttrListContactLists.InsertIsqL( *newContact, ECmpFolded );
        iSize += newContact->Size();
        CleanupStack::PopAndDestroy(); // newContact
        }

    // reset array of the lists
    iAttributeLists.ResetAndDestroy();


    // first read  number of the Attribute lists
    count = aStream.ReadInt32L();
    for ( TInt x ( 0 ) ;  x < count ; x++ )
        {
        CPEngAttributeListItem* newLists =
            CPEngAttributeListItem::NewLC( aStream );
        iAttributeLists.AppendL( newLists );
        CleanupStack::Pop(); // newLists
        iSize += newLists->SizeOfEntry();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::StorageId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngAttributeListManager::StorageId() const
    {
    return KAttributeListsStoreID;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::EntrySize()
// -----------------------------------------------------------------------------
//
TUint32 CPEngAttributeListManager::EntrySize() const
    {
    return iSize;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::HandleSIDsChangeL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::HandleSIDsChangeL(
    CPtrCArray& /* aChangedSIDs */ )
    {
    iStorageManager->RetrieveL( *this );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::HandleSIDNotifyError()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::HandleSIDNotifyError(
    TInt /* aError */ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::FindAttributeList()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListManager::FindAttributeList(
    const RArray<TUint32>& aAttributeList ) const
    {

    // go through all attribute lists and look for the same attribute list
    TInt count ( iAttributeLists.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        if ( CompareAttributeLists( iAttributeLists[x]->PresenceAttributes() ,
                                    aAttributeList ) )
            {
            // Lists of presence attributes matches, return index of
            // the Attribute List Item
            return x;
            }
        }

    // no list of attributes which matches was found
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::FindCurrentDefaultAttributeList()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListManager::FindCurrentDefaultAttributeList() const
    {
    return FindDefaultAttrList( &CPEngAttributeListItem::CurrentlyDefault );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::FindNewDefaultAttributeList()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListManager::FindNewDefaultAttributeList() const
    {
    return FindDefaultAttrList( &CPEngAttributeListItem::NewDefault );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::StoreChangesL()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListManager::StoreChangesL()
    {
    TRAPD( err, iStorageManager->StoreL( *this ) );
    if ( err == KErrAccessDenied )
        {
        CPtrCArray* arr = NULL;
        HandleSIDsChangeL( *arr );
        }
    User::LeaveIfError( err );
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::CompareAttributeLists()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListManager::CompareAttributeLists(
    const RArray<TUint32>&  aListA ,
    const RArray<TUint32>&  aListB ) const
    {
    TInt count( aListA.Count() );
    if ( count !=  aListB.Count() )
        {
        // they do not even have same count of the presence attributes so
        // they cannot be same
        return EFalse;
        }

    // array are in order without duplicates, so we can easily go through
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        if ( aListA[x] != aListB[x] )
            {
            // do not match -> Attribute lists are not same
            return EFalse;
            }
        }

    // it went all fine, then Attribute lists are same
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::FindItemInArray()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListManager::FindItemInArray( const CDesCArray& aArray,
                                                 const TDesC& aItem ) const
    {
    TInt position( 0 );
    if ( aArray.FindIsq( aItem, position, ECmpFolded ) == KErrNone )
        {
        return position;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::FindContactInAttrLists()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListManager::FindContactInAttrLists(
    const TDesC& aContact,
    CPEngAttributeListItem::TPEngAttrListItemArrays aArray ) const
    {
    // go through all attribute lists and look for the contact in Current ones
    TInt count ( iAttributeLists.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        if ( KErrNotFound != FindItemInArray(
                 iAttributeLists[x]->ArrayOfContacts( aArray ),
                 aContact ) )
            {
            // contact was found, return index
            return x;
            }
        }

    // contact was not found in any of the lists
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::FindDefaultAttrList()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListManager::FindDefaultAttrList(
    ItemDefaultSet aDefaultSetCall ) const
    {
    TInt count ( iAttributeLists.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        if ( ( iAttributeLists[x]->*aDefaultSetCall )() )
            {
            return x;
            }
        }

    // no default list
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::GetAttributeListForContactL()
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CPEngAttributeListManager::GetAttributeListForContactL(
    const TDesC& aContact,
    TPEngAttributeListsSet2 aAttrListsSet,
    CPEngAttributeListItem::TPEngAttrListItemArrays aArrayCurrent,
    CPEngAttributeListItem::TPEngAttrListItemArrays aArrayNew )
    {
    switch ( aAttrListsSet )
        {
        case EPEngNetworkAttributeLists:
            {
            TInt index( FindContactInAttrLists( aContact, aArrayCurrent ) );
            if ( index == KErrNotFound )
                {
                User::Leave( KErrNotFound );
                }

            // get list of attributes and create model with it
            return CPEngAttributeListModel::NewL(
                       iAttributeManager,
                       iAttributeLists[index]->PresenceAttributes() );
            }


        case EPEngLocalAttributeLists:
            {
            TInt index( FindContactInAttrLists( aContact, aArrayNew ) );
            if ( index == KErrNotFound )
                {
                //try to have a look in the published ones
                index = FindContactInAttrLists( aContact, aArrayCurrent );
                if ( index == KErrNotFound )
                    {
                    User::Leave( KErrNotFound );
                    }
                }

            // get list of attributes and create model with it
            return CPEngAttributeListModel::NewL(
                       iAttributeManager,
                       iAttributeLists[index]->PresenceAttributes() );
            }

        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    // just for compiler
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::AttachAttributeListToContactL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::AttachAttributeListToContactL(
    const TDesC& aContact,
    MPEngAttributeList2& aList,
    CPEngAttributeListItem::TPEngAttrListItemArrays aArrayCurrent,
    CPEngAttributeListItem::TPEngAttrListItemArrays aArrayNew,
    CDesCArray& aToDeleteContacts )
    {
    TBool need2Store( EFalse );
    CPEngAttributeListModel* model =
        static_cast< CPEngAttributeListModel* >( &aList );

    // If defined contact is defined for some other Attribute list,
    // it can be ignored, since WV server will remove that connection
    // and link it to the new attribute list what has to be checked
    // it that contact is not in new part of any attribute lists
    // and as well delete part, form there it has to be deleted

    // first look in the new contacts of the attribute list
    TInt index( FindContactInAttrLists( aContact, aArrayNew ) );
    if ( index != KErrNotFound )
        {
        need2Store = ETrue;
        // it was found in new, remove it from there
        TInt count ( iAttributeLists[index]->DeleteContactFromList( aContact,
                                                                    aArrayNew ) );

        // check if there is still some contact in this attribute list,
        // if not delete it
        if ( count == 0 )
            {
            // delete attribute list
            iSize -= iAttributeLists[ index ]->SizeOfEntry();
            delete iAttributeLists[ index ];
            iAttributeLists.Remove( index );
            }
        }

    // now look in delete contacts of the attribute list
    index = FindItemInArray( aToDeleteContacts, aContact );
    if ( index != KErrNotFound )
        {
        need2Store = ETrue;
        // it was found in delete ones, remove it, it won't be neccesarry
        aToDeleteContacts.Delete( index );
        iSize -= aContact.Size();
        }


    // now find if list of attributes already somewhere exists
    TInt listIndex ( FindAttributeList( model->PresenceAttributes() ) );
    if ( listIndex != KErrNotFound )
        {
        // attribute list already exists, check current
        // if it does not exists already, if not add it to the new ones

        if ( KErrNotFound != FindItemInArray(
                 iAttributeLists[listIndex]->ArrayOfContacts( aArrayCurrent ),
                 aContact ) )
            {
            // contact is in current, no sense to do anything,
            if ( need2Store )
                {
                User::LeaveIfError( StoreChangesL() );
                }
            return;
            }
        else
            {
            // not in current, add it to the new contact lists
            iAttributeLists[ listIndex ]->AddContactToListL( aContact,
                                                             aArrayNew );
            iSize += aContact.Size();
            }

        // now store changes to the store in any case
        User::LeaveIfError( StoreChangesL() );
        return;
        }


    // Attribute lists does not exist, create it
    CPEngAttributeListItem* newListItem = CPEngAttributeListItem::NewLC(
                                              model->PresenceAttributes() );

    // add contact to the new ones and append it to the array
    newListItem->AddContactToListL( aContact, aArrayNew );
    iAttributeLists.AppendL( newListItem );
    iSize += newListItem->SizeOfEntry() ;
    CleanupStack::Pop(); // newListItem
    User::LeaveIfError( StoreChangesL() );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListManager::DeleteAttributeListFromContactL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListManager::DeleteAttributeListFromContactL(
    const TDesC& aContact,
    CPEngAttributeListItem::TPEngAttrListItemArrays aArrayNew,
    CDesCArray& aToDeleteContacts )
    {
    // there is now check if contact is in current, does not matter,
    // just try if it is in new, and delete if from there and add it to the
    // delete contacts queue
    TInt indexNew ( FindContactInAttrLists( aContact, aArrayNew ) );
    if ( indexNew != KErrNotFound )
        {
        TInt count ( iAttributeLists[indexNew]->DeleteContactFromList( aContact,
                                                                       aArrayNew ) );
        // check if there is still some contact in this attribute list,
        // if not delete it
        if ( count == 0 )
            {
            // delete attribute list, it's empty
            iSize -= iAttributeLists[ indexNew ]->SizeOfEntry();
            delete iAttributeLists[ indexNew ];
            iAttributeLists.Remove( indexNew );
            }
        }

    // now add contact to the delete contact lists queue,
    // if it's already there silently ignore
    TRAPD( e, aToDeleteContacts.InsertIsqL( aContact, ECmpFolded ) );
    iSize += aContact.Size();
    NListLibTools::LeaveIfMajorErrorL( e );
    User::LeaveIfError( StoreChangesL() );
    }

//  End of File





















