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
* Description: this class handles the operation for performing contact list base synhronization
*
*/


// INCLUDE FILES
#include "CPEngCntLstBaseSyncOp.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngAdvTransactionStatus2.h"

#include <CPEngContactListTransaction2.h>
#include <MPEngContactListTransactionObserver2.h>



// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CPEngCntLstBaseSyncOp* CPEngCntLstBaseSyncOp::NewL( TInt aPriority,
                                                    CPEngContactListTransaction2& aInterface,
                                                    CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                                    MPEngContactListTransactionObserver2& aObserver,
                                                    RPEngManagerClient& aServer,
                                                    TBool aNeedToUnsubscribe )
    {
    CPEngCntLstBaseSyncOp* self = new ( ELeave ) CPEngCntLstBaseSyncOp( aPriority,
                                                                        aInterface,
                                                                        aObserver,
                                                                        aServer,
                                                                        aNeedToUnsubscribe );

    CleanupStack::PushL( self );
    self->ConstructL( aUsedSlot );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CPEngCntLstBaseSyncOp::~CPEngCntLstBaseSyncOp()
    {
    CActive::Cancel();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngCntLstBaseSyncOp::CPEngCntLstBaseSyncOp( TInt aPriority,
                                              CPEngContactListTransaction2& aInterface,
                                              MPEngContactListTransactionObserver2& aObserver,
                                              RPEngManagerClient& aServer,
                                              TBool aNeedToUnsubscribe )
        : CPEngAsyncOperation( aPriority, aServer ),
        iInterface( aInterface ),
        iObserver( aObserver ),
        iUnsubscribe( aNeedToUnsubscribe )
    {
    }



// EPOC default constructor can leave.
void CPEngCntLstBaseSyncOp::ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot )
    {
    BaseConstructL( aUsedSlot );

    //Contact list base sync has empty data
    if ( iUnsubscribe )
        {
        _LIT( KUnsubscribe, "UNSUB" );
        HBufC* unSubs = KUnsubscribe().AllocLC();

        InitTransaction( unSubs, EPEngTransOpCntListBaseSync );
        CleanupStack::Pop( unSubs );
        }
    else
        {
        InitTransaction( NULL, EPEngTransOpCntListBaseSync );
        }
    }


// -----------------------------------------------------------------------------
// CPEngCntLstBaseSyncOp::BaseSync()
// -----------------------------------------------------------------------------
//
void CPEngCntLstBaseSyncOp::BaseSync()
    {
    IssueTransaction();
    }


// -----------------------------------------------------------------------------
// CPEngCntLstBaseSyncOp::DoHandleOpSuccessL()
// -----------------------------------------------------------------------------
void CPEngCntLstBaseSyncOp::DoHandleOpSuccessL(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )

    {
    }


// -----------------------------------------------------------------------------
// CPEngCntLstBaseSyncOp::DoHandleOpFailure()
// -----------------------------------------------------------------------------
void CPEngCntLstBaseSyncOp::DoHandleOpFailure(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngCntLstBaseSyncOp::DoNotifyObserver()
// -----------------------------------------------------------------------------
TPEngAsyncOpResult CPEngCntLstBaseSyncOp::DoNotifyObserver(
    MPEngAdvTransactionStatus2& aStatus,
    TInt aTransactionOperation )
    {
    TRAPD( err, iObserver.HandleContactListTransactionCompleteL(
               aStatus,
               iInterface,
               aTransactionOperation ) );
    if ( err != KErrNone )
        {
        iObserver.HandleContactListTransactionError( err,
                                                     iInterface,
                                                     aTransactionOperation );
        }

    return EPEngAsyncOpCompleted;
    }


//End of file


