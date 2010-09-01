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
*     WV Engine pure data API definition
*
*/


#ifndef RIMPSPURECLI_H
#define RIMPSPURECLI_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <bamdesca.h>
#include "impsconst.h"
#include "impsclient.h"

// CONSTANTS

// service type
enum TImpsServiceType
    {
    EImpsServiceNone = 0x0000,
    EImpsServiceAccess = 0x0001,
    EImpsServiceIM = 0x0002,
    EImpsServicePresence = 0x0004, 
    EImpsServiceGroup = 0x0010,
    EImpsServiceCommon = 0x0100   
    };

// FORWARD DECLARATIONS
class MImpsPureHandler2;
class CImpsPureCommand2;


// CLASS DECLARATION

/**
*  Pure data WV Client API
*
*  Applications use Access service via RImpsPureClient2 class. 
*  First, they need to
*  connect to the server by calling RegisterL() member function.
*  Immediately after that an error handler must be registered by 
*  RegisterErrorObserverL.  
*  
*  When everything is done, call Unregister() to end
*  the session.
*
*  A user of this API must be aware that raising/releasing
*  the PDP context may affect the response times of called functions.
*
*
*  Observer methods:
*  -----------------------
*  Client can be notified when a certain type of message is received 
*  from the remote server.
*  Client has to be connected in the server in order to get notify events.
*  If the client is disconnected by server by calling Unregister(), all notify
*  requests and possible incoming events are canceled. 
*
*  Notify events are session-specific, ie. if you have several RImpsPureClients
*  connected to the server, each has its own notify requests. 
*
*  In order to use notify services, you need to implement MImpsPureHandler2 
*  and give a pointer to your class in RegisterL.
*  Observer method doesn't need to do anything else than your application needs 
*  it to do.
*/ 

class RImpsPureClient2 : public RImpsClient2
    {

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */      
        IMPORT_C RImpsPureClient2();

    public: // New functions        

        // INITILIZATION AND CONNECT

       /**
        * Registers the listener object for Pure events and connects to
        * the SOS Server. 
        * Leaves with KImpsErrorAlreadyInUse if the client 
        * has already registered.
        * @param aEngine WV Engine server instance
        * @param aObserver The observer.
        * @param aType aServiceType
        * @param aReceiveNew determines if new pushed messages are received.
        *        If EFalse then new messages are filtered out and 
        *        responses to own requests are deliverd only.
        * @param aPriority Observer priority. Refer to CActive priority.
        */
        IMPORT_C void RegisterL( RImpsEng& aEngine,
                                 MImpsPureHandler2* aObserver,
                                 TImpsServiceType aType = EImpsServicePresence,
                                 TBool aReceiveNew = ETrue,
                                 TInt aPriority = CActive::EPriorityStandard );



        /**
        * Unregisters the listener object and disconnects from the server.
        */
        IMPORT_C void Unregister();

        /**
        * Send pure transaction content
        * Only primitives that belong to the presence service are allowed,
        * otherwise leaves with KImpsErrorUnknownMessageType.
        * If content size exceeds the maximum, 
        * leaves with KImpsErrorMessageTooBig.
        * Max. content size can be retrieved by calling 
        * method MaxTransactionContentSizeL()
        * @param aContent UTF-8 coded transaction content in XML format
        */
        IMPORT_C TInt SendPureDataL( const TDesC8& aContent );
      
        /**
        * Pointer to the pure data handler
        * @return handler
        */
        MImpsPureHandler2* Handler();

    private:

       /**
        * Registers the listener object for Presence events and connects to
        * the Symbian Server.
        * @param aEngine WV Engine server instance
        * @param aHandler the observer
        * @return general error code
        */
        TInt DoRegister( RImpsEng& aEngine,
                         CImpsHandler2* aHandler );

    private: // data

        CImpsPureCommand2*             iCommand;
        MImpsPureHandler2*             iPureCallback;
       
    private: // friend classes
        friend class CImpsPureCommand2;
    
    };

// CLASS DECLARATION

/**
* MImpsPureHandler2
*
* Abstract interface for handling the notify events from the server.
* User derives his class from this and implements the methods below.
*/

class MImpsPureHandler2
    {

    public: // New functions

        /**
        * Observer method for pure message content.
        * @param aOpId operation id received in SendPureDataL. If this is 0,
        *        then the HandlePureDataL was initated by SAP.
        * @param aBuffer UTF-8 coded message content in XML format
        * @param aCspId CSP session identifier
        */
        virtual void HandlePureDataL( TInt aOpId,
                                      const TDesC8& aBuffer,
                                      TImpsCspIdentifier& aCspId )  = 0;

    };

#endif // RIMPSPURECLI_H

// End of file
