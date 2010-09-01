/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Adapter for Http transport.
*
*
*/


#ifndef HTTPTRANSPORTADAPTER_H
#define HTTPTRANSPORTADAPTER_H

// INCLUDES
#include <f32file.h>
//HTTP Stack
#include <http.h>
//Bearer observer
#include <msgbearerobscallback.h>
//IMPS Sender interface
#include "ImpsDataChannelAPI.h"
//Internal headers
#include "ImpsTransportTimer.h"

//COMMON CONSTANTS
_LIT( KLogDir,                      "ImpsDataChannel" );
_LIT( KTransportLogFile,            "ImpsDataChannel.txt" );
const TInt KLogBufferMaxSize        = 2000;
_LIT( KDateFormat,                  "%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3" );
const TInt KMaxSubmitSize           = 1024;
const TInt KMaxHeaderNameLen        = 32;
const TInt KMaxHeaderValueLen       = 128;
const TInt KBufferMaxSize           = 2048;



//FORWARD DECLARATIONs
class CImpsHttpTransaction;
class MImpsReceiver;

// CLASS DECLARATION
class CHttpTransportAdapter : public CBase,
            public MImpsSender,
            public MMsgBearerObsCallback,
            public MHTTPTransactionCallback,
            public MImpsTransportTimerCallback
    {
    public:  // Constructors and destructor

        /**
        * Symbian OS default constructor.
        * @param aSession A reference to an opened file session
        * @param aMimeType MIME type of the messages to be sent
        * @param aReceiver The receiving object
        * @return CHttpTransportAdapter
        */
        static CHttpTransportAdapter* NewL( MImpsDataReceiver& aReceiver,
                                            MMsgConnManager& aConnManager,
                                            const TDesC8& aMimeType );

        /**
        * Opens a connection to IMPS server
        * @param aId ID of the request
        * @param aLog Is logging enabled or not
        * @param aProxy Shall we connect through a proxy server?
        * @param aSAP The address of the remote host
        * @param aIAP The ID of the access point to use
        */
        virtual void OpenL( const TDesC& aSAP );

        /**
        * Closes connection to WV server.
        * @return TInt
        */
        virtual void Close();

        /**
        * Send one IMPS message.
        * @param aId Transaction ID
        * @param aMessage Message to be sent
        * @param aExpiryTime The time period after which the message expires
        */
        virtual void SendL( const TInt aId,
                            const TDesC8& aMessage,
                            const TInt aExpiryTime = 0 );

        /**
        * Cancel the sending of all pending requests.
        * Obviously, the transactions cannot be "sucked back",
        * but they can be closed so that the responses never
        * reach the client application
        */
        virtual void CancelAll();

        /**
        * Cancel the sending of a named transaction.
        * Obviously, the transaction cannot be "sucked back",
        * but it can be closed so that the response never
        * reaches the client application
        */
        virtual void CancelTransaction( const TInt aTID );

        /**
        * Return the number of pending requests.
        * @return TInt
        */
        inline virtual TInt PendingRequests() const;

        /**
        * Destructor, which makes the C++ destructor "visible" to users
        * of the MImpsSender interface. This function simply says "delete this".
        */
        inline virtual void Destroy();

        /**
        * Destructor.
        */
        virtual ~CHttpTransportAdapter();

    public:   //New functions

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        inline TPtrC8 MimeType() const;

        /**
        * Returns the address of the default SAP
        * @return TUriParser The default SAP
        */
        inline TUriParser8 DefaultSAP() const;

        /**
        * Returns a reference to the currently active HTTP session
        * NOTE: Causes a panic if the session is closed.
        * @return RHTTPSession The currently active HTTP session
        */
        inline RHTTPSession& HttpSession();

        /**
        * Returns a reference to the receiver object
        * @return MImpsDataReceiver The current data receiver
        */
        inline MImpsDataReceiver& ReceiverHandle() const;

        /**
        * Check whether the HTTP session has been closed
        * @return TBool
        */
        inline TBool SessionClosed() const;

#ifdef _DEBUG

        /**
        * A global logging function for 16 bit data.
        * @param aCommand command to be handled
        */
        static void WriteToLog( TRefByValue<const TDesC> aFmt, ... );

        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static void WriteToLog( TRefByValue<const TDesC8> aFmt, ... );

#endif

    private:

        /**
        * C++ default constructor.
        */
        CHttpTransportAdapter( MImpsDataReceiver& aReceiver,
                               MMsgConnManager& aConnManager );

        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */
        void ConstructL( const TDesC8& aMimeType );

        /**
        * Initialises an HTTP session
        */
        void InitialiseSessionL();

        /**
        * Writes data to the input buffer
        * @param TDesC8& aBodyData
        */
        void WriteDataToInputBufferL( const TDesC8& aBodyData );

        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        void SetProxyL();

        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        void HandleBearerEventL( TBool aIsAuthClose, TMsgBearerEvent aBearerEvent );

        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        void HandleTemporaryRedirectionL( RHTTPTransaction aTransaction );

        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        void HandlePermanentRedirectionL( RHTTPTransaction aTransaction );

        /**
        * Stores the address of the SAP currently in use
        * @param aURL Address of the remote host
        */
        void SetUrlL( const TDesC& aURL );

        /**
        * Sets the MIME type of the messages
        * @param aMimeType MIME type of the messages
        */
        void SetMimeTypeL( const TDesC8& aMimeType );

        /**
        * Map an HTTP request to the corresponding IMPS request
        * @param aTransaction HTTP Transaction
        * @return CImpsHttpTransaction
        */
        CImpsHttpTransaction* ImpsTransaction( const RHTTPTransaction& aTransaction );

        /**
        * Returns the Content-Length header, if specified
        * @param aResponse HTTP response
        * @return TInt
        */
        TInt ContentLengthL( RHTTPResponse aResponse ) const;

        /**
        * Returns the Content-Length header, if specified
        * @param aResponse HTTP response
        * @return TInt
        */
        TPtrC8 LocationL( RHTTPResponse aResponse ) const;

        /**
        * Convert undefined HTTP stack errors into
        * something a but more meaningful
        * @param const TInt aError
        * @return TInt
        */
        TInt ConvertUndefinedError( const TInt aError );

        /**
        * Remove the named transaction from the queue of pending requests.
        * @param aRemove The transaction to be removed
        */
        void RemoveFromQueue( const CImpsHttpTransaction* aRemove );

#ifdef _DEBUG

        /**
        * Write response headers to the log file
        * @param aTrans HTTP Transaction
        */
        void DumpResponseHeadersL( RHTTPTransaction& aTrans,
                                   const TInt aTransactionID );

        /**
        * Remove the log file from a previous session
        */
        void DeleteLogFilesL();

#endif

        /**
        * From MHTTPTransactionCallback, handles all responses
        * coming from the remote host
        * @param aTransaction The finished transaction
        * @param aEvent Type of the event
        */
        void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

        /**
        * From MHTTPTransactionCallback, called when the RunL() of a transaction leaves
        * @param aInt The error the HTTP framework left with
        * @param aTransaction The failed transaction
        * @param aEvent The event that was being processed
        * @return KErrNone (HTTP framework panics if client returns any other error)
        */
        TInt MHFRunError( TInt aInt, RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

        /**
        * Handles timer expiry event
        * @param aRequester The object that started the timer
        * @param aStatus Status of the timer object
        */
        void HandleTransportTimerEventL( TImpsTimingRequester* aRequester,
                                         TInt aStatus );

    private: //Data

        TBool                                       iConnected;
        TBool                                       iSuspend;
        TBool                                       iSessionClosed;
        RHTTPSession                                iHttpSession;
        RPointerArray<CImpsHttpTransaction>         iTransactionQueue;
        TUriParser8                                 iDefaultURL;
        MImpsDataReceiver&                          iReceiver;
        MMsgConnManager&                            iConnManager;
        MHTTPDataSupplier*                          iRespBody;
        CBufBase*                                   iResponseBuffer;
        HBufC8*                                     iProxyBuffer;
        HBufC8*                                     iUrlBuffer;
        HBufC8*                                     iMimeBuffer;
        TInt                                        iCurrentDataLength;
    };

#include "HttpTransportAdapter.inl"

#endif

// End of File
