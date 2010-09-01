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
* Description: Class for describing imps sub session including Auxiliary class
* 
*/


#ifndef CImpsSubSession_H
#define CImpsSubSession_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "impsclientsrv.h"

// FORWARD DECLARATIONS
class CImpsServer;
class CImpsSession;
class CImpsVariant;
class CWvEvent;
class CRequest;

// CONSTANTS
const TInt KImpsMaxBuffered = 10;   // max nbr of queued events

// CLASS DECLARATION

/**
*  Auxiliary class to store OOM error op-id
*/
class TImpsOOMError
    {
    public:

        TImpsOOMError();

        void SetOpId( TInt aOpId );

        TInt OpId();

        void SetAsSent();

        TBool IsSent();

    private:
        TInt    iOpId;
        TBool   iSent;
    };

/**
*  Auxiliary class to store OOM error op-id:s
*/
class TImpsOOMErrors
    {

    public:
        /**
        * Store error
        * @param aOpCode op-id that has failed
        */
        void StoreOOM( TInt aOpCode );

        /**
        * Get next OOM error and set is as a sent message.
        * @return op-id, 0 if not found any.
        */
        TInt GetOOM();

        /**
        * Remove the sent OOM error
        * @return op-id, 0 if not found any.
        */
        TInt RemoveOOM();

        /**
        * Clean opid codes
        */
        void Reset();

        /**
        * Check if there are OOM erros in the queue
        * @return ETrue if there are such errors
        */
        TBool Exists();

    private:
        TInt iR;
        TInt iW;
        TFixedArray<TImpsOOMError, KImpsMaxBuffered> iOOMList;

    };

