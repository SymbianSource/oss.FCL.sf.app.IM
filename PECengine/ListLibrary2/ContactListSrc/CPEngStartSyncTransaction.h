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
* Description:  Transction handler to do basic synchronization
*
*/

#ifndef __CPENGSTARTSYNCTRANSACTION_H__
#define __CPENGSTARTSYNCTRANSACTION_H__


//  INCLUDES
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"
#include <e32base.h>
#include <badesca.h>



// FORWARD DECLARATIONS
class   MPEngStorageManager;
class   CPEngTransactionStatus;
class   MPEngXMLParser;
class   MPEngContactListTransactionManager;
class   MPEngXMLSerializer;


// CLASS DECLARATION
/**
 * Transction handler to do basic synchronization
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngStartSyncTransaction ):
        public CBase,
        public MPEngOutgoingTransactionHandler
    {
private:

    /**
     * Private enumerations.
     */
    enum TPEngSyncStartState
        {
        EPEngGetListOfLists = 0,
        EPEngUnSubscribe,
        EPEngTransactionCompleted
        };

public:

    /**
     * Two-phased constructor.
     */
    static CPEngStartSyncTransaction* NewLC(
        MPEngContactListTransactionManager& aCntLstSettingsManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId,
        TBool aNeedToUnsubscribe );

    /**
     * Destructor.
     */
    virtual ~CPEngStartSyncTransaction();


public: // Functions from MPEngOutgoingTransactionHandler

    /**
     * Synchronous method, get Outgoing transaction request
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void RequestL( TDes8& aSendBuffer );


    /**
     * Signals to the transaction handler that this it is last one
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
    TBool TransactionCompleted();


    /**
     * Gets transaction status result class
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    CPEngTransactionStatus* TransactionResult();


    /**
     * Releases the handler
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void ReleaseHandler();



private: ///New functions

    /**
     * Create XML message to retrieve all contact lists of the wv server
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    void GetXMLGetListOfContactListsL( MPEngXMLSerializer& aXmlSerializer );

    /**
     * Create XML message to unsubscribe all contact lists of the wv server
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    void GetXMLUnsubscribeAllL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Parse XML Get list of server contact lists response
     *
     * @since 3.0
     * @param  aResponse response buffer
     * @return aXMLparser XML parser
     */
    void ParseGetListOfListsResponseL( const TDesC8& aResponse,
                                       MPEngXMLParser* aXMLparser );

    /**
     * Parse XML Unsunbscribe all response
     *
     * @since 3.0
     * @param  aResponse response buffer
     * @return aXMLparser XML parser
     */
    void ParseUnsubscreAllResponseL( const TDesC8& aResponse,
                                     MPEngXMLParser* aXMLparser );

private: // Constructors

    /**
     * C++ constructor.
     */
    CPEngStartSyncTransaction(
        MPEngContactListTransactionManager& aCntLstSettingsManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId,
        TBool aNeedToUnsubscribe );


    /**
     * Symbian constructor
     */
    void ConstructL();


private:  // Data

    // REF: Contact list manager
    MPEngContactListTransactionManager&     iContactListManager;

    // OWN: Transaction status class
    CPEngTransactionStatus*                 iTransactionStatus;

    // OWN: State of the start of the synchronization
    TInt                                    iSynchronizationState;

    // OWN: List of server contact lists
    CDesC16ArrayFlat                        iServerCntLists;

    /// OWN: Server default contact list
    HBufC*                                  iDefaultCntList;

    /// OWN: CPS Version
    TPEngWVCspVersion&                      iCSPVersion;

    /// Operation Id
    TInt                                    iOperationId;

    //
    TBool                                   iUnsubscribe;
    };

#endif      //  __CPENGSTARTSYNCTRANSACTION_H__

//  End of File

