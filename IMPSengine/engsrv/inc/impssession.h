/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Classes used to describe IMSP CSP session.
*              Major for Imps Session.
*/


#ifndef CImpsSession_H
#define CImpsSession_H

//  INCLUDES
#include <e32base.h>
#include "impsclientsrv.h"

// FORWARD DECLARATIONS
class CImpsServer;
class CImpsSubSession;
class CImpsVariant;

// CLASS DECLARATION

/**
*  Common event for all client session types
*/
class CWvEvent : public CBase    // a notify event
    {
    public:
        CWvEvent( TImpsEventType aType );
        ~CWvEvent();
        void Destroy();

        TImpsEventType      iType;      // notification type, see impsconst.h
        TImpsServRequest    iRequestType;
        TImpsMessageType    iMessageType;  // response message
        TInt                iReqMesType;   // request type
        TBool               iSent;      // sent to client
        TInt                iOpCode;    // op-id
        TInt                iStatus;
        TInt                iAux;       // auxiliary for small data
        HBufC8*             iPackedMessage;
        TDblQueLink         iLink;
    };

/**
*  Common request data for all service session types
*/
class CRequest : public CBase
    {
    public:
        CRequest(
            const TDesC& aTID,
            TInt aOpId,
            TImpsServRequest aRequestType,
            TTime aExpiry,
            TImpsMessageType aMessageType );
        ~CRequest();
        void Destroy();

        TBuf<KImpsMaxTID> iTID;          // CSP TID
        TInt             iOpId;
        TImpsServRequest iRequestType;
        TImpsMessageType iMessageType;
        TTime            iExpiry;
        TDblQueLink      iLink;
    };


