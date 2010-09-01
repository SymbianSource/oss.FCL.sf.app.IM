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
* Description:  Stored contact data container interface
*
*/



#ifndef MCASTOREDCONTACTS_H
#define MCASTOREDCONTACTS_H

#include "impsbuilddefinitions.h"
#include "TStorageManagerGlobals.h"

#include <e32std.h>
#include <badesca.h>
#include "MCAStoredContact.h"

// FORWARD DECLARATIONS
class MCAStoredContactsObserver;
class MCAContactList;

// CLASS DECLARATION

/**
 *  Interface for stored contacts container
 *
 *  @lib CAStorage.dll
 *  @since 3.0
 */
class MCAStoredContacts
    {
    public: // Definitions

        enum TContactListProperty
            {
            EPrimaryCollapseInUse,
            ESecondaryCollapseInUse
            };

    public: // New functions

        /**
         * Add an observer.
         * @param aObserver The observer
         */
        virtual void AddObserverL( MCAStoredContactsObserver* aObserver ) = 0;

        /**
         * Remove an observer.
         * @param aObserver The observer
         */
        virtual void RemoveObserver(
            MCAStoredContactsObserver* aObserver ) = 0;

        /**
         * Remove a given contact
         * @param aListId List which contains contact.
         * @param aContactId The contact to remove
         */
        virtual void RemoveContact( const TDesC& aListId,
                                    const TDesC& aContactId ) = 0;

        /**
         * Find a contact from the store by wv User ID
         * @param aContactListId List which contains contact.
         * @param aId the User ID of the contact to find
         * @return The contact
         */
        virtual MCAStoredContact* FindContact( const TDesC& aContactListId,
                                               const TDesC& aId ) = 0;

        virtual MCAStoredContact* FindContact( const TDesC& aContactListId,
                                               const TDesC& aId,
                                               MCAContactList*& aContactList ) = 0;

        /**
         * Find any contact which id is aContactId
         * @param aContactId Contactid to recognize contact.
         * @return First one which matches contactId. If not found return NULL
         */
        virtual MCAStoredContact* FindAnyContact( const TDesC& aContactId ) = 0;

        /**
         * Find any contact which nick is aNick
         * @param aNick Nickname to recognize contact.
         * @return First one which matches nickname. If not found return NULL
         */
        virtual MCAStoredContact* FindAnyContactByNick( const TDesC& aNick ) = 0;

        /**
         * Count of lists.
         * @return Count of lists.
         */
        virtual TInt ListCount() const = 0;

        /**
         * List in index.
         * @param aIndex. Index of list.
         * return Contact list.
         */
        virtual MCAContactList& ListAt( TInt aIndex ) const = 0;

        /**
         * Find contact list by id
         * @param aListId. Id to find.
         * @return Contactlist. If not found return NULL
         */
        virtual MCAContactList* FindContactList( const TDesC& aListId ) = 0;

        /**
         * Ownership of contacts does not go to the caller.
         * MUST NOT delete contacts in array.
         * @param aArray Array where contacts are appended.
         * @param aContacftId Id of contacts that are appended to array.
         */
        virtual void FindContacts( RPointerArray< MCAStoredContact >& aArray,
                                   const TDesC& aContactId ) const = 0;

        /**
         * Find out if contact exists in all list.
         * @param aContactId. Id to check.
         * @return ETrue if contact is in all lists. EFalse if not.
         */
        virtual TBool ContactInAllLists( const TDesC& aContactId ) const = 0;

        /**
         * Add new contact list.
         * @param aContactListId Id for new contact list.
         * @param aDisplayName Display name of new contact list.
         */
        virtual MCAContactList* AddContactListIdL( const TDesC& aContactListId,
                                                   const TDesC& aDisplayName,
                                                   TBool aNotify = ETrue ) = 0;

        /**
         * Remove contact list from store.
         * @param aContactListId. Id of contact list to be removed.
         */
        virtual void RemoveContactList( const TDesC& aContactListId ) = 0;

        /**
         * Set collapsed status of contact list.
         * @param aContactListId. Id of contact list to be collapsed
         *                        or expanded.
         * @param aCollapsed. Status of collapsed. ETrue if collapsed.
         *                    EFalse if expanded.
         */
        virtual void SetContactListCollapsed( const TDesC& aContactListId,
                                              TBool aCollapsed ) = 0;


        /**
         * Creates an empty contact. The contact is also made to phone book.
         * The caller needs to set appropriate data to the contact,
         * and call the SaveChangesL method afterwards.
         * Ownership of contact is in storage.
         * @param aContactListId, Id of contactlist where contact is created.
         * @param aNickname The nick name, or KNullDesC if there's no nickname
         * @param aWVID The wv id
         * @return The created contact, or NULL if the wvid already existed
         */
        virtual MCAStoredContact* CreateContactL(
            const TDesC& aContactListId,
            const TDesC& aNickname,
            const TDesC& aWVID ) = 0;

        /**
         * Inserts contact to the defined contact list
         * If contact already exists in the contact list,
         * call is ignored and existing contact item is returned instead
         */
        virtual MCAStoredContact* CreateContactL(
            MCAContactList& aContactList,
            const TDesC& aNickname,
            const TDesC& aWVID,
            TStorageManagerGlobals::TPresenceStatus aStatus,
            TStorageManagerGlobals::TClientType aType,
            const TDesC& aAlias,
            const TDesC& aStatusText ) = 0;


        /**
         * Helper routine for handling observer events for contacts
         * @param aList Contact list where contact can be found
         */
        virtual void SendContactListUpdatedEvent( MCAContactList& aList ) = 0;

        /**
         * Removes all contacts.
         */
        virtual void RemoveAllContactsL() = 0;

        /**
         * Clears all selected-flags
         */
        virtual void ClearSelected() = 0;

        /**
         * Gets all selected contacts
         * @parram aSelected. Array of selected contacts.
         *                    Ownership is not transferred.
         *                    MUST NOT delete items from array.
         * @param aFilter Filter for contact selection @see TStorageManagerGlobals::TFilterType.
         */
        virtual void GetSelectedL( RPointerArray<MCAStoredContact>& aSelected,
                                   TStorageManagerGlobals::TFilterType aFilter ) = 0;

        /**
         * Count of stored contacts
         * @param aSkipOfflineContacts ETrue skip offline contacts.
         *                             EFalse Do not skip.
         * @return Count of contacts
         */
        virtual TInt ContactCount(
            TBool aSkipOfflineContacts = EFalse ) const = 0;

        /**
         * Enable or disable WV hiding
         * @param aHiding ETrue if hidden
         */
        virtual void SetWVHiding( TBool aHiding ) = 0;

        /**
         * Enable or disable WV hiding only Prefix( i.e "wv:")
         * @param aHiding ETrue if hidden
         */
        virtual void SetWVHidingPrefixOnly( TBool aHidingPrefixOnly ) = 0;  // UI CR ID:101-39728: Show domain part of the user ID
        /**
         *  CAUtils depends now from storage :(
         * Accessor for WV hiding flag
         * @return ETrue if hiding is enabled
         */
        virtual TBool WVHiding() = 0;
        /**
         *  CAUtils depends now from storage :(
         * Accessor for WV hiding flag
         * @return ETrue if hiding only prefix  is enabled
         */
        virtual TBool WVHidingPrefixOnly() = 0;      // UI CR ID: 101-39728: Show domain part of the user ID

        /**
         * Enable or disable Capitalization of contactlists
         * @param aCapital ETrue if capitalizing is enabled.
         */
        virtual void SetCapitalizingEnabled( TBool aCapital ) = 0; // UI CR ID: 101-39727

        /**
         *  CAUtils depends now from storage :(
         * @return ETrue if if capitalizing is enabled.
         */
        virtual TBool CapitalizingEnabled() = 0;       // UI CR ID:101-39727
        /**
         * Set watched status
         * @param aContactId. Id which watch status is changed.
         * @param aStatus. ETrue if watched. EFalse if not.
         */
        virtual void SetWatched( const TDesC& aContactId, TBool aStatus ) = 0;

        /**
         * Set watched status
         * @param aContactId. Id which watch status is changed.
         * @param aAmount Amount of pending messages.
         */
        virtual void SetPendingMessages( const TDesC& aContactId, TInt aAmount ) = 0;

        /**
         * Set blocked status
         * @param aContactId. Id which blocked status is changed.
         * @param aStatus. ETrue if blocked. EFalse if not.
         */
        virtual void SetBlocked( const TDesC& aContactId, TBool aStatus ) = 0;

        /**
         * Set block status. Only contact list aListId is checked
         * for aContactId, if id is found inside aListId its blocked
         * status is set according to aStatus.
         * @since S60 v3.2
         * @param aContactId Id which blocked status is changed.
         * @param aListId List id from which aContactId is searched.
         * @aStatus ETrue if blocked, EFalse if not.
         */
        virtual void SetBlocked( const TDesC& aContactId,
                                 const TDesC& aListId,
                                 TBool aStatus ) = 0;
        /**
        * Online status of user.
        * @param aContactId Id which status is wanted.
        * @return Onlinestatus of user.
        */
        virtual TStorageManagerGlobals::TPresenceStatus OnlineStatus(
            const TDesC& aContactId ) = 0;

        /**
         * Update presence information of contact.
         * @param aContactId Id of contact which presence is updated.
         * @param aStatus. New presence status
         * @param aAlias. New alias.
         * @param aStatusText New status message text.
         */
        virtual MCAStoredContact* UpdatePresenceL( const TDesC& aContactId,
                                                   TStorageManagerGlobals::TPresenceStatus aStatus,
                                                   TStorageManagerGlobals::TClientType aType,
                                                   const TDesC& aAlias,
                                                   const TDesC& aStatusText,
                                                   TStorageManagerGlobals::TPresenceStatus& aOldOnlineStatus ) = 0;

        /**
         * Identification of contact. This is resolved from known
         * identifications.
         * Alias, nickname and contactId.
         * Nickname is shown if set.
         * @param aContactId Id of contact which identification needed.
         * @return Identification of contact.
         *      Alias if nickname is not set and alias is enabled.
         *      Wvid without domain and prefix if nickname does not
                exists and alias is not enabled or not exists.
         */
        virtual const TPtrC Identification( const TDesC& aContactId ) = 0;

        /**
         * Sort contacts by current algorithm.
         * @aContactListId. Contact list to be sorted.
         * If KNullDesC -> All lists are sorted.
         */
        virtual void Sort( const TDesC& aContactListId = KNullDesC ) = 0;

        /**
         * Resort one contact, what ever contact list it is in
         */
        virtual void ResortContactInAll( MCAStoredContact* aContact ) = 0;

        /**
         * Set sorting algorithm. Two possible algorithms.
         * Alphabetically or by presence.
         * @param aAlgorithm. Algorithm for sorting.
         */
        virtual void SetSortAlgorithm(
            MCAStoredContact::TSortAlgorithm aAlgorithm ) = 0;

        /**
         * Own status.
         * @return Contact which contains own status.
         */
        virtual MCAStoredContact& OwnStatus() = 0;

        /**
         * Set the value to show nickname or not
         * @param aShowNickName 'Show nick name' -value
         */
        virtual void SetShowNickname( TBool aShowNickName ) = 0;

        /**
         * Lock/unlock contact list. When lock is on
         * contact list cannot be expanded or collapsed.
         * @param aLocked ETrue if locked, EFalse if unlocked.
         */
        virtual void SetContactListLock( TBool aLocked ) = 0;

        /**
         * Reset contact lists for default values. For example
         * expande/collapse status. If only one lists exists,
         * it is expanded, and if more, then all are collapsed.
         */
        virtual void ResetContactLists() = 0;

        /**
         * Set contact list property. For example choose between
         * primary and secondary list properties. ( Collapsed/expanded )
         */
        virtual void SetContactListProperty( TContactListProperty aProperty ) = 0;

        /**
         * Initialize synchronise processing. Set current item etc.
         * @return KErrNotFound if no lists found.
         */
        virtual TInt InitSynchroniseProcess() = 0;

        /**
         * Set next contact list for synch.
         * @return KErrNotFound if no lists found.
         */
        virtual TInt SetNextForSynchroniseProcess() = 0;

        /**
         * Get current list which is processed.
         * @return Reference to current list.
         * @leaves with KErrNotFound, if no list found.
         */
        virtual MCAContactList& ListInSyncProcessL() = 0;

        /**
         * Check if all lists are synchronised or not.
         * @return ETrue if all are synchronised.
         */
        virtual TBool IsAllSynchronised( TBool &aIsFailed ) const = 0;

        /**
         * Check if contact is found in storage.
         * @param aContact contact to find.
         * @return ETrue if contact is found from storage
         */
        virtual TBool IsContactValid( const MCAStoredContact* aContact ) const = 0;

        /**
         * Sorts contact lists alphabetically.
         * @since S60 v3.2
         */
        virtual void SortContactLists() = 0;
    protected:
        /**
         * For protection.
         */
        virtual ~MCAStoredContacts() {};
    };

#endif      // MCASTOREDCONTACTS_H

// End of File
