/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MCAGROUPMANAGERINTERFACE_H
#define MCAGROUPMANAGERINTERFACE_H

// INCLUDES
#include <badesca.h>

// FORWARD DECLARATIONS
class MCAGroupOperations;
class MCAGroupEventObserver;
class CImpsCommonGroupProps;
class CImpsPrivateGroupProps;

// CLASS DECLARATION

/**
 *  Group manager interface to the UI.
 *
 *  @lib CAEngine.lib
 *  @since 2.1
 */
class MCAGroupManagerInterface
    {
    public:

        /**
         * Returns the number of created chats and increments the value by one.
         * @return Is the current amount of created chats in this session.
         */
        virtual TInt NumberOfCreatedGroupsDuringSession() const = 0;

        /**
         * Populates given list with groups that logged user has created.
         * @param aGroupList Group-list to be populated
         * @return The errorcode
         */
        virtual TInt PopulateCreatedGroupsL( CDesCArray& aGroupList ) const = 0;

    public: // new 2.1 stuff

        /**
         * @return Number of joined groups
         */
        virtual TInt JoinedGroups() = 0;

        /**
         * Get the class reponsible for network group operations.
         * Ownership is not transferred.
         * @param aId The group id
         * @return The group operations class
         */
        virtual MCAGroupOperations* GroupOperationsL( const TDesC& aId ) = 0;

        /**
         * Set an observer for events of individual group (join, leave, etc.)
         * @param aId The group id
         * @param aObs The observer
         */
        virtual void SetGroupEventObserverL( const TDesC& aId, MCAGroupEventObserver* aObserver ) = 0;

        /**
         * Create a group. Creates a group to Storage Manager,
         * and the associated group wrapper.
         * The ownership of the common and private group properties is
         * transferred, so be sure to CleanupStack::Pop those out
         * immediately BEFORE calling this method!
         * @param aProperties initial common group properties.
         * @param aPrivProperties initial private group properties.
         * @param aScreenName specifies the screen name if joining is made
         * with the same call.
         * @param aJoinGroup specifies if we want to join to this group.
         * @param aIsWhisperingEnabled Can user receive private
         * messages (ETrue)
         * @param aErrorCode contains the possible error code after function
         * returns.
         * @return The group ID. If non-NULL, the ownership is transferred to
         * the calling function. If NULL, no group could have been created due
         * to an error condition.
         */
        virtual HBufC* CreateGroupL(
            CImpsCommonGroupProps* aProperties,
            CImpsPrivateGroupProps* aPrivProperties,
            const TDesC& aScreenName,
            TBool aJoinGroup,
            TBool aIsWhisperingEnabled,
            TInt& aErrorCode
        ) = 0;

        /**
         * Join the given group. Use this method instead of calling
         * the wrapper directly. This method makes sure
         * the common and private properties exist. Wraps
         * the JoinL in MCAGroupOperations.
         * @param aGroupId The group to join
         * @param aScreenName The screen name in this group
         * @param aUsers ETrue if list of joined users to be told in response
         * @param aIsFavourite The group is favourite (ETrue)
         * @param aIsWhisperingEnabled User can receive
         *        private messages (ETrue)
         * @return IMPS error code from JoinL
         */
        virtual TInt JoinGroupL( const TDesC& aGroupId,
                                 const TDesC& aScreenName,
                                 TBool aUsers, TBool aIsFavourite,
                                 TBool aIsWhisperingEnabled ) = 0;

        /**
         * Leaves every joined group.
         * @see description in LeaveGroupL.
         */
        virtual void LeaveJoinedGroupsL() = 0;

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
         * Generates groupId
         * @param aResourcePart Resource-part string (group-name)
         * @return GroupId, ownership is transferred to the caller
         */
        virtual HBufC* GenerateGroupIdLC( const TDesC& aResourcePart ) = 0;

        /**
         * Delete the given group
         * @param aGroupId The group id of the group to delete
         * @param aDeleteFromNetwork Defines whether the group should
         *        be deleted also from network server or not
         * @return The errorcode
         */
        virtual TInt DeleteGroupL( const TDesC& aGroupId, TBool aDeleteFromNetwork ) = 0;

        /**
         * Check if the list of users is allowed to access
         * (e.g. via invite) the given group. The users which
         * were not in the list are returned via aNotInList.
         * @param aGroupId The group id
         * @param aCheckUsers The users for whom to check access
         * @param aNotInList The users which were not in the list
         * @return ETrue if the currently logged user is an administrator
         * in this group.
         */
        virtual TBool IsAllowedAccessL( const TDesC& aGroupId,
                                        const CDesCArray& aCheckUsers,
                                        CDesCArray& aNotInList ) = 0;

        /**
         * Check if the given group is open or closed for everyone.
         * @param aGroupId The group id of the group to check
         * @return ETrue if group is open, EFalse if closed
         */
        virtual TBool IsGroupOpenL( const TDesC& aGroupId ) = 0;

        /**
         * Get the group name for the given group id
         * @param aGroupId
         * @return The group name, ownership is transferred.
         */
        virtual HBufC* GroupNameL( const TDesC& aGroupId ) = 0;

        virtual  void CancelGroupOperationL(const TDesC& aGroupId) = 0;
    protected:

        // Deletion through this class is not wanted.
        virtual ~MCAGroupManagerInterface() { };
    };

#endif      // MCAGROUPMANAGERINTERFACE_H

// End of File
