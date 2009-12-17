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



#ifndef CCAGROUPUTILSPC_H
#define CCAGROUPUTILSPC_H

//  INCLUDES
#include	"CCAChatInfoArray.h"
#include	"MCAGroupOperations.h"
#include	"MCAGroupUtilsPC.h"
#include    "MCAInviteConflictResolver.h"

#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CCAEngine;
class MCAGroupOperations;
class CImpsCommonGroupProps;
class CImpsPrivateGroupProps;
class CCAServerContactsArray;
class MCAConversationMessage;
class MCAStoredGroups;



// CLASS DECLARATION

/**
*  Process component for Group editing and displaying utilities
*
*  @lib chatng.app
*/
class CCAGroupUtilsPC : public CBase,
            public MCAGroupUtilsPC
    {
    public:  // constructor
        /**
        * Static constructor.
        * @param aEngine Handle to application engine
        * @return Instance of this class
        */
        static CCAGroupUtilsPC* NewLC( CCAEngine& aEngine );

        /**
        * Static constructor.
        * @param aEngine Handle to application engine
        * @return Instance of this class
        */
        static CCAGroupUtilsPC* NewL( CCAEngine& aEngine );

        /**
        * Destructor
        */
        ~CCAGroupUtilsPC();

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


        /*
        *@see MCAGroupUtilsPC
        */
        CCAChatInfoArray* GetChatInfoArrayL( const TDesC& aGroupId );

        /*
        *@see MCAGroupUtilsPC
        */
        TInt GetCreatedChatRoomListL( CDesCArray& aTempArray, CDesCArray& aGroupIds );

        /*
        *@see MCAGroupUtilsPC
        */
        TInt SwitchToJoinedGroupL( const TDesC& aGroupID, TCADnlChatViewBuf& aViewDnl ,
                                   const MCAConversationMessage* aFwdMessage,
                                   TBool aInviteQuery );


        /**
        * To show the screen name query with given group name
        * @param aScreenName: display name
        */
        void ScreenNameQueryL( TDes& aScreenName );

        /**
         * Helper function for EditChatGroupPropertiesL and CreateNewChat.
         * Moves own user id to first in list.
         * @param aList Reference to a list where own user id is removed and moved to
         * the first position.
         */
        void MoveOwnIdToFirstL( CDesCArray& aList );

        /**
        * Updates the group member status. Leaves on errors.
        * This method will calculate delta lists based on previous and new
        * lists
        * @param aGroupId GroupID
        * @param aOldAdmins List of previous admins
        * @param aAdmins List of new admins
        * @param aOldUserList List of previous users
        * @param aUserList List of new users
        * @param aOldRejected List of previous rejected users
        * @param aRejected List of new rejected users.
        */
        void UpdateGroupMembersL( const TDesC& aGroupId,
                                  const CDesCArray& aOldAdmins, const CDesCArray& aAdmins,
                                  const CDesCArray& aOldUserList, const CDesCArray& aUserList,
                                  const CDesCArray& aOldRejected, const CDesCArray& aRejected );

        /**
        * Adds members to the group. Leaves on errors
        * @param aGroupId GroupID
        * @param aAdmins Admins
        * @param aUserList Users
        * @param aRejected Rejected users
        */
        void AddGroupMembersL( const TDesC& aGroupId, CDesCArray& aAdmins,
                               const CDesCArray& aUserList, const CDesCArray& aRejected );

        /**
         * Gets the banned users of group
         * @param aRejectList List of rejected users
         * @param aGroup: group id whose rejected lists ve to be fetched
         * @return CSP error code
         */
        TInt GetRejectListL( CDesCArray& aRejectList , const TDesC& aGroupId  );

        /**
         * Gets the members of group
         * @param aUserList List of users
         * @param aScreenNames List of screen names
         * @param aModers List of moderators
         * @param aAdmins List of administrators
         * @return CSP error code
         */
        TInt GetMembersL( CDesCArray& aUserList,
                          CDesCArray& aScreenNames,
                          CDesCArray& aModers,
                          CDesCArray& aAdmins, const TDesC& aGroup );

        /*
        *@see MCAGroupUtilsPC
        */
        const TPtrC GetIdentificationL( const TDesC& aContactId );


        /**
         * Creates new group to server
         * @param aScreenName screen name
         * @param aJoinGroup Should the group be joined after creation
         * @return CSP error code
         */
        HBufC* CreateGroupL( const TDesC& aScreenName, const TDesC& aGroupName, TInt& aError );

        /**
        * Join a group.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @param aGroupId group id
        * @param aScreenName screen name for the user, optional (may be zero length)
        * @param aUsers ETrue if a user wants to get list of joined users in a reponse,
        * @param aIsFavourite : if the group is saved as favourite
        * @return operation-id (positive)
        */
        TInt JoinGroupL( const TDesC& aGroupId,
                         const TDesC& aScreenName,
                         TBool aUsers, TBool aIsFavourite );

        /**
        * To fetch the data of a group
        * @param aGroupId: id of the group whose data has to be fetched
        * @param aUserList: list of users allowed access to the group
        * @param aScreenNames: screen names of all the users joined to the group
        * @param aModerators: the users given moderator privileges to the group
        * @param aAdmins: the users who have been given admin privileges to the list
        * @param aRejected: list of users banned from entering the group
        * @return errorcode
        */
        TInt GetGroupDataL( const TDesC& aGroupId,
                            CDesCArray& aUserList, CDesCArray& aScreenNames,
                            CDesCArray& aModerators, CDesCArray& aAdmins,
                            CDesCArray& aRejected );

        /*
        *@see MCAGroupUtilsPC
        */
        virtual void GetDefaultGroupNameL( TPtr& aChatGroupName,
                                           TEnumsPC::TListSelectionType aSelType );

        /*
        *@see MCAGroupUtilsPC
        */
        TInt ProcessEditedGroupPropertiesL( const TDesC& aGroupId,
                                            CDesCArray& aOldAdmins,
                                            CDesCArray& aNewAdmins,
                                            CDesCArray& aOldUsers,
                                            CDesCArray& aNewUsers,
                                            CDesCArray& aOldRejected,
                                            CDesCArray& aNewRejected );

        /*
        *@see MCAGroupUtilsPC
        */
        HBufC* GetAliasL();

        /*
        *@see MCAGroupUtilsPC
        */
        void CreateOrJoinGroupL( const TDesC& aGroupId,
                                 TBool& aIsJoined, TBool& aIsFavourite );


        /*
        *@see MCAGroupUtilsPC
        */
        void SaveAsFavouriteL( const TDesC& aGroupId );

        /**
         * Name of the group
         * @return name of the group
         */
        TPtrC GetEngineDefaultGroupName( const TDesC& aGroupId ) const;

        /**
        * Set the group name property of the specified group
        * @param aGroupId: group id of the group whose name has to be changed
        */
        void SetGroupNameL( const TDesC& aGroupId );

        /*
        *@see MCAGroupUtilsPC
        */
        TInt DeleteGroupL( const TDesC& aGroupId, TBool aDeleteFromNetwork );

        /*
        *@see MCAGroupUtilsPC
        */
        TInt DeleteChatGroupFromNetworkL( const TDesC& aGroupId, TBool aDeleteFromNetwork );

        /*
        *@see MCAGroupUtilsPC
        */
        void DeleteFailedGroupL( const TDesC& aGroupId, TBool aDeleteFromNetwork );

        /*
        *@see MCAGroupUtilsPC
        */
        TPtrC GetGroupNameForJoinedGroupL( const TDesC& aGroupId );

        /*
        *@see MCAGroupUtilsPC
        */
        HBufC* CreateGroupSimpleL( const TDesC& aScreenName,
                                   TBool aJoinGroup,
                                   TBool aIsWhisperingEnabled,
                                   TInt& aErrorCode ) ;


        /**
        * To send invitations from the group view to selected contacts
        * @param aUserIDs: list of users to whom the invites have to be sent
        * @param aGroupId: id of the group from which the invite is being sent
        * @param aMessage: the message that has to be sent to the invitees
        * @return int: error code
        */
        TInt SendInvitationsL( const CDesCArray& aUserIDs,
                               const TDesC& aGroupID,
                               const TDesC& aMessage );

    public:

        /**
         * Find a group from the store
         * @param aId the ID of the group to find
         * @return The group
         */
        TBool FindGroup( const TDesC& aId ) ;

        /**
         * Get the User Ids of participants of the group (joined users)
         * Makes copies of the descriptors.
         * @param aParticipantList
         */
        void GetParticipantsL( CDesCArray& aParticipantList,
                               const TDesC& aGroupId ) const ;

        /**
         * Check if the user has joined this group
         * @return ETrue if the group has been joined
         */
        TBool IsJoined( const TDesC& aGroupId ) const ;

        /**
         * See if this is the user's group
         * @return ETrue if this group is the user's group
         */
        TBool IsOwnGroup( const TDesC& aGroupId ) const ;

        /**
         * Check if the user is administrator in this group
         * @return ETrue if the user is administrator
         */
        TBool IsAdmin( const TDesC& aGroupId ) const ;

        /**
         * Leave the given group. Use this method instead of
         * calling the wrapper directly (to avoid race
         * conditions). This method also does cleanup and
         * removes the group wrapper, if necessary.
         * Wraps the LeaveL in MCAGroupOperations.
         * @param aGroupId The group to leave
         * @return IMPS error code from LeaveL
         */
        TInt LeaveGroupL( const TDesC& aGroupId ) ;


        /**
         * Checks if chat group is favourite or not
         * @param aGroupId The id of group for which to know if user has saved has favorite
         * @return ETrue if favourite EFalse if not
         */
        TBool IsFavouriteChatGroup( const TDesC& aGroupId ) const ;

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
        CCAGroupUtilsPC( CCAEngine& aEngine );

    private:    // Data

        // Handle to application engine.
        CCAEngine& iEngine;

        // Owns. Used temporarily in CreateNewChatL.
        HBufC* iGroupId;

        //Pointer to current group. Doesnt own
        MCAStoredGroup* currentGroup;

        //doesn't own,handle to groups interface
        MCAStoredGroups* iStoredGroups;

        //doesnt own. pointer to group manager interface
        MCAGroupManagerInterface* iGroupManagerInterface;
    };

#endif      // CCAGROUPUTILSPC_H

// End of File
