/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"

#include "ImpsPluginPanics.h"
#include "CImpsPluginSendDataOperation.h"

#include <flogger.h>

#include <F32File.h>

#include  <APPARC.h>


// CONSTANTS
const TInt KOpIdInitvalue = 1000;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::CImpsPluginPureDataHandler
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CImpsPluginPureDataHandler::CImpsPluginPureDataHandler(
    MImpsPluginAccessHandler& aAccessHandler,
    RImpsEng& aSessClient )
        :
        iAccessHandler( aAccessHandler ),
        iSessClient( aSessClient ),
        iOpidIterator( KOpIdInitvalue )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::CImpsPluginPureDataHandler Start-End" ) );
    }

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImpsPluginPureDataHandler::ConstructL(
    RImpsEng& aSessClient )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::ConstructL Start" ) );
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

    User::LeaveIfError( iFs.Connect() );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::ConstructL End %d " ), bufferSize );
#ifdef IMPS_ENABLE_DEBUG_PRINT
    // init logs connection Id
    //iConnectionLogsId = PEngPacketLogger::GetConnectionId();
#endif
    }

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImpsPluginPureDataHandler* CImpsPluginPureDataHandler::NewL(
    RImpsEng& aSessClient,
    MImpsPluginAccessHandler& aAccessHandler )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::NewL Start" ) );
    CImpsPluginPureDataHandler* self = new ( ELeave ) CImpsPluginPureDataHandler(
        aAccessHandler,
        aSessClient );
    CleanupStack::PushL( self );
    self->ConstructL( aSessClient );
    CleanupStack::Pop( self );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::NewL End" ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::NewLC()
// Destructor
// -----------------------------------------------------------------------------
//
CImpsPluginPureDataHandler::~CImpsPluginPureDataHandler()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::~CImpsPluginPureDataHandler Start" ) );

    delete iTrasferBuffer;
    iIncomingData.ResetAndDestroy();

    if ( iIncReqStatus )
        {
        User::RequestComplete( iIncReqStatus, KErrCancel );
        iIncReqStatus = NULL;
        }
    iPureClient.Unregister();
    iOperations.ResetAndDestroy();
    iAccessHandler.UnregisterSessionObserver( *this );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::~CImpsPluginPureDataHandler End" ) );
    }


// =============================================================================
// =============== New Functions of the MImpsPureDataHandler ===================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::TransferBufferL()
// -----------------------------------------------------------------------------
//
TPtr8 CImpsPluginPureDataHandler::TransferBufferL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::TransferBufferL Start" ) );
    TPtr8 buf = iTrasferBuffer->Des();
    buf.Zero();
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::TransferBufferL End" ) );
    return buf;
    }


// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::SendDataL()
// -----------------------------------------------------------------------------
//
TInt CImpsPluginPureDataHandler::SendDataL(
    TRequestStatus& aStatus )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::SendDataL: Start" ) );

    if ( iTrasferBuffer->Length() == 0 )
        {
        // buffer is empty, nothing to send, leave
        User::Leave( KErrArgument );
        }

    // allocate slot in array by adding NULL pointer and removing it
    iOperations.AppendL( NULL );
    iOperations.Remove( iOperations.Count() - 1 );
    CImpsPluginSendDataOperation* operation =
        CImpsPluginSendDataOperation::NewLC();

    TInt opId( iPureClient.SendPureDataL( *iTrasferBuffer ) );

    // code for xmlfile

    IMPS_DP( D_IMPS_LIT( "*****************************************" ) );

    TFileName fileName = _L( "c:\\logs\\presadap12\\send.xml" );

    User::LeaveIfError( CApaApplication::GenerateFileName( iFs, fileName ) );



    RFile f;

    if ( f.Create( iFs, fileName, EFileWrite ) == KErrNone )

        {

        f.Write( 0, *iTrasferBuffer );

        f.Close();

        }





    IMPS_DP( D_IMPS_LIT( "-------------------------------------------" ) );

    operation->SetIds( opId, iOpidIterator++ );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::SendDataL opId %d" ), opId );

    iOperations.Append( operation );
    CleanupStack::Pop( operation ); // operation
    // now it is safe to set request status
    operation->SetRequestStatus( aStatus );
    aStatus = KRequestPending;

    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::SendDataL: End" ) );
    return opId;
    }


// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::ResponseLC()
// -----------------------------------------------------------------------------
//
HBufC8* CImpsPluginPureDataHandler::ResponseL(
    TInt aTransId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::ResponseL Start- id:  %d" ), aTransId );
    // is it incoming data request
    if ( aTransId == 0 )
        {
        IMPS_DP( D_IMPS_LIT( "=======================================================" ) );
        if ( iIncomingData.Count() == 0 )
            {
            User::Leave( KErrNotFound );
            }
        // take first one from the array
        HBufC8* buff = iIncomingData[ 0 ];
        iIncomingData.Remove( 0 );
        TFileName fileName = _L( "c:\\logs\\presadap12\\notification.xml" );

        User::LeaveIfError( CApaApplication::GenerateFileName( iFs, fileName ) );



        RFile f;

        if ( f.Create( iFs, fileName, EFileWrite ) == KErrNone )

            {

            f.Write( 0, *buff );

            f.Close();

            }

        return buff;
        }
    // try to find operation
    TInt x( FindOperationL( aTransId, CImpsPluginSendDataOperation::OperationId ) );
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

    // code for xmlfile



    TFileName fileName = _L( "c:\\logs\\presadap12\\receive.xml" );

    User::LeaveIfError( CApaApplication::GenerateFileName( iFs, fileName ) );



    RFile f;

    if ( f.Create( iFs, fileName, EFileWrite ) == KErrNone )

        {

        f.Write( 0, *buff );

        f.Close();

        }

    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::ResponseL: End" ) );
    return buff;
    }


// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::CancelSending()
// -----------------------------------------------------------------------------
//
void CImpsPluginPureDataHandler::CancelSending(
    TInt aTransId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::CancelSending Start - id:  %d" ),
             aTransId );

    // try to find operation
    TInt x( FindOperation( aTransId, CImpsPluginSendDataOperation::OperationId ) );
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

    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::CancelSending End " ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::ListenIncomingData()
// -----------------------------------------------------------------------------
//
TInt CImpsPluginPureDataHandler::ListenIncomingData(
    TRequestStatus& aStatus )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::ListenIncomingData Start" ) );

    __ASSERT_DEBUG( !iIncReqStatus,
                    User::Panic( KImpsPluginPanic,
                                 EListenerAlreadyActive )
                  );

    if ( !iIncReqStatus )
        {
        iIncReqStatus = &aStatus;
        aStatus = KRequestPending;
        }
    else
        {
        User::Leave( KErrNotReady );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::ListenIncomingData End Count is %d" ), iIncomingData.Count() );
    return iIncomingData.Count();
    }


// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::CancelListening()
// -----------------------------------------------------------------------------
//
void CImpsPluginPureDataHandler::CancelListening()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::CancelListening Start" ) );

    if ( iIncReqStatus )
        {
        User::RequestComplete( iIncReqStatus, KErrCancel );
        iIncReqStatus = NULL;
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::CancelListening End" ) );
    }


// =============================================================================
// =============== New Functions of the MImpsPureHandler =======================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::HandlePureDataL()
// -----------------------------------------------------------------------------
//
void CImpsPluginPureDataHandler::HandlePureDataL(
    TInt aOpId,
    const TDesC8& aBuffer,
    TImpsCspIdentifier& /* aCspId*/ )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::HandlePureDataL - id:  %d" ),
             aOpId );

    // is it incoming data handler
    if ( aOpId == 0 )
        {
        IMPS_DP( D_IMPS_LIT( "=======================================================" ) );

        HBufC8* buff = aBuffer.AllocLC();
        iIncomingData.AppendL( buff );
        CleanupStack::Pop( buff ); // buff
        if ( iIncReqStatus )
            {
            User::RequestComplete( iIncReqStatus, KErrNone );
            iIncReqStatus = NULL;
            }
        return;
        }

    // get buffer and store it in the correct Operation if it is our operation
    TInt x( FindOperationL( aOpId, CImpsPluginSendDataOperation::OperationId ) );
    if ( x != KErrNotFound )
        {

        iOperations[ x ]->SetResponse( aBuffer.AllocL() );
        iOperations[ x ]->CompletedOperation( KErrNone );
        }
    }


// =============================================================================
// =============== New Functions of the MImpsErrorHandler ======================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CImpsPluginPureDataHandler::HandleErrorL(
    TInt aStatus,
    TInt aOpId,
    const TDesC* /*aDescription*/,
    const CImpsDetailed* /* aDetailedRes*/,
    TImpsCspIdentifier& /*aCspId*/ )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::HandleErrorL - id:  %d, %d" ),
             aStatus,
             aOpId );
    IMPS_DP( D_IMPS_LIT( "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" ) );
    // get buffer and store it in the correct Operation
    TInt x( FindOperationL( aOpId, CImpsPluginSendDataOperation::OperationId ) );

    iOperations[ x ]->CompletedOperation( aStatus );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::HandleErrorL End" ) );
    }


// =============================================================================
// ============== New Functions of the MPEngSessionStatusObserver ==============
// =============================================================================


// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CImpsPluginPureDataHandler::StatusChangedL(
    TPEngNWSessionSlotState aNewState,
    TPEngNWSessionSlotEvent /*aNewEvent*/ )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::StatusChangedL Start" ) );
    if ( aNewState == EPEngNWPresenceSessionClosed )
        {
        // cancel all operations
        iOperations.ResetAndDestroy();
        //iPluginHost.HandleConnectionTerminated(
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::StatusChangedL End" ) );
    }


// =============================================================================
// =============== New Functions of main class =================================
// =============================================================================



// =============================================================================
// =============== New private Functions of the base class =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::FindOperation()
// Find operation in array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CImpsPluginPureDataHandler::FindOperation(
    TInt aOpId,
    TInt ( CImpsPluginSendDataOperation:: *aIdFunc )() const ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::FindOperation Start" ) );
    TInt count ( iOperations.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( aOpId == ( ( *iOperations[ x ] ).*aIdFunc )() )
            {
            IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::FindOperation Returns %d" ), x );
            return x;
            }
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::FindOperation End" ) );
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CImpsPluginPureDataHandler::FindOperationL()
// Find operation in array, leave if not found
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CImpsPluginPureDataHandler::FindOperationL(
    TInt aOpId,
    TInt ( CImpsPluginSendDataOperation:: *aIdFunc )() const ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::FindOperation Start" ) );
    TInt x ( FindOperation( aOpId, aIdFunc ) );
    if ( x == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginPureDataHandler::FindOperation End Returns %d" ), x );
    return x;
    }
// End of File


