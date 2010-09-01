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
* Description:  Reactive authorization manager.
*
*/

#ifndef __CPENGAUTHORIZATIONMANAGER_H__
#define __CPENGAUTHORIZATIONMANAGER_H__


//  INCLUDES
#include <e32base.h>
#include "CPEngAuthorizationEngine.h"
#include "MPEngAuthorizationManager.h"
#include "MPEngAuthorizationManagerSet.h"


// FORWARD DECLARATIONS
class   CPEngAuthorizationRequest;

// CLASS DECLARATION
/**
 * Reactive authorization manager.
 * Provides interface to access all active authorizations.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngAuthorizationManager : public CPEngAuthorizationEngine,
            public MPEngAuthorizationManager,
            public MPEngAuthorizationManagerSet

    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor,
         */
        static CPEngAuthorizationManager* NewLC( MPEngListLibFactory& aFactory,
                                                 const CPEngSessionSlotId& aSessionId );

        /**
         * Destructor.
         */
        virtual ~CPEngAuthorizationManager();


    public:  // From MPEngAuthorizationManager

        /**
         * Close reference
         * @see <MPEngAuthorizationManager.h>
         */
        void Close();


        /**
         * Count all authorizations
         * @see <MPEngAuthorizationManager.h>
         */
        TInt AuthorizationsCount() const;


        /**
         * Get authorization request
         * @see <MPEngAuthorizationManager.h>
         */
        MPEngAuthorizationRequest& Authorization( TInt aIndex );


        /**
         * Count pending authorizations request
         * @see <MPEngAuthorizationManager.h>
         */
        TInt PendingAuthorizationsCount()  const;


        /**
         * Get pending authorizations request
         * @see <MPEngAuthorizationManager.h>
         */
        MPEngAuthorizationRequest& PendingAuthorization( TInt aIndex );


        /**
         * Issued authorization status count
         * @see <MPEngAuthorizationManager.h>
         */
        TInt RespondedAuthorizationsCount() const;


        /**
         * Issued authorization status
         * @see <MPEngAuthorizationManager.h>
         */
        const MPEngAuthorizationStatus& RespondedAuthorization(
            TInt aIndex ) const;


        /**
         * Server denied authorizations count
         * @see <MPEngAuthorizationManager.h>
         */
        TInt ServerDeniedAuthorizationsCount() const;

        /**
         * Server denied authorization
         * @see <MPEngAuthorizationManager.h>
         */
        const MPEngAuthorizationStatus& ServerDeniedAuthorization(
            TInt aIndex ) const;

        /**
         * Remove all Authorization responses
         * @see <MPEngAuthorizationManager.h>
         */
        void RemoveAllAuthorizationResponses();


        /**
         * Buffer with all packed authorization responses.
         * @see <MPEngAuthorizationManager.h>
         */
        HBufC16* BufferedTransactionsLC();


        /**
         * Register authorization observer.
         * @see <MPEngAuthorizationManager.h>
         */
        void RegisterAuthorizationObserverL(
            MPEngAuthorizationObserver& aObserver );

        /**
         * Unregister authorization observer.
         * @see <MPEngAuthorizationManager.h>
         */
        void UnRegisterAuthorizationObserver(
            MPEngAuthorizationObserver& aObserver );



    protected:  // From MPEngAuthorizationManagerSet

        /**
         * Set authorization response
         *
         * @see <MPEngAuthorizationManagerSet.h>
         */
        void SetAuthorizedAttributesL(
            const CPEngAuthorizationRequest& aAuthRequest,
            TArray<TUint32>& aAttributes,
            TBool aKeepOldAuthorization );

        void DenyAuthorizationL( const TDesC& aUserId );
        void CancelAuthorizationL( const TDesC& aUserId );


        /**
         * Removes buffered authorization response
         * @see <MPEngAuthorizationManagerSet.h>
         */
        void RemoveAuthorizationResponse( const TDesC& aUserId );


    private: //From CPEngStoreEntry / MPEngSIDChangeObserver

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


    private: // new functions

        /**
         * Rebuild array of pending and denied authorizations
         *
         * @since 3.0
         */
        void RebuildSupportingArraysL();


        /**
         * Notify observers about events
         *
         * @since 3.0
         */
        void NotifyAllChangesToObservers();


        /**
         * Notify observers about events
         *
         * @since 3.0
         */
        template <class NotifyItem>
        void NotifyChangeToObservers(
            void ( MPEngAuthorizationObserver:: *aNotifFunc )( NotifyItem& ),
            NotifyItem& aArgument );

        /**
         * Set all flags as to be notified
         *
         * @since 3.0
         */
        void UpdateAllFlags();


        /**
         * Get Authorization response.
         *
         * @since 3.0
         * @param aUserId user id of the Authorization response
         * @return authorization response
         */
        CPEngAuthorizationResponse& AuthorizationResponseL(
            const TDesC& aUserId );


        /**
         * Enable lock
         *
         * @since 3.0
         * @return ETrue if lock is put to clean up stack
         */
        TBool EnableLockLC();


        /**
         * Release lock
         *
         * @since 3.0
         */
        void ReleaseLock();


    public: // Lock clean up support

        /**
         * Handle release event
         * Used for Lock clean up stack support
         *
         * @since 3.0
         */
        void Release();


    private:

        /**
         * C++ constructor.
         */
        CPEngAuthorizationManager( MPEngListLibFactory& aFactory );

        /**
         * Symbian constructor
         */
        void ConstructL( const CPEngSessionSlotId& aSessionSlot );


    private: // Data

        /// OWN: Buffered authorization responses
        ///      Elements OWN
        RPointerArray<CPEngAuthorizationResponse>        iResponses;

        /// OWN: Pending authorizations,
        //       Elements REFerenced
        RPointerArray<CPEngAuthorizationRequest>         iPendingAuth;

        /// OWN: Denied authorization
        ///      Elements REFerenced
        RPointerArray<CPEngAuthorizationResponse>        iDeniedAuth;


        /// REF: Authorization Observers
        RPointerArray<MPEngAuthorizationObserver>        iObservers;


        /// OWN: Size of the buffered responses
        TInt                                             iBufRespSize;

    };

#endif      //  __CPENGAUTHORIZATIONMANAGER_H__

//  End of File






















