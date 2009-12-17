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
* Description:  Group data container implementation
*
*/



#ifndef CCAGROUP_H
#define CCAGROUP_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "MCAExtendedStoredGroup.h"
#include "TStorageManagerGlobals.h"

// If new fields are added, increase the version number
// (for backward compatibility with older versions)
const TInt KCAGroupVersion = 1;

// FORWARD DECLARATIONS
class CCAStorage;
class RReadStream;
class RWriteStream;
class MCAStoredGroupsObserver;

// CLASS DECLARATION

/**
 *  Group data implementation
 *
 *  @lib CAStorage.dll
 *  @since 2.1
 */
class CCAGroup : public CBase,
            public MCAExtendedStoredGroup
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aStorage Storage instance
         * @param aObserver group data container observer
         */
        IMPORT_C static CCAGroup* NewL( CCAStorage* aStorage,
                                        MCAStoredGroupsObserver* aObserver );

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCAGroup();

    public: // Functions from base classes

        /**
         * From MCAStoredGroup
         */
        IMPORT_C TBool IsJoined() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SetJoined( TBool aJoinStatus );

        /**
         * From MCAStoredGroup
         */
        IMPORT_C void GetMembersL( CDesCArray& aMemberList ) const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void AddMembersLocallyL( const MDesCArray& aMemberList );

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void RemoveMembersLocally( const MDesCArray& aMemberList );

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void ResetMembersLocally();

        /**
         * From MCAStoredGroup
         */
        IMPORT_C void GetParticipantsL( CDesCArray& aParticipantList ) const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void AddParticipantL( const MDesCArray& aParticipantList );

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void RemoveParticipantL( const MDesCArray& aParticipantList );

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void ResetParticipantsL();

        /**
         * From MCAStoredGroup
         */
        IMPORT_C TStorageManagerGlobals::TCAStorageType StorageType() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SetStorageType(
            TStorageManagerGlobals::TCAStorageType aType );

        /**
         * From MCAStoredGroup
         */
        IMPORT_C TBool IsOwnGroup() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SetOwnGroup( TBool aOwnGroupStatus );

        /**
         * From MCAStoredGroup
         */
        IMPORT_C const TDesC& GroupId() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SetGroupIdL( const TDesC& aGroupId );

        /**
         * From MCAStoredGroup
         */
        IMPORT_C const TDesC& GroupName() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SetGroupNameL( const TDesC& aGroupName );

        /**
         * From MCAStoredGroup
         */
        IMPORT_C TBool IsAdmin() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SetAdmin( const TBool aStatus );

        /**
         * From MCAStoredGroup
         */
        IMPORT_C TInt NumParticipants() const;

        /**
         * From MCAStoredGroup
         */
        IMPORT_C TBool IsVisible() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SetVisible( const TBool aVisible );

        /**
         * From MCAStoredGroup
         */
        IMPORT_C const TDesC& Server() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SetServerL( const TDesC& aServer );

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void SaveChangesL();

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C void InternalizeL( RReadStream& aStream );

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C TInt32 MaximalSize() const;

        /**
         * From MCAExtendedStoredGroup
         */
        IMPORT_C TInt32 Version() const;

    private:

        /**
         * C++ default constructor.
         * @param aStorage Storage instance
         * @param aObserver group data container observer
         */
        CCAGroup( CCAStorage* aStorage, MCAStoredGroupsObserver* aObserver );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:    // Data

        // persistent storage class
        CCAStorage* iStorage; // used

        // joined the group (ETrue)
        TBool iIsJoined;

        // group is own (ETrue)
        TBool iIsOwnGroup;

        // visible in UI (ETrue)
        TBool iIsVisible;

        // administrator status in this group (ETrue)
        TBool iIsAdmin;

        // the group id
        HBufC* iGroupId;    // owns

        // the group name
        HBufC* iGroupName;    // owns

        // the server name for this group
        HBufC* iServerName; // owns

        // locally added members of the group
        CDesCArraySeg* iMembers;   // owns

        // participants of the group
        CDesCArraySeg* iParticipants;  // owns

        // the storage type of the group
        TStorageManagerGlobals::TCAStorageType iStorageType;

        // observer for group events
        MCAStoredGroupsObserver* iObserver; // used

        // flag whether to invoke observer or not upon SaveChangesL,
        // determines whether anything was changed
        TInt iIsChanged;
    };

#endif      // CCAGROUP_H

// End of File
