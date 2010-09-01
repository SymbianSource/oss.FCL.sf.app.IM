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
* Description:  Implements instant messaging services interface.
 *
*/



#ifndef CCAIMPSIMCLIENT_H
#define CCAIMPSIMCLIENT_H

//  INCLUDES
#include "MCAImpsImClient.h"

// FORWARD DECLARATIONS
class RImpsEng;

// CLASS DECLARATION

/**
 *  Implements MCAImpsImClient.
 *  Forwards requests to RImpsImClient
 *
 *  @lib CAAdapter.dll
 *  @since 1.2
 */
class CCAImpsImClient : public CBase, public MCAImpsImClient
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aImpsEng Reference to RImpsEng.
         */
        static CCAImpsImClient* NewL( RImpsEng& aImpsEng );

        /**
         * Destructor.
         */
        ~CCAImpsImClient();

    public: // Functions from base classes

        /**
         * @see RImpsImClient2
         */
        void RegisterL(
            MImpsImHandler2* aImpsObserver,
            MImpsBlockingHandler2* aBlockingHandler,
            TUid aApplicationId,
            TInt aPriority = 0 );

        /**
         * @see RImpsImClient2
         */
        void Unregister();

        // MESSAGING

        /**
         * @see RImpsImClient2
         */
        // This is an adapter to IMPS Engine API, we cannot
        // influence the const descriptor pointer parameter passing
        // in the API.
        TInt SendTextMessageL(  const TDesC* aSenderSn, // CSI: 17 # See description
                                const MDesCArray* aUserIds,
                                const TDesC* aGroupId, // CSI: 17 # See description
                                const MDesCArray* aScreenNames,
                                const TDesC& aContent,
                                TBool aDeliveryReportWanted );

        /**
         * @see RImpsImClient2
         */
        // This is an adapter to IMPS Engine API, we cannot
        // influence the const descriptor pointer parameter passing
        // in the API.
        TInt SendContentMessageL( const TDesC* aSenderSn, // CSI: 17 # See description
                                  const MDesCArray* aUserIds,
                                  const TDesC* aGroupId, // CSI: 17 # See description
                                  const MDesCArray* aScreenNames,
                                  const TDesC& aContentType,
                                  const TDesC8& aContent,
                                  TBool aDeliveryReportWanted );

        /**
         * @see RImpsImClient2
         */
        TInt BlockEntityRequestL( const CDesCArray* aBlockEntity,
                                  const CDesCArray* aUnBlockEntity,
                                  TBool aBlockedListInUse,
                                  const CDesCArray* aGrantedEntityList,
                                  const CDesCArray* aUnGrantEntity,
                                  TBool aGrantedListInUse );

        /**
         * @see RImpsImClient2
         */
        TInt GetBlockedListRequestL();

        /**
         * Register an error event observer.
         * If this is not registered then a client does not
         * receive error events from WV engine.
         * An application may implement only one HandleErrorL method
         * and give pointer to it in each WV client instance.
         * @since 1.2
         * @param aObs error event observer
         */
        void RegisterErrorObserverL( MImpsErrorHandler2 &aObs );

        /**
         * Registers the service status change observer.
         * This can be called any time
         * @since 1.2
         * @param aObs status observer implementation. Ownership NOT transferred.
         */
        void RegisterStatusObserverL( MImpsStatusHandler2* aObs );

        /**
         * Unregisters an error observer.
         * Forwards the call to WV Engine always
         * This method is not necessary if unregister of the particular
         * client has been executed (Unregister).
         * @since 1.2
         */
        void UnregisterErrorObserverL();

        /**
         * Get the maximum TransactionContent size that WV engine
         * and WV server can handle, related to ParserSize.
         * Note that if service negotiation is not finished, this
         * method returns 0.
         *
         * @since 3.0
         * @return size or zero
         */
        TInt MaxTransactionContentLengthL( );

        /**
         * Unregisters the service status change observer.
         * Forwards the call to WV Engine always
         * This method is not necessary if unregister of the particular
         * client type has been executed (Unregister).
         * @since 1.2
         */
        void UnregisterStatusObserverL();

        /**
         * Internal use only.
         * Error observer accessor
         * @since 1.2
         * @return error observer pointer. Ownership NOT transferred.
         */
        MImpsErrorHandler2* ErrorHandler() const;

        /**
         * Internal use only.
         * Status observer accessor. This is missing from WV Engine.
         * Introduced for consistency.
         * @since 1.2
         * @return Status observer pointer. Ownership NOT transferred.
         */
        MImpsStatusHandler2* StatusHandler() const;

        /**
         * Available services accessor
         * Notice that there are no services in NOT_LOGGED state, i.e.
         * before login has been executed successfully from any client.
         * @param aServices Service tree having all supported features
         *        and functions.
         * @since 1.2
         */
        void GetServicesL( TImpsServices& aServices );

        /**
         * Used to get the interface. Visible because we need
         * to force lazy initialization (i.e. CCAImpsImClient
         * needs to do non-lazy init actually).
         * @see MCAImpsImClient.h
         */
        RImpsImClient2* InterfaceL();


    private:

        /**
         * C++ default constructor.
         * @param aImpsEng Reference to RImpsEng.
         */
        CCAImpsImClient( RImpsEng& aImpsEng );

    private:    // Data

        // WV Engine IM interface.
        RImpsImClient2 iClient;

        // Indicator if we are doing lazy initialization with WV Engine.
        TBool iLazyInitialization;

        // Indicator if we have registered WV Engine interface.
        TBool iRegistered;

        // Store priority if we are doing lazy initialization.
        TInt iPriority;

        // Store application id if we are doing lazy initialization.
        TUid iApplicationId;

        // IM observer, stored in lazy initialization. Doens't own.
        MImpsImHandler2* iImHandler;

        // Blocking observer, stored in lazy initialization. Doens't own.
        MImpsBlockingHandler2* iBlockingHandler;

        // Status observer, stored in lazy initialization. Doens't own.
        MImpsStatusHandler2* iStatusHandler;

        // Error observer, stored in lazy initialization. Doens't own.
        MImpsErrorHandler2* iErrorHandler;

        // Reference to IMPS engine server session instance.
        RImpsEng& iImpsEng;
    };

#endif      // MCAIMPSIMCLIENT_H

// End of File
