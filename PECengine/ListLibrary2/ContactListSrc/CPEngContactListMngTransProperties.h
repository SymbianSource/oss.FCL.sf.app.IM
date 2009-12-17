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
* Description:  Contact list properties update handler
*
*/

#ifndef __CPENGCONTACTLISTMNGTRANSPROPERTIES_H__
#define __CPENGCONTACTLISTMNGTRANSPROPERTIES_H__

//  INCLUDES
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"
#include <e32base.h>


// FORWARD DECLARATIONS
class CPEngContactListModBase;
class CPEngTransactionStatus;
class MPEngContactListSettingsManager;
class MPEngContactListTransactionManager;


// CLASS DECLARATION
/**
 * Handler to update the contact list properties
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListMngTransProperties ) :
        public CBase,
        public MPEngOutgoingTransactionHandler
    {
public:

    /**
     * Two-phased constructor.
     */
    static CPEngContactListMngTransProperties* NewLC(
        CPEngContactListModBase& aContactList,
        MPEngContactListTransactionManager& aManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListMngTransProperties();


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
    CPEngTransactionStatus* TransactionResult();


    /**
     * Releases the handler
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void ReleaseHandler();



private: // Constructors

    /**
     * C++ constructor.
     */
    CPEngContactListMngTransProperties(
        CPEngContactListModBase& aContactList,
        MPEngContactListTransactionManager& aManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );


    /**
     * Symbian constructor.
     */
    void ConstructL();


private:    // Data

    /// OWN: Contact List model
    CPEngContactListModBase&                iContactList;

    /// REF: Contact List Transaction manager
    MPEngContactListTransactionManager&     iManager;

    /// OWN: Transaction status class
    CPEngTransactionStatus*                 iTransactionStatus;

    /// OWN: Transaction state
    TBool                                   iTransactionCompleted;

    /// OWN: CSP version
    TPEngWVCspVersion&                      iCSPVersion;

    /// OWN: Operation Id
    TInt                                    iOperationId;
    };

#endif      //  __CPENGCONTACTLISTMNGTRANSPROPERTIES_H__


//  End of File

