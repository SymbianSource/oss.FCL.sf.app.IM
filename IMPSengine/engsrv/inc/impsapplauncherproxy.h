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
* Description: Including  CLaunchRequest,CImpsAppLauncherProxy,MImpsAppLaunchHandler.
*              Starts a separate process to launch application
*/



#ifndef CIMPSAPPLAUNCHERPROXY_H
#define CIMPSAPPLAUNCHERPROXY_H

//  INCLUDES
#include <e32base.h>
#include "impsservercommon.h"


// FORWARD DECLARATIONS
class MImpsAppLaunchHandler;

// CLASS DECLARATION
/**
*  Launch request
*/

class CLaunchRequest : public CBase
    {
    public:
        static CLaunchRequest* NewL(
            const TDesC& aCID,
            const TDesC& aSAP,
            const TDesC& aUserId );

        ~CLaunchRequest();
        void Destroy();
        bool operator==( CLaunchRequest& rhs );
        const TDesC& ApplicationId();
        const TDesC& Sap();
        const TDesC& UserId();


    public: //data
        TDblQueLink             iLink;          //lint !e1925

    private:
        CLaunchRequest( );
        void ConstructL(
            const TDesC& aCID,
            const TDesC& aSAP,
            const TDesC& aUserId );
    private:
        HBufC*                  iCID;
        HBufC*                  iSAP;
        HBufC*                  iUserId;
    };

// CLASS DECLARATION
/**
*  Application launcher proxy
*  Starts a separate process to launch application
*  and manages the launch requests
*
*  @since 2.6
*/
class CImpsAppLauncherProxy : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aLaunchObserver the application launch observer
        * @return CImpsAppLauncherProxy instance
        */
        static CImpsAppLauncherProxy* NewL( MImpsAppLaunchHandler* aLaunchObserver );

        /**
        * Destructor.
        */
        virtual ~CImpsAppLauncherProxy();

    public: // New functions

        /**
        * Initiates the application launch for the given Client ID
        * @since 2.6
        * @param aApplicationId the Application ID of the application to be started
        * @param aSAP the remote SAP server from where the pending IM came
        * @param aUserId teh user ID who received the IM
        * @return error Application
        */
        TInt LaunchApplicationL( const TDesC& aApplicationId,
                                 const TDesC& aSAP,
                                 const TDesC& aUserID );

    protected:  // New functions

        /**
        * Starts the process which loads the launcher ECom plugin
        * @since 2.6
        * @param aRequest
        * @return error status
        */
        TInt DoStartProcess( CLaunchRequest& aRequest );

    protected:  // Functions from base classes

        /**
        * From CActive
        */
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        * @param aLaunchObserver the application launch observer
        */
        CImpsAppLauncherProxy( MImpsAppLaunchHandler* aLaunchObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Queues the request
        */
        void QueueRequest( CLaunchRequest& aRequest );

        /**
        * Process the next request in queue
        */
        void ProcessNextRequestL();

        /**
        * Checks whether the request is already in queue
        */
        TBool IsDuplicate( CLaunchRequest& aRequest );

    private:    // Data
        CLaunchRequest*         iCurrentRequest;   // the request being handled curently
        MImpsAppLaunchHandler*  iLaunchObserver; // callback handler
        TDblQue<CLaunchRequest> iRequestList; // request queue

    };


// CLASS DECLARATION
/**
*  Application launcher observer
*  Informs the result of the application launch
*
*  @since 2.6
*/
class MImpsAppLaunchHandler
    {
    public:
        virtual void HandleAppLaunch( const TDesC& aApplicationId, TInt aStatus ) = 0;
    };

#endif      // CIMPSAPPLAUNCHERPROXY_H   

// End of File
