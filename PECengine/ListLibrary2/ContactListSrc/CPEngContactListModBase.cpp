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
* Description:  Contact list model implementation
*
*/

// INCLUDE FILES
#include    "CPEngContactListModBase.h"

#include    "CPEngContactListModItemContainer.h"
#include    "CPEngContactListModChangeMonitor.h"
#include    "CPEngContactListSettings.h"
#include    "MPEngContactListSettingsManager.h"
#include    "MPEngAttributeList2.h"
#include    "MPEngStorageManager.h"
#include    "PEngListLibraryPanics.h"
#include    "PEngContactIdsTools.h"
#include    "PresenceDebugPrint.h"

#include    <e32std.h>
#include    <bamdesca.h>



// CONSTANTS

// 1001 -  initial version
// 1002 -  store of the delta and monitor have now length of wv IDs,
static const TInt KListStoreVersion = 1002;


// Minimal size of the store entry for contact list model
// count of permanent,cached, and 2 change monitor arrays
static const TInt KListStoreMinSize = 16;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListModBase::CPEngContactListModBase()
// -----------------------------------------------------------------------------
//
CPEngContactListModBase::CPEngContactListModBase(
    CPEngContactListSettings& aListSettings,
    MPEngContactListSettingsManager& aCntLstSettingsManager )
        : CPEngStoreEntry( StoreTypeForListType( aListSettings.BaseSettings().iContactListType ) ),
        iAccessCount( 1 ), // set initial access count to 1
        iSettings( &aListSettings ),
        iCntLstSettingsManager ( aCntLstSettingsManager )
    {
    iSize = KListStoreMinSize;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::ConstructL( MPEngStorageManager& aStorageManager )
    {
    iStorageId = iSettings->Name().AllocL();
    iStorageId->Des().Fold();
    BaseConstructL( aStorageManager );      // CSI: 9 #

    iChangesMonitor = CPEngContactListModChangeMonitor::NewL( *this );

    iStorageManager->RetrieveL( *this );

    iCntLstSettingsManager.AddContactListSettingsObserverL( this );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::NewL()
// -----------------------------------------------------------------------------
//
CPEngContactListModBase* CPEngContactListModBase::NewL(
    CPEngContactListSettings& aListSettings,
    MPEngStorageManager& aStorageManager,
    MPEngContactListSettingsManager& aCntLstSettingsManager )
    {
    CPEngContactListModBase* self = NewLC( aListSettings,
                                           aStorageManager,
                                           aCntLstSettingsManager );

    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::NewL()
// -----------------------------------------------------------------------------
//
CPEngContactListModBase* CPEngContactListModBase::NewLC(
    CPEngContactListSettings& aListSettings,
    MPEngStorageManager& aStorageManager,
    MPEngContactListSettingsManager& aCntLstSettingsManager )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListModBase::NewLC() - %S" ),
             &aListSettings.Name() ) ;

    CPEngContactListModBase* self = new ( ELeave )
    CPEngContactListModBase( aListSettings,
                             aCntLstSettingsManager );

    CleanupClosePushL( *self );
    self->ConstructL( aStorageManager );

    return self;
    }


// Destructor
CPEngContactListModBase::~CPEngContactListModBase()
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListModBase::~CPEngContactListModBase" ) ) ;

    // panic if object is still accessed
    __ASSERT_DEBUG( iAccessCount == 0 , Panic( ERefAccessedObjectDeleted ) );


    iCntLstSettingsManager.RemoveContactListSettingsObserver( this );
    iCntLstSettingsManager.RemoveModel( this );

    ResetLocalView();
    ResetNetworkView();

    iServerOrder.Reset();

    delete iChangesMonitor;
    delete iStorageId;
    }