/**
*  SOS Client-server framework session for WV engine
*/
class CImpsSession : public CSession2
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aClient client thread
        */
        CImpsSession( RMessage2& aMsg );

        /**
        * Destructor.
        */
        virtual ~CImpsSession();

    public: // New functions

        /**
        * Handles a client request; tests the opcode and gives
        * service according to that. ServiceL calls this.
        * @param aMessage service request from client
        */
        void DispatchMessageL( const RMessage2& aMessage );

        /**
        * Search pending request using CSP transaction-id
        * @param aTid transaction-id
        * @param aOpId op-id if matching one found (OUT)
        * @param aRequestType request type if found (OUT)
        * @param aReqMsgType request CSP primitive if found (OUT)
        * @param aCSP CSP connection id
        * @return subsession handle if search match
        */
        TUint CheckRequests(
            const TDesC& aTid,
            TInt& aOpId,
            TImpsServRequest& aRequestType,
            TImpsMessageType& aReqMsgType,
            TImpsSessIdent aCSP );

        /**
        * Discard expired pending requests and send error event to client
        * @param aExpiryTime expiry time interval
        * @param aServiceType service type considered
        * @param aSess CSP session entity
        */
        void DiscardRequests(
            TTime aExpiryTime,
            TImpsEventType aServiceType,
            TImpsSessIdent aCSP,
            MImpsCSPSession* aSess );

        /**
        * Discard expired pending requests and send error event to client
        * @param aError error code to be sent
        * @param aServiceType service type considered
        * @param aSess CSP session entity
        */
        void DiscardRequests(
            TInt aError,
            TImpsEventType aServiceType,
            TImpsSessIdent aCSP,
            MImpsCSPSession* aSess );

        /**
        * Discard a pending request and send an error event to client
        * @param aTid transaction-id
        * @param aServiceType service type considered
        * @param aCode error code
        * @param aSess CSP session entity
        * @return ETrue if request found and deleted
        */
        TBool DiscardRequest(
            const TDesC& aTid,
            TImpsEventType aServiceType,
            TInt aCode,
            TImpsSessIdent aCSP );

        /**
        * Minimum expiry time accessor
        * @return expiry time of subsession in seconds
        */
        TInt ExpiryTime();


        /**
        * Send or buffer new event to client
        * @param aFields data to be sent
        * @param aOpId corresponding op-id
        * @param aRequestType request type
        * @param aReqMsgType requesting CSP primitive starting
        *        transaction if this client has initiated it.
        * @param aHandle subsession handle
        */
        void SendEvent(
            CImpsFields* aFields,
            TInt aOpId,
            TImpsServRequest aRequestType,
            TImpsMessageType aReqMsgType,
            TUint aHandle );

        /**
        * Send Logout event only for Access clients (=session type)
        * @param aRespStatus response status caused this
        * @param aOpId op-id, optional
        * @param aSub subsession that is ignored.
        * @param aHandle subsession that is handled only.
        */
        void SendLogoutEvent(
            TInt aRespStatus,
            TInt aOpId,
            CImpsSubSession* aSub,
            TUint aHandle );

        /**
        * Send or buffer new status event to client
        * @param aStatus new status
        */
        void SendEvent(
            EImpsInternalStatus aStatus );

        /**
        * Check if notification should be sent when TID not given
        * This means usually SAP originated transaction.
        * @param aFields data received from SAP.
        * @param aCSP CSP connection id
        * @return ETrue if matching client found (client-id and
        * media type ok)
        */
        TBool CheckNotifications( CImpsFields* aFields, TImpsSessIdent aCSP );

        /**
        * Check all orphan messages from engine
        */
        void HandleAllOrphans();

        /**
        * Close the session
        */
        void CloseSession();

        /**
        * Delete all waiting client requests.
        */
        void DeleteAllRequests( );

        /**
        * Delete all waiting client requests.
        * @param aCSP CSP connection indetification
        */
        void DeleteAllRequests( TImpsSessIdent* aCSP );

        /**
        * Delete a specified client request
        * @param aOpId corresponding op-id
        * @param aSubHandle subsession handle
        */
        void DeleteRequest( TInt aOpId, TUint aSubHandle );

        /**
        * ApplicationId accessor
        * @return ApplicationId
        */
        TPtrC ApplicationId();

        /**
        * Complete the pending message and set the session status
        * @param aStatus completation status
        */
        void CompleteMe( TInt aStatus );

        /**
        * Reads the 8-bit data from client to the buffer
        * @param aIndex pointer to the message index
        * @param aBuffer target buffer in server memory.
        *        Buffer is reallocated if necessary.
        * @return ETrue if new buffer is allocated
        */
        TBool ReadBuffer8L(
            TInt aIndex,
            HBufC8*& aBuffer );

        /**
        * Delete a subsession
        * @param aHandle subsession id
        */
        void DeleteSubSession( TUint aHandle );

        /**
        * Check if CSP session connection matches
        * @param aCSP CSP connection indetification
        * @return aTrue if matches
        */
        TBool MatchSession( TImpsSessIdent aCSP );

        /**
        * Indicate that the message request has been completed
        */
        inline void SetCompleted();

        /**
        * Server accessor, wrapper for CSharableSession::Server()
        * @return current server instance
        */
        inline CImpsServer* CImpsSession::Server();  //lint !e1511

        /**
        * Get current message of this session
        * That's for CImpsServer only
        * @return last data sent by client API
        */
        inline CImpsFields* ImpsFields() const;

        /**
        * Get buffer for streamed message
        * @return buffer
        */
        inline HBufC8* StreamBuf() const;

        /**
        * Get buffer for streamed message
        * @return buffer
        */
        inline HBufC8** StreamBufAddr();

        /**
        * Create new data structure without deleting old one.
        * That's for a server if it has to queue current request.
        */
        void NewFieldsL();

        /**
        * Is this CIR watcher session
        * @return ETrue this is CIR watcher session.
        */
        inline TBool IsCIRWatcherSession() const;

        /**
        * Subsession (service) types of this session
        * @return event type bit mask
        */
        inline TInt Types() const;

        /**
        * Number of subsessions
        */
        inline TInt NbrSubs() const;

        /**
        * Current message
        */
        inline RMessagePtr2 Message() const;

        // multi:
        /**
        * user-id mutator
        * @param aId user-id
        */
        void SetUserIdL( const TDesC& aId );

        /**
        * Clean CSP session-id
        */
        void CleanSID();

        /**
        * CSP Session-id mutator
        * @param aSID session-id
        */
        void ModifySIDL( const TDesC& aSID );

        /**
        * CSP Session-id accessor
        * @return aId user-id
        */
        inline TPtrC SID( ) const;

        /**
        * client-id accessor
        * @return aId user-id
        */
        inline TPtrC UserId( ) const;

        /**
        * SAP mutator
        * @param aId SAP address
        */
        void SetSAPL( const TDesC& aId );

        /**
        * SAP accessor
        * @return aId user-id
        */
        inline TPtrC SAP( ) const;

        /**
        * Is this session logged in
        */
        inline TBool IsThisLogged() const;


    public: // Functions from base classes

        /**
         * From CSharableSession (pure virtual)
         * Handles service request messages from clients.
         * @param aMessage message from client
         */
        void ServiceL( const RMessage2& aMessage );

    protected:  // Functions from base classes

        /**
        * From CSharableSession::CreateL
        * @param aServer WV engine server core and soul
        */
        // void CreateL( const CServer& aServer );
        void CreateL();

    private: // New functions

        /**
        * New subsession creation
        * @param aType subsession type
        * @param aMessage client request
        */
        void NewSubSessionL(
            TImpsEventType aType,
            const RMessage2& aMessage );

        /**
        * Subsession handle accessor
        * @param aHandle 4th client-server parameter
        * @return Subsession entity
        */
        CImpsSubSession* ConnectionFromHandle( TUint aHandle );

        /**
        * Panics the client thread
        * @param aPanic panic reason
        */
        void PanicClient( TImpsPanic aPanic ) const;

        /**
        * Current service tree
        * Sends current negotiated service tree to a client thread.
        */
        void CurrentServicesL();

        /**
        * Push message generation
        */
        void PushMessageL();

        /**
        * set this client session inactive, i.e. messages should not be sent
        * Login request makes this session active again althogh creation
        * of new session is preferred way to make a new CSP login.
        */
        void SetThisInactive();

        /**
        * Assign the application id
        */
        void AssignIdL();

        // By default, prohibit copy constructor
        CImpsSession( const CImpsSession& );
        // Prohibit assigment operator
        CImpsSession& operator= ( const CImpsSession& );

        /**
        * Is this client session active, i.e. messages should be sent
        * @return ETrue if this is active
        */
        inline TBool IsThisActive() const;

        /**
        * CSP session specific client-server session counter
        * @return number of sessions
        */
        TInt NbrOfSessionsL( );


    private:    // Data
        TBool               iCanceled;

        HBufC*              iApplicationId;  // application id
        HBufC*              iSID;       // CSP Session-id
        HBufC*              iSAP;
        HBufC*              iUserId;    // CSP user-id

        TBuf<KImpsMaxTID>   iLogoutTID;

        HBufC8*             iStream;   // message stream between client->server
        CImpsFields*        iFields;   // current message in unpacked form
        TBool               iMessageCompleted;  // if the session is not active

        TBool               iCIRSess;   // Is this CIR watcher session

        CObjectIx*          iSubSessions; // object index which stores subsessions
        CObjectCon*         iContainer;   // object container for this session
        TInt                iResourceCount;
        TUint               iTypes; // bit mask of supported services

        RMessagePtr2        iMsgR;      // current client-server request
    };

#include "ImpsSession.inl"

#endif

// End of File
