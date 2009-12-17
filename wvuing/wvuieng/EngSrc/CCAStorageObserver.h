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
* Description:  Observes storage events and reacts according to settings
*
*/


#ifndef CCASTORAGEOBSERVER_H
#define CCASTORAGEOBSERVER_H

// INCLUDES
#include "PublicEngineDefinitions.h"
#include "MCAStoredContactsObserver.h"
#include "MCAContactLists.h"

#include "impsbuilddefinitions.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class MCASettings;
class MCAStoredContacts;
class MCAStoredContact;
class MCAPresence;
class MCABlocking;
class MCAIncomingMessages;

// CLASS DECLARATION

/**
*  Observes storage events and reacts according to settings
*
*
*  @lib CAEngine.lib
*  @since 2.1
*/
class CCAStorageObserver :	public CBase,
            public MCAStoredContactsObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aApplicationSettings Settings manager interface
        * @param aContactStorage Contact storage interface
        * @param aPresenceHandler Presence manager interface
        * @param aBlockingManager Blocking manager interface
        * @param aIncomingMessages Incoming messages interface
        */
        static CCAStorageObserver* NewL(
            MCASettings& aApplicationSettings,
            MCAStoredContacts& aContactStorage,
            MCAPresence& aPresenceHandler,
            MCABlocking& aBlockingManager );

        /**
        * Destructor.
        */
        virtual ~CCAStorageObserver();

    private: // From MCAStoredContactsObserver

        /**
         * Called when a contact is deleted.
         * @since 2.1
         * @param aId The ID of the deleted contact
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        void HandleContactDelete( const TDesC& aId );

        /**
         * Called when a contact is added.
         * @since 2.1
         * @param aId The ID of the added contact
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact );

        /**
         * Called when a contact is changed.
         * @since 2.1
         */
        void HandleChange( MCAContactList* aList, MCAStoredContact* aId,
                           TStorageManagerGlobals::TCAObserverEventType aEventType,
                           TBool aUserIdChanged );

        /**
         * Called when backup/restore starts/stops.
         * There is no distinction between backup/restore (Symbian limitation).
         * However, the called party should fetch new list(s) as they
         * may have changed if the operation was restore operation.
         * @param aEventType The type of backup/restore event
         * @since 2.1
         */
        void HandleBackupRestoreEvent( const TStorageManagerGlobals::
                                       TCAObserverEventType aEventType );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        * @see CCAStorageObserver::NewL for parameters
        */
        CCAStorageObserver( MCASettings& aApplicationSettings,
                            MCAStoredContacts& aContactStorage,
                            MCAPresence& aPresenceHandler,
                            MCABlocking& aBlockingManager );

        /**
         * Handles deletion of contact
         * @param aUserId User id
         */
        void IMHandleDelete( const TDesC& aUserId );

        /**
         * Handles addition of contact
         * @param aContact contact data
         */
        void IMHandleAddition( MCAStoredContact& aContact );

        /**
         * Calls blocking manager's delete contact
         * @param aUserId User id
         */
        void DoBlockingManagerDeleteL( const TDesC& aUserId );

        /**
         * Calls blocking manager's add contact
         * @param aUserId User Id
         */
        void DoBlockingManagerAdditionL( const TDesC& aUserId );

        /**
         * Checks contact's blocking status and pending messages
         * @param aContact contact data
         */
        void DoUpdateContactStatus( MCAStoredContact& aContact );
        /**
         * Handles the possible errors
         * @param aError error code
         */
        void HandleError( TInt aError );

    private:    // Data
        // Doesn't own.
        MCASettings& iApplicationSettings;
        // Doesn't own.
        MCAStoredContacts& iContactStorage;
        // Doesn't own.
        MCAPresence& iPresenceHandler;
        // Doesn't own.
        MCABlocking& iBlockingManager;

        // Owns
        HBufC* iLastUserId;
    };

#endif      // CCASTORAGEOBSERVER_H

// End of File
