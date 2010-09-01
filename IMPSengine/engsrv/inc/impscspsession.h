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
* Description: Include CTransaction. CSP transaction state object
*              include CImpsCSPSession. The core of the whole WV engine.
*/


#ifndef CImpsCSPSession_H
#define CImpsCSPSession_H

#ifdef LOCAL_IMPS
#ifndef _FAKE_RESPONSE
#define _FAKE_RESPONSE
#endif
#endif

//  INCLUDES
#include <e32base.h>
#include <s32file.h>
#include "impsconst.h"
#include "impscspsessionapi.h"
#include "Impsservercommon.h"
#include "impsservices.h"
#include "impsmessageinterpreterapi.h"
#include "impsdataaccessorapi.h"
#include "impssrvutils.h"
#include "impscommonenums.h"

// DATA TYPES
// Sub-state operations
enum TImpsCSPState
    {
    EImpsCSPIdle,
    EImpsCSPShuttingDown,
    EImpsCSPDisconnecting,
    EImpsCSPLogging,
    EImpsCSPLogged,
    EImpsCSPNegotiation,
    EImpsCSPOnLine,
    EImpsCSPOnLineIdle,         // PDP-context closed
    EImpsCSPOnLineIdleStart,    // PDP-context closing
    EImpsCSPOnLineIdleEnd       // PDP-context re-opening
    };

// Poll-state
enum TImpsPollMode
    {
    EImpsPollNone,
    EImpsPollAdaptive,  // regular mode
    EImpsPollPending,   // pending send poll
    EImpsPollExtWait,   // Extra wait after on-line resume
    EImpsPollCIR        // cir mode
    };

// PDP:
enum TImpsPDPPend
    {
    EImpsPDPPendNone,
    EImpsPDPPendOpen,
    EImpsPDPPendClose
    };


// SAP initated transaction response status
enum TImpsSAPResStatus
    {
    EImpsSAPNone,       // no sap requests
    EImpsSAPMore,       // more sap requests
    EImpsSAPLast        // last sap request handled
    };
// Logout transaction type
enum TImpsLogoutTrans
    {
    EImpsLogoutNone,
    EImpsLogoutTerminal,   // terminal initiated
    EImpsLogoutSAP         // SAP inititiated
    };


// FORWARD DECLARATIONS
class CImpsFields;
class CImpsIdleTimer;
class CImpsSendReceive2;
class CImpsSendQueued;
class CImpsAliveManager;
class CImpsCirManager;
class CImpsVariant;
class CImpsPDPIdleTimer;
class CImpsPDPOpenTimer;
class CImpsTidQueue;

// CLASS DECLARATION

/**
*  CSP transaction state object
*/

class CTransaction : public CBase    // a synch request from client
    {
    public:
        CTransaction(
            TBool aOrphan,
            const TDesC& aTID,
            const TDesC& aSessionId,
            TImpsMessageType aMessageType,
            CImpsFields* aFields );
        ~CTransaction();
        void Destroy();

        TBuf<KImpsMaxTID>   iTID;           //lint !e1925  // CSP TID
        TBuf<KImpsMaxSID>   iSessionId;     //lint !e1925  // CSP session id
        TImpsMessageType    iMessageType;   //lint !e1925  // CSP message type
        CImpsFields*        iFields;        //lint !e1925
        TBool               iOrphan;        //lint !e1925  // Orphan msg?
        TDblQueLink         iLink;          //lint !e1925
    };

/**
*  Client request buffer element
*/
class CReq : public CBase    // a notify event
    {
    public:
        CReq( CImpsFields* aFields );
        ~CReq();
        void Destroy();

        CImpsFields*        iFields; //lint !e1925
        TDblQueLink         iLink;   //lint !e1925
        TTime               iExpiry; //lint !e1925
    };



