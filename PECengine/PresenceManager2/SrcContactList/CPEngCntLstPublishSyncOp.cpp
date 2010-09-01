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
* Description: this class handles the operation for publishing / synhronizing contact lists.
*
*/


// INCLUDE FILES
#include "CPEngCntLstPublishSyncOp.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngAdvTransactionStatus2.h"
#include "PEngMessagePacker.h"

#include <CPEngContactListTransaction2.h>
#include <MPEngContactListTransactionObserver2.h>



// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CPEngCntLstPublishSyncOp* CPEngCntLstPublishSyncOp::NewL( TInt aPriority,
                                                          CPEngContactListTransaction2& aInterface,
                                                          CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                                          const MDesCArray& aContactListNames,
                                                          TBool aSynchronization,
                                                          MPEngContactListTransactionObserver2& aObserver,
                                                          RPEngManagerClient& aServer )
    {
    CPEngCntLstPublishSyncOp* self = new ( ELeave ) CPEngCntLstPublishSyncOp( aPriority,
                                                                              aInterface,
                                                                              aObserver,
                                                                              aServer );

    CleanupStack::PushL( self );
    self->ConstructL( aUsedSlot, aContactListNames, aSynchronization );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CPEngCntLstPublishSyncOp::~CPEngCntLstPublishSyncOp()
    {
    CActive::Cancel();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngCntLstPublishSyncOp::CPEngCntLstPublishSyncOp( TInt aPriority,
                                                    CPEngContactListTransaction2& aInterface,
                                                    MPEngContactListTransactionObserver2& aObserver,
                                                    RPEngManagerClient& aServer )
        : CPEngAsyncOperation( aPriority, aServer ),
        iInterface( aInterface ),
        iObserver( aObserver )
    {
    }



// EPOC default constructor can leave.
void CPEngCntLstPublishSyncOp::ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                           const MDesCArray& aContactListNames,
                                           TBool aSynchronization )
    {
    BaseConstructL( aUsedSlot );


    //Check against duplicates
    const TInt listNameCount = aContactListNames.MdcaCount();
    TInt ii;
    for ( ii = 0; ii < listNameCount - 1; ii++ )
        {
        for ( TInt jj( ii + 1 ); jj < listNameCount; jj++ )
            {
            if ( aContactListNames.MdcaPoint( ii ) == aContactListNames.MdcaPoint( jj ) )
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }



    //Pack the publish request
    HBufC16* data = PEngMessagePacker::PackDesArrayL( aContactListNames );
    if ( aSynchronization )
        {
        InitTransaction( data, EPEngTransOpCntListSyncList );
        }
    else
        {
        InitTransaction( data, EPEngTransOpCntListUpdateList );
        }
    }


// -----------------------------------------------------------------------------
// CPEngCntLstPublishSyncOp::IssuePublishSync()
// -----------------------------------------------------------------------------
//
void CPEngCntLstPublishSyncOp::IssuePublishSync()
    {
    IssueTransaction();
    }


// -----------------------------------------------------------------------------
// CPEngCntLstPublishSyncOp::DoHandleOpSuccessL()
// -----------------------------------------------------------------------------
void CPEngCntLstPublishSyncOp::DoHandleOpSuccessL(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )

    {
    }


// -----------------------------------------------------------------------------
// CPEngCntLstPublishSyncOp::DoHandleOpFailure()
// -----------------------------------------------------------------------------
void CPEngCntLstPublishSyncOp::DoHandleOpFailure(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngCntLstPublishSyncOp::DoNotifyObserver()
// -----------------------------------------------------------------------------
TPEngAsyncOpResult CPEngCntLstPublishSyncOp::DoNotifyObserver(
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


