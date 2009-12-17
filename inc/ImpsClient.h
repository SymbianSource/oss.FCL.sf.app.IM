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
* Description:  Abstract base class for WV engine service APIs.
*
*/

#ifndef IMPS_CLIENT_H
#define IMPS_CLIENT_H

// INCLUDES
#include <e32base.h>
#include "impsconst.h"
#include "impsservices.h"
#include "impsdetailed.h"


// DATA TYPES

enum TImpsServiceStatus
    {
    EImps_ON_LINE,
    EImps_NOT_LOGGED,
    EImps_SHUTTING_DOWN
    };


// FORWARD DECLARATIONS
class CImpsHandler2;
class MImpsStatusHandler2;
class MImpsErrorHandler2;
class RImpsClient2;  

// CLASS DECLARATION

/**
* RImpsEng
*/

class RImpsEng : public RSessionBase
    {

    public: // Constructors and destructor

        /**
        * Constructor
        */
        IMPORT_C RImpsEng();


    public: // New functions
        
        /**
        * Connect to the engine.
        * Starts engine process if necessary
        * @param aRate error failure rate, for testing only.
        * @return General error code
        */
        IMPORT_C TInt Connect( TInt aRate = 0 );

        /**
        * Close connection to engine
        */
        IMPORT_C void Close();
        
        /**
        * Csp identifier
        * @return TImpsCspIdentifier 
        */
        TImpsCspIdentifier* CspIdentifier();
        
        /**
        * Set csp identifier
        * @param aCspId id of the csp session
        */
        void SetCspIdentifier( TImpsCspIdentifier aCspId );
        

    protected:

        /**
        * Gives the client side code version.
        *
        * @return version
        */      
        TVersion Version() const;
        

    private:

        /**
        * Check if server is running
        * @return ETrue if running
        */
        TBool IsServerStarted();

        /**
        * Start server thread
        * @param aRate OOM failure reate for testing only
        * @param sRound number of round, determines the drive where to start. 
        * @return error code
        */
        TInt StartServer( TInt aRate, TInt aRound );


    private: // data

        TBool                   iSessionOk;
        TImpsCspIdentifier      iCspId;
   
    };


/**
*  RImpsClient2 API
*
*  Abstract base class for WV engine service APIs.
*
*/ 