/**
*  CImpsCSPSession.
*  The core of the whole WV engine.
*/
class CImpsCSPSession : public CBase, public MImpsCSPSession
    {
    public:  // Constructors and destructor


        /**
        * constructor
        * @param aServer server thread core
        * @param aSettings initial settings
        * @param aFs file system, for debug logs only
        * @paran aVariant run time variation accossor
        * @param aVer CSP protocol version to be supported
        */
        static CImpsCSPSession* NewL(
            CImpsServer& aServer,
            TImpsSrvSettings& aSettings,
            RFs& aFs,
            CImpsVariant& aVariant,
            TImpsCspVersion aVer );

        /**
        * Destructor.
        */
        virtual ~CImpsCSPSession();

    public: // New functions

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
        * @param aLoginExpiry expiry time for this request
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
            TTime aLoginExpiry,
            TBool aReactive );

        /**
        * Logout SAP
        * @param aCancel ETrue if login cancel functionaliy wanted
        * @leave KImpsErrorNotLogged if not logged. Does nothing.
        * @leave KImpsErrorTerminalOffLine if terminal off line. Rejects CSP anyway.
        * @return TID
        */
        TPtrC LogoutL( TBool aCancel );

        /**
        * CIR message handler
        * @param aCookie entire session cookie message
        */
        void CirMessageL(
            const TDesC8& aCookie );

        /**
        * Send data from session current data to WV server.
        * Login must have been done.
        * @param aFields data
        * @param aExpiry expiry time in seconds, if 0, then time in gerarated.
        * @param aOwnerCh OUTPUT ETrue is ownership of aFields is changed
        * @return transaction-id
        */
        TPtrC SendDataL(
            CImpsFields* aFields,
            TInt aExpiry,
            TBool& aOwnerChange );

        /**
        * Callback for response from transport watcher
        * @param aDataPtr data received, It must be set to NULL after data is consumed.
        */
        void TransportResponseL( HBufC8** aDataPtr );

        /**
        * Callback for error from transport watcher
        * @param aTID transaction-id, 0 means undefined
        * @param aError error code
        */
        void TransportErrorL(
            const TDesC& aTID,
            TInt aError
        );

        /**
        * Get next message to be sent. Copies the data to iOutputBuffer.
        * Validates first the expiry time and discards a message is necessary.
        */
        void GetNextMessageL( );

        /**
        * Send Poll request
        */
        void SendPollL();

        /**
        * Send KeepAlive request
        * @return TID of KeepAlive request
        */
        TPtrC DoSendAliveL( );

        /**
        * Send KeepAlive request
        * @return TID of KeepAlive request
        */
        TPtrC SendAliveL( );

        /**
        * Send KeepAlive request when GPRS resumes
        */
        void SendAliveInResume();

        /**
        * Is the server logging in curretly
        * @return ETrue if log in active
        */
        TBool IsLogging();

        /**
        * Is the server logged in
        * @return ETrue if logged in
        */
        TBool IsLogged();

        /**
        * Is the CSP session capabilities negotiated
        * @return ETrue if negotiated
        */
        TBool IsNegotiated();

        /**
        * Current User-id accessor
        * @return UserId
        */
        TPtrC UserId();

        /**
        * Current SAP accessor
        * @return SAP
        */
        TPtrC SAP();

        /**
        * Current CSP Session-id accessor
        * @return Session-id
        */
        TPtrC SID();

        /**
        * Current password accesor
        * @return password
        */
        TPtrC Password();

        /**
        * Transport status change indicator
        * @param aConnectionState  new status, on_line, off_line or no_iap
        */
        void TransportStatus( EImpsInternalStatus aConnectionState );

        /**
        * active services accessor
        * @return current WV services
        */
        TImpsServices* Services();

        /*
        * Is engine shutting down
        * @return ETrue if shutting down
        */
        TBool IsShuttingDown();

        /**
        * Is there pendig login operation
        * @return ETrue if yes
        */
        TBool IsPendingLogin();

        /**
        * Cancel a transaction.
        * Removes request from server's transaction queue and
        * asks transport to cancel the request too.
        * @param aTID transaction id to identify the request.
        */
        void CancelTrans( const TDesC& aTID );

        /**
        * Cancel active data channel requests.
        */
        void CancelData();

        /**
        * Close All.
        */
        void CloseAll();

        /**
        * Close PDP conntext only.
        */
        virtual void ClosePDP();

        /**
        * Open PDP context only.
        * @return error code
        */
        virtual TInt OpenPDP();

        /**
        * Removes request from server's transaction queue
        * @param aTID transaction id to identify the request.
        */
        void DiscardTid( const TDesC& aTid );

        /**
        * Give next non expired request waiting.
        * Deletes expired requests and informs corresponding session
        * @return request or NULL if not any
        */
        CReq* GiveNextRequestL();

        /**
        * Send so many queued client requests as possible to transport.
        *
        */
        void DoSendAllQueuedL();

        /**
        * Do the internal logout routines immediately
        */
        void DoLogout( );

        /**
        * Ip-Cir-channel opened callback
        */
        void CirChOpened();

        /**
        * Ip-Cir-channel error callback
        */
        void CirChError();

        /**
        * Receive terminal off_line/on_line indication
        * @param aParam True if connections allowed, else False.
        */
        void SetConnAllowed( TBool aParam );

        /**
        * Check exiparation of current requests and asks server to
        * remove requests from client sessions if necessary
        * @param sType service types
        * @param aExpiry expiration time for requests
        */
        void CheckExpiry(
            TImpsEventType aType,
            const TTime aExpiry  );

        /**
        * Destructor
        */
        void Destroy();

        /**
        * Is this logout transaction response
        * Harmonize WV 1.1 and WV 1.2 logout responses.
        * @param aFields received message, must not be NULL.
        * @return type of the logout transaction
        */
        TImpsLogoutTrans IsLogoutResp( CImpsFields* aFields );

        /**
        * Maximum parser size
        */
        TInt MaxParserSize();

        /**
        * Maximum IM message size
        */
        TInt MaxMessageSize();


    protected:  // Functions from base classes


        /**
        * C++ default constructor.
        */
        CImpsCSPSession( CImpsServer& aServer,
                         TImpsSrvSettings& aSettings,
                         RFs& aFs,
                         CImpsVariant& aVariant,
                         TImpsCspVersion aVer );

    private:

        /**
        * Memory allocations.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsCSPSession( const CImpsCSPSession& );
        // Prohibit assigment operator
        CImpsCSPSession& operator= ( const CImpsCSPSession& );

        /**
        * Insert client request into request queue.
        * This does not copy of the fields parameter anymore!
        * This is a buffer used while transport is busy
        * @param aFields request data from client
        * @param aExpiry expiry time in seconds
        * @param aLast ETrue if added to the end of request list,
        *        otherwise as the first one.
        */
        void QueueClientRequestL(
            CImpsFields* aFields,
            TInt aExpiry,
            TBool aLast = ETrue );

        /**
        * Route message to clients
        */
        void RouteMessageL();

        /**
        * Queue pending CSP transaction, SAP originated one.
        * @param aOrphan ETrue if the message is an orphan, i.e.
        * no subsession has accepted that because of illegal client-id.
        * Does not copy fields anymore!
        */
        void QueueTidL( TBool aOrphan );

        /**
        * Validate SAP initiated TID in iRcv.
        * Validation means that the TID is not already handled.
        * Saves also SAP into tid-client-history if not already there if
        * aClientHistory==ETrue.
        * @param aClientHistory ETrue if client-msg-history is to be validated,
        *        otherwise SAP-response-history is to be validated.
        * @return ETrue if TID not found in appropriate history
        */
        TBool ValidateTid( TBool aClientHistory );

        /**
        * Send so many queued requests as possible.
        * This starts an own active object to carry out this issue.
        */
        void SendAllQueued();

        /**
        * Create response for SAP Intiated transaction
        * @return SAP request status
        */
        TImpsSAPResStatus CreateSapResponse();

        /**
        * Generate TID
        */
        TPtrC GenerateTid();

        /**
        * Generate CSP Session Cookie
        */
        TPtrC GenerateCookie();

        /**
        * Delete pending transactions
        */
        void DeleteTransactions();

        /**
        * Delete client requests
        */
        void DeleteRequests();

        /**
        * Delete specific client requests
        * @param aTID transaction id
        */
        void DeleteRequest( const TDesC& aTID );

        /**
        * Check if there are any client request queued
        */
        TBool RequestExist();

        /**
        * Check if there are any SAP initiated request queued
        */
        TBool SapReqExist();

        /**
        * Handles negotiation data to internal data structures
        * Both Client capabilities and WV Services and Login response
        * @param aFields received message
        */
        void SessionStartupL( CImpsFields* aFields );

        /**
        * Send next buffered message to SAP
        * @param aFields message to be sent
        */
        void DoSendNextBufferedL( CImpsFields* aFields );

        /**
        * Encode and send a single regular message
        * @param aAc data accessor for a message, cannot be null
        * @param aTID CSP TransactionID
        * @param aMsgType message primitive
        * @param aExpiryTime expiry time
        */
        void DoSendSingleL(
            MImpsDataAccessor* aAc,
            const TDesC& aTID,
            TImpsMessageType aMsgType,
            TInt aExpiryTime = 0 );

        /**
        * Send LogoutRequest primitive
        * @param aTID CSP TransactionID to be used
        */
        void DoSendLogoutL( const TDesC& aTID );

        /**
        * Make response primitive for SAP initiated request
        * @param aTrans request of transaction
        */
        void DoMakeSapResponseL( CTransaction* aTrans );

        /**
        * Handle ClientCapabilityResponse
        * @param aFields received message
        */
        void HandleClientCapabilityResL ( CImpsFields* aFields );

        /**
        * Handle LoginResponse
        * @param aFields received message
        */
        void HandleLoginResponseL ( CImpsFields* aFields );

        /**
        * Send ServiceNegotiationRequest
        */
        void DoSendServiceNegotiationReqL();

        /**
        * Send CreateAttributeListRequest
        */
        void DoSendCreateAttributeListReqL();

        /**
        * Send ClientCapabilityRequest
        */
        void DoSendClientCapabilityReqL ();

        /**
        * Increase poll time
        */
        void IncreasePollTime();

        /**
        * Reset poll time
        */
        void ResetPollTime();

        /**
        * Transport status change indicator
        * @param aConnectionState  new status, on_line, off_line or no_iap
        */
        void DoTransportStatusL( EImpsInternalStatus aConnectionState );

        /**
        * Do the internal logout routines immediately
        * @param aOpId operation-id
        */
        void DoLogoutNow( TInt aOpId );

        /**
        * Start IP based CIR channels
        */
        void DoStartIpCIRL();

        /**
        * Calculates CIR expiration time
        * @return expiry time
        */
        TTime CIRExpiryTime( );

        /**
        * Internal state change
        * @param aNew new status
        */
        void NewState( TImpsCSPState aNew );

        /**
        * Decrese iTr
        */
        void TrMinus();

        /**
        * Reset session settings
        */
        void ResetSession();

        /**
        * Close All.
        * @param aSynch ETrue if connection is closed synchronously immediately
        */
        void CloseAll( TBool aSynch );

        /**
        * Start login cancel
        */
        void StartLoginCancel();

        /**
        * Handle pending logout situation
        */
        void DoPendingLogout();

        /**
        * Current User-id accessor
        * @return UserId
        */
        inline TPtrC DoUserId() const;

        /**
        * Current SAP accessor
        * @return SAP address
        */
        inline TPtrC DoSAP() const;

        /**
        * Is the server logged in
        * @return ETrue if logged in
        */
        inline TBool DoIsLogged() const;

        /**
        * Is the server logged in
        * @return ETrue if logged in
        */
        inline TBool DoIsLogging() const;

        /**
        * Is the CSP session capabilities negotiated
        * @return ETrue if negotiated
        */
        inline TBool DoIsNegotiated() const;

        /*
        * Is engine shutting down
        * @return ETrue if shutting down
        */
        inline TBool DoIsShuttingDown() const;

        /**
        * Logout transaction id accessor
        * @return Logout TID
        */
        inline TPtrC LogoutTID() const;

        /**
        * SAP address accessor
        * @return SAP address
        */
        inline TPtrC SAP() const;

        /**
        * Login TID accessor
        * @return TID
        */
        inline TPtrC LogTid() const;

        /**
        * Login password accessor
        * @return password
        */
        inline TPtrC LogPwd() const;

        /**
        * Is PDP idle closed in ON_LINE?
        * @return ETrue if PDP is closed in CSP ON_LINE
        */
        inline TBool IsPDPIdleOnLine() const;

        /**
        * Is PDP closed in OFF_LINE?
        * @return ETrue if PDP is closed in CSP is OFF_LINE
        */
        inline TBool IsPDPIdleOffLine() const;

        /**
        * Is PDP closed?
        * @return ETrue if PDP is closed
        */
        inline TBool IsPDPIdle() const;

    public:     // Data

    protected:  // Data

    private:    // Data
        CImpsServer&        iServer;
        TImpsCspVersion     iCSPVersion;

        MImpsDecoder*       iDecoder;       // CSP message decoder
        MImpsEncoder*       iEncoder;       // CSP message encoder

        TImpsSrvSettings    iSettings;
        RFs&                iFs;
        CImpsVariant&       iVariant;
        // Pending transaction queue
        TDblQue<CTransaction>    iTransactionList;
        // Buffer for client requests
        TDblQue<CReq>       iRequestList;

        TImpsCSPState       iCSPState;
        TBool               iPendingLogout;

        TBuf<KImpsMaxSID>   iCSPSessionId;  // current CSP session-id
        TBuf8<KImpsMaxSID>  iCSPSessionId8;  // current CSP session-id
        TInt                iLastSent;      // message type
        TInt                iLastReceived;  // message type

        CBufFlat*           iTransportOut;  // transport ouput buffer

        CImpsSendReceive2*  iReceiver2;      // transport callback

        CImpsFields*        iSnd;        // internal data structure
        CImpsFields*        iRcv;        // internal data structure
        CImpsFields*        iAllMessages;// internal data structure
        CImpsIdleTimer*     iIdleTimer;  // Poll timer
        CImpsPDPIdleTimer*  iPDPIdleTimer; // PDP-context Idle Timer
        CImpsPDPOpenTimer*  iPDPOpenTimer; // PDP open re-try timer
        CImpsAliveManager*  iAliveMgr;   // Keep-alive timer
        TInt                iPollTime;
        TInt                iAliveTime;

        CImpsSendQueued*    iSendQ;
        TBool               iPollWasRequested;   // Poll requested by SAP
        HBufC*              iUserId;        // User-ID, may be WV address encoded
        TBuf<KImpsMaxTID>   iTempTid;       // to generate own tid
        TBuf<KImpsMaxTID>   iTempTid2;      // login and negotiation phase tid
        // used also for pending KeepAlive-req
        TBuf<KImpsMaxCookie>  iCookie;      // CSP session cookie for CIR
        TBuf8<KImpsMaxCookie> iCookie8;     // CSP session cookie for CIR

        TImpsServices       iServices;      // Current services

        EImpsInternalStatus iIntStatus;     // Internal status

        TBuf<KImpsMaxClientId>   iLogCID;   // Login client-id

        TInt                iMultiTrans;    // CSP MultiTrans
        TInt                iTr;            // nbr of current transactions

        TImpsMessageType    iMsg;           // Message type in negotiation phase

        TTime               iNegoExpiry;    // Expiry time for negotiation command

        TInt                iLoginPhase;    // Login
        HBufC*              iLogPwd;        // Login password
        HBufC*              iLogTid;        // Login TID
        HBufC*              iKey1;          // private client key
        HBufC*              iKey2;          // private client key
        HBufC*              iTCPAddr;       // CIR TCP address
        TUint               iTCPPort;       // CIR TCP port
        TInt                iUDPPort;       // CIR UDP port
        CImpsCirManager*    iCirManager;
        TBuf<KImpsMaxTID>   iLogoutTID;
        TBool               iConAllowed;
        TBool               iPendingAlive;

        HBufC*              iSAP;           // SAP TCP/IP address (data channel)
        TUint32             iIAP;           // IAP for data channel

        TImpsPollMode       iPollState;     // Send poll after network problem
        // 0=no problem, 1=try once, 2=try in resume
        TInt                iPollInResume;  // Send poll after GPRS resume
        TImpsPDPPend        iPendingPDP;    // Pending PDP open
        TBool               iPendPDPLogout; // PDP has tp open befotre logout msg
        CImpsTidQueue*      iTidSapHistory; // SAP initiated TIDs for validity check
        CImpsTidQueue*      iTidCliHistory; // SAP initiated TIDs for validity check
        TBool               iTcpCIRError;   // If WV 1.2 CIR element is T in received msg
        TBool               iReactive;      // reactive presence auth negotiation
        TBool               iCancelLogin;   // cancel login in progress

#ifdef _FAKE_RESPONSE
        TBuf<KImpsMaxTID>   iTestMsgTid;    // local mode test auxiliary
#endif


    private:    // Friend classes
        // internal module test suite class
        friend class TImpsSrvTestUtils;

    };

#include "ImpsCSPSession.inl"

#endif      // ?INCLUDE_H   

// End of File
