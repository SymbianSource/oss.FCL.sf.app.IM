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
* Description:  Implements WV engine access services interface
*
*/



#ifndef CCAIMPSACCESSCLIENT_H
#define CCAIMPSACCESSCLIENT_H

//  INCLUDES
#include "MCAImpsAccessClient.h"

// CLASS DECLARATION

/**
*  Implements MCAImpsAccessClient.
*  Forwards requests to RImpsAccessClient
*
*  @lib CAAdapter.dll
*  @since 1.2
*/
class CCAImpsAccessClient : public CBase, public MCAImpsAccessClient
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAImpsAccessClient* NewL( RImpsEng& aImpsEng );

        /**
        * Destructor.
        */
        ~CCAImpsAccessClient();

    private:

        /**
        * C++ default constructor.
        */
        CCAImpsAccessClient( RImpsEng& aImpsEng );

    public: // Functions from base classes

        // INITILIZATION AND CONNECT

        /**
        * Registers the listener object for Access events and connects to
        * the Symbian OS Server.
        * @since 1.2
        * @param aObserver The observer.
        * @param aPriority Observer priority. Refer to CActive priority.
        */
        virtual void RegisterL(
            MImpsAccessHandler2* aObserver,
            TInt aPriority = 0 );

        /**
        * Unregisters the listener object and disconnects from the server.
        * @since 1.2
        */
        virtual void Unregister();

        /**
        * @see MCAImpsAccessClient
        */
        TInt NumberOfSessionsL( TImpsCspIdentifier& aCspId );

        // ACCESS SERVICES

        /**
        * @see MCAImpsAccessClient
        */
        TInt LoginL(  TImpsCspIdentifier aCspId,
                      const TDesC& aPsw,
                      const TDesC& aClientId,
                      TUint32 aAP,
                      const TDesC* aKey1 = NULL,
                      const TDesC* aKey2 = NULL );

        /**
        * @see MCAImpsAccessClient
        */
        TInt LogoutL();

        /**
        * Register an error event observer.
        * If this is not registered then a client does not
        * receive error events from WV engine.
        * An application may implement only one HandleErrorL method
        * and give pointer to it in each WV client instance.
        * @since 1.2
        * @param aObs error event observer
        */
        void RegisterErrorObserverL( MImpsErrorHandler2& aObs );

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
        * Status observer accessor. This is missing from WV Engine. Introduced for consistency.
        * @since 1.2
        * @return Status observer pointer. Ownership NOT transferred.
        */
        MImpsStatusHandler2* StatusHandler() const;

        /**
        * Available services accessor
        * Notice that there are no services in NOT_LOGGED state, i.e.
        * before login has been executed successfully from any client.
        * @since 1.2
        * @param aServices Service tree having all supported features and functions.
        */
        void GetServicesL( TImpsServices& aServices );

        /**
        * Returns actual WV Engine interface class pointer which is registered.
        * @since 1.2
        * @return RImpsAccessClient pointer which is registered and can be used.
        *  Ownership NOT transferred.
        */
        RImpsAccessClient2* InterfaceL();

    private:    // Data

        // WV Engine access interface.
        RImpsAccessClient2 iClient;

        // Indicator if we are doing lazy initialization with WV Engine.
        TBool	iLazyInitialization;

        // Indicator if we have registered WV Engine interface.
        TBool	iRegistered;

        // Store priority if we are doing lazy initialization.
        TInt	iPriority;

        // Used to store operation ID of LoginStatus if we don't start WV Engine but we answer back to UI.
        TInt	iStatusId;

        // CIdle class which is used to call back to UI if we don't start WV Engine. Owns.
        CIdle   *iCIdle;

        // Access observer, stored in lazy initialization. Doesn't own.
        MImpsAccessHandler2*	iAccessHandler;

        // Status observer, stored in lazy initialization. Doesn't own.
        MImpsStatusHandler2* iStatusHandler;

        // Error observer, stored in lazy initialization. Doesn't own.
        MImpsErrorHandler2* iErrorHandler;

        // Reference to IMPS engine server session instance.
        RImpsEng& iImpsEng;
    };

#endif      // MCAIMPSACCESSCLIENT_H

// End of File
