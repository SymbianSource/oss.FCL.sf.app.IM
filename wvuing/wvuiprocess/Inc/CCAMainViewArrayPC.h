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
* Description:  Implementation class for MCAMainViewArrayPC, to decide how to fill the
*				 single view array decorator
*
*/



#ifndef CCAMAINVIEWARRAYPC_H
#define CCAMAINVIEWARRAYPC_H

#include <e32base.h>


//Forward Declaration
class MCAContactListModel;
class MCAArrayItemPC;
class CCAEngine;
class MCAInvite;
class MCAInvitation;
class MCAServerContactsArrayPC;
class MCAStoredGroup;
class CCAGroupListArrayPC;
class MCASingleListViewCB;
class MCAStoredGroups;

#include "MCAMainViewObserverManagerPC.h"
#include "MCAMainViewObserverPC.h"
#include "MCAStoredContactsObserver.h"
#include "MCAMainViewArrayPC.h"
#include "MCAWatcherObserver.h"
#include "MCAStoredContacts.h"
#include "MCAChatInterface.h"
#include "ChatDefinitions.h"
#include "MCASettingsObserver.h"
#include "MCAInviteObserver.h"
#include "MCASettingsPC.h"
#include "MCAChatObserver.h"
#include "MWVSettingsObserverng.h"
#include "MCAStoredGroupsObserver.h"
#include "MCASyncObserverPC.h"

#include <MIMPSSAPObserver.h>
#include <CIMPSSAPSettingsStore.h>
#include <cimpspresenceconnectionuing.h>

#include "MCAListInitObserver.h"

