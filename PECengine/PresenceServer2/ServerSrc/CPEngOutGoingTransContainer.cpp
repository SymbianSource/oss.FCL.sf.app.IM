/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container of the one outgoing transaction handler
*				  It process one handler until it is completed
* 				  canceled or some error ocures.
* 				  Its completion is signaled to the observer
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngOutGoingTransContainer.h"
#include    "MPEngOutgoingTransactionHandler.h"
#include    "MPEngPureDataHandler.h"
#include	"MPEngHandlerSendData.h"

//	Debug prints
#include	"PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngOutGoingTransContainer::CPEngOutGoingTransContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngOutGoingTransContainer::CPEngOutGoingTransContainer(
    MPEngHandlerSendData& aRequestHandler,
    MPEngPureDataHandler& aPureDataHandler,
    RPointerArray<MPEngOutgoingTransactionHandler>&
    aTransactionHandlersArray )
        : CActive( CActive::EPriorityStandard ),
        iPureDataHandler( aPureDataHandler ),
        iRequestHandler( aRequestHandler ),
        iNewTransactionHandlers( aTransactionHandlersArray )

    {
    }

// -----------------------------------------------------------------------------
// CPEngOutGoingTransContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngOutGoingTransContainer::ConstructL()
    {
    // add active object to the active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPEngOutGoingTransContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngOutGoingTransContainer* CPEngOutGoingTransContainer::NewL(
    MPEngHandlerSendData& aRequestHandler,
    MPEngPureDataHandler& aPureDataHandler,
    RPointerArray<MPEngOutgoingTransactionHandler>&
    aTransactionHandlersArray )
    {
    CPEngOutGoingTransContainer* self = NewLC( aRequestHandler,
                                               aPureDataHandler,
                                               aTransactionHandlersArray );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngOutGoingTransContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngOutGoingTransContainer* CPEngOutGoingTransContainer::NewLC(
    MPEngHandlerSendData& aRequestHandler,
    MPEngPureDataHandler& aPureDataHandler,
    RPointerArray<MPEngOutgoingTransactionHandler>&
    aTransactionHandlersArray )
    {
    CPEngOutGoingTransContainer* self =
        new( ELeave ) CPEngOutGoingTransContainer(
        aRequestHandler,
        aPureDataHandler,
        aTransactionHandlersArray );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CPEngOutGoingTransContainer::~CPEngOutGoingTransContainer()
    {
    PENG_DP( D_PENG_LIT( "CPEngOutGoingTransContainer::~CPEngOutGoingTransContainer()" ) );
    Cancel();
    delete iTransactionHandler;
    }

// -----------------------------------------------------------------------------
// -------- Function from the CActive ------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngOutGoingTransContainer::StartTransaction
// Start performing transaction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngOutGoingTransContainer::StartTransactionL(
    MPEngOutgoingTransactionHandler* aOutgoingTransaction )
    {
    PENG_DP( D_PENG_LIT( "CPEngOutGoingTransContainer::StartTransactionL()" ) );
    TPtr8 outgoingBuffer( iPureDataHandler.TransferBufferL() );
    aOutgoingTransaction->RequestL( outgoingBuffer );
    iTransactionId = iPureDataHandler.SendDataL( iStatus );
    SetActive();
    // take ownership of transaction
    iTransactionHandler = aOutgoingTransaction;
    }

// =============================================================================
// ============== Function from the CActive ====================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngOutGoingTransContainer::RunL
// Handles an active object’s request completion event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngOutGoingTransContainer::RunL()
    {
    // what is necessary depends how request went
    TInt errCode ( iStatus.Int() );
    PENG_DP( D_PENG_LIT( "CPEngOutGoingTransContainer::RunL() - result:  %d" ),
             errCode );

    switch ( errCode )
        {
        case KErrNone:
            {
            HBufC8* response = iPureDataHandler.ResponseL( iTransactionId );
            User::LeaveIfNull( response );
            CleanupStack::PushL( response );
            TBool last ( iRequestHandler.LastRunningTransaction() );
            if ( last )
                {
                iTransactionHandler->LastRunningTransactionHandler();
                }
            // TO-DO do processing asynchronous
            iTransactionHandler->ProcessResponseL( *response, iStatus );
            CleanupStack::PopAndDestroy( response ); // response

            iTransactionHandler->NewTransactionHandlersL(
                iNewTransactionHandlers );
            if ( iNewTransactionHandlers.Count() != 0 )
                {
                // new transaction handlers were created, inform RequestHandler
                iRequestHandler.CreateNewContainersRunThemL();
                }

            if ( !iTransactionHandler->TransactionCompleted() )
                {
                // we are not done, just run another round
                StartTransactionL( iTransactionHandler );
                return;
                }
            break;
            }

        default:
            {
            // transfer error code
            iTransactionHandler->ReleaseHandler();
            }
        }
    // Get status and complete container, status ownership transfered
    CPEngTransactionStatus* status =
        iTransactionHandler->TransactionResult();
    iRequestHandler.CompleteTransContainer( this, status, errCode );
    PENG_DP( D_PENG_LIT( "CPEngOutGoingTransContainer::RunL() - End" ) );
    }

// -----------------------------------------------------------------------------
// CPEngOutGoingTransContainer::RunError
// Handles a leave occurring in the request completion event handler RunL()
// implementation should return KErrNone, if it handles the leave
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngOutGoingTransContainer::RunError(
    TInt aError )
    {
    // release handler and complete container
    iTransactionHandler->ReleaseHandler();
    // Get status and complete container, status ownership transfered
    CPEngTransactionStatus* status =
        iTransactionHandler->TransactionResult();
    iRequestHandler.CompleteTransContainer( this, status, aError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEngOutGoingTransContainer::DoCancel
// Implements cancellation of an outstanding request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngOutGoingTransContainer::DoCancel()
    {
    PENG_DP( D_PENG_LIT( "CPEngOutGoingTransContainer::DoCancel()" ) );
    // request was canceled, cancel log in, update state and complete RMessage
    iPureDataHandler.CancelSending( iTransactionId );
    iTransactionHandler->ReleaseHandler();
    // Container is not removed here
    }

//  End of File





















