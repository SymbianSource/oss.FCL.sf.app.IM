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
* Description:  Reactive authorization engine.
*
*/

#ifndef __CPENGAUTHORIZATIONENGINE_H__
#define __CPENGAUTHORIZATIONENGINE_H__


//  INCLUDES
#include <e32base.h>
#include "CPEngStoreEntry.h"
#include "MPEngAuthorizationEngine.h"



// FORWARD DECLARATIONS
class CPEngSessionSlotId;
class CPEngAuthorizationResponse;
class CPEngAuthorizationRequest;
class CPEngAuthorizationItem;
class MPEngListLibFactory;



// CLASS DECLARATION
/**
 * Reactive authorization engine base class.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngAuthorizationEngine : protected CPEngStoreEntry,
            protected MPEngAuthorizationEngine
    {
    public:  // Two-phased constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CPEngAuthorizationEngine();

    public: // Session recognize functions

        /**
         * Session Id
         * @since 3.0
         * @return session Id
         */
        const CPEngSessionSlotId& SessionId( ) const;


        /**
         * Increase ref count
         *
         * @since 3.0
         * @return server address
         */
        void Open( );


    protected: // From MPEngReactAuthEngine

        /**
         * Update authorization request state
         *
         * @see <MPEngReactAuthEngine.h>
         */
        void UpdateAuthorizationStateL(
            CPEngAuthorizationResponse& aAuthResponse,
            TBool aPartialAcceptance );

        /**
         * Remove authorization request, User id is invalide
         *
         * @see <MPEngReactAuthEngine.h>
         */
        void RemoveAuthorizationRequestL(
            CPEngAuthorizationResponse& aAuthResponse );

        /**
         * Add Authorization request
         *
         * @see <MPEngReactAuthEngine.h>
         */
        void AddAuthRequestLX( CPEngAuthorizationRequest* aAuthRequest );


        /**
         * Add Authorization status
         * @see <MPEngReactAuthEngine.h>
         */
        void AddAuthStatusLX( CPEngAuthorizationResponse* aAuthStatus );


        /**
         * Engine store size counter
         * @see <MPEngReactAuthEngine.h>
         */
        TInt& SizeCounter();


        /**
         * Lock for Network publishing
         * @see <MPEngReactAuthEngine.h>
         */
        void LockForNetworkPublishing();


        /**
         * Unlock from Network publishing
         * @see <MPEngReactAuthEngine.h>
         */
        void UnLockForNetworkPublishing();


    protected: // Functions from CPEngStoreEntry

        /**
         * Externalize to the given stream
         * @see <MPEngStoreEntry.h>
         */
        void ExternalizeL( RWriteStream& aStream,
                           TPEngStorageType aStorageType ) const;

        /**
         * Internalize from the given stream
         * @see <MPEngStoreEntry.h>
         */
        void InternalizeL( RReadStream& aStream,
                           TPEngStorageType aStorageType );

        /**
         * This method is used to retrieve ID for storing.
         * @see <MPEngStoreEntry.h>
         */
        const TDesC& StorageId() const;


        /**
         * This method is used to ask entry size in bytes.
         * @see <MPEngStoreEntry.h>
         */
        TUint32 EntrySize() const;


        /**
         * Handle change of the Storage IDs
         * @see <MPEngSIDChangeObserver.h>
         */
        void HandleSIDsChangeL( CPtrCArray& aChangedSIDs );


        /**
         * Called when SIDs notifier fails
         * @see <MPEngSIDChangeObserver.h>
         */
        void HandleSIDNotifyError( TInt aError );


    protected: // New functions

        /**
         * Authorization requests count
         *
         * @since 3.0
         * @return count of authorization requests
         */
        TInt AuthorizationRequestCount() const;


        /**
         * Get Authorization request from the store
         *
         * @since 3.0
         * @param aIndex index of the authorization request
         * @return authorization request
         */
        CPEngAuthorizationRequest& AuthorizationRequest( TInt aIndex );


        /**
         * Authorizations statuses count
         *
         * @since 3.0
         * @return count of authorization statuses
         */
        TInt AuthorizationStatusCount() const;


        /**
         * Get Authorization status from the store
         *
         * @since 3.0
         * @param aIndex index of the authorization status
         * @return authorization status
         */
        CPEngAuthorizationResponse& AuthorizationStatus( TInt aIndex );


        /**
         * Find authorization request
         *
         * @since 3.0
         * @param aUserId user id to look by
         * @return authorization request
         */
        CPEngAuthorizationRequest& AuthorizationRequestL( const TDesC& aUserId );


        /**
         * Find authorization response in passed array
         *
         * @since 3.0
         * @param aArray array to look in
         * @param aUserId user id to look by
         * @return index in array or KErrNotFound
         */
        static TInt FindAuthResponse(
            const RPointerArray<CPEngAuthorizationResponse>& aArray,
            const TDesC& aUserId  );


    private: // New helpers

        /**
         * Find authorization item in array
         *
         * @since 3.0
         * @param aArray array to look in
         * @param aUserId user id to look for item
         * @return index of item in array or KErrNotFound
         */
        template <class T>
        static TInt FindUserIdItem( const RPointerArray<T>& aArray,
                                    const TDesC& aUserId );

        /**
         * Adopt local flag and clean up old array
         *
         * @since 3.0
         * @param aArray Array to clean.
         * @param aNewItem Entry for which to adopt local flag,
         */
        template <class T>
        static void AdoptAndClean( RPointerArray<T>& aArray,
                                   T& aNewItem );


        /**
         * Moves elements from one array to another.
         *
         * @since 3.0
         * @param aTarget Target for move.
         * @param aSource Source to move from.
         */
        template <class T>
        static void AppendRemoveL( RPointerArray<T>& aTarget,
                                   RPointerArray<T>& aSource );


    protected: // protected that they can be called from derived classes

        /**
         * C++ constructor.
         */
        CPEngAuthorizationEngine( MPEngListLibFactory& aFactory );

        /**
         * Symbian constructor
         */
        void ConstructL( const CPEngSessionSlotId& aSessionSlot );


    protected: //Data
        /// REF: List lib factory
        MPEngListLibFactory&                            iFactory;

        /// Access count
        TInt                                            iAccessCount;


    private:  //Data

        /// OWN: Authorization requests
        RPointerArray<CPEngAuthorizationRequest>        iAuthRequests;

        /// OWN: Responded Authorizations
        RPointerArray<CPEngAuthorizationResponse>       iAuthStatuses;

        /// OWN: Session Id
        CPEngSessionSlotId*                             iSessionId;
    };

#endif      //  __CPENGAUTHORIZATIONENGINE_H__

//  End of File






















