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
* Description:  Access handler of the network session
*
*/

// INCLUDE FILES
#include "CPEngPureDataHandler.h"

#include "PEngTransAdapterPanics.h"
#include "CPEngSendDataOperation.h"
#include "MPEngSessionManager.h"
#include "MPEngAccessHandler.h"

// Packet logger
#include "PEngPacketLogger.h"


// CONSTANTS
const TInt KOpIdInitvalue = 1000;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::CPEngPureDataHandler
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngPureDataHandler::CPEngPureDataHandler(
    MPEngSessionManager& aSessManager,
    MPEngAccessHandler& aAccessHandler,
    const CPEngSessionSlotId& aSessionId,
    RImpsEng& aSessClient )
        : iCount( 1 ), // init to 1
        iSessionManager( aSessManager ),
        iAccessHandler( aAccessHandler ),
        iSessionId( aSessionId ),
        iSessClient( aSessClient ),
        iOpidIterator( KOpIdInitvalue )
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::CPEngPureDataHandler" ) );
    iAccessHandler.Open(); // CSI: 65 #
    iSessionManager.OpenRef();
    }

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngPureDataHandler::ConstructL(
    RImpsEng& aSessClient )
    {
    iPureClient.RegisterL(  aSessClient,            // main client
                            this,                   // Observer
                            EImpsServicePresence,   // Type of service
                            ETrue,                  // Incoming data interest
                            ETrue );                // Any content type
    iPureClient.RegisterErrorObserverL( *this );
    TInt bufferSize ( iPureClient.MaxTransactionContentLengthL() );

    // if size is zero, leave cause WV Engine is not ready
    if ( bufferSize == 0 )
        {
        User::Leave( KErrNotReady );
        }
    iTrasferBuffer = HBufC8::NewL( bufferSize );
#ifdef PENG_ENABLE_DEBUG_PRINT
    // init logs connection Id
    iConnectionLogsId = PEngPacketLogger::GetConnectionId();
#endif
    }

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngPureDataHandler* CPEngPureDataHandler::NewL(
    RImpsEng& aSessClient,
    MPEngSessionManager& aSessManager,
    MPEngAccessHandler& aAccessHandler,
    const CPEngSessionSlotId& aSessionId )
    {
    CPEngPureDataHandler* self = new ( ELeave ) CPEngPureDataHandler(
        aSessManager,
        aAccessHandler,
        aSessionId,
        aSessClient );
    CleanupClosePushL( *self );
    self->ConstructL( aSessClient );
    CleanupStack::Pop();
    return self;
    }


// Destructor
CPEngPureDataHandler::~CPEngPureDataHandler()
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::~CPEngPureDataHandler" ) );

    delete iTrasferBuffer;
    iIncomingData.ResetAndDestroy();

    if ( iIncReqStatus )
        {
        User::RequestComplete( iIncReqStatus, KErrCancel );
        }
    iPureClient.Unregister();
    iSessionManager.ClosePureDataHandler();
    // destroy arrays, operations are canceled
    iOperations.ResetAndDestroy();
    iAccessHandler.UnregisterSessionObserver( *this );
    iAccessHandler.Close();
    }


