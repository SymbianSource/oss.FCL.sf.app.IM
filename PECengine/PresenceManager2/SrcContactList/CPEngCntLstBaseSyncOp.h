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
* Description: this class handles the operation for performing contact list base synhronization
*
*/

#ifndef CPENGCNTLSTBASESYNCOP_H
#define CPENGCNTLSTBASESYNCOP_H

//  INCLUDES
#include <E32Base.h>
#include <badesca.h>
#include "CPEngAsyncOperation.h"


// FORWARD DECLARATIONS
class MPEngContactListTransactionObserver2;
class MPEngAdvTransactionStatus2;
class CPEngContactListTransaction2;




// CLASS DECLARATION


/**
 * Operation handler for performing contact list
 * base synhronization.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngCntLstBaseSyncOp ) : public CPEngAsyncOperation
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngCntLstBaseSyncOp* NewL( TInt aPriority,
    CPEngContactListTransaction2& aInterface,
    CPEngNWSessionSlotStorageProxy& aUsedSlot,
    MPEngContactListTransactionObserver2& aObserver,
    RPEngManagerClient& aServer,
    TBool aNeedToUnsubscribe );

    /**
     * Destructor.
     */
    virtual ~CPEngCntLstBaseSyncOp();

private:

    /**
     * C++ default constructor.
     */
    CPEngCntLstBaseSyncOp( TInt aPriority,
                           CPEngContactListTransaction2& aInterface,
                           MPEngContactListTransactionObserver2& aObserver,
                           RPEngManagerClient& aServer,
                           TBool aNeedToUnsubscribe );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot );


public: //New services


    /**
     *
     *
     *
     *
     * @since 3.0
     */
    void BaseSync();




private: //Template method from base

    /**
     * Notifies from operation complete.
     */
    void DoHandleOpSuccessL( MPEngAdvTransactionStatus2& aStatus,
                             TInt aTransactionOperation );

    void DoHandleOpFailure( MPEngAdvTransactionStatus2& aStatus,
                            TInt aTransactionOperation );

    TPEngAsyncOpResult DoNotifyObserver( MPEngAdvTransactionStatus2& aStatus,
                                         TInt aTransactionOperation );




private:

    //REF: The implemented interface
    CPEngContactListTransaction2&     iInterface;

    //REF: The observer
    MPEngContactListTransactionObserver2&     iObserver;

    // Does lists need to be subscribed during sync
    TBool iUnsubscribe;

    };

#endif  //CPENGCNTLSTBASESYNCOP_H



