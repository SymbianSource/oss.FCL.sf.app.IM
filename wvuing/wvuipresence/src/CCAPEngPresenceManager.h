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
* Description:  Manager for presence operations (fetcher/notifier/publisher).
*
*/


#ifndef CCAPENGPRESENCEMANAGER_H
#define CCAPENGPRESENCEMANAGER_H

//  INCLUDES
#include "CAPresenceDefinitions.h"
#include "TStorageManagerGlobals.h"
#include "MCAPresence.h"
#include "TCARequestQueue.h"
#include "MCASettings.h"
#include "SServerPrefers.h"
#include "TDecodeAttrParams.h"
#include "MCAPresenceUpdater.h"

#include <MPEngPresenceObserver2.h>
#include <MPEngAttributeTransactionObserver2.h>
#include <MPEngReactAuthObserver.h>
#include <MPEngReactAuthTransactionObserver.h>

// FORWARD DECLARATIONS
class CPEngAttributeListTransaction;
class CPEngAttributeStore2;
class CPEngPresenceNotifier2;
class CPEngAttributeTransaction;
class MPEngPresenceAttrModel;
class MCASettings;
class CCAPEngListManager;
class CCAPresenceErrors;
class MCAStoredContacts;
class CPEngTrackedPresenceIDs2;
class MPEngPresenceAttrModel2;

class MCAReactiveAuthObserver;
class MPEngAuthorizationRequest;
class MPEngAuthorizationStatus;
class CPEngReactAuthStore;
class CPEngReactAuthNotifier;
class CPEngReactAuthTransaction;
class CCAState;

// CLASS DECLARATION

/**
 *  Manager for presence-list operations
 *
 *  @lib CAPresence.dll
 *  @since 2.1
 */
