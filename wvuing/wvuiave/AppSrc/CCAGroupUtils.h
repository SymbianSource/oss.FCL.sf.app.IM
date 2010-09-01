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
* Description:  Group handling utilities
*
*/



#ifndef CCAGROUPUTILS_H
#define CCAGROUPUTILS_H

//  INCLUDES
#include "MCAInviteConflictResolver.h"
#include "TEnumsPC.h"

#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class MCAViewSwitcher;
class CCAAppUi;
class MCAConversationMessage;
class MCAServerContactsArrayPC;
class MCAGroupPC;
class MCAGroupUtilsPC;
// CLASS DECLARATION

/**
*  Group properties editing and displaying utilities
*
*  @lib chatng.app
*/
class CCAGroupUtils : public CBase,
            public MCAInviteConflictResolver
    {
    public:  // constructor

        /**
        * Static constructor.
        * @param aEngine Handle to application engine
        * @param aViewSwitcher General interface for switching chat-client views
        * @return Instance of this class
        */
        static CCAGroupUtils* NewLC( MCAViewSwitcher& aViewSwitcher );

        /**
        * Destructor
        */
        ~CCAGroupUtils();

    public:

        // simple group generation ("SGC") FSM states
        enum TSGCState
            {
            ESGCInviteContacts = 0, // choose contacts to invite (Begin state)
            ESGCGroupName,          // select group name
            ESGCInviteMessage,      // invite message query
            ESGCScreenName,         // screenname query or alias usage
            ESGCCreateGroup,        // create the group
            ESGCDoSendInvite,       // send the invitations, if any
            ESGCDoJoin,             // actually join the group
            ESGCError,              // error state
            ESGCQueryCancel,        // ask for cancelling
            ESGCNone                // no state
            };

        // Chat group join mode
        enum TJoinMode
            {
            EStoredId = 0,  // Group id received from some store
            EManualId       // Group id entered manually by user
            };

    public: //from MCAInviteConflictResolver

        /**
         * @see MCAInviteConflictResolver for details
         */
        void ResolveInviteConflictL( const TDesC& aGroup,
                                     CDesCArray& aArray );

    public:  // new methods

        /**
         * This method checks if user is admin of the group
         * @param aGroupId: id of the group whose admin status has to be checked
         * @return ETrue if favourite else EFalse
         */
        TBool IsAdmin( const TDesC& aGroupId ) const;

        /**
         * Get the User Ids of participants of the group (joined users)
         * Makes copies of the descriptors.
         * @param aParticipantList - list of all the participants in a group
         * @param aGroupID - groupid for which participantlist is required
         */
        void GetParticipantsL( CDesCArray& aParticipantList,
                               const TDesC& aGroupID ) const ;

        /**
         * Leave the given group.
         * @param aGroupId The group to leave
         * @return IMPS error code from LeaveL
         */
        TInt LeaveGroupL( const TDesC& aGroupId );

        /**
         * This method displays pop-up list containing joined members
         * @param aGroupId The group whose members have to be displayed
         */
        void DisplayJoinedMembersL( const TDesC& aGroupId );

        /**
         * Delete the given group
         * @param aGroupId The group id of the group to delete
         * @return The errorcode
         */
        TInt DeleteGroupL( const TDesC& aGroupId );

        /**
        * Find the group name of group with given group id
        * @Param aGroupId: group id
        * @return : name of the group
        */
        TPtrC GetGroupName( const TDesC& aGroupId ) const;

        /**
         * Find a group from the store
         * @param aGroupId the ID of the group to find
         * @return true if group has been found else return false
         */
        TBool FindGroup( const TDesC& aGroupId ) const;

        /**
         * Saves the focused general chat group as favourite
         * @param aGroupId: id of the group which has to be saved
         */
        void SaveAsFavouriteL( const TDesC& aGroupId );

        /**
         * This method checks if group is favourite
         * @param aGroupId: id of the group whose favorite status has to be checked
         * @return ETrue if favourite else EFalse
         */
        TBool IsFavouriteChatGroup( const TDesC& aGroupId );

        /**
         * This method checks if the user has joined to the group
         * @param aGroupId: id of the group whose joined status has to be checked
         * @return ETrue if joined else EFalse
         */
        TBool IsJoinedToGroup( const TDesC& aGroupId );

        /**
         * This method checks if group owned(created) by the user
         * @param aGroupId: id of the group whose owner status has to be checked
         * @return ETrue if owned else EFalse
         */
        TBool IsOwnGroup( const TDesC& aGroupId ) const;

        /**
         * Launch chat group properties editor
         * @param aGroupId GroupID for target group
         */
        TInt EditChatGroupPropertiesL( const TDesC& aGroupId );

        /**
        * Launch chat group properties editor and create new chat
        * @param aResourceId Resource id for title text
        * @param aViewId ViewId of current view
        * @param aContactListID - invitation should be sent to all
        * online contacts of this list. aContactListID should be null
        * if focus was not on a contact list in singlelist view
        * @return WVID of newly created group or NULL if not successful
        *         The ownership is passed to calling function!
        */
        HBufC* CreateNewChatL(  TInt aResourceId, const TUid aViewId,
                                TBool aContactFound,
                                TEnumsPC::TListSelectionType aSelType = TEnumsPC::EMultiSelect,
                                TInt aSelectedListIndex = 0 );

        /**
        * Joins the specified chat group and switches the view
        * @param aGroupId GroupID
        * @param aScreenName Default screenname for query
        * @param aFwdMessage Forwarded message.
        * @param aJoinMode Is group id entered manually by user
        *                  or received from some storage.
        * @return Return value of real join-request
        */
        TInt JoinGroupL( const TBool aFromFwd,
                         const TDesC& aGroupId,
                         const TDesC& aScreenName = KNullDesC,
                         const MCAConversationMessage* aFwdMessage = NULL,
                         TJoinMode aJoinMode = EStoredId );

        /**
        * Switches to joined group
        * @param aGroupID GroupID
        * @param aFwdMessage forwarded message
        * @param aInviteQuery If ETrue, launch the invite query
        */
        void SwitchToJoinedGroupL( const TBool aFromFwd,
                                   const TDesC& aGroupID,
                                   const MCAConversationMessage* aFwdMessage = NULL,
                                   TBool aInviteQuery = EFalse );

        /**
        * Displays a delete query of user created chat rooms
        */
        TInt DisplayCreatedChatRoomsListL();

        /**
        * Displays chat room info dialog
        * @param aGroupId GroupID
        */
        void DisplayChatInfoL( const TDesC& aGroupId );

        /**
         * Do the asynchronous joining process via CAsyncCallBack.
         * Uses iErr, iAsync* variables.
         */
        TInt DoAsyncJoin();

        /**
         * Helper method for querying the screen name or alias.
         * Uses variation to determine the default value.
         * @param aScreenName Pointer to the screen name buffer
         */
        TInt ScreenNameQueryL( TDes& aScreenName );

        /**
         * Helper method for switching back to the view again
         * @param aGroupID
         * @param aPrevView View where to switch when pressing back
         * @param aNextView View which is switched to next
         */
        void PrepareToSwitchBackL( const TDesC& aGroupID,
                                   TUid aPrevView,
                                   TUid aNextView );
       TPtrC GetJoinGroupID();

    private: // new methods

        /**
         * Helper Asks screen name
         * @param aScreenName Reference to a descriptor where
         *        screen name will be stored.
         * @return EAknSoftkey enumeration
         */
        TInt AskScreenNameL( TDes& aScreenName );

        /**
         * Launch chat group properties editor and create new chat,
         * the normal style (with access and editor lists, etc.)
         * @param aResourceId Resource id for title text
         * @param aViewId ViewId of current view
         * @return WVID of newly created group or NULL if not successful
         *         The ownership is passed to calling function!
         */
        HBufC* CreateNewChatNormalL( TInt aResourceId, const TUid aViewId );

        /**
         * Launch chat group properties editor and create new chat,
         * simplified version.
         * @param aResourceId Resource id for title text
         * @param aViewId ViewId of current view
         * @param aContactListID listid for which the list of online contacts is required
         * for sending invitations
         * @return WVID of newly created group or NULL if not successful
         *         The ownership is passed to calling function!
         */
        HBufC* CreateNewChatSimpleL( TInt aResourceId, const TUid aViewId,
                                     TBool aContactFound,
                                     TEnumsPC::TListSelectionType aSelType,
                                     /*MCAServerContactsArrayPC*  aOnlineContacts = NULL*/
                                     TInt aSelectedListIndex = 0  );

        /**
         * FSM state change logic wrapper.
         * @param aAknResult The result from user query (Avkon enum),
         *                   to handle Cancels
         * @param aErrorCode The network error code from previous operation, if
         * any. Use KErrNone to force skipping of error state transition.
         * @param aNewState The new state to change to when successful
         */
        void StateChange( TInt aAknResult, TInt aErrorCode,
                          TSGCState aNewState );

    private: // constructors and destructors

        /**
        * constructor
        */
        void ConstructL();

        /**
         * Default constructor
         * @param aEngine Handle to engine
         * @param aViewSwitcher Handle to view switcher
         */
        CCAGroupUtils( MCAViewSwitcher& aViewSwitcher );

    private:    // Data

        // Doesn't own. Handle to appui
        CCAAppUi* iAppUi;

        // Handle to view switcher
        MCAViewSwitcher& iViewSwitcher;

        // Owns. Used temporarily in CreateNewChatL.
        HBufC* iGroupId;
        HBufC* iJoinGroupId;
        // error for asynchronous group joining
        TInt iErr;

        // leave error for asynchronous group joining
        TInt iAsyncLeave;

        // parameters used with asynchronous group joining: group id and group name
        HBufC* iAsyncGroupId;
        HBufC* iAsyncScreenName;

        // Not owned. ref to forwarded message.
        const MCAConversationMessage* iAsyncFwdMessage;

        // Members are required for asynchronous call call backs
        CAsyncCallBack* iAsyncCb;
        CActiveSchedulerWait iWait;

        // simplified group creation FSM stuff
        TSGCState iCurrState;       // current state for FSM
        TSGCState iPrevState;       // previous state for FSM
        TInt iNwError;              // error from network

        // Screen name
        TPtr iScreenNamePtr;

        //Doesnt own. Pointer to group process component.
        MCAGroupPC* iGroupPC;

        //Doesnt Own. Pointer to group utils process component
        MCAGroupUtilsPC* iGroupUtilsPC;
    };

#endif      // CCAGROUPUTILS_H

// End of File
