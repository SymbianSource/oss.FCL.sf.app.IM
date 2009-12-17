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
* Description:  Container class for search view
*
*/



#ifndef CCASEARCHVIEWCONTAINER_H
#define CCASEARCHVIEWCONTAINER_H

//  INCLUDES
#include "MCASearchObserverInterfacePC.h"
#include "MCALayoutChangeObserver.h"
#include "MCAServiceStateObserver.h"
#include "TEnumsPC.h"


#include <bldvariant.hrh>
#include <eiklbo.h>
#include <coecntrl.h>


// FORWARD DECLARATIONS
class CAknDoubleStyleListBox;
class MCAViewSwitcher;
class MCARecipientObserver;
class CCABlockingUI;
class CAknTitlePane;
class MCASearchInterfacePC;
class MCASearchDataPC;
class CCACommandManager;
class CCAAppUi;



// CLASS DECLARATION

/**
 *  Container class for search view
 *
 *  @lib chatng.app
 *  @since 3.2
 */
class CCASearchViewContainer :  public CCoeControl,
            public MEikListBoxObserver,
            public MCASearchObserverInterfacePC,
            public MCALayoutChangeObserver,
            public MCAServiceStateObserver
    {
    public:

        // Type of search
        enum TSearchType
            {
            ESearchGroups = 0,
            ESearchUsers
            };


    public:  // Constructors and destructor

        /*
        * Static constructor
        * @param aSearchInterfacePC ,search Interface
        * @param aRect ref. to Parent rect
        * @param aViewSwitcher ref. to  View switcher
        * @param aMenuObserver ref. to recipient observer
        * @param aSearchUsers is true if Search is of type ESearchUsers
        * @param aGroupSyncDone is true if Search is of type ESearchGroups
        * @param aListId ref. to ListId of the contact to be added
        */
        static CCASearchViewContainer* NewL(
            MCASearchInterfacePC* aSearchInterfacePC,
            const TRect& aRect, MCAViewSwitcher& aViewSwitcher,
            MCARecipientObserver& aMenuObserver,
            TBool aSearchUsers,
            TBool aGroupSyncDone,
            const TDesC& aListId );

        /**
        * Destructor.
        */
        virtual ~CCASearchViewContainer();

        /**
        * From CCoeControl
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    protected:

        /**
        * Default Symbian OS constructor.
        * @param aRect Parent rect
        * @param aListId ref. to ListId of the contact to be added
        */
        void ConstructL( const TRect& aRect, const TDesC& aListId );

        /*
        * Constructor
        * @param aSearchInterfacePC ,search Interface process component
        * @param aViewSwitcher ref. to  View switcher
        * @param aMenuObserver ref. to recipient observer
        * @param aSearchUsers is true if Search is of type ESearchUsers
        * @param aGroupSyncDone is true if Search is of type ESearchGroups
        */
        CCASearchViewContainer( MCASearchInterfacePC* aSearchInterfacePC,
                                MCAViewSwitcher& aViewSwitcher,
                                MCARecipientObserver& aMenuObserver,
                                TBool aSearchUsers,
                                TBool aGroupSyncDone );

    public: // New functions

        /**
        * This method shows search type dialog
        */
        TBool DisplaySearchTypeDlgL();

        /**
         * This method shows search criteria dialog
         * @param aSearchType Type of search ESearchGroups|ESearchUsers
         */
        static TBool DisplaySearchCriteriaDlgL( const TSearchType aSearchType );

        /**
         * This method shows search string data query
         * @param aSearchCriteria specifies Search Criteria eg ESearchUserName etc.
         */
        static TBool DisplaySearchStringQueryL(
            TEnumsPC::TSearchCriteria aSearchCriteria );

        /**
         * This method is called when users' search criteria is ESearchUserName
         */
        static TBool DisplayMultiLineDataQueryL( );

        /**
         * This method returns current search type
         * @return ESearchGroups or ESearchUser
         */
        TSearchType SearchType() const;

        /**
        * Searches more results.
        */
        void SearchMoreL();

        /**
        * Searches previous results.
        */
        void SearchPreviousL();

        /**
        * Sets the observer to NULL so we cannot receive notifications anymore.
        */
        void SetObserverToNull();

        /**
         * Saves focused group to favourite.
         */
        void SaveAsFavouriteL();

        /**
         * Adds contact to friends
         */
        void AddToFriendsL();

        /**
         * Starts conversation with contact
         */
        void StartConversationL();

        /**
         * Invites contact
         */
        void InviteL();

        /**
         * Ends search. Called when left search view
         */
        void EndSearchL();

        /**
         * Starts new search
         */
        void NewSearchL();

        /**
         * Joins to selected group
         */
        void JoinToGroupL();

        /**
         * Blocks selected person
         */
        void BlockUserL();

        /**
         * Checks if user is blocked
         * @return ETrue if blocked
         */
        TBool IsBlocked() const;

        /**
         * this method returns handle to listbox
         * @return handle to listbox
         */
        CEikListBox* ListBox() const;

        /**
         * this method returns search data
         * @return searchdata
         */
        TPtrC SearchData() const;

        /**
         * This method returns the onlinestatus of currently selected contact
         * returns EFalse if the contact is offline or
         * if the selected item is group
         * @return ETrue if online contact
         */
        TBool IsOnline() const;

        /**
         * Displays text query for blocking users
         */
        void BlockUserWVIdL();

        /**
         * Displays unblock query
         */
        void UnBlockL();

        /**
         * Displays blocked list
         */
        void DisplayBlockedListL();

        /**
         * This method checks if there is more results available
         * @return EFalse if the search cannot be continued and
         *         ETrue if the search can be continued.
         */
        TBool MoreResultsAvailable() const;

        /**
         * This method checks if there is previous results available
         * @return EFalse if the search hasn't any previous results
         *         ETrue if there is any previous results
         */
        TBool PrevResultsAvailable() const;

        /*
         * Get the storage type of this group and check if it is a persistent grp.
         * If the group is marked as favourite, then it will be persistent
         * @return ETrue if favourite else EFalse
         */
        TBool IsFavouriteChatGroup() const;

        /**
        * After search we must update "the view" list box.
        * @param aGroup specifies whether this is group or not.
        */
        void UpdateListBoxL( TBool aGroup );

        /**
         * Group synchronization has been done
         */
        void SetGroupSyncDone();

        /**
         * List id of list where possible new contact is added.
         */
        const TDesC& ListId() const;

        /**
        * Make this object responsible for destroying itself.
        * Destroying happens after the asynchronous task is complete.
        * Deletion is asynchronous so the object might not be destroyed
        * before this method returns.
        * @return None
        */
        void DeleteSelfD();

        /**
         * sets the primary and secondary text of this view
         *
         *
         */
        void SetEmptyTextsToListboxL();

    private: // Functions from MCASearchObserverInterfacePC

        /**
         * @see MCASearchObserverInterfacePC
         */
        void HandleSearchError( TInt aErrorCode );

        /**
         * @see MCASearchObserverInterfacePC
         */
        void HandleSearchFinished( );

    private: // From MCALayoutChangeObserver

        /**
         * Called when layout/skins change
         * @since 3.2
         */
        void LayoutChangedL( TInt aType );

    private: // MCAServiceStateObserver

        /**
         * @see MCAServiceStateObserver
         */
        void HandleServiceStateChangeL( TServiceState aState );

    private:  // New functions

        /**
         * Starts search procedure
         * @param aSearchPairs CSearchPairs
         */
        void StartSearchL( );

        /**
        * CIdle-kind of function for background task implementation
        * @param aInstance Instance of this class
        * @return Need for recall
        */
        static TInt BackgroundTasks( TAny *aInstance );

        /**
        * Real implementation of background tasks
        * @return Need for recall
        */
        TInt DoBackgroundTasks();

        /**
         * Startup container.
         * Show wait note or the search type dialog
         * @return ETrue if call this again, EFalse if not (for CIdle)
         */
        TBool ContainerStartL();

    private: // Functions from base classes

        /**
        * From MEikListBoxObserver, Handles event's generated by listbox
        * @param aListBox Pointer to listbox from where this event originated
        * @param aEventType Type of event received.
        */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

        /**
        * From CoeControl, Handles "size changed"-events..
        */
        void SizeChanged();

        /**
        * From CoeControl, Returns the number of control contained by this class.
        * @return Number of controls contained
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl, Returns handle to control pointed by aIndex
        * @param aIndex Wanted control's index [0..n]
        * @return Handle to wanted control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl, Handles key-events
        * @param aEvent Event that occured
        * @param aType Type of key-event (EEventKey, EEventKeyUp or EEventKeyDown)
        * @return Containers response to event (EKeyWasNotConsumed/ EKeyWasConsumed)
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );

        /**
         * Gets help context
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
        * Fetches presence information of the contacts.
        * @param aLowIndex is the lowest index whose presence we can get.
        * @param aHighIndex is the highest.
        */
        void FetchPresenceL( const TInt aLowIndex, const TInt aHighIndex );

        /**
         * From CCoeControl.
         * Handle focus event change.
         * @see CCoeControl
         */
        void FocusChanged( TDrawNow aDrawNow );



    private: //New functions

        /**
         * This function updates text to navi pane
         * @param aEndSearch specifies if this happens in end search
         *        or elsewhere.
         */
        void UpdateNaviPaneTextL( TBool aEndSearch );

        /**
        * Clears list box contents. I.e. item array.
        */
        void ClearListBoxContents();

    private:    // Data

        //Owns
        CAknDoubleStyleListBox* iListBox;

        //Pointer to SearchInterfacePC,doesnt own
        MCASearchInterfacePC* iSearchInterfacePC;

        //Pointer to SearchDataPC,doesnt own
        MCASearchDataPC* iSearchDataPC;

        //View switching interface
        MCAViewSwitcher& iViewSwitcher;

        //Owns. Blocking UI side
        CCABlockingUI* iBlockingUI;

        // Search type
        TSearchType iSearchType;

        // Previously used search type
        TSearchType iPreviousSearchType;

        // Current page number
        TInt iCurrentResultPage;

        // How many pages of results we have
        TInt iTotalResultPages;


        // Information indexes to the search data
        TInt iLowIndex;
        TInt iHighIndex;
        TInt iNextSearchIndex;

        // Owns: This array describes online users.
        // These users are users that are online in one search.
        CDesCArrayFlat* iOnlinePresenceList;
        //Owns
        CDesCArray* iItemArray;
        //Online friends
        RArray<TBool> iOnlineArray;

        MCARecipientObserver& iMenuObserver;

        // Are there previous results
        TBool iPreviousResultsShowing;

        // Owns.
        CIdle* iIdle;

        // Is this container responsible to destroy itself
        // when the async task is complete.
        TBool iDeleteFlag;

        // Is async task complete (ETrue) or not
        TBool iTaskComplete;

        // are we searching users (ETrue) or groups (EFalse)
        TBool iSearchUsers;

        // group synchronization done (ETrue) or not
        TBool iGroupSyncDone;

        // wait note visible (ETrue)
        TBool iWaitVisible;

        /// List id of added contact, if needed.
        HBufC* iListId;

        //Doesn't own, pointer to CCAAppUi
        CCAAppUi* iAppUi;

        //Doesn't own, Pointer to CCACommandManager
        CCACommandManager* iCommandManager;

    };

#endif      // CCASEARCHVIEWCONTAINER_H

// End of File
