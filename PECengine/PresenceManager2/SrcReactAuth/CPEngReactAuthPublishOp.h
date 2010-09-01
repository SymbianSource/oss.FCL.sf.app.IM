/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Operation handler for performing respond publish.
*
*/

#ifndef CPENGREACTAUTHPUBLISHOP_H
#define CPENGREACTAUTHPUBLISHOP_H

//  INCLUDES
#include <E32Base.h>
#include <badesca.h>
#include "CPEngAsyncOperation.h"


// FORWARD DECLARATIONS
class MPEngReactAuthTransactionObserver;
class MPEngAdvTransactionStatus2;
class CPEngReactAuthTransaction;
class MPEngAuthorizationManager;
class MPEngAuthorizationRespond;



// CLASS DECLARATION

/**
 * Operation handler for performing reactive
 * authorization respond publish.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngReactAuthPublishOp ) : public CPEngAsyncOperation
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngReactAuthPublishOp* NewL( TInt aPriority,
    CPEngReactAuthTransaction& aInterface,
    CPEngNWSessionSlotStorageProxy& aUsedSlot,
    MPEngReactAuthTransactionObserver& aObserver,
    RPEngManagerClient& aServer,
    MPEngAuthorizationRespond* aResponds );

    /**
     * Destructor.
     */
    virtual ~CPEngReactAuthPublishOp();

private:

    /**
     * C++ default constructor.
     */
    CPEngReactAuthPublishOp( TInt aPriority,
                             CPEngReactAuthTransaction& aInterface,
                             MPEngReactAuthTransactionObserver& aObserver,
                             RPEngManagerClient& aServer );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot,
                     MPEngAuthorizationRespond* aResponds );


public: //New services


    /**
     *
     *
     *
     *
     * @since 3.0
     */
    void PublishResponds();




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

private: //Helpers

    void CleanResponds();


private:

    //REF: The implemented interface
    CPEngReactAuthTransaction&     iInterface;

    //REF: The observer
    MPEngReactAuthTransactionObserver&     iObserver;

    //OWN:
    MPEngAuthorizationManager*  iAuthManager;

    //OWN:
    MPEngAuthorizationRespond* iResponds;

    //OWN:
    TBool iRespondsOwned;
    };

#endif  //CPENGREACTAUTHPUBLISHOP_H



