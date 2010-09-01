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
* Description:  Stored group data interface
*
*/


#ifndef MCASTOREDGROUP_H
#define MCASTOREDGROUP_H

#include <e32std.h>
#include <badesca.h>

#include "TStorageManagerGlobals.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  Interface for stored groups.
 *
 *  @lib CAStorage.dll
 *  @since 2.1
 */
class MCAStoredGroup
    {
    public: // Interface

        /**
         * Check if the user has joined this group
         * @since 2.1
         * @return ETrue if the group has been joined
         */
        virtual TBool IsJoined() const = 0;

        /**
         * Get the User Ids of all members of the group. Result set is called
         * "members" and is the union of participants, administrators and
         * moderators. Makes copies of the descriptors.
         * @since 2.1
         * @param aMemberList The list of members
         */
        virtual void GetMembersL( CDesCArray& aMemberList ) const = 0;

        /**
         * Get the User Ids of participants of the group (joined users)
         * Makes copies of the descriptors.
         * @since 2.1
         * @param aParticipantList
         */
        virtual void GetParticipantsL( CDesCArray& aParticipantList ) const = 0;

        /**
         * Get the Group Id associated with this group.
         * @since 2.1
         * @return The group id, or KNullDesC if not set.
         */
        virtual const TDesC& GroupId() const = 0;

        /**
         * Get the Group Name associated with this group.
         * @since 2.1
         * @return The group name, or KNullDesC if not set.
         */
        virtual const TDesC& GroupName() const = 0;

        /**
         * Get the storage type of this group
         * @since 2.1
         * @return The storage type of this group
         */
        virtual TStorageManagerGlobals::TCAStorageType StorageType() const = 0;

        /**
         * See if this is the user's group
         * @since 2.1
         * @return ETrue if this group is the user's group
         */
        virtual TBool IsOwnGroup() const = 0;

        /**
         * Check if the user is administrator in this group
         * @return ETrue if the user is administrator
         * @since 2.1
         */
        virtual TBool IsAdmin() const = 0;

        /**
         * Get number of participants
         * @return The number of participants (joined normal users)
         * @since 2.1
         */
        virtual TInt NumParticipants() const = 0;

        /**
         * Check if the group is visible in UI or not
         * @return ETrue if the group is visible
         * @since 2.1
         */
        virtual TBool IsVisible() const = 0;

        /**
         * Return the server of this group
         * @return The server
         * @since 2.1
         */
        virtual const TDesC& Server() const = 0;
    };

#endif      // MCASTOREDGROUP_H

// End of File