class RImpsClient2 : public RSubSessionBase
    {

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */      
        IMPORT_C RImpsClient2();

    public: // New functions        

        // INITILIZATION AND CONNECT

        /**
        * Registers the service status change observer.
        * Ignored if API has already registered an observer
        * This can be called after the actual registeration of the
        * particular client has been made (RegisterL).
        * @param aObs status observer implementation
        */
        IMPORT_C void RegisterStatusObserverL( MImpsStatusHandler2* aObs );

        /**
        * Unregisters the service status change observer.
        * Ignored if API does not have registered observer.
        * This method is not necessary if unregister of the particular
        * client type has been executed (Unregister).
        */
        IMPORT_C void UnregisterStatusObserverL( );

        /**
        * Available services accessor
        * Notice that the method leaves with KImpsErrorServices until
        * service negotiation has been run.
        * @param aServices Service tree having 
        * all supported features and functions.
        */
        IMPORT_C void GetServicesL( TImpsServices& aServices );

        /**
        * Register an error event observer.
        * If this is not registered then a client does not
        * receive error events from WV engine.
        * An application may implement only one HandleErrorL method
        * and give pointer to it in each WV client instance.
        * @param aObs error event observer
        */
        IMPORT_C void RegisterErrorObserverL( MImpsErrorHandler2& aObs );

        /**
        * Unregisters an error observer.
        * Ignored if API does not have an error observer.
        * This method is not necessary if unregister of the particular
        * client has been executed (Unregister).
        */
        IMPORT_C void UnregisterErrorObserverL( );

        /**
        * Get the maximum TransactionContent size that WV engine
        * and WV server can handle, related to ParserSize.
        * Note that if service negotiation is not finished, this
        * method returns 0.
        * 
        * @return size or zero
        */
        IMPORT_C TInt MaxTransactionContentLengthL( );

        /**
        * Get WV CSP version currently used
        * Used literals are defined in impsconst.h
        * @return version, e.g. "CSP1.1"
        */
        IMPORT_C TPtrC WVVersionL(); 

        /**
        * Get CSP identifier used in login
        * Leaves with KErrNotFound if not logged in.
        * @return TImpsCspIdentifier
        */
        IMPORT_C TImpsCspIdentifier* CspIdentifierL(); 

        /**
        * Cancel the ongoing request
        * Removes the request from engine's message queue
        */
        IMPORT_C void CancelTransaction( TInt aOperationId ); 
        
        /**
        * Sets the expiry time for requests
        * New expiry time will be used for following requests that 
        * are sent by corresponding impsclient.
        * Negative values are ignored.
        */
        IMPORT_C void SetExpiryTime( TInt aSeconds ); 

        /**
        * Internal use only.
        * Error observer accessor
        * @return error observer
        */
        MImpsErrorHandler2* ErrorHandler();

        /**
        * Internal use only
        * @return TImpsCspIdentifier
        */
        TImpsCspIdentifier* CspIdentifier(); 
        

    protected: // New functions

        /**
        * Unregisters the listener object and disconnects from the server.
        */
        void DoUnregister();

        /**
        * Assign application id for this session.
        *
        * This is an optional method, but highly recommended to provide
        * enhanced message routing in the terminal.
        * Possible reason for the method to fail is that 
        * RegisterL has not executed.
        * 
        * @param aClientId client-id for message routing
        * @return error code 
        */
        TInt DoAssign( const TDesC& aClientId );

        /**
        * Increases the operation id and if it goes over the upper range limit
        * set it back to the lower limit
        */
        void IncreaseOpId( );

        /**
        * Sets the operation id's upper and lower limit.
        * Also set the operation id to the lowerlimit.
        */
        void SetOpIdRange( );

       /**
        * Registers the listener object for Access events and connects to
        * the Symbian OS Server.
        * @param aEngine WV Engine server instance
        * @param aHandler event observer
        * @param aReq request to server TImpsServRequest
        * @return operation id
        */
        TInt DoRegister2( RImpsEng& aEngine,
                          CImpsHandler2* aHandler,
                          TInt aReq );

    private:

       /**
        * Registers the listener object for events and connects to
        * the Symbian OS Server.
        * @param aEngine WV Engine server instance
        * @param aHandler event observer
        * @return error code
        */
        virtual TInt DoRegister( RImpsEng& aEngine,
                                 CImpsHandler2* aHandler ) = 0;


    protected: // data

        CImpsHandler2*            iHandler;
        CActive*                  iActiveCommand;
        TBool                     iRunning;
        HBufC*                    iClientId;
        TInt                      iOpId; 
        TInt                      iLimitUpperValue;
        TInt                      iLimitLowerValue;
        TBool                     iSessionOpen;
        TBool                     iHandleNew;
        TBool                     iAnyContent;
        RImpsEng*                  iEngine; 
     
    private: // friend classes
        friend class CImpsHandler2;
    
    };

/**
* MImpsStatusHandler2
*
* Abstract interface for handling general status events from the server.
* User derives his class from this and implements the methods below.
*/

class MImpsStatusHandler2
    {

    public: // New functions

    /**
    * Observer method for changes in service status.
    * Each service type API may register this observer 
    * by RegisterStatusObserverL.
    * @param aStatus new status.
    * @param aCspId CSP session identifier
    */
    virtual void HandleStatusChangeL( TImpsServiceStatus aStatus,
                                      TImpsCspIdentifier& aCspId )  = 0;

    };

/**
* MImpsErrorHandler2
*
* Abstract interface for handling error events from the server.
* User derives his class from this and implements the method below.
*/

class MImpsErrorHandler2
    {

    public: // New functions

    /**
    * Observer method for error messages.
    * This provides data for partial error responses too.
    * Each service API MUST register this observer by RegisterErrorObserverL
    * immediately after Register method.
    * @param aStatus error code. 
    * @param aOpId operation id to map responses to the requests.
    * @param aDescripton error description
    * @param aDetailedRes detailed error description
    * @param aCspId CSP session identifier
    */
    virtual void HandleErrorL( TInt aStatus,
                               TInt aOpId,
                               const TDesC* aDescription,
                               const CImpsDetailed* aDetailedRes,
                               TImpsCspIdentifier& aCspId ) = 0;

    };


#endif      // IMPS_CLIENT_H  
            
// End of File
