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
* Description:  Manager for presence-list handling
 *               (Contact-list updater/ Attribute-list publisher ).
 *
*/



#ifndef CCAPENGLISTMANAGER_H
#define CCAPENGLISTMANAGER_H
#include "MCAListInitObserver.h"
//  INCLUDES
#include "MCAContactLists.h"
#include "TCARequestQueue.h"

#include <MPEngContactListTransactionObserver2.h>
#include <MPEngAttributeListTransactionObserver2.h>
#include <MPEngPresenceObserver2.h>
#include <PEngWVServices2.h>

#include <badesca.h>

// FORWARD DECLARATIONS
class CPEngAttributeListTransaction2;
class CPEngAttributeListStore2;
class CPEngContactListStore2;
class MPEngContactList2;
class MCAStoredContact;
class MCAStoredContacts;
class MCASettings;
class MPEngAttributeList2;
class CCAPresenceErrors;
class CPEngPresenceNotifier;
class CCAPEngListObserver;
class CPEngNWSessionSlotID2;
class MCAPresenceUpdater;
class MCAContactList;
class MCAListObserver;
// CLASS DECLARATION

/**
 *  Manager for contact/attribute list handling.
 *
 *  @lib CAPresence.dll
 *  @since 2.1
 */
class CCAPEngListManager :	public CBase,
            public MPEngContactListTransactionObserver2,
            public MPEngAttributeListTransactionObserver2,
            public MPEngPresenceObserver2,
            public MCAContactLists
    {


    private:
        enum TNetworkOperation
            {
            EContactListUpdate,
            EAttributeListUpdate
            };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAPEngListManager* NewL( MCASettings& aApplicationSettings,
                                         RArray<TUint32>& aHandledAttributes,
                                         MPEngPresenceObserver2& aNotifierObserver,
                                         CPEngNWSessionSlotID2* aSessionSlotID,
                                         MCAPresenceUpdater& aUpdater );

        /**
         * Destructor.
         */
        virtual ~CCAPEngListManager();

    public: // New functions

        /**
         * Sets login state of system. Should be called before doing
         * other operations, because this setting affects functionality
         * @since 2.1
         * @param aIsLoggedIn Is logged in
         */
        void SetLoggedIn( TBool aIsLoggedIn, CPEngNWSessionSlotID2* aSessionSlotID );

        /**
         * Authorizes presence to all
         * @since 2.1
         */
        void SetAuthorizationToAllL();

        /**
         * Authorizes presence to none
         * @since 2.1
         */
        void SetAuthorizationToNoneL();

        /**
         * Authorizes presence to given users
         * @since 2.1
         * @param aUsers User ids
         */
        void SetAuthorizationToFriendsL();

        /**
         *  Set friend list show status.
         *  @since 2.1
         *  @param aShowFriendList. ETrue if friend list can be shown, EFalse if not.
         */
        void ShowFriendList( TBool aShowFriendList );

        /**
         * Replace attributes in the notifier
         * with the given ones. Also restarts the notifier,
         * if needed.
         * @param aAttributes The attributes to add
         */
        void ReplaceAttributesL( RArray<TUint32>& aAttributes );

        /**
         * Subscribe all lists.
         * @param aSubscribe ETrue subscribe, EFalse unsubscribe
         */
        void SubscribeListsL( TBool aSubscribe );

    public:
        /**
         * Cancel pending requests to network
         */
        void CancelPendingRequests();

    public :
        /**
          * @see MCAContactLists
          */
        void AddObserver( MCAListInitObserver* aObserver ) ;

        /**
          * @see MCAContactLists
          */
        void RemoveObserver() ;

        /**
         * @see MCAContactLists
         */
        TBool IsListInitializationCompleted();

        TBool IsContactFetchingCompleted();

        void AddMainViewProcessObserver( MCAListInitObserver* aObserver );

        void RemoveMainViewProcessObserver();

    protected: // From MCAContactLists

        /**
         * @see MCAContactLists
         */
        TInt FetchContactsL( TBool aBaseSyncDone = EFalse );

        /**
        * @see MCAContactLists
        */
        void FetchServerContactsL(	const TDesC& aContactListId,
                                   CPtrCArray& aWVIds );

        /**
        * @see MCAContactLists
        */
        void UpdateServerContactsL( const TDesC& aContactListId,
                                    const MDesCArray& aWVIds,
                                    const MDesCArray& aNicknames );

        /**
         * Deletes one contact from server side contact list
         * @see MCAContactLists
         */
        void DeleteServerContactL( const TDesC& aContactListId,
                                   const TDesC& aWVId,
                                   const TBool aUpdateToServer = ETrue );

        /**
         * Deletes several contacts from server side contact list
         * @see MCAContactLists
         */
        void DeleteServerContactL( const TDesC& aContactListId,
                                   const MDesCArray& aWVIds,
                                   const TBool aUpdateToServer = ETrue );

        /**
         * Updates deleted contacts to server. This have to be called after
         * DeleteServerContactL is called without updating changes to server
         * (aUpdateToServer). Otherwise the changed data might not be update to
         * server. Use this method to make multiple updates to network with single
         * operation.
         * @see MCAContactLists
         */
        void CommitDeleteServerContactL();

        /**
         * Adds one contact to server side contact list
         * @see MCAContactLists
         */
        MCAStoredContact* AddServerContactL( const TDesC& aContactListId,
                                             const TDesC& aWVId,
                                             const TDesC& aNickname,
                                             TBool aCanRemove = ETrue,
                                             const TDesC* aOldWVId = NULL );

        /**
         * Adds several contacts to server side contact list
         * @see MCAContactLists
         */
        void AddServerContactL( const TDesC& aContactListId,
                                const MDesCArray& aWVIds,
                                const MDesCArray& aNicknames );

        /**
         * @see MCAContactLists
         */
        void UpdateContactNicknameL( const TDesC& aContactListId,
                                     const TDesC& aWVId,
                                     const TDesC& aNickname );

        /**
        * @see MCAContactLists
        */
        TContactListsWrapper FetchServerContactListsL( );


        /**
         * @see MCAContactLists
         */
        void ReleaseContactListsL( const TDesC& aExcludedContactList );

        /**
         * @see MCAContactLists
         */
        TInt SetDisplayNameL( const TDesC& aContactListID, const TDesC& aDisplayName );

        /**
         * @see MCAContactLists
         */
        TInt DeleteContactListL( const TDesC& aContactListID
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                                 , TBool aFailedList = EFalse
#endif // IMPS_CONTACT_FETCH_BACKGROUND
                               );

        /**
         * @see MCAContactLists
         */
        TInt CreateContactListL( const TDesC& aDisplayName );

        /**
         * @see MCAContactLists
         */
        const MCAPresenceErrors& LastOperationResult() const;

        /**
         * @see MCAContactLists
         */
        TPtrC IdentificationL( const TDesC& aUserId );

        /**
         * @see MCAContactLists
         */
        TBool IsSyncDone();


    private: // From MPEngPresenceObserver

        /**
         * @see MPEngPresenceObserver
         */
        void HandlePresenceChangeL( CPEngPresenceNotifier2& aNotifier,
                                    CPEngTrackedPresenceIDs2& aChangedPresenceIDs );

        /**
         * @see MPEngPresenceObserver
         */
        void HandlePresenceError( TInt aError,
                                  CPEngPresenceNotifier2& aNotifier );

    private: // From MPEngAttributeListTransactionObserver2

        /*
         * @see MPEngAttributeListTransactionObserver2
         */
        void HandleAttributeListTransactionError(
            TInt aError,
            CPEngAttributeListTransaction2& aTransaction,
            TInt aTransactionOperation );

        /*
         * @see MPEngAttributeListTransactionObserver2
         */
        void HandleAttributeListTransactionCompleteL(
            MPEngTransactionStatus2& aStatus,
            CPEngAttributeListTransaction2& aTransaction,
            TInt aTransactionOperation  );


    private: // From MPEngContactListTransactionObserver2

        /*
         * @see MPEngContactListTransactionObserver2
         */
        void HandleContactListTransactionError(
            TInt aError,
            CPEngContactListTransaction2& aTransaction,
            TInt aTransactionOperation );

        /*
         * @see MPEngContactListTransactionObserver2
         */
        void HandleContactListTransactionCompleteL(
            MPEngTransactionStatus2& aStatus,
            CPEngContactListTransaction2& aTransaction,
            TInt aTransactionOperation  );

    private:

        /**
         * C++ default constructor.
         */
        CCAPEngListManager( RArray<TUint32>& aHandledAttributes,
                            MPEngPresenceObserver2& aNotifierObserver,
                            MCASettings& aApplicationSettings,
                            MCAPresenceUpdater& aUpdater );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( CPEngNWSessionSlotID2* aSessionSlotID );

        MPEngAttributeList2* AttributeListLC( CPEngAttributeListStore2* aListStore );
        TBool IsLoggedIn() const;

        void UpdateToNetworkL( TNetworkOperation aOperation, const TDesC& aListID = KNullDesC, TBool aWait = ETrue );
        void AttachAttributeListL(  MDesCArray& aContactLists, MPEngAttributeList2& aAttributeList );
        void DetachAttributeListL(  MDesCArray& aContactLists );

        /**
         * Restarts attribute notifier.
         * @since 3.0
         * @param aLists Contact list ids to listent to or NULL
         *               to listen to all lists
         */
        void RestartNotifierL( MDesCArray* aListIds = NULL );
        void RemoveFromNotifierL( const TDesC& aId );
        void AddToNotifierL( const TDesC& aId );

        /**
         * Subscribes or unsubscribes list
         * @param aList contact list
         * @param aListId contact list id
         * @param aSubscribe ETrue if the list should be subscribed
         * @param aUpdate, ETrue if we want to update list to server EFalse if not.
         */
        void SubscribeListL( MPEngContactList2& aList, const TDesC& aListId, TBool aSubscribe, TBool aUpdate = ETrue );

        /**
         *  This is called when logout occurs. Releases all member data.
         *  @param TBool aReleaseContactListModels Release also contact list models (ETrue)
         */
        void ReleaseListManager( TBool aReleaseContactListModels );

        /**
         * Add the attributes from iAttributes to
         * the MPEngAttributeList2
         */
        void AddDynamicAttributesToListL( MPEngAttributeList2* aList );

        /**
         * Helper methods to add contacts in multiple "rounds" with CIdle
         * when fetching server contacts
         */
        static TInt AppendContacts( TAny *aInstance );
        TInt DoAppendContacts();

        /**
         * Helper method for next step of fetch contacfs.
         */
        static TInt FetchContacts( TAny *aInstance );

        /**
         * State machine for fetching. Engine of fetching.
         */
        TInt FetchStateMachineL();

        TInt StartFetchIdle();

        /**
         * Params for appending contacts asynchronously.
         */
        struct TAppendContactsParams
            {
            MPEngContactList2* iContactList;
            CDesCArray* iWVIds;
            CDesCArray* iNicknames;
            MCAStoredContacts* iContactStorage;
            TInt iEntryIndex;
            MCAContactList* iCaContactList;
            };

        /**
         * Get the service tree from the current session.
         * @param aSessionSlotID session slot id
         * @return service tree
         * @since 3.1
         */
        TPEngWVCspServicesTree2 ServiceTreeL( CPEngNWSessionSlotID2& aSessionSlotID );

        /**
         * Check if attribute lists are supported in this session.
         * @param aSessionSlotID session slot id
         * @return ETrue if attribute lists are supported
         * @since 3.1
         */
        TBool AttribListsSupportedL( CPEngNWSessionSlotID2& aSessionSlotID );

        /**
         * Update attribute lists to network according to current settings
         * @since 3.1
         */
        void UpdatePresenceAuthorizeL();

    private:    // Data
        // Errorcode storage for async calls
        TInt iOperationError;
        // "Is application logged in"-flag
        TBool iIsLoggedIn;
        // Array of attributes handled
        RArray<TUint32>& iAttributes;
        // Handle to attribute notifier
        MPEngPresenceObserver2& iNotifierObserver;
        // Doesn't own. Handle to settings API
        MCASettings* iApplicationSettings;
        // Owns. PEC attribute list store
        CPEngAttributeListStore2* iPEngAttributeListStore;
        // Owns. PEC contact list store
        CPEngContactListStore2* iPEngContactListStore;
        // Owns. PEC contact list updater
        CPEngContactListTransaction2* iPEngContactListUpdater;
        // Owns. PEC Attribute-list publisher
        CPEngAttributeListTransaction2* iPEngAttributeListPublisher;
        // Owns. Presence notifier from Presence Engine
        CPEngPresenceNotifier2* iPEngAttributeNotifier;
        // Request queue
        TCARequestQueue iRequestQueue;
        // Error data container
        CCAPresenceErrors* iErrors;

        // observer for contact list events
        CCAPEngListObserver* iListObserver;

        TAppendContactsParams iAppendContactsParams;

        // alias support activated or not
        TBool iAliasSupport;

        // Updater for presence
        MCAPresenceUpdater& iPresenceUpdater;

        // Contacts for appending.
        RArray< TAppendContactsParams > iAppendContactsArray;

        // owned: Idle for appending contacts
        CIdle* iIdle;

        // owned: Idle for launching fetchcontact
        CIdle* iFetchIdle;

        // owns. Array of list ids to subscribe
        CDesCArray* iListIds;

        // Steps for contact list synchronization
        TContactFetchOperation iContactFetchStep;

        // Start again if set
        TBool iStartFetchAgain;

        // Attributelist support flag
        TBool iAttribListSupported;

        // DeleteServerContact array of changed lists
        RPointerArray<HBufC> iUpdateListQueue;
        // Is base sync done or not.
        TBool iBaseSyncDone;

        // owned : register by CCASingleListView
        MCAListInitObserver* iObserver;

        // register by CCAMainViewArrayPC
        MCAListInitObserver* iMainViewProcessObserver;
        // indicate that contact fetch is complete
        TBool iContactFetchingComplete;

        // owned : List init is done or not
        TBool iListInitInProcees;

    };

#endif      // CCAPENGLISTMANAGER_H

// End of File
