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
* Description: Including CTrReq : transport request buffer element.
*              Including CImpsSendReceive2: This is an adapter between WV engine core and transport dll.
*/


#ifndef CImpsSendReceive2_H
#define CImpsSendReceive2_H

//  INCLUDES
#include <f32file.h>
#include "impscspsessionapi.h"
#include "impsdatachannelapi.h"

// DATA TYPES

// Command types for connection monitor
enum TImpsTransCmd
    {
    EImpsTransNone,
    EImpsTransOpenAP,
    EImpsTransCloseDelay,
    EImpsTransCloseAP,
    EImpsTransWaitOpen
    };

// Connection states
enum TImpsConnectionState
    {
    EImpsOnline = 0,    // active
    EImpsOffline,       // suspend
    EImpsNoAP           // lost or uninitialized
    };

// State machine
enum TImpsTrState
    {
    EImpsTrInit = 0,
    EImpsTrAPOpening,
    EImpsTrAPOpen,
    EImpsTrClosing,
    EImpsTrClosingPDP,  // close PDP context only, not bearer events.
    EImpsTrCloseDelayed,
    EImpsTrClosedPDP,
    EImpsTrOpeningPDP,
    EImpsTrDataOpen
    };


// FORWARD DECLARATIONS
class CImpsConnManager;


/**
*  transport request buffer element
*/
class CTrReq : public CBase
    {
    public:
        CTrReq(
            const TDesC& aTID,
            TInt aOpId );
        virtual ~CTrReq();
        void Destroy();

        TInt                iOpId;  //lint !e1925
        TBuf<KImpsMaxTID>   iTID;   //lint !e1925 // CSP TID
        TDblQueLink         iLink;  //lint !e1925
    };

