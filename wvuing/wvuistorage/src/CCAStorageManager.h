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
* Description:  Storage manager header file
*
*/


#ifndef CCASTORAGEMANAGER_H
#define CCASTORAGEMANAGER_H

#include "impsbuilddefinitions.h"

//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <babackup.h>

#include "MCAStoredContacts.h"
#include "TCAStoragePanics.h"
#include "TStorageManagerGlobals.h"
#include "MCAStoredGroups.h"
#include "MCAStoredContactsObserver.h"
#include "MCAStoredGroupsObserver.h"
#include "MCARefreshTimerObserver.h"
#include "MCAStorageInfo.h"

// FORWARD DECLARATIONS
class CBaBackupSessionWrapper;
class CCAContactList;
class MCAStoredContactsObserver;
class CCAStorage;
class CCAGroup;
class CCARefreshTimer;
class CCAContactSorter;


// CLASS DECLARATION
/**
 * Storage Manager.
 *
 * Note! The following methods do not allow themselves to be called if backup
 * is in progress (use MBackupOperationObserver or CBaBackupSessionWrapper).
 * In such a case, the methods either leave or return without doing anything.
 *
 * - AddContactL (both variants)
 * - RemoveContactL
 * - PopulateContactsList
 * - CreateGroupL
 * - DeleteGroupL
 * - PopulateGroupsList
 *
 * After the backup has finished, an HandleListInvalidation event is sent to
 * observers.
 *
 * @lib CAStorage.dll
 * @since 3.0
 */
