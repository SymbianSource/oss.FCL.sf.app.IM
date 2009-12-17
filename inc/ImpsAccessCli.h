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
* Description: 
*     Interface for WV access services
*
*/


#ifndef IMPS_ACCESS_CLIENT_H
#define IMPS_ACCESS_CLIENT_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <bamdesca.h>
#include "impsconst.h"
#include "impsclient.h"

// CONSTANTS

// FORWARD DECLARATIONS
class MImpsAccessHandler2;
class CImpsAccessCommand2;


// CLASS DECLARATION

/**
*  RImpsAccessClient2 API
*
*  Applications use Access service via RImpsAccessClient2 class. 
*  First, they need to connect to the server by calling RegisterL()
*  member function. Immediately after that an error handler
*  must be registered by RegisterErrorObserverL.   
*  
*  When everything is done, call Unregister() to end
*  the session.
*
*  A user of this API must be aware that raising/releasing
*  the PDP context may affect the response times of called functions.
*
*  One client-server session can have only one CSP session.
*
*  Observer methods:
*  -----------------------
*  Client can be notified when a certain type of message is received 
*  from the remote server.
*  Client has to be connected in the server in order to get notify events.
*  If the client is disconnected by server by calling Unregister(), 
*  all notify requests and possible incoming events are cancelled. 
*
*  Notify events are session-specific, i.e. if you have several 
*  ImpsAccessClients connected to the server, each has its own notify 
*  requests. 
*
*  In order to use notify services, you need to implement 
*  MImpsAccessHandler2 and give a pointer to your class in RegisterL.
*  Observer method doesn't need to do anything else than your application 
*  needs it to do.
*
* 
*  Addressing:
*  -----------
*  WV CSP specification defines characters that must be escaped if they
*  occur within the User-ID, Resource or Domain portions of a Wireless Village
*  address. WV engine does NOT escape those characters on behalf of user 
*  application. WV CSP also denies usage of certain characters in UserID
*  totally.
*
*/ 


