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
* Description:  Container for all controls in single-list view
*
*/



#ifndef CCASINGLELISTCONTAINER_H
#define CCASINGLELISTCONTAINER_H

// INCLUDES
#include "MCALayoutChangeObserver.h"
#include "MCAMainViewObserverPC.h"

#include <bldvariant.hrh>
#include <eiklbo.h> // MEikListBoxObserver
#include <AknInfoPopupNoteController.h> // for CAKnInfoPopupNote
#include <coecobs.h>
#include <mvpbksinglecontactoperationobserver.h> // for phonebook2
#include <mvpbkcontactstoreobserver.h>
#include <aiwservicehandler.h>
#include <TVPbkFieldVersitProperty.h>
#include <aiwcontactselectiondatatypes.h>

#include "MCAServerChangeNotify.h"
#include <MVPbkContactStoreListObserver.h>

// FORWARD DECLARATIONS
class CEikListBox;
class CAknSearchField;
class MCAViewSwitcher;
class CCAStatusPaneHandler;
class CCABlockingUI;
class CCAAppUi;
class CCAContactEditor;
class CEikButtonGroupContainer;
class CAknInfoPopupNoteController;
class CCAContactListBox;
class CAknListBoxFilterItems;
class MCABlockingPC;


class MCACommandHelper;
class MCAMainViewArrayPC;
class MCASearchInterfacePC;
class MCASearchDataPC;

