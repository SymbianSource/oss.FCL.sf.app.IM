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

// INCLUDE FILES
#include "CPEngContactListTransaction2Imp.h"
#include "CPEngAsyncOperation.h"
#include "TPEngMDesCArrayAdapter.h"
#include "CPEngNWSessionSlotID2.h"
#include "CPEngNWSessionSlotStorageProxy.h"

#include "CPEngCntLstBaseSyncOp.h"
#include "CPEngCntLstPublishSyncOp.h"



//LOCAL constants
#ifdef _DEBUG
namespace
    {
    //Panic
    _LIT( KPEngCntListTransPanic, "PEngCntLstTrn" );

    //Panic reasons
    enum TPEngCntListTransPanicReasons
        {
        ECntListTransUnkownTransOp
        };

    void PEngCntListTransPanic( TPEngCntListTransPanicReasons aPanicReason )
        {
        User::Panic( KPEngCntListTransPanic, aPanicReason );
        }

    }
#endif



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContactListTransaction2Imp* CPEngContactListTransaction2Imp::NewL(
    CPEngContactListTransaction2& aInterface,
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngContactListTransaction2Imp* self =
        new ( ELeave ) CPEngContactListTransaction2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngContactListTransaction2Imp::~CPEngContactListTransaction2Imp()
    {
    //Cancel also deletes the operation objects
    CancelContactListPubSync();
    CancelBaseSynchronizeContactLists();
    iTransactionServer.Close();
    delete iUsedSlot;
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::CPEngContactListTransaction2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngContactListTransaction2Imp::CPEngContactListTransaction2Imp(
    CPEngContactListTransaction2& aInterface,
    TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransaction2Imp::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    User::LeaveIfError( iTransactionServer.Connect() );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::IsBaseSynchronizeContactListsActive()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListTransaction2Imp::IsBaseSynchronizeContactListsActive() const
    {
    return ( iCntLstBaseSyncOp != NULL );       // CSI: 64 #
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::BaseSynchronizeContactLists()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListTransaction2Imp::BaseSynchronizeContactLists(
    MPEngContactListTransactionObserver2& aObserver,
    TBool aNeedToUnsubscribe )
    {
    if ( iCntLstBaseSyncOp )
        {
        return KErrInUse;
        }

    CPEngCntLstBaseSyncOp* op = NULL;
    TRAPD( err,
           op = CPEngCntLstBaseSyncOp::NewL( iCActivePriority,
                                             iInterface,
                                             *iUsedSlot,
                                             aObserver,
                                             iTransactionServer,
                                             aNeedToUnsubscribe ); );

    if ( err == KErrNone )
        {
        //Success
        op->BaseSync();
        op->SetOwner( *this );
        iCntLstBaseSyncOp = op;
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::CancelBaseSynchronizeContactLists()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransaction2Imp::CancelBaseSynchronizeContactLists()
    {
    if ( iCntLstBaseSyncOp )
        {
        iCntLstBaseSyncOp->CancelOpD();
        iCntLstBaseSyncOp = NULL;
        }
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::IsContactListPubSyncActive()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListTransaction2Imp::IsContactListPubSyncActive() const
    {
    return ( iCntLstPublishSyncOp != NULL );        // CSI: 64 #
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::SynchronizeContactLists()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListTransaction2Imp::SynchronizeContactLists(
    const MDesCArray& aContactListNames,
    MPEngContactListTransactionObserver2& aObserver )

    {
    return DoContactListTransaction( aContactListNames,
                                     ETrue,
                                     aObserver );
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::PublishContactLists()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListTransaction2Imp::PublishContactLists(
    const MDesCArray& aContactListNames,
    MPEngContactListTransactionObserver2& aObserver )
    {
    return DoContactListTransaction( aContactListNames,
                                     EFalse,
                                     aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::CancelContactListPubSync()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransaction2Imp::CancelContactListPubSync()
    {
    if ( iCntLstPublishSyncOp )
        {
        iCntLstPublishSyncOp->CancelOpD();
        iCntLstPublishSyncOp = NULL;
        }
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::HandleAsyncOperationDestruction()
// from MPEngAsyncOperationOwner
// -----------------------------------------------------------------------------
//
void CPEngContactListTransaction2Imp::HandleAsyncOperationDestruction(
    CPEngAsyncOperation* aOperation )
    {
    if ( aOperation == iCntLstPublishSyncOp )
        {
        iCntLstPublishSyncOp = NULL;
        return;
        }

    if ( aOperation == iCntLstBaseSyncOp )
        {
        iCntLstBaseSyncOp = NULL;
        return;
        }


    __ASSERT_DEBUG( EFalse, PEngCntListTransPanic( ECntListTransUnkownTransOp ) );
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransaction2Imp::DoContactListTransaction()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListTransaction2Imp::DoContactListTransaction(
    const MDesCArray& aContactListNames,
    TBool aSynchronization,
    MPEngContactListTransactionObserver2& aObserver )
    {
    if ( iCntLstPublishSyncOp )
        {
        return KErrInUse;
        }

    CPEngCntLstPublishSyncOp* op = NULL;
    TRAPD( err,
           op = CPEngCntLstPublishSyncOp::NewL( iCActivePriority,
                                                iInterface,
                                                *iUsedSlot,
                                                aContactListNames,
                                                aSynchronization,
                                                aObserver,
                                                iTransactionServer ); );

    if ( err == KErrNone )
        {
        //Success
        op->IssuePublishSync();
        op->SetOwner( *this );
        iCntLstPublishSyncOp = op;
        }

    return err;
    }


// End of File