// =============================================================================
// =============== New Functions of the MPEngPureDataHandler ===================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::Close()
// -----------------------------------------------------------------------------
//
void CPEngPureDataHandler::Close()
    {
    iCount--;
    if ( iCount == 0 )
        {
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngPureDataHandler::TransferBufferL()
// -----------------------------------------------------------------------------
//
TPtr8 CPEngPureDataHandler::TransferBufferL()
    {
    TPtr8 buf = iTrasferBuffer->Des();
    buf.Zero();
    return buf;
    }


// -----------------------------------------------------------------------------
// CPEngPureDataHandler::SendDataL()
// -----------------------------------------------------------------------------
//
TInt CPEngPureDataHandler::SendDataL(
    TRequestStatus& aStatus )
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::SendDataL:" ) );

    if ( iTrasferBuffer->Length() == 0 )
        {
        // buffer is empty, nothing to send, leave
        User::Leave( KErrArgument );
        }
    // allocate slot in array by adding NULL pointer and removing it
    iOperations.AppendL( NULL );
    iOperations.Remove( iOperations.Count() - 1 );
    CPEngSendDataOperation* operation =
        CPEngSendDataOperation::NewLC();

    TInt opId( iPureClient.SendPureDataL( *iTrasferBuffer ) );
    operation->SetIds( opId, iOpidIterator++ );

#ifdef PENG_ENABLE_DEBUG_PRINT
    iConnectionLogsId = PEngPacketLogger::WriteOutPacket(
                            *iTrasferBuffer,
                            operation->LogId(),
                            opId,
                            iConnectionLogsId,
                            iSessionId.UserId(),
                            iSessionId.ServiceAddress() );
#endif

    iOperations.Append( operation );
    CleanupStack::Pop(); // operation
    // now it is safe to set request status
    operation->SetRequestStatus( aStatus );
    aStatus = KRequestPending;
    return opId;
    }


// -----------------------------------------------------------------------------
// CPEngPureDataHandler::ResponseLC()
// -----------------------------------------------------------------------------
//
HBufC8* CPEngPureDataHandler::ResponseL(
    TInt aTransId )
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::ResponseL - id:  %d" ), aTransId );
    // is it incoming data request
    if ( aTransId == 0 )
        {
        if ( iIncomingData.Count() == 0 )
            {
            User::Leave( KErrNotFound );
            }
        // take first one from the array
        HBufC8* buff = iIncomingData[ 0 ];
        iIncomingData.Remove( 0 );
        return buff;
        }
    // try to find operation
    TInt x( FindOperationL( aTransId, CPEngSendDataOperation::OperationId ) );
    // is operation completed?
    TRequestStatus* status = iOperations[ x ]->Status();
    if ( status && ( *status == KRequestPending ) )
        {
        User::Leave( KErrNotReady );
        }

    // send buffer and remove operation
    HBufC8* buff = iOperations[ x ]->Response();
    delete iOperations[ x ];
    iOperations.Remove( x );
    return buff;
    }


// -----------------------------------------------------------------------------
// CPEngPureDataHandler::CancelSending()
// -----------------------------------------------------------------------------
//
void CPEngPureDataHandler::CancelSending(
    TInt aTransId )
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::CancelSending - id:  %d" ),
             aTransId );

    // try to find operation
    TInt x( FindOperation( aTransId, CPEngSendDataOperation::OperationId ) );
    if ( x == KErrNotFound )
        {
        // nothing to cancel
        return;
        }
    TRequestStatus* status = iOperations[ x ]->Status();
    if ( status && ( *status == KRequestPending )  )
        {
        // operation pending, cancel it
        iPureClient.CancelTransaction(  aTransId );
        iOperations[ x ]->CompletedOperation( KErrCancel );
        }
    delete iOperations[ x ];
    iOperations.Remove( x );
    }


// -----------------------------------------------------------------------------
// CPEngPureDataHandler::ListenIncomingData()
// -----------------------------------------------------------------------------
//
TInt CPEngPureDataHandler::ListenIncomingData(
    TRequestStatus& aStatus )
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::ListenIncomingData" ) );

    __ASSERT_DEBUG( !iIncReqStatus,
                    User::Panic( KPEngTransferAdapterPanic,
                                 EListenerAlreadyActive )
                  );
    iIncReqStatus = &aStatus;
    aStatus = KRequestPending;
    return iIncomingData.Count();
    }


// -----------------------------------------------------------------------------
// CPEngPureDataHandler::CancelListening()
// -----------------------------------------------------------------------------
//
void CPEngPureDataHandler::CancelListening()
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::CancelListening" ) );

    if ( iIncReqStatus )
        {
        User::RequestComplete( iIncReqStatus, KErrCancel );
        iIncReqStatus = NULL;
        }
    }


