/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Stored contact data interface (getters)
*
*/



#ifndef MCASTOREDCONTACT_H
#define MCASTOREDCONTACT_H

#include "impsbuilddefinitions.h"

#include <e32std.h>
#include <cntdef.h>

#include "TStorageManagerGlobals.h"

// CLASS DECLARATION

/**
 *  Getter interface for contacts.
 *
 *  @lib CAStorage.dll
 *  @since 3.0
 */
class MCAStoredContact
    {
    public: // Definitions

        enum TSortAlgorithm
            {
            ECompareAlphabetically,
            ECompareByPresence
            };

    public: // Interface

        /**
         * User id
         * @return User ID of this contact
         */
        virtual const TDesC& UserId() const = 0;

        virtual const TDesC& Nickname() const = 0;

        virtual const TDesC& Alias() const = 0;

        /**
         * Returns the text used to identify this contact in the UI.
         * If there is no identification, returns the WV User Id.
         * Make a copy of the returned string for usage, as it can be destroyed
         * at any point following a call to SetIdentification.
         * @return String based on rules presented above.
         */
        virtual const TDesC& Identification() const = 0;

        /**
         * Online status
         * @return On-line status of this contact
         */
        virtual TStorageManagerGlobals::TPresenceStatus OnlineStatus() const = 0;

        /**
         * Check if this contact is blocked.
         * @return ETrue if the contact is blocked
         */
        virtual TBool IsBlocked() const = 0;

        /**
         * Check if this contact is watched
         * @return ETrue if the contact is watched
         */
        virtual TBool IsWatched() const = 0;

        /**
         * Client type
         * @return Client type of this contact
         */
        virtual TStorageManagerGlobals::TClientType ClientType() const = 0;

        /**
         * Set Client type
         */
        virtual void SetClientType( TStorageManagerGlobals::TClientType aType ) = 0;

        /**
         * Set online status
         */
        virtual void SetOnlineStatus( TStorageManagerGlobals::TPresenceStatus aOnlineStatus ) = 0;

        /**
         * Set block status
         */
        virtual void SetBlocked( TBool aBlocked ) = 0;

        /**
         * Set watch status
         */
        virtual void SetWatched( TBool aWatched ) = 0;

        /**
         * Set amount of pending messages
         * @param aAmount Amount of pending messages
         */
        virtual void SetPendingMessages( TInt aAmount ) = 0;

        /**
         * Inform observers about changes
         */
        virtual void SignalChanges() = 0;

        virtual void SetUserIdL( const TDesC& aUserId ) = 0;

        virtual void SetAliasL( const TDesC& aAlias ) = 0;

        virtual void SetNicknameL( const TDesC& aNickname ) = 0;

        virtual TBool Selected() const = 0;

        virtual void SetSelected( TBool aSelected ) = 0;

        virtual TInt PendingMessages() const = 0;

        virtual const TDesC& StatusText() const = 0;

        virtual void SetStatusTextL( const TDesC& aStatusText ) = 0;

        /**
         * Destruction.
         */
        virtual ~MCAStoredContact() {};
    };

#endif      // MCASTOREDCONTACT_H

// End of File
