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
* Description: Tcp CirWatcher
*
*
*/


#ifndef __TCPCIRWATCHER_H__
#define __TCPCIRWATCHER_H__

// INCLUDES
#include <in_sock.h>
#include <es_sock.h>
#include <msgbearerobscallback.h>
#include "ImpsIpCirWatcherAPI.h"
#include "TcpCirWatcherTimerCallback.h"

//FORWARD DECLARATIONS
class CTcpCirWatcherTimer;
class TTcpWatcherParameters;
class MImpsConnectionManager;
class MImpsBearerObserverCallback;

// CLASS DECLARATION
class CTcpCirWatcher : public CImpsTcpCirWatcher,
            public MMsgBearerObsCallback,
            public MTcpCirWatcherTimerCallback

    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param TTcpWatcherParameters Parameter wrapper object
        * @return CTcpCirWatcher
        */
        static CTcpCirWatcher* NewL( TTcpWatcherParameters* aParameters );

        /**
        * Destructor.
        */
        virtual ~CTcpCirWatcher();

    public:   //New functions

        /**
        * Start a TCP Watcher. This is an asynchronous request
        * that completes as soon as the watcher receives a response
        * to the HELO request. In such case the watcher completes the
        * client's request and starts to ping the server in intervals
        * defined in the constructor call (default) or by the dedicated
        * method SetPingInterval(). The new value becomes effective
        * as soon as the ping request following the change of the value
        * has been issued.
        *
        * It should be noted that the call to this method means anything
        * only when the watcher is at idle state. In other words, the
        * watcher must be stopped before an attempt to start it again may
        * take place. If, for instance, the client wishes to change the
        * server to which the watcher is connected, it must first call
        * StopTCPWatcher() method and only then initiate a new connection
        * to a different server.
        *
        * NOTE: The contents of TDesC8& parameters are NOT copied. Thus the
        * client must make sure that the data the descriptors point to
        * exist at least as long as it takes the watcher to complete the
        * client's request.
        *
        * @param aStatus Request status of the client
        * @param aServerPort Port number of the server
        * @param aServerAddress Address of the server
        * @param aCSPSessionID ID of the client's CSP Session
        */
        virtual void StartTCPWatcherL( TRequestStatus& aStatus,
                                       const TUint aServerport,
                                       const TDesC& aServerAddress,
                                       const TDesC8& aCSPSessionID );

        /**
        * Stops a TCP Watcher. This is a synchronous request
        * which cancels all socket reads and writes and,
        * eventually, closes the outbound socket, too.
        */
        virtual void StopTCPWatcher();

        /**
        * Returns the current state of the watcher. This method comes
        * in handy for example in situations where a GPRS resume has
        * just been occurred. In such case the client is able to ask
        * whether or not the watcher has recovered from the event.
        */
        virtual TUint WatcherStatus() const;

        /**
        * Set the value of the PING timer.
        */
        virtual void SetPingInterval( const TInt aPingInterval );

#ifdef _DEBUG

        /**
        * A global logging function
        */
        static void WriteToLog( TRefByValue<const TDesC8> aFmt, ... );

