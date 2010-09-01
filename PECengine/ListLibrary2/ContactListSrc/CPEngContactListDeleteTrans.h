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
* Description:  Handler to delete contact list.
*
*/

#ifndef __CPENGCONTACTLISTDELETETRANS_H__
#define __CPENGCONTACTLISTDELETETRANS_H__


//  INCLUDES
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"
#include <e32base.h>


// CLASS DECLARATION
class MPEngContactListTransactionManager;
class CPEngTransactionStatus;
class MPEngXMLParser;
class CPEngContactListModBase;
class MPEngXMLSerializer;


/**
 * Handler to delete contact list.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListDeleteTrans ) :
        public CBase,
        public MPEngOutgoingTransactionHandler
    {
private:

    /**
     * Private enumerations.
     */
    enum TPEngDeleteTransState
        {
        EPEngUnSubscribeContactList = 0,
        EPEngDeleteContactList,
        EPEngTransactionCompleted
        };


public:

    /**
     * Two-phased constructor.
     */
    static CPEngContactListDeleteTrans* NewLC(
        CPEngContactListModBase& aContactList,
        MPEngContactListTransactionManager& aSettingsManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListDeleteTrans();


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
    TBool TransactionCompleted();


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



private:    // New functions

    /**
     * Get XML request to unsubscribe contact list
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    void GetXMLUnsubscribeContactListL( MPEngXMLSerializer& aXmlSerializer );

    /**
     * Get XML request to delete contact list
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     */
    void GetXMLDeleteContactListL( MPEngXMLSerializer& aXmlSerializer );


private: // Constructors

    /**
     * C++ constructor.
     */
    CPEngContactListDeleteTrans(
        CPEngContactListModBase& aContactList,
        MPEngContactListTransactionManager& aSettingsManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );


    /**
     * Symbian constructor.
     */
    void ConstructL();

private:    // Data

    /// OWN: Contact list model
    CPEngContactListModBase&                iContactList;

    /// REF: Manager of the contact lists
    MPEngContactListTransactionManager&     iSettingsManager;

    // OWN: State of the transaction handler
    TInt                                    iTransactionState;

    // OWN: Transaction status class
    CPEngTransactionStatus*                 iTransactionStatus;

    /// REF: CSP Version
    TPEngWVCspVersion&                      iCSPVersion;

    /// OWN: Operation Id
    TInt                                    iOperationId;

    };

#endif      //  __CPENGCONTACTLISTDELETETRANS_H__




//  End of File

