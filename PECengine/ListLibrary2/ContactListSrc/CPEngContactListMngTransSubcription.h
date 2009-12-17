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
* Description:  Handler to update contact list subscription.
*
*/


#ifndef __CPENGCONTACTLISTMNGTRANSSUBCRIPTION_H__
#define __CPENGCONTACTLISTMNGTRANSSUBCRIPTION_H__

//  INCLUDES
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"
#include <e32base.h>


// FORWARD DECLARATIONS
class CPEngContactListModBase;
class CPEngSessionSlotId;
class CPEngTransactionStatus;
class MPEngXMLSerializer;
class MPEngXMLParser;


// CLASS DECLARATION

/**
 * Handle to update contact list subscription.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListMngTransSubcription ):
        public CBase,
        public MPEngOutgoingTransactionHandler
    {

public:

    /**
     * Two-phased constructor.
     */
    static CPEngContactListMngTransSubcription* NewLC(
        CPEngContactListModBase& aContactList,
        CPEngSessionSlotId& aSessionSlotId,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListMngTransSubcription();


public: // From MPEngOutgoingTransactionHandler

    /**
     * Synchronous method, get Outgoing transaction request
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void RequestL( TDes8& aSendBuffer );

    /**
     * Signals to the transaction handler thst it is last
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
    void ReleaseHandler( );



private:  // New functions

    /**
     * Creates XML message to unsubscribe contact lists
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    void GetXMLUnsubscribeListL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Creates XML message to re-subscribe contact list
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    void GetXMLSubscribeListL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Parse XML Unsubscribe contact list response
     *
     * @since 3.0
     * @param aResponse request response
     * @param aXMLparser XML parser
     */
    void ParseUnsubscribeListResponseL( const TDesC8& aResponse,
                                        MPEngXMLParser& aXMLparser );

    /**
     * Parse XML re-subscribe contact list response
     *
     * @since 3.0
     * @param aResponse request response
     * @param aXMLparser XML parser
     */
    void ParseSubscribeListResponseL( const TDesC8& aResponse,
                                      MPEngXMLParser& aXMLparser );


private: // Constructors

    /**
     * C++ constructor.
     */
    CPEngContactListMngTransSubcription( CPEngContactListModBase& aContactList,
                                         CPEngSessionSlotId& aSessionSlotId,
                                         TPEngWVCspVersion& aCSPVersion,
                                         TInt aOperationId );

    /**
     * Symbian constructor.
     */
    void ConstructL();

private:    // Data

    /// OWN: Contact list model
    CPEngContactListModBase&            iContactList;

    /// REF: ession slot
    CPEngSessionSlotId&                 iSessionSlotId;

    // OWN: Transaction status class
    CPEngTransactionStatus*             iTransactionStatus;

    // OWN: Transaction state
    TBool                               iTransactionCompleted;

    /// REF: CSP Version
    TPEngWVCspVersion&                  iCSPVersion;

    /// OWN: Operation Id
    TInt                                iOperationId;
    };

#endif      //  __CPENGCONTACTLISTMNGTRANSSUBCRIPTION_H__




//  End of File

