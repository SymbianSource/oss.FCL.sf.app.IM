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
* Description:  Presence-list handling interface for clients
*
*/


#ifndef MCACONTACTLISTS_H
#define MCACONTACTLISTS_H

// INCLUDES
#include "MCAPresence.h"
#include <e32std.h>
#include <badesca.h>
#include "TCAWrappers.h"
#include "impsbuilddefinitions.h"

// FORWARD DECLARATIONS
class MCAPresenceObserver;
class MCAWatcherObserver;
class MCAPresenceErrors;
class MCAStoredContact;
class MCAListInitObserver;
// CLASS DECLARATION

/**
*  Presence list handling interface for clients
*
*  @lib CAPresence.lib
*  @since 1.2
*/
class MCAContactLists
    {
    public:

        enum TListOperation
            {
            EAdd,		// Add to list
            ERemove,	// Remove from list
            EClearAdd,  // Clear whole list and add
            ERemoveAll	// Removes everything from list
            };


        enum TContactFetchOperation
            {
            EBaseSynchronise = 0,
            EPublishAttributeLists,
            EInitializeLists,
            ERestartSynchronize,
            ESynchronizeList,
            EFetchContacts,
            ESubscribeList,
            EFinalizeList,
            EPublishListsDone,
            ESynchronizeDone
            };

    public:
        /**
         * Fetches all contacts from network server
         * @param aBaseSyncDone, Is base sync already done
         *          ETrue BS is done and there's no need to do it again
         *          EFalse BS is not done, and it has to be done first
         * @return Error code from server
         */
        virtual TInt FetchContactsL( TBool aBaseSyncDone = EFalse ) = 0;

        /**
        * Fetches given contact list contents from network server.
        * NOTE:  Given arrays must have same number of items (they are matched)
        * NOTE2: Engine will reset given arrays before operating with them
        * @param aContactListId Id of list to be fetched
        * @param aWVIds wvids of server-side contacts
        */
        virtual void FetchServerContactsL(	const TDesC& aContactListId,
                                           CPtrCArray& aWVIds ) = 0;

        /**
        * Updates network server's contact list with given list.
        * If given list does not contain some wvid, that is currently on server,
        * system will delete the id from server...
        * NOTE: Given arrays must have same number of items (they are matched)
        * @param aContactListId Id of list to be updated
        * @param aWVIds wvids of server-side contacts
        * @param aNicknames nicknames of server-side contacts
        */
        virtual void UpdateServerContactsL( const TDesC& aContactListId,
                                            const MDesCArray& aWVIds,
                                            const MDesCArray& aNicknames ) = 0;

        /**
         * Deletes one contact from server side contact list
         * @param aContactListId Id of list to be updated
         * @param aWVId wvid of server-side contact
         * @param aUpdateToServer
         *          ETrue update to server directly
         *          EFalse put change into queue (CommitDeleteServerContactL)
         */
        virtual void DeleteServerContactL( const TDesC& aContactListId,
                                           const TDesC& aWVId,
                                           const TBool aUpdateToServer = ETrue ) = 0;

        /**
         * Deletes several contacts from server side contact list
         * @param aContactListId Id of list to be updated
         * @param aWVId wvid of server-side contact
         * @param aUpdateToServer
         *          ETrue update to server directly
         *          EFalse put change into queue (CommitDeleteServerContactL)
         */
        virtual void DeleteServerContactL( const TDesC& aContactListId,
                                           const MDesCArray& aWVIds,
                                           const TBool aUpdateToServer = ETrue ) = 0;

        /**
         * Updates deleted contacts to server. This have to be called after
         * DeleteServerContactL is called without updating changes to server
         * (aUpdateToServer). Otherwise the changed data might not be update to
         * server. Use this method to make multiple updates to network with single
         * operation.
         */
        virtual void CommitDeleteServerContactL() = 0;

        /**
         * Adds one contact to server side contact list
         * @param aContactListId Id of list to be updated
         * @param aWVId wvid of server-side contact
         * @param aNickname nickname of server-side contact
         * @param aDoNotRemove, This is used for editing old one.
         *                      EFalse if editing, ETrue if adding.
         * @param aOldWVId, when editing, remove old userid if different than new one.
         */
        virtual MCAStoredContact* AddServerContactL(
            const TDesC& aContactListId,
            const TDesC& aWVId,
            const TDesC& aNickname,
            TBool aCanREmove = ETrue,
            const TDesC* aOldWVId = NULL  ) = 0;

        /**
         * Adds several contacts to server side contact list
         * @param aContactListId Id of list to be updated
         * @param aWVId wvid of server-side contact
         * @param aNickname nickname of server-side contact
         */
        virtual void AddServerContactL(    const TDesC& aContactListId,
                                           const MDesCArray& aWVIds,
                                           const MDesCArray& aNicknames ) = 0;

        /**
         * Alters the nickname
         * @param aContactListId Id of list to be updated
         * @param aWVId wvid of server-side contact
         * @param aNickname nickname of server-side contact
         */
        virtual void UpdateContactNicknameL( const TDesC& aContactListId,
                                             const TDesC& aWVId,
                                             const TDesC& aNickname ) = 0;

        /**
        * Fetches the list of lists from server.
        * @param aContactListIds List names
        */
        virtual TContactListsWrapper FetchServerContactListsL() = 0;

        /**
        * Releases resources related to all other contact list, except given one
        * @param aExcludedContactList Name of list to be kept
        */
        virtual void ReleaseContactListsL( const TDesC& aExcludedContactList ) = 0;

        /**
         * Set new display name for the contact list
         * @param aContactListID    ID of the list
         * @param aDisplayName      New display name for list
         * @return Error code
         *
         * @since 3.0
         */
        virtual TInt SetDisplayNameL( const TDesC& aContactListID, const TDesC& aDisplayName ) = 0;

        /**
         * Delete the contact list
         * @param aContactListID    ID of the list
         * @return Error code
         *
         * @since 3.0
         */
        virtual TInt DeleteContactListL( const TDesC& aContactListID
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                                         , TBool aFailedList = EFalse
#endif // IMPS_CONTACT_FETCH_BACKGROUND
                                       ) = 0;

        /**
         * Create contact list to server with given display name
         * List id will be generated from display name
         * @param aDisplayName Display name for new contact list
         * @return Error code
         *
         * @since 3.0
         */
        virtual TInt CreateContactListL( const TDesC& aDisplayName ) = 0;

        /**
        * Returns a reference to error container, that was filled by the last
        * operation response.
        * @return Reference to error container
        */
        virtual const MCAPresenceErrors& LastOperationResult() const = 0;

        /**
         * Cancel pending requests to network
         * @since 2.5
         */
        virtual void CancelPendingRequests() = 0;

        /**
         * Get temporary identification of contact which does not belong to contact list.
         * @since 2.6
         * @param aUserId, Contact id.
         * @return Contact identication. Nick or ripped userid.
         */
        virtual TPtrC IdentificationL( const TDesC& aUserId ) = 0;

        /**
         * Check if synchronization of contact lists is done
         * @since 3.2
         * @return ETrue if done
         */
        virtual TBool IsSyncDone() = 0;

        /**
            * AddObserver method for registering the other class
         */
        virtual void AddObserver( MCAListInitObserver* aObserver ) = 0 ;

        /**
            * RemoveObserver method for unregistering the other class
         */
        virtual void RemoveObserver() = 0;

        /**
         * IsListInitialization return the init bool value
         * @since 3.2
         * @return EFalse if done
         */
        virtual TBool IsListInitializationCompleted() = 0;

        virtual TBool IsContactFetchingCompleted() = 0;

        virtual void AddMainViewProcessObserver( MCAListInitObserver* aObserver ) = 0;

        virtual void RemoveMainViewProcessObserver() = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCAContactLists() {};
    };

#endif      // MCACONTACTLISTS_H

// End of File

