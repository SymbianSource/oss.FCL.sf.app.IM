/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Storage manager implementation
 *
*/


// INCLUDE FILES
#include "CCAStorageManager.h"
#include "CCAStorage.h"
#include "CCARefreshTimer.h"
#include "CCAContactList.h"
#include "CCAGroup.h"
#include "CCAContact.h"
#include "CCAContactSorter.h"

#include "CAUtils.h"

#include "ChatDebugPrint.h"
#include "chatdebugassert.h"

#include <PEngPresenceEngineConsts2.h> // for KPEngUserOwnPresenceId

// CONSTANTS
// Own device type, default to mobile
const TStorageManagerGlobals::TClientType KOwnDeviceType =
    TStorageManagerGlobals::EMobile;

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAStorageManager::InstanceL
// Factory method to return the singleton instance of this class,
// used by CCAStorageManagerFactory
// -----------------------------------------------------------------------------
//
CCAStorageManager* CCAStorageManager::InstanceL()
    {
    CCAStorageManager *sm = NULL;
    sm = static_cast<CCAStorageManager*>( Dll::Tls() );
    if ( ! sm )
        {
        // no existing instance, create a new one
        CCAStorageManager *manager = CCAStorageManager::NewL();
        sm = manager;
        CleanupStack::PushL( manager );
        User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( sm ) ) );
        CleanupStack::Pop( manager );
        }

    return sm;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ReleaseL
// Factory method to release the singleton instance of this class,
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::ReleaseL()
    {
    // about to unload, so kill the singleton instance
    if ( Dll::Tls() )
        {
        CCAStorageManager *storage = NULL;
        storage = static_cast<CCAStorageManager*>( Dll::Tls() );
        if ( storage )
            {
            delete storage;
            storage = NULL;
            }
        }
    // Ignore error
    Dll::SetTls( NULL );
    return KErrNone;
    }

