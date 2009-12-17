/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence handling interface for clients.
*
*/


#ifndef MCAPRESENCE_H
#define MCAPRESENCE_H

// INCLUDES
#include <e32std.h>
#include <badesca.h>
#include "impsbuilddefinitions.h"

// FORWARD DECLARATIONS
class MCAPresenceObserver;
class MCAWatcherObserver;
class MCAContactLists;
class CCAPresenceErrors;
class CPEngNWSessionSlotID2;
class MCAStoredContact;
class MCAReactiveAuthObserver;

struct SServerPrefers;

// CLASS DECLARATION

/**
*  Presence handling interface for UI.
*
*  @lib CAPresence.lib
*  @since 1.2
*/
class MCAPresence
    {
    public:

        enum TSubscribeMode
            {
            ESubscribe,		// Subscribe
            EUnSubscribe	// Unsubscribe
            };

        enum TAuthorizationMode
            {
            EAuthorizedToAll,	  // Everybody can see your presence
            EAuthorizedToFriends, // Only friends can see your presence
            EAuthorizedToNone	  // Nobody can see your presence
            };

        enum TNetworkStatus
            {
            ELoggedIn,		// Client is logged in
            ELoggedOut,		// Client is logged out
            ELoggingOut,    // Client is about to log out
            EUpdateBrand    // Brand status of client is changed
            };

        enum TPresenceStatus
            {
            EUnknown = -1,
            EOffline,
            EOnline,
            EAway,
            EBusy
            };

        enum TDynamicAttributes
            {
            EAliasAttr = 0
            // add more here
            };

    public:

        /**
        * If network state changes, module state should be updated by calling
        * this method.
        * @param aState New state.
        * @param aServerPreferences Server preferences structure
        */
        virtual void HandleNetworkStateL( TNetworkStatus aState,
                                          const SServerPrefers& aServerPreferences,
                                          CPEngNWSessionSlotID2* aSessionSlotID = NULL ) = 0;

        /**
        * If some setting value changes, this must be called for presence
        * module to have correct behaviour
        * @param aState New state.
        */
        virtual void HandleSettingsChangeL( TInt aChangedSettingEnum ) = 0;

        /**
        * Registers observer for presence data changes in friends-list.
        * Unregister can be by passing NULL pointer.
        * @param aObserver Pointer to observer
        */
        virtual void SetPresenceObserver( MCAPresenceObserver* aObserver ) = 0;

        /**
        * Retrieves the observer.
        * @return Pointer to the observer.
        */
        virtual MCAPresenceObserver* PresenceObserver() const = 0;

        /**
        * Registers observer for presence data changes in friends-list.
        * Unregister can be by passing NULL pointer.
        * @param aObserver Pointer to observer.
        */
        virtual void SetWatcherObserver( MCAWatcherObserver* aObserver ) = 0;

        /**
        * Adds a contact, whose presence status is monitored and if
        * it changes, then client receives an event through.
        * MCAWatcherObserver-interface. Given contact must be in friends-list!
        * @param aWatchList Contact, that gets added.
        */
        virtual void AddWatcherL( const TDesC& aWVId ) = 0;

        /**
        * Removes a contact from observed contacts list.
        * Given contact must be in observed list.
        * @param aWatchList Contact, that is to be removed.
        */
        virtual void RemoveWatcherL( const TDesC& aWVId ) = 0;

        /**
        * Refreshes friends-storage presence data.
        */
        virtual void RefreshFriendsL() = 0;

        /**
        * Places all friends (from aUserList) that are online to aOnlineList
        * and users that are offline to aOfflineList.
        * @param aUserList List of users, whose presence data is to be fetched.
        * @param aOnlineList List of users, that are online, can be NULL.
        * @param aOfflineList List of users, that are offline, can be NULL.
        * @param aUpdateStorage If ETrue, contact storage flags are updated
        */
        virtual void GetOnlineUsersL(
            const CDesCArray& aUserList,
            CDesCArray* aOnlineList,
            CDesCArray* aOfflineList,
            TBool aUpdateStorage = EFalse ) = 0;

        /**
        * Fills aOnlineList with the user id's from friends-list that are
        * online.
        * @param aOnlineList List that will be populated with online friends.
        * @param aFetchFromNetwork Fetch presence information from network
        */
        virtual void GetOnlineFriendsL( CDesCArray& aOnlineList,
                                        TBool aFetchFromNetwork = EFalse ) = 0;

        /**
        * Subscribes/Unsubscribes friends-list
        * @param aMode Request type
        * @param aForced Force operation even if application settings state
        * that application is in manual-mode
        */
        virtual void SubscribeFriendsL( const TSubscribeMode aMode ) = 0;

        /**
        * Returns list-handling interface
        * @return List-handling interface
        */
        virtual MCAContactLists* ContactLists() = 0;

        /**
        * Returns a reference to error container, that was filled by the last
        * operation response.
        * @return Reference to error container
        */
        virtual const CCAPresenceErrors& LastOperationResult() const = 0;

        /**
         * Cancel pending requests to network
         * @since 2.5
         */
        virtual void CancelPendingRequests() = 0;

        /**
         * Synchronizes our presence-related settings to the server
         * (subcriptions to the friends, presence authorization).
         * aServerPreferences The resource branded flags for this server,
                              in a struct
         * @since 2.5
         */
        virtual void SynchronizePresenceSettingsL(
            const SServerPrefers& aServerPreferences ) = 0;

        /**
         * Changes client own presence status, and sets status message
         * @param aStatus Presence status
         * @param aStatusMessage Presence status message
         * @return Error code.
         */
        virtual TInt ChangeStatusL( TPresenceStatus aStatus,
                                    const TDesC& aStatusMessage ) = 0;

        /**
         * Changes presence status message
         * @param aStatusMessage Presence status message
         * @return Operation error.
         */
        virtual TInt ChangeStatusMessageL( const TDesC& aStatusMessage ) = 0;

        /**
         * Gets client own current presence status
         * @return current presence status
         */
        virtual TPresenceStatus StatusL()  = 0;
        /**
         * Get alias for given user.
         * Use this only for one-shot rare operations.
         * @param aUserId The user id for which to fetch alias attribute
         * @return The alias, ownership is transferred.
         * Leaves with KErrNotFound if the alias cannot be found,
         * otherwise some other KErr-code (e.g. KErrNoMemory)
         */
        virtual HBufC* AliasL( const TDesC& aUserId ) = 0;

        /**
         * Get alias for given user.
         * Use this only for one-shot rare operations.
         * @param aContact The user for which to fetch alias attribute
         * @return The alias, ownership is transferred.
         * Leaves with KErrNotFound if the alias cannot be found,
         * otherwise some other KErr-code (e.g. KErrNoMemory)
         */
        virtual HBufC* AliasL( MCAStoredContact* aContact ) = 0;

        /**
         * Adds own presence status observer.
         * Leaves with KErrNotReady if not logged in.
         */
        virtual void AddOwnPresenceStatusObserverL(
            MCAPresenceObserver* aObserver )  = 0;

        /**
         * Initializes presence engine interface
         */
        virtual void InitializePresenceAPIL(
            CPEngNWSessionSlotID2& aSessionSlotID )  = 0;

        /**
         * Add given attribute to the list of attributes.
         * After this the new attributesd will be
         * subscribed to.
         * @param aAttribute The attribute to add
         */
        virtual void AddAttributeL( TInt aAttribute ) = 0;

        /**
         * Remove given attribute from the list of attributes
         * After this the new attributesd will be
         * subscribed to.
         * @param aAttribute The attribute to remove
         */
        virtual void RemoveAttributeL( TInt aAttribute ) = 0;

        /**
         * Reset the attributes to defaults.
         * @param aNotifyListManager Notify the list manager about the
         * resetting (ETrue), or not (EFalse)
         */
        virtual void ResetAttributesL( TBool aNotifyListManager = ETrue ) = 0;

        /**
         * Update the alias data to network server, if supported
         */
        virtual void UpdateAliasAttributeL() = 0;

        /**
         * Set the observer for reactive authorization
         */
        virtual void SetReactiveAuthObserver(
            MCAReactiveAuthObserver* aRAObserver ) = 0;

        /**
         * Send reactive authorization response
         * @param aId The id number given through observer
         * @param aResponse The response, ETrue if authorizing,
                            EFalse if denying
         */
        virtual void SendReactiveAuthResponseL( TInt aId, TBool aResponse ) = 0;

        /**
         * @return The number of pending requests
         */
        virtual TInt PendingRARequestCount() = 0;

        /**
         * Get the information from the next reactive authorization request for
         * processing
         * @param aId Will contain the id for the request
         * @param aUserId Will contain the user id
         * @param aLastPos Position of previous found request in queue
         * @return ETrue if there is another request in the queue
         */
        virtual TBool GetNextPendingRARequestL( TInt& aId, TDes& aUserId,
                                                TInt& aLastPos ) = 0;

        /**
         * @return The number of new status messages
         */
        virtual TInt RAStatusCount() = 0;

        /**
         * Get the information from the next reactive authorization status
         * request for processing
         * @param aId Will contain the id for the request
         * @param aStatus ETrue if accepted, EFalse if denied
         * @param aUserId Will contain the user id
         * @param aLastPos Position of previous found request in queue
         * @return ETrue if there is another request in the queue
         */
        virtual TBool GetNextRAStatusL( TInt& aId, TBool& aStatus,
                                        TDes& aUserId, TInt& aLastPos ) = 0;

        /**
         * Called after the login is fully done, this method will notify the
         * observers of any RA messages received during the login phase.
         */
        virtual void PostLoginRAFlushL() = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCAPresence() {};
    };

#endif      // MCAPRESENCE_H

// End of File