class CCAPEngPresenceManager :  public CBase,
            public MCAPresence,
            public MPEngPresenceObserver2,
            public MPEngAttributeTransactionObserver2,
            public MPEngReactAuthObserver,
            public MPEngReactAuthTransactionObserver,
            public MCAPresenceUpdater
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAPEngPresenceManager* NewL(
            MCASettings* aApplicationSettings );

        /**
         * Destructor.
         */
        virtual ~CCAPEngPresenceManager();

    public: // New public methods

        /**
         * Returns settings API handle
         * @since 2.1
         * @return Settings API handle
         */
        MCASettings* SettingsAPI() const;

        /**
         * Sets setting API handle
         * @since 2.1
         * @param aApplicationSettings Settings API handle
         */
        void SetSettingsAPIL( MCASettings* aApplicationSettings );

    public: // From MCAPresence

        /**
         * @see MCAPresence
         */
        void HandleNetworkStateL( TNetworkStatus aState,
                                  const SServerPrefers& aServerPreferences,
                                  CPEngNWSessionSlotID2* aSessionSlotID );

        /**
         * @see MCAPresence
         */
        void HandleSettingsChangeL( TInt aChangedSettingEnum );

        /**
         * @see MCAPresence
         */
        void SetPresenceObserver( MCAPresenceObserver* aObserver );

        /**
         * @see MCAPresence
         */
        MCAPresenceObserver* PresenceObserver() const;

        /**
         * @see MCAPresence
         */
        void SetWatcherObserver( MCAWatcherObserver* aObserver );

        /**
         * @see MCAPresence
         */
        void AddWatcherL( const TDesC& aWVId );

        /**
         * @see MCAPresence
         */
        void RemoveWatcherL( const TDesC& aWVId );

        /**
         * @see MCAPresence
         */
        void RefreshFriendsL();

        /**
         * @see MCAPresence
         */
        void GetOnlineUsersL(   const CDesCArray& aUserList,
                                CDesCArray* aOnlineList,
                                CDesCArray* aOfflineList,
                                TBool aUpdateStorage = EFalse );

        /**
         * @see MCAPresence
         */
        void GetOnlineFriendsL( CDesCArray& aOnlineList,
                                TBool aFetchFromNetwork = EFalse );

        /**
         * @see MCAPresence
         */
        void SubscribeFriendsL( const TSubscribeMode aMode );

        /**
         * @see MCAPresence
         */
        MCAContactLists* ContactLists();

        /**
         * @see MCAPresence
         */
        const CCAPresenceErrors& LastOperationResult() const;

        /**
         * Cancel pending requests to network
         */
        void CancelPendingRequests();

        /**
         * @see MCAPresence
         */
        void SynchronizePresenceSettingsL(
            const SServerPrefers& aServerPreferences );

        /**
         * @see MCAPresence
         */
        HBufC* AliasL( const TDesC& aUserId );

        /**
         * @see MCAPresence
         */
        HBufC* AliasL( MCAStoredContact* aContact );

        /**
         * @see MCAPresence
         */
        void ChangeStatusL( TPresenceStatus aStatus );

        /**
         * @see MCAPresence
         */
        TInt ChangeStatusL( TPresenceStatus aStatus,
                            const TDesC& aStatusMessage );

        /**
         * @see MCAPresence
         */
        TInt ChangeStatusMessageL( const TDesC& aStatusMessage );

        /**
         * @see MCAPresence
         */
        MCAPresence::TPresenceStatus StatusL();

        /**
         * @see MCAPresence
         */
        void AddOwnPresenceStatusObserverL( MCAPresenceObserver* aObserver );

        /**
         * @see MCAPresence
         */
        void InitializePresenceAPIL( CPEngNWSessionSlotID2& aSessionSlotID );

        /**
         * @see MCAPresence
         */
        void AddAttributeL( TInt aAttribute );

        /**
         * @see MCAPresence
         */
        void RemoveAttributeL( TInt aAttribute );

        /**
         * @see MCAPresence
         */
        void ResetAttributesL( TBool aNotifyListManager /* = ETrue */ );

        /**
         * @see MCAPresence
         */
        void UpdateAliasAttributeL();

        /**
         * @see MCAPresence
         */
        void SetReactiveAuthObserver( MCAReactiveAuthObserver* aRAObserver );

        /**
         * @see MCAPresence
         */
        void SendReactiveAuthResponseL( TInt aId, TBool aResponse );

        /**
         * @see MCAPresence
         */
        TInt PendingRARequestCount();

        /**
         * @see MCAPresence
         */
        TBool GetNextPendingRARequestL( TInt& aId,
                                        TDes& aUserId,
                                        TInt& aLastPos );

        /**
         * @see MCAPresence
         */
        TInt RAStatusCount();

        /**
         * @see MCAPresence
         */
        TBool GetNextRAStatusL( TInt& aId, TBool& aStatus,
                                TDes& aUserId, TInt& aLastPos );

        /**
         * @see MCAPresence
         */
        void PostLoginRAFlushL();

    private: // From MPEngAttributeTransactionObserver2

        /**
         * @see MPEngAttributeTransactionObserver2
         */
        void HandleAttributeTransactionError(
            TInt aError,
            CPEngAttributeTransaction2& aTransaction,
            TInt aTransactionOperation );

        /**
         * @see MPEngAttributeTransactionObserver2
         */
        void HandleAttributeTransactionCompleteL(
            MPEngTransactionStatus2& aStatus,
            CPEngAttributeTransaction2& aTransaction,
            TInt aTransactionOperation );


    private: // From MPEngPresenceObserver2

        /**
         * @see MPEngPresenceObserver2
         */
        void HandlePresenceChangeL( CPEngPresenceNotifier2& aNotifier,
                                    CPEngTrackedPresenceIDs2& aChangedPresenceIDs );

        /**
         * @see MPEngPresenceObserver2
         */
        void HandlePresenceError( TInt aError,
                                  CPEngPresenceNotifier2& aNotifier );

    public: // from MPEngReactAuthObserver

        // @see MPEngReactAuthObserver
        void HandlePendingAuthorizationReqL(
            CPEngReactAuthNotifier& aNotifier,
            TArray< MPEngAuthorizationRequest* >& aPendingAuthReqs );

        // @see MPEngReactAuthObserver
        void HandlerAuthorizationStatusL(
            CPEngReactAuthNotifier& aNotifier,
            TArray< const MPEngAuthorizationStatus* > aAuthStatuses );

        // @see MPEngReactAuthObserver
        void HandleReactAuthError(
            TInt aError,
            CPEngReactAuthNotifier& aNotifier );

    private: // from MPEngReactAuthTransactionObserver

        void HandleReactAuthTransactionCompleteL(
            MPEngTransactionStatus2& aStatus,
            CPEngReactAuthTransaction& aTransaction,
            TInt aTransactionOperation );

        void HandleReactAuthTransactionError(
            TInt aError,
            CPEngReactAuthTransaction& aTransaction,
            TInt aTransactionOperation );

    private: // From MCAPresenceUpdater

        /**
         * @see MCAPresenceUpdater
         */
        void UpdatePresenceL( const TDesC& aUserId );

        /**
         * @see MCAPresenceUpdater
         */
        void ExtractAttributeModelsForUserL(
            const TDesC& aUserId,
            RPointerArray<MPEngPresenceAttrModel2>& aAttrModels );

        const RPointerArray<CCAState>& PresenceStates();

        void SetAttributeProcessing( TBool aEnabled );

    private:

        /**
         * C++ default constructor.
         */
        CCAPEngPresenceManager();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( MCASettings* aApplicationSettings );

        // Prohibit copy constructor if not deriving from CBase.
        // CCAPEngPresenceManager( const CCAPEngPresenceManager& )
        // Prohibit assigment operator if not deriving from CBase.
        // CCAPEngPresenceManager& operator=( const CCAPEngPresenceManager& )
        void DecodeFetcherDataL(
            const MDesCArray& aUserList,
            CDesCArray* aStateOnline = NULL,
            CDesCArray* aStateOffline = NULL,
            CDesCArray* aStateUnknown = NULL,
            TBool aUpdateStorage = EFalse );

        static TInt DecodeAttrModels( TAny *aInstance );
        TInt DoDecodeAttrModels();

        void DecodeNotifierDataL(
            CPEngTrackedPresenceIDs2* aChangedPresenceIDs,
            const MDesCArray* aUserList );

        TBool IsLoggedIn() const;

        /**
         * Populates the attribute models from PEC Engine cache
         * of fetched/subscribed attributes
         * @param aArray Will contain the populated attribute models
         * @param aUserList The list of users to parse, ignored
         *                  in case of notifier
         * @param aChangedPresenceIDs The IDs which were changed
         *                            (NULL if fetcher)
         */
        void PopulateAttrModelsL(
            RPointerArray<MPEngPresenceAttrModel2>& aArray,
            const MDesCArray* aUserList = NULL,
            CPEngTrackedPresenceIDs2* aChangedPresenceIDs = NULL );

        // See MCASettings.h for authorization modes
        void SetPresenceAuthorizationL( TInt aAppSettingsAuthValue );

        void FetchAttributesL(  const MDesCArray& aUsers,
                                CDesCArray *aStateOnline,
                                CDesCArray *aStateOffline,
                                CDesCArray *aStateUnknown,
                                TBool aUpdateStorage );

        /**
         * Initializes PEng API related handlers
         * @param aSessionSlotID a network session slot id
         */
        void InitializePEngAPIL();


        /**
         * Initializes a pending own presence observer
         */
        void InitializeOwnPresenceObserverL();

        /**
         * Notifies own presence observer,
         */
        void NotifyOwnPresenceObserverL();

        /**
         * Map internal dynamic attributes to PEC attributes,
         * @param aAttribute The attribute
         */
        TUint32 InternalToPECAttrL( TDynamicAttributes aAttribute );

        /**
         * Helper method to extract the attribute model for the given attribute
         * out of the current attribute store.
         * @params aId The WVID
         * @params aArray Where to put the attribute (appended)
         * @params aAttribute The attribute to parse
         * Will leave with errors.
         */
        void ExtractAttributeModelL(
            const TDesC& aId,
            RPointerArray<MPEngPresenceAttrModel2>& aArray,
            TUint32 aAttribute );

        /**
         * Extracts all supported attribute models for the users
         * given in the list. Attribute models are appended to
         * the given array.
         * @param aUserList The list of users
         * @param aAttrModels The attribute models to which to append
         *                    the extracted models
         */
        void ExtractAttributeModelsForUsersL(
            const MDesCArray& aUserList,
            RPointerArray<MPEngPresenceAttrModel2>& aAttrModels );

    private:    // Data
        // Owns. Internal list-manager
        CCAPEngListManager* iListManager;
        // Doesn't own. Presence module observer
        MCAPresenceObserver* iPresenceObserver;
        // Owns. Presence fetcher from Presence Engine
        CPEngAttributeTransaction2* iPEngAttributeTransaction;

        // Owns. Presence attribute store from Presence Engine
        CPEngAttributeStore2* iPEngAttributeStore;
        // Doesn't own.
        MCASettings* iApplicationSettings;
        // Doesn't own.
        MCAWatcherObserver* iWatcherObserver;
        // Owns. Queue for incoming requests
        TCARequestQueue iRequestQueue;
        // Error data container
        CCAPresenceErrors* iErrors;

        RArray<TUint32> iAttributes;
        TInt iOperationError;
        TInt iAuthMode;
        TInt iNetworkState;

        TBool iPEngAPIInitialized;

        // Doesn't own. Own presence status observer
        MCAPresenceObserver* iOwnStatusObserver;

        // Owns. presence notifier
        CPEngPresenceNotifier2* iOwnPresenceNotifier;

        // Owns. Network session slot ID
        CPEngNWSessionSlotID2* iSessionSlotID;

        // a flag that tells if there is an observer
        // which needs initialization
        TBool iObserverQueued;

        // cached app auth settings value
        TInt iCachedAppSettingsAuthValue;

        // a flag which tells if there is a cached
        // status value (both status and status text)
        TBool iCachedStatus;
        // cached presence status
        TPresenceStatus iCachedPresenceStatus;
        // cached status text
        HBufC* iCachedStatusText;

        // ----- reactive authorization -----

        // ETrue if reactive authorization in use
        TBool iRAUsed;

        // observer for reactive authorization, not owned
        MCAReactiveAuthObserver* iRAObserver;

        // reactive authorization store, owned
        CPEngReactAuthStore* iRAStore;

        // reactive authorization notifer, owned
        CPEngReactAuthNotifier* iRANotifier;

        // reactive authorization transaction, owned
        CPEngReactAuthTransaction* iRATransaction;

        // alias support enabled (ETrue) or not (EFalse)
        TBool iAliasUsed;

        // Server preferences structure given by the IM Engine
        // used for branding purposes
        SServerPrefers iServerPrefers;

        // owned
        RPointerArray<CCAState> iOwnStates;

        // owned
        RPointerArray<CCAState> iPresenceStates;

        // owned. Model arrays are used for attribute decoding.
        RArray< TDecodeAttrParams > iAttrArrays;

        // owned: Idle for attribute decoding.
        CIdle* iIdle;

        // should we process attributes or not
        TBool iAttributeProcessing;

        // Objects fetched with FetchAttributesToObjects() are placed here
        RPointerArray<MPEngPresenceAttrModel2> iFetchObjects;
    };

#endif      // CCAPENGPRESENCEMANAGER_H

// End of File

