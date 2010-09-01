/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer class for active presence list handling
*               (Contact-list updater/ Attribute-list publisher ).
*
*/


#ifndef CCAPENGLISTOBSERVER_H
#define CCAPENGLISTOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <MPEngContactListObserver2.h>

#include <badesca.h>

// FORWARD DECLARATIONS
class CPEngContactListNotifier;
class MCAStoredContacts;
class CPEngContactListStore2;
class CPEngNWSessionSlotID2;
class CPEngPresenceNotifier2;
class MCAPresenceUpdater;

// CLASS DECLARATION

/**
 *  Observer class for contact list events.
 *
 *  @lib CAPresence.dll
 *  @since 2.1
 */
class CCAPEngListObserver :	public CBase,
            public MPEngContactListObserver2
    {

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAPEngListObserver* NewL( CPEngNWSessionSlotID2* aSessionSlotID,
                                          CPEngPresenceNotifier2& aAttributeNotifier,
                                          RArray<TUint32>& aHandledAttributes,
                                          MCAPresenceUpdater& aUpdater );

        /**
         * Destructor.
         */
        virtual ~CCAPEngListObserver();

    public: // New functions

        /**
         * Set the active contact list name
         * for the list observer. All other lists are ignored
         * except the list set as active.
         * @param aListId The contact list id
         * @since 2.1
         */
        void SetActiveListIdL( const TDesC& aListId );

        /**
         * Set the contact list names for the list observer.
         * @param aListIds The contact list ids
         * @since 3.0
         */
        void SetListIds( const MDesCArray& aListIds );

        /**
         * Skips storage removal next time HandleContactListChange gets called
         */
        void SkipStorageRemoval();

        /**
         * Clears old values from list notifier.
         */
        void ClearListNotifier();

        /**
         * Start notifying list.
         * @param aListId, List id for list to be notified.
         */
        void StartNotifier( const TDesC& aListId );

        /**
         * Add list to notifier. Will start notifier if not already started.
         * @param aListId, List id for list to be notified.
         */
        void AddL( const TDesC& aListId );

        /**
         * Remove list from notifier. If last list is removed, the
         * notifier is stopped.
         * @param aListId, List id for list to be notified.
         */
        void Remove( const TDesC& aListId );


    protected: // From MPEngContactListTransactionObserver2

        /**
         * @see MPEngContactListTransactionObserver2
         * @since 2.1
         */
        void HandleContactListChangeL( CPEngContactListNotifier2& aNotifier,
                                       const TDesC& aContactListName );

        /**
         * @see MPEngContactListTransactionObserver2
         * @since 2.1
         */
        void HandleContactListError( TInt aError,
                                     CPEngContactListNotifier2& aNotifier );


    private: // internal functions

        /**
         * C++ default constructor.
         */
        CCAPEngListObserver( CPEngPresenceNotifier2& aAttributeNotifier,
                             RArray<TUint32>& aHandledAttributes,
                             MCAPresenceUpdater& aUpdater );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( CPEngNWSessionSlotID2* aSessionSlotID );

    private:    // Data

        // notifier for contact list events
        CPEngContactListNotifier2* iPEngContactListNotifier; // owns

        // memory cache of contacts
        MCAStoredContacts* iStoredContacts;

        // active contact list name
        HBufC* iActiveListId; /// owns

        // skip removal from storage in HandleContactListChange
        TBool iSkipRemoval;

        // owns. PEC contact list store
        CPEngContactListStore2* iContactListStore;

        // handle to attributenotifier
        CPEngPresenceNotifier2& iAttributeNotifier;

        // Array of attributes handled
        RArray<TUint32>& iAttributes;

        // Presence updater
        MCAPresenceUpdater& iPresenceUpdater;
    };

#endif      // CCAPENGLISTOBSERVER_H 

// End of File
