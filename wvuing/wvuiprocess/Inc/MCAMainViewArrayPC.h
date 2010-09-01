/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for CCAMainViewArrayPC, to  be used by the
*                single view array decorator to fill the listbox of the single view
*
*/


#ifndef MCAMAINVIEWARRAYPC_H
#define MCAMAINVIEWARRAYPC_H

#include <e32base.h>
#include <BADESCA.h>

#include "TEnumsPC.h"
#include "MCAMainViewObserverManagerPC.h"
#include "MCAPresenceError.h"
#include "MCAContactListModel.h"
#include "MCAPresence.h"
#include "ChatDefinitions.h"
#include "MCAListInitObserver.h"

const TInt KInitIndex = 0;
const TInt KOwnDataIndex = 0;	// default value as per UI spec
const TInt KOpenChatsIndex = 1; // default value as per UI spec


//Forward Declarations
class MCAMainViewObserverPC;
class MCAStoredContact;
class MCAInvitation;
class MCAServerContactsArrayPC;
class MCASingleListViewCB;
class MCAArrayItemPC;
// INTERFACE DECLARATION
// Helper struct for holding the aItemID, aListID, aItemType
struct TCAItemDetails
    {
    TBuf< 2 * KMaxWVIDLength > aItemID;
    TBuf< KMaxWVIDLength > aListID;
    TEnumsPC::TItem aItem;
    };
