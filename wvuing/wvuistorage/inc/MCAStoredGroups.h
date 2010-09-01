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
* Description:  Stored group data container interface
*
*/



#ifndef MCASTOREDGROUPS_H
#define MCASTOREDGROUPS_H

#include <e32std.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class MCAStoredGroup;
class MCAExtendedStoredGroup;
class MCAStoredGroupsObserver;

// CLASS DECLARATION

/**
 *  Interface for stored groups container
 *
 *  @lib CAStorage.dll
 *  @since 2.1
 */
class MCAStoredGroups
    {
    public: // New functions

        /**
         * Add an observer.
         * @since 2.1
         * @param aObserver The observer
         */
        virtual void AddObserverL( MCAStoredGroupsObserver*
                                   aObserver ) = 0;

        /**
         * Remove an observer.
         * @since 2.1
         * @param aObserver The observer
         */
        virtual void RemoveObserver( MCAStoredGroupsObserver*
                                     aObserver ) = 0;

        /**
         * Create a new group.
         * Ownership is not transferred to the caller.
         * @since 2.1
         * @return The created contact
         */
        virtual MCAExtendedStoredGroup* CreateGroupL() = 0;

        /**
         * Remove a given group
         * @since 2.1
         * @param aContact The contact to remove
         */
        virtual void DeleteGroupL( MCAStoredGroup* aGroup ) = 0;

        /**
         * Find a group from the store
         * @since 2.1
         * @param aId the ID of the group to find
         * @return The group
         */
        virtual MCAStoredGroup* FindGroup( const TDesC& aId ) = 0;

        /**
         * Populate the given list with groups
         * @since 2.1
         * @param aList The list to populate
         * @param aReturnOnlyVisible Return only groups marked visible (ETrue)
         */
        virtual void PopulateGroupsListL(
            RPointerArray<MCAStoredGroup>& aList,
            TBool aReturnOnlyVisible = ETrue ) = 0;

        /**
         * Signal change in this group. Using this method is responsibility of
         * the client which changes data contained within MCAStoredGroup. The
         * client may also choose to not signal the change event.
         * @since 2.1
         * @param aGroupId The group id
         */
        virtual void SignalGroupChangedL( const TDesC& aGroupId ) = 0;

        /**
         * Signal an added group. Using this method is responsibility of
         * the client which creates the group and sets its name. The
         * client may also choose to not signal the addition event.
         * @since 2.1
         * @param aGroupId The group id
         */
        virtual void SignalGroupAddedL( const TDesC& aGroupId ) = 0;

        /**
         * Save the changes made to the given group. This also takes care of
         * informing the observers. An alternative is to use the
         * SignalGroupChangedL, which only signals about changes, but does not
         * save the group.
         * @since 2.1
         * @param aGroupId The id of the group to save
         */
        virtual void SaveGroupL( const TDesC& aGroupId ) = 0;

        /**
         * Make all own groups hidden by setting their visibility-flag.
         * @param aAllGroups If ETrue, hides other than user's own groups also.
         *        This can be used when the group-operations are not supported
         *        by the server and all storage groups must be hidden.
         */
        virtual void HideAllGroups( TBool aAllGroups = EFalse ) = 0;

        /**
         * Make all groups shown by unsetting their visibility-flag
         */
        virtual void ShowAllGroups() = 0;

        /**
         *  Count of stored groups
         *  @since 2.6
         *  @return Count of groups
         */
        virtual TInt GroupCount( TBool aOnlyVisible ) const = 0;

        /**
         * Check flash memory level. Leaves with KErrDiskFull
         * if memory level below critical level.
         * @since 3.1
         * @param aGroup Group that is going to be written to database.
         */
        virtual void CheckFreespaceLevelL( MCAExtendedStoredGroup* aGroup ) = 0;



        /**
        * Check whether group is deleted before joining or not
        * @since 3.1
        * @return ETrue or EFalse.
        */
        virtual TBool IsGroupDeleted() = 0;

        /**
        * set whether group is deleted before joining or not
        * @since 3.1
        * @param aValue boolean value.
        */
        virtual void SetGroupDeleted( TBool aValue ) = 0 ;


    };

#endif      // MCASTOREDGROUPS_H

// End of File
