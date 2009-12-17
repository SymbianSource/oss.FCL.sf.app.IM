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
* Description:  Presence attribute transactions.
*
*/

#ifndef CPENGATTRIBUTETRANSACTION2IMP_H
#define CPENGATTRIBUTETRANSACTION2IMP_H

//  INCLUDES
#include <E32Base.h>
#include <CPEngAttributeTransaction2.h>
#include "MPEngAsyncOperationOwner.h"
#include "RPEngManagerClient.h"



//FORWARD DECLARATIONS
class CPEngAttributeTransaction2;
class CPEngAsyncOperation;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotStorageProxy;
class CPEngAttributeFetchOp;


/**
 * Presence attribute transactions.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeTransaction2Imp ): public CBase,
        public MPEngAsyncOperationOwner

    {
public: /* Construction */


    /**
     * Instantiates CPEngAttributeTransaction2Imp object.
     *
     * @return New CPEngAttributeTransaction2Imp instance.
     */
    static CPEngAttributeTransaction2Imp* NewL(
        CPEngAttributeTransaction2& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CPEngAttributeTransaction2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngAttributeTransaction2Imp(
        CPEngAttributeTransaction2& aInterface,
        TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Attribute transaction implementation */

    TBool IsPublishOwnAttributesActive() const;


    TInt PublishAndUnLockOwnAttribute(
        MPEngPresenceAttrModel2*& aModel,
        MPEngAttributeTransactionObserver2& aObserver );


    TInt PublishAndUnLockOwnAttributes(
        RPointerArray<MPEngPresenceAttrModel2>& aModels,
        MPEngAttributeTransactionObserver2& aObserver );


    void CancelPublishOwnAttributes();


    TBool IsFetchAttributesActive() const;


    TInt FetchAttributes( const MDesCArray& aPresenceIDs,
                          const TArray<TUint32>& aTypes,
                          MPEngAttributeTransactionObserver2& aObserver,
                          TInt aOptions,
                          TBool aResultAsLoclObjects );

    void CancelFetchAttributes();
    TInt GetFetchedAttributes( RPointerArray< MPEngPresenceAttrModel2>& aModels );


private: //from MPEngAsyncOperationOwner


    /**
     * Notifies from async operation destruction.
     * @since 3.0
     * @param aOperation The dying operation.
     */
    void HandleAsyncOperationDestruction( CPEngAsyncOperation* aOperation );





private: //Implementation



private: //Data

    //REF: Implemented interface
    CPEngAttributeTransaction2& iInterface;

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Attribute fetch
    CPEngAttributeFetchOp*  iAttributeFetchOp;

    //OWN: Attribute publish
    CPEngAsyncOperation*    iAttributePublishOp;

    // OWN: Server
    RPEngManagerClient          iTransactionServer;

    };

#endif  // CPENGATTRIBUTETRANSACTION2IMP_H

// End of File
