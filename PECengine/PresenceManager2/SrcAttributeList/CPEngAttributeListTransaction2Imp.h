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

#ifndef CPENGATTRIBUTELISTTRANSACTION2IMP_H
#define CPENGATTRIBUTELISTTRANSACTION2IMP_H

//  INCLUDES
#include <E32Base.h>
#include <CPEngAttributeListTransaction2.h>
#include "MPEngAsyncOperationOwner.h"
#include "RPEngManagerClient.h"




//FORWARD DECLARATIONS
class CPEngAttributeListTransaction2;
class CPEngAsyncOperation;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotStorageProxy;


/**
 * Presence contact list transactions.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeListTransaction2Imp ): public CBase,
        public MPEngAsyncOperationOwner

    {
public: /* Construction */


    /**
     * Instantiates CPEngAttributeListTransaction2Imp object.
     *
     * @return New CPEngAttributeListTransaction2Imp instance.
     */
    static CPEngAttributeListTransaction2Imp* NewL(
        CPEngAttributeListTransaction2& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CPEngAttributeListTransaction2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngAttributeListTransaction2Imp(
        CPEngAttributeListTransaction2& aInterface,
        TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Attribute list implementation */


    TBool IsPublishAttributeListsActive() const;
    TInt PublishAttributeLists( MPEngAttributeListTransactionObserver2& aObserver );
    void CancelPublishAttributeLists();


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
    CPEngAttributeListTransaction2& iInterface;

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Attribute list publish op
    CPEngAsyncOperation*    iAttributeListPublishOp;

    // OWN: Server
    RPEngManagerClient          iTransactionServer;


    };

#endif  //CPENGATTRIBUTELISTTRANSACTION2IMP_H

// End of File