/**
*  SOS Client-server framework sub-session for WV engine
*/
class CImpsSubSession : public CObject
    {
    public:  // Constructors and destructor

        /**
        * Constructor
        * @param aSession client-server parent session
        * @param aType subsession type
        * @param aMessage service request from client
        * @return subsession entity
        */
        static CImpsSubSession* NewL(
            CImpsSession* aSession,
            TImpsEventType aType,
            const RMessage2 aMessage );

        /**
        * Destructor.
        */
        virtual ~CImpsSubSession();

    public: // New functions

        /**
        * Assign new id, that causes orphan message check.
        * @param sCSP CSP connection id
        */
        void AssignIdL( TImpsSessIdent aCSP );

        /**
        * Causes orphan message check.
        * @param sCSP CSP connection id
        */
        void HandleAllOrphans( TImpsSessIdent aCSP );

        /**
        * Search pending request using CSP transaction-id
        * @param aTid transaction-id
        * @param aOpId op-id if matching one found (OUT)
        * @param aRequestType request type if found (OUT)
        * @param aReqMsgType request CSP primitive if found (OUT)
        * @return ETrue if search match
        */
        TBool CheckRequests(
            const TDesC& aTid,
            TInt& aOpId,
            TImpsServRequest& aRequestType,
            TImpsMessageType& aReqMsgType );

        /**
        * Discard expired pending requests and send error event to client
        * @param aExpiryTime expiry time interval
        * @param aServiceType service type considered
        */
        void DiscardRequests(
            TTime aExpiryTime,
            TImpsEventType aServiceType,
            MImpsCSPSession* aSess );

        /**
        * Discard all pending requests and send error event to client
        * @param aError error code to be sent
        * @param aServiceType service type considered
        */
        void DiscardRequests(
            TInt aError,
            TImpsEventType aServiceType,
            MImpsCSPSession* aSess );

        /**
        * Discard a pending request and send error event to client
        * @param aTid transaction-id
        * @param aServiceType service type considered
        * @param aCode error code
        * @return ETrue if request found and deleted
        */
        TBool DiscardRequest(
            const TDesC& aTid,
            TImpsEventType aServiceType,
            TInt aCode );

        /**
        * Send or buffer new event to client
        * @param aFields data to be sent
        * @param aOpId corresponding op-id
        * @param aRequestType request type
        * @param aReqMsgType requesting CSP primitive starting
        *        transaction if this client has initiated it.
        */
        void SendEvent(
            CImpsFields* aFields,
            TInt aOpId,
            TImpsServRequest aRequestType,
            TImpsMessageType aReqMsgType );

        /**
        * Send or buffer new status event to client
        * @param aStatus new status
        */
        void SendEvent(
            EImpsInternalStatus aStatus );

        /**
        * Send Logout event only for Access clients (=session type)
        * @param aRespStatus response status caused this
        * @param aOpId op-id, optional
        */
        void SendLogoutEvent( TInt aRespStatus, TInt aOpId = 0 );

        /**
        * Send Login event only for Access clients (=session type).
        * This can be used when existing CSP session is shared between
        * client sessions.
        * @param aRespStatus response status caused this
        * @param aOpId op-id, optional
        */
        void SendLoginEvent( TInt aRespStatus, TInt aOpId = 0 );

        /**
        * Send OOM error event
        * @return ETrue if OOM event was found and sent.
        */
        TBool SendOOMError();

        /**
        * Check if notification should be sent when TID not given
        * This means usually SAP originated transaction.
        * Takes care of content type limitations and filters
        * new messages off if they are not wanted in this subsession.
        * @param aFields data received from SAP.
        * @return ETrue if data should be sent to a client thread of
        *         this Symbian client-server session.
        */
        TBool CheckNotification( CImpsFields* aFields );

        /**
        * Cancel the subsession.
        * Sends number of pending events to client.
        */
        void Unregister();

        /**
        * Deletes this subsession.
        * Removes this entity finally.
        */
        void DeleteSub();

        /**
        * Delete all pending events
        * @return number of events outstanding
        */
        TInt DeleteAllEvents();

        /**
        * Delete all waiting requests.
        */
        void DeleteAllRequests();

        /**
        * Delete a specified client request
        * @param aOpId corresponding op-id
        * @return ETrue if request found
        */
        TBool DeleteRequest( TInt aOpId );

        /**
        * Login
        * @param aReactive reactive presence auth negotiation
        */
        void LoginL( TBool aReactive );

        /**
        * Logout
        * @param sCSP CSP connection id
        * @param aCancel ETrue if Login cancel functionality wanted
        * @return CSP TID
        */
        TPtrC LogoutL( TImpsSessIdent aCSP, TBool aCancel );

        /**
        * Send CSP Primitive to SAP
        * @param aFunction client-server command
        * @param sCSP CSP connection id
        */
        void SendPrimitiveL( const TInt aFunction, TImpsSessIdent aCSP );

        /**
        * Generate and send GetBlokedlist primitive to SAP
        * @param sCSP CSP connection id
        */
        void SendGetBlockedL( TImpsSessIdent aCSP );

        /**
        * Serve next event request from client
        * @param aMsg next event request
        */
        void NextEventL( const RMessage2& aMsg );

        /**
        * Serve event body request from client
        * @param aMsg next event request
        */
        void SendEventBodyL( const RMessage2& aMsg );

        /**
        * Client cancels transaction.
        * @param aMsg client-server message including opid
        * @param sCSP CSP connection id
        * @return ETrue if request is found and cancelled
        */
        TBool CancelTrans( RMessagePtr2 aMsg, TImpsSessIdent aCSP );

        /**
        * Request expiry time mutator
        * @param aMsg client request
        */
        void SetExpiryTime( RMessagePtr2 aMsg );

        /**
        * Request expiry time mutator
        * @param aVal time in seconds, ignored if not > 0.
        */
        void SetExpiryTime( TInt aVal );

        /**
        * Request expiry time accessor
        * @return request expiry time in seconds
        */
        TInt ExpiryTime();

        /**
        * Indicate that the message request has been completed ???
        */
        inline void SetCompleted();

        /**
        * Get current message of this session
        * That's for CImpsServer only
        * @return last data sent by client API
        */
        inline CImpsFields* ImpsFields() const;

        /**
        * Get buffer for streamed message
        * return buffer
        */
        inline HBufC8* StreamBuf() const;

        /**
        * Get buffer for streamed message
        * return buffer
        */
        inline HBufC8** StreamBufAddr();

        /**
        * Create new data structure without deleting old one.
        * This calls session's NewFieldsL
        */
        inline void NewFieldsL();

        /**
        * Is status observer on/off
        * @return ETrue if client has registered status observer
        */
        inline TBool StatusObserver() const;

        /**
        * Event (service) type of this session
        * @return event type
        */
        inline TImpsEventType Type() const;

        /**
        * iStatusObserver mutator
        * @param ETrue if status observer is set
        */
        inline void SetStatusObserver( TBool aVal );

        /**
        * iDetailedError mutator
        * @param ETrue if detailed error observer is set
        */
        inline void SetDetailedError( TBool aVal );

        /**
        * Handle Mutator
        * @param aVal subsession handle
        */
        inline void SetHandle( TUint aVal );

        /**
        * Handle Accessor
        * @return subsession handle
        */
        inline TUint Handle(  );

        /**
        * Logout operation id accessor
        * @return iLogOpId
        */
        inline TInt LogOpId();

    protected:

        /**
        * Constructor
        * @param aSession client-server parent session
        * @param aType subsession type
        * @param aMessage service request from client
        */
        void ConstructL(
            CImpsSession* aSession,
            TImpsEventType aType,
            const RMessage2 aMessage );

        const RMessagePtr2 Message() const;

        CImpsServer* Server();


    private: // New functions

        /**
        * Send regular event
        * @param aEvent event
        */
        void DoSendBaseEvent( CWvEvent& aEvent );

        /**
        * Send regular event headers to client thread (short message or error)
        * @param aEvent event
        */
        void DoSendEventHeader( CWvEvent& aEvent );

        /**
        * Send event including message body to client thread
        * @param aEvent event
        */
        void DoSendEventBody( CWvEvent& aEvent );

        /**
        * Create event item for IPC
        * @param aEvent event (IN)
        * @param aData IPC data (OUT)
        */
        void DoCreateEventIPC( CWvEvent& aEvent, SImpsEventData* aData );

        /**
        * Write event headers
        * @param aMsg client-server message
        * @param aData serialized IPC data
        * @return error code
        */
        TInt DoWriteEventHeaders( RMessagePtr2 aMsg, const TDes& aData );

        /**
        * Write event body
        * @param aMsg client-server message
        * @param aEvent event including serialized message body
        * @return error code
        */
        TInt DoWriteEventBody( RMessagePtr2 aMsg, CWvEvent& aEvent );

        /**
        * Send next event headers to a client
        */
        void DoNextEvent();

        /**
        * Send event body to a client
        */
        void DoEventBodyL();

        /**
        * Read serialized message data from client thread
        * Converts serialized message to internal data structure.
        */
        void GetWVDataL();

        // By default, prohibit copy constructor
        CImpsSubSession( const CImpsSubSession& );
        // Prohibit assigment operator
        CImpsSubSession& operator= ( const CImpsSubSession& );

        /**
        * Create an event entity and add it to event queue
        * @param aOpId op-id returned if match
        * @param aRequestType request type returned if match
        * @param aReqMesType request message type
        * @param aFields source data to be sent to client thread
        */
        void DoCreateEventL(
            TInt aOpId,
            TImpsServRequest aRequestType,
            TImpsMessageType aReqMesType,
            CImpsFields* aFields );

        /**
        * Create an status event entity and add it to event queue
        * @param aStatus new engine status
        */
        void DoCreateEventL(
            EImpsInternalStatus aStatus );

        /**
        * Create internal error events, like expiry events
        * @param aOpCode op-id
        * @param aStatus status
        * @param aReqMesType requesting CSP primitive
        * @param aRcvMesType received CSP primitive, optional
        * @param aReqType corresponding client-server request, optional
        */
        void DoCreateEventL(
            TInt aOpCode,
            TInt aStatus,
            TImpsMessageType aReqMesType,
            TImpsMessageType aRcvMesType = EImpsMessageNone,
            TImpsServRequest aReqType = EImpsServNone );

        /**
        * Split and convert GroupChnageNotice to
        * HandleNewUsersL, HandleLeftUsersL and HandleGroupEventsL events.
        * @param aOpId op-id returned if match
        * @param aRequestType request type returned if match
        * @param aFields source data to be sent to client thread
        */
        void DoSplitGroupChangeL(
            TInt aOpId,
            TImpsServRequest aRequestType,
            CImpsFields* aFields );

        /**
        * Check if notification should be sent when TID not given
        * This means usually SAP originated transaction.
        * Takes care of content type limitations and filters
        * new messages off if they are not wanted in this subsession.
        * @param aFields data received from SAP.
        * @return ETrue if data should be sent to a client thread of
        *         this Symbian client-server session.
        */
        TBool DoCheckNotificationL( CImpsFields* aFields );

        /**
        * Send online status event to client thread
        * @param aEvent status event
        */
        void DoSendStatusEvent( CWvEvent& aEvent );

        /**
        * Convert CSP error code to IMPS error code used in client API.
        * @param aCSPError CSP error code
        * @return IMPS error code
        */
        TInt ErrorCode( TInt aCSPError );

        /**
        * Event counter
        * @return number of events in queue
        */
        TInt NbrEvents();

        /**
        * Store event msg pointer
        * @param aMsg client-server message
        */
        void StoreEventMsg( RMessagePtr2 aMsg );

        /**
        * Complete event msg pointer and delete it from a queue
        * @param aStatus completion status
        */
        void CompleteEventMsg( TInt aStatus );

        /**
        * Next Message request accessor
        * @return message
        */
        inline RMessagePtr2 EventMsg();

        /**
        * Event buffer size in client thread
        * @return buffer size
        */
        TInt EventMsgBufSize( ) const;

        /**
        * C++ default constructor.
        */
        CImpsSubSession( );

        /**
        * Convert interna status code to a special error code for IPC
        * @param aStatus status code
        * @return error code
        */
        TInt ConvertStatusCode( EImpsInternalStatus aStatus ) const;



    private:    // Data
        TBool               iCanceled;
        TImpsEventType      iServiceType;
        TBool               iHandleNew;

        // Event queue
        TDblQue<CWvEvent>   iEventList;

        // Request queue
        TDblQue<CRequest>   iRequestList;
        TUint               iSubHandle;    // handle of this subsession

        TBool               iStatusObserver; // Status handler on/off
        TBool               iDetailedError;  // Detailed handler on/off

        CImpsSession*       iSession;
        TImpsMessageType    iLastEvent; // Type of last event _created_ excluding
        // status events
        CImpsVariant*       iVariant;
        RMessagePtr2        iEventMsg;   // next message request from client

        TImpsOOMErrors      iOOMErr;
        TInt                iExpiryTime; // request expiration in seconds
    };

#include "impssubsession.inl"


#endif

// End of File