class CCAMainViewArrayPC:   public CBase,
            public MCAMainViewArrayPC,  // this class own interface.
            public MCASettingsObserver,	// events from engine.
            public MCAStoredGroupsObserver, // events from engine.
            public MCAMainViewObserverManagerPC,
            public MCAWatcherObserver,
            public MCAInviteObserver,
            public MCAChatObserver,

            // This callback is required when ever the server is added or deleted
            // we get a callback and update the singlelist
            public MIMPSSAPObserver,
            public MCAListInitObserver,
            public MCAStoredContactsObserver

    {
    public:
        struct TArrayPairs // Struct to hold identification and buffer
            {
            TBuf<KMaxWVIDLength> iIdentification;
            MCAMessagesReadInterface* iReadInterface;
            };

        struct TInvitationArray // Struct to hold identification and buffer
            {
            TDesC* iUserID;
            TDesC* iMessage;
            };
    public: //Constructor and Destructors
        /**
        * construction
        */
        void ConstructL();
        /**
        * construction
        * @param aContactListModel : a reference to the contact list model object at the chat engine
        */
        static CCAMainViewArrayPC* NewL( CCAEngine& aEngine,
                                         MCAContactListModel& aContactListModel,
                                         MCAChatInterface& aChatInterface );

    private:
        /**
        * default constructor
        * @param aContactListModel : a reference to the contact
        * list model object at the chat engine
        */
        CCAMainViewArrayPC( CCAEngine& aEngine,
                            MCAContactListModel& aContactListModel,
                            MCAChatInterface& aChatInterface );

    public:
        /**
         * virtual destructor
         */
        virtual ~CCAMainViewArrayPC();


    public: // From MCAMainViewArrayPC

        /**
         * Gets the details of the given index
         * like aItemId, aListId,aItemType.
         * @return the structure filled with tha above details.
         */
        TCAItemDetails GetArrayItemIdDetails( TInt aIndex );

        /**
         * Gets the Index, given item details
         * like aItemId, aListId,aItemType.
         * @return the index
         */
        TInt GetArrayItemIdIndexL( const TCAItemDetails& aItemDetails );
        /**
         * @return returns ETrue if any contact is found in contactlists
         * else returns EFalse if no contact is found
         */
        TBool IsAnyContactFound();

        /**
         * Populate the group array iGroupArray with groups
         */
        void  PopulateGroupsListL();

        /**
        * SetSkipStorageEvents: Called by UI component to set flag for skipping storage events
        * @param aSkip : A flag to check whether to skip strage events or not
        * @return void
        */
        virtual void SetSkipStorageEvents( TBool aSkip );

        /**
        * GetSkipStorageEvents: Called by UI component to get the flag for skipping storage events
        * @return boolean
        */
        virtual TBool GetSkipStorageEvents();

        /**
        * Count: Called by UI components to get total count of array
        * @return integer: Count of array
        */
        virtual TInt Count() const;

        /**
        * GetType: Function to get the type of list item
        * @param aIndex: index of the list item
        * @return TEnumsPC::TItem: type of the list item as EContactItem, EOwnStatusItem etc
        */
        virtual TEnumsPC::TItem GetType( TInt aIndex ) const;

        /**
        * IsCollapsed: To find if the list box item is collapsed
        * @param aIndex: index of the list item
        * @return bool: True if the list item is collapsed else false.
        */
        virtual TBool IsCollapsed( TInt aIndex ) const;

        /**
        * IsMsgPending: To find if there are any pending messages to
        *				decide the icons for display
        * @param aIndex: index of the list item
        * @return bool: True if there are pending messages else false.
        */
        virtual TBool IsMsgPending( TInt aIndex ) ;

        /**
        * IsBlocked: To find if the list item is a blocked contact
        * @param aIndex: index of the list item
        * @return bool: True if the list item is a blocked contact else false
        */
        virtual TBool IsBlocked( TInt aIndex ) const;

        /**
        * ForceExpanded: To expand or collapse all the list items
        * @param aAllExpanded: True if all items have to be expanded else false
        * @return bool: True if there are pending messages else false.
        */
        virtual void ForceExpanded( TBool aAllExpanded );

        /**
        * IsForceExpanded: To find if all the list items have been force expanded
        * @return bool: True if all the list items are force expanded else false
        */
        virtual TBool IsForceExpanded() const;

        /**
        * GetOnlineStatus: To get the online status of the list item as Online, Away, Busy etc
        * @param aIndex: Index of the list item
        * @return TEnumsPC::TOnlineStatus
        */
        virtual TEnumsPC::TOnlineStatus GetOnlineStatus( TInt aIndex );

        /**
        * SetCollapsed: To set the list item to a collapsed or expanded state
        * @param aStatus: The collapsed or expanded state to be set for the list item
        * @param aIndex: Index of the list item whose state has to be set
        * @return void
        */
        virtual void SetCollapsed( TBool aStatus, TInt aIndex );

        /**
        * SetSelected:
        * @param aSelected: selected or not
        * @return aIndex ,given index
        */
        virtual void SetSelected( TBool aSelected, TInt aIndex );

        /**
        * GetSelectedL: get the selcetd
        * @param aSelectedContact: selected conatct
        * @param aSelectedIdentifications
        * @return aFilter
        */
        virtual void GetSelectedL( CDesCArray& aSelectedContact,
                                   CDesCArray* aSelectedIdentifications,
                                   TEnumsPC::TFilterType aFilter );

        /**
         * Sets the selectionmode
         * @param aSelectionMode to be set
         * @param aSelectedListIndex - selected contactlist index - used for
         * group creation using existing contact list to get all the contacts in that
         * contact list
         */

        void SetContactSelectionL( TEnumsPC::TListSelectionType aSelectionMode,
                                   TInt aSelectedListIndex );

        /**
         * Resets the selectionmode
         */
        void ResetContactSelectionL();

        /**
         * Returns index for given invitation
         * @param aIndex
         * @return index of invitation
         */
        TInt GetItemEngineIndex( TInt aIndex ) const;

        /**
         * Sets the focus to the owndata on logout.
         */
        virtual void SetCurrentItemIndexL( TBool aFreshLogin = EFalse  );

        /**
        * Find any contact which id is aContactId
        * @param aContactId Contactid to recognize contact.
        * @return ETrue if contact found else EFalse
        */
        TBool FindAnyContactL( const TDesC& aContactId );


        /**
         * Appends the received invitation to the top of the array
         * @param aInvitation
         */
        void AddInvitation( const MCAInvitation* aInvitation );

        /**
         * Returns index of invitation for given wvid and
         * Deletes item from array.
         * @since 3.2
         * @param Index of invitation
         * @return index at which the invite has been deleted.
         */
        virtual TInt FindInvitationAndDelete( const TDesC& aInviteID /*TInt aIndex*/ );


        virtual TInt FindInvitationAndDelete( TInt aIndex );



        /**
         * SetMsgPending: To set the pending messages for a list item
         * @param aPendingMsgs: The no. of pending messages to be set for the list item
         * @param aIndex: Index of the list item
         * @return void
         */
        virtual void SetMsgPending( TInt aPendingMsgs, TInt aIndex );

        /**
         * SetBlocked: To set the blocked status of a list item of type contact
         * @param aStatus: The blocked status  to be set for the list item
         * @param aIndex: Index of the list item whose status has to be set
         * @return void
         */
        virtual void SetBlocked( TBool aStatus, TInt aIndex );

        /**
         * SetOnlineStatus: To set the online status of a list item
         * @param aStatus: The online status to be set for the list item
         * @param aIndex: Index of the list item whose status has to be set
         * @return void
         */
        virtual void SetOnlineStatus( TEnumsPC::TOnlineStatus aStatus, TInt aIndex );

        /**
         * GetItemNameText: To get item name of each list item
         * @param aIndex: Index of the list item whose name is to be returned
         * @return TPtrC: Name of the list item
         */
        virtual TPtrC GetItemNameText( TInt aIndex );

        /**
         * GetFormattedItemNameTextL: To set/rename item name of each list item
         * @param aIndex: Index of the list item whose name is to be changed/set
         * @return TPtr: Error code to check if the name has been set
         */
        virtual TPtr GetFormattedItemNameTextL( TInt aIndex );

        /**
         * ClearArray: clear everything in the array
         * @return void
         */
        virtual void ClearArray( TBool aClearOpenChatsArray );


        /** ResetArray: reset the array,clear and fill with the new data in the array
         * @return void
         */
        virtual void ResetArray( TBool aClearOpenChatsArray );

        /**
         * SetDefaultFilter: To set the default filters of each item
         * @param aMyFilter: TEnumsPC::TFilterType to be set
         * @return void
         */
        virtual void SetDefaultFilter( TEnumsPC::TFilterType aMyFilter );

        /**
        * ResetDefaultFilter: To reset the default filters of each item
        * @return void
        */
        virtual void ResetDefaultFilter();

        /**
         * GetDefaultFilter: To get the default filters of list item
         * @return TEnumsPC::TFilterType: type of filter
         */
        virtual TEnumsPC::TFilterType GetDefaultFilter() const;

        /**
         * Should the model show "All" item in beginning
         * of each contact list.
         * @param aShow If ETrue, first item in every contact list is "All"
         */
        void ShowAll( TBool aShow );


        /**
        * ClientType: To get the client type of list item at given index
        * @return TEnumsPC::TClientType: type of client
        */
        virtual TEnumsPC::TClientType ClientType( TInt aIndex );

        /**
        * Selected: To know if the list item at given index is selected currently
        * @param aIndex: index of list item
        * @return bool: true if the list item at given index is selected currently else false
        */
        virtual TBool Selected( TInt aIndex );

        /**
        * IsWatched: To get the status on whether the list item at given index is
        * being watched for notifications on that contacts online status changes
        * @param aIndex: index of list item
        * @return bool: true if the contact is being watched else false
        */
        virtual TBool IsWatched( TInt aIndex );

        /**
        * AddWatcher: To add watcher to presence
           */
        virtual void AddWatcherL( TPtrC contact/*wvid*/ );


        /**
        * RemoveWatcher: To remove watcher from presence
        *
        */
        virtual void RemoveWatcherL( TPtrC contact/*wvid*/ );

        /**
        * Refreshes friends-storage presence data.
        */
        virtual void RefreshFriendsL();

        /**
         * Clears all selected-flags
         */
        virtual void ClearSelectedL( );

        /**
        * Places all friends to aUsers array
        */

        /* Places all friends (from aUserList) that are online to aOnlineList
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
            TBool aUpdateStorage = EFalse ) ;

        /**
        * Synchronised: To get the synchronised status for the list item
        * @param aIndex: index of list item
        * @return TEnumsPC::TSyncStatus: the synchronization status of the list item
        */
        virtual TEnumsPC::TSyncStatus Synchronised( TInt aIndex );

        /**
        * IsAllSynchronised: To get the synchronised status of all the list items
        * @return TEnumsPC::TSyncStatus: the synchronization status of all the items
        */
        virtual TEnumsPC::TSyncStatus IsAllSynchronised();

        /**
         * Changes client own presence status, and sets status message
         * @param aStatus Presence status -online,oflline etc
         * @param aStatusMessage Presence status message
         * @return Error code.
         */
        TInt ChangeStatusL( TEnumsPC::TOnlineStatus aStatus,
                            const TDesC& aStatusMessage ) ;

        /**
         * Changes presence status message
         * @param aStatusMessage Presence status message
         * @return Operation error.
         */
        TInt ChangeStatusMessageL( const TDesC& aStatusMessage ) ;

        /**
        * StatusL: To get the own status like Online, Away, Busy etc
        * @return TEnumsPC::TOnlineStatus
        */
        TEnumsPC::TOnlineStatus StatusL();

        /**
        * GetOwnStatusConverter: To get TEnumsPC:TOnlineStatus type of enum from
        * the presence side MCAPresence::TPresenceStatus enums for own status
        * @param aPresenceStatus: presence side MCAPresence::TPresenceStatus enums
        * @return TEnumsPC::TOnlineStatus
        */
        TEnumsPC::TOnlineStatus GetOwnStatusConverter(
            MCAPresence::TPresenceStatus  aPresenceStatus );
        /**
         * Fills aOnlineList with the user id's from friends-list that are
         * online.
         * @param aOnlineList List that will be populated with online friends.
         * @param aFetchFromNetwork Fetch presence information from network
         */
        void GetOnlineContactsL( CDesCArray& aOnlineList,
                                 TBool aFetchFromNetwork = EFalse );



        /**
         * RegisterObserverL: To add the UI component as an observer to listen to the
         * event from the MainViewArrayPC
         * @see MCAListInitObserver
         * @return void
         */
        void RegisterObserverL( MCAListInitObserver* aObserver ) ;

        /**
        * UnRegisterObserverL: To remove form the listener
        * @see MCAListInitObserver
        * @return void
        */
        void UnRegisterObserverL() ;

        /**
        * AIsListInitializationCompleteL:
        * @return Bool value
        */
        TBool IsListInitializationCompletedL() ;



        /**
         * Refresh the conversation item in openchats to
         * reflect the changes upon addation/chang/deletion of a contact
         * @param aContactId: userId whose nickname is changed.
         */
        void RefreshConversationItemText( const TDesC& aContactId );

        /**
         * Refresh the Invitation item in openchats to
         * reflect the changes upon addation/chang/deletion of a contact
         * @param aContactId: userId whose nickname is changed.
         */
        void RefreshInvitationItemTextL( const TDesC& aContactId );

    public: // From MCAMainViewObserverManagerPC

        /**
        * AddObserverL: To add the UI component as an observer to listen to the
        *               event from the MainViewArrayPC
        * @see MCAMainViewObserverManagerPC
        * @return void
        */
        void AddObserverL( MCAMainViewObserverPC* aObserver );

        /**
        * RemoveObserver: To remove the UI component as an observer to the
        *               events from the MainViewArrayPC
        * @see MCAMainViewObserverManagerPC
        * @return void
        */
        void RemoveObserver( MCAMainViewObserverPC* aObserver );

        /**
         * AddObserverL: To add the UI component as an observer to listen to the
         *               event from the MainViewArrayPC
         * @see MCAMainViewObserverManagerPC
         * @return void
         */
        void AddObserver( MCASyncObserverPC* aObserver );

        /**
         * RemoveObserver: To remove the UI component as an observer to the
         *               events from the MainViewArrayPC
         * @see MCAMainViewObserverManagerPC
         * @return void
         */
        void RemoveObserver( );

    public: //MCAChatObserver
        /**
        * Handle events.
        * @param aEvent Event to handle.
        * @param aMessage, Event occurred because of message
        */
        virtual void HandleChatEvent( TChatEventType aEvent,
                                      MCAMessage* aMessage = NULL );

    public: //MCAsyncObserverPC

        virtual void NotifySyncCompleted();
    public: //From MCAStoredContactsObserver

        /**
        * HandleContactDelete: To handle the contact delete event from the MCAStoredContactsObserver
        * @see MCAStoredContactsObserver
        * @param aContactId: contact whihc has been deleted
        * @return void
        */
        virtual void HandleContactDelete( const TDesC& aContactId );

        /**
        * HandleAddition: To handle the contact addition event from the MCAStoredContactsObserver
        * @see MCAStoredContactsObserver
        * @param aList: reference to the list to which the contact has been added
        * @param aContact:  reference to the contact which has been added
        * @return void
        */
        virtual void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact );

        /**
        * HandleChange: To handle the Chnage event from the MCAStoredContactsObserver
        * @see MCAStoredContactsObserver
        * @param aList: reference to the list at which a change has occured
        * @param aContact:  reference to the contact at which a change has occured
        * @param aEventType: TStorageManagerGlobals::TCAObserverEventType :
        * type of change event that has occured
        * @return void
        */
        virtual void HandleChange( MCAContactList* aList,
                                   MCAStoredContact* aContact,
                                   TStorageManagerGlobals::TCAObserverEventType aEventType,
                                   TBool aUserIdChanged );


    public: //From MCASettingsObserver

        /**
        * HandleSettingsChangeL: To handle the change events in settings
        * @see MCASettingsObserver
        * @param aChangedSettingEnum: enumeration of the type of settings change
        * @return void
        */
        virtual void HandleSettingsChangeL( TInt aChangedSettingEnum );

    public: //From MCAWatcherObserver

        /**
        * HandleWatcherEvent: To handle the watch events for a contact
        * @see MCAWatcherObserver
        * @param aContact:  reference to the contact at which a watch event has occured
        */
        void HandleWatcherEvent( const MCAStoredContact* aContact );

    public: // From MCAInviteObserver

        /**
         * @see MCAInviteObserver
         */
        void HandleInvitationEventL( TInviteEventType aEvent,
                                     const MCAInvitation* aInvitation );

        /**
         * @see MCAInviteObserver
         */
        void HandleInvitationResponse( TBool aAcceptance,
                                       const TDesC& aUserId,
                                       const TDesC& aGroupName,
                                       const TDesC& aResponse );
    public://for CallBacks.

        void RegisterCallBack( MCASingleListViewCB* aSingleViewCB );

        void UnRegisterCallBack( );
    public:

        /*
        * gets the alias of the specified contact at aIndex
        * @param aIndex: index of contact
        * @return the alias
        */
        virtual const TDesC& GetSelectedContactAlias( TInt aIndex );

        /*
        * gets the nickname of the specified contact at aIndex
        * @param aIndex: index of contact
        * @return the nickname
        */
        virtual const TDesC& GetSelectedContactNickname( TInt aIndex );

        /*
        * gets the user id of the specified contact at aIndex
        * @param aIndex: index of contact
        * @return the user id
        */
        virtual const TDesC& GetSelectedContactUserId( TInt aIndex );

        /*
        * gets the identification of the specified contact at aIndex
        * @param aIndex: index of contact
        * @return the identification
        */
        virtual const TDesC& GetSelectedContactIdentification( TInt aIndex );


        /*
        * Gets the MCAStoredContact type for the specified contact at aIndex
        * This method is for use by the CCAContactEditor class and is not exposed
        * via the MCAMainViewArrayPC interface
        * @param aIndex: index of contact
        * @return the alias
        */
        MCAStoredContact* GetContactForContactEditor( TInt aIndex );

        // See the comment @MCAMainViewArrayPC

        virtual void MapContactListPositions( RArray<TInt>& aPositionArray );

        /*
        * gets the displayname of the specified List at aIndex
        * @param aIndex: index of list
        * @return the displayname
        */

        virtual TPtrC DisplayName( TInt aIndex );

        /**
        * SetDisplayName: To set/rename item name of each list item
        * @param aIndex: Index of the list item whose name is to be changed/set
        * @param aItemName: name to be set for the list item at specified index
        * @return TInt: Error code to check if the name has been set
        */
        virtual TInt SetDisplayNameL( TInt aIndex, const TPtrC aItemName );

        /*
         * gets the FilteredCount
         * @param aFilter: type of filetr
         * @param aIndex: index of list
         * @return the count
         */
        virtual TInt GetFilteredCount( TEnumsPC::TFilterType aFilter, TInt aIndex );
        /**
        * Set a separate filter for the single list view array items
        * @param aFilter: filter to be set
        */
        virtual void SetSingleListArrayFilter( TEnumsPC::TFilterType aFilter );

        /**
        * Get a separate filter for the single list view array items
        * @return filter
        */
        virtual TEnumsPC::TFilterType GetSingleListArrayFilter( );


    public:
        /**
        * Find a contact from the store by wv User ID
        * @param aContactListId List which contains contact.
        * @param aId the User ID of the contact to find
        * @return The contact
        */
        virtual TInt FindContactL( const TDesC& aContactListId,
                                   const TDesC& aId );

        /**
        * Find a contact list from the store by wv User ID
        * @param aContactListId List which contains contact list id.
        * @return The contact list index
        */
        virtual TInt FindContactListIndexL( const TDesC& aListId );
        /* gets the list id of the specified list at aIndex
        * @param aIndex: index of list
        * @return the list id
        */
        virtual const TDesC& GetSelectedListId( TInt aIndex );

        /**
        * Checks status of aContact and shows needed notes accordign to variation.
        * @param aIndex: index of contact to be checked.
        * @return Integer value, negative if offline status is not ignored or
        *              allowed by variation, positive if contact is online
        *              or user decides to ignore offline status. See possible
        *              return values from IMUtils::TIgnoreOfflineStatus enum.
        */
        virtual TInt IgnoreOfflineStatusL( TInt aIndex );

        /**
        * SetMessageReadInterfaceL: Calls the MessageReadInterface() of engine component
        * to set the read interface
        * @param aIndex: index of contact for whom the read interface (conversation) has
        * to be set
        * @return void
        */
        virtual void SetMessageReadInterfaceL( TInt aIndex );

        /**
        * ChatExistsL: Checks if there is an existing chat for the contact
        * at index
        * @param aIndex: index of contact for which the existing
        * conversation has to be checked
        * @return bool: If chat exists for contact at aIndex return true
        * else return false
        */
        virtual TBool ChatExistsL( TInt aIndex );

        /**
         * Get index for list
         * @param aIndex: index of Contact item to be found.
         * @param aIgnoreOwnItem. If ETrue own item is not taken into account
         *                        when resolving index.
         * @param aIgnoreEmptyLists If ETrue, empty lists are ignored.
         * @return Index of list. If list not found returns KErrNotFound
         */
        virtual TInt IndexOfList( TInt aIndex,
                                  TBool aIgnoreOwnItem = EFalse,
                                  TBool aIgnoreEmptyLists = ETrue ) const;


        /**
         * Check if synchronization of contact lists is done
         * @return ETrue if done
         */
        virtual TBool IsSyncDoneL();


        /**
        * To create a contact list with given name
        * @param aDisplayName: name of the list to be created
        * @return KErrNone if list is created else retrun KErrAlreadyExists
        */

        virtual TInt CreateContactListL( const TDesC& aDisplayName , TInt& aIndex ) ;

        /**
        * To sort the contact list in alphbetical order
        * @param alistId: list id of renamed contact list
        */
        virtual TInt SortContactListAfterRenameL( const TDesC& alistId ) ;

        /**
        * Find if there is a list with given name already
        * @param aNamePtr: name of the list to be searched
        * @return KErrNone if theres no list with the name aNamePtr else return KErrAlreadyExists
        */

        virtual TInt ListAlreadyExistsL( const TPtr aNamePtr );

        /**
        * Check if there are any contact list in the contact lists
        * @return KErrGeneral if no contact list is available else return KErrNone
        */
        virtual TInt ContactListsExistL();


        /**
         * Find the no. of contact items in the contact list
         * @param aIndex: the index of contact list
         * @return TInt: the no. of contact items in the list at aIndex
         */
        virtual TInt CountOfContactsInList( TInt aIndex );

        /**
         * Count of lists.
         * @return Count of lists.
         */
        virtual TInt ListCountL() const;

        /**
         * Delete the contact list
         * @param aIndex:    ID of the list
         * @return Error code
         */
        virtual TInt DeleteContactListL( TInt aIndex );
        /**
        * Deletes one contact from server side contact list
        * @param aIndex: Index of list to be updated
        * @return Error code
        */
        virtual TInt DeleteServerContactL( TInt aIndex );

        /**
        * Get the status text for the contact
        * @param aIndex: Index of contact item
        * @return Error code
        */
        virtual const TDesC& StatusText( TInt aIndex ) const;

        /**
        * Invoke the fetch contacts method from presence component
        */
        virtual void FetchContactsL();
        /**
         * to convert engine enum value to PC enum values
         * @param aStatus: value of engine value
         */
        TEnumsPC::TCASettingValues ConvertToEnumsPC( TInt aStatus );


    public:

        const RPointerArray<MCAArrayItemPC>& GetOpenChatsArray() const ;

        /**
        * @returns the index of the given item W.R.T singlelistview.
        */
        TInt FindIndexOfOpenChatsItem( TEnumsPC::TItem aItemType,
                                       const TDesC& aId );

        TInt GetOpenChatsActiveIndex()const;

    private:
        /**
        * FillArray: fill up the complete array
        * @return void
        */
        void FillArrayL( TBool aFillOpenChatsArray );


        /**
        * FillOwnDataL: fill up fill the owndata item
        * @return void
        */
        TBool  FillOwnDataL();

        /**
        * FillOpenChatsL: then fill the conversations..or open chats....
        * @return void
        */
        //
        void FillOpenChatsL();

        /**
        * FillContactListsL: then fill the contacts lists/contacts..etc..
        * @return void
        */
        void FillContactListsL();

        void FillSelectedContactListL( );



        /**
        * FillItemL: fill up the individual item in the array
        * @param aIndex: index of the list item for which the array has to be filled
        * @return void
        */
        void FillItemL( TInt aIndex );

        /**
        * GetResourceValueL: To get the integer equivalent of resource string
        * @param aResourceID: id of the resource string
        * @return TInt: integer equivalent of resource string
        */
        TInt GetResourceValueL( TInt aResourceID );

        /**
        * GetOnlineStatusConverter: To get TEnumsPC:TOnlineStatus type of enum from
        * the engine /storage side TStorageManagerGlobals::TPresenceStatus enums
        * @param aEngOnlineStatus: engine /storage side TStorageManagerGlobals::TPresenceStatus enums
        * @return TInt: TEnumsPC::TOnlineStatus
        */
        TEnumsPC::TOnlineStatus GetOnlineStatusConverter(
            TStorageManagerGlobals::TPresenceStatus aEngOnlineStatus );


        /**
        * GetTypeConverter: To get TEnumsPC:TItem type of enum from
        * the engine side MCAContactListModel::TItemType enums
        * @param aItemType: engine side MCAContactListModel::TItemType enums
        * @return TInt: EnumsPC::TItem
        */
        TEnumsPC::TItem GetTypeConverter( MCAContactListModel::TItemType aItemType ) const;

        /**
        * ToUpdatePresence: To get the information on whether presence has to be updated
        * by anding the two filters
        * @parm aFilter1: TEnumsPC::TFilterType
        * @parm aFilter2: TEnumsPC::TFilterType
        */
        virtual TBool ToUpdatePresence( TEnumsPC::TFilterType aFilter1,
                                        TEnumsPC::TFilterType aFilter2 );

        /**
        * To set the filter type for engine
        */
        virtual void SetFilter( TEnumsPC::TFilterType aFilter );

        /**
        * To get the filter type for engine
        */
        virtual TEnumsPC::TFilterType Filter();

        /**
        * To set the group count down flag, for each logout. For the first time login
        * it is set in the constructor to EFalse. This flag ensures that the
        * initial deletion of groups from iGroupArray and the subsequent additions
        * of user created groups are not reflected on the UI
        */
        virtual void ResetGroupCountDownOnLogout();

        virtual const TDesC& GetInviteItemID( TInt aIndex );




        // This callback is required when ever the server is added or deleted
        // we get a callback and update the singlelist
        /**
         * From MIMPSSAPObserver
         * @see MIMPSSAPObserver for more information
         */
        void HandleSAPEvent( TSAPEvent aEvent );
    public: // From MCAStoredGroupsObserver

        /**
         * Called when a group is deleted.
         * @param aId The ID of the deleted group
         */
        void HandleDelete( const TDesC& aId );

        /**
         * Called when a group is added.
         * @param aId The ID of the added group
         */
        void HandleAddition( const TDesC& aId );

        /**
         * Called when a group is changed.
         * @param aId The ID of the changed group
         */
        void HandleChange( const TDesC& aId );

        /**
         * Called when backup/restore starts/stops.
         * There is no distinction between backup/restore (Symbian limitation).
         * However, the called party should fetch new list(s) as they
         * may have changed if the operation was restore operation.
         * @param aEventType Type of backup/restore event
         */
        void HandleBackupRestoreEvent(
            const TStorageManagerGlobals::TCAObserverEventType aEventType );

    public: //Added methods for sorting of openchats

        //To Insert a newly arrived Invite on top of openchats list
        void InsertInviteItemL( const MCAInvitation* aInvitation );
        void InsertGroupItemL( MCAStoredGroup* aGroup );
        void InsertConversationItemL( const TDesC& aUserId,
                                      const TDesC& aIdentification );
        void MoveGroupItem( MCAStoredGroup* aGroup, TInt aFoundIndex );
        void RemoveConversationItem( const TDesC& aUserId );

        TBool IsOpenChatsMsgPending(
            const TEnumsPC::TItem aType,
            const TDesC& aId );

    public:       //from MCAMainViewArrayPC.h

        /**
        * See MCAMainViewArrayPC.h
        */
        const TDesC& PendingMessageInfo();

        /**
        * See MCAMainViewArrayPC.h
        */
        const TDesC& ChatGroupPendingMessageInfo( TDesC& aGroupName );

        /**
        * See MCAMainViewArrayPC.h
        */
        TInt GetIndexOfTopPendingMessage();

    public:        // MCAListInitObserver
        /**
         * Callback HandleListInitChange from presence list manager
         * after the list init complete
         */
        void HandleListInitChange( TBool aCompleted );


    private:

        TStorageManagerGlobals::TFilterType GetFilterConverterUITOEngine(
            TEnumsPC::TFilterType aFilter );

        void HandleError( TInt );

        void HandleItemRemovalL( const TDesC& aContactId );

        void HandleItemAdditionL( MCAStoredContact & aContact );


    private://Added methods for sorting of openchats

        TInt ChatGroupMessagesPendingCount() const;

        void ResetOpenChatsArrayL( TEnumsPC::TItem aItem );

        void InsertIntoInactiveOpenChats( TBool aEnd );

        void SetContactListIndex();
        TEnumsPC::TItem GetOpenChatType( TInt aIndex ) const;

        TInt RemoveGroupItem( const TDesC& aGroupId );

        //method to get bool equivalent for selction mode. Added to make code readable
        //Returns true if single list view selection mode else false.
        TBool SingleListViewSelectMode() const;

        void RefreshOpenChatsListItemL();

        void RefreshGroupItem( MCAStoredGroup* aGroup, TInt aFoundIndex );


    private:

        //Refenence to engine
        CCAEngine& iEngine;

        TBool iSkipStorageEvents;

        /**
        * list of items here
        */
        RPointerArray<MCAArrayItemPC> iMainViewListItemArray;

        // we need to maintain the list of observers here.whenever any event is
        // happening then we need call back the UI

        /*
        * Observers : replace this array with pointer variable and overload = operator.
        */
        RPointerArray< MCAMainViewObserverPC > iMainViewObservers;

        /*
        * reference to the contact list model
        */
        MCAContactListModel& iContactListModel;

        /*
        * Reference to chat interface
        */
        MCAChatInterface& iChatInterface;

        /**
         * Reference to Invite Interface
         */
        MCAInvite& iInviteInterface;

        //Owns. Array of active invitations
        RPointerArray<MCAInvitation> iInvitationsArray;

        /*
        * reference to the contact list interface
        */
        MCAStoredContacts* iContactInterface;

        // is "all" item displayed in beginning of each list
        //used in displaying blockinglist
        TBool iShowAll;

        // Owns
        HBufC* iRejectReason;
        /*
        * starting index of Owndata	, it's 0 as per ui spec
        */
        int iOwnDataIndex;
        /*
        * starting index of OpenChats	, it's 1 as per ui spec
        */
        int iOpenChatsIndex;
        /*
        * starting index of all the contactlists
        * it's iOpenChatsIndex+iOwnDataIndex+NoofConversations, if anything is exists
        */
        int	iContactListIndex;
        /*
        * flag to check whether openchats folder expanded or collapsed
        */
        TBool iOpenChatListItemExpanded;
        /*
        * Flag to check whether own data added or not
        */
        TBool iOwnDataAdded;

        /*
        * Flag if mainviewarraypc is used in selectoin mode from contact selection dialog
        */

        TEnumsPC::TListSelectionType iSelectionMode;

        TInt iSelectedListIndex;

        TBool iSetListCollapsedFlag;


        TBool iForceExpandDone;
        /*
        * Flag if user or findbox is expanded the array pc
        */
        TBool iExpandAll;

        /*
         * reference to the settings interface
         */
        MCASettings* iSettings;

        /*
           * Owns. Buffer that hold's information about fetched item
           */
        HBufC* iData;

        TInt iMaxLengthOfIcons;

        /**
        * Current filter applied to the engine contact list model
        */
        TEnumsPC::TFilterType iFilter;

        /**
        * Default filter applied to the engine
        */
        TEnumsPC::TFilterType iDefaultFilter;

        /**
        * Current filter applied to the single list view array
        */

        TEnumsPC::TFilterType iSingleListArrayFilter;


        // This callback is required when ever the server is added or deleted
        // we get a callback and update the singlelist

        // We own this pointer
        CIMPSSAPSettingsStore* iSAPSettingsStore;


        TInt iPToPChatCount;
        MCASingleListViewCB* iSingleViewCB;

        TCAItemDetails iItemDetails;

        // Owns. Array of group data
        CCAGroupListArrayPC* iGroupArray;

        // Does not own. Pointer to Stored Groups Interface
        MCAStoredGroups* iChatGroups;

        //To track the deletion of stored groups and then the subsequent
        //addition of groups after logging in each time.
        TBool iGroupCountDown;
        // we own this pointer
        //holds the last logged in user id
        HBufC* iLastloggedinUserId;

        //Count of all the save Groups to track the Open Count count on login
        TInt iOnlySavedGroups;

        /**
        * list of open chats items here
        */
        RPointerArray<MCAArrayItemPC> iOpenChatsArray;

        //Beginning index of Non-Joined groups in iOpenChatsArray
        TInt iOpenChatsInActiveIndex;
        MCASyncObserverPC* iSyncObserver;


        // Own this Pointer, as constrution of which fires an event to PEC engine
        CIMPSPresenceConnectionUi *iConnectionUI;


        // store the Contact's userId which need to refresh after contact list fetching complete.
        RPointerArray<HBufC> iRefreshConvArray;

    };

#endif   //CCAMainViewArrayPC_H
// End of File