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
* Description: Including CImpsScheduler: WV engine thread scheduler.
*              Including CImpsConn: CSP session connections.
*              Including CImpsCSPDestroyer : This deletes CSP session entity asynchronously
*/


#ifndef CIMPSSERVER_H
#define CIMPSSERVER_H

//  INCLUDES
#include <e32base.h>
#include <s32file.h>
#include "impsconst.h"
#include "impsservercommon.h"
#include "impsservices.h"
#include "impsmessageinterpreterapi.h"
#include "impsdataaccessorapi.h"
#include "impssrvutils.h"
#include "impscspsessionapi.h"
#include "impscommonenums.h"

// DATA TYPES

// Server operations (states)
enum TImpsSrvOperation
    {
    EImpsSrvNone,
    EImpsSrvShuttingDown,
    EImpsSrvFinish      // no return back, scheduler will be stopped
    };

// Terminal off-line states
enum TImpsSR2TOff
    {
    EImpsSR2TOffNone,
    EImpsSR2TOffStarted
    };


// FORWARD DECLARATIONS
class CImpsSession;
class CImpsSubSession;
class CImpsFields;
class CImpsShutdownTimer;
class CImpsExpiryTimer;
class CImpsVariant;
class CImpsOrphans;
class CImpsCSPDestroyer;
class MImpsCSPSession;
class CImpsTOffObserver;

// CLASS DECLARATION

/**
* CImpsScheduler
* WV engine thread scheduler.
*/
class CImpsScheduler: public CActiveScheduler
    {
    public:  // from base class

        /**
        * Error handling that ignores leaves of
        * active object RunL methods
        */
        void Error( TInt anError ) const;
    };

/**
*  CSP session connections
*/
class CImpsConn : public CBase
    {
    public:

        /** Two-phased constructor.
        * @param aSess CSP session entiry
        * @param aServer server entity
        * @return CImpsConn
        */
        static CImpsConn* CImpsConn::NewL(
            MImpsCSPSession* aSess, CImpsServer& aServer );

        /**
        * Delete the CSP session asynchronously, i.e. this
        * can be deleted inside transport response.
        */
        void DeleteCSPAsynch();

        /**
        * Match with a a specific CSP session entity
        * @param aCSP CSP connection id
        * @return ETrue if matches
        */
        TBool MatchCSP( TImpsSessIdent aCSP );

        /**
        * destructors
        */
        ~CImpsConn();

        /**
        * This deletes related CSP session entity and cancels
        * peninding asynchrounous deletion if such exists.
        */
        void Destroy();

        /**
        * Session accesor
        * @return CSP session entity
        */
        MImpsCSPSession* Sess();

    private:  // methods

        /**
        * contructor
        * @param aServer server entity
        */
        void ConstructL( CImpsServer& aServer );

        /**
        * contructor
        * @param aSess CSP session entity
        * @param aTimer delay timer for delayed off-line events
        */
        CImpsConn( MImpsCSPSession* aSess );

    private:  // data
        MImpsCSPSession*    iSess;
        CImpsCSPDestroyer*  iRemover;
    public:
        TDblQueLink         iLink;          //lint !e1925
    };


/**
* CImpsCSPDestroyer
* This deletes CSP session entity asynchronously
*/
class CImpsCSPDestroyer: public CActive
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        * @param aSess CSP session entity
        * @param aPriority active object priority
        */
        CImpsCSPDestroyer(
            CImpsServer& aServer,
            MImpsCSPSession& aSess,
            TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsCSPDestroyer();

        /**
        * Start deletion.
        */
        void DeleteCSPAsynch();

    protected:
        // From base class
        void RunL();
        void DoCancel();
    private:
        CImpsServer&        iServer;
        MImpsCSPSession&    iSess;
        TBool               iCanceled;

    };

/**
*  CImpsServer.
*  The core of the whole WV engine.
*/

