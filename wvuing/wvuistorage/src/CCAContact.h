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
* Description:  Contact data container implementation
*
*/



#ifndef CCACONTACT_H
#define CCACONTACT_H

#include "impsbuilddefinitions.h"

//  INCLUDES
#include <e32base.h>

#include "MCAStoredContact.h"

// FORWARD DECLARATIONS
class CCAStorage;
class MCAStoredContactsObserver;
class MCAStorageInfo;

// CLASS DECLARATION

/**
 *  Contact data implementation
 *
 *  @lib CAStorage.dll
 *  @since 2.1
 */
class CCAContact : public CBase,
            public MCAStoredContact
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aStorage Storage instance
         * @param aObserver contact data container observer
         *
         */
        static CCAContact* NewL( MCAStoredContactsObserver* aObserver,
                                 MCAStorageInfo* aVariation );

        /**
         * Destructor.
         */
        ~CCAContact();


        /**
         * Set user id
         */
        void SetUserIdL( const TDesC& aUserId );
        void SetNicknameL( const TDesC& aNickname );
        void SetAliasL( const TDesC& aAlias );

    public: // Functions from base classes

        /**
         * @see MCAContact
         */
        const TDesC& UserId() const;

        /**
         * @see MCAContact
         */
        const TDesC& Nickname() const;

        /**
         * @see MCAContact
         */
        const TDesC& Alias() const;

        /**
         * @see MCAContact
         */
        const TDesC& Identification() const;

        /**
         * @see MCAContact
         */
        TStorageManagerGlobals::TPresenceStatus OnlineStatus() const;

        /**
         * @see MCAContact
         */
        TBool IsBlocked() const;

        /**
         * @see MCAContact
         */
        TBool IsWatched() const;

        /**
         * @see MCAContact
         */
        TStorageManagerGlobals::TClientType ClientType() const;

        /**
         * @see MCAContact
         */
        void SetClientType( TStorageManagerGlobals::TClientType aType );

        /**
         * @see MCAContact
         */
        void SetOnlineStatus(
            TStorageManagerGlobals::TPresenceStatus aOnlineStatus );

        /**
         * @see MCAContact
         */
        void SetBlocked( TBool aBlocked );

        /**
         * @see MCAContact
         */
        void SetWatched( TBool aWatched );

        /**
         * @see MCAStoredContact
         */
        void SetPendingMessages( TInt aAmount );

        /**
         * @see MCAContact
         */
        void SignalChanges();

        /**
         * @see MCAContact
         */
        TBool Selected() const;

        /**
         * @see MCAContact
         */
        void SetSelected( TBool aSelected );

        /**
         * @see MCAContact
         */
        TInt PendingMessages() const;

        /**
         * @see MCAContact
         */
        const TDesC& StatusText() const;

        /**
         * @see MCAContact
         */
        void SetStatusTextL( const TDesC& aStatusText );

    public:

        /**
         * From MCAContact
         */
        HBufC* PropertyL(
            TStorageManagerGlobals::TCAStorageProperty aProperty );

        /**
         * From MCAContact
         */
        void SetPropertyL(
            TStorageManagerGlobals::TCAStorageProperty aProperty,
            const TDesC& aData );


    private:

        /**
         * C++ default constructor.
         * @param aStorage Storage instance
         * @param aObserver contact data container observer
         */
        CCAContact( MCAStoredContactsObserver* aObserver,
                    MCAStorageInfo* aVariation );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Update the identification string to be up-to-date
         * according to the logic 1. nickname, 2. alias, 3. userid.
         */
        void UpdateIdentification();

    private:    // Data

        // observer who wants to know about changes etc.
        MCAStoredContactsObserver* iObserver; // used

        // storage variation interface
        MCAStorageInfo* iVariation;

        // has active watcher (ETrue)
        TBool iWatcherActive;

        // is blocked (ETrue)
        TBool iIsBlocked;

        // identification
        TPtrC iIdentification;   // owns

        // user WV ID
        HBufC* iUserId;     // owns

        // presence status (online, offline, etc)
        TStorageManagerGlobals::TPresenceStatus iPresenceStatus;

        // Client type (PC, Mobile, Unknown)
        TStorageManagerGlobals::TClientType iClientType;

        // internal flag for contact deletion management
        TBool iDeletionImminent;

        // Data contained in the contact has changed.
        // We use a binary or (|) trick.
        TInt iIsChanged;

        // ETrue if the user id has been changed
        TBool iUserIdChanged;

        // Nickname. No longer in Pbk but contact has this. Uses old API
        // for maximum compatibility. Owns.
        HBufC* iNickname;

        // Alias. Owns
        HBufC* iAlias;

        /// Status text. Owns.
        HBufC* iStatusText;

        /// Is contact selected or not.
        TBool iSelected;

        /// Amount of pending messages
        TInt iPendingMessages;
    };

#endif      // CCACONTACT_H

// End of File