// =============================================================================
// =============== New Functions of the MImpsPureHandler =======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::HandlePureDataL()
// -----------------------------------------------------------------------------
//
void CPEngPureDataHandler::HandlePureDataL(
    TInt aOpId,
    const TDesC8& aBuffer,
    TImpsCspIdentifier& /* aCspId*/ )
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::HandlePureDataL - id:  %d" ),
             aOpId );

    // is it incoming data handler
    if ( aOpId == 0 )
        {
#ifdef PENG_ENABLE_DEBUG_PRINT
        iConnectionLogsId = PEngPacketLogger::WriteNotifyPacket(
                                aBuffer,
                                iLastServerOpId,
                                iConnectionLogsId,
                                iSessionId.UserId(),
                                iSessionId.ServiceAddress() );
#endif

        HBufC8* buff = aBuffer.AllocLC();
        iIncomingData.AppendL( buff );
        CleanupStack::Pop(); // buff
        if ( iIncReqStatus )
            {
            User::RequestComplete( iIncReqStatus, KErrNone );
            iIncReqStatus = NULL;
            }
        return;
        }

    // get buffer and store it in the correct Operation if it is our operation
    TInt x( FindOperationL( aOpId, CPEngSendDataOperation::OperationId ) );
    if ( x != KErrNotFound )
        {
#ifdef PENG_ENABLE_DEBUG_PRINT
        iConnectionLogsId = PEngPacketLogger::WriteInPacket(
                                aBuffer,
                                iOperations[ x ]->LogId(),
                                aOpId,
                                iConnectionLogsId,
                                iSessionId.UserId(),
                                iSessionId.ServiceAddress() );

        iLastServerOpId = iOperations[ x ]->LogId();
#endif
        iOperations[ x ]->SetResponse( aBuffer.AllocL() );
        iOperations[ x ]->CompletedOperation( KErrNone );
        }
    }


// =============================================================================
// =============== New Functions of the MImpsErrorHandler ======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CPEngPureDataHandler::HandleErrorL(
    TInt aStatus,
    TInt aOpId,
    const TDesC* /*aDescription*/,
    const CImpsDetailed* /* aDetailedRes*/,
    TImpsCspIdentifier& /*aCspId*/ )
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::HandleErrorL - id:  %d, %d" ),
             aStatus,
             aOpId );

    // get buffer and store it in the correct Operation
    TInt x( FindOperationL( aOpId, CPEngSendDataOperation::OperationId ) );
#ifdef PENG_ENABLE_DEBUG_PRINT
    iConnectionLogsId = PEngPacketLogger::WriteError(
                            aStatus,
                            iOperations[ x ]->LogId(),
                            aOpId,
                            iConnectionLogsId,
                            iSessionId.UserId(),
                            iSessionId.ServiceAddress() );

#endif
    iOperations[ x ]->CompletedOperation( aStatus );
    }


// =============================================================================
// ============== New Functions of the MPEngSessionStatusObserver ==============
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngPureDataHandler::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CPEngPureDataHandler::StatusChangedL(
    TPEngNWSessionSlotState aNewState,
    TPEngNWSessionSlotEvent /*aNewEvent*/ )
    {
    if ( aNewState == EPEngNWPresenceSessionClosed )
        {
        // cancel all operations
        iOperations.ResetAndDestroy();
        }
    }


// =============================================================================
// =============== New Functions of main class =================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::Open()
// -----------------------------------------------------------------------------
//
void CPEngPureDataHandler::Open()
    {
    iCount++;
    }


// =============================================================================
// =============== New private Functions of the base class =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::FindOperation()
// Find operation in array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngPureDataHandler::FindOperation(
    TInt aOpId,
    TInt ( CPEngSendDataOperation:: *aIdFunc )() const ) const
    {
    TInt count ( iOperations.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( aOpId == ( ( *iOperations[ x ] ).*aIdFunc )() )
            {
            return x;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CPEngPureDataHandler::FindOperationL()
// Find operation in array, leave if not found
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngPureDataHandler::FindOperationL(
    TInt aOpId,
    TInt ( CPEngSendDataOperation:: *aIdFunc )() const ) const
    {
    TInt x ( FindOperation( aOpId, aIdFunc ) );
    if ( x == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    return x;
    }
// End of File