class CImpsServer : public CPolicyServer

    {
    public:  // Constructors and destructor

        /**
        * constructs and starts the server.
        */
        static CImpsServer* New();

        /**
        * Destructor.
        */
        virtual ~CImpsServer();

    public: // New functions

        /**
        * Memory allocations. Called by a thread start function!
        */
        void ConstructL();

        /**
        * Deletes this session's first buffered event from the list and sends the next
        * one if there are any waiting.
        *
        * @param aSession the session concerned
        *
        */
        void NextEvent( CImpsSession* aSession );

        /**
        * Close client-server session.
        * Cancel transport and start idle timer if there is only one session.
        * @param aCSP CSP connection id
        * @param aComplete TRUE is session entity creation is complete
        */
        void CloseSession( TImpsSessIdent aCSP, TBool aComplete );

        /**
        * Stop the engine right now
        */
        void DoShutDown();

        /**
        * Server bootstrap code.
        *
        * @param aSignal signal that's used to inform the client thread when the server
        *                is up.
        *
        */
        IMPORT_C static TInt ThreadStart( TImpsSignal& aSignal );

        /**
        * Login WV server
        * @param aUser User-id  (max 50 characters)
        * @param aPsw password  (max 50 characters)
        * @param aClientId client id used to log in server.
        *        The client-id having prefix '+' or number is considered
        *        as MSISDN number (E.164),
        *        otherwise it is handled as URL (max 200 character).
        *        This MAY be used for CIR addressing if SAP is not able to
        *        detect MSISDN number in other way
        * @param aServer SAP URI, may include port number
        * @param aAP CommDb id for access point
        * @param aAnyAP ETrue if any other open access point is accepted
        * @param aKey1 future option
        * @param aKey2 future option
        * @param aSub requesting subsession
        * @param aLoginExpiry epiry time of login request
        * @param aReactive reactive presence auth negotiation
        * @return TID
        */
        TPtrC LoginL(
            const TDesC& aUser,
            const TDesC& aPassword,
            const TDesC& aClientId,
            const TDesC& aSAP,
            TUint32 aAP,
            const TDesC& aKey1,
            const TDesC& aKey2,
            CImpsSubSession* aSub,
            const TTime aLoginExpiry,
            TBool aReactive );

        /**
        * Logout SAP
        * @param aCancel ETrue if login cancel functionaliy wanted
        * @param aCSP CSP connection id
        * @return TID
        */
        TPtrC LogoutL(
            TBool aCancel,
            TImpsSessIdent aCSP );

        /**
        * CIR message handler
        * @param aCookie entire session cookie message
        */
        void CirMessageL(
            const TDesC8& aCookie );

        /**
        * Send data from session current data to WV server.
        * Login must have been done.
        * @param aSession requestion client session
        * @param aCSP CSP connection id
        * @return transaction-id
        */
        TPtrC SendDataL(
            CImpsSubSession* aSession,
            TImpsSessIdent aCSP
        );

        /**
        * Get number of client sessions
        * @param aIgnoreCIR ETrue if CIR watcher session is ignored
        * @return number of sessions
        */
        TInt NbrSessions( TBool aIgnoreCIR );

        /**
        * Get number of client sessions for the specified CSP connection
        * @param aIgnoreCIR ETrue if CIR watcher session is ignored
        * @param aCSP CSP connection id
        * @return number of sessions
        */
        TInt NbrSessions( TBool aIgnoreCIR, TImpsSessIdent aCSP );

        /**
        * Stop the idle timer
        */
        void StopTimer() const;

        /**
        * Is the CSP session logged in
        * @param aCSP CSP connection id. If all values are zero length then
        * check if any session is logged in
        * @param password, OUTPUT, optional
        * @return ETrue if logged in
        */
        TBool IsLogged( TImpsSessIdent aCSP, TPtrC* aPsw );

        /**
        * CSP session-id accessor
        * @param aCSP CSP connection id. If all values are zero length then
        * check if any session is logged in
        * @return CSP session-id
        */
        TPtrC SID( TImpsSessIdent aCSP );

        /**
        * Is the CSP session capabilities negotiated
        * @param aCSP CSP connection id
        * @return ETrue if negotiated
        */
        TBool IsNegotiated( TImpsSessIdent aCSP );

        /**
        * Transport status change indicator
        * @param aConnectionState  new status, on_line, off_line or no_iap
        */
        void TransportStatus( EImpsInternalStatus aConnectionState,
                              MImpsCSPSession* aSess );

        /**
        * Active services accessor
        * @param aCSP CSP connection id
        * @return current WV services
        */
        TImpsServices* Services( TImpsSessIdent aCSP );

        /**
        * Is there pendig login operation
        * @param aCSP CSP connection id
        * @return ETrue if yes
        */
        TBool IsPendingLogin( TImpsSessIdent aCSP );

        /**
        * Generate expiry time
        * @param aMsgType WV message primitive
        * @return expiry time
        */
        TTime ExpiryTime( TImpsMessageType aMsgType );

        /**
        * Generate expiry time
        * @param aSrv WV service type
        * @return expiry time
        */
        TTime ExpiryTime( TImpsEventType aSrv );

        /**
        * Generate expiry time
        * @param aExpiry expiry time in seconds
        * @return expiry time
        */
        TTime ExpiryTime( TInt aExpiry );

        /**
        * Default Expiry time accessor
        * @param aSrv WV service type
        * @return expiry time in seconds
        */
        TInt ExpirySeconds( TImpsEventType aSrv );

        /**
        * check expired messages
        * @param aType WV service type
        */
        void CheckExpiryL( TImpsEventType aType );

        /**
        * check expired messages
        * @param aType WV service type
        */
        void CheckExpiryL( TImpsEventType aType, TTime aExpiry );

        /**
        * Cancel a transaction.
        * Removes request from server's transaction queue and
        * asks transport to cancel the request too.
        * @param aTID transaction id to identify the request.
        * @param aCSP CSP session id
        */
        void CancelTrans( const TDesC& aTID, TImpsSessIdent aCSP );

        /**
        * Cancel a transaction.
        * Removes request from server's transaction queue and
        * asks transport to cancel the request too.
        * @param aTID transaction id to identify the request.
        * @param aSess CSP session entity
        */
        void CancelTrans( const TDesC& aTID, MImpsCSPSession* aSess );

        /**
        * Do the logout routines
        * @param aOpId operation id, KImpsNullId means SAP initiated logout
        * @param aCSPDis if CSP session really disconnected (existed)
        */
        void DoLogoutSrv( TInt aOpId, TBool aCSPDis,
                          MImpsCSPSession* aSess );

        /**
        * Create container
        * @return container
        */
        CObjectCon* NewContainerL();

        /**
        * Remove container
        * @param aCon container
        */
        void RemoveContainer( CObjectCon* aCon );

        /**
        * Maximum transaction content
        * @return max size for WV message
        */
        TInt BufferSize();

        /**
        * Receive terminal off_line/on_line indication
        * @param aParam True if connections allowed, else False.
        */
        void SetConnAllowed( TBool aParam );

        /**
        * Decodes accessor
        * Subsession needs this to build XML-formatted response
        * @return Message decoder
        */
        inline MImpsDecoder& Decoder() const;

        /**
        * iConnAllowed accessor
        * @retuns ETrue if connection allowed and terminal is not OFF-LINE
        */
        inline TBool IsConAllowed() const;

        /**
        * Discard a request from a session
        * @param aTid transaction-id to be discarded
        * @param aType event type (service type)
        * @param aCode an error code to a client request
        * @param aCSP CSP session id
        */
        void DiscardRequest(
            const TDesC& aTid,
            TImpsEventType aType,
            TInt aCode,
            TImpsSessIdent aCSP );

        /**
        * Discard a request from a session
        * @param aType event type (service type)
        * @param aExpiry request expiration time
        * @param aCSP CSP session id
        * @param aSess CSP session entity
        */
        void DiscardRequests(
            TImpsEventType aType,
            TTime aExpiry,
            TImpsSessIdent aCSP,
            MImpsCSPSession* aSess );

        /**
        * Discard all requests from a session
        * @param aType event type (service type)
        * @param aError error code to be sent
        * @param aCSP CSP session id
        * @param aSess CSP session entity
        */
        void DiscardRequests(
            TImpsEventType aType,
            TInt aError,
            TImpsSessIdent aCSP,
            MImpsCSPSession* aSess );

        /**
        * Discard Logout request that actually sends logout events to clients
        * @param aTID transaction-id
        * @param aCSP CSP session id
        */
        void DiscardLogout(
            const TDesC& aTID,
            TImpsSessIdent aCSP );

        /**
        * Make and send simple error event to certain type of sessions
        * @param aType session event type
        * @param aCode error code
        * @param aOpId operation-id
        * @param aCSP CSP session id
        */
        void SendErrorEvent(
            TImpsEventType aType,
            TInt aCode,
            TInt aOpId,
            TImpsSessIdent aCSP );

        /**
        * Send online status change events to clients
        * @param aStatus new engine status
        * @param aCSP CSP session id
        */
        void SendStatusEvents(
            EImpsInternalStatus aStatus,
            TImpsSessIdent aCSP );

        /**
        * Handle notification orders for incoming message.
        * Create corresponding events
        * @param aFields response message
        * @param aCSP CSP session id
        * @return True if matching subsession is found.
        * (client-id and media type ok)
        */
        TBool CheckNotifications(
            CImpsFields* aFields,
            TImpsSessIdent aCSP );

        /**
        * Handles pending requests for the incoming message
        * @param aFields response message
        * @param aFound OUT: true if found request
        * @param aCSP CSP session id
        * @return error code
        */
        void CheckRequestsL(
            CImpsFields* aFields,
            TBool& aFound,
            TImpsSessIdent aCSP );

        /**
        * Handle orphan message
        * @param aFields msg, CImpsServer takes the ownership of this msg.
        * @param aCSP CSP session id
        */
        void HandleOrphanL( CImpsFields* aFields, TImpsSessIdent aCSP );

        /**
        * Get next orphan message for the subsession
        * @param aCID client-id
        * @param aServiceType the type of the client which requests the message
        * @param aCSP CSP session id
        * @return new message, NULL if no match. Ownership of the msg is
        * passed to the caller of this method.
        */
        CImpsFields* NextOrphanLC(
            const TDesC& aCID,
            TImpsEventType aServiceType,
            TImpsSessIdent aCSP );

        /**
        * Check all orphans messages and send them to clients.
        * This is called when a client session starts to share
        * an existing CSP session.
        */
        void HandleAllOrphans();

        /**
        * Get matching CSP session entity
        * @param aCSP CSP connection id
        * @param aAll sessions shutting down are returned too
        * @returm matching CSP session or NULL
        */
        MImpsCSPSession* GetCSP( TImpsSessIdent aCSP,
                                 TBool aAll );

        /**
        * Create CSP session entity
        * @param aCSP CSP connection id
        * @param aVer CSP version
        * @returm CSP session
        */
        MImpsCSPSession* NewCSPL( TImpsSessIdent aCSP, TImpsCspVersion aVer );

        /**
        * Delete CSP session entity
        * @param aSess CSP session entity
        */
        void DeleteCSP( MImpsCSPSession* aSess );

        /**
        * CSP session entity counter
        * @return number of CSP sessions
        */
        TInt NbrCSPs();

        /**
        * CSP session id seed to generate unique ids across CSP sessions
        */
        TInt TidSeed();

        /**
        * Close all CSP sessions immediately and go to SHUTTING_DOWN state
        */
        void LogoutAll();

        /**
        * CSP version
        */
        inline TImpsCspVersion CspVersion();

        /**
        * Is shutting down?
        * @return ETrue if state is SHUTTING_DOWN
        */
        inline TBool IsShuttingDown();

        /**
        * Current settings accessor
        * @return current variant settings
        */
        inline const TImpsSrvSettings* Settings() const;

        /**
        * Reset expiry timer if needed
        * The time may decrease only
        * @param aSeconds subsession expiry time in seconds. 0 means undefined and default values are used.
        * @param aInit set this in login so that timer is started first time.
        */
        void SetExpiryTimer( TInt aSeconds, TBool aInit );

        /**
        * Reset expiry timer if needed
        * The time may increase only when a subsession is closed
        * @param aSeconds subsession expiry time in seconds. 0 means undefined value.
        */
        void ResetExpiryTimer( TInt aSeconds );


    public:  // Functions from base classes

        /**
        * Opens a new session to the server.
        *
        * @param aVersion required version of the server.
        * @param aMessage client request
        * @return pointer to a new session
        *
        * Leaves if:
        * - Required version not supported (KErrNotSupported)
        * - In very rare case when engine is going down (KErrNotReady)
        */
        CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

    protected:  // Functions from base classes

        /**
        * C++ default constructor.
        */
        CImpsServer();

    private: // Functions from base classes


        // From CPolicyServer
        CPolicyServer::TCustomResult CustomSecurityCheckL(
            const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing );


        void GetCSPVersion();

    private:

        // By default, prohibit copy constructor
        CImpsServer( const CImpsServer& );
        // Prohibit assigment operator
        CImpsServer& operator= ( const CImpsServer& );

        /**
        * Send extra events to HandleLogoutL methods to clients
        * @param aSub subsession to be ignored
        *        IF NULL the all subsessions handled
        * @param aOpId operation-id, -1 means SAP initiated disconnect
         * @param aCSP CSP connection id
        */
        void SendLogoutEvents( CImpsSubSession* aSub,
                               TInt aOpId, TImpsSessIdent aCSP );

        /**
        * Transport status change indicator
        * @param aConnectionState new status, on_line, off_line or no_iap
        * @param aSess CSP session entity
        */
        void DoTransportStatusL( EImpsInternalStatus aConnectionState,
                                 MImpsCSPSession* aSess );

        /**
        * Do the logout routines
        * @param aSub requesting subsession, logout event ignored for this
        * @param aOpId operation id, KImpsNullId means SAP initiated logout
        * @param aCSPDis true if CSP really disconnected
        * @param aCSP CSP connection id
        */
        void DoLogoutNow(
            CImpsSubSession* aSub,
            TInt aOpId,
            TBool aCSPDis,
            TImpsSessIdent aCSP );

        /**
        * Close all CSP session emtities
        * @param aCSP CSP connection id
        */
        void CloseAllCSPs();

        /**
        * Start to delete CSP session entity asynchronously
        * @param aSess CSP session entity
        */
        void DeleteCSPAsynch( MImpsCSPSession* aSess );

    public:     // Data

    protected:  // Data

    private:    // Data
        CObjectConIx*       iContainerIndex;
        RFs                 iFs;

        CImpsFields*        iSnd;        // internal data structure

        CImpsShutdownTimer* iShutTimer;
        CImpsExpiryTimer*   iExpiryTimer;

        TImpsSrvSettings    iSettings;

        CImpsVariant*       iVariant;
        TBool               iConAllowed;    // Is network connections allowed

        TImpsCspVersion     iCSPVersion;
        TImpsSrvOperation   iOperation;

        CImpsOrphans*       iOrphans;
        TDblQue<CImpsConn>  iCSPList;

        TInt                iSeed;
        CImpsTOffObserver*  iObserver;
        TInt                iExpiryInterval; // current expiry times value
    };

#ifdef __WINS__
EXPORT_C TInt ThreadFunction( TAny* aThreadParams );
#endif

#include "ImpsServer.inl"

#endif      // ?INCLUDE_H   

// End of File
