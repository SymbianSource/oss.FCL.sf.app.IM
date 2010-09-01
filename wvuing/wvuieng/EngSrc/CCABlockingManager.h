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
* Description:  Blocking manager handles the blocking and granting
 *
*/


#ifndef CCABLOCKINGMANAGER_H
#define CCABLOCKINGMANAGER_H

// INCLUDES
#include "MCABlocking.h"
#include "MCAImpsImClient.h"
#include "MCASettingsObserver.h"
#include "MCASettings.h"
#include "TStorageManagerGlobals.h"
#include "PublicEngineDefinitions.h"
#include "PrivateEngineDefinitions.h"
#include "MCABackgroundTask.h"
#include "mcastoredcontactsobserver.h"

#include "ImpsImCli.h"

#include <e32base.h>

// FORWARD DECLARATIONS
class CCARequestMapper;
class MCAStoredContacts;
class MCAImpsFactory;

// CLASS DECLARATION

/**
 *  Blocking manager handles the blocking and granting
 *  ( user ) operations.
 *
 *  @lib CAEngine.lib
 *  @since 1.2
 */
class CCABlockingManager : public CBase,
            public MImpsBlockingHandler2,
            public MCABlocking,
            public MCASettingsObserver,
            public MCABackgroundTask,
            public MCAStoredContactsObserver
    {

    private: // internal enumerations

        // internal states for blocking manager
        enum TBlockingMgrState
            {
            // normal operation
            EStateNormal = 0,

            // deleting of grant list, response to request needs to
            // be handled differently
            EStateDeletingGrantList
            };

    public:

        /**
         * Static constructor.
         * @param aImpsImServer is the API to the blocking services.
         * @param aRequestMapper is the pointer to the request mapper.
         *        Used in mapping requests to responses.
         * @param aSettingsManager The settings API
         */
        IMPORT_C static CCABlockingManager* NewL(
            MCAImpsFactory* aIMPSFactory,
            CCARequestMapper* aRequestMapper,
            MCASettings* aSettingsManager
        );

        /**
        * Virtual Destructor.
        */
        IMPORT_C virtual ~CCABlockingManager();

    protected: // Inherited from MCABlocking

    	/**
         * From MCABlocking
         * @see MCABlocking::SetAppUiExitFlag.
         */    	
    	void SetAppUiExitFlag( TBool aFlag );
    	
    	/**
         * From MCABlocking
         * @see MCABlocking::GetBlockedListFromServerL.
         */
        TInt GetBlockedListFromServerL();

        /**
         * From MCABlocking
         * @see MCABlocking::BlockedList.
         */
        const CDesCArray* BlockedList();

        /**
         * From MCABlocking
         * @see MCABlocking::InsertL.
         */
        TInt InsertL( const CDesCArray* aBlockList,
                      const CDesCArray* aGrantList,
                      TBool aForceUpdate = EFalse );

        /**
         * From MCABlocking
         * @see MCABlocking::RemoveL.
         */
        TInt RemoveL( const CDesCArray* aBlockList,
                      const CDesCArray* aGrantList,
                      TBool aForceUpdate = EFalse );

        /**
         * From MCABlocking
         * @see MCABlocking::IsContactBlocked
         */
        TBool IsContactBlocked( const TDesC& aContact,
                                TBool aUseGrant = EFalse ) const;

        /**
         * From MCABlocking
         * @see MCABlocking::ContactListChangedL
         */
        void ContactListChangeL( TBool aUpdateInProgress );

        /**
         * From MCABlocking
         * @see MCABlocking::SetTemporaryGrantL
         */
        TInt SetTemporaryGrantL( const TDesC& aContact, TBool aOn );


    protected:  // from MImpsBlockingHandler

        /**
         * Handle response to getblockedlist request.
         * @param aBlockedList list of blocked users.
         * @param aBlockedListInUse is blocked list use or not.
         * @param aGrantedList list of granted users.
         * @param aGrantedListInUse is granted list use or not.
         */
        virtual void GetBlockedListResponseL(
            const MDesCArray* aBlockedList,
            TBool aBlockedListInUse,
            const MDesCArray* aGrantedList,
            TBool aGrantedListInUse,
            TImpsCspIdentifier& aCspId );

        /**
         * Server call back for handling successful operation requests.
         * @param aOperationId to map responses to the requests.
         * @param aStatus error code. KErrNone if successful.
         */
        virtual void HandleCompleteL(
            TInt aOperationId,
            TImpsCspIdentifier& aCspId );

    protected: // From MCASettingsObserver

        /**
         * Handles setting changes.
         * @param aChangedSettingEnum specifies the changed setting.
         * @since 2.1
         */
        void HandleSettingsChangeL( TInt aChangedSettingEnum );

    protected: //From MCABackgroundTask

        /**
         * This is the place where Block/GrantList synchronisation is started
         * after getting online event
         * @param aSubTask is the number of subtask, its counting down
         * @see MCABackgroundTask::HandleBackgroundTaskL
         */
        void HandleBackgroundTaskL( TInt aSubTask );

    protected: // From MCAStoredContactsObserver

        /**
         * @see MCAStoredContactsObserver
         */
        void HandleContactDelete( const TDesC& aContactId );

        /**
         * @see MCAStoredContactsObserver
         */
        void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact );

        /**
         * @see MCAStoredContactsObserver
         */
        void HandleChange( MCAContactList* aList,
                           MCAStoredContact* aContact,
                           TStorageManagerGlobals::TCAObserverEventType aEventType,
                           TBool aUserIdChanged );

    public:  // new methods

        /**
         * If network state changes, BlockingManager state should be updated
         * by calling this method.
         * @param aState New state.
         * @param aUseGrant ETrue if grant list can be used.
         */
        void HandleNetworkStateChange( TNetworkState aState,
                                       TBool aCanUseGrant );

    private: // new methods

        /**
         * Does the actual handling
         */
        virtual void DoGetBlockedListResponseL(
            const MDesCArray* aBlockedList,
            TBool aBlockedListInUse,
            const MDesCArray* aGrantedList,
            TBool aGrantedListInUse );

        /**
         * Does the actual handling
         */
        virtual void DoHandleCompleteL(
            TInt aOperationId,
            TInt aStatus );

        /**
         * Evaluate current grantlist handling strategy
         * @return ETrue if grant list is in use
         */
        TBool GrantListInUse();

        /**
         * Clear the current grant list.  Used internally when
         * strategy is to allow nobody.
         */
        void ClearGrantListL( TBool aForceUpdate = EFalse );

        /**
         * Add given wv id to the internal block list
         * @param aId The wv id
         */
        void AddToBlockListL( const TDesC& aId );

        /**
         * Remove given wv id from the internal block list
         * @param aId The wv id
         */
        void RemoveFromBlockList( const TDesC& aId );

        /**
         * Check if given wv id is in the internal block list
         * @param aId The wv id
         * @return ETrue if the contact is in the list (is blocked)
         */
        TBool InternallyBlocked( const TDesC& aId ) const;

        /**
         * Add all friends to the internal grant list
         * @param aDoNotIncludeList, List of items which we do not want to
         *                           include from contact list.
         */
        void GrantFriendsL( CDesCArray* aDoNotIncludeList = NULL );

        /**
         * Insert or remove to grant or block list.
         * Leaves with KErrArgument if aRequest is not EBlockRequest
         * or EUnBlockRequest.
         * @param aInsertBlockList WV IDs to be inserted to blocklist.
         * @param aRemoveBlockList WV IDs to be removed from blocklist.
         * @param aInsertGrantList WV IDs to be inserted to grantlist.
         * @param aRemoveGrantList WV IDs to be removed from grantlist.
         * @param aAdditionalData A pointer to an array which is set to
         *                        CCARequest as additional data.
         * @param aRequest Request type, either block or unblock request.
         * @return error code.
         */
        TInt DoBlockRequestL( const CDesCArray* aInsertBlockList,
                              const CDesCArray* aRemoveBlockList,
                              const CDesCArray* aInsertGrantList,
                              const CDesCArray* aRemoveGrantList,
                              const CDesCArray* aAdditionalData,
                              TOperationRequests aRequest );

        /**
         *  Checks if contacts is granted or not
         *  @since 2.1
         *  @param aContactId, contact which is checked
         *  @return ETrue if granted, EFalse if not.
         */
        TBool IsContactGranted( const TDesC& aContact ) const;

        /**
        * Creates the CDesCArray from the source.
        * Ownership is transferred to caller!
        * @since 2.1
        * @param aSource Array to be copied
        * @return Array containing the same elements as source array
        */
        static CDesCArray* CloneArrayLC( const MDesCArray& aSource );

        /**
         * Creates the CDesCArray containing the items from arrayDiff that
         * are not on arrayTest
         * Ownership is transferred to caller!
         * @since 2.1
         * @param aArrayDiff Array whose differences are copied
         * @param aArrayTest Array to be tested on
         * @return Array containing the differences
         */
        static CDesCArray* CreateNeutralDiffLC( const MDesCArray& aArrayDiff,
                                                const MDesCArray& aArrayTest );



        /**
         *  Check if InsertL/RemoveL should be processed or not.
         *  @since 2.1
         *  @param aBlockList, List of wvids to block
         *  @param aGrantList, List of wvids to grant
         *  @param aForceUpdate Force update (ETrue) or not
         *  @return ETrue, if update is needed.
         */
        TBool NeedUpdate( const CDesCArray* aBlockList,
                          const CDesCArray* aGrantList,
                          TBool aForceUpdate );

        /**
         *  Add contacts to granted list.
         *  @param aInsertArray. Array of contacts to be
         *                       inserted to granted list.
         */
        void AddToGrantedListL( const MDesCArray& aInsertArray );

        /**
         *  Remove contacts from granted list and modify removeArray
         *  @param aRemoveArray Array of contacts to be removed.
         *                      Remove those contacts from aRemoveArray which
         *                      are not meant to be deleted. For example:
         *                      Those which are still in other lists.
         */
        void RemoveFromGrantedList( CDesCArray& aRemoveArray );

    private:    // constructors and destructors

        /**
         * Default constructor.
         * @param aStorage is the pointer to the storage side. Used in updating
         *        flags etc.
         * @param aImpsImServer is the API to the server side.
         * @param aRequestMapper is the pointer to the request mapper.
         *        Used in mapping requests to responses.
         */
        CCABlockingManager(
            MCAImpsFactory* aIMPSFactory,
            CCARequestMapper* aRequestMapper,
            MCASettings* aSettingsManager );

        /**
         * ConstructL method.
         */
        void ConstructL();

    private: // Data

        // Doesn't own. Pointer to the IMPS interface factory.
        MCAImpsFactory* iImpsFactory;

        // Doesn't own. Pointer to the request mapper.
        CCARequestMapper* iRequestMapper;

        // doesn't own, pointer to settings API
        MCASettings* iSettingsManager;

        // Owns. List of blocked users, returned via BlockedList
        CDesCArrayFlat* iBlockedList;

        // Owns. List of granted users, returned via BlockedList
        CDesCArrayFlat* iGrantedList;

        // Maps the GetBlockedListFromServerL to GetBlockedListResponseL.
        TInt iGetBlockedListOpId;

        // Flag to tell if background tasks are run or not.
        TBool iBackgroundTaskReady;

        // handle to contacts list interface
        MCAStoredContacts* iStoredContacts; // not owned

        // the used granting strategy
        MCASettings::TCASettingValues iStrategy;

        // internal state
        TBlockingMgrState iMgrState;

        // Is our list valid
        TBool iBlocklistIsValid;

        // Can Grant list be used.
        TBool iCanUseGrant;

        //
        TBool iUpdateInProgress;

        TBool iLoggedIn;
        
        TBool iAppUiExit;
    };

#endif // CCABLOCKINGMANAGER_H
