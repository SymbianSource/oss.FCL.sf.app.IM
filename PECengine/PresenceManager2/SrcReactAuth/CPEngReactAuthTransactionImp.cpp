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

// INCLUDE FILES
#include "CPEngReactAuthTransactionImp.h"
#include "CPEngAsyncOperation.h"
#include "CPEngNWSessionSlotID2.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "CPEngReactAuthPublishOp.h"



//LOCAL constants
#ifdef _DEBUG
namespace
    {
    //Panic
    _LIT( KPEngReactAuthTransPanic, "PEngRctAuthTrn" );

    //Panic reasons
    enum TPEngReactAuthTransPanicReasons
        {
        EReactAuthUnkownTransOp
        };

    void PEngReactAuthTransPanic( TPEngReactAuthTransPanicReasons aPanicReason )
        {
        User::Panic( KPEngReactAuthTransPanic, aPanicReason );
        }

    }
#endif



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngReactAuthTransactionImp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngReactAuthTransactionImp* CPEngReactAuthTransactionImp::NewL(
    CPEngReactAuthTransaction& aInterface,
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngReactAuthTransactionImp* self =
        new ( ELeave ) CPEngReactAuthTransactionImp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngReactAuthTransactionImp::~CPEngReactAuthTransactionImp()
    {
    //Cancel also deletes the operation objects
    CancelPublishReactAuthResponds();
    iTransactionServer.Close();
    delete iUsedSlot;
    }



// -----------------------------------------------------------------------------
// CPEngReactAuthTransactionImp::CPEngReactAuthTransactionImp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngReactAuthTransactionImp::CPEngReactAuthTransactionImp(
    CPEngReactAuthTransaction& aInterface,
    TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthTransactionImp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngReactAuthTransactionImp::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    User::LeaveIfError( iTransactionServer.Connect() );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthTransactionImp::IsPublishReactAuthRespondsActive()
// -----------------------------------------------------------------------------
//
TBool CPEngReactAuthTransactionImp::IsPublishReactAuthRespondsActive() const
    {
    return ( iReactAuthRespondOp != NULL );     // CSI: 64 #
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthTransactionImp::PublishReactAuthResponds()
// -----------------------------------------------------------------------------
//
TInt CPEngReactAuthTransactionImp::PublishReactAuthResponds(
    MPEngAuthorizationRespond*& aResponds,
    MPEngReactAuthTransactionObserver& aObserver )
    {
    if ( iReactAuthRespondOp )
        {
        return KErrInUse;
        }

    CPEngReactAuthPublishOp* op = NULL;
    TRAPD( err,
           op = CPEngReactAuthPublishOp::NewL( iCActivePriority,
                                               iInterface,
                                               *iUsedSlot,
                                               aObserver,
                                               iTransactionServer,
                                               aResponds );
         );

    if ( err == KErrNone )
        {
        //Success
        //Operation takes the ownership of aResponds
        aResponds = NULL;
        op->PublishResponds();
        op->SetOwner( *this );
        iReactAuthRespondOp = op;
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthTransactionImp::CancelPublishReactAuthResponds()
// -----------------------------------------------------------------------------
//
void CPEngReactAuthTransactionImp::CancelPublishReactAuthResponds()
    {
    if ( iReactAuthRespondOp )
        {
        iReactAuthRespondOp->CancelOpD();
        iReactAuthRespondOp = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthTransactionImp::HandleAsyncOperationDestruction()
// from MPEngAsyncOperationOwner
// -----------------------------------------------------------------------------
//
void CPEngReactAuthTransactionImp::HandleAsyncOperationDestruction(
    CPEngAsyncOperation* aOperation )
    {
    if ( aOperation == iReactAuthRespondOp )
        {
        iReactAuthRespondOp = NULL;
        return;
        }

    __ASSERT_DEBUG( EFalse, PEngReactAuthTransPanic( EReactAuthUnkownTransOp ) );
    }



// End of File


