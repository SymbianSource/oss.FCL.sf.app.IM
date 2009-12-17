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
* Description:  Handler to get watcher list.
*
*/

#ifndef __CPENGGETWATCHERSTRANSACTION_H__
#define __CPENGGETWATCHERSTRANSACTION_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"


// FORWARD DECLARATIONS
class CPEngTransactionStatus;
class CPEngContactListModBase;



// CLASS DECLARATION
/**
 * Transaction to fetch list of watchers.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngGetWatchersTransaction ):
        public CBase,
        public MPEngOutgoingTransactionHandler
    {
public:

    /**
     * Two-phased constructor.
     */
    static CPEngGetWatchersTransaction* NewLC(
        CPEngContactListModBase& aWatchers,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );
    /**
     * Destructor.
     */
    virtual ~CPEngGetWatchersTransaction();


public: // Functions from MPEngOutgoingTransactionHandler

    /**
     * Synchronous method, get Outgoing transaction request
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void RequestL( TDes8& aSendBuffer );


    /**
     * Signals to the transaction handler that it is last
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void LastRunningTransactionHandler(  );


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


private: // constructor

    /**
     * C++ default constructor.
     */
    CPEngGetWatchersTransaction( CPEngContactListModBase& aWatchers,
                                 TPEngWVCspVersion& aCSPVersion,
                                 TInt aOperationId );

    /**
    * Symbian constructor.
    */
    void ConstructL();

private:    // Data

    /// OWN: flag if transaction is completed
    TBool                           iIsTransactionCompleted;

    /// OWN: transaction status class
    CPEngTransactionStatus*     iTransactionStatus;

    /// OWN: watcher list main array class
    CPEngContactListModBase&        iWatchers;

    /// OWN: CSP Version
    TPEngWVCspVersion&              iCSPVersion;

    /// OWN: Operation Id
    TInt                            iOperationId;

    };

#endif      //  __CPENGGETWATCHERSTRANSACTION_H__

//  End of File