#endif

    private:

        /**
        * C++ default constructor is public, because
        * FinishConstuctL() takes care of memory reservations.
        */
        CTcpCirWatcher( const TInt aPingInterval,
                        MImpsCirReceiver& aCirReceiver,
                        MMsgConnManager& aConnManager );

        /**
        * Connect a TCP Socket to the server
        */
        void ConnectTCPSocketL();

        /**
        * Send the initial "Hello World!" message
        */
        void SendHELOMessageL();

        /**
        * Receive data from the socket
        */
        void ReceiveFromSocket();

        /**
        * Wait for response to the HELO request
        * (=receive data from the socket)
        */
        void WaitForHELOResponse();

        /**
        * Send a ping request
        */
        void SendPingL();

        /**
        * Handle a timer event
        */
        void HandleTimerEventL( const TUint aExpiredRequest );

        /**
        * Handle a succesful request completion
        */
        void HandleRequestCompletionL();

        /**
        * Handle bearer suspension
        */
        void HandleBearerSuspension();

        /**
        * Handle bearer activation
        */
        void HandleBearerActivationL();

        /**
        * Handle an erroneous request completion
        */
        void HandleErrorL( const TInt aError );

        /**
        * Determine the state of the socket
        */
        void DetermineSocketStateL( const TInt aError );

        /**
        * Check the validity of the sender
        */
        TBool CheckAddressValidity();

        /**
        * Send the received CIR message to client
        */
        void ReportCirL();

        /**
        * Complete the request of the client application with an error
        */
        void CompleteWithError( const TInt aError );

        /**
        * Reset the necessary member variables before connecting
        */
        void Reset();

        /**
        * Save the necessary parameters before connecting
        */
        void SetConnectionDetailsL( const TDesC& aServerAddress,
                                    const TDesC8& aCSPSessionID,
                                    const TUint aServerPort );

        /**
        * Wait for timer to expire
        */
        void WaitForNextPing();

        /**
        * Handle a change in the status of the bearer.
        */
        void HandleBearerEventL( TBool aIsAuthClose, TMsgBearerEvent aBearerEvent );

        /**
        * Return the name of the listener
        */
#ifdef _DEBUG
        TPtrC8 Name() const;
#endif

        /**
        * Delete the watcher's log file
        */
        void DeleteLogFileL();

        /**
        * Symbian OS second-phase constructor.
        */
        void ConstructL();

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    private: //Data

        enum TImpsCIRWatcherState
            {
            EImpsCIRWatcherIdle                     = KImpsCIRWatcherIdle,
            EImpsCIRWatcherUnknown                  = KImpsCIRWatcherUnknown,
            EImpsCIRWatcherAwaitingDNS              = KImpsCIRWatcherAwaitingDNS,
            EImpsCIRWatcherConnectingTCPSocket      = KImpsCIRWatcherConnectingTCPSocket,
            EImpsCIRWatcherSendingHELOMessage       = KImpsCIRWatcherSendingHELOMessage,
            EImpsCIRWatcherReceivingHELOResponse    = KImpsCIRWatcherReceivingHELOResponse,
            EImpsCIRWatcherReceivingFromSocket      = KImpsCIRWatcherReceivingFromSocket,
            EImpsCIRWatcherSendingPINGRequest       = KImpsCIRWatcherSendingPINGRequest,
            EImpsCIRWatcherWaitingPINGResponse      = KImpsCIRWatcherWaitingPINGResponse,
            EImpsCIRWatcherBearerSuspended          = KImpsCIRWatcherBearerSuspended,
            EImpsCIRWatcherStopping                 = KImpsCIRWatcherStopping
            };

        TUint                           iServerPort;
        TUint                           iConnRetries;
        TInt                            iPingRetries;
        TInt                            iCompletionCode;
        TBool                           iClientRequestPending;
        RSocket                         iTCPCIRSocket;
        TBuf8<512>                      iSocketReceiveBuffer;
        TSockXfrLength                  iRecvLength;
        HBufC8*                         iCSPSessionID;
        HBufC8*                         iSocketSendBuffer;
        HBufC*                          iAddressBuffer;
        TInt                            iPingInterval;
        TInetAddr                       iSocketAddress;
        HBufC8*                         iPacket;
        TRequestStatus*                 iClientStatus;
        TNameEntry                      iHostAddressDnsEntry;
        RHostResolver                   iHostResolver;
        MImpsCirReceiver&               iCirReceiver;
        CTcpCirWatcherTimer*            iWatcherTimer;
        MMsgConnManager&                iConnectionManager;
        TImpsCIRWatcherState            iWatcherStatus;
        TImpsCIRWatcherState            iLastGoodWatcherStatus;
        RSocketServ                     iSocketSession;
        RConnection                     iConnection;
    };

#endif

// End of File
