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
* Description:  Implementation of the CImConnection interface (imconnection.h)
*
*/



#ifndef IMCONNECTIONIMPL_H
#define IMCONNECTIONIMPL_H

//  INCLUDES
#include <e32base.h>
#include <imconnection.h>
#include <ImpsAccessCli.h>



// FORWARD DECLARATIONS
class CContactDBReader;

// CLASS DECLARATION

/**
*  Implementation of the MImConnection interface
*
*  @lib imclient.lib
*  @since 3.0
*/
class CImConnectionImpl : public CImConnection,
            public MImpsAccessHandler2,
            public MImpsErrorHandler2,
            public MImpsStatusHandler2
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImConnectionImpl* NewL( TDesC& aApplicationId );

        /**
        * Destructor.
        */
        virtual ~CImConnectionImpl();

    public: // New functions

        /**
        * Return the IMPS engine
        * @since 3.0
        * @return the IMPS engine
        */
        RImpsEng& ImpsEngine();

        /**
        * Return the Contact DB reader
        * @since 3.0
        * @return the reader
        */
        CContactDBReader& ContactDBReader();

        /**
        * Returns the Application ID with which the client connected
        * @since 3.0
        * @return ClientId
        */
        const TDesC& ApplicationId();

        /**
        * Converts the error code returned by the IMPS engine to API error code.
        * @since 3.0
        * @param aErroCode ImpsEngine error code
        * @return API error code
        */
        TInt ConvertImpsEngineErrorCode( TInt aErrorCode );


    public: // Functions from base classes
        /**
        * From MImConnection
        */
        MImClient* CreateImClientL();

        /**
        * From MImConnection
        */
        void RegisterObserverL(
            MImConnectionObserver* aObserver );

        /**
        * From MImConnection
        */
        void UnregisterObserver();

        /**
        * From MImConnection
        */
        TImConnectionStatus ImConnectionStatus();

        /**
        * From MImConnection
        */
        void LoginL(
            const TDesC&  aServer,
            const TDesC&  aUserID,
            const TDesC&  aPassword,
            const TUint32 aAP );

        /**
        * From MImConnection
        */
        void CancelLoginL( );

        /**
        * From MImConnection
        */
        void LogoutL( );

    public: // from MImpsAccessHandler2

        void HandleLoginL(
            TInt aId,
            TImpsCspIdentifier& aCspId );

        void HandleLoginCancelL(
            TInt aCancelledOpId,
            TImpsCspIdentifier& aCspId );

        void HandleLogoutL(
            TInt aId,
            TImpsCspIdentifier& aCspId );


        void HandleNbrSessionsL(
            TInt aId,
            TInt aNbr,
            TImpsCspIdentifier& aCspId
        );

        void HandleErrorL(
            TInt                 aStatus,
            TInt                 aOpId,
            const TDesC*         aDescription,
            const CImpsDetailed* aDetailedRes,
            TImpsCspIdentifier&  aCspId );

    public: // from MImpsStatusHandler
        void HandleStatusChangeL(
            TImpsServiceStatus aStatus,
            TImpsCspIdentifier& aCspId );


    private:

        enum TConnectionState
            {
            EImNotLogged = 0,
            EImLoggingIn,
            EImCancelingLogin,
            EImLoggingOut,
            EImLoggedIn
            };
        /**
        * C++ default constructor.
        */
        CImConnectionImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TDesC& aApplicationId );

        /**
        * Changes the current state
        */
        void SetState( TConnectionState aNewState );

    private:    // Data
        RImpsEng                iEngine; // IMP engine
        RImpsAccessClient2      iAccessClient; // IMPS access client
        MImConnectionObserver*  iClientObserver; // Client observer
        CContactDBReader*       iContactReader; // Contact DB reader
        HBufC*                  iApplicationId;// Application ID of the
        // 3rd party app (owned)
        TReal                   iCspVersion; // the CSP version
        TInt                    iCurrentOpId;
        TConnectionState        iState;

    };

#endif      // IMCONNECTIONIMPL_H   

// End of File
