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
* Description:  Stored group interface (setters)
*
*/



#ifndef MCAEXTENDEDSTOREDGROUP_H
#define MCAEXTENDEDSTOREDGROUP_H

#include <e32std.h>
#include <badesca.h>

#include "MCAStoredGroup.h"
#include "TStorageManagerGlobals.h"

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// CLASS DECLARATION

/**
 *  Interface for stored groups (setters).
 *
 *  @lib CAStorage.dll
 *  @since 2.1
 */
class MCAExtendedStoredGroup : public MCAStoredGroup
    {
    public: // Interface

        /**
         * Set whether this group is joined by the user or not
         * @since 2.1
         * @param aJoinStatus ETrue if the group is joined
         */
        virtual void SetJoined( TBool aJoinStatus ) = 0;

        /**
         * Add members to the group locally. Makes copies of the
         * descriptors.
         * @since 2.1
         * @param aMemberList The list of User Ids of members to add
         */
        virtual void AddMembersLocallyL( const MDesCArray& aMemberList ) = 0;

        /**
         * Remove members from the group locally
         * @since 2.1
         * @param aMemberList The list of User Ids of members to remove
         */
        virtual void RemoveMembersLocally( const MDesCArray& aMemberList ) = 0;

        /**
         * Removes all members from the group locally
         * @since 2.1
         */
        virtual void ResetMembersLocally() = 0;

        /**
         * Set whether this group is the user's own group
         * @since 2.1
         * @param aOwnGroupStatus ETrue if the group is the user's own
         */
        virtual void SetOwnGroup( TBool aOwnGroupStatus ) = 0;

        /**
         * Add a participant to the group. Makes copies of the desciptors.
         * @since 2.1
         * @param aParticipantList The list of participants' User Ids whom to add
         */
        virtual void AddParticipantL( const MDesCArray& aParticipantList ) = 0;

        /**
         * Remove participant from the group
         * @since 2.1
         * @param aParticipantList The list of participants' User Ids whom to
         * remove
         */
        virtual void RemoveParticipantL( const MDesCArray& aParticipantList ) = 0;

        /**
         * Remove all participants from the group
         * @since 2.1
         */
        virtual void ResetParticipantsL() = 0;

        /**
         * Set the storage type
         * @since 2.1
         * @param aStorageType The storage type
         */
        virtual void SetStorageType( TStorageManagerGlobals::TCAStorageType aType ) = 0;

        /**
         * Set the Group Id associated with this group.
         * @since 2.1
         * @param The group id
         */
        virtual void SetGroupIdL( const TDesC& aGroupId ) = 0;

        /**
         * Set the Group Name associated with this group.
         * @since 2.1
         * @param The group name
         */
        virtual void SetGroupNameL( const TDesC& aGroupName ) = 0;

        /**
         * Set the administrator status of the user in this group
         * @since 2.1
         * @param aStatus ETrue if the user is administrator
         */
        virtual void SetAdmin( const TBool aStatus ) = 0;

        /**
         * Set the visibility status of the group
         * @since 2.1
         * @param aVisible ETrue if the group can be seen in UI
         */
        virtual void SetVisible( const TBool aVisible ) = 0;

        /**
         * Set the home server of this group
         * @since 2.1
         * @param aServer The server in which this group resides
         */
        virtual void SetServerL( const TDesC& aServer ) = 0;

        /**
         * Save the changes made to groups
         * @since 2.1
         */
        virtual void SaveChangesL() = 0;

        /**
         * Externalize the group instance data.
         * Used by CCAStorage.
         * @since 2.1
         * @param aStream The stream into which to externalize
         */
        virtual void ExternalizeL( RWriteStream& aStream ) const = 0;

        /**
         * Internalize the group instance data.
         * Used by CCAStorage.
         * @since 2.1
         * @param aStream The stream into which to internalize
         */
        virtual void InternalizeL( RReadStream& aStream ) = 0;

        /**
         * Return the theoretical maximum size needed by the contact if it is
         * externalized.
         * @since 2.1
         * @return The maximal size
         */
        virtual TInt32 MaximalSize() const = 0;

        /**
         * Get version number of the contact implementation. Needed for
         * backward compatibility.
         * @since 2.1
         * @return The version number, where 1 is the first version.
         */
        virtual TInt32 Version() const = 0;
    };

#endif      // MCAEXTENDEDSTOREDGROUP_H

// End of File
