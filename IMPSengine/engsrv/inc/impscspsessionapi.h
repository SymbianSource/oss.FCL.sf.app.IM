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
* Description:  This file defines an interface for sending and receiving data
*               from CSP session as well as opening and closing methods.
*
*
*/

#ifndef WV_CSPSESSION_API_H
#define WV_CSPSESSION_API_H

//  INCLUDES
#include "impsservercommon.h"

// FORWARD DECLARATRIONS
class CImpsFields;
class TImpsServices;

// CLASS DECLARATIONS

/**
* MImpsDataReceiver
*
* Abstract interface for handling responses from CSP Sessoin.
*/

class MImpsCSPSession
    {

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
        * @param aExpiryTime expiry time of this request
        * @param aReactive reactive presence auth negotiation
        * @return TID
        */
        virtual TPtrC LoginL(
            const TDesC& aUser,
            const TDesC& aPassword,
            const TDesC& aClientId,
            const TDesC& aSAP,
            TUint32 aAP,
            const TDesC& aKey1,
            const TDesC& aKey2,
            TTime aExpiryTime,
            TBool aReactive ) = 0;

        /**
        * Logout SAP
        * @param aCancel ETrue if login cancel functionaliy wanted
        * @return TID
        */
        virtual TPtrC LogoutL( TBool aCancel ) = 0;

        /**
        * Receive terminal off_line/on_line indication
        * @param aParam True if connections allowed, else False.
        */
        virtual void SetConnAllowed( TBool aParam ) = 0;

        /**
        * CIR message handler. OPa: Both server and MSession.
        * @param aCookie entire session cookie message
        */
        virtual void CirMessageL( const TDesC8& aCookie ) = 0;

        /**
        * Send data from session current data to WV server.
        * Login must have been done.
        * @param aFields data
        * @param aExpiry expiry time in seconds, if 0, then time in gerarated.
        * @param aOwnerCh OUTPUT ETrue is ownership of aFields is changed
        * @return transaction-id
        */
        virtual TPtrC SendDataL(
            CImpsFields* aFields,
            TInt aExpiry,
            TBool& aOwnerChange ) = 0;

        /**
        * Callback for response from transport watcher
        * @param aDataPtr data received. It must be set to NULL after data is consumed.
        */
        virtual void TransportResponseL( HBufC8** aDataPtr ) = 0;

        /**
        * Callback for error from transport watcher
        * @param aTID transaction-id, 0 means undefined
        * @param aError error code. KImpsErrorNoIAP means lost PDP context.
        */
        virtual void TransportErrorL(
            const TDesC& aTID,
            TInt aError ) = 0;

        /**
        * Cancel active data channel requests.
        */
        virtual void CancelData() = 0;

        /**
        * Close All. MServer::TransportStatus( NO_IAP ) is the result.
        */
        virtual void CloseAll() = 0;

        /**
        * Close PDP context only.
        */
        virtual void ClosePDP() = 0;

        /**
        * Open PDP context only.
        * @return error code
        */
        virtual TInt OpenPDP() = 0;

        /**
        * Transport status change indicator
        * @param aConnectionState  new status, on_line, off_line or no_iap
        */
        virtual void TransportStatus(
            EImpsInternalStatus aConnectionState ) = 0;

        /**
        * Get next message to be sent.
        * Validates first the expiry time and discards a message is necessary.
        */
        virtual void GetNextMessageL() = 0;

        /**
        * Send Poll request
        */
        virtual void SendPollL() = 0;

        /**
        * Send KeepAlive request
        * @return TID of KeepAlive request
        */
        virtual TPtrC SendAliveL( ) = 0;

        /**
        * Send KeepAlive request when GPRS resumes
        */
        virtual void SendAliveInResume( ) = 0;

        /**
        * Is the server logging in curretly
        * @return ETrue if log in active
        */
        virtual TBool IsLogging() = 0;

        /**
        * Is the server logged in
        * @return ETrue if logged in
        */
        virtual TBool IsLogged() = 0;

        /**
        * Is the CSP session capabilities negotiated
        * @return ETrue if negotiated
        */
        virtual TBool IsNegotiated() = 0;

        /**
        * Current User-id accessor
        * @return UserId
        */
        virtual TPtrC UserId() = 0;

        /**
        * Current SAP accessor
        * @return SAP
        */
        virtual TPtrC SAP() = 0;

        /**
        * Current CSP Session-id accessor
        * @return Session-id
        */
        virtual TPtrC SID() = 0;

        /**
        * Current CSP Session password accessor
        * @return password
        */
        virtual TPtrC Password() = 0;

        /**
        * active services accessor
        * @return current WV services
        */
        virtual TImpsServices* Services() = 0;

        /*
        * Is engine shutting down
        * @return ETrue if shutting down
        */
        virtual TBool IsShuttingDown() = 0;

        /**
        * Is there pendig login operation
        * @return ETrue if yes
        */
        virtual TBool IsPendingLogin() = 0;

        /**
        * Cancel a transaction. OPa: Server myös.
        * Removes request from server's transaction queue and
        * asks transport to cancel the request too.
        * @param aTID transaction id to identify the request.
        */
        virtual void CancelTrans( const TDesC& aTID ) = 0;

        /**
        * Send so many queued client requests as possible to transport.
        */
        virtual void DoSendAllQueuedL() = 0;

        /**
        * Do the internal logout routines immediately
        */
        virtual void DoLogout( ) = 0;

        /**
        * Ip-Cir-channel opened callback
        */
        virtual void CirChOpened() = 0;

        /**
        * Ip-Cir-channel error callback
        */
        virtual void CirChError() = 0;

        /**
        * Check exiparation of current requests and asks server to
        * remove requests from client sessions if necessary
        * @param sType service types
        * @param aExpiry expiration time for requests
        */
        virtual void CheckExpiry(
            TImpsEventType aType,
            const TTime aExpiry  ) = 0;

        /**
        * Destructor
        */
        virtual void Destroy() = 0;

        /**
        * Maximum parser size
        */
        virtual TInt MaxParserSize() = 0;

        /**
        * Maximum IM message size
        */
        virtual TInt MaxMessageSize() = 0;
    };


#endif      // ?INCLUDE_H   

// End of File
