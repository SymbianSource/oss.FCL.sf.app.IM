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
* Description:  Asynchronous operation base class.
*
*/

// INCLUDE FILES
#include "CPEngAsyncOperation.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "CPEngSessionSlotId.h"
#include "MPEngAsyncOperationOwner.h"
#include "CPEngTransactionStatus.h"



//LOCAL constants
namespace
    {
    //Panic
    _LIT( KPEngAsyncOpPanic, "PEngAsyncOp" );

    //Panic reasons
    enum TPEngAsyncOpPanicReasons
        {
        EEngAsyncOpHasAlreadyOwner,
        EEngAsyncOpAlreadyInUse
        };

    void PEngAsyncOpPanic( TPEngAsyncOpPanicReasons aPanicReason )
        {
        User::Panic( KPEngAsyncOpPanic, aPanicReason );
        }
    }



// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CPEngAsyncOperation::CPEngAsyncOperation
// -----------------------------------------------------------------------------
CPEngAsyncOperation::CPEngAsyncOperation( TInt aPriority,
                                          RPEngManagerClient& aServer )
        : CActive( aPriority ),
        iCancelling( EFalse ),
        iTransactionServer( aServer )
    {
    CActiveScheduler::Add( this );
    }



// -----------------------------------------------------------------------------
// CPEngAsyncOperation::~CPEngAsyncOperation()
// -----------------------------------------------------------------------------
CPEngAsyncOperation::~CPEngAsyncOperation()
    {
    if ( iOperationOwner )
        {
        iOperationOwner->HandleAsyncOperationDestruction( this );
        }

    delete iTransactionStatus;
    delete iTransactionData;

    iTransactionClient.Close();
    }


// -----------------------------------------------------------------------------
// CPEngAsyncOperation::BaseConstructL()
// -----------------------------------------------------------------------------
void CPEngAsyncOperation::BaseConstructL(
    CPEngNWSessionSlotStorageProxy& aUsedSlot )
    {
    iTransactionStatus = CPEngTransactionStatus::NewL();
    HBufC8* sessSlotIdPkg = aUsedSlot.BaseId().PackLC();
    User::LeaveIfError( iTransactionClient.Connect( *sessSlotIdPkg,
                                                    aUsedSlot.AppId(),
                                                    iTransactionServer ) );

    CleanupStack::PopAndDestroy( sessSlotIdPkg );
    }



// -----------------------------------------------------------------------------
// CPEngAsyncOperation::SetOwner()
// -----------------------------------------------------------------------------
void CPEngAsyncOperation::SetOwner( MPEngAsyncOperationOwner& aOperationOwner )
    {
    __ASSERT_ALWAYS( !iOperationOwner,
                     PEngAsyncOpPanic( EEngAsyncOpHasAlreadyOwner ) );

    iOperationOwner = &aOperationOwner;
    }



// -----------------------------------------------------------------------------
// CPEngAsyncOperation::CancelOpD()
// -----------------------------------------------------------------------------
//
void CPEngAsyncOperation::CancelOpD()
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
// CPEngAsyncOperation::InitTransaction()
// -----------------------------------------------------------------------------
//
void CPEngAsyncOperation::InitTransaction( HBufC16* aTransactionData,
                                           TInt aTransactionOperation )
    {
    delete iTransactionData;
    iTransactionData = aTransactionData;
    iTransactionOperation = aTransactionOperation;
    }



// -----------------------------------------------------------------------------
// CPEngAsyncOperation::IssueTransaction()
// -----------------------------------------------------------------------------
//
void CPEngAsyncOperation::IssueTransaction()
    {
    __ASSERT_ALWAYS( !IsActive(),
                     PEngAsyncOpPanic( EEngAsyncOpAlreadyInUse ) );

    TInt err = KErrNone;
    if ( iTransactionData )
        {
        err = iTransactionClient.UpdateData( iTransactionOperation,
                                             *iTransactionData,
                                             iStatus );
        }
    else
        {
        err = iTransactionClient.UpdateData( iTransactionOperation,
                                             KNullDesC,
                                             iStatus );
        }


    if ( err == KErrNone )
        {
        SetActive();
        }

    else
        {
        iStatus = KRequestPending;
        TRequestStatus *pS = &iStatus;
        User::RequestComplete( pS, err );
        SetActive();
        }
    }




// -----------------------------------------------------------------------------
// CPEngAsyncOperation::DoCancel()
// -----------------------------------------------------------------------------
void CPEngAsyncOperation::DoCancel()
    {
    iTransactionClient.CancelUpdateData();

    iTransactionStatus->Reset();
    iTransactionStatus->SetStatus( KErrCancel );

    DoHandleOpFailure( *iTransactionStatus,
                       iTransactionOperation );

    DoNotifyObserver( *iTransactionStatus,
                      iTransactionOperation );
    }




// -----------------------------------------------------------------------------
// CPEngAsyncOperation::RunL()
// -----------------------------------------------------------------------------
void CPEngAsyncOperation::RunL()
    {
    if ( iStatus.Int() < KErrNone )
        {
        //Operation failed somehow
        User::Leave( iStatus.Int() );
        }

    if ( iStatus.Int() == KErrNone )
        {
        //operation succesfull but no extra data available
        iTransactionStatus->Reset();
        iTransactionStatus->SetStatus( KErrNone );
        }

    else
        {
        //Fetch & decode operation results
        HBufC8* resultPkg = HBufC8::NewLC( iStatus.Int() );
        TPtr8 resultPkgPtr = resultPkg->Des();
        User::LeaveIfError( iTransactionClient.FetchLastRequestResult( resultPkgPtr ) );

        iTransactionStatus->Reset();
        iTransactionStatus->UnpackResultsL( *resultPkg );
        CleanupStack::PopAndDestroy(); // resultPkg
        }

    //Let derived class handle operation success
    //-> try to decode data from transaction status
    DoHandleOpSuccessL( *iTransactionStatus, iTransactionOperation );


    //And let derived class notify observer about operation completion
    TPEngAsyncOpResult opStatus = DoNotifyObserver( *iTransactionStatus,
                                                    iTransactionOperation );

    //Is transaction completed or should we wait for new round
    if ( opStatus == EPEngAsyncOpCompleted )
        {
        delete this;
        }
    else
        {
        iTransactionStatus->Reset();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAsyncOperation::RunError()
// -----------------------------------------------------------------------------
TInt CPEngAsyncOperation::RunError( TInt aError )
    {
    iTransactionStatus->Reset();
    iTransactionStatus->SetStatus( aError );

    DoHandleOpFailure( *iTransactionStatus,
                       iTransactionOperation );


    TPEngAsyncOpResult opStatus = DoNotifyObserver( *iTransactionStatus,
                                                    iTransactionOperation );

    if ( opStatus == EPEngAsyncOpCompleted )
        {
        delete this;
        }

    return KErrNone;
    }


// End of file
