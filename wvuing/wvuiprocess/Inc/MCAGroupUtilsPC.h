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
* Description:  Interface for group operations
*
*/


#ifndef MCAGROUPUTILSPC_H
#define MCAGROUPUTILSPC_H


// INCLUDES

#include "TEnumsPC.h"
#include "badesca.h"
#include "ChatDefinitions.h"



// FORWARD DECLARATIONS

class MCAGroupChatObserverPC;
class MCAGroupEventObserverPC;
class MCAGroupMsgErrorObserverPC;
class CCAChatInfoArray;
class MCAServerContactsArrayPC;
class MCAGroupOperations;
class CImpsCommonGroupProps;
class CImpsPrivateGroupProps;


/**
 *  Group interface process component for UI and Engine.
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAGroupUtilsPC

    {
    public:



        /*
        * Get the created chat room's information on joined users
        */
        virtual TInt GetCreatedChatRoomListL( CDesCArray& aTempArray,
                                              CDesCArray& aGroupIds ) = 0;

        /*
        *
        */
        virtual TInt SwitchToJoinedGroupL( const TDesC& aGroupID,
                                           TCADnlChatViewBuf& aViewDnl ,
                                           const MCAConversationMessage* aFwdMessage,
                                           TBool aInviteQuery ) = 0;


        /*
        *
        */
        virtual void CreateOrJoinGroupL( const TDesC& aGroupId,
                                         TBool& aIsJoined, TBool& aIsFavourite ) = 0;




        /*
        * Deletes the group
        * @param aGroupId: id of the group
        * @param aDeleteFromNetwork: Deletes this group from network and phone if true.
        */
        virtual TInt DeleteGroupL( const TDesC& aGroupId,
                                   TBool aDeleteFromNetwork ) = 0;

        /*
        *
        * @param aGroupId: id of the group
        * @param aDeleteFromNetwork: Deletes this group from network and phone if true.
        */
        virtual TInt DeleteChatGroupFromNetworkL( const TDesC& aGroupId,
                                                  TBool aDeleteFromNetwork ) = 0;

        /*
        *
        * @param aGroupId: id of the group
        * @param aDeleteFromNetwork: Deletes this group from network and phone if true.
        */
        virtual void DeleteFailedGroupL( const TDesC& aGroupId,
                                         TBool aDeleteFromNetwork ) = 0;


        /*
        *
        * @param aGroupId: id of the group
        */
        virtual TPtrC GetEngineDefaultGroupName( const TDesC& aGroupId ) const = 0;


        /*
        *
        * @param aGroupId: id of the group
        */
        virtual TPtrC GetGroupNameForJoinedGroupL( const TDesC& aGroupId ) = 0;

        /**
        * Create a simple group
        * @param aScreenName: screen name of the user creating to the group
        * @param aJoinGroup: Join to the group after creating
        * @param aIsWhisperingEnabled: is provate messages allowed
        * @param aErrorCode: returnt he error code during creating groups
        */
        virtual HBufC* CreateGroupSimpleL( const TDesC& aScreenName,
                                           TBool aJoinGroup,
                                           TBool aIsWhisperingEnabled,
                                           TInt& aErrorCode )  = 0;


        /**
        * To show the screen name query with given group name
        * @param aScreenName: display name
        */
        virtual void ScreenNameQueryL( TDes& aScreenName ) = 0;

        /**
         * Helper function for EditChatGroupPropertiesL and CreateNewChat.
         * Moves own user id to first in list.
         * @param aList Reference to a list where own user id is removed and moved to
         * the first position.
         */
        virtual void MoveOwnIdToFirstL( CDesCArray& aList ) = 0;

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

        virtual void UpdateGroupMembersL( const TDesC& aGroupId,
                                          const CDesCArray& aOldAdmins, const CDesCArray& aAdmins,
                                          const CDesCArray& aOldUserList, const CDesCArray& aUserList,
                                          const CDesCArray& aOldRejected, const CDesCArray& aRejected ) = 0;


        /**
        * Adds members to the group. Leaves on errors
        * @param aGroupId GroupID
        * @param aAdmins Admins
        * @param aUserList Users
        * @param aRejected Rejected users
        */
        virtual void AddGroupMembersL( const TDesC& aGroupId, CDesCArray& aAdmins,
                                       const CDesCArray& aUserList, const CDesCArray& aRejected ) = 0;


        /**
        * Gets the banned users of group
        * @param aRejectList List of rejected users
        * @return CSP error code
        */
        virtual TInt GetRejectListL( CDesCArray& aRejectList , const TDesC& aGroup ) = 0;

        /**
         * Gets the members of group
         * @param aUserList List of users
         * @param aScreenNames List of screen names
         * @param aModers List of moderators
         * @param aAdmins List of administrators
         * @return CSP error code
         */
        virtual TInt GetMembersL( CDesCArray& aUserList,
                                  CDesCArray& aScreenNames,
                                  CDesCArray& aModers,
                                  CDesCArray& aAdmins, const TDesC& aGroup ) = 0;


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
        virtual const TPtrC GetIdentificationL( const TDesC& aContactId ) = 0;

        /**
        * Creates new group to server
        * @param aScreenName screen name
        * @param aJoinGroup Should the group be joined after creation
        * @return CSP error code
        */
        virtual HBufC* CreateGroupL( const TDesC& aScreenName,
                                     const TDesC& aGroupName,
                                     TInt& aError ) = 0;

        /**
        * Join a group.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @param aGroupId group id
        * @param aScreenName screen name for the user, optional (may be zero length)
        * @param aUsers ETrue if a user wants to get list of joined users in a reponse,
        * @param aIsFavourite : if the group is saved as favourite
        * @return operation-id (positive)
        */
        virtual TInt JoinGroupL( const TDesC& aGroupId,
                                 const TDesC& aScreenName,
                                 TBool aUsers, TBool aIsFavourite )  = 0;


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
        virtual TInt GetGroupDataL( const TDesC& aGroupId,
                                    CDesCArray& aUserList, CDesCArray& aScreenNames,
                                    CDesCArray& aModerators, CDesCArray& aAdmins,
                                    CDesCArray& aRejected ) = 0;

        /**
         * Helper function for EditChatGroupPropertiesL.
         * Processes edited group properties.
         * @param aGroupId Reference to group ID.
         * @param aGroupOps Reference to group operations interface.
         * @param aOldAdmins Reference to list of old administrators.
         * @param aNewAdmins Reference to list of new administrators.
         * @param aOldUsers Reference to list of old users.
         * @param aNewUsers Reference to list of new users.
         * @param aOldRejected Reference to list of old rejected users.
         * @param aNewRejected Reference to list of new users.
         * @param aNewCommonProps Reference to new group properties.
         * @param aNewPrivateProps Reference to new group private properties.
         * @return Error code
         */
        virtual TInt ProcessEditedGroupPropertiesL( const TDesC& aGroupId,
                                                    CDesCArray& aOldAdmins,
                                                    CDesCArray& aNewAdmins,
                                                    CDesCArray& aOldUsers,
                                                    CDesCArray& aNewUsers,
                                                    CDesCArray& aOldRejected,
                                                    CDesCArray& aNewRejected ) = 0;

        /*
        * Get the alias of user
        */
        virtual HBufC* GetAliasL() = 0;

        /**
        * Saves the specified group as favourite
        * @param aGroupId: group id of the group to be saved as persistent type
        */
        virtual void SaveAsFavouriteL( const TDesC& aGroupId ) = 0;

        virtual void GetDefaultGroupNameL( TPtr& aChatGroupName,
                                           TEnumsPC::TListSelectionType aSelType )  = 0;

        /*
        * Get the information to be shown in show chat info options in signlelist view
        * and group view.
        * @param aGroupId: Id of group whose information has to be returned
        * @return chatinfor array instance
        */
        virtual CCAChatInfoArray* GetChatInfoArrayL(
            const TDesC& aGroupId ) = 0;



        /**
        * Set the group name property of the specified group
        * @param aGroupId: group id of the group whose name has to be changed
        */
        virtual void SetGroupNameL( const TDesC& aGroupId ) = 0;

        /**
        * To send invitations from the group view to selected contacts
        * @param aUserIDs: list of users to whom the invites have to be sent
        * @param aGroupId: id of the group from which the invite is being sent
        * @param aMessage: the message that has to be sent to the invitees
        * @return int: error code
        */
        virtual TInt SendInvitationsL( const CDesCArray& aUserIDs,
                                       const TDesC& aGroupID, const TDesC& aMessage ) = 0;


        /**
         * Find a group from the store
         * @param aId the ID of the group to find
         * @return The group
         */
        virtual TBool FindGroup( const TDesC& aId ) = 0;

        /**
         * Get the User Ids of participants of the group (joined users)
         * Makes copies of the descriptors.
         * @param aParticipantList
         */
        virtual void GetParticipantsL( CDesCArray& aParticipantList,
                                       const TDesC& aGroupId ) const = 0;

        /**
         * Check if the user has joined this group
         * @param aGroupId the groupid for which to know if user has joined
         * @return ETrue if user is joined to the group with group id aGroupId
         */
        virtual TBool IsJoined( const TDesC& aGroupId ) const = 0;

        /**
         * See if this is the user's group
         * @param aGroupId the groupid for which to know if user is owner
         * @return ETrue if this group is the user's group
         */
        virtual TBool IsOwnGroup( const TDesC& aGroupId ) const = 0;

        /**
         * Check if the user is administrator in this group
         * @param aGroupId the groupid for which to know if user is administrator
         * @return ETrue if the user is administrator
         */
        virtual TBool IsAdmin( const TDesC& aGroupId ) const = 0;

        /**
         * Leave the given group. Use this method instead of
         * calling the wrapper directly (to avoid race
         * conditions). This method also does cleanup and
         * removes the group wrapper, if necessary.
         * Wraps the LeaveL in MCAGroupOperations.
         * @param aGroupId The group to leave
         * @return IMPS error code from LeaveL
         */
        virtual TInt LeaveGroupL( const TDesC& aGroupId ) = 0;

        /**
         * Checks if chat group is favourite or not
         * @param aGroupId The group for which to know if user has saved has favorite
         * @return ETrue if favourite EFalse if not
         */
        virtual TBool IsFavouriteChatGroup(
            const TDesC& aGroupId ) const = 0;

    public:

        /**
         * Destructor.
         */
        virtual ~MCAGroupUtilsPC()
            {

            };


    };

#endif      // MCAGROUPUTILSPC_H

// End of File
