/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Abstract interface for group services (RImpsGroupClient)
*
*/



#ifndef MCAIMPSGROUPCLIENT_H
#define MCAIMPSGROUPCLIENT_H

//  INCLUDES
#include "MCAImpsClient.h"
#include <ImpsGroupCli.h>

// CLASS DECLARATION

/**
*  Interface for group services.
*  Same interface as RImpsGroupClient.
*  Comments are copied from RImpsGroupClient.
*
*  @lib CAAdapter.dll
*  @since 1.2
*/
class MCAImpsGroupClient : public MCAImpsClient
    {
    public: // New functions

        // INITILIZATION AND CONNECT

        /**
        * Registers the listener object for Group events.
        * @since 1.2
        * @param aObserver The observer instance.
        * @param aApplicationId application UID or
        *                another unique id for your application.
        * @param aPriority observer priority. Refer to CActive priority.
        */
        virtual void RegisterL(
            MImpsGroupHandler2* aObserver,
            TUid aApplicationId,
            TInt aPriority = 0 ) = 0;

        /**
        * Unregisters the listener object and disconnects from the Symbian OS server.
        * @since 1.2
        */
        virtual void Unregister() = 0;

        // WV PROTOCOL REQUESTS

        /**
        * From MCAImpsGroupClient Creates a group.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @param aProperties initial group properties
        * @param aScreenName ScreenName of the user
        * @param aJoinGroup indicates that the newly created group is joined (or not) at creation time.
        * @param aSubscribeNotification indicates if we sucscribe notification.
        * @return oparation-id (positive)
        */
        virtual TInt CreateGroupL(
            const TDesC& aGroupId,
            const CImpsCommonGroupProps* aProperties,
            const TDesC& aScreenName,
            TBool aJoinGroup,
            TBool aSubscribeNotification = EFalse
        ) = 0;

        /**
        * Delete a group.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt DeleteGroupL(
            const TDesC& aGroupId
        ) = 0;

        /**
        * Join a group.
        * MImpsGroupHandler::HandleJoinL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @param aScreenName screen name for the user, optional (may be zero length)
        * @param aUsers ETrue if a user wants to get list of joined users in a reponse,
        * @param aSubscribeNotification indicates if we sucscribe notification.
        * @return oparation-id (positive)
        */
        virtual TInt JoinGroupL(
            const TDesC& aGroupId,
            const TDesC& aScreenName,
            TBool aUsers,
            TBool aSubscribeNotification = EFalse
        ) = 0;

        /**
        * Leave a group.
        * MImpsGroupHandler::HandleLeaveL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt LeaveGroupL(
            const TDesC& aGroupId
        ) = 0;

        /**
        * Get group members.
        * MImpsGroupHandler::HandleGroupMembersL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt GroupMembersL(
            const TDesC& aGroupId
        ) = 0;

        /**
        * From MCAImpsGroupClient Add group members.
        * The new users' type is ordinary initially.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aUserList list of users to be added (user-ids)
        * @param aScreenNameList Screennames of the users
        * @return oparation-id (positive)
        */
        virtual TInt AddMembersL(
            const TDesC& aGroupId,
            const MDesCArray& aUserList,
            const MDesCArray& aScreenNameList
        ) = 0;

        /**
        * Remove group members.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aUserList list of users to be removed
        * @return oparation-id (positive)
        */
        virtual TInt RemoveMembersL(
            const TDesC& aGroupId,
            const MDesCArray& aUserList
        ) = 0;

        /**
        * Modify members' access rights.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aAdminList list of new administrators user-ids
        * @param aModerList list of new moderators user-ids
        * @param aOrdinaryList list of new ordinary users user-ids
        * @return oparation-id (positive)
        */
        virtual TInt ModifyMembersAccessL(
            const TDesC& aGroupId,
            const MDesCArray* aAdminList,
            const MDesCArray* aModerList,
            const MDesCArray* aOrdinaryList
        ) = 0;

        /**
        * Get group properties.
        * MImpsGroupHandler::HandleGroupPropertiesL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @return oparation-id (positive)
        */
        virtual TInt GroupPropertiesL(
            const TDesC& aGroupId
        ) = 0;

        /**
        * Set group properties.
        * MImpsGroupHandler::HandlecompleteL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aGroupProps common properties
        * @param aOwnProps user's own properties
        * @return oparation-id (positive)
        */
        virtual TInt SetGroupPropertiesL(
            const TDesC& aGroupId,
            const CImpsCommonGroupProps* aGroupProps,
            const CImpsPrivateGroupProps* aOwnProps
        ) = 0;

        /**
        * Update or get list of rejected users.
        * MImpsGroupHandler::HandleRejectListL handles the server response.
        * If you want to get the current reject list only, then
        * give aGroupId parameter only.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aRejectedList users to be added to the reject list (user-ids)
        *        May be NULL.
        * @param aEnabledList users to be removed from the reject list (user-ids)
        *        May be NULL.
        * @return oparation-id (positive)
        */
        virtual TInt SetRejectListL(
            const TDesC& aGroupId,
            const MDesCArray* aRejectedList,
            const MDesCArray* aEnabledList
        ) = 0;

        /**
        * Subscribe group change notice.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * After successfull subscription HandleGroupPropertiesL,
        * HandleNewUsersL and HandleLeftUsersL methods are called
        * whenever remote service sends notifications.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt SubscribeL(
            const TDesC& aGroupId
        ) = 0;

        /**
        * Unsubscribe group change notice.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt UnsubscribeL(
            const TDesC& aGroupId
        ) = 0;

        /**
        * Check the group change subscription status.
        * MImpsGroupHandler::HandleSubscriptionL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt CheckSubscriptionL(
            const TDesC& aGroupId
        ) = 0;

    protected:  // New functions

        /**
        * Destructor.
        */
        virtual ~MCAImpsGroupClient() { };
    };

#endif      // MCAIMPSGROUPCLIENT_H

// End of File
