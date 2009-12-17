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
* Description: 
* class for describing CIR watcher. listening for Cir Channel.
*
*/


#ifndef __IMPSCIRWATCHERAPI_H__
#define __IMPSCIRWATCHERAPI_H__

//List here the supported CIR types
enum TImpsSupportedCirTypes
    {
    EImpsTcpCir = 0,
    EImpsUdpCir
    };

//How many times connectiong should be retried
const TUint KMaxNumberOfConnAttempts                = 10;

//How many times pinging should be retried
const TInt KMaxNumberOfPingAttempts                 = 5;

//The default UDP Port number
const TUint KDefaultUDPPortNumber                   = 3717;

//Status constants
const TUint KImpsCIRWatcherIdle                     = 5001;
const TUint KImpsCIRWatcherUnknown                  = 5002;
const TUint KImpsCIRWatcherAwaitingDNS              = 5003;
const TUint KImpsCIRWatcherConnectingTCPSocket      = 5004;
const TUint KImpsCIRWatcherSendingHELOMessage       = 5005;
const TUint KImpsCIRWatcherReceivingHELOResponse    = 5006;
const TUint KImpsCIRWatcherSendingPINGRequest       = 5007;
const TUint KImpsCIRWatcherWaitingPINGResponse      = 5008;
const TUint KImpsCIRWatcherReceivingFromSocket      = 5009;
const TUint KImpsCIRWatcherBearerSuspended          = 5010;
const TUint KImpsCIRWatcherStopping                 = 5011;
const TUint KImpsCIRWatcherUDPWatcherActive         = 5012;

//FORWARD DECLARATIONS
class MMsgConnManager;
class MImpsCirReceiver;

// CLASS DECLARATION
class CImpsTcpCirWatcher : public CActive
    {
    public:

        /**
        * Two-phased constructor.
        * @param TInt Interval of the ping requests
        * @param MImpsCirReceiver& Reference to the receiving object
        * @param MImpsConnectionManager& Reference to the current
        *                                IMPS Connection Manager
        * @return CImpsTcpCirWatcher
        */
        static CImpsTcpCirWatcher* NewL( const TInt aPingInterval,
                                         MImpsCirReceiver& aCirReceiver,
                                         MMsgConnManager& aConnManager );

        /**
        * Destructor. A simple wrapper to the C++ destructor.
        *
        * NOTE that CIR watcher uses the connection and socket
        * server instances owned by MImpsConnectionManager,
        * so one should be very careful NOT to Destroy() the
        * ConnMan instance before an instance of this interface.
        * At deletion time this class might have outstanding socket
        * reads and/or writes, the cancelling of which will surely
        * fail in case the socket session on which the sockets were
        * initially opened has been prematurely closed.
        */
        virtual ~CImpsTcpCirWatcher();

    public:   //New functions

        /**
        * Start a TCP Watcher. This is an asynchronous request
        * that completes as soon as the watcher receives a response
        * to the HELO request. In such case the watcher completes
        * client's request and starts to ping the server in intervals
        * defined in the constructor call (default) or by the dedicated
        * method SetPingInterval(). The new value becomes effective
        * as soon as the ping request following the change of the value
        * has been issued.
        *
        * It should be noted that a call to this method means anything
        * only when watcher is at idle state. In other words, watcher
        * must be stopped before an attempt to start it again may take place.
        * If, for instance, a client wishes to change the server which
        * the watcher is connected to, it must first call StopTCPWatcher()
        * method and only then initiate a new connection to a different server.
        *
        * @param aStatus Request status of the client
        * @param aServerPort Port number of the server
        * @param aServerAddress Address of the server
        * @param aCSPSessionID ID of the client's CSP Session
        */
        virtual void StartTCPWatcherL( TRequestStatus& aStatus,
                                       const TUint aServerport,
                                       const TDesC& aServerAddress,
                                       const TDesC8& aCSPSessionID ) = 0;

        /**
        * Stops a TCP Watcher. This is a synchronous request
        * which cancels all socket reads and writes and,
        * eventually, closes the outbound socket, too.
        */
        virtual void StopTCPWatcher() = 0;

        /**
        * Returns the current state of the watcher. This method comes
        * in handy for example in situations where a GPRS resume has
        * just been occurred. In such case the client is able to ask
        * whether or not the watcher has recovered from the event.
        * In principle, the following two status values indicate an
        * error:
        *
        *     - KImpsCIRWatcherUnknown
        *     - KImpsCIRWatcherBearerSuspended
        *
        * If the watcher is ready to be started, the value should be
        * KImpsCIRWatcherIdle, not KImpsCIRWatcherUnknown. The latter
        * most likely indicates an irretrievable error, whereas the
        * status KImpsCIRWatcherBearerSuspended is an error in each
        * case: if suspend is the prevailing status on the client side
        * too, nothing can be sent or received; if, on the other hand,
        * this method returns a status value that suggests the watcher
        * thinks bearer is still suspended while the client has received
        * a resume event, the two modules are seriously out of sync.
        */
        virtual TUint WatcherStatus() const = 0;

        /*
        * Set the interval in which the watcher pings the server.
        * The new value becomes effective as soon as the ping request
        * following the change of the value has been issued; i.e. if
        * there is already a timer counting at the time of the call of
        * this method, it makes no sense to interrupt it.
        */
        virtual void SetPingInterval( const TInt aPingInterval ) = 0;

    protected:

        /**
        * C++ constructor
        * @return CImpsTcpCirWatcher
        */
        CImpsTcpCirWatcher();

    private:  //Data

        TUid                        iDestructorKey;
    };

