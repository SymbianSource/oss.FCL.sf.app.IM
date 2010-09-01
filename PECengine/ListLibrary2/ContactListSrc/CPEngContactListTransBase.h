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
* Description:  Base class for contact list create and synchronize handlers
*
*/

#ifndef __CPENGCONTACTLISTTRANSBASE_H__
#define __CPENGCONTACTLISTTRANSBASE_H__

//  INCLUDES
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"
#include <e32base.h>
#include <badesca.h>


// FORWARD DECLARATIONS
class   CPEngTransactionStatus;
class   MPEngXMLParser;
class   CPEngContactListModBase;
class   CPEngSessionSlotId;
class   MPEngXMLSerializer;
class   MPEngContactListTransactionManager;


// CLASS DECLARATION

/**
 * Base class for contact list create and synchronize handlers
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListTransBase ):
        public CBase,
        public MPEngOutgoingTransactionHandler
    {
protected:  // Enumerations

    /**
     * Contact list synhronization state.
     */
    enum TPengCntListSyncState
        {
        EPEngCntListCreate,
        EPEngCntListFetch,
        EPEngSubscribeCntList,
        EPEngTransactionCompleted
        };


public:

    /**
     * Destructor.
     */
    virtual ~CPEngContactListTransBase();


protected: // Constructors

    /**
     *  C++ constructor.
     */
    CPEngContactListTransBase( CPEngContactListModBase& aContactList,
                               CPEngSessionSlotId& aSessionSlotId,
                               MPEngContactListTransactionManager& aManager,
                               TPEngWVCspVersion& aCSPVersion,
                               TInt aOperationId );


    /**
     * Symbian constructor
     */
    void ConstructL();


public: // From MPEngOutgoingTransactionHandler

    /**
     * Synchronous method, get Outgoing transaction request
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void RequestL( TDes8& aSendBuffer );


    /**
     * Signals to the transaction handler that this it is last
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void LastRunningTransactionHandler();


    /**
     * Process the response to the request.
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void ProcessResponseL( const TDesC8& aResponse,
                           TRequestStatus& aStatus );

    /**
     * Cancels asynchronous processing of the request
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void CancelProcessing();


    /**
     * Support for simultaneous transaction handling
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void NewTransactionHandlersL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers );

    /**
     * Function to signal completing of the transaction
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    TBool TransactionCompleted( );


    /**
     * Gets transaction status result class
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    CPEngTransactionStatus* TransactionResult( );


    /**
     * Releases the handler
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void ReleaseHandler();


protected: // New functions


    /**
     * Template function to create XML message to create contact list
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    virtual void DoGetXMLCreateCntListL(
        MPEngXMLSerializer& aXmlSerializer ) = 0;

    /**
     * Template function to create XML message to fetch contact list
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    virtual void DoGetXMLFetchCntListL(
        MPEngXMLSerializer& aXmlSerializer ) = 0;


    /**
     * Template function to parse XML message from creating contact list
     *
     * @since 3.0
     * @param aResponse XML request response buffer
     * @param aXMLparser XML parser
     */
    virtual void DoParseCreateCntListResponseL(
        const TDesC8& aResponse,
        MPEngXMLParser& aXMLparser ) = 0;


    /**
     * Template function to parse XML message from fetching contact list
     *
     * @since 3.0
     * @param aResponse XML request response buffer
     * @param aXMLparser XML parser
     */
    virtual void DoParseFetchCntListResponseL(
        const TDesC8& aResponse,
        MPEngXMLParser& aXMLparser ) = 0;

    /**
     * Complete contact list transaction,
     * update contact list properties
     *
     * @since 3.0
     */
    void CompleteCntListTransactionL();



private: // New functions

    /**
     * Create XML message to subscribe contact
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    void GetXMLSubscribeListL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Parse Subscribe contact list response
     *
     * @since 3.0
     * @param aResponse XML request response buffer
     * @param aXMLparser XML parser
     */
    void ParseSubscribeListResponseL( const TDesC8& aResponse,
                                      MPEngXMLParser& aXMLparser );


protected:  // Data


    /// OWN: Contact list model
    CPEngContactListModBase&            iContactList;

    /// REF: Session slot
    CPEngSessionSlotId&                 iSessionSlotId;

    /// REF: Contact List Transaction manager
    MPEngContactListTransactionManager& iManager;

    /// OWN: Transaction status class
    CPEngTransactionStatus*             iTransactionStatus;

    /// OWN: State of the presence contact list synchronization
    TPengCntListSyncState               iContactListSyncState;

    /// REF: CPS Version
    TPEngWVCspVersion&                  iCSPVersion;

    /// OWN: Operation Id
    TInt                                iOperationId;
    };

#endif      //  __CPENGCONTACTLISTTRANSBASE_H__

//  End of File

