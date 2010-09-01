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
* Description: this class is an operation handler for fetching attributes from network.
*
*/


#ifndef CPENGATTRIBUTEFETCHOP_H
#define CPENGATTRIBUTEFETCHOP_H

//  INCLUDES
#include "CPEngAsyncOperation.h"
#include <E32Base.h>
#include <BaMDesCa.h>



// FORWARD DECLARATIONS
class MPEngAttributeTransactionObserver2;
class CPEngAttributeTransaction2;
class MPEngPresenceAttrManager;
class MPEngPresenceAttrModel2;


/**
 * Operation handler for fetching attributes from network.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeFetchOp ) : public CPEngAsyncOperation
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeFetchOp* NewL( TInt aPriority,
    CPEngAttributeTransaction2& aInterface,
    CPEngNWSessionSlotStorageProxy& aUsedSlot,
    TBool aResultAsLocalObjects,
    const MDesCArray& aPresenceIDs,
    const TArray<TUint32>& aTypes,
    MPEngAttributeTransactionObserver2& aObserver,
    RPEngManagerClient& aServer );

    /**
     * Destructor.
     */
    virtual ~CPEngAttributeFetchOp();


private:

    /**
     * C++ default constructor.
     */
    CPEngAttributeFetchOp( TInt aPriority,
                           CPEngAttributeTransaction2& aInterface,
                           MPEngAttributeTransactionObserver2& aObserver,
                           RPEngManagerClient& aServer );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot,
                     const MDesCArray& aPresenceIDs,
                     const TArray<TUint32>& aTypes,
                     TBool aResultAsLocalObjects );




public: //New services


    /**
     * Starts the fetch operation
     *
     * @since 3.0
     */
    void Fetch();


    /**
     *
     *
     * @since 3.0
     */
    TInt GetFetchedAttributes(
        RPointerArray< MPEngPresenceAttrModel2>& aModels );



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



private: //Data

    //REF: The implemented interface
    CPEngAttributeTransaction2&     iInterface;

    //REF: The observer
    MPEngAttributeTransactionObserver2&     iObserver;

    //OWN: Attribute manager
    MPEngPresenceAttrManager*   iAttrManager;

    //OWN: Result models
    RPointerArray<MPEngPresenceAttrModel2>  iResultModels;
    };

#endif // CPENGATTRIBUTEFETCHOP_H




