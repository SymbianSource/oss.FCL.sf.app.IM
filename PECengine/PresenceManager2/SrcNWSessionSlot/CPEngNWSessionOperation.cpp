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
* Description:  NWSessionOperation base class.
*
*/

// INCLUDE FILES
#include "CPEngNWSessionOperation.h"
#include "MPEngNWSessionOperationOwner.h"
#include "CPEngTransactionStatus.h"
#include "CPEngSessionSlotId.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngNWSessionProxy.h"


#include <CPEngNWSessionSlot2.h>
#include <CPEngNWSessionSlotID2.h>
#include <MPEngNWSessionOperationObserver2.h>



//LOCAL constants
#ifdef _DEBUG
namespace
    {
    //Panic
    _LIT( KPEngNWSessionOpPanic, "PEngNwSesOp" );

    //Panic reasons
    enum TPEngNWSessionOpPanicReasons
        {
        EEngNWSesOpHasAlreadyOwner,
        };

    void PEngNWSessionOpPanic( TPEngNWSessionOpPanicReasons aPanicReason )
        {
        User::Panic( KPEngNWSessionOpPanic, aPanicReason );
        }
    }
#endif



// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::CPEngNWSessionOperation
// -----------------------------------------------------------------------------
CPEngNWSessionOperation::CPEngNWSessionOperation( TInt aPriority,
                                                  CPEngNWSessionProxy& aNWSessionProxy,
                                                  CPEngNWSessionSlot2& aInterface,
                                                  MPEngNWSessionOperationObserver2& aObserver )
        : CActive( aPriority ),
        iNWSessionProxy( aNWSessionProxy ),
        iInterface( aInterface ),
        iObserver( aObserver ),
        iCancelling( EFalse )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::~CPEngNWSessionOperation()
// -----------------------------------------------------------------------------
CPEngNWSessionOperation::~CPEngNWSessionOperation()
    {
    if ( iOperationOwner )
        {
        iOperationOwner->HandleNWSessionOperationDestruction( this );
        }

    delete iTransactionStatus;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::BaseConstructL()
// -----------------------------------------------------------------------------
void CPEngNWSessionOperation::BaseConstructL()
    {
    iTransactionStatus = CPEngTransactionStatus::NewL();
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::SetOwner()
// -----------------------------------------------------------------------------
void CPEngNWSessionOperation::SetOwner( MPEngNWSessionOperationOwner& aOperationOwner )
    {
    __ASSERT_DEBUG( !iOperationOwner,
                    PEngNWSessionOpPanic( EEngNWSesOpHasAlreadyOwner ) );

    iOperationOwner = &aOperationOwner;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::CancelOpD()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionOperation::CancelOpD()
    {
    if ( iCancelling )
        {
        //Cancel already running...
        //==>This call is coming from observer callback which
        //was called from DoCancel(). Break the nesting here
        return;
        }

    iCancelling = ETrue;
    CActive::Cancel();
    iCancelling = EFalse;

    delete this;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::HandleOperationIssued()
// -----------------------------------------------------------------------------
void CPEngNWSessionOperation::HandleOperationIssued( TInt aError,
                                                     TInt aNWSessionSlotOp )
    {
    iNWSessionSlotOp = aNWSessionSlotOp;

    if ( aError == KErrNone )
        {
        SetActive();
        }

    else
        {
        //Complete with the error
        iStatus = KRequestPending;
        TRequestStatus *pS = &iStatus;
        User::RequestComplete( pS, aError );
        SetActive();
        }
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::NotifyObserverFromCancel()
// -----------------------------------------------------------------------------
void CPEngNWSessionOperation::NotifyObserverFromCancel()
    {
    iTransactionStatus->Reset();
    iTransactionStatus->SetStatus( iStatus.Int() );

    TRAPD( err,
           iObserver.HandleNWSessionOperationCompleteL( *iTransactionStatus,
                                                        iInterface,
                                                        iNWSessionSlotOp ); );

    if ( err != KErrNone )
        {
        iObserver.HandleNWSessionOperationNotifyError( err,
                                                       iInterface,
                                                       iNWSessionSlotOp );
        }
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::RunL()
// -----------------------------------------------------------------------------
void CPEngNWSessionOperation::RunL()
    {

    iTransactionStatus->Reset();
    iTransactionStatus->SetStatus( iStatus.Int() );
    DoHandleOpCompleted( iStatus.Int() );

    iObserver.HandleNWSessionOperationCompleteL( *iTransactionStatus,
                                                 iInterface,
                                                 iNWSessionSlotOp );
    //All done, delete self
    delete this;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionOperation::RunError()
// -----------------------------------------------------------------------------
TInt CPEngNWSessionOperation::RunError( TInt aError )
    {
    //Leave from observer callback, see RunL() above
    iObserver.HandleNWSessionOperationNotifyError( aError,
                                                   iInterface,
                                                   iNWSessionSlotOp );

    //All done, delete self
    delete this;
    return KErrNone;
    }


// End of file