class CCAView;
//for phonebook
class CVPbkContactManager;
class CPbk2SortOrderManager;
class MPbk2ContactNameFormatter;
class CAiwServiceHandler;
// CLASS DECLARATION
/**
*  Container class for friends SINGLE list view
*
*  @lib chatng.app
*  @since 3.2
*/
class CCASingleListContainer :  public CCoeControl,
            public MEikListBoxObserver,
            public MCAMainViewObserverPC,
            public MCALayoutChangeObserver,
            public MCoeControlObserver,
            public MCAServerChangeNotify,
            public MAiwNotifyCallback,
            public MVPbkSingleContactOperationObserver,
            public MVPbkContactStoreListObserver


    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        * @param aViewSwitcher Handle to view switcher
        * @param aSelectedIndexId Contact item ID of user that was selected
        *        in view's listbox
        */
        void ConstructL( TRect aRect, MCAViewSwitcher& aViewSwitcher,
                         TInt aSelectedIndexId );

        /**
        * Destructor.
        */
        virtual ~CCASingleListContainer();

        /*
        * C++ default constructor
        * @param aCommandHelper: reference to CommandHelper instance
        * @param reference to App Ui instance
        */
        CCASingleListContainer( MCACommandHelper& aCommandHelper, CCAView& aView, CCAAppUi* aAppUi );

    public:
        /**
         * From MCAMainViewObserverPC, called when a contact is deleted.
         * @since 2.1
         * @see MCAMainViewObserverPC
         */
        virtual void HandleDelete( const TDesC& aContactId ,
                                   TEnumsPC::TItem aType );

        /**
         * From MCAMainViewObserverPC, called when a contact is added.
         * @since 2.1
         * @see MCAMainViewObserverPC
         */
        virtual void HandleAddition( TEnumsPC::TItem aType, TInt aIndex );

        /**
         * From MCAMainViewObserverPC, called when a contact is changed.
         * @since 2.1
         * @see MCAMainViewObserverPC
         */
        virtual void HandleChange( TEnumsPC::TItem aType, TInt aIndex,
                                   TEnumsPC::TChange changeType );
        /**
        * Gets called when watched friend's presence status changes. This event
        * is restricted to only network originated events, so if user refreshes
        * the friends-list by himself, then this event will not raise.
        * @param aString: Contact that was changed
        * @aOnlineStatus: online status of contact that was changed
        */
        virtual void HandleWatcherEvent( const TDesC &  aString ,
                                         TEnumsPC::TOnlineStatus aOnlineStatus );
        /**
        * Handles watcher events
        * @param aString: Contact that was changed
        * @aOnlineStatus: online status of contact that was changed
        */
        void DoHandleWatcherEventL( const TDesC &  aString ,
                                    TEnumsPC::TOnlineStatus aOnlineStatus );
        /**
         * See MCAMainViewObserverPC
         */
        void HandleInvitationEventL( TEnumsPC::TInviteType aInviteType,
                                     const TDesC& aUserID,
                                     const TDesC& aMessage );

        /**
         * See MCAMainViewObserverPC
         */
        void HandleInvitationResponse( TBool aAcceptance,
                                       const TDesC& aUserId,
                                       const TDesC& aGroupName,
                                       const TDesC& aResponse );
    public:


        /*
        * IsChatsForceExpanded
        * chat expanded or not
        * @return user id
        */
        TBool IsChatsForceExpanded();

        /*
        * Return the user id of the selected contact, group id of the
        * selected group, user id of sender of selected invite
        * @return user id
        */
        TPtrC SelectedUserId() const;

        /**To be added after refactoring of invitationview dialog
         * opens the selected invitation
         * @param aIndex Index of invitation to be opened
         */
        void OpenInvitationL( );

        /**
         * Deletes currently active invitation
         */
        void DeleteInvitationL();
        /**
         * To handle the addition of an item to the list box
         */
        void HandleItemAdditionL();

        /**
         * Renames contact list on server
         */
        void RenameListL();

        /**
         * Expands or collapses contact list on view
         */
        void ExpandCollapseListL();


        /**
        * Returns selected contact's type
        * @return currently selected contact's type. If no contacts then return EUnknown
        */
        TEnumsPC::TItem SelectedType() const;

        /**
        * Returns synchronised status
        * @return synchronization status
        */
        TEnumsPC::TSyncStatus Synchronised() const;

        /**
        * Returns the status of own data
        * @return online status for the owndata
        */
        TEnumsPC::TOnlineStatus OwnStatus() const;

        /**
        * Returns selected contact's status
        * @return currently selected contact's online status
        */
        TEnumsPC::TOnlineStatus GetOnlineStatus() const;

        /**
        * Returns the collapsed state of selected contactlist
        * @return true if contactlist is collapsed else return false
        */
        TBool IsCollapsed() const;

        /**
        * Returns the watch status of selected contact
        * @return true if the contact is being watched else return false
        */
        TBool IsWatched() const;

        /**
        * Returns the no. of items in the listbox
        * @return integer
        */
        TInt Count() const;

        /**
        * Refreshes the current screen.
        * Actual implementation
        * @since 2.1
        * @param aSortList Should the list be sorted or not
        */
        void DoRefreshViewL( TBool aSortList ) const;

        /**
        * Refreshes the current screen
        * @since 2.1
        * @param aSortList Should the list be sorted or not
        */
        void RefreshView( TBool aSortList ) const;

        /**
        * Sets current recipient for message
        * @since 2.1
        */
        //void SetCurrentRecipient() const;

        /**
        * Get's current index of listbox
        * @since 2.1
        * @return Index of currently selected item else KErrNotFound
        */
        TInt CurrentListboxIndex() const;

        /**
        * Deletes currently focused friend
        * @since 2.1
        */
        void DeleteFriendL();

        /**
        * Edit/add friend from wanted listbox index
        * @since 2.1
        * @param aCreateNew Flag to create new friend
        * @return User id of edited or new contact
        */
        TInt EditFriendL( TEnumsPC::TContactEditorMode aCreateNew );

        /**
         * Move highlighted contact to another list
         * @since 2.1
         */
        void MoveContactL();

        /**
         * Updates view after edit
         * aIndex is the index of item that has changed
         * aType is list item type
         */
        void UpdateViewL( TInt aIndex, TEnumsPC::TItem aType, TBool aSyncSuccess );

        /**
         * Refresh the presence status of contacts
         * @param aManualLaunch ETrue if the user has started the command
         */
        void RefreshPresenceL( TBool aManualLaunch );

        /**
        * Switches the container to details mode
        * @since 2.1
        */
        void ShowDetailsL();

        /**
        * Checks if the details view is active
        * @since 2.1
        * @param aContext On return contains ETrue if user has requested
        *        a context sensitive menu to be shown
        * @return ETrue if the details view is active, EFalse otherwise
        */
        TBool MenuShown( TBool& aContext );

        /**
        * Check if listbox has an item that is currently focused
        * @since 2.1
        * @return ETrue, if there are any items.
        */
        TBool ListboxItems() const;

        /**
         * Displays Blocked List
         * @since 2.1
         */
        void DisplayBlockedListL();

        /**
         * Displays unblock list query
         * @since 2.1
         */
        void DisplayUnblockListQueryL();

        /**
         * Blocks selected user
         * @since 2.1
         */
        void BlockUserL();

        /**
         * Checks if user is blocked
         * @since 2.1
         * @return ETrue if blocked
         */
        TBool IsBlocked() const;

        /**
         * Finds joined chats for focused user
         */
        void FindJoinedChatsL();

        /**
         * Displays confirmation note
         * @param aTextResourceId Resource ID of text to be shown
         */
        void DisplayConfirmationNoteL( const TInt aTextResourceId );

        /**
        * Start conversation or switches to existing one
        */
        void OpenConversationL();
        /**
        * closes existing conversation
        * @since 3.2
        */
        void CloseConversationL() ;

        /**
         * Returns handle to listbox
         * @return handle to CEikListBox
         */
        CEikListBox* ListBox() const;

        /**
         * Displays block text query
         */
        void DisplayBlockTextQueryL();

        /**
         * @return ETrue if opening conversation is ongoing
         */
        TBool OpeningConversation();

        /**
         *  Clears any strings from findbox and resets it.
         */
        void ResetFindBoxL();

        /**
         * Skip the HandleAddition and HandleDelete originating as IM Storage
         * observer events if we have wait note ongoing. Speed optimization. When
         * changing the list there can be many contacts, which would result in
         * useless (and slow) redrawing of the friends list.
         * Reset by the view once the wait note is dismissed.
         * @param aSkip ETrue if events are to be skipped
         */
        void SetSkipStorageEvents( TBool aSkip );

        /**
         * Prepare destruction, for example delete objects
         * which break some functionality because of lazy destruction of this
         * container. ( iFindBox )
         */
        void PrepareDestruction();

        /**
         * Sets the lock status of softkeys
         * @param aLock new status of lock (ETrue: locked, EFalse: Unlocked)
         */
        void SetCbaLockL( TBool aLock );

        /**
        * Make this object responsible for destroying itself.
        * Destroying happens after the asynchronous task is complete.
        * Deletion is asynchronous so the object might not be destroyed
        * before this method returns.
        * @return None
        */
        void DeleteSelfD();

        /**
        * Handles events from findpane and forwards them to listbox filter.
        * @see MCoeControlObserver
        */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

        /**
         * Sets current list box index.
         * @param aIndex Index for list box.
         */
        void SetListboxIndexL( TInt aIndex );

        /**
         * Updates Softkeys according to current focus
         * @param aUseDefaultCba Flag to force the default sofkeys for view.
         * Default value is false.
         */
        void UpdateCbaL( TBool aUseDefaultCba = EFalse );

        /**
         * This method launches chat properties dialog in add mode
         * @return The groupd id of newly created group
         *         or NULL if not successful
         *         The ownership is passed to calling function!
         */
        HBufC* CreateNewChatL();

        /**
         * This method sets the currently focused group
         * @param aGroupId The WVID of group to be focused
         */
        void FocusGroupL( const TDesC& aGroupId );



        /**
         * This method launches pop-up list that contains info about joined chat
         */
        void DisplayChatInfoL();


        /**
         * Join group when group id is entered manually
         */
        //void JoinGroupL(TPtrC aGroupId);

        /**
         * Joins group after creating new chat
         * @param aGroupId Group id that is created
         */
        void JoinGroupL( const TDesC& aGroupId );

        /**
         * Checks if user has joined to chat group
         * @param aIndex Index of chat group
         * @return EFalse if not joined
         */
        TBool IsJoinedToGroup( );

        /**
         * Checks if chat group is favourite
         * @return EFalse if the isn't saved
         */
        TBool IsFavourite( );

        /**
         * Saves the focused general chat group as favourite
         */
        void SaveAsFavouriteL();

        /**
         * Deletes the focused favourite chat group
         * @param aGroupId The group id of the group to delete
         */
        void DeleteGroupL( TPtrC aGroupId );

        /**
         * Leaves focuses joined chat group
         */
        void LeaveGroupL();

        /**
         * Checks if user is admin in selected group
         * @return ETrue if admin else EFalse
         */
        TBool IsUserAdmin() const;

        /**
         * This method displays pop-up list containing joined members
         */
        void DisplayJoinedMembersL();

        /**
         * This method launches edit chat properties dialog
         */
        void EditChatGroupPropertiesL();


        /**
         * Displays list query where are contacts from contact list
         * @since 3.2
         */
        void DisplayRecipientListQueryL();


        /**
         * Displays text query
         * @since 3.2
         */
        void DisplayRecipientTextQueryL();

        /**
         * Displays text query to enter group ID.
         *@dataPtr	pointer to TDes
         */
        void DisplayRecipientTextQueryForGrpIDL( TDes& aDataPtr );

        /**
         * Switches to selected conversation
         * @param aUserId User id
         * @since 3.2
         */
        void SwitchToConvViewL( const TDesC& aUserId = KNullDesC );

        /**
        * return true if the user is the owner of the group
        */
        TBool IsOwnGroup() const;

        /**
        * return true if the background task is completed
        */
        TBool IsBckGrdTaskComplete ( );



    private: // New methods

        /**
         * Checks that the presence status of contact is up-to-date
         * @param aContact Contact to be updated
         * @return The error code of the presence fetch operation
         */
        TInt UpdatePresenceL( const TInt aIndex );

        /**
        * Activates find-pane
        */
        void ActivateFindPaneL();

        /**
        * De-activates find-pane
        */
        void DeactivateFindPaneL();

        /**
        * Layouts current components according to AVKON LAF
        */
        void SetLayout();

        /**
         * Displays info note for corresponding resource id
         * @param aResourceId Resource ID for info note
         */
        void DisplayInfoNoteL( const TInt aResourceId );

        /**
         * Displays scroll bars
         * @param aListBox Handle to listbox
         */
        void UpdateScrollBars( CEikListBox* aListBox );

        /**
         * Update indexes and find-pane after deletion
         */
        void UpdateViewAfterDeleteL();

        /**
         * Updates the listbox filter after the list is changed
         */
        void UpdateFilterL() const;

        /**
        * Loads/updates bitmaps
        */
        void LoadBitmapsL();

        /*
        * Error handler.
        * @param aError Error code
        */
        void HandleError( TInt aError ) const;

    public:

        //Newly added functions due to Phonebook2 related changes
        /**
         * Used to select a single contact from phonebook
         * Starts the single contact selection service from phonebook
         */
        void ContactSelectionFromPhoneBookL();

        /**
         * Get the WVId of the oontact selected from phonebook
         * @param aTitle - the title of the contact aNumberToSearch
         * @param aNumberToSearch - the phone number of the contact
         * @returns the WVId of the oontact selected from phonebook
         */
        HBufC* GetWVIdOfSelPbkContactL( const TDesC& aTitle,
                                        const TDesC& aNumberToSearch );


        /**
         * Add the contact selected form phonebook to the contact list
         * @param aContact - the contact selected from phonebook
         * and which is to be added to the contact list
         */
        void AddSelectedPbk2ContactL( MVPbkStoreContact* aContact );


        /**
         * @param aArray - the Versit properties mapped to a field type( MVPbkFieldType ).
         * @param aName - Name of the field type to be matched e.g. EVPbkVersitNameTEL
         * @returns ETrue if aArray contains a field type of name aName
         */
        TBool ContainsFieldType( TArray<TVPbkFieldVersitProperty> aArray,
                                 TVPbkFieldTypeName aName );
        /**
         * Executes a service command for single phonebook contact selection service
         * @param aData - version 2 of single entry contact selection data used to add filters
         * @param aUri1,aUri2,aUri3 - can be used to provide some input param to the
         * the generic service command handler, if required
         */
        void ExecutePbkSingleEntryFetchL( TAiwSingleEntrySelectionDataV2 aData,
                                          const TDesC& aUri1 = KNullDesC,
                                          const TDesC& aUri2 = KNullDesC,
                                          const TDesC& aUri3 = KNullDesC );

        /**
         * Does some Intializiation required
         * like AIW service handler intialization for Single
         * Contact Selection Service from phonebook2
         */
        void InitPbk2SingleEntryFetchL();
        /**
        * Releases all the resources initialized for phonebook single entry fetch
        */
        void ReleasePbkSingleEntryFetch();

        /**
        * Closes all the contact stores
        */
        void CloseAllContactStores();


        //From MAiwNotifyCallback

        /**
         * From MAiwNotifyCallback
         * Call back
         *
         * @param aCmdId the command ID
         * @param aEventId The event ID
         * @param aEventParamList Input paramiter list
         * @param aInParamList Output Paramiter list
         * @return error code
         */
        TInt HandleNotifyL( TInt aCmdId, TInt aEventId,
                            CAiwGenericParamList& aEventParamList,
                            const CAiwGenericParamList& aInParamList );



        //From MVPbkSingleContactOperationObserver

        /**
         * From MVPbkSingleContactOperationObserver
         * called by observer when require opration finished
         *
         * @param aOperation the opration instance
         * @param aContact contact for the operation
         * @return none
         */
        void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );

        /**
         * From MVPbkSingleContactOperationObserver
         * called by observer when require opration faild
         *
         * @param aOperation the opration instance
         * @param aError a system wild error code
         * @return none
         */
        void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, TInt aError );


        //From MVPbkContactStoreObserver
        void OpenComplete();
        /**
         * From MVPbkContactStoreObserver
         * called by observer when Store ready
         *
         * @param aContactStore the contace store
         * @return none
         */
        void StoreReady( MVPbkContactStore& aContactStore );

        /**
         * From MVPbkContactStoreObserver
         * called by observer when Store is not ready
         *
         * @param aContactStore the contace store
         * @return none
         */
        void StoreUnavailable( MVPbkContactStore& aContactStore,
                               TInt aReason );


        /**
        * From MVPbkContactStoreObserver
        * called by observer when there is store event
        *
        * @param aContactStore the contace store
        * @param aStoreEvent the store event
        * @return none
        */
        void HandleStoreEventL( MVPbkContactStore& aContactStore,
                                TVPbkContactStoreEvent aStoreEvent );

        /**
         * Show info popup note
         */
        void ShowInfoPopupL();

        /**
         * Get access to listbox model's filter
         * @return model's filter
         */
        CAknListBoxFilterItems* ListFilter();



    private: // Functions from base classes

        /**
        * From CoeControl, Handles "size changed"-events
        * @see CCoeControl
        */
        void SizeChanged();

        /**
         * From CoeControl
         * @see CCoeControl
         */
        void Draw( const TRect& aRect ) const;

        /**
        * From CoeControl, Returns the number of control contained by this
        * class.
        * @see CCoeControl
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl, Returns handle to control pointed by aIndex
        * @see CCoeControl
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl, Handles key-events
        * @see CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent,
                                     TEventCode aType );

        /**
        * From MEikListBoxObserver, Handles event's generated by listbox
        * @see MEikListBoxObserver
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );


        /**
         * From CCoeControl
         * Gets help context
         * @param aContext Help context
         * @see CCoeControl
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
         * From CCoeControl.
         * Handle focus event change.
         * @see CCoeControl
         */
        void FocusChanged( TDrawNow aDrawNow );

        /** From MCALayoutChangeObserver
        * Called when layout or skin changes
        * @see MCALayoutChangeObserver
        */
        void LayoutChangedL( TInt aType = 0 );

        /** From MCASettingsObserver
         * Handles setting changes.
         * @param aChangedSettingEnum specifies the changed setting.
         * @since 2.1
         */
        void HandleSettingsChangeL( TInt aChangedSettingEnum );

        /**Handles server changes.
         * @param aKey specifies key
         * @since 2.1
         */
        void HandleServerChangeL ( TServerNotify aKey );

    private: //data

        /**
        * reference to the Search Data Interface with the ProcesssManager
        */
        MCASearchDataPC* iDataInterfacePC;

        /**
        * reference to the Search Data Interface with the ProcesssManager
        */
        MCASearchInterfacePC* iSearchInterfacePC;


        /**
        *	Reference to the MainViewArrayPC instance with the Processmanager
        */
        MCAMainViewArrayPC* iMainViewArrayPC;


        // Owns. Listbox to view friends-list data
        CCAContactListBox* iListBox;

        // Owns. Search field.
        CAknSearchField* iFindbox;
        TBool iFindPaneIsVisible;

        // Doesn't own. Handle to view switcher
        MCAViewSwitcher* iViewSwitcher;

        //Owns. Blocking UI
        CCABlockingUI* iBlockingUI;

        //Active scheduler wait during search
        CActiveSchedulerWait iSearchWait;

        // Doesn't own. Pointer to status pane handler.
        CCAStatusPaneHandler* iStatusPane;

        // doesn't own
        CCAAppUi* iAppUi;


        // Is this container responsible to destroy itself
        // when the async task is complete.
        TBool iDeleteFlag;

        // Is async task complete (ETrue) or not
        TBool iTaskComplete;

        // ETrue if opening conversation is ongoing
        TBool iOpeningConversation;

        // skip storage events (when the wait note is showing)
        TBool iSkipStorageEvents;

        // CBA. not owned
        CEikButtonGroupContainer* iCba;

        /// Popup info for showing info popupnote.
        CAknInfoPopupNoteController* iInfoPopup;

        /// Helper to invoke commands from container.
        MCACommandHelper& iCommandHelper;

        CCAView& iView;
        // Storage for previous focus
        TEnumsPC::TItem iLastFocusedItemType;

        // Lock status foc softkeys
        TBool iCbaLock;

        // To keep list box focus correct after blocking
        TInt iIndexPrevBlocking;

        //To track the blocked status
        TBool iBlocked;

        //To track the filter applied
        TInt iMyFilter;

        //To decide whether to show offline contacts or not
        TBool iShowOffline;

        //To autoupdate/refresh of contacts/lists
        TInt iAutoUpdate;

        //Does not own. reference to the blockingPC
        MCABlockingPC* iBlockingPC;

        /**
        * a flag to force expand the listbox if theres only one list item
        * whenever the application is launched/logged in
        */
        TBool iForceExpandSingleList;

        //for invitations
        TBool iInvitationDlg;

        TBool iInvitationOpen;

        // A flag to forceexpand the chats folder.
        TBool iForceExpandChats;

        TBool iPreviousState;

        TBool iCurrentState;
        // Own: Virtual Phonebook contact manager
        CVPbkContactManager* iContactManager;
        /// Own: AIW service handler
        CAiwServiceHandler* iServiceHandler;
        // Own: Contact name Formatter
        MPbk2ContactNameFormatter* iNameFormatter;
        // Own: Sort Order manager
        CPbk2SortOrderManager* iSortOrderManager;
        // Own: Pointer to CVPbkFieldTypeSelector
        CVPbkFieldTypeSelector* iContactViewFilter;
        //Flag to check if atleast one store is opened
        TBool iStoresOpened;
        //Flag to check if opening a store is completed
        TBool iOperationComplete;

        TBool iIsMovingFlag;
    };

#endif //CCASINGLELISTCONTAINER_H

// End of File
