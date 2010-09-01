/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Network operations for one group
*
*/



#ifndef MCAGROUPOPERATIONS_H
#define MCAGROUPOPERATIONS_H

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS
class CImpsCommonGroupProps;
class CImpsPrivateGroupProps;
class MCAStoredGroup;

// CLASS DECLARATION

/**
 *  MCAGroupOperations interface class. Provides network related
 *  operations for one group.
 *
 *  @lib CAEngine.lib
 *  @since 2.1
 */
class MCAGroupOperations
    {

    public: // New functions

        /**
         * Returns the operation id of current operation
         * @return Operation id
         */
        virtual	TInt OperationId() = 0;

        /**
         * Joins the group
         * @param aScreenName screen name
         * @param aUsers If we want the group's user list, set this to ETrue
         * @param aIsFavourite Is this group saved or not
         * @return CSP error code
         */
        virtual TInt JoinL( const TDesC& aScreenName, TBool aUsers,
                            TBool aIsFavourite ) = 0;

        /**
         * Creates new group to server
         * @param aScreenName screen name
         * @param aJoinGroup Should the group be joined after creation
         * @return CSP error code
         */
        virtual TInt CreateGroupL( const TDesC& aScreenName,
                                   TBool aJoinGroup ) = 0;

        /**
         * Leaves the group
         * @return CSP error code
         */
        virtual TInt LeaveL() = 0;

        /**
         * Deletes the group from server
         * @return CSP error code
         */
        virtual TInt DeleteFromNetworkL() = 0;

        /**
         * Updates the group properties to server
         * @param aCommonProps Common group properties
         * @param aPrivateProps Private group properties
         * @return CSP error code
         */
        virtual TInt UpdatePropertiesL( CImpsCommonGroupProps* aCommonProps,
                                        CImpsPrivateGroupProps* aPrivateProps
                                      ) = 0;

        /**
         * Adds members to group
         * @param aUserList List of users
         * @param aScreenNameList List of screen names
         * @return CSP error code
         */
        virtual TInt AddMembersL( const CDesCArray& aUserList,
                                  const CDesCArray& aScreenNameList ) = 0;

        /**
         * Removes members from group
         * @param aUserList List of users
         * @return CSP error code
         */
        virtual TInt RemoveMembersL( const CDesCArray& aUserList ) = 0;

        /**
         * Modifies members status in group
         * @param aAdminList Administrators
         * @param aModerList Moderators
         * @param aOrdinaryList Ordinary group members
         * @return CSP error code
         */
        virtual TInt ModifyMembersL( const CDesCArray* aAdminList,
                                     const CDesCArray* aModerList,
                                     const CDesCArray* aOrdinaryList ) = 0;

        /**
         * Gets the properties of group
         * @param aCommonProps Common group properties
         * @param aPrivateProps Private group properties
         * @return CSP error code
         */
        virtual TInt GetPropertiesL( CImpsCommonGroupProps*& aCommonProps,
                                     CImpsPrivateGroupProps*& aPrivateProps
                                   ) = 0;

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
                                  CDesCArray& aAdmins ) = 0;

        /**
         * Gets the banned users of group
         * @param aRejectList List of rejected users
         * @return CSP error code
         */
        virtual TInt GetRejectListL( CDesCArray& aRejectList ) = 0;

        /**
         * Sets the banned users of group
         * @param aRejectList List of rejected users
         * @param aEnabledList List of users to be removed from rejected list
         * @return CSP error code
         */
        virtual TInt SetRejectListL( const CDesCArray& aRejectedList,
                                     const CDesCArray& aEnabledList ) = 0;

    protected:  // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~MCAGroupOperations() {};

    };

#endif      // MCAGroupOperations_H

// End of File