class CCAStorageManager : public CBase,
            public MCAStoredContacts,
            public MCAStoredGroups,
            public MCAStoredContactsObserver,
            public MCAStoredGroupsObserver,
            public MCARefreshTimerObserver,
            public MCAStorageInfo
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CCAStorageManager* NewL();

        /**
         * Destructor.
         */
        ~CCAStorageManager();

    public: // New functions
        /**
         * Creates instance of storage
         * @return CCAStorageManager pointer
         */
        static CCAStorageManager* InstanceL();

        /**
         * Release instance of storage
         * @return KErrNone if released.
         */
        static TInt ReleaseL();

    public: // Functions from base classes

        /**
         * From MCAStoredContacts
         */
        void AddObserverL( MCAStoredContactsObserver* aObserver );

        /**
         * From MCAStoredContacts
         */
        void RemoveObserver( MCAStoredContactsObserver* aObserver );

        /**
         * From MCAStoredContacts
         */
        MCAContactList* AddContactListIdL( const TDesC& aContactListId,
                                           const TDesC& aDisplayName,
                                           TBool aNotify = ETrue );

        /**
         * From MCAStoredContacts
         */
        void RemoveContactList( const TDesC& aContactListId );

        /**
         * From MCAStoredContacts
         */
        void SetContactListCollapsed( const TDesC& aContactListId,
                                      TBool aCollapsed );

        /**
         * From MCAStoredContacts
         */
        void RemoveContact( const TDesC& aListId, const TDesC& aContactId );

        /**
         * From MCAStoredContacts
         */
        MCAStoredContact* FindContact( const TDesC& aContactListId,
                                       const TDesC& aId );

        MCAStoredContact* FindContact( const TDesC& aContactListId,
                                       const TDesC& aId,
                                       MCAContactList*& aContactList );
        /**
         * From MCAStoredContacts
         */
        MCAStoredContact* FindAnyContact( const TDesC& aContactId );

        /**
         * From MCAStoredContacts
         */
        MCAStoredContact* FindAnyContactByNick( const TDesC& aNick );

        /**
         * From MCAStoredContacts
         */
        TInt FindContact( const MCAStoredContact& aContact ) const;

        /**
         * From MCAStoredContacts
         */
        TBool ContactInAllLists( const TDesC& aContactId ) const;

        /**
         * From MCAStoredContacts
         */
        TInt ListCount() const;

        /**
         * From MCAStoredContacts
         */
        MCAContactList& ListAt( TInt aIndex ) const;

        /**
         * From MCAStoredContacts
         */
        void FindContacts( RPointerArray< MCAStoredContact >& aArray,
                           const TDesC& aContactId ) const;

        /**
         * From MCAStoredContacts
         */
        TInt ContactCount( TBool aSkipOfflineContacts = EFalse ) const;

        /**
         * From MCAStoredContacts
         */
        void SetWVHiding( TBool aHiding );

        /**
         * From MCAStoredContacts
         */
        TBool WVHiding();
        /**
         * From MCAStoredContacts
         */
        void SetWVHidingPrefixOnly( TBool aHidingPrefixOnly ); //  UI CR ID :101-39728: Show domain part of the user ID


        /**
         * From MCAStoredContacts
         */
        TBool WVHidingPrefixOnly();   // UI CR ID: 101-39728: Show domain part of the user ID

        /**
        * From MCAStoredContacts
        */
        void SetCapitalizingEnabled( TBool aCapital );  // UI CR ID: 101-39727

        /**
         * From MCAStoredContacts
         */
        TBool CapitalizingEnabled();  // UI CR ID: 101-39727

        /**
         * From MCAStoredContacts
         */
        void SetWatched( const TDesC& aContactId, TBool aStatus );

        /**
         * From MCAStoredContacts
         */
        void SetPendingMessages( const TDesC& aContactId, TInt aAmount );

        /**
         * From MCAStoredContacts
         */
        void SetBlocked( const TDesC& aContactId, TBool aStatus );

        /**
         * From MCAStoredContacts
         */
        void SetBlocked( const TDesC& aContactId,
                         const TDesC& aListId,
                         TBool aStatus );

        /**
         * From MCAStoredContacts
         */
        void Sort( const TDesC& aContactListId = KNullDesC );

        /**
         * Resort one contact, what ever contact list it is in
         */
        void ResortContactInAll( MCAStoredContact* aContact );

        /**
         * From MCAStoredContacts
         */
        void SetSortAlgorithm( MCAStoredContact::TSortAlgorithm aAlgorithm );

        /**
         * From MCAStoredContacts
         */
        TStorageManagerGlobals::TPresenceStatus OnlineStatus(
            const TDesC& aContactId );

        /**
         * From MCAStoredContacts
         */
        MCAStoredContact* UpdatePresenceL(   const TDesC& aContactId,
                                             TStorageManagerGlobals::TPresenceStatus aStatus,
                                             TStorageManagerGlobals::TClientType aType,
                                             const TDesC& aAlias,
                                             const TDesC& aStatusText,
                                             TStorageManagerGlobals::TPresenceStatus& aOldOnlineStatus );
        /**
         * From MCAStoredContacts
         */
        const TPtrC Identification( const TDesC& aContactId );

        /**
         * From MCAStoredContacts
         */
        MCAStoredContact& OwnStatus();

        /**
         * From MCAStoredContacts
         */
        void SetContactListLock( TBool aLocked );

        /**
         * From MCAStoredContacts
         */
        void ResetContactLists();

        /**
         * From MCAStoredContacts
         */
        void SetContactListProperty( TContactListProperty aProperty );

        /**
         * From MCAStoredContacts
         */
        TInt InitSynchroniseProcess();

        /**
         * From MCAStoredContacts
         */
        TInt SetNextForSynchroniseProcess();

        /**
         * From MCAStoredContacts
         */
        MCAContactList& ListInSyncProcessL();

        /**
         * From MCAStoredContacts
         */
        TBool IsAllSynchronised( TBool &aIsFailed ) const;

        /**
         * From MCAStoredGroups
         */
        void AddObserverL( MCAStoredGroupsObserver* aObserver );

        /**
         * From MCAStoredGroups
         */
        void RemoveObserver( MCAStoredGroupsObserver* aObserver );

        /**
         * From MCAStoredGroups
         */
        MCAExtendedStoredGroup* CreateGroupL();

        /**
         * From MCAStoredGroups
         */
        void DeleteGroupL( MCAStoredGroup* aGroup );

        /**
         * From MCAStoredGroups
         */
        MCAStoredGroup* FindGroup( const TDesC& aId );

        /**
         * From MCAStoredGroups
         */
        void PopulateGroupsListL( RPointerArray<MCAStoredGroup>& aList,
                                  TBool aReturnOnlyVisible );

        /**
         * From MCAStoredGroups
         */
        void SignalGroupChangedL( const TDesC& aGroupId );

        /**
         * From MCAStoredGroups
         */
        void SignalGroupAddedL( const TDesC& aGroupId );

        /**
         * From MCAStoredGroups
         */
        void SaveGroupL( const TDesC& aGroupId );

        /**
         * From MCAStoredGroups
         */
        virtual void HideAllGroups( TBool aAllGroups = EFalse );

        /**
         * From MCAStoredGroups
         */
        virtual void ShowAllGroups();

        /**
         *  Count of stored groups
         *  @return Count of groups
         */
        virtual TInt GroupCount( TBool aOnlyVisible ) const;

        /**
         * From MCAStoredGroups
         */
        virtual void CheckFreespaceLevelL( MCAExtendedStoredGroup* aGroup );

        /**
         * From MCAStoredContacts
         */
        void SetNotifyOnSave( TBool aStatus );

        /**
         * MCAStoredContactsObserver
         */
        void HandleContactDelete( const TDesC& aContact );

        /**
         * MCAStoredContactsObserver
         */
        void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact );

        /**
         * MCAStoredContactsObserver
         */
        void HandleChange( MCAContactList* aList, MCAStoredContact* aContact,
                           TStorageManagerGlobals::TCAObserverEventType aEventType,
                           TBool aUserIdChanged );

        /**
         * MCAStoredContactsObserver
         * MCAStoredGroupsObserver
         * This method is unused.
         */
        void HandleBackupRestoreEvent(
            const TStorageManagerGlobals::TCAObserverEventType aEventType );

        /**
         * MCAStoredGroupsObserver
         */
        void HandleDelete( const TDesC& aId );

        /**
         * MCAStoredGroupsObserver
         */
        void HandleAddition( const TDesC& aId );

        /**
         * MCAStoredGroupsObserver
         */
        void HandleChange( const TDesC& aId );

        /**
         * MCAStoredContacts
         */
        MCAStoredContact* CreateContactL(
            const TDesC& aContactList,
            const TDesC& aNickname,
            const TDesC& aWVID );

        /**
         * MCAStoredContacts
         */
        MCAStoredContact* CreateContactL(
            MCAContactList& aContactList,
            const TDesC& aNickname,
            const TDesC& aWVID,
            TStorageManagerGlobals::TPresenceStatus aStatus,
            TStorageManagerGlobals::TClientType aType,
            const TDesC& aAlias,
            const TDesC& aStatusText );

        /**
         * MCAStoredContacts
         */
        void SendContactListUpdatedEvent( MCAContactList& aList );


        /**
         * From MCAStoredContacts
         */
        void RemoveAllContactsL();

        /**
         * From MCAStoredContacts
         */
        void SetShowNickname( TBool aShowNickname ) ;

        /**
         * Clears all selected-flags
         */
        void ClearSelected();

        /**
         * Gets all selected contacts
         */
        void GetSelectedL( RPointerArray<MCAStoredContact>& aSelected,
                           TStorageManagerGlobals::TFilterType aFilter );

        /**
         * From MCARefreshTimerObserver
         */
        void HandleTimeWaited( const TInt aError );

        /**
         * From MCAStorageInfo
         */
        TBool ShowNickname();

        /**
         * From MCAStorageInfo
         */
        TBool CollapseLocked();

        /**
         * From MCAStoredContacts
         */
        TBool IsContactValid( const MCAStoredContact* aContact ) const;

        /**
         * From MCAStoredContacts
         */
        void SortContactLists();


        /**
        * From MCAStoredContacts
        */
        TBool IsGroupDeleted();

        /**
         * From MCAStoredContacts
         */
        void SetGroupDeleted( TBool aValue );

    private:

        /**
         * C++ default constructor.
         */
        CCAStorageManager();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        // internal contact list managing helper routines

        /**
         * Find the index of the list element in our list of groups that
         * has the given group id.
         * @since 2.1
         * @param aId The id to search
         * @return The index in the list, or KErrNotFound
         */
        TInt FindGroupIndexByGroupId( const TDesC& aId );

        /**
         * Helper routine for handling observer events for contacts
         * @param aType The type of observer event
         * @param aContact Event occured for contact.
         * @param aContactId, Id of contact
         * @param aList Contact list where contact can be found
         * @param aUserIdChanged, Is user id changed or not.
         */
        void SendObserverEvent(
            TStorageManagerGlobals::TCAObserverEventType aType,
            MCAStoredContact* aContact,
            const TDesC& aContactId,
            MCAContactList* aList = NULL,
            TBool aUserIdChanged = EFalse );

        /**
         * Helper routine for handling observer events for groups
         * @since 2.1
         * @param aType The type of observer event
         * @param aParameter The group id
         */
        void SendObserverEvent(
            TStorageManagerGlobals::TCAObserverEventType aType,
            const TDesC& aParameter );

        /**
         * Helper routine for backup and restore events
         * @param aType The event type
         * @param aParam The parameter
         */
        void SendBRObserverEvent(
            TStorageManagerGlobals::TCAObserverEventType aType );

        /**
         * Synchronize groups with persistent storage. Called from Constructor.
         * @since 2.1
         */
        void SynchronizeGroupsL();

        /**
         * Notify all the observers about the given event
         * @param aType The type of observer event
         * @param aContact The contact in question
         * (or -1 == KCAStorageDontKnowPos to force search for given contact id
         * in aParameter)
         * @param aFromPEC, ETrue, if event is coming from PEC
         * @since 2.1
         */
        void NotifyAllObservers( TStorageManagerGlobals::TCAObserverEventType aType,
                                 MCAStoredContact* aContact,
                                 const TDesC& aContactId,
                                 MCAContactList *aList,
                                 TBool aUserIdChanged );

        /**
         * Notify all the observer about the given event after small delay
         * If new notify events come during delay, then notify is "shifted"
         * with new delay time and notify event is change to "multiplechanges".
         * @param aType The type of observer event
         * @param aContact The contact in question
         * (or -1 == KCAStorageDontKnowPos to force search for given contact id
         * in aParameter)
         * @param aFromPEC, ETrue, if event is coming from PEC
         * @since 2.6
         */
        void NotifyAllObserversWithDelay(
            TStorageManagerGlobals::TCAObserverEventType aType,
            MCAStoredContact* aContact, const TDesC& aContactId,
            MCAContactList* aList,
            TBool aUserIdChanged );

        /**
         * Actual implementation for NotifyAllObserversWithDelay
         * @see NotifyAllObserversWithDelay
         */
        void DoNotifyAllObserversWithDelayL(
            TStorageManagerGlobals::TCAObserverEventType aType,
            MCAStoredContact* aContact, const TDesC& aContactId,
            MCAContactList* aList,
            TBool aUserIdChanged );

        // TLinearOrder
        static TInt ContactListOrderByDisplayName(
            const CCAContactList& aContactA,
            const CCAContactList& aContactB );

        static TBool ContactListFindByContactId(
            const CCAContactList& aContactA,
            const CCAContactList& aContactB );

        /**
         * Find contact list for internal use.
         * @param aListId. List to find.
         * @return Contact list if found. if not found return NULL
         */
        CCAContactList* FindContactListInternal( const TDesC& aListId );

        /**
         * Find contact list
         * @param aListId. List to find.
         * @return Contact list if found. If not found return NULL
         */
        MCAContactList* FindContactList( const TDesC& aListId );

        TInt FindContactListById( const TDesC& aListId );

        /**
         * Populate contact array
         * @param aArray. Array to be populated by contacts.
         * @param aContactId. Id of contacts which are appended to array.
         */
        void PopulateContactArrayL( RPointerArray< MCAContactList >& aListArray,
                                    RPointerArray< MCAStoredContact >& aContactArray,
                                    const TDesC& aContactId );

        /**
         * Find next list for sync
         * @param Offset for earlier item.
         */
        TInt FindNextForSynch( TInt aOffset );

    private:    // Data

        // observers list, used with MCAStoredContacts inherited methods
        RPointerArray< MCAStoredContactsObserver > iContactObservers;
        // owns the array, but not the observers

        // observers list, used with MCAStoredGroups inherited methods
        RPointerArray< MCAStoredGroupsObserver > iGroupObservers;
        // owns the array, but not the observers

        // list of contacts
        RPointerArray< CCAContactList > iContactList;   // owns

        // list of groups
        RPointerArray< CCAGroup > iGroupList;   // owns

        // the persistent storage
        CCAStorage* iStorage;   // owns

        // the backup server session
        CBaBackupSessionWrapper* iBackupSession; // owns

        // notify upon save or not
        TBool iNotifyUponSave;

        // hide "wv:" from contact identification (ETrue)
        TBool iWVHiding;

        // Timer used to wait before calling HandleChange to observers.
        CCARefreshTimer* iRefreshTimer; // owns

        // Helper variables for shifted change events.
        TInt iRefreshStartCounter;
        HBufC*  iLatestContactToRefresh; // WVID of latest contact
        MCAContactList* iLatestContactList; // latest contactlist
        TStorageManagerGlobals::TCAObserverEventType iRefreshEventType;
        TBool iRefreshUserIdChanged;

        /// Sorter for contacts. Owns
        CCAContactSorter* iContactSorter;

        /// Contact for own status
        MCAStoredContact* iOwnStatus;

        /// Operation error
        TInt iOperationError;

        /// Contact list lock status ETrue locked, EFalse unlocked.
        TBool iContactListLock;

        // To show nickname or not
        TBool iShowNickname;

        // Current contact list to be synchronised.
        TInt iCurrentListSync;
        // hide "wv:" from contact identification (ETrue) only wv: part
        TBool iWVHidingPrefixOnly;
        // capitalizig the listname
        TBool iCapital;


        TBool iGroupDeleted;

    };

#endif      // CCASTORAGEMANAGER_H

// End of File
