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
* Description:  This file defines an interface for sending and receiving data
*               from transport module. In other words a transport module must
*               provide the methods defined below and call the observer methods
*               for incoming data.
*
*
*/

#ifndef WV_DATACHANNEL_API_H
#define WV_DATACHANNEL_API_H

// CLASS DECLARATIONS

/**
* MImpsDataReceiver
*
* Abstract interface for handling responses from data channel module.
* Server implements this class and gives a pointer to the data channel.
* Data channel module calls the method when it has received data.
*/

class MImpsDataReceiver
    {

    public: // New functions

        /**
        * Callback for transport request response
        * This handles the responses to the SendL() method from MImpsSender
        *
        * NOTE: It is very important to bear in mind that the memory
        *       area which the fourth parameter points to, MUST be
        *       deallocated by the object that implements this abstract
        *       interface! After the IMPS Data Channel component has
        *       called this method, it is no longer responsible for
        *       maintenance (including deallocation) of this chunk of
        *       heap. Failure to free the memory area will result in
        *       a serious memory leak and, consequently, a crash.
        *       Also note that in many error cases, including a request
        *       timing out, the pointer will be NULL as there is no data
        *       to relay.
        *
        * @param TInt Operation-id given in SendL() method
        * @param TInt Internal error code, KErrNone if successful
        * @param TInt HTTP status code, 200 if successful
        * @param HBufC8* Pointer to the raw eight-bit CSP data;
        *                must be deleted by the implementing class
        *                after its contents are no longer needed
        */
        virtual void TransportResponse( const TInt aId,
                                        const TInt aError,
                                        const TInt aHttpStatus,
                                        HBufC8* aCspMessage ) = 0;
    };

/**
* MImpsSender
*
* Abstract interface for sending messages to the data channel.
* The data channel module implements this class.
*/

//The bearer connection manager used by MImpsSender instances
class MMsgConnManager;

class MImpsSender
    {

    public: // New functions

        /**
         * Opens a session to the HTTP Stack.
         *    - If already opened then error code is returned ( KErrAlreadyExists )
         *    - Leaves with KImpsErrorBearerSuspended if GPRS is suspended
         *    - KErrOutOfMemory if new objects cannot be allocated
         *    (- some other system-wide error code returned by the HTTP framework)
         *
         * This is a synchronous method: after a call to this method, the client
         * application can assume the data channel to be open - provided the
         * method call did not leave.
         *
         * @param aSAP WV SAP.
         *        The address has a format:
         *        hostport [ "/" hpath [ "?" search ]]
         *        Refer to RFC1738
         * @return void
         */
        virtual void OpenL( const TDesC& aSAP ) = 0;

        /**
        * Closes the session to HTTP Stack
        * @return Status code of the performed operation.
        */
        virtual void Close() = 0;

        /**
        * Send a request.
        * When this method returns the data is saved and aMessage
        * parameter can be re-used.
        * If the state of the bearer connection is other than
        * EImpsBearerActive, leaves with an error immediately:
        *   * EImpsBearerLost state - KImpsErrorBearerNotReady
        *   * EImpsBearerSuspended state - KImpsErrorBearerSuspended
        * The function may also leave with KImpsErrorSessionNotOpen
        * in case OpenL() has not been called prior to the call of this method.
        *
        * A response is received through MImpsReceiver.
        * The third parameter, aExpiryTime is optional. The function
        * first checks whether the value is something other than 0. If
        * it is, a timer gets instantiated and activated, and expires
        * according to the value passed in the parameter. The
        * value should be presented in seconds.
        *
        * @param aId operation-id to identify the request.
        * @param aMessage data to be sent
        * @param aExpiryTime expiry time of the request in seconds
        */
        virtual void SendL( const TInt aId,
                            const TDesC8& aMessage,
                            const TInt aExpiryTime = 0 ) = 0;

        /**
        * Destructor. A simple wrapper to the C++ destructor.
        *
        * NOTE that HTTP Stack uses the connection and socket
        * server instances owned by MImpsConnectionManager,
        * so one should be very careful NOT to Destroy() the
        * ConnMan instance before an instance of this interface.
        * At deletion time HTTP Stack might have outstanding socket
        * reads and/or writes, the cancelling of which will surely
        * fail in case the socket session on which the sockets were
        * initially opened has been prematurely closed.
        */
        virtual void Destroy() = 0;

        /**
        * Cancel all pending transactions.
        * After this request it is guaranteed that observer methods
        * are not called until new requests have been made.
        */
        virtual void CancelAll() = 0;

        /**
        * Cancel a named transaction.
        */
        virtual void CancelTransaction( const TInt aTID ) = 0;

        /**
        * Return the number of requests to which
        * a response has not yet been received.
        */
        virtual TInt PendingRequests() const = 0;

    };

/**
* Creates a new sender object
* @param RFs& aSession Reference to an opened file session
* @param TDesC8& aMimeType MIME type of the messages to be sent
* @param MImpsReceiver* aReceiver Pointer to the receiving object
* @return MImpsSender
*/
IMPORT_C MImpsSender* NewImpsSenderL( MImpsDataReceiver& aReceiver,
                                      MMsgConnManager& aConnManager,
                                      const TDesC8& aMimeType );

#endif      // ?INCLUDE_H   

// End of File
