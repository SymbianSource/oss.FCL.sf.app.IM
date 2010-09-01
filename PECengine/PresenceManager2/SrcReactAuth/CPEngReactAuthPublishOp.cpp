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


// INCLUDE FILES
#include "CPEngReactAuthPublishOp.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngAdvTransactionStatus2.h"

#include <CPEngReactAuthTransaction.h>
#include <MPEngReactAuthTransactionObserver.h>
#include "PEngListLibraryFactory.h"
#include "MPEngAuthorizationManager.h"
#include "MPEngAuthorizationRespond.h"



// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CPEngReactAuthPublishOp* CPEngReactAuthPublishOp::NewL( TInt aPriority,
                                                        CPEngReactAuthTransaction& aInterface,
                                                        CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                                        MPEngReactAuthTransactionObserver& aObserver,
                                                        RPEngManagerClient& aServer,
                                                        MPEngAuthorizationRespond* aResponds )
    {
    CPEngReactAuthPublishOp* self = new ( ELeave ) CPEngReactAuthPublishOp( aPriority,
                                                                            aInterface,
                                                                            aObserver,
                                                                            aServer );

    CleanupStack::PushL( self );
    self->ConstructL( aUsedSlot, aResponds );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CPEngReactAuthPublishOp::~CPEngReactAuthPublishOp()
    {
    CActive::Cancel();
    CleanResponds();
    if ( iAuthManager )
        {
        iAuthManager->Close();
        }
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngReactAuthPublishOp::CPEngReactAuthPublishOp( TInt aPriority,
                                                  CPEngReactAuthTransaction& aInterface,
                                                  MPEngReactAuthTransactionObserver& aObserver,
                                                  RPEngManagerClient& aServer )
        : CPEngAsyncOperation( aPriority, aServer ),
        iInterface( aInterface ),
        iObserver( aObserver )
    {
    }



// EPOC default constructor can leave.
void CPEngReactAuthPublishOp::ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                          MPEngAuthorizationRespond* aResponds )
    {
    __ASSERT_ALWAYS( aResponds, User::Leave( KErrArgument ) );

    BaseConstructL( aUsedSlot );        // CSI: 9 #
    iAuthManager = PEngListLibraryFactory::AuthorizationManagerL( aUsedSlot.BaseId() );
    iResponds = aResponds;

    //Pack the publish request
    HBufC16* data = iAuthManager->BufferedTransactionsLC();
    InitTransaction( data, EPEngTransOpAuthorizationResponse );
    CleanupStack::Pop( data );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthPublishOp::PublishResponds()
// -----------------------------------------------------------------------------
//
void CPEngReactAuthPublishOp::PublishResponds()
    {
    iRespondsOwned = ETrue;
    IssueTransaction();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthPublishOp::DoHandleOpSuccessL()
// -----------------------------------------------------------------------------
void CPEngReactAuthPublishOp::DoHandleOpSuccessL(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )

    {
    //Release the holded respond
    //Delete frees the lock also
    CleanResponds();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthPublishOp::DoHandleOpFailure()
// -----------------------------------------------------------------------------
void CPEngReactAuthPublishOp::DoHandleOpFailure(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )
    {
    //Release the holded respond
    //Delete frees the lock also
    CleanResponds();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthPublishOp::DoNotifyObserver()
// -----------------------------------------------------------------------------
TPEngAsyncOpResult CPEngReactAuthPublishOp::DoNotifyObserver(
    MPEngAdvTransactionStatus2& aStatus,
    TInt aTransactionOperation )
    {
    TRAPD( err, iObserver.HandleReactAuthTransactionCompleteL(
               aStatus,
               iInterface,
               aTransactionOperation ) );
    if ( err != KErrNone )
        {
        iObserver.HandleReactAuthTransactionError( err,
                                                   iInterface,
                                                   aTransactionOperation );
        }


    return EPEngAsyncOpCompleted;
    }



// -----------------------------------------------------------------------------
// CPEngReactAuthPublishOp::CleanResponds()
// -----------------------------------------------------------------------------
void CPEngReactAuthPublishOp::CleanResponds()
    {
    if ( iRespondsOwned )
        {
        delete iResponds;
        iResponds = NULL;
        }
    }


//End of file


