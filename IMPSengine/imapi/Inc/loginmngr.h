/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Class for handling the login issues.
*
*/



#ifndef IMAPI_LOGINMANAGER_H
#define IMAPI_LOGINMANAGER_H

//  INCLUDES
#include <impsaccesscli.h>

// FORWARD DECLARATIONS
class MLoginObserver;
class CImConnectionImpl;
//class TImpsCspIdentifier;

// CLASS DECLARATION

/**
*  Handles the login issues to WV server
*
*  @lib
*  @since S60 2.6
*/
class CLoginManager : public CActive,
            public MImpsAccessHandler2,
            public MImpsErrorHandler2,
            public MImpsStatusHandler

    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CLoginManager* NewL(
            RImpsEng& aEngine,
            CImConnectionImpl& aConnection );

        /**
        * Destructor.
        */
        virtual ~CLoginManager();

    public: // New functions

        /**
        * Starts the login sequence to WV server
        * @since S60 2.6
        * @param aObserver Observer of the completion
        * @return
        */
        TInt StartLogin( MLoginObserver* aObserver );

        /**
        * Cancels the login
        * @since S60 2.6
        */
        void CancelLogin( );

        /**
        * Is the LoginManager logged in
        * @since S60 2.6
        * @param
        * @return
        */
        TBool IsLoggedIn() const;

        /**
        * Returns the current state of the IMPS engine
        * @since S60 2.6
        * @param
        * @return
        */
        TImpsServiceStatus EngineStatus() const;

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

//        void HandleLoginStatusL(
//            TInt aId,
//            TInt aStatus,
//            TBool aIsLoggedIn
//            );

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
        void HandleStatusChangeL( TImpsServiceStatus aStatus );

    protected:  // New functions

        /**
        * Notifies the login observers
        * @since 2.6
        * @param aError the error code
        */
        void NotifyObserver( TInt aError );

    protected:  // From CActive
        void RunL();

        void DoCancel();

        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CLoginManager( RImpsEng& aEngine, CImConnectionImpl& aConnection );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:     // Data
        // Internal state enum of the class
        enum TState
            {
            EInitializing = 0,
            ELoggingIn,
            EFinishing,
            EReady
            };

        TState                          iState; // Internal state
        TBool                           iLoggedIn; // Is the object logged in
        RImpsAccessClient2              iAccessClient; // IMPS access client
        RImpsEng&                       iEngine; // Reference to the IMPS engine
        TImpsServiceStatus              iEngineState; // ImpsEngine State

        MLoginObserver*                 iObserver;
        CImConnectionImpl&              iConnection;

    };



// CLASS DECLARATION

/**
*  Login observer interface
*
*  @lib
*  @since S60 2.6
*/

class MLoginObserver
    {
    public:
        /**
        * This is called when login is complete
        * @since S60 2.6
        * @param aErrorCode Error status of the operation
        */
        virtual void HandleLogin( const TInt aErrorCode ) = 0;

        /**
        * This is called when logout even is received
        * @since S60 2.6
        */
        virtual void HandleNotLogged() = 0;
    };

#endif      // IMAPI_LOGINMANAGER_H   

// End of File
