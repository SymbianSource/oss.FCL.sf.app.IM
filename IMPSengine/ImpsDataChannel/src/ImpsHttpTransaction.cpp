/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Http Transaction class for Imps.
*
*
*/


// INCLUDE FILES
#include <hal.h>
#include "ImpsHttpTransaction.h"
#include "HttpTransportAdapter.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CImpsHttpTransaction::CImpsHttpTransaction
//
// ----------------------------------------------------------
//
CImpsHttpTransaction::CImpsHttpTransaction( CHttpTransportAdapter* aTransportAdapter,
                                            const TInt aTID ) :
        iTransportAdapter( aTransportAdapter ),
        iTID( aTID ),
        iSent( EFalse ),
        iCancelled( EFalse ),
        iConstructed( EFalse )

    {
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::NewL
//
// ----------------------------------------------------------
//
CImpsHttpTransaction* CImpsHttpTransaction::NewL( CHttpTransportAdapter* aTransportAdapter,
                                                  const TInt aTID,
                                                  const TDesC8& aRequestBody )
    {
    CImpsHttpTransaction* self = new ( ELeave ) CImpsHttpTransaction( aTransportAdapter, aTID );
    CleanupStack::PushL( self );
    self->ConstructL( aRequestBody );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::ConstructL
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::ConstructL( const TDesC8& aRequestBody )
    {
    ConstructRequestL();
    CopyRequestDataL( aRequestBody );
    iResponseData = CBufSeg::NewL( 500 );
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::ConstructRequestL
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::ConstructRequestL()
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsHttpTransaction::ConstructRequestL()" ) );
#endif
    RHTTPSession session = iTransportAdapter->HttpSession();
    RStringF method = session.StringPool().OpenFStringL( K8BitRequestPost );
    CleanupClosePushL( method );
    iHttpTransaction = session.OpenTransactionL( iTransportAdapter->DefaultSAP(), *iTransportAdapter, method );
    iConstructed = ETrue;
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "HTTP-ID: %d TID: %d constructed." ), iHttpTransaction.Id(), iTID );
#endif
    RStringF contTypeStr = session.StringPool().OpenFStringL( iTransportAdapter->MimeType() );
    CleanupClosePushL( contTypeStr );
    RHTTPHeaders hdr = iHttpTransaction.Request().GetHeaderCollection();
    THTTPHdrVal contType( contTypeStr );
    hdr.SetFieldL( session.StringPool().StringF( HTTP::EContentType, RHTTPSession::GetTable() ), contType );
    //This transaction supplies the stack with the payload data.
    iHttpTransaction.Request().SetBody( *this );
    CleanupStack::PopAndDestroy( 2 );  //contTypeStr, method
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::~CImpsHttpTransaction
//
// ----------------------------------------------------------
//
CImpsHttpTransaction::~CImpsHttpTransaction()
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsHttpTransaction::~CImpsHttpTransaction(). Destructor called. TID: %d" ), iTID );
#endif
    delete iRequestData;
    delete iResponseData;
    delete iExpiryTimer;
    if ( iConstructed && !iTransportAdapter->SessionClosed() )
        {
#ifdef _DEBUG
        CHttpTransportAdapter::WriteToLog( _L8( "HTTP-ID: %d TID: %d destructed." ), iHttpTransaction.Id(), iTID );
#endif
        iHttpTransaction.Close();
        }
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::GetNextDataPart
//
// ----------------------------------------------------------
//
TBool CImpsHttpTransaction::GetNextDataPart( TPtrC8& aDataPart )
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L( "CImpsHttpTransaction::GetNextDataPart(), iTID: %d" ), iTID );
#endif
    aDataPart.Set( iRequestData->Des() );
    return ETrue;
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::DispatchMessageL
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::DispatchMessageL()
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L( "CImpsHttpTransaction::DispatchMessageL(), iTID: %d" ), iTID );
    iSendTime = TimeL();