class RImpsAccessClient2 : public RImpsClient2
    {

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */      
        IMPORT_C RImpsAccessClient2();

    public: // New functions        

        // INITILIZATION AND CONNECT

       /**
        * Registers the listener object for Access events and connects to
        * the Symbian OS Server.
        * Leaves with KImpsErrorAlreadyInUse if the client 
        * has already registered.
        * @param aEngine WV Engine server instance
        * @param aObserver The observer.
        * @param aPriority Observer priority. Refer to CActive priority.
        */
        IMPORT_C void RegisterL( RImpsEng& aEngine,
                                 MImpsAccessHandler2* aObserver,
                                 TInt aPriority = CActive::EPriorityStandard );

        /**
        * Unregisters the listener object and disconnects from the server.
        */
        IMPORT_C void Unregister();

        /**
        * Get number of Symbian OS sessions connected to the server
        * @param aCspId Identifier for the session.
        * @return number of sessions 
        */
        IMPORT_C TInt NumberOfSessionsL( TImpsCspIdentifier& aCspId );


        // ACCESS SERVICES

        /**
        * Login to Service Access Point (SAP).
        * MImpsAccessHandler2 handles successful server response.
        * Errors are handled by MImpsErrorHandler2:
        *   KImpsErrorAP if access point cannot be accessed
        *   KImpsErrorShuttingDown if authoritative session stop is ongoing.
        *   .
        * Leaves with KErrArgument if maximum lengths of
        * parameters are exceeded.
        * Leaves with KErrNotReady if login cancellation
        * is still in progress.
        * The CSP session id is stored in the engine, and it is not
        * visible in this API.
        * If there is ongoing login to the same CSP session leaves with
        * KErrNotReady.
        * @param aCspId contains SAP URI and userId. Note that a 
        *                       strict match will be used when compared to 
        *                       existing sessions. 
        * @param aPsw password  
        * @param aClientId client id used to log in server.
        *        The client-id having prefix '+' or number is considered 
        *        as MSISDN number (E.164), 
        *        otherwise it is handled as URL.
        *        This MAY be used for CIR addressing if SAP is not able to
        *        detect MSISDN number in other way
        * @param aAP CommDb id for access point
        * @param aKey1 privateKey1 or NULL
        * @param aKey2 privateKey2 or NULL
        * @return operation-id (positive) 
        */
        IMPORT_C TInt LoginL( TImpsCspIdentifier aCspId,
                              const TDesC& aPsw,
                              const TDesC& aClientId,
                              TUint32 aAP,
                              const TDesC* aKey1 = NULL,
                              const TDesC* aKey2 = NULL );

                              
        /**
        * Login to Service Access Point (SAP) using reactive authorization.
        * MImpsAccessHandler2 handles successful server response.
        * Errors are handled by MImpsErrorHandler2:
        *   KImpsErrorAP if access point cannot be accessed
        *   KImpsErrorShuttingDown if authoritative session stop is ongoing.
        *   .
        * Leaves with KErrArgument if maximum lengths of
        * parameters are exceeded.
        * Leaves with KErrNotReady if login cancellation
        * is still in progress.
        * The CSP session id is stored in the engine, and it is not
        * visible in this API.
        * If there is ongoing login to the same CSP session leaves with
        * KErrNotReady.
        * @param aCspId contains SAP URI and userId. Note that a 
        *                       strict match will be used when compared to 
        *                       existing sessions. 
        * @param aPsw password  
        * @param aClientId client id used to log in server.
        *        The client-id having prefix '+' or number is considered 
        *        as MSISDN number (E.164), 
        *        otherwise it is handled as URL.
        *        This MAY be used for CIR addressing if SAP is not able to
        *        detect MSISDN number in other way
        * @param aAP CommDb id for access point
        * @param aKey1 privateKey1 or NULL
        * @param aKey2 privateKey2 or NULL
        * @return operation-id (positive) 
        */
        IMPORT_C TInt ReactiveLoginL( TImpsCspIdentifier aCspId,
                              const TDesC& aPsw,
                              const TDesC& aClientId,
                              TUint32 aAP,
                              const TDesC* aKey1 = NULL,
                              const TDesC* aKey2 = NULL );                             

        /**
        * Logout the client session.
        * MImpsAccessHandler2 handles successful server response.
        * Errors are handled by MImpsErrorHandler2
        *        KImpsErrorNotLogged if was not logged in.
        *        KImpsErrorShuttingDown if authoritative session stop is ongoing.
        *        Note: Error messages are not sent for all pending regular 
        *        requests in case of a CSP session logout (either a terminal 
        *        or a SAP initiated logout). After the status has changed to 
        *        NOT_LOGGED, the pending regular requests are ignored and the 
        *        corresponding observer methods are never called. Therefore 
        *        it is highly recommendable that one client instance of the 
        *        application registers a status observer to detect the 
        *        NOT_LOGGED status.
        * @return operation-id (positive) 
        */
        IMPORT_C TInt LogoutL( );

        /**
        * Cancel the ongoing login
        * Causes a logout request to the WV server.
        * MImpsAccessHandler2 handles successful server response.
        * Errors are handled by MImpsErrorHandler2
        *   KImpsErrorNotLogged if is not tried to log in.
        *   KImpsErrorShuttingDown if authoritative session stop is ongoing.
        * Leaves with KErrArgument if there is no ongoing login 
        * If the login is completed in
        * engine, the request cannot be canceled, leaves with KErrNotFound.
        * In this kind of situations handleLogin or handleError will be called.
        * @param aCancelOpId, operation-id of the login call to be cancelled.
        */
        IMPORT_C void CancelLoginL( TInt aCancelOpId );

        /**
        * Authorative session stop
        * Closes all CSP sessions at once
        * 
        * After this method has been called, WVEngine calls         
        * HandleStatusChangeL(EImps_SHUTTING_DOWN).The client should then call
        * Unregister() immediately. When all clients are disconnected, WVEngine 
        * goes down. All method calls except Unregister()
        * and Close() during the shutdown will cause KImpsErrorShuttingDown 
        * Note: If there are any pending requests the WVEngine DOES NOT
        * complete the requests.  
        */
        IMPORT_C void CloseCspSessionsL( );

        /**
        * Pointer to the observer handler
        * @return handler
        */
        MImpsAccessHandler2* Handler();

        /**
        * Get stored operation id
        * @return opId
        */
        TInt LoginOpId();

        /**
        * Is the login cancelled
        * @return boolean
        */
        TBool LoginCancelled();

        /**
        * Login cancel handled
        */
        void CancelHandled();
        
    private:

       /**
        * Registers the listener object for Access events and connects to
        * the Symbian OS Server.
        * @param aEngine WV Engine server instance
        * @param aHandler the observer
        * @return general error code
        */
        TInt DoRegister( 
            RImpsEng& aEngine,
            CImpsHandler2* aHandler );


    private: // data

        CImpsAccessCommand2*             iCommand;
        MImpsAccessHandler2*             iHandlerCallBack;
        // Local copies of data to enable internally asyncronous request
        TPtrC8                           iSAP; 
        TInt							 iLoginOpId;
        TBool							 iLoginCancelled;
       
    private: // friend classes
        friend class CImpsAccessCommand2;
    
    };

// CLASS DECLARATION

/**
* MImpsAccessHandler2
*
* Abstract interface for handling the notify events from the server.
* User derives his class from this and implements the methods below.
*/

class MImpsAccessHandler2
    {

    public: // New functions

    /**
    * Observer method for Login event.
    * Login may be initiated by other application.
    * @param aId operation id received in LoginL. If this is 0, then the
    *        login was initated by other application or SAP.
    * @param aCspId CSP session identifier
    */
    virtual void HandleLoginL( TInt aId,
                               TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Observer method for Logout event.
    * @param aId operation id received in LogoutL. If this is 0, then the
    *        logout was initated by other application or caused by a serious 
    *        network error. SAP initiated logouts are identfied with -1.
    * @param aCspId CSP session identifier
    */
    virtual void HandleLogoutL( TInt aId,
                                TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Observer method for CancelLogin event.
    * @param aCancelledOpId operation id of the cancelled login.
    * @param aCspId CSP session identifier
    */
    virtual void HandleLoginCancelL( TInt aCancelledOpId,
                                     TImpsCspIdentifier& aCspId )  = 0;


    /**
    * Obsolete, NOT IN USE
    * @param aId operation id 
    * @param aNbr number of sessions 
    * @param aCspId CSP session identifier
    * 
    */
    virtual void HandleNbrSessionsL( TInt aId,
                                     TInt aNbr,
                                     TImpsCspIdentifier& aCspId )  = 0;

    };


#endif