/*
* MCAMainViewArrayPC: This is main interface of arraywrapper on
* Process component side.UI Side array decorator has to use this
* array and get the required details.
*
* @lib wvuiprocessng.lib
*/
class MCAMainViewArrayPC : public MCAMainViewObserverManagerPC
    {

    public: //destructor
        virtual ~MCAMainViewArrayPC() {};


    public: // From MCAMainViewObserverManagerPC
        /**
        * AddObserverL: To add the UI component as an observer to listen to the
        * event from the MainViewArrayPC
        * @see MCAMainViewObserverManagerPC
        * @return void
        */
        virtual void AddObserverL( MCAMainViewObserverPC* aObserver ) = 0;

        /**
        * RemoveObserver: To remove the UI component as an observer to the
        * events from the MainViewArrayPC
        * @see MCAMainViewObserverManagerPC
        * @return void
        */
        virtual void RemoveObserver( MCAMainViewObserverPC* aObserver ) = 0;

        /**
        * AddObserverL: To add the UI component as an observer to listen to the
        * event from the MainViewArrayPC
        * @see MCAMainViewObserverManagerPC
        * @return void
        */
        virtual void AddObserver( MCASyncObserverPC* aObserver ) = 0;

        /**
        * RemoveObserver: To remove the UI component as an observer to the
        * events from the MainViewArrayPC
        * @see MCAMainViewObserverManagerPC
        * @return void
        */
        virtual void RemoveObserver( ) = 0;

    public:


        /**
        * SetSkipStorageEvents: Called by UI component to set flag for skipping
        * storage events
        * @param aSkip : A flag to check whether to skip strage events or not
        * @return void
        */
        virtual void SetSkipStorageEvents( TBool aSkip ) = 0;

        /**
        * GetSkipStorageEvents: Called by UI component to get the flag for skipping
        * storage events
        * @return boolean
        */
        virtual TBool GetSkipStorageEvents() = 0;

        /**
        * Count: Called by UI components to get total count of array
        * @return integer: Count of array
        */
        virtual TInt Count() const = 0; // total count of array

        /**
        * GetType: Function to get the type of list item
        * @param aIndex: index of the list item
        * @return TEnumsPC::TItem: type of the list item as EContactItem,
        * EOwnStatusItem etc
        */
        virtual TEnumsPC::TItem GetType( TInt aIndex ) const = 0;

        /**
        * IsCollapsed: To find if the list box item is collapsed
        * @param aIndex: index of the list item
        * @return bool: True if the list item is collapsed else false.
        */
        virtual TBool IsCollapsed( TInt aIndex ) const = 0;

        /**
        * IsMsgPending: To find if there are any pending messages to
        * decide the icons for display
        * @param aIndex: index of the list item
        * @return bool: True if there are pending messages else false.
        */
        virtual TBool IsMsgPending( TInt aIndex )  = 0;

        /**
        * IsBlocked: To find if the list item is a blocked contact
        * @param aIndex: index of the list item
        * @return bool: True if the list item is a blocked contact else false
        */
        virtual TBool IsBlocked( TInt aIndex ) const = 0;

        /**
        * ForceExpanded: To expand or collapse all the list items
        * @param aAllExpanded: True if all items have to be expanded else false
        * @return bool: True if there are pending messages else false.
        */
        virtual void ForceExpanded( TBool aAllExpanded ) = 0;

        /**
        * IsForceExpanded: To find if all the list items have been force expanded
        * @return bool: True if all the list items are force expanded else false
        */
        virtual TBool IsForceExpanded() const = 0;

        /**
        * GetOnlineStatus: To get the online status of the list item as Online,
        * Away, Busy etc
        * @param aIndex: Index of the list item
        * @return TEnumsPC::TOnlineStatus
        */
        virtual TEnumsPC::TOnlineStatus GetOnlineStatus( TInt aIndex ) = 0;


        /**
        * SetCollapsed: To set the list item to a collapsed or expanded state
        * @param aStatus: The collapsed or expanded state to be set for the list item
        * @param aIndex: Index of the list item whose state has to be set
        * @return void
        */
        virtual void SetCollapsed( TBool aStatus , TInt aIndex )  = 0;

        virtual void SetSelected( TBool aSelected, TInt aIndex ) = 0;

        virtual void GetSelectedL(  CDesCArray& aSelectedContact,
                                    CDesCArray* aSelectedIdentifications,
                                    TEnumsPC::TFilterType aFilter ) = 0;

        /**
        * SetMsgPending: To set the pending messages for a list item
        * @param aPendingMsgs: The no. of pending messages to be set for the list item
        * @param aIndex: Index of the list item
        * @return void
        */
        virtual void SetMsgPending( TInt aPendingMsgs, TInt aIndex )  = 0;

        /**
        * SetBlocked: To set the blocked status of a list item of type contact
        * @param aStatus: The blocked status  to be set for the list item
        * @param aIndex: Index of the list item whose status has to be set
        * @return void
        */
        virtual void SetBlocked( TBool aStatus , TInt aIndex )  = 0;

        /**
        * SetOnlineStatus: To set the online status of a list item
        * @param aStatus: The online status to be set for the list item
        * @param aIndex: Index of the list item whose status has to be set
        * @return void
        */
        virtual void SetOnlineStatus( TEnumsPC::TOnlineStatus aStatus,
                                      TInt aIndex ) = 0;

        /**
        * GetItemNameText: To get item name of each list item
        * @param aIndex: Index of the list item whose name is to be returned
        * @return TPtrC: Name of the list item
        */
        virtual TPtrC GetItemNameText( TInt aIndex ) = 0;

        /**
        * ClearArray: clear everything in the array
        * @return void
        */
        virtual void ClearArray( TBool aClearOpenChatsArray ) = 0;
        // clear everything in the array.....

        /**
        * RefreshArray: clear and fill with the new data in the array
        * @return void
        */
        virtual void ResetArray( TBool aClearOpenChatsArray ) = 0;

        /**
        * SetDefaultFilter: To set the default filters of each item
        * @param aMyFilter: TEnumsPC::TFilterType to be set
        * @return void
        */
        virtual void SetDefaultFilter( TEnumsPC::TFilterType iMyFilter ) = 0;

        /**
        * ResetDefaultFilter: To reset the default filters of each item
        * @return void
        */
        virtual void ResetDefaultFilter() = 0;

        /**
        * GetDefaultFilter: To get the default filters of list item
        * @return TEnumsPC::TFilterType: type of filter
        */
        virtual TEnumsPC::TFilterType GetDefaultFilter() const = 0;

        /**
        * ClientType: To get the client type of list item at given index
        * @return TEnumsPC::TClientType: type of client
        */
        virtual TEnumsPC::TClientType ClientType( TInt aIndex ) = 0;

        /**
        * Selected: To know if the list item at given index is selected currently
        * @param aIndex: index of list item
        * @return bool: true if the list item at given index is selected currently else false
        */
        virtual TBool Selected( TInt aIndex ) = 0;

        /**
        * IsWatched: To get the status on whether the list item at given index is
        * being watched for notifications on that contacts online status changes
        * @param aIndex: index of list item
        * @return bool: true if the contact is being watched else false
        */
        virtual TBool IsWatched( TInt aIndex ) = 0;

        /**
        * Synchronised: To get the synchronised status for the list item
        * @param aIndex: index of list item
        * @return TEnumsPC::TSyncStatus: the synchronization status of the list item
        */
        virtual TEnumsPC::TSyncStatus Synchronised( TInt aIndex ) = 0;

        /**
        * IsAllSynchronised: To get the synchronised status of all the list items
        * @return TEnumsPC::TSyncStatus: the synchronization status of all the items
        */
        virtual TEnumsPC::TSyncStatus IsAllSynchronised() = 0;

        /**
        * AddWatcher: To add watcher to presence
           */
        virtual void AddWatcherL( TPtrC contact/*wvid*/ ) = 0;

        /**
        * RemoveWatcher: To remove watcher from presence
        *
        */
        virtual void RemoveWatcherL( TPtrC contact/*wvid*/ ) = 0;

        /**
        * Refreshes friends-storage presence data.
        */
        virtual void RefreshFriendsL() = 0;

        /**
        * Places all friends (from aUserList) that are online to aOnlineList
        * and users that are offline to aOfflineList.
        * @param aUserList List of users, whose presence data is to be fetched.
        * @param aOnlineList List of users, that are online, can be NULL.
        * @param aOfflineList List of users, that are offline, can be NULL.
        * @param aUpdateStorage If ETrue, contact storage flags are updated
        */
        virtual void GetOnlineUsersL(
            const CDesCArray *aUsers,
            CDesCArray* aOnlineList,
            CDesCArray* aOfflineList,
            TBool aUpdateStorage = EFalse ) = 0;
        /**
        * Invoke the fetch contacts method from presence component
        */
        virtual void FetchContactsL() = 0;
        /**
         * Changes client own presence status, and sets status message
         * @param aStatus Presence status -online,oflline etc
         * @param aStatusMessage Presence status message
         * @return Error code.
         */
        virtual TInt ChangeStatusL( TEnumsPC::TOnlineStatus aStatus,
                                    const TDesC& aStatusMessage ) = 0;

        /**
         * Changes presence status message
         * @param aStatusMessage Presence status message
         * @return Operation error.
         */
        virtual TInt ChangeStatusMessageL( const TDesC& aStatusMessage ) = 0;

        /**
        * StatusL: To get the own status like Online, Away, Busy etc
        * @return TEnumsPC::TOnlineStatus
        */
        virtual TEnumsPC::TOnlineStatus StatusL() = 0;

        /**
        * GetOwnStatusConverter: To get TEnumsPC:TOnlineStatus type of enum from
        * the presence side MCAPresence::TPresenceStatus enums for own status
        * @param aPresenceStatus: presence side MCAPresence::TPresenceStatus enums
        * @return TEnumsPC::TOnlineStatus
        */
        virtual TEnumsPC::TOnlineStatus GetOwnStatusConverter(
            MCAPresence::TPresenceStatus  aPresenceStatus ) = 0;
    public :

        /*
        * gets the alias of the specified contact at aIndex
        * @param aIndex: index of contact
        * @return the alias
        */
        virtual const TDesC& GetSelectedContactAlias( TInt aIndex ) = 0;

        /*
        * gets the nickname of the specified contact at aIndex
        * @param aIndex: index of contact
        * @return the nickname
        */
        virtual const TDesC& GetSelectedContactNickname( TInt aIndex ) = 0;

        /*
        * gets the user id of the specified contact at aIndex
        * @param aIndex: index of contact
        * @return the user id
        */
        virtual const TDesC& GetSelectedContactUserId( TInt aIndex ) = 0;

        /*
        * gets the identification of the specified contact at aIndex
        * @param aIndex: index of contact
        * @return the identification
        */
        virtual const TDesC& GetSelectedContactIdentification( TInt aIndex ) = 0;


    public:

        /**
        * IgnoreOfflineStatusL: To decide whether to ignore the offline status of a contact
        * @param aIndex: index of the contact item in the listbox
        * @return bool: true if the offline status of contact can be ignored else return false
        */
        virtual TInt IgnoreOfflineStatusL( TInt aIndex ) = 0;
        /**
         * SetMessageReadInterfaceL: Calls the MessageReadInterface() of engine component
         * to set the read interface
         * @param aIndex: index of contact for whom the read interface (conversation) has
         * to be set
         * @return void
        */
        virtual void SetMessageReadInterfaceL( TInt aIndex ) = 0;

        /**
         * ChatExistsL: Checks if there is an existing chat for the contact
         * at index
         * @param aIndex: index of contact for which the existing
         * conversation has to be checked
         * @return bool: If chat exists for contact at aIndex return true
         * else return false
         */
        virtual TBool ChatExistsL( TInt aIndex ) = 0;

        /**
         * IndexOfList: find the index of the list if the item is a contact list else find the
         * index of parent contactlist of the contact at aIndex
         * @param aIndex: index of contactitem or list whose list index has tto be returned
         * @return TInt: If the item is a contactlist type return aIndex else it will return
         * the index of the parent contactlist of the contactitem type
         */
        virtual TInt IndexOfList( TInt aIndex,
                                  TBool aIgnoreOwnItem = EFalse,
                                  TBool aIgnoreEmptyLists = ETrue ) const = 0;

        /**
         * Check if synchronization of contact lists is done
         * @return ETrue if done
         */
        virtual TBool IsSyncDoneL() = 0;


        /**
        * To create a contact list with given name
        * @param aDisplayName: name of the list to be created
        * @return KErrNone if list is created else retrun KErrAlreadyExists
        */

        virtual TInt CreateContactListL( const TDesC& aDisplayName ,
                                         TInt& aIndex ) = 0;

        /**
        * To sort the contact list in alphbetical order
        * @param alistId: list id of renamed contact list
        */
        virtual TInt SortContactListAfterRenameL( const TDesC& aListId ) = 0 ;

        /**
        * Find if there is a list with given name already
        * @param aNamePtr: name of the list to be searched
        * @return KErrNone if theres no list with the name aNamePtr else return KErrAlreadyExists
        */

        virtual TInt ListAlreadyExistsL( const TPtr aNamePtr ) = 0;

        /**
         * Check if there are any contact list in the contact lists
         * @return KErrGeneral if no contact list is available else return KErrNone
         */
        virtual TInt ContactListsExistL() = 0;


        /**
         * Find the no. of contact items in the contact list
         * @param aIndex: the index of contact list
         * @return TInt: the no. of contact items in the list at aIndex
         */
        virtual TInt CountOfContactsInList( TInt aIndex ) = 0;

        /**
         * Count of lists.
         * @return Count of lists.
         */
        virtual TInt ListCountL() const = 0;

        /**
         * Delete the contact list
         * @param aIndex:    ID of the list
         * @return Error code
         */
        virtual TInt DeleteContactListL( TInt aIndex ) = 0;

        /**
        * Deletes one contact from server side contact list
        * @param aIndex: Index of list to be updated
        * @return Error code
        */
        virtual TInt DeleteServerContactL( TInt aIndex ) = 0;

        /*
        * gets the list id of the specified list at aIndex
        * @param aIndex: index of list
        * @return the list id
        */
        virtual const TDesC& GetSelectedListId( TInt aIndex ) = 0;

        /**
        * Find a contact from the store by wv User ID
        * @param aContactListId List which contains contact.
        * @param aId the User ID of the contact to find
        * @return The contact
        */
        virtual TInt FindContactL( const TDesC& aContactListId,


                                   const TDesC& aId ) = 0;



        /**
        * Find a contact list from the store by wv User ID
        * @param aContactListId List which contains contact list id.
        * @return The contact list index
        */
        virtual TInt FindContactListIndexL( const TDesC& aListId ) = 0;


        /**
        * Get the status text for the contact
        * @param aIndex: Index of contact item
        * @return Error code
        */
        virtual const TDesC& StatusText( TInt aIndex ) const = 0;

        /**
         * Construct an array of contact list positions in model.
         * aPositionArray is cleared before appending positions!
         * @param aPositionArray array in which to store positions
         * @return none
         */
        virtual void MapContactListPositions( RArray<TInt>& aPositionArray ) = 0;
        /*
        * gets the displayname of the specified List at aIndex
        * @param aIndex: index of list
        * @return the displayname
        */

        virtual TPtrC DisplayName( TInt aIndex ) = 0;

        /**
        * SetDisplayName: To set/rename item name of each list item
        * @param aIndex: Index of the list item whose name is to be changed/set
        * @param aItemName: name to be set for the list item at specified index
        * @return TInt: Error code to check if the name has been set
        */
        virtual TInt SetDisplayNameL( TInt aIndex, const TPtrC aItemName ) = 0;


        /**
         *Sets the selectionmode
         */
        virtual void SetContactSelectionL(
            TEnumsPC::TListSelectionType aSelectionMode,
            TInt aSelectedListIndex ) = 0;
        /**
         *Sets the selectionmode
         */
        virtual void ResetContactSelectionL() = 0;

        /**
         * Should the model show "All" item in beginning
         * of each contact list.
         * @param aShow If ETrue, first item in every contact list is "All"
         */
        virtual void ShowAll( TBool aShow ) = 0;

        /**
         * GetFilteredCount
         */
        virtual TInt GetFilteredCount(
            TEnumsPC::TFilterType aFilter, TInt aIndex ) = 0;

        /**
         * Clears all selected-flags
         */
        virtual void ClearSelectedL( ) = 0;

        //for invitations
        /**
         * Returns index for given invitation
         * @param aIndex
         * @return index of invitation
         */
        virtual TInt GetItemEngineIndex( TInt aIndex  ) const = 0;

        /**
        * ToUpdatePresence: To get the information on whether presence has to be updated
        * by anding the two filters
        * @parm aFilter1: TEnumsPC::TFilterType
        * @parm aFilter2: TEnumsPC::TFilterType
        */
        virtual TBool ToUpdatePresence( TEnumsPC::TFilterType aFilter1,
                                        TEnumsPC::TFilterType aFilter2 ) = 0;

        /**
         * Returns index of invitation for given wvid and
         * Deletes item from array.
         * @since 3.2
         * @param Index of invitation
         */
        virtual TInt FindInvitationAndDelete(
            const TDesC& aInviteID /*TInt aIndex*/ ) = 0;

        /**
         * Returns index of invitation for given wvid and
         * Deletes item from array.
         * @since 3.2
         * @param Index of invitation
         */
        virtual TInt FindInvitationAndDelete( TInt aIndex ) = 0;

        /**
        * Find any contact which id is aContactId
        * @param aContactId Contactid to recognize contact.
        * @return ETrue if contact found else EFalse
        */
        virtual TBool FindAnyContactL( const TDesC& aContactId ) = 0;


        /**
        * Set a separate filter for the single list view array items
        * @param aFilter: filter to be set
        */
        virtual void SetSingleListArrayFilter(
            TEnumsPC::TFilterType aFilter ) = 0;
        /**
        * Get a separate filter for the single list view array items
        * @return filter
        */
        virtual TEnumsPC::TFilterType GetSingleListArrayFilter() = 0;

        /**
        * To set the filter type for engine
        */
        virtual void SetFilter( TEnumsPC::TFilterType aFilter ) = 0;

        /**
        * To get the filter type for engine
        */
        virtual TEnumsPC::TFilterType Filter() = 0;


        /**
        * Fills aOnlineList with the user id's from friends-list that are
        * online.
        * @param aOnlineList List that will be populated with online friends.
        * @param aFetchFromNetwork Fetch presence information from network
        */
        virtual void GetOnlineContactsL( CDesCArray& aOnlineList,
                                         TBool aFetchFromNetwork = EFalse ) = 0;

        /**
         * Gets the details of the given index
         * like aItemId, aListId,aItemType.
         * @return teh structure filled with tha above details.
         */
        virtual TCAItemDetails GetArrayItemIdDetails( TInt aIndex ) = 0;

        /**
         * Gets the Index, given item details
         * like aItemId, aListId,aItemType.
         * @return the index
         */
        virtual TInt GetArrayItemIdIndexL( const TCAItemDetails& aItemDetails ) = 0;

        /**
         * Registers the singlelistview for callback.
         */
        virtual void RegisterCallBack( MCASingleListViewCB* aSingleViewCB ) = 0;

        /**
         * UnRegisters the singlelistview for callback.
         */
        virtual void UnRegisterCallBack( ) = 0;

        /**
         * @return returns ETrue if any contact is found in contactlists
         * else returns EFalse if no contact is found
         */
        virtual TBool IsAnyContactFound() = 0;

        /**
         * Populate the group array iGroupArray with groups
         */
        virtual void  PopulateGroupsListL() = 0;

        /**
        * To set the group count down flag, for each logout. For the first time login
        * it is set in the constructor to EFalse. This flag ensures that the
        * initial deletion of groups from iGroupArray and the subsequent additions
        * of user created groups are not reflected on the UI
        */
        virtual void ResetGroupCountDownOnLogout() = 0;


        virtual const TDesC& GetInviteItemID( TInt aIndex ) = 0;

        virtual const RPointerArray<MCAArrayItemPC>& GetOpenChatsArray() const = 0;

        /**
        * @returns the index of the given item W.R.T singlelistview.
        */
        virtual TInt FindIndexOfOpenChatsItem( TEnumsPC::TItem aItemType,
                                               const TDesC& aId ) = 0;

        virtual TInt GetOpenChatsActiveIndex()const = 0;

        /**
         * Sets the focus to the owndata on logout.
         */
        virtual void SetCurrentItemIndexL( TBool aFreshLogin = EFalse ) = 0;


    public: //Added methods for sorting of openchats

        virtual void InsertConversationItemL( const TDesC& aUserId,
                                              const TDesC& aIdentification ) = 0;
        virtual void RemoveConversationItem( const TDesC& aUserId ) = 0;

        virtual TBool IsOpenChatsMsgPending(
            const TEnumsPC::TItem aType,
            const TDesC& aId ) = 0;

        /**
         * First pending message containers WVID.
         * @return First pending message WVID.
         */
        virtual const TDesC& PendingMessageInfo() = 0;

        /**
         * First pending message containers groupID.
         * @param First pending message groupName.
         * @return First pending message groupID.
         */
        virtual const TDesC& ChatGroupPendingMessageInfo( TDesC& aGroupName ) = 0;


        /**
        * RegisterObserverL: To add the UI component as an observer to listen to the
        * event from the MainViewArrayPC
        * @see MCAListInitObserver
        * @return void
        */
        virtual void RegisterObserverL( MCAListInitObserver* aObserver ) = 0;

        /**
        * UnRegisterObserverL:
        * @see MCAListInitObserver
        * @return void
        */
        virtual void UnRegisterObserverL() = 0;

        /**
        * AIsListInitializationCompleteL:
        * @return Bool value
        */
        virtual TBool IsListInitializationCompletedL()  = 0;


        /**
         * Refresh the conversation item in openchats to
         * reflect the changes upon addation/chang/deletion of a contact
         * @param aContactId: userId whose nickname is changed.
         */
        virtual void RefreshConversationItemText( const TDesC& aContactId ) = 0;
        /**
         * Refresh the invitation item in openchats to
         * reflect the changes upon addation/chang/deletion of a contact
         * @param aContactId: userId whose nickname is changed.
         */
        virtual void RefreshInvitationItemTextL( const TDesC& aContactId ) = 0;
        /*
        *Get the index of top pending conversation in single list view.
        *This index can be used high light that perticular item.
        */
        virtual TInt GetIndexOfTopPendingMessage() = 0;

    };

#endif      // MCAMAINVIEWARRAYPC_H

