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
* Description: this class handles the operation for publishing / synhronizing contact lists.
*
*/

#ifndef CPENGCNTLSTPUBLISHSYNCOP_H
#define CPENGCNTLSTPUBLISHSYNCOP_H

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
 * Operation handler for publishing / synhronizing contact lists.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngCntLstPublishSyncOp ) : public CPEngAsyncOperation
    {
public:  // enumerations



public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngCntLstPublishSyncOp* NewL( TInt aPriority,
    CPEngContactListTransaction2& aInterface,
    CPEngNWSessionSlotStorageProxy& aUsedSlot,
    const MDesCArray& aContactListNames,
    TBool aSynchronization,
    MPEngContactListTransactionObserver2& aObserver,
    RPEngManagerClient& aServer );

    /**
     * Destructor.
     */
    virtual ~CPEngCntLstPublishSyncOp();

private:

    /**
     * C++ default constructor.
     */
    CPEngCntLstPublishSyncOp( TInt aPriority,
                              CPEngContactListTransaction2& aInterface,
                              MPEngContactListTransactionObserver2& aObserver,
                              RPEngManagerClient& aServer );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot,
                     const MDesCArray& aContactListNames,
                     TBool aSynchronization );


public: //New services


    /**
     *
     *
     *
     *
     * @since 3.0
     */
    void IssuePublishSync();




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

    };

#endif  //CPENGCNTLSTPUBLISHSYNCOP_H