class CImpsUdpCirWatcher : public CBase
    {
    public:

        /**
        * Two-phased constructor.
        * @return CImpsUdpCirWatcher Pointer to the UDP/IP CIR Watcher
        */
        static CImpsUdpCirWatcher* NewL();

        /**
        * C++ destructor
        */
        virtual ~CImpsUdpCirWatcher();

    public:   //New functions

        /**
        * Registers a UDP Watcher. UDP (User Datagram Protocol) is an
        * unreliable protocol in the sense that it does not guarantee
        * controlled transmission of user's data. The protocol is
        * essentially connectionless, which is, basically, also the
        * reason for its unreliability. Since the sending and receiving
        * parties do not synchronise, or "shake hands" at any time,
        * it may happen that the packets constituting the user data
        * arrive in random order or, in the worst case, get lost
        * altogether while en route. It is therefore advisable to
        * use TCP Watcher if reliability is the key aspiration.
        *
        * UDP Watcher, on the other hand, consumes a lot less resources,
        * since there is basially only one pending socket read, which,
        * from the point of view of the system, entails a single active
        * object waiting for a request (=the socket read) to complete.
        * A task could not be much more light-weight.
        *
        * It should be noted that the service port can (and may) be the
        * same for two different receivers. The watcher checks the number
        * of the port each time a new receiver is registered and if it finds
        * that the specified port is already reserved, it simply bundles up
        * the receivers that wish to receive CIRs from that specific port.
        *
        * The return value is KErrNone if the method executes successfully.
        * Otherwise it is one of the system-wide error codes.
        *
        * @param TInt The UDP Port to listen to
        * @param MImpsCirReceiver The object that receives incoming CIRs
        * @return TInt Error code
        **/
        virtual void RegisterCirReceiverL( const MImpsCirReceiver& aCirReceiver,
                                           const TInt aServicePort = KDefaultUDPPortNumber ) = 0;

        /**
        * Unregister a UDP Watcher. The return value is KErrNone if
        * the method executes successfully. Leaves with KErrNotFound
        * if the receiver to be unregistered has not been registered.
        * Otherwise the error is one of the system-wide error codes.
        *
        * @param MImpsCirReceiver The receiver to unregister
        * @return TInt Error code
        */
        virtual void UnregisterCirReceiverL( const MImpsCirReceiver& aCirReceiver ) = 0;

        /**
        * Reset all watchers on a specified port.
        *
        * NOTE: By default resets all watchers on all ports!
        *
        * @param TInt The port to reset; by default all ports
        * @return void
        */
        virtual void Reset( const TInt aServicePort = KErrArgument ) = 0;

        /**
        * Destructor.
        *
        * NOTE: This method only deletes the actual singleton in case
        *       there are no more references to it. If a client needs
        *       to get rid of the object right away, it should use the
        *       DeleteSingleton() method, instead.
        */
        virtual void Destroy() = 0;

        /**
        * Destructor. Deletes the singleton object.
        *
        * NOTE: After a call to this method, ALL references a client
        *       may still have to the UDP watcher will be unusable!
        */
        virtual void DeleteSingleton() = 0;

    protected:

        /**
        * C++ constructor
        * @return CImpsTcpCirWatcher
        */
        CImpsUdpCirWatcher();

        /**
        * Stores the destructor key returned by the ECOM framework.
        * This method is implemented in the plugin DLL so as not to
        * reveal too much of the implementation to client application.
        *
        * @param TUid Destructor key
        * @return void
        */
        virtual void StoreDestructorKeyL( const TUid aDestructorKey ) = 0;

    protected:  //Data

        RArray<TUid>                iDestructorKeyArray;
    };

/**
* MImpsCirReceiver
*
* Abstract interface for receiving CIRs from the server
* The client module implements this class.
*/
class MImpsCirReceiver
    {
    public:   //New functions

        /**
        * CIRWatcher calls this method when it receives a CIR
        * from the remote server.
        *
        * @TDesC8& The incoming data
        * @TImpsSupportedCirTypes The type of the received CIR
        * @return void
        */
        virtual void CirReceivedL( const TDesC8& aMessage,
                                   const TImpsSupportedCirTypes aCirType ) = 0;

        /**
        * CIRWatcher calls this method in case it encounters an
        * error situation from which it cannot recover. In general,
        * the watcher is designed to be as self-contained as possible,
        * but it can still happen that something quite unexpected
        * happens that forces the client to handle the situation by,
        * for instance, stopping the whole service and reinstating
        * the watcher after the erroneus conditions have been dealt with.
        * This could be called an emergency brake of a sort.
        *
        * @param TInt Error code
        * @param TImpsSupportedCirTypes The type of the received CIR
        * @return void
        */
        virtual void HandleErrorL( const TInt aReceiverError,
                                   const TImpsSupportedCirTypes aCirType ) = 0;
    };

/**
* This class is for packaging of the parameters needed by the
* TCP Watcher. The ECOM framework only accepts one parameter per
* constructor; thus the parameters need to be bundled together
* into a single utility object, the pointer to which will eventually
* be delivered to the constructor of the implementing class.
*/
class TTcpWatcherParameters
    {
    public:

        /**
        * C++ Constructor
        *
        * @param TInt Ping interval
        * @param MImpsCirReceiver& Reference to the receiver
        * @param MImpsConnectionManager& Reference to the connection manager
        * @return TTcpWatcherParameters
        */
        TTcpWatcherParameters( const TInt aPingInterval,
                               MImpsCirReceiver& aCirReceiver,
                               MMsgConnManager& aConnManager ) :
                iPingInterval( aPingInterval ),
                iCirReceiver( aCirReceiver ),
                iConnManager( aConnManager ) {}

    public:

        const TInt                  iPingInterval;
        MImpsCirReceiver&           iCirReceiver;
        MMsgConnManager&            iConnManager;
    };

#include "ImpsIpCirWatcherAPI.inl"

#endif

// End of File