/**
*  This is an adapter between WV engine core and transport dll.
*/
class CImpsSendReceive2 : public CBase, public MImpsDataReceiver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aFs file server
        * @param aServer WV engine core
        * @param aOutputBuffer output buffer
        * @param aMimeType MIME content type to be added in HTTP headers.
        */
        static CImpsSendReceive2* NewL(
            RFs&  aFs,
            MImpsCSPSession& aServer,
            CBufFlat& aOutputBuffer,
            TDesC8& aMimeType );

        /**
        * Destructor.
        */
        virtual ~CImpsSendReceive2();

    public: // New functions

        /**
        * Opens a session to WV SAP/HTTP proxy.
        * If already opened, error code in status is returned in leave.
        * @param aID operation id to identify the request.
        *             Used in error situations.
        *             Zero length id means that data channel is opened only.
        * @param aXmlLog if xml messages are written to log
        *        +1 logging is on, +2 log also polling
        * @param aSAP WV SAP
        * @param aIAP CommDb IAP id
        * @param aDelayed ETrue if open AP must be delayed a little bit.
        */
        void OpenL(
            const TDesC& aTID,
            TInt  aXmlLog,
            const TDesC& aSAP,
            TInt  aIAP,
            TBool aDelayed = EFalse );

        /**
        * Closes the Data channel to WV SAP/HTTP proxy and AP
        */
        void CloseTr();

        /**
        * Closes the session to WV SAP/HTTP proxy.
        * Yields the control for active scheduler until performs the operation.
        * @return error code
        */
        TInt Close2( );

        /**
        * Close IAP only. GPRS state changes are still received thru
        * callback method.
        */
        void ClosePDP();

        /**
        * Open PDP and data channel only
        * @param aSAP WV SAP
        * @param aIAP CommDb IAP id
        */
        void OpenPDPL(
            const TDesC& aSAP,
            TInt  aIAP );

        /**
        * Send a request and waits a response.
        * Cleans current entry from iRequList in error case so that
        * DeleteId is not needed.
        * @param aTID transaction id to identify the request.
        *            Used in error situations.
        * @param aExpiryTime expiry time
        */
        void SendAndWaitL( const TDesC& aTID, TInt aExpiryTime = 0 );

        /**
        * Is the connection made
        */
        TBool isConnected();

        /**
        * Cancel transport transactions, affects Data Channel only
        */
        void Cancel();

        /**
         * Number of pending requests
         */
        inline TInt NbrOfPending();

        /**
        * Cancel a transaction
        * @param aTID transaction id to identify the request.
        */
        void CancelTrans( const TDesC& aTID );

        /**
        * Cancel a login transaction
        * This differs from CancelTrans so that this calls
        * CSP session's TransportErrorL callback with specific
        * error codes when ready.
        * @param aTID transaction id to identify the request.
        * @return ETrue if Login transaction was sent and Logout should be sent later.
        */
        TBool CancelLoginTrans( const TDesC& aTID );

    public: // Functions from base classes

        /**
        * InputBuffer accessor
        * @return input buffer to be used for raw 8-bit response data
        */
        CBufFlat& InputBuffer();

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
        * @param HBufC8** Pointer to the raw eight-bit CSP data;
        *                must be deleted by the implementing class
        *                after its contents are no longer needed
        */
        virtual void TransportResponse( TInt aId,
                                        TInt aError,
                                        TInt aHttpStatus,
                                        HBufC8* aCspMessage );

        /**
        * Observer for connection status change and completion of
        * open and close operations.
        * @param aConnectionState new state or command executed
        * @param aStatus error code for the command
        * @param aOperation True if operation response (open or close)
        * @param aAuthClose True if authoritatively closed
        */
        void APStatusEvent(
            TImpsConnectionState aConnectionState,
            TInt aStatus,
            TBool aOperation,
            TBool aAuthClose = EFalse );

        /**
        * Deletes members while active scheduler is still running
        */
        void Destroy();

        /**
        * Connection Manager accessor
        */
        MMsgConnManager& ConnManagerHandleL();

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        * @param aFs file server
        * @param aServer WV engine core
        * @param aInputBuffer input buffer
        * @param aOutputBuffer output buffer
        */
        CImpsSendReceive2(
            RFs& aFs,
            MImpsCSPSession& aServer,
            CBufFlat& aOutputBuffer );

        /**
        * Opens a session to WV SAP/HTTP proxy.
        * If already opened, error code in status is returned in leave.
        * Other parameters givnen in previous OpenL methos
        * @param aID operation id to identify the request.
        * @param aDelayed ETrue if open AP must be delayed a little bit
        */
        void OpenL( const TDesC& aTID, TBool aDelayed );

        /**
        * Generate unique transport operation-id
        */
        inline void GenerateId();

        /**
        * SAP address accessor
        * @return SAP address
        */
        inline TPtrC SAP() const;

        /**
        * Create a request element for transport adapter
        * @param aTid transaction-id
        * @return new operation-id
        */
        TInt AddIdL( const TDesC& aTid );

        /**
        * Gives the transaction-id from operation-id
        * @param aId operation-id
        * @return transaction-id
        */
        TPtrC OpidToTid( TInt aId );

        /**
        * Gives operation-id from transaction-id
        * @param aId transaction-id
        * @return operation-id
        */
        TInt TidToOpid( const TDesC& aId );

        /**
        * Deletes the request element
        * @param aId operation-id
        * @return ETrue if found and deleted
        */
        TBool DeleteId( TInt aId );

        /**
        * Delete all request elements
        */
        void DeleteAllIds();

        /**
        * Callback for transport request response
        * @param aId operation-id given in send request, 0 means undefined
        * @param aError status code, KErrNone if successful
        * @param TInt HTTP status code, 200 if successful
        * @param HBufC8** Pointer to the raw eight-bit CSP data;
        *                must be deleted by the implementing class
        *                after its contents are no longer needed
        */
        void DoTransportResponseL(
            TInt aId,
            TInt aError,
            TInt aHttpStatus,
            HBufC8** aCspPtr
        );

        /**
        * Opens a connection and/or data channel
        * @param aCmd command
        * @param IAP, optional
        */
        void Open2L( TInt aInt, TBool aDelayed );

        /**
        * Is the DataChannel OK
        * @return ETrue if DataChannel open
        */
        TBool DoIsConnectedL();

        /**
        * By default constructor is private.
        * @param aMimeType MIME content type to be added in HTTP headers.
        */
        void ConstructL( TDesC8& aMimeType );

        // By default, prohibit copy constructor
        CImpsSendReceive2( const CImpsSendReceive2& );
        // Prohibit assigment operator
        CImpsSendReceive2& operator= ( const CImpsSendReceive2& );

        /**
        * Internal state change
        * @param aNew new status
        */
        void NewTrState( TImpsTrState aNew );

        /**
        * Handle response for our request to transport modules
        * @param aConnectionState new state or command executed
        * @param aStatus error code for the command
        */
        void DoHandleRespL(
            TImpsConnectionState aConnectionState,
            TInt aStatus );

        /**
        * Handle transport event handling
        * @param aConnectionState new state or command executed
        * @param True if authoratively closed, valid only in bearer-lost.
        */
        void DoHandleTrEventL(
            TImpsConnectionState aConnectionState, TBool aAuthClose );

        /**
        * Open data channel
        * @return error code
        */
        TInt DoOpenDataCh();


    private:    // Data
        MImpsCSPSession&    iServer;
        CBufFlat&           iOutputBuffer;
        TBool               iProxy;
        TBool               iXmlLog;
        TInt                iID;
        TInt                iOpenId;    // used for open/close method
        TParse              iParse;
        RFs&                iFs;
        TInt                iIAP;
        MImpsSender*        iSender;
        // Request queue
        TDblQue<CTrReq>     iRequList;

        CImpsConnManager*   iConnMan;
        HBufC*              iSAP;           // TCP/IP address
        // TImpsTransCmd       iPendingCmd;
        TBuf8<50>           iMimeType;
        TImpsTrState        iTrState;
    };

inline TInt CImpsSendReceive2::NbrOfPending()
    {
    return iSender ? iSender->PendingRequests() : 0;
    }

inline void CImpsSendReceive2::GenerateId( )
    {
    iID++;
    }

inline TPtrC CImpsSendReceive2::SAP() const
    {
    return iSAP ? TPtrC( *iSAP ) : TPtrC();
    }



#endif      // INCLUDE_H   

// End of File