#endif
    iHttpTransaction.SubmitL();
    SetStatus( ETrue );
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::AppendDataL
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::AppendDataL( const TPtrC8& aBodyPart,
                                        const TBool aLastChunk )
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsHttpTransaction::AppendDataL() - Length of the chunk: %d" ), aBodyPart.Length() );
#endif
    iLastChunk = aLastChunk;
    iResponseData->ResizeL( iCurrentDataLength + aBodyPart.Length() );
    iResponseData->Write( iCurrentDataLength, aBodyPart );
    iCurrentDataLength = iCurrentDataLength + aBodyPart.Length();
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::FinaliseRequestL
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::FinaliseRequestL( const TInt aErrorCode )
    {
    TInt dataLength = iResponseData->Size();
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsHttpTransaction::FinaliseRequestL() TID: %d" ), iTID );
    CHttpTransportAdapter::WriteToLog( _L8( "  Content-Length: %d" ), iContentLength );
    CHttpTransportAdapter::WriteToLog( _L8( "  Actual length: %d" ), dataLength );
    if ( iContentLength >= 0 && iContentLength < KMaxTInt )
        {
        if ( iContentLength == dataLength )
            CHttpTransportAdapter::WriteToLog( _L8( "  Correct" ) );
        else
            CHttpTransportAdapter::WriteToLog( _L8( "  Actual length and the value of Content-Length header do not match!" ) );
        }
#endif
    if ( dataLength > 0 )
        {
#ifdef _DEBUG
        CHttpTransportAdapter::WriteToLog( _L8( "  Gather data & complete the request" ) );
#endif
        //CleanupStack is not used here, since there are no leaving methods
        //before the ownership of the allocated data is transferred in the
        //TransportResponse() method call on client's side
        HBufC8* wholeData = HBufC8::NewL( dataLength );
        TPtr8 pointer( wholeData->Des() );
        iResponseData->Read( 0, pointer, dataLength );
        iResponseData->Reset();
        iCurrentDataLength = 0;
        iTransportAdapter->ReceiverHandle().TransportResponse( iTID, aErrorCode,
                                                               HttpStatus(), wholeData );
        //So - let's hope the guys on the other side remember
        //to release the data...!?
        }
    else
        {
#ifdef _DEBUG
        CHttpTransportAdapter::WriteToLog( _L8( "  No data for this transaction" ), dataLength );
#endif
        iTransportAdapter->ReceiverHandle().TransportResponse( iTID, aErrorCode,
                                                               HttpStatus(), NULL );
        }
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::ReleaseData
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::ReleaseData()
    {
    /* Not just yet... We MAY want to resend the data.
    iRequestData->Des().Zero();*/
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::DoReleaseData
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::DoReleaseData()
    {
    //Response has arrived, the request data can be deleted now.
    delete iRequestData;
    iRequestData = NULL;
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::ResendL
//
// ----------------------------------------------------------
//
TBool CImpsHttpTransaction::ResendL()
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L( "CImpsHttpTransaction::ResendL(), iTID: %d" ), iTID );
#endif
    if ( iNumberOfRetries < KMaxNumberOfRetries )
        {
        //Close the old transaction
        iHttpTransaction.Close();
        iConstructed = EFalse;
        //Create a new one and send it to the stack.
        ConstructRequestL();
        DispatchMessageL();
#ifdef _DEBUG
        CHttpTransportAdapter::WriteToLog( _L( "  Transaction %d retransmitted." ), iTID );
#endif
        return ETrue;
        }
    else return EFalse;
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::SetStatus
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::SetStatus( const TBool aSent )
    {
    iSent = aSent;
    if ( iSent )
        {
        if ( iExpiryTimer != NULL && !iExpiryTimer->IsActive() )
            iExpiryTimer->ActivateTimer( iExpiryTime );
        }
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L8( "CImpsHttpTransaction::SetStatus(): Status %d, TID: %d" ), iSent, iTID );
#endif
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::SetStatus
//
// ----------------------------------------------------------
//
const TInt CImpsHttpTransaction::HttpStatus() const
    {
    return iHttpTransaction.Response().StatusCode();
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::SetExpiryTimeL
//
// ----------------------------------------------------------
//
void CImpsHttpTransaction::SetExpiryTimeL( const TTimeIntervalMicroSeconds32 aExpiryTime,
                                           MImpsTransportTimerCallback* aCallback )
    {
    if ( iExpiryTimer == NULL )
        iExpiryTimer = CImpsTransportTimer::NewL( aCallback, this );
    iExpiryTime = aExpiryTime;
    }

// ----------------------------------------------------
// CImpsHttpTransaction::CopyRequestData
//
// ----------------------------------------------------
//
void CImpsHttpTransaction::CopyRequestDataL( const TDesC8& aRequestData )
    {
#ifdef _DEBUG
    CHttpTransportAdapter::WriteToLog( _L( "CImpsHttpTransaction::CopyRequestDataL()" ) );
#endif
    delete iRequestData;
    iRequestData = NULL;
    iRequestData = HBufC8::NewL( aRequestData.Length() );
    iRequestData->Des().Copy( aRequestData );
    }

#ifdef _DEBUG

// ---------------------------------------------------------
// CHttpTestAppView::ConstructL
//
// ---------------------------------------------------------
//
TInt CImpsHttpTransaction::TimeL() const
    {
    TInt period = 0;
    User::LeaveIfError( HAL::Get( HALData::ESystemTickPeriod, period ) );
    TInt millisecsPerTick = period / 1000;
    return User::TickCount() * millisecsPerTick;
    }

#endif