// panic handler
GLDEF_C void Panic( TCAStoragePanics aPanic )
    {
    User::Panic( KChatStoragePanicString, aPanic );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::CCAStorageManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAStorageManager::CCAStorageManager() :
        iNotifyUponSave( ETrue ),
        iRefreshStartCounter( 0 ),
        iShowNickname( ETrue ),
        iGroupDeleted( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ConstructL
// Symbian 2nd phase constructor can leave.
// The constructor also does synchronizing with Phone Book by removing the
// deleted elements from the persistent storage
// -----------------------------------------------------------------------------
//
void CCAStorageManager::ConstructL()
    {
    CHAT_DP_TXT( "CCAStorageManager::ConstructL" );
    CHAT_DP_TXT( "CCAStorageManager::Creating Storage" );
    iStorage = CCAStorage::NewL();

    iRefreshTimer = CCARefreshTimer::NewL( this );

    // create sorter for contacts
    iContactSorter = CCAContactSorter::NewL();

    // handle Groups
    CHAT_DP_TXT( "CCAStorageManager::ConstructL - Synching group data with \
                  persistent storage" );
    SynchronizeGroupsL();

    iOwnStatus = CCAContact::NewL( this, this );
    iOwnStatus->SetClientType( KOwnDeviceType );

    CHAT_DP_TXT( "CCAStorageManager::ConstructL - Done" );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SynchronizeGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SynchronizeGroupsL()
    {
    // read the groups, if any
    TBool fetching( ETrue );
    for ( TInt i = 0; fetching ; i++ )
        {
        CCAGroup* group = CCAGroup::NewL( iStorage, this );
        CleanupStack::PushL( group );

        TInt storStatus( KErrNone );
        TInt err( KErrNone );

        if ( 0 == i )
            {
            // first iteration
            CHAT_DP_TXT( "CCAStorageManager::SynchronizeGroupsL - Loading \
                          first group" );
            // Load Leaves if storage has no more groups
            TRAP( err, storStatus = iStorage->LoadFirstL( group ) );
            }
        else
            {
            // subsequent iterations
            CHAT_DP_TXT( "CCAStorageManager::SynchronizeGroupsL - Loading next \
                          group" );
            // Load Leaves if storage has no more groups
            TRAP( err, storStatus = iStorage->LoadNextL( group ) );
            }

        if ( ( KErrNotFound == storStatus ) || ( KErrNone != err ) )
            {
            // no more groups, that's all
            CHAT_DP_TXT( "CCAStorageManager::SynchronizeGroupsL - No groups in \
                          persistent storage" );
            fetching = EFalse;
            }
        else
            {
            // found some group, add to our list
            CHAT_DP_TXT( "CCAStorageManager::SynchronizeGroupsL - Found group, \
                          adding to list" );

            // valid group, add to our list
            group->SetStorageType( TStorageManagerGlobals::EStoragePersistent );
            TInt status = iGroupList.Append( group );
            User::LeaveIfError( status );
            }

        if ( fetching == EFalse )
            {
            // all done
            CleanupStack::PopAndDestroy( group ); // group
            }
        else
            {
            // going to next iteration
            CleanupStack::Pop( group ); // group
            }
        }

    CHAT_DP_TXT( "CCAStorageManager::SynchronizeGroupsL - Done" );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCAStorageManager* CCAStorageManager::NewL()
    {
    CCAStorageManager* self = new( ELeave ) CCAStorageManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCAStorageManager::~CCAStorageManager()
    {
    CHAT_DP_TXT( "CCAStorageManager::~CCAStorageManager" );

    delete iRefreshTimer;
    delete iLatestContactToRefresh;

    iContactObservers.Reset();
    iContactObservers.Close();

    iGroupObservers.Reset();
    iGroupObservers.Close();

    iContactList.ResetAndDestroy();
    iGroupList.ResetAndDestroy();

    delete iStorage;

    delete iContactSorter;

    delete iOwnStatus;

    CHAT_DP_TXT( "CCAStorageManager::~CCAStorageManager - I am dead" );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SendObserverEventL
// If the changed/deleted contact is in our list, the observers are notified.
// Otherwise the observers are not notified.
// -----------------------------------------------------------------------------
void CCAStorageManager::SendObserverEvent(
    TStorageManagerGlobals::TCAObserverEventType aType,
    MCAStoredContact* aContact,
    const TDesC& aContactId,
    MCAContactList* aList /*= NULL*/,
    TBool aUserIdChanged /*= EFalse*/ )
    {
    CHAT_DP_TXT( "CCAStorageManager::SendObserverEventL (contact)" );
    CHAT_DP_TXT( "CCAStorageManager::SendObserverEventL - Notifying \
                  MCAStoredContactsObserver observers" );

    if ( !iContactListLock )
        {
        switch ( aType )
            {
            case TStorageManagerGlobals::EStorageEventPostChange: // flowtrough
            case TStorageManagerGlobals::EStorageEventOwnStatusChange:
                {
                NotifyAllObserversWithDelay( aType, aContact, aContactId, aList,
                                             aUserIdChanged );
                break;
                }
            default:
                {
                NotifyAllObservers( aType, aContact, aContactId, aList, aUserIdChanged );
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::NotifyAllObservers
// -----------------------------------------------------------------------------
//
void CCAStorageManager::NotifyAllObservers( TStorageManagerGlobals::TCAObserverEventType aType,
                                            MCAStoredContact* aContact, const TDesC& aContactId,
                                            MCAContactList* aList,
                                            TBool aUserIdChanged )
    {
    CHAT_DP_FUNC_ENTER( "NotifyAllObservers" );

    // tell all of the observers about the event.
    // the amount of observers might change during the loop,
    // so the count MUST be taken in every round
    for ( TInt i = 0; i < iContactObservers.Count(); i++ )
        {
        MCAStoredContactsObserver* obs = iContactObservers[i];
        switch ( aType )
            {
            case TStorageManagerGlobals::EStorageEventPreChange:
            case TStorageManagerGlobals::EStorageEventPostChange:
                {
                obs->HandleChange( aList, aContact, aType, aUserIdChanged );
                break;
                }
            case TStorageManagerGlobals::EStorageEventContactDelete:
                {
                obs->HandleContactDelete( aContactId );
                break;
                }
            case TStorageManagerGlobals::EStorageEventContactAddition:
                {
                if ( aList )
                    {
                    obs->HandleAddition( *aList, *aContact );
                    }
                break;
                }
            case TStorageManagerGlobals::EStorageEventOwnStatusChange:
            case TStorageManagerGlobals::EStorageEventMultipleChanges:
            case TStorageManagerGlobals::EStorageEventListAddition:
            case TStorageManagerGlobals::EStorageEventListDelete:
            case TStorageManagerGlobals::EStorageEventListChanged:
                {
                obs->HandleChange( aList, NULL, aType, EFalse );
                break;
                }
            default:
                {
                break;
                }
            }
        }

    CHAT_DP_FUNC_DONE( "NotifyAllObservers" );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::NotifyAllObserversWithDelay
// -----------------------------------------------------------------------------
//
void CCAStorageManager::NotifyAllObserversWithDelay(
    TStorageManagerGlobals::TCAObserverEventType aType,
    MCAStoredContact* aContact, const TDesC& aContactId,
    MCAContactList* aList,
    TBool aUserIdChanged )
    {
    TRAPD( err, DoNotifyAllObserversWithDelayL( aType, aContact, aContactId,
                                                aList, aUserIdChanged ) );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::DoNotifyAllObserversWithDelayL
// -----------------------------------------------------------------------------
//
void CCAStorageManager::DoNotifyAllObserversWithDelayL(
    TStorageManagerGlobals::TCAObserverEventType aType,
    MCAStoredContact* /*aContact*/, const TDesC& aContactId,
    MCAContactList* aList, TBool aUserIdChanged )
    {
    HBufC* tmpID = aContactId.AllocL();
    delete iLatestContactToRefresh;
    iLatestContactToRefresh = tmpID;
    iLatestContactList = aList;
    tmpID = NULL;
    iRefreshEventType = aType;
    iRefreshUserIdChanged = aUserIdChanged;
    ++iRefreshStartCounter;
    if ( iRefreshTimer->IsActive() )
        {
        iRefreshTimer->Cancel();
        }
    iRefreshTimer->Start();
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SendObserverEventL (overridden for group)
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SendObserverEvent(
    TStorageManagerGlobals::TCAObserverEventType aType,
    const TDesC& aParameter )
    {
    CHAT_DP_TXT( "CCAStorageManager::SendObserverEventL (group)" );

    TInt pos( FindGroupIndexByGroupId( aParameter ) );
    if ( KErrNotFound == pos )
        {
        return;     // not found in the list
        }

    CHAT_DP_TXT( "CCAStorageManager::SendObserverEventL - Notifying \
                  MCAStoredGroupsObserver observers" );

    // tell all of the observers about the event
    const TInt count( iGroupObservers.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        MCAStoredGroupsObserver* obs = iGroupObservers[i];

        switch ( aType )
            {
            case TStorageManagerGlobals::EStorageEventGroupChange:
                {
                obs->HandleChange( aParameter );
                break;
                }

            case TStorageManagerGlobals::EStorageEventContactDelete:
                {
                obs->HandleDelete( aParameter );
                break;
                }

            case TStorageManagerGlobals::EStorageEventContactAddition:
                {
                obs->HandleAddition( aParameter );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::AddObserverL
// From MCAStoredContacts
// -----------------------------------------------------------------------------
//
void CCAStorageManager::AddObserverL( MCAStoredContactsObserver*
                                      aObserver )
    {
    __ASSERT_ALWAYS( aObserver, Panic( EObserverIsNull ) );

    TInt status( iContactObservers.Append( aObserver ) );
    User::LeaveIfError( status );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::RemoveObserverL
// From MCAStoredContacts
// -----------------------------------------------------------------------------
void CCAStorageManager::RemoveObserver( MCAStoredContactsObserver*
                                        aObserver )
    {
    __ASSERT_ALWAYS( aObserver, Panic( EObserverIsNull ) );

    const TInt status( iContactObservers.Find( aObserver ) );
    if ( status != KErrNotFound )
        {
        iContactObservers.Remove( status );
        iContactObservers.Compress();
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::AddObserverL
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::AddObserverL( MCAStoredGroupsObserver*
                                      aObserver )
    {
    __ASSERT_ALWAYS( aObserver, Panic( EObserverIsNull ) );

    TInt status( iGroupObservers.Append( aObserver ) );
    User::LeaveIfError( status );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::RemoveObserverL
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::RemoveObserver( MCAStoredGroupsObserver*
                                        aObserver )
    {
    __ASSERT_ALWAYS( aObserver, Panic( EObserverIsNull ) );

    const TInt status( iGroupObservers.Find( aObserver ) );
    if ( status != KErrNotFound )
        {
        iGroupObservers.Remove( status );
        iGroupObservers.Compress();
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::AddContactListIdL
// -----------------------------------------------------------------------------
//
MCAContactList* CCAStorageManager::AddContactListIdL(
    const TDesC& aContactListId,
    const TDesC& aDisplayName,
    TBool aNotify )
    {
    CCAContactList* contactList = CCAContactList::NewLC( *this,
                                                         *iContactSorter,
                                                         *this,
                                                         aContactListId,
                                                         aDisplayName );
    TIdentityRelation< CCAContactList > findBy(
        CCAStorageManager::ContactListFindByContactId );
    TInt indexOfList( iContactList.Find( contactList, findBy ) );
    if ( indexOfList == KErrNotFound )
        {
        TLinearOrder< CCAContactList > order(
            CCAStorageManager::ContactListOrderByDisplayName );
        iContactList.InsertInOrderAllowRepeats( contactList, order );
        CleanupStack::Pop( contactList );
        }
    else
        {
        CleanupStack::PopAndDestroy( contactList );
        contactList = iContactList[ indexOfList ];
        }
    // Inform observers
    if ( aNotify )
        {
        SendObserverEvent( TStorageManagerGlobals::EStorageEventListAddition,
                           NULL,
                           KNullDesC, contactList, EFalse );
        }
    return contactList;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SetContactListCollapsed
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetContactListCollapsed( const TDesC& aContactListId,
                                                 TBool aCollapsed )
    {
    TInt pos( FindContactListById( aContactListId ) );
    if ( pos >= 0 )
        {
        MCAContactList& list = *iContactList[ pos ];
        list.SetCollapsed( aCollapsed );
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::RemoveContactList
// -----------------------------------------------------------------------------
//
void CCAStorageManager::RemoveContactList( const TDesC& aContactListId )
    {
    TInt pos( FindContactListById( aContactListId ) );
    if ( pos >= 0 )
        {
        delete iContactList[ pos ];
        iContactList.Remove( pos );
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::RemoveContact
// From MCAStoredContacts.
// -----------------------------------------------------------------------------
void CCAStorageManager::RemoveContact( const TDesC& aListId,
                                       const TDesC& aContactId )
    {
    CCAContactList* list = FindContactListInternal( aListId );
    if ( list )
        {
        if ( KErrNone == list->RemoveContact( aContactId ) )
            {
            SendObserverEvent( TStorageManagerGlobals::EStorageEventContactDelete,
                               NULL,
                               aContactId );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindContact
// From MCAStoredContacts.
// Try to load with given ID, return NULL if not found.
// -----------------------------------------------------------------------------
MCAStoredContact* CCAStorageManager::FindContact( const TDesC& aContactListId,
                                                  const TDesC& aId )
    {
    CCAContactList* list = FindContactListInternal( aContactListId );
    if ( list )
        {
        return list->FindContact( aId );
        }
    CHAT_DP_FUNC_DONE( "CCAStorageManager::FindContact" );
    return NULL;
    }

MCAStoredContact* CCAStorageManager::FindContact( const TDesC& aContactListId,
                                                  const TDesC& aId,
                                                  MCAContactList*& aContactList )
    {
    CCAContactList* list = FindContactListInternal( aContactListId );
    aContactList = list;
    if ( list )
        {
        return list->FindContact( aId );
        }
    CHAT_DP_FUNC_DONE( "CCAStorageManager::FindContact" );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ContactInAnyList
// From MCAStoredContacts.
// -----------------------------------------------------------------------------
TBool CCAStorageManager::ContactInAllLists( const TDesC& aContactId ) const
    {
    TInt listCount( iContactList.Count() );
    for ( TInt a( 0 ); a < listCount; ++a )
        {
        if ( !iContactList[ a ]->FindContact( aContactId ) )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindContact
// From MCAStoredContacts.
// Try to load with given ID, return NULL if not found.
// -----------------------------------------------------------------------------
MCAStoredContact* CCAStorageManager::FindAnyContact( const TDesC& aContactId )
    {
    if ( aContactId.Compare( KPEngUserOwnPresenceId ) == 0 )
        {
        return iOwnStatus;
        }

    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MCAStoredContact* contact = iContactList[ a ]->FindContact( aContactId );
        if ( contact )
            {
            return contact;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindAnyContactByNick
// From MCAStoredContacts.
// Try to load with given nick, return NULL if not found.
// -----------------------------------------------------------------------------
MCAStoredContact* CCAStorageManager::FindAnyContactByNick( const TDesC& aNick )
    {
    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MCAStoredContact* contact =
            iContactList[ a ]->FindContactByNick( aNick );
        if ( contact )
            {
            return contact;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ListCount
// Count of contactlists
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::ListCount() const
    {
    return iContactList.Count();
    }


// -----------------------------------------------------------------------------
// CCAStorageManager::ListAt
// Reference to contact list
// -----------------------------------------------------------------------------
//
MCAContactList& CCAStorageManager::ListAt( TInt aIndex ) const
    {

    return *iContactList[ aIndex ];
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ListAt
// Reference to contact list
// -----------------------------------------------------------------------------
//
void CCAStorageManager::FindContacts( RPointerArray< MCAStoredContact >& aArray,
                                      const TDesC& aContactId ) const
    {
    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MCAStoredContact* contact = iContactList[a]->FindContact( aContactId );
        if ( contact )
            {
            TInt err( aArray.Append( contact ) );
            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                return;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ContactCount
// Count of contacts
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::ContactCount( TBool aSkipOfflineContacts ) const
    {
    TInt contactCount( 0 );
    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        contactCount += iContactList[ a ]->ContactCount( aSkipOfflineContacts,
                                                         EFalse );
        }
    return contactCount;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SetWVHiding
// From MCAStoredContacts
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetWVHiding( TBool aHiding )
    {
    iWVHiding = aHiding;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::WVHiding
// From MCAStoredContacts
// -----------------------------------------------------------------------------
//
TBool CCAStorageManager::WVHiding()
    {
    return iWVHiding;
    }
// -----------------------------------------------------------------------------
// CCAStorageManager::SetWVHidingPrefixOnly  UI CR : 101-39728
// From MCAStoredContacts
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetWVHidingPrefixOnly( TBool aHidingPrefixOnly )
    {
    iWVHidingPrefixOnly = aHidingPrefixOnly;
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CCAStorageManager::CapitalizingEnabled  UI CR : 101-39727
// From MCAStoredContacts
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetCapitalizingEnabled( TBool aCapital )
    {
    iCapital = aCapital;
    }
// CCAStorageManager::WVHidingPrefixOnly    UI CR : 101-39728
// From MCAStoredContacts
// -----------------------------------------------------------------------------
//
TBool CCAStorageManager::WVHidingPrefixOnly()
    {
    return iWVHidingPrefixOnly;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::CapitalizingEnabled   UI CR : 101-39727
// From MCAStoredContacts
// -----------------------------------------------------------------------------
//
TBool CCAStorageManager::CapitalizingEnabled()
    {
    return iCapital;
    }
// -----------------------------------------------------------------------------
// CCAStorageManager::CreateGroupL
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
MCAExtendedStoredGroup* CCAStorageManager::CreateGroupL()
    {
    CHAT_DP_FUNC_ENTER( "CreateGroupL" );

    if ( ! iStorage )
        {
        CHAT_DP_FUNC_DP( "CreateGroupL", "No Storage Manager" );
        User::Leave( KErrNotReady );
        }

    CCAGroup* group = CCAGroup::NewL( iStorage, this );

    // add to our list
    TInt status( iGroupList.Append( group ) ); // takes ownership (RPointerArray)
    if ( status != KErrNone )
        {
        delete group;
        User::Leave( status );
        }

    CHAT_DP_FUNC_DONE( "CreateGroupL" );
    return group;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::DeleteGroup
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::DeleteGroupL( MCAStoredGroup* aGroup )
    {
    CHAT_DP_FUNC_ENTER( "DeleteGroupL" );

    if ( ! iStorage )
        {
        CHAT_DP_FUNC_DP( "DeleteGroupL", "No Storage Manager" );
        User::Leave( KErrNotReady );
        }

    const TInt pos( FindGroupIndexByGroupId( aGroup->GroupId() ) );

    if ( KErrNotFound == pos )
        {
        // not in the list
        CHAT_DP_FUNC_DP( "DeleteGroupL", "Group not in the list, not deleting" );
        return;
        }

    // notify observers BEFORE deleting group
    SendObserverEvent( TStorageManagerGlobals::EStorageEventContactDelete,
                       aGroup->GroupId() );

    if ( aGroup->StorageType() == TStorageManagerGlobals::EStoragePersistent )
        {
        // is saved to persistent storage, so we can delete from
        // persistent storage
        CHAT_DP_FUNC_DP( "DeleteGroupL", "Deleting from persistent storage" );
        iStorage->DeleteL( ( MCAExtendedStoredGroup* ) aGroup );
        }
    // otherwise it's just in the internal list,
    // so delete from internal list
    CHAT_DP_FUNC_DP( "DeleteGroupL", "Deleting from internal list" );

    delete iGroupList[ pos ];
    iGroupList.Remove( pos );
    iGroupList.Compress();

    CHAT_DP_FUNC_DONE( "DeleteGroupL" );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindGroup
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
MCAStoredGroup* CCAStorageManager::FindGroup( const TDesC& aId )
    {
    const TInt pos( FindGroupIndexByGroupId( aId ) );
    return KErrNotFound == pos ? NULL : iGroupList[ pos ];
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::PopulateGroupsList
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::PopulateGroupsListL(
    RPointerArray<MCAStoredGroup>& aList,
    TBool aReturnOnlyVisible )
    {
    aList.Reset();

    if ( ! iStorage )
        {
        return;
        }

    const TInt count( iGroupList.Count() );
    for ( int i = 0; i < count; i++ )
        {
        if ( aReturnOnlyVisible )
            {
            if ( iGroupList[ i ]->IsVisible() )
                {
                // only add visible groups
                TInt status( aList.Append( iGroupList[ i ] ) );
                User::LeaveIfError( status );
                }
            }
        else
            {
            // add all groups
            TInt status( aList.Append( iGroupList[ i ] ) );
            User::LeaveIfError( status );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SignalGroupChangedL
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SignalGroupChangedL( const TDesC& aGroupId )
    {
    SendObserverEvent( TStorageManagerGlobals::EStorageEventGroupChange, aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SignalGroupAddedL
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SignalGroupAddedL( const TDesC& aGroupId )
    {
    SendObserverEvent( TStorageManagerGlobals::EStorageEventContactAddition, aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SaveGroupL
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SaveGroupL( const TDesC& aGroupId )
    {
    const TInt pos( FindGroupIndexByGroupId( aGroupId ) );

    if ( pos == KErrNotFound )
        {
        // no such group
        User::Leave( KErrNotFound );
        }

    iGroupList[ pos ]->SaveChangesL();
    SignalGroupAddedL( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::HideAllGroupsL
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HideAllGroups( TBool aAllGroups /*= EFalse*/ )
    {
    CHAT_DP_FUNC_ENTER( "HideAllGroups" );

    const TInt count( iGroupList.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        CCAGroup * group = iGroupList[i];
        if ( aAllGroups || group->IsOwnGroup() )
            {
            // only OWN groups are set to invisible if aAllGroups is EFalse..
            group->SetVisible( EFalse );
            }
        }

    CHAT_DP_FUNC_DONE( "HideAllGroups" );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ShowAllGroups
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::ShowAllGroups()
    {
    CHAT_DP_FUNC_ENTER( "ShowAllGroups" );

    const TInt count( iGroupList.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        CCAGroup * group = iGroupList[i];
        // ALL groups are set to visible...
        group->SetVisible( ETrue );
        }

    CHAT_DP_FUNC_DONE( "ShowAllGroups" );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::GroupCount
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::GroupCount( TBool aOnlyVisible ) const
    {
    if ( ! aOnlyVisible )
        {
        return iGroupList.Count();
        }

    // find out number of visible groups
    TInt count = iGroupList.Count();
    TInt grCount = 0;
    for ( TInt i = 0; i < count; i++ )
        {
        grCount += iGroupList[ i ]->IsVisible() ? 1 : 0;
        }

    return grCount;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::CheckFreespaceLevelL
// From MCAStoredGroups
// -----------------------------------------------------------------------------
//
void CCAStorageManager::CheckFreespaceLevelL( MCAExtendedStoredGroup* aGroup )
    {
    iStorage->CheckFreespaceLevelL( aGroup );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SetNotifyOnSave
// From MCAStoredContacts
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetNotifyOnSave( TBool aStatus )
    {
    iNotifyUponSave = aStatus;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::HandleDelete
// From MCAStoredContactsObserver
// We know when the contact is deleted, because it happens
// via Storage Manager, so this method unused.
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HandleContactDelete( const TDesC& /* aContact */ )
    {
    // unused
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::HandleAddition
// From MCAStoredContactsObserver
// We know when the contact is added, because it happens
// via Storage Manager, so this method unused.
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HandleAddition( MCAContactList& /*aList*/,
                                        MCAStoredContact& /* aContact */ )
    {
    // unused
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::HandleChange
// From MCAStoredContactsObserver
// The contact might be saved via CCAContact::SaveChangesL call,
// in which case we wouldn't know about it. But this observer
// call is invoked if the contact was SaveChangesL'd AND there was
// changed information.
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HandleChange( MCAContactList* aList,
                                      MCAStoredContact* aContact,
                                      TStorageManagerGlobals::TCAObserverEventType aEventType,
                                      TBool aUserIdChanged )
    {
    // contact was changed, possibly the contact WVID has changed (which is the
    // key).  keep our lists sorted. we do it by removing the old from the
    // list, then adding it again. it's faster (2*O(logN)) than sorting
    // (O(NlogN)).
    if ( !aContact && !aList )
        {
        return;
        }

    if ( iNotifyUponSave )
        {
        // ignore the event type.
        // this is a post-change as it comes directly from the CCAContact
        if ( aContact ) // contact
            {
            NotifyAllObserversWithDelay(
                TStorageManagerGlobals::EStorageEventPostChange,
                aContact, aContact->UserId(),
                NULL, aUserIdChanged );
            }
        else if ( aList )
            {
            NotifyAllObserversWithDelay(
                aEventType,
                NULL, KNullDesC,
                aList, aUserIdChanged );
            }
        else //own status
            {
            NotifyAllObserversWithDelay(
                TStorageManagerGlobals::EStorageEventOwnStatusChange,
                NULL, KPEngUserOwnPresenceId,
                NULL, aUserIdChanged );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::HandleBackupRestoreEvent
// From MCAStoredContactsObserver, MCAStoredGroupsObserver
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HandleBackupRestoreEvent(
    const TStorageManagerGlobals::TCAObserverEventType /* aEventType */ )
    {
    // unused
    }


// -----------------------------------------------------------------------------
// CCAStorageManager::HandleDelete
// From MCAStoredGroupsObserver
// All deletions happen via Storage Manager.
// But group hiding is handled as deletion from UI point of view.
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HandleDelete( const TDesC& aId )
    {
    SendObserverEvent( TStorageManagerGlobals::EStorageEventContactDelete, aId );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::HandleAddition
// From MCAStoredGroupsObserver
// Unused: all additions happen via Storage Manager
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HandleAddition( const TDesC& /* aId */ )
    {
    // unused
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::HandleChange
// From MCAStoredGroupsObserver
// The group might be saved via CCAContact::SaveChangesL call,
// in which case we wouldn't know about it. But this observer
// call is invoked if the group was SaveChangesL'd AND there was
// changed information.
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HandleChange( const TDesC& aId )
    {
    if ( iNotifyUponSave )
        {
        // ignoring error because can't do anything
        // if signal group change fails
        TInt ignore;
        TRAP( ignore, SignalGroupChangedL( aId ) );
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::HandleTimeWaited
// From MCARefreshTimerObserver
// -----------------------------------------------------------------------------
//
void CCAStorageManager::HandleTimeWaited( const TInt /* aError */ )
    {
    if ( !iContactListLock )
        {
        if ( iRefreshStartCounter == 1 )
            {
            // checking that contact still exists
            MCAStoredContact* contact = FindAnyContact( *iLatestContactToRefresh );
            MCAContactList* contactList = NULL;
            for ( TInt a( 0 ); a < iContactList.Count() && !contactList; ++a )
                {
                if ( iContactList[ a ] == iLatestContactList )
                    {
                    contactList = iLatestContactList;
                    }
                }
            if ( contact )
                {
                switch ( iRefreshEventType )
                    {
                    case TStorageManagerGlobals::EStorageEventPostChange:
                    case TStorageManagerGlobals::EStorageEventOwnStatusChange:
                        {
                        NotifyAllObservers( iRefreshEventType,
                                            contact, contact->UserId(),
                                            NULL, iRefreshUserIdChanged );
                        break;
                        }
                    default:
                        {
                        break;
                        }
                    }
                }
            else if ( contactList )
                {
                NotifyAllObservers( iRefreshEventType, NULL, KNullDesC, contactList, EFalse );
                }
            }
        else
            {
            for ( TInt i = 0; i < iContactObservers.Count(); i++ )
                {
                MCAStoredContactsObserver* obs = iContactObservers[i];
                obs->HandleChange(
                    NULL, NULL, /* multiple changes, contact id does not matter */
                    TStorageManagerGlobals::EStorageEventMultipleChanges,
                    iRefreshUserIdChanged );
                }
            }
        iRefreshStartCounter = 0;
        }
    }


// -----------------------------------------------------------------------------
// CCAStorageManager::CollapseLocked
// From MCAStorageInfo
// -----------------------------------------------------------------------------
//
TBool CCAStorageManager::CollapseLocked()
    {
    return iContactListLock;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ShowNickname
// From MCAStorageInfo
// -----------------------------------------------------------------------------
//
TBool CCAStorageManager::ShowNickname()
    {
    return iShowNickname;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::CreateContactL
// From MCAExtendedStoredContact
// -----------------------------------------------------------------------------
//
MCAStoredContact* CCAStorageManager::CreateContactL(
    const TDesC& aContactList,
    const TDesC& aNickname,
    const TDesC& aWVID )
    {
    CHAT_DP_FUNC_ENTER( "CreateContactL" );

    CCAContactList* contactList = FindContactListInternal( aContactList );
    if ( !contactList )
        {
        User::LeaveIfError( iOperationError );
        User::Leave( KErrNotFound );
        }

    CCAContact* contact = CCAContact::NewL( this, this );
    CleanupStack::PushL( contact );

    contact->SetUserIdL( aWVID );
    contact->SetNicknameL( aNickname );

    MCAStoredContact* addedContact = contactList->AddContactL( contact );

    TBool notify( EFalse );
    if ( addedContact == contact )
        {
        // contact added
        CleanupStack::Pop( contact );
        notify = ETrue;
        }
    else
        {
        // contact existed
        CleanupStack::PopAndDestroy( contact );
        // was something updated?
        if ( KErrNone != addedContact->Nickname().Compare( aNickname ) )
            {
            addedContact->SetNicknameL( aNickname );
            notify = ETrue;
            }
        if ( KErrNone != addedContact->UserId().Compare( aWVID ) )
            {
            addedContact->SetUserIdL( aWVID );
            notify = ETrue;
            }
        // something was updated, we need to resort the contact
        if ( notify )
            {
            contactList->ResortContact( addedContact );
            }
        }
    if ( notify )
        {
        SendObserverEvent( TStorageManagerGlobals::EStorageEventContactAddition,
                           addedContact,
                           addedContact->UserId(), contactList, EFalse );
        }
    return addedContact;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::CreateContactL
// -----------------------------------------------------------------------------
//
MCAStoredContact* CCAStorageManager::CreateContactL(
    MCAContactList& aContactList,
    const TDesC& aNickname,
    const TDesC& aWVID,
    TStorageManagerGlobals::TPresenceStatus aStatus,
    TStorageManagerGlobals::TClientType aType,
    const TDesC& aAlias,
    const TDesC& aStatusText )
    {
    CCAContactList& contactList = ( CCAContactList& )aContactList;

    CCAContact* contact = CCAContact::NewL( this, this );
    CleanupStack::PushL( contact );

    contact->SetUserIdL( aWVID );
    contact->SetNicknameL( aNickname );

    // update attributes
    contact->SetOnlineStatus( aStatus );
    contact->SetAliasL( aAlias );
    contact->SetStatusTextL( aStatusText );
    contact->SetClientType( aType );

    MCAStoredContact* addedContact = contactList.AddContactL( contact );
    // was contact inserted to array, or did it exists?
    if ( addedContact == contact )
        {
        CleanupStack::Pop( contact );
        }
    else
        {
        CleanupStack::PopAndDestroy( contact );
        // contact was already therem, update attributes and resort that contact
        // update attributes
        addedContact->SetOnlineStatus( aStatus );
        addedContact->SetAliasL( aAlias );
        addedContact->SetStatusTextL( aStatusText );
        addedContact->SetClientType( aType );
        contactList.ResortContact( addedContact );
        }

    // this is used at fetching, so do not notify any observer, will be done for whole list
    return addedContact;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SendContactListUpdatedEvent
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SendContactListUpdatedEvent( MCAContactList& aList )
    {
    SendObserverEvent( TStorageManagerGlobals::EStorageEventListChanged,
                       NULL,
                       KNullDesC, &aList, EFalse );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::RemoveAllContactsL
// -----------------------------------------------------------------------------
//
void CCAStorageManager::RemoveAllContactsL()
    {
    CHAT_DP_FUNC_ENTER( "RemoveAllContactsL" );

    // work backwards because the removal removes from
    // the array, in which case the length changes!
    iContactList.ResetAndDestroy();

    // Notify observers
    NotifyAllObservers( TStorageManagerGlobals::EStorageEventMultipleChanges,
                        NULL, KNullDesC(), NULL, EFalse );

    CHAT_DP_FUNC_DONE( "RemoveAllContactsL" );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SetShowNickname
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetShowNickname( TBool aShowNickname )
    {
    iShowNickname = aShowNickname;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ClearSelected
// -----------------------------------------------------------------------------
//
void CCAStorageManager::ClearSelected()
    {
    TInt listCount( iContactList.Count() );
    for ( TInt listI( 0 ); listI < listCount; ++listI )
        {
        MCAContactList& list = *iContactList[listI];
        list.SetSelected( EFalse );

        TInt count( list.Count() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            MCAStoredContact& contact = list[i];
            contact.SetSelected( EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::GetSelectedL
// -----------------------------------------------------------------------------
//
void CCAStorageManager::GetSelectedL( RPointerArray<MCAStoredContact>& aSelected,
                                      TStorageManagerGlobals::TFilterType aFilter )
    {
    TInt listCount( iContactList.Count() );

    for ( TInt listI( 0 ); listI < listCount; ++listI )
        {
        MCAContactList& list = *iContactList[listI];
        TBool listSelected( list.Selected() );

        TInt count( list.Count() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            MCAStoredContact& contact = list[i];

            if ( aFilter != TStorageManagerGlobals::EFilterAll )
                {
                // Check with the filter
                if ( ( listSelected || contact.Selected() )
                     && list.FilterAllowsContact( &contact, aFilter ) )
                    {
                    aSelected.AppendL( &contact );
                    }
                }
            else
                {
                // Do not use filter
                if ( listSelected || contact.Selected() )
                    {
                    aSelected.AppendL( &contact );
                    }
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CCAStorageManager::FindGroupIndexByGroupId
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::FindGroupIndexByGroupId( const TDesC& aId )
    {
    const TInt count( iGroupList.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        CCAGroup* group = iGroupList[i];

        if ( 0 == CAUtils::NeutralCompare( group->GroupId(), aId ) )
            {
            CHAT_DP_FUNC_DP( "FindGroupIndexByGroupId", "Found matching group" );
            // found it
            return i;
            }
        }

    CHAT_DP_FUNC_DP( "FindGroupIndexByGroupId", "No such group" );

    return KErrNotFound;
    }

// TLinearOrder
// -----------------------------------------------------------------------------
// CCAStorageManager::ContactListOrderByDisplayName
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::ContactListOrderByDisplayName(
    const CCAContactList& aContactListA,
    const CCAContactList& aContactListB )
    {
#ifdef IMPS_CONTACT_FETCH_BACKGROUND

    // Check fetch status
    TBool listANotFetched =
        aContactListA.Synchronised() == MCAContactList::ESynchroniseNotDone;
    TBool listBNotFetched =
        aContactListB.Synchronised() == MCAContactList::ESynchroniseNotDone;

    if ( listANotFetched && !listBNotFetched )
        {
        // list A not fetched, B fetched/failed => order B first
        return 1;
        }
    else if ( !listANotFetched && listBNotFetched )
        {
        // list A fetched/failed, B not fetched => order A first
        return -1;
        }
    else
        {
        // Both fetched/failed or both not fetched => order by display name
        return aContactListA.DisplayName().CompareC( aContactListB.DisplayName() );
        }

#else

    return aContactListA.DisplayName().CompareC( aContactListB.DisplayName() );

#endif // IMPS_CONTACT_FETCH_BACKGROUND    
    }

// TIdentityRelation
// -----------------------------------------------------------------------------
// CCAStorageManager::ContactListFindByContactId
// -----------------------------------------------------------------------------
//
TBool CCAStorageManager::ContactListFindByContactId(
    const CCAContactList& aContactListA,
    const CCAContactList& aContactListB )
    {
    const MCAContactList& listA = aContactListA;
    const MCAContactList& listB = aContactListB;
    return ( CAUtils::NeutralCompare( listA.ListId(),
                                      listB.ListId(), EFalse ) == 0 );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SetWatched
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetWatched( const TDesC& aContactId, TBool aStatus )
    {
    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MCAStoredContact* contact =
            iContactList[ a ]->FindContact( aContactId );
        if ( contact )
            {
            contact->SetWatched( aStatus );
            contact->SignalChanges();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SetWatched
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetPendingMessages( const TDesC& aContactId, TInt aAmount )
    {
    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MCAStoredContact* contact =
            iContactList[ a ]->FindContact( aContactId );
        if ( contact )
            {
            contact->SetPendingMessages( aAmount );
            contact->SignalChanges();
            }
        }
    }
// -----------------------------------------------------------------------------
// CCAStorageManager::SetBlocked
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetBlocked( const TDesC& aContactId, TBool aStatus )
    {
    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MCAStoredContact* contact =
            iContactList[ a ]->FindContact( aContactId );
        if ( contact )
            {
            contact->SetBlocked( aStatus );
            contact->SignalChanges();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SetBlocked
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetBlocked( const TDesC& aContactId,
                                    const TDesC& aListId,
                                    TBool aStatus )
    {
    CCAContactList* list = FindContactListInternal( aListId );
    if ( list )
        {
        MCAStoredContact* contact = list->FindContact( aContactId );
        if ( contact )
            {
            contact->SetBlocked( aStatus );
            contact->SignalChanges();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::Sort
/// KNullDesC -> sort all
// -----------------------------------------------------------------------------
//
void CCAStorageManager::Sort( const TDesC& aContactListId /* = KNullDesC */ )
    {
    if ( aContactListId.Length() != 0 )
        {
        MCAContactList* list = FindContactList( aContactListId );
        if ( list )
            {
            list->Sort();
            }
        }
    else
        {
        TInt count( iContactList.Count() );
        for ( TInt a( 0 ); a < count; ++a )
            {
            MCAContactList& list = *iContactList[ a ];
            list.Sort();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::Sort
/// KNullDesC -> sort all
// -----------------------------------------------------------------------------
//
void CCAStorageManager::ResortContactInAll( MCAStoredContact* aContact )
    {
    TInt count( iContactList.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iContactList[ x ]->ResortUnKnownContact( aContact );
        }
    }


// -----------------------------------------------------------------------------
// CCAStorageManager::SetSortAlgorithm
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetSortAlgorithm(
    MCAStoredContact::TSortAlgorithm aAlgorithm )
    {
    iContactSorter->SetSortAlgorithm( aAlgorithm );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::OnlineStatus
// -----------------------------------------------------------------------------
//
TStorageManagerGlobals::TPresenceStatus CCAStorageManager::OnlineStatus(
    const TDesC& aContactId )
    {
    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MCAStoredContact* contact = iContactList[ a ]->FindContact( aContactId );
        if ( contact )
            {
            return contact->OnlineStatus();
            }
        }
    return TStorageManagerGlobals::EUnknown;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::UpdatePresenceL
// -----------------------------------------------------------------------------
//
MCAStoredContact* CCAStorageManager::UpdatePresenceL(
    const TDesC& aContactId,
    TStorageManagerGlobals::TPresenceStatus aStatus,
    TStorageManagerGlobals::TClientType aType,
    const TDesC& aAlias,
    const TDesC& aStatusText,
    TStorageManagerGlobals::TPresenceStatus& aOldOnlineStatus )
    {
    RPointerArray< MCAStoredContact > tempArray;
    CleanupClosePushL( tempArray );
    RPointerArray< MCAContactList > listArray;
    CleanupClosePushL( listArray );
    PopulateContactArrayL( listArray, tempArray, aContactId );
    MCAStoredContact* contact = NULL;
    MCAStoredContact* watchedContact = NULL;
    TInt count( tempArray.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        contact = tempArray[ a ];
        TBool updated( EFalse );
        aOldOnlineStatus = contact->OnlineStatus();
        if ( aOldOnlineStatus != aStatus )
            {
            contact->SetOnlineStatus( aStatus );
            updated = ETrue;
            }
        if ( KErrNone != aAlias.Compare( contact->Alias() ) )
            {
            contact->SetAliasL( aAlias );
            updated = ETrue;
            }
        contact->SetStatusTextL( aStatusText );
        contact->SetClientType( aType );
        if ( updated )
            {
            // contacts online state was updated, resort the contact
            listArray[ a ]->ResortContact( contact );
            contact->SignalChanges();
            watchedContact = contact;
            }
        }
    CleanupStack::PopAndDestroy( 2 ); // listArray, tempArray
    return watchedContact;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::PopulateContactArray
// -----------------------------------------------------------------------------
//
void CCAStorageManager::PopulateContactArrayL(
    RPointerArray< MCAContactList >& aListArray,
    RPointerArray< MCAStoredContact >& aContactArray, const TDesC& aContactId )
    {
    TInt count( iContactList.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MCAStoredContact* contact =
            iContactList[ a ]->FindContact( aContactId );
        if ( contact )
            {
            // also populate lists of lists
            aListArray.AppendL( iContactList[ a ] );
            aContactArray.AppendL( contact );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::Identification
// -----------------------------------------------------------------------------
//
const TPtrC CCAStorageManager::Identification( const TDesC& aContactId )
    {
    if ( aContactId.Length() == 0 )
        {
        TPtrC retVal( KNullDesC );
        return retVal;
        }

    // search from contact lists
    MCAStoredContact* contact = FindAnyContact( aContactId );
    if ( contact )
        {
        return contact->Identification();
        }
    else
        {
        // could not find contact from contact lists,
        // check if it's my own id
        if ( 0 == CAUtils::NeutralCompare( aContactId, iOwnStatus->UserId() ) )
            {
            return iOwnStatus->Identification();
            }
        }
    return CAUtils::DisplayId( aContactId );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::OwnStatus
// -----------------------------------------------------------------------------
//
MCAStoredContact& CCAStorageManager::OwnStatus()
    {
    return *iOwnStatus;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SetContactListLock
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetContactListLock( TBool aLocked )
    {
    TBool notify( !aLocked && iContactListLock );
    iContactListLock = aLocked;
    if ( notify )
        {
        SendObserverEvent( TStorageManagerGlobals::EStorageEventMultipleChanges,
                           NULL,
                           KNullDesC );
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindContactListInterna
// -----------------------------------------------------------------------------
//
void CCAStorageManager::ResetContactLists()
    {
    TInt count( ListCount() );
    TBool collapsed( count == 1 ? EFalse : ETrue );

    for ( TInt a( 0 ); a < count; ++a )
        {
        ListAt( a ).SetCollapsed( collapsed );
        }
    if ( count )
        {
        SendObserverEvent( TStorageManagerGlobals::EStorageEventMultipleChanges,
                           NULL, KNullDesC );
        }

    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindContactListInternal
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetContactListProperty( TContactListProperty aProperty )
    {
    TBool primaryInUse( aProperty == EPrimaryCollapseInUse );

    TInt count( iContactList.Count() );
    switch ( aProperty )
        {
        case EPrimaryCollapseInUse:
        case ESecondaryCollapseInUse:
            {
            for ( TInt a( 0 ); a < count; ++a )
                {
                iContactList[ a ]->SetPrimaryInuse( primaryInUse );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::InitSynchroniseProcess
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::InitSynchroniseProcess()
    {
    // Set all failed to NotDone
    TInt count = iContactList.Count();
    for ( TInt a = count - 1; a >= 0; --a )
        {
        if ( iContactList[ a ]->Synchronised() == MCAContactList::ESynchroniseFailed )
            {
            iContactList[ a ]->SetSynchronised( MCAContactList::ESynchroniseNotDone );
            }
        }
    return iCurrentListSync = FindNextForSynch( 0 );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::InitSynchroniseProcess
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::SetNextForSynchroniseProcess()
    {
    return iCurrentListSync = FindNextForSynch( iCurrentListSync );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::IsAllSynchronised
// -----------------------------------------------------------------------------
//
TBool CCAStorageManager::IsAllSynchronised( TBool &aIsFailed ) const
    {
    TInt count = iContactList.Count();
    aIsFailed = EFalse;
    TBool isSync = ETrue;
    for ( TInt a = count - 1; a >= 0; --a )
        {
        MCAContactList::TSynchroniseState syncState = iContactList[ a ]->Synchronised();
        if ( syncState == MCAContactList::ESynchroniseNotDone )
            {
            isSync = EFalse;
            }
        else if ( syncState == MCAContactList::ESynchroniseFailed )
            {
            aIsFailed = ETrue;
            }
        }
    return isSync;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::ListInSyncProcessL
// -----------------------------------------------------------------------------
//
MCAContactList& CCAStorageManager::ListInSyncProcessL()
    {
    __CHAT_ASSERT_DEBUG( iCurrentListSync < iContactList.Count() );
    User::LeaveIfError( iCurrentListSync );
    return *iContactList[ iCurrentListSync ];
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindContactListInternal
// -----------------------------------------------------------------------------
//
CCAContactList* CCAStorageManager::FindContactListInternal(
    const TDesC& aListId )
    {
    CHAT_DP_FUNC_ENTER( "CCAStorageManager::FindContact" );
    TInt contactListIndex( FindContactListById( aListId ) );
    return ( contactListIndex >= 0 ? iContactList[ contactListIndex ] : NULL );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindContactList
// -----------------------------------------------------------------------------
//
MCAContactList* CCAStorageManager::FindContactList( const TDesC& aListId )
    {
    return FindContactListInternal( aListId );
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::FindContactListById
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::FindContactListById(
    const TDesC& aListId )
    {
    TInt count( iContactList.Count() );
    for ( TInt x ( 0 ) ; x < count ; ++x )
        {
        if ( KErrNone == CAUtils::NeutralCompare(
                 aListId,
                 iContactList[ x ]->ListId(), EFalse )
           )
            {
            return x;
            }
        }
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CCAStorageManager::FindContactListById
// -----------------------------------------------------------------------------
//
TInt CCAStorageManager::FindNextForSynch( TInt aOffset )
    {
    TInt offset( aOffset == KErrNotFound ? 0 : aOffset );
    TInt count = iContactList.Count();
    TInt currentListSync = KErrNotFound;
    while ( currentListSync == KErrNotFound && offset < count )
        {
        if ( iContactList[ offset ]->Synchronised() == MCAContactList::ESynchroniseNotDone )
            {
            currentListSync = offset;
            }
        ++offset;
        }
    return currentListSync;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::IsValidContact
// -----------------------------------------------------------------------------
//
TBool CCAStorageManager::IsContactValid( const MCAStoredContact* aContact ) const
    {
    // own status
    if ( aContact == iOwnStatus )
        {
        // valid
        return ETrue;
        }

    // search lists
    TInt count = iContactList.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iContactList[ i ]->FindIndexOfContact( aContact ) != KErrNotFound )
            {
            // contact found -> valid
            return ETrue;
            }
        }

    // contact not found
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAStorageManager::SortContactLists
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SortContactLists()
    {
    SendObserverEvent( TStorageManagerGlobals::EStorageEventPreChange,
                       NULL, KNullDesC );

    TLinearOrder< CCAContactList > order(
        CCAStorageManager::ContactListOrderByDisplayName );

    iContactList.Sort( order );

    SendObserverEvent( TStorageManagerGlobals::EStorageEventMultipleChanges,
                       NULL, KNullDesC );
    }
// -----------------------------------------------------------------------------
// CCAStorageManager::IsGroupDeleted
// -----------------------------------------------------------------------------
//

TBool CCAStorageManager::IsGroupDeleted()
    {
    return 	iGroupDeleted;
    }
// -----------------------------------------------------------------------------
// CCAStorageManager::SetGroupDeleted
// -----------------------------------------------------------------------------
//
void CCAStorageManager::SetGroupDeleted( TBool aValue )
    {
    iGroupDeleted = aValue;
    }
//  End of File
