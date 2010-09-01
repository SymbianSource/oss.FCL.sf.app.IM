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
* Description:  Presence reactive authorization transactions.
*
*/

#ifndef CPENGREACTAUTHTRANSACTIONIMP_H
#define CPENGREACTAUTHTRANSACTIONIMP_H

//  INCLUDES
#include <E32Base.h>
#include <CPEngReactAuthTransaction.h>
#include "MPEngAsyncOperationOwner.h"
#include "RPEngManagerClient.h"


//FORWARD DECLARATIONS
class CPEngReactAuthTransaction;
class CPEngAsyncOperation;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotStorageProxy;
class MPEngAuthorizationRespond;


/**
 * Presence reactive authorization transactions.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngReactAuthTransactionImp ): public CBase,
        public MPEngAsyncOperationOwner

    {
public: /* Construction */


    /**
     * Instantiates CPEngReactAuthTransactionImp object.
     *
     * @return New CPEngReactAuthTransactionImp instance.
     */
    static CPEngReactAuthTransactionImp* NewL(
        CPEngReactAuthTransaction& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CPEngReactAuthTransactionImp();



protected:

    /**
     * C++ constructor.
     */
    CPEngReactAuthTransactionImp(
        CPEngReactAuthTransaction& aInterface,
        TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Reactive authorization implementation */

    TBool IsPublishReactAuthRespondsActive() const;
    TInt PublishReactAuthResponds( MPEngAuthorizationRespond*& aResponds,
                                   MPEngReactAuthTransactionObserver& aObserver );
    void CancelPublishReactAuthResponds();


private: //from MPEngAsyncOperationOwner


    /**
     * Notifies from async operation destruction.
     * @since 3.0
     * @param aOperation The dying operation.
     */
    void HandleAsyncOperationDestruction( CPEngAsyncOperation* aOperation );




private: //Data

    //REF: Implemented interface
    CPEngReactAuthTransaction& iInterface;

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    // OWN: Server
    RPEngManagerClient iTransactionServer;

    // OWN:
    CPEngAsyncOperation *iReactAuthRespondOp;

    };

#endif  //CPENGREACTAUTHTRANSACTIONIMP_H

// End of File
