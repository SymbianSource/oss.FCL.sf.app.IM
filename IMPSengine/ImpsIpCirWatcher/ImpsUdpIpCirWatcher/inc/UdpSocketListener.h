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
* Description: Listener for Udp socket.
*
*
*/


#ifndef __UDPSOCKETLISTENER__
#define __UDPSOCKETLISTENER__

// INCLUDES
#include <in_sock.h>
#include <es_sock.h>
#include "ImpsIpCirWatcherAPI.h"

//FORWARD DECLARATIONS

// CLASS DECLARATION
class CUdpSocketListener : public CActive

    {
    public:  // Constructors and destructor

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        static CUdpSocketListener* NewL( const TInt aServicePort,
                                         const MImpsCirReceiver& aReceiver );

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        const TInt ServicePort() const;

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        TInt Contains( const MImpsCirReceiver& aReceiver ) const;

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        void AppendReceiverL( const MImpsCirReceiver& aReceiver );

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        TBool RemoveReceiver( const MImpsCirReceiver& aReceiver );

        /**
        * Destructor.
        */
        virtual ~CUdpSocketListener();

    public:   //New functions

        /**
        * Returns the current state of the watcher. This method comes
        * in handy for example in situations where a GPRS resume has
        * just been occurred. In such case the client is able to ask
        * whether or not the watcher has recovered from the event.
        */
        TInt StartUdpSocketListen( RSocketServ& aSocketSession );

    private:

        /**
        * C++ default constructor is private
        */
        CUdpSocketListener( const TInt aServicePort );

        /**
        * Symbian OS second-phase constructor.
        */
        void ConstructL( const MImpsCirReceiver& aReceiver );

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * Bind the listener socket and start listening
        */
        void BroadcastMessageL();

        /**
        * Bind the listener socket and start listening
        */
        void BroadcastErrorL( const TInt aError );

        /**
        * Bind the listener socket and start listening
        */
        void Activate();

    private: //Data

        RSocket                                 iListenerSocket;
        const TInt                              iServicePort;
        TBuf8<512>                              iSocketReceiveBuffer;
        TSockXfrLength                          iRecvLen;
        TInetAddr                               iInetAddress;
        TBool                                   iDestructed;
        RPointerArray<MImpsCirReceiver>         iReceiverQueue;
    };

#endif

// End of File
