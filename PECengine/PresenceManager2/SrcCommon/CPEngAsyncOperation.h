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
* Description:  Asynchronous operation base class.
*
*/

#ifndef CPENGASYNCOPERATION_H
#define CPENGASYNCOPERATION_H

//  INCLUDES
#include "RPEngManagerClient.h"
#include "RPEngTransferClient.h"
#include "PEngPresenceManagerDefs.h"

#include <E32Base.h>



// FORWARD DECLARATIONS
class MPEngAdvTransactionStatus2;
class MPEngAsyncOperationOwner;
class CPEngTransactionStatus;
class CPEngNWSessionSlotStorageProxy;


// CLASS DECLARATION

/**
 * Asynchronous operation base class.
 *
 * Provides:
 * - adding to scheduler
 * - protection against nested canceling
 * - operation owning services
 *
 *  @lib
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngAsyncOperation ): public CActive
    {
public:  // destructor

    /**
     * Destructor.
     */
    virtual ~CPEngAsyncOperation();

protected: //Constructor

    /**
     * Constructor.
     */
    CPEngAsyncOperation( TInt aPriority,
                         RPEngManagerClient& aServer );


    /**
     * Symbian OS base class constructor.
     */
    void BaseConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot );


public:  // New external services


    /**
     * Sets the operation owner.
     * Owner can be set just once.
     */
    void SetOwner( MPEngAsyncOperationOwner& aOperationOwner );

    /**
     * Cancels the operation and deletes it
     */
    void CancelOpD();



protected:  // Services for derived implementations

    /**
     *
     */
    void InitTransaction( HBufC16* aTransactionData,
                          TInt aTransactionOperation );

    /**
     *
     */
    void IssueTransaction();

    /**
     * Completes the operation with given error.
     */
    void CompleteWithError( TInt aErr );



protected:  // Template methods for derived implementations

    /**
     * Notifies derived implementation from
     * operation step complete or failure with error.
     */
    virtual void DoHandleOpSuccessL( MPEngAdvTransactionStatus2& aStatus,
                                     TInt aTransactionOperation ) = 0;

    virtual void DoHandleOpFailure( MPEngAdvTransactionStatus2& aStatus,
                                    TInt aTransactionOperation ) = 0;

    virtual TPEngAsyncOpResult DoNotifyObserver( MPEngAdvTransactionStatus2& aStatus,
                                                 TInt aTransactionOperation ) = 0;




private: //From CActive

    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );



protected:  // Data

    //REF: Operation owner
    MPEngAsyncOperationOwner*   iOperationOwner;

    //OWN: Is cancel call running?
    TBool   iCancelling;



    //OWN: Transaction operation identifier
    TInt    iTransactionOperation;

    //OWN: Transaction data
    HBufC16*     iTransactionData;



    //OWN: Transaction status container for result
    CPEngTransactionStatus*     iTransactionStatus;

    //REF:
    RPEngManagerClient&     iTransactionServer;

    //OWN:
    RPEngTransferClient     iTransactionClient;

    };

#endif  //CPENGASYNCOPERATION_H

