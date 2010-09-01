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
* Description: this class is an operation handler for publishing user own attributes.
*
*/

#ifndef CPENGATTRIBUTEPUBLISHOP_H
#define CPENGATTRIBUTEPUBLISHOP_H

//  INCLUDES
#include <E32Base.h>
#include <badesca.h>
#include "CPEngAsyncOperation.h"


// FORWARD DECLARATIONS
class MPEngAttributeTransactionObserver2;
class MPEngAdvTransactionStatus2;
class CPEngAttributeTransaction2;
class MPEngPresenceAttrModel2;
class MPEngPresenceAttrManager;




// CLASS DECLARATION


/**
 * Operation handler for publishing user own attributes.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributePublishOp ) : public CPEngAsyncOperation
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngAttributePublishOp* NewL( TInt aPriority,
    CPEngAttributeTransaction2& aInterface,
    CPEngNWSessionSlotStorageProxy& aUsedSlot,
    RPointerArray<MPEngPresenceAttrModel2>& aModels,
    MPEngAttributeTransactionObserver2& aObserver,
    RPEngManagerClient& aServer );

    /**
     * Destructor.
     */
    virtual ~CPEngAttributePublishOp();

private:

    /**
     * C++ default constructor.
     */
    CPEngAttributePublishOp( TInt aPriority,
                             CPEngAttributeTransaction2& aInterface,
                             MPEngAttributeTransactionObserver2& aObserver,
                             RPEngManagerClient& aServer );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot,
                     RPointerArray<MPEngPresenceAttrModel2>& aModels );


public: //New services


    /*
     * Starts the publish operation.
     * Takes the ownership in construction
     * given attribute models.
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
    CPEngAttributeTransaction2&     iInterface;

    //REF: The observer
    MPEngAttributeTransactionObserver2&     iObserver;

    //OWN: Models that are published
    RPointerArray< MPEngPresenceAttrModel2 >    iModels;

    //OWN: Are the attribute models in previous array owned?
    TBool iModelsOwned;

    //OWN: Attribute manager
    MPEngPresenceAttrManager*   iAttrManager;


    };

#endif  // CPENGATTRIBUTEPUBLISHOP_H