// =============================================================================
// ===============Functions of MPEngContactListAdvance class ===================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListModBase::ContactItemAdvance()
// -----------------------------------------------------------------------------
//
MPEngContactItemAdvance& CPEngContactListModBase::ContactItemAdvance(
    TInt aIndex,
    TPEngContactListView aView )
    {
    switch ( aView )
        {
        case EPEngCntListLocalView:
            {
            return *( iLocalView[ aIndex ] );
            }

        case EPEngCntListNetworkView:
            {
            return *( iNetworkView[ aIndex ] );
            }

        default:
            {
            Panic( EWrongViewId );
            return *( iNetworkView[ aIndex ] );
            }
        }
    }



// =============================================================================
// ===============Functions of MPEngContactList2 class =========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListModBase::AddContactL()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModBase::AddContactL( const TDesC& aContact,
                                           const TDesC& aNickName )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListModBase::AddIdL: %S, : %S" ),
             &this->StorageId(), &aContact );

    LeaveIfListUpdateInProgressL();
    iChangesMonitor->Reset();

    // Check if contact exists in the local list view
    TInt pos( KErrNotFound );
    if ( KErrNotFound != DoFindContact( iLocalView, aContact, pos ) )
        {
        // Contact already exists, leave
        User::Leave( KErrAlreadyExists );
        }

    // does contact exists in the network view
    TInt index ( DoFindContact( iNetworkView, aContact ) );
    if ( index != KErrNotFound )
        {
        // contact already exist on the network, it was probably removed
        // and now it is added, update nick name and reference
        iNetworkView[ index ]->UpdateNickNameL( aNickName );
        iLocalView.InsertL( iNetworkView[ index ], pos );
        iLocalView[ pos ]->Open( CPEngContactListModItemContainer::ELocalRef );
        }

    else
        {
        // Contact does not exist in any view, add it to the local one
        CPEngContactListModItemContainer* contact =
            CPEngContactListModItemContainer::NewNewNickLC( *this,
                                                            aContact,
                                                            aNickName );

        contact->SetFreshContact( ETrue );
        iLocalView.InsertL( contact, pos );
        iLocalView[ pos ]->Open( CPEngContactListModItemContainer::ELocalRef );
        CleanupStack::PopAndDestroy(); // contact
        }


    StoreContactListL();
    return pos;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::RemoveContactL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::RemoveContactL( const TDesC& aContact )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListModBase::RemoveIdL - %S : %S " ),
             &this->StorageId(), &aContact );

    LeaveIfListUpdateInProgressL();
    iChangesMonitor->Reset();

    // does contact exist in the local view?
    TInt pos( DoFindContact( iLocalView, aContact ) );
    if ( pos == KErrNotFound )
        {
        // nothing to delete
        return;
        }


    iLocalView[ pos ]->CloseRef( CPEngContactListModItemContainer::ELocalRef );
    iLocalView.Remove( pos );

    StoreContactListL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::FindContact()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModBase::FindContact( const TDesC& aContact,
                                           TPEngContactListView aView ) const
    {
    switch ( aView )
        {
        case EPEngCntListLocalView:
            {
            return DoFindContact( iLocalView, aContact );
            }

        case EPEngCntListNetworkView:
            {
            return DoFindContact( iNetworkView, aContact );
            }

        case EPEngAddedContacts:
            {
            const TDesC& domain = iCntLstSettingsManager.UserDomain();
            return iChangesMonitor->FindContactIdInAdded( aContact, domain );
            }

        case EPEngRemovedContacts:
            {
            const TDesC& domain = iCntLstSettingsManager.UserDomain();
            return iChangesMonitor->FindContactIdInRemoved( aContact, domain );
            }

        default:
            {
            Panic( EWrongViewId );
            return KErrNotFound;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::Count()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModBase::Count( TPEngContactListView aView ) const
    {
    switch ( aView )
        {
        case EPEngCntListLocalView:
            {
            return iLocalView.Count();
            }

        case EPEngCntListNetworkView:
            {
            return iNetworkView.Count();
            }

        case EPEngAddedContacts:
            {
            return iChangesMonitor->CountAddedContactIds();
            }

        case EPEngRemovedContacts:
            {
            return iChangesMonitor->CountRemovedContactIds();
            }

        default:
            {
            Panic( EWrongViewId );
            return 0;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ContactItem()
// -----------------------------------------------------------------------------
//
MPEngContactItem& CPEngContactListModBase::ContactItem( TInt aIndex,
                                                        TPEngContactListView aView )
    {
    return ContactItemAdvance( aIndex, aView );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ContactItem()
// -----------------------------------------------------------------------------
//
const MPEngContactItem& CPEngContactListModBase::ContactItem(
    TInt aIndex,
    TPEngContactListView aView ) const
    {
    switch ( aView )
        {
        case EPEngCntListLocalView:
            {
            return *( iLocalView[ aIndex ] );
            }

        case EPEngCntListNetworkView:
            {
            return *( iNetworkView[ aIndex ] );
            }

        default:
            {
            Panic( EWrongViewId );
            return *( iNetworkView[ aIndex ] );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::RemovedContacts()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListModBase::RemovedContacts( ) const
    {
    return iChangesMonitor->RemovedContactIds();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::AddedContacts()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListModBase::AddedContacts( ) const
    {
    return iChangesMonitor->AddedContactIds();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ListProperties()
// -----------------------------------------------------------------------------
//
MPEngContactListProperties& CPEngContactListModBase::ListProperties()
    {
    return *iSettings;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::RemoveAllContactIdsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::RemoveAllContactsL()
    {
    // reset local view and leave only network view
    ResetLocalView();
    StoreContactListL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::SuspendUpdateNotification()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::SuspendUpdateNotification()
    {
    BufferServerSideNotifications();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ReleaseUpdateNotification()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::ReleaseUpdateNotification()
    {
    ReleaseServerSideBuffering();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::RollBackAllChangesL()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModBase::RollBackAllChangesL()
    {
    LeaveIfListUpdateInProgressL();

    RollbackL();

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::UpdateInProgress()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListModBase::UpdateInProgress() const
    {
    return iStorageManager->Locked( *this, EStorageLockLevelHigh );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::UpdateInProgress()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListModBase::UpdateRequired() const
    {
    TInt countLoc( iLocalView.Count() );
    TInt countNet( iNetworkView.Count() );
    if ( countLoc != countNet )
        {
        return ETrue;
        }
    for ( TInt x( 0 ) ; x < countLoc ; ++x )
        {
        if ( ( KErrNotFound == iNetworkView.Find( iLocalView[ x ] ) )
             ||
             ( iLocalView[ x ]->NewNick() )
           )
            {
            return ETrue;
            }
        }
    for ( TInt i( 0 ) ; i < countNet ; ++i )
        {
        if  ( KErrNotFound == iLocalView.Find( iNetworkView[ i ] ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }



// =============================================================================
// ===============Functions of CPEngStoreEntry class ===========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListModBase::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::ExternalizeL( RWriteStream& aStream,
                                            TPEngStorageType aStorageType ) const
    {
    // what is the type of the list, if it is not mixed store all at once
    if ( EPEngMixedCachedVersionCon == StorageType() )
        {
        // whole list is cached, store it all
        ExternalizeContactsL( aStream, EPEngStorageBasicPermanent );
        ExternalizeContactsL( aStream, EPEngStorageBasicCached );
        iChangesMonitor->ExternalizeArrayL( aStream );
        return;
        }


    // write asked part
    switch ( aStorageType )
        {
        case EPEngStorageBasicPermanent:
            {
            // first write version of the storage
            aStream.WriteInt32L( KListStoreVersion );

            // store permanent part of the contact list
            ExternalizeContactsL( aStream, aStorageType );
            break;
            };


        case EPEngStorageBasicCached:
            {
            // store cached part of the contact list
            ExternalizeContactsL( aStream, aStorageType );
            iChangesMonitor->ExternalizeArrayL( aStream );
            break;
            }


        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::InternalizeL( RReadStream& aStream,
                                            TPEngStorageType aStorageType )
    {
    // what is the type of the list, if it is not mixed read all at once
    if ( EPEngMixedCachedVersionCon == StorageType() )
        {
        // whole list is cached, retrieve it once
        Reset();
        InternalizePermanentContactsL( aStream );
        InternalizeCachedContactsL( aStream );
        iChangesMonitor->InternalizeArrayL( aStream );
        return;
        }


    // read offered part
    switch ( aStorageType )
        {
        case EPEngStorageBasicPermanent:
            {
            // read permanent part of the contact list
            // fist check version
            Reset();
            if ( KListStoreVersion != aStream.ReadInt32L() )
                {
                // Version is wrong, remove file from store and reset
                iStorageManager->Delete( *this );
                return;
                }

            // now internalize all permanent parts
            InternalizePermanentContactsL( aStream );
            break;
            }


        case EPEngStorageBasicCached:
            {
            // read cached part of the contact list
            InternalizeCachedContactsL( aStream );
            iChangesMonitor->InternalizeArrayL( aStream );
            break;
            }


        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::StorageId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListModBase::StorageId() const
    {
    return *iStorageId;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::EntrySize()
// -----------------------------------------------------------------------------
//
TUint32 CPEngContactListModBase::EntrySize() const
    {
    return iSize;
    }



// =============================================================================
// ===============Functions of MPEngSIDChangeObserver ==========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListModBase::HandleSIDsChangeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::HandleSIDsChangeL( CPtrCArray& /* aChangedSIDs */ )
    {
    TInt err( iStorageManager->RetrieveL( *this ) );
    if ( err == KErrNotFound )
        {
        Reset();
        return;
        }

    if ( err != KErrNone )
        {
        iStorageManager->Delete( *this );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::HandleSIDNotifyError()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::HandleSIDNotifyError( TInt /* aError */ )
    {
    Reset();
    }



// =============================================================================
// ===============Functions of MPEngContactListModStore ========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListModBase::StoreSizeCount()
// -----------------------------------------------------------------------------
//
TInt& CPEngContactListModBase::StoreSizeCount()
    {
    return iSize;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::StoreEntryL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::StoreEntryL()
    {
    StoreL();
    }




// =============================================================================
// ===============Functions of MPEngContactListSettingsObserver class ==========
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListModBase::RefreshSettingsReference()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::RefreshSettingsReference()
    {
    iSettings = iCntLstSettingsManager.ContactListSettingsOrNull( StorageId() );
    }



// =============================================================================
// ===============Functions of main class ======================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListModBase::StoreTypeForListType()
// -----------------------------------------------------------------------------
//
TPEngStorageType CPEngContactListModBase::StoreTypeForListType(
    TPEngContactListType aListType )
    {
    return ( aListType == EPEngCachedContactList ) ? EPEngMixedCachedVersionCon :
           EPEngMixedPermanentCachedVersionCon;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::Open()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::Open()
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::Close()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::Close()
    {
    iAccessCount--;

    if ( iAccessCount == 0 )
        {
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::StoreContactListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::StoreContactListL()
    {
    TRAPD( err, iStorageManager->StoreL( *this ) ) ;
    if ( err != KErrNone )
        {
        if ( err == KErrDiskFull )
            {
            if ( KErrNotFound == iStorageManager->RetrieveL( *this ) )
                {
                Reset();
                }
            }

        User::Leave( err );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::FillAddNickListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::FillAddNickListL(
    RPointerArray<CPEngContactListModItemContainer>& aAddNickList )
    {
    TInt count( iLocalView.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( ( KErrNotFound == iNetworkView.Find( iLocalView[ x ] ) ) ||
             ( iLocalView[ x ]->NewNick() ) )
            {
            aAddNickList.AppendL( iLocalView[ x ] );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::FillRemoveNickListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::FillRemoveNickListL(
    RPointerArray<CPEngContactListModItemContainer>& aRemoveNickList )
    {
    TInt count( iNetworkView.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( KErrNotFound == iLocalView.Find( iNetworkView[ x ] ) )
            {
            aRemoveNickList.AppendL( iNetworkView[ x ] );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::CommitAddContactsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::CommitAddContactsL()
    {
    TInt count( iLocalView.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iLocalView[ x ]->CommitNickName();
        iLocalView[ x ]->SetFreshContact( EFalse );

        if ( KErrNotFound == iNetworkView.Find( iLocalView[ x ] ) )
            {
            TInt pos( KErrNotFound );
            CPEngContactListModItemContainer* cnt = iLocalView[ x ];
            DoFindContact( iNetworkView, cnt->Id(), pos );

            iNetworkView.InsertL( cnt, pos );
            cnt->Open( CPEngContactListModItemContainer::ENetworkRef ); // CSI: 65 #

            // no order, add to the end
            cnt->SetServerIndex( iServerOrder.Count() );
            iServerOrder.AppendL( cnt );

            // add contact also to the added
            iChangesMonitor->InsertAddedContactIdL( cnt->Id() );
            }
        }

    StoreL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::CommitRemoveContactsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::CommitRemoveContactsL()
    {
    for ( TInt x( iNetworkView.Count() - 1 ) ; x >= 0 ; --x )
        {
        if ( KErrNotFound == iLocalView.Find( iNetworkView[ x ] ) )
            {
            CPEngContactListModItemContainer* contact = iNetworkView[ x ];

            // cache the index that's being removed
            TInt serverIndex = contact->ServerIndex();

            // add contact also to the removed
            iChangesMonitor->InsertRemovedContactIdL( contact->Id() );

            // remove from the network ordered
            // let it panic, if not found
            // arrays should be always in sync
            iServerOrder.Remove( iServerOrder.Find( contact ) );

            contact->CloseRef( CPEngContactListModItemContainer::ENetworkRef );

            iNetworkView.Remove( x );

            // and reorganize iNetworkView's server indexes
            ReorganizeServerIndexOrder( serverIndex );
            }
        }

    StoreL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::RollbackL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::RollbackL()
    {
    // reset local list and add there items from the network one
    ResetLocalView();

    TInt count( iNetworkView.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iLocalView.AppendL( iNetworkView[ x ] );
        iNetworkView[ x ]->Open( CPEngContactListModItemContainer::ELocalRef );
        iNetworkView[ x ]->RollBackNickname();
        }

    StoreContactListL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::FindContactInAll()
// -----------------------------------------------------------------------------
//
CPEngContactListModItemContainer* CPEngContactListModBase::FindContactInAll(
    const TDesC& aContact )
    {
    TInt index ( DoFindContact( iLocalView, aContact ) );
    if ( index != KErrNotFound )
        {
        return iLocalView[ index ];
        }

    index = DoFindContact( iNetworkView, aContact );
    if ( index != KErrNotFound )
        {
        return iNetworkView[ index ];
        }

    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ContactExistsOnServer()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListModBase::ContactExistsOnServer(
    CPEngContactListModItemContainer& aContact )
    {
    return aContact.RefActive( CPEngContactListModItemContainer::ENetworkRef );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::AdoptNewContactsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::AdoptNewContactsL(
    RPointerArray<CPEngContactListModItemContainer>& aContacts )
    {
    ResetLocalView();
    ResetNetworkView();

    TInt count = aContacts.Count();
    for ( TInt ii( 0 ) ; ii < count ; ++ii )
        {
        // take always first in array
        CPEngContactListModItemContainer* cnt = aContacts[ 0 ];
        iLocalView.AppendL( cnt );
        cnt->Open( CPEngContactListModItemContainer::ELocalRef );   // CSI: 65 #
        iNetworkView.AppendL( cnt );
        cnt->Open( CPEngContactListModItemContainer::ENetworkRef ); // CSI: 65 #
        iServerOrder.AppendL( NULL );
        aContacts.Remove( 0 );
        cnt->Close();
        }


    RestoreServerOrderedArray();
    StoreL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::AdoptNetworkViewL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::AdoptNetworkViewL(
    RPointerArray<CPEngContactListModItemContainer>& aContacts )
    {
    ResetNetworkView();

    TInt count( aContacts.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        // take always first in array
        CPEngContactListModItemContainer* cnt = aContacts[ 0 ];
        iNetworkView.AppendL( cnt );
        cnt->Open( CPEngContactListModItemContainer::ENetworkRef );     // CSI: 65 #
        iServerOrder.AppendL( NULL );
        aContacts.Remove( 0 );
        cnt->Close();
        }

    RestoreServerOrderedArray();
    StoreL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::RemoveBadContactL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::RemoveBadContactL( const TDesC& aContact )
    {
    iChangesMonitor->InsertRemovedContactIdL( aContact );
    iChangesMonitor->RemoveAddedContactId( aContact );

    CPEngContactListModItemContainer* cnt = FindContactInAll( aContact );


    if ( cnt && cnt->RefActive( CPEngContactListModItemContainer::ENetworkRef ) )
        {
        TInt serverIndex = cnt->ServerIndex();

        iNetworkView.Remove( iNetworkView.Find( cnt ) );
        cnt = cnt->CloseRef( CPEngContactListModItemContainer::ENetworkRef );
        iServerOrder.Remove( iServerOrder.Find( cnt ) );

        // and reorganize iNetworkView's server indexes
        ReorganizeServerIndexOrder( serverIndex );
        }


    if ( cnt && cnt->RefActive( CPEngContactListModItemContainer::ELocalRef ) )
        {
        iLocalView.Remove( iLocalView.Find( cnt ) );
        cnt->CloseRef( CPEngContactListModItemContainer::ELocalRef );
        }

    StoreL();
    }



// -----------------------------------------------------------------------------
// CPEngContactListModBase::Reset()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::Reset()
    {
    ResetLocalView();
    ResetNetworkView();
    iChangesMonitor->Reset();

    iSize = KListStoreMinSize;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ResetLocalView()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::ResetLocalView()
    {
    TInt count( iLocalView.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iLocalView[ x ]->CloseRef( CPEngContactListModItemContainer::ELocalRef );
        }

    iLocalView.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ResetNetworkView()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::ResetNetworkView()
    {
    TInt count( iNetworkView.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iNetworkView[ x ]->CloseRef( CPEngContactListModItemContainer::ENetworkRef );
        }

    iNetworkView.Reset();
    iServerOrder.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::LeaveIfListUpdateInProgressL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::LeaveIfListUpdateInProgressL() const
    {
    if ( UpdateInProgress() )
        {
        User::Leave( KErrInUse );
        }
    }



// -----------------------------------------------------------------------------
// CPEngContactListModBase::ChangeMonitor()
// -----------------------------------------------------------------------------
//
CPEngContactListModChangeMonitor& CPEngContactListModBase::ChangeMonitor()
    {
    return *iChangesMonitor;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::Settings()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings& CPEngContactListModBase::Settings()
    {
    return *iSettings;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ServerOrderedContacts()
// -----------------------------------------------------------------------------
//
const RPointerArray<CPEngContactListModItemContainer>&
CPEngContactListModBase::ServerOrderedContacts() const
    {
    return iServerOrder;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::DoFindContact()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModBase::DoFindContact(
    const RPointerArray<CPEngContactListModItemContainer>& aArray,
    const TDesC& aContact,
    TInt& aIndex,
    TBool aFirstPrefered /* EFalse */ ) const
    {
    const TDesC& domain = iCntLstSettingsManager.UserDomain();
    TInt high( aArray.Count() );


    if ( !high ||
         ( aFirstPrefered && 0 > NContactIdsTools::CompareContactIds( aContact,
                                                                      aArray[ 0 ]->Id(),
                                                                      domain ) ) )
        {
        aIndex = 0;
        return KErrNotFound;
        }


    TInt low( 0 );
    TInt ret( KErrNotFound );
    while ( high > low )
        {
        TInt inx = ( low + high ) >> 1;
        TInt k ( NContactIdsTools::CompareContactIds( aContact,
                                                      aArray[ inx ]->Id(),
                                                      domain ) );
        if ( k == 0 )
            {
            aIndex = inx;
            return KErrNone;
            }
        else if ( k > 0 )
            low = inx + 1;
        else
            high = inx;
        }
    aIndex = high;
    return ret;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::DoFindContact()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModBase::DoFindContact(
    const RPointerArray<CPEngContactListModItemContainer>& aArray,
    const TDesC& aContact ) const
    {
    TInt index( KErrNotFound );
    TInt err( DoFindContact( aArray, aContact, index ) );
    if ( err != KErrNotFound )
        {
        return index;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ExternalizeContactsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::ExternalizeContactsL( RWriteStream& aStream,
                                                    TPEngStorageType aStorageType ) const
    {
    TInt count( iLocalView.Count() );
    for ( TInt x( 0 ) ; x < count  ; ++x )
        {
        aStream.WriteInt32L( KErrNone );
        iLocalView[ x ]->ExternalizeL( aStream, aStorageType );
        }

    count = iNetworkView.Count();
    for ( TInt y( 0 ) ; y < count ; ++y )
        {
        if ( !iNetworkView[ y ]->RefActive( CPEngContactListModItemContainer::ELocalRef ) )
            {
            aStream.WriteInt32L( KErrNone );
            iNetworkView[ y ]->ExternalizeL( aStream, aStorageType );
            }
        }

    aStream.WriteInt32L( KErrNotFound );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::InternalizePermanentContactsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::InternalizePermanentContactsL( RReadStream& aStream )
    {
    CPEngContactListModItemContainer* contact = NULL;

    while ( KErrNone == aStream.ReadInt32L() )
        {
        contact = CPEngContactListModItemContainer::NewLC( *this, aStream );

        if ( contact->RefActive( CPEngContactListModItemContainer::ELocalRef ) )
            {
            iLocalView.AppendL( contact );
            contact->Open( CPEngContactListModItemContainer::ELocalRef ); // CSI: 65 #
            }

        if ( contact->RefActive( CPEngContactListModItemContainer::ENetworkRef ) )
            {
            iNetworkView.AppendL( contact );
            contact->Open( CPEngContactListModItemContainer::ENetworkRef ); // CSI: 65 #
            iServerOrder.AppendL( NULL );
            }

        CleanupStack::PopAndDestroy(); // contact
        }

    RestoreServerOrderedArray();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::InternalizeCachedContactsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::InternalizeCachedContactsL( RReadStream& aStream )
    {
    TInt count( iLocalView.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        aStream.ReadInt32L(); // KErrNone
        iLocalView[ x ]->InternalizeL( aStream, EPEngStorageBasicCached );
        }

    count = iNetworkView.Count();
    for ( TInt y( 0 ) ; y < count ; ++y )
        {
        if ( ! iNetworkView[ y ]->RefActive( CPEngContactListModItemContainer::ELocalRef ) )
            {
            aStream.ReadInt32L(); // KErrNone
            iNetworkView[ y ]->InternalizeL( aStream, EPEngStorageBasicCached );
            }
        }

    aStream.ReadInt32L(); // KErrNotFound
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::RestoreServerOrderedArray()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::RestoreServerOrderedArray()
    {
    TInt count( iNetworkView.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        CPEngContactListModItemContainer* contact = iNetworkView[ x ];
        __ASSERT_DEBUG( count > contact->ServerIndex() , Panic( ERefServerOrderedArrayNotFilled ) );
        iServerOrder[ contact->ServerIndex() ] = contact;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModBase::ReorganizeServerIndexOrder()
// -----------------------------------------------------------------------------
//
void CPEngContactListModBase::ReorganizeServerIndexOrder( TInt aIndex )
    {
    TInt networkViewCount( iNetworkView.Count() );

    for ( TInt ii( 0 ) ; ii < networkViewCount; ii++ )
        {
        TInt srvIndex( iNetworkView[ii]->ServerIndex() );
        if ( srvIndex > aIndex )
            {
            srvIndex--;
            iNetworkView[ii]->SetServerIndex( srvIndex );
            }
        }
    }



//  End of File


















