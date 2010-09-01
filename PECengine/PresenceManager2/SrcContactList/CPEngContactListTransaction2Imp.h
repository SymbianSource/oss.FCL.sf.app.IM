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
* Description:  Presence contact list transactions.
*
*/

#ifndef CPENGCONTACTLISTTRANSACTION2IMP_H
#define CPENGCONTACTLISTTRANSACTION2IMP_H

//  INCLUDES
#include <E32Base.h>
#include <CPEngContactListTransaction2.h>
#include "MPEngAsyncOperationOwner.h"
#include "RPEngManagerClient.h"




//FORWARD DECLARATIONS
class CPEngContactListTransaction2;
class CPEngAsyncOperation;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotStorageProxy;


/**
 * Presence contact list transactions.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListTransaction2Imp ): public CBase,
        public MPEngAsyncOperationOwner

    {
public: /* Construction */


    /**
     * Instantiates CPEngContactListTransaction2Imp object.
     *
     * @return New CPEngContactListTransaction2Imp instance.
     */
    static CPEngContactListTransaction2Imp* NewL(
        CPEngContactListTransaction2& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CPEngContactListTransaction2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngContactListTransaction2Imp(
        CPEngContactListTransaction2& aInterface,
        TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Contact list implementation */



    TBool IsBaseSynchronizeContactListsActive() const;
    TInt BaseSynchronizeContactLists( MPEngContactListTransactionObserver2& aObserver,
                                      TBool aNeedToUnsubscribe );
    void CancelBaseSynchronizeContactLists();

    TBool IsContactListPubSyncActive() const;
    TInt SynchronizeContactLists( const MDesCArray& aContactListNames,
                                  MPEngContactListTransactionObserver2& aObserver );

    TInt PublishContactLists( const MDesCArray& aContactListNames,
                              MPEngContactListTransactionObserver2& aObserver );

    void CancelContactListPubSync();




private: //from MPEngAsyncOperationOwner


    /**
     * Notifies from async operation destruction.
     * @since 3.0
     * @param aOperation The dying operation.
     */
    void HandleAsyncOperationDestruction( CPEngAsyncOperation* aOperation );



private: //from helpers

    TInt DoContactListTransaction( const MDesCArray& aContactListNames,
                                   TBool aSynchronization,
                                   MPEngContactListTransactionObserver2& aObserver );




private: //Implementation



private: //Data

    //REF: Implemented interface
    CPEngContactListTransaction2& iInterface;

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Contact list base sync op
    CPEngAsyncOperation*    iCntLstBaseSyncOp;

    //OWN: Contact list publish / synchronization op
    CPEngAsyncOperation*    iCntLstPublishSyncOp;

    // OWN: Server
    RPEngManagerClient          iTransactionServer;

    };

#endif  //CPENGCONTACTLISTTRANSACTION2IMP_H

// End of File
