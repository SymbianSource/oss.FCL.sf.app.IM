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
* Description: this class is an operation handler for performing attribute
* list publish operation.
*
*/

#ifndef CPENGATTRLSTPUBLISHSYNCOP_H
#define CPENGATTRLSTPUBLISHSYNCOP_H

//  INCLUDES
#include <E32Base.h>
#include <badesca.h>
#include "CPEngAsyncOperation.h"


// FORWARD DECLARATIONS
class MPEngAttributeListTransactionObserver2;
class MPEngAdvTransactionStatus2;
class CPEngAttributeListTransaction2;



// CLASS DECLARATION


/**
 * Operation handler for performing attribute
 * list publish operation.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttrLstPublishSyncOp ) : public CPEngAsyncOperation
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngAttrLstPublishSyncOp* NewL(
        TInt aPriority,
        CPEngAttributeListTransaction2& aInterface,
        CPEngNWSessionSlotStorageProxy& aUsedSlot,
        MPEngAttributeListTransactionObserver2& aObserver,
        RPEngManagerClient& aServer );

    /**
     * Destructor.
     */
    virtual ~CPEngAttrLstPublishSyncOp();

private:

    /**
     * C++ default constructor.
     */
    CPEngAttrLstPublishSyncOp(
        TInt aPriority,
        CPEngAttributeListTransaction2& aInterface,
        MPEngAttributeListTransactionObserver2& aObserver,
        RPEngManagerClient& aServer );

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
    void Publish();




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
    CPEngAttributeListTransaction2&     iInterface;

    //REF: The observer
    MPEngAttributeListTransactionObserver2&     iObserver;

    };

#endif  //CPENGATTRLSTPUBLISHSYNCOP_H



