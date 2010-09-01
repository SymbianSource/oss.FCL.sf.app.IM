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
* Description: Adapter for Http transport.
*
*
*/


// INCLUDE FILES
#include <in_iface.h>
#include <impserrors.h>
#include <msgconnmanagerapi.h>
#include "HttpTransportAdapter.h"
#include "ImpsHttpTransaction.h"
#ifdef _DEBUG
#include "flogger.h"
#endif

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CHttpTransportAdapter::CHttpTransportAdapter
//
// ----------------------------------------------------------
//
CHttpTransportAdapter::CHttpTransportAdapter( MImpsDataReceiver& aReceiver,
                                              MMsgConnManager& aConnManager ) :
        iSuspend( EFalse ),
        iSessionClosed( ETrue ),
        iReceiver( aReceiver ),
        iConnManager( aConnManager )
    {
    }

// ----------------------------------------------------------
// CHttpTransportAdapter::NewL
//
// ----------------------------------------------------------
//
CHttpTransportAdapter* CHttpTransportAdapter::NewL( MImpsDataReceiver& aReceiver,
                                                    MMsgConnManager& aConnManager,
                                                    const TDesC8& aMimeType )
    {
    CHttpTransportAdapter* self = new ( ELeave ) CHttpTransportAdapter( aReceiver, aConnManager );
    CleanupStack::PushL( self );
    self->ConstructL( aMimeType );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------------
// CHttpTransportAdapter::ConstructL
//
// ----------------------------------------------------------
//
void CHttpTransportAdapter::ConstructL( const TDesC8& aMimeType )
    {
#ifdef _DEBUG
    DeleteLogFilesL();
#endif
    SetMimeTypeL( aMimeType );
    }

// ----------------------------------------------------
// CHttpTransportAdapter::~CHttpTransportAdapter()
// Destructor
// ----------------------------------------------------
//
CHttpTransportAdapter::~CHttpTransportAdapter()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CHttpTransportAdapter::~CHttpTransportAdapter(). Destructor called." ) );
#endif
    iHttpSession.Close();
    iSessionClosed = ETrue;
    iTransactionQueue.ResetAndDestroy();
    iTransactionQueue.Close();
    delete iUrlBuffer;
    delete iProxyBuffer;
    delete iMimeBuffer;
#ifdef _DEBUG
    WriteToLog( _L8( "  Destructor ended." ) );
#endif
    }

// ----------------------------------------------------
// CHttpTransportAdapter::InitialiseSessionL
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::InitialiseSessionL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CHttpTransportAdapter::InitialiseSessionL()" ) );
#endif
    iHttpSession.OpenL();
    RSocketServ& session = iConnManager.SocketSession();
    RConnection& connection = iConnManager.Connection();
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    RStringPool stringPool = iHttpSession.StringPool();
    TInt conn = reinterpret_cast <TInt> ( &connection );
    connInfo.SetPropertyL( stringPool.StringF( HTTP::EHttpSocketServ, RHTTPSession::GetTable() ),
                           THTTPHdrVal( session.Handle() ) );
    connInfo.SetPropertyL( stringPool.StringF( HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
                           THTTPHdrVal( conn ) );
    connInfo.SetPropertyL( stringPool.StringF( HTTP::EHTTPVersion, RHTTPSession::GetTable() ),
                           THTTPHdrVal( stringPool.StringF( HTTP::EHttp11, RHTTPSession::GetTable() ) ) );
    iSessionClosed = EFalse;
#ifdef _DEBUG
    WriteToLog( _L8( "InitialiseSessionL() ends." ) );
#endif
    }

// ----------------------------------------------------
// CHttpTransportAdapterder::CancelTransaction
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::CancelTransaction( const TInt aTID )
    {
#ifdef _DEBUG
    WriteToLog( _L( "CHttpTransportAdapter::CancelTransaction(): TID %d" ), aTID );
#endif
    TInt count = iTransactionQueue.Count();
#ifdef _DEBUG
    WriteToLog( _L( "  %d items in transaction queue." ), count );
#endif
    for ( TInt i = 0;i < count;i++ )
        {
        CImpsHttpTransaction* transaction = iTransactionQueue[i];
        if ( transaction->TID() == aTID )
            {
#ifdef _DEBUG
            WriteToLog( _L( "    Request TID %d set to cancelled state." ), transaction->TID() );
#endif
            RemoveFromQueue( transaction );
            break;
            }
        }
    }

// ----------------------------------------------------
// CHttpTransportAdapter::CancelAll()
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::CancelAll()
    {
#ifdef _DEBUG
    WriteToLog( _L( "CHttpTransportAdapter::CancelAll()" ) );
#endif
    CImpsHttpTransaction* transaction = NULL;
    TInt count = iTransactionQueue.Count();
#ifdef _DEBUG
    WriteToLog( _L( "  %d items in transaction queue" ), count );
#endif
    for ( TInt i = 0;i < count;i++ )
        {
        transaction = iTransactionQueue[i];
        delete transaction;
        transaction = NULL;
#ifdef _DEBUG
        WriteToLog( _L( "    Item %d deleted from transaction queue" ), i );
#endif
        }
    iTransactionQueue.Reset();
    }

// ----------------------------------------------------
// CHttpTransportAdapter::SendL
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::SendL( const TInt aTID, const TDesC8& aMessage,
                                   const TInt aExpiryTime )
    {
#ifdef _DEBUG
    WriteToLog( _L( "SendL() called, aTID: %d" ), aTID );
#endif
    //GPRS is suspended
    if ( iSuspend )
        {
#ifdef _DEBUG
        WriteToLog( _L( "SendL(): GPRS suspended, leaves with \"KImpsErrorBearerSuspended\"." ) );
#endif
        User::Leave( KImpsErrorBearerSuspended );
        }
    //OpenL() has not been called
    else if ( iUrlBuffer == NULL )
        {
#ifdef _DEBUG
        WriteToLog( _L( "SendL(): No SAP defined, leaves with KImpsErrorSessionNotOpen" ) );
#endif
        User::Leave( KImpsErrorSessionNotOpen );
        }
    else
        {
        CImpsHttpTransaction* transaction = CImpsHttpTransaction::NewL( this, aTID, aMessage );
        CleanupStack::PushL( transaction );
        //Append the transaction to the end of the list of pending
        //requests. The request gets removed as soon as a response
        //arrives or timer expires.
        TInt error = iTransactionQueue.Append( transaction );
        if ( error != KErrNone )
            User::Leave( error );
        else
            {
            CleanupStack::Pop();  //transaction
            //If the client application wants to time this request,
            //aExpiryTime parameter should not be 0. If it is,
            //ignore it; if not, instantiate timer and set this
            //object as the receiver of the prospective expiry event.
            //This requires the MImpsTransportTimerCallback interface
            //and HandleTransportTimerEvent function to be implemented
            if ( aExpiryTime != 0 )
                transaction->SetExpiryTimeL( /* The timer expects milliseconds,
                                              * so multiply by million. */
                    aExpiryTime * 1000000, this );
            //If suspend occurs after this,
            //there is absolutely nothing that can be done
            if ( iConnManager.Status() )
                transaction->DispatchMessageL();
            else
                User::Leave( KImpsErrorBearerSuspended );
            }
        }
    }

// ----------------------------------------------------
// CHttpTransportAdapter::Open
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::OpenL( const TDesC& aSAP )
    {
#ifdef _DEBUG
    TBuf8<256> eightBuf;
    eightBuf.Copy( aSAP );
    WriteToLog( _L8( "CHttpTransportAdapter::OpenL()" ) );
    WriteToLog( _L8( " SAP: %S" ), &eightBuf );
#endif
    if ( !iConnManager.Status() )
        User::Leave( KImpsErrorBearerSuspended );
    if ( iSessionClosed )
        {
        InitialiseSessionL();
        SetUrlL( aSAP );
        SetProxyL();
#ifdef _DEBUG
        WriteToLog( _L8( "  URL: %S" ), &iDefaultURL.UriDes() );
#endif
        iConnManager.AddEventSubscriberL( this );
        }
    else
        {
#ifdef _DEBUG
        WriteToLog( _L8( "  OpenL(): Already open, leaves with KErrAlreadyExists." ) );
#endif
        User::Leave( KErrAlreadyExists );
        }
    }

// ----------------------------------------------------
// CHttpTransportAdapter::Close
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::Close()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CHttpTransportAdapter::Close()" ) );
#endif
    if ( iTransactionQueue.Count() > 0 )
        CancelAll();
    if ( iSuspend )
        iSuspend = EFalse;
    delete iUrlBuffer;
    iUrlBuffer = NULL;
    iHttpSession.Close();
    iSessionClosed = ETrue;
    iConnManager.RemoveEventSubscriber( this );
    }

// ----------------------------------------------------
// CHttpTransportAdapter::SetProxyL
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::SetProxyL()
    {
    //It must be checked if there is the
    //dummy proxy defined in CommsDb
    _LIT8( KPortSeparatorColon, ":" );
    _LIT( KDummyProxy, "www.dummyproxy.com" );
#ifdef _DEBUG
    WriteToLog( _L8( "CHttpTransportAdapter::SetProxyL" ) );
#endif
    delete iProxyBuffer;
    iProxyBuffer = NULL;
    HBufC* address = iConnManager.ReadFromCommsDbLC( EMsgProxyAddress );
    if ( address != NULL && address->CompareF( KDummyProxy ) != 0 )
        {
        HBufC* port = iConnManager.ReadFromCommsDbLC( EMsgProxyPort );
        iProxyBuffer = HBufC8::NewL( address->Length() +
                                     port->Length() + 1 );  //":"
        iProxyBuffer->Des().Copy( *address );
        iProxyBuffer->Des().Append( KPortSeparatorColon );
        iProxyBuffer->Des().Append( *port );
        CleanupStack::PopAndDestroy( 2 );  //port, address
#ifdef _DEBUG
        TPtrC8 proxy( *iProxyBuffer );
        WriteToLog( _L8( "  Proxy set to: %S" ), &proxy );
#endif
        RStringF proxyAddress = iHttpSession.StringPool().OpenFStringL( iProxyBuffer->Des() );
        RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
        THTTPHdrVal proxyUsage( iHttpSession.StringPool().StringF( HTTP::EUseProxy, RHTTPSession::GetTable() ) );
        connInfo.SetPropertyL( iHttpSession.StringPool().StringF( HTTP::EProxyUsage, RHTTPSession::GetTable() ), proxyUsage );
        THTTPHdrVal proxyAddr( proxyAddress );
        connInfo.SetPropertyL( iHttpSession.StringPool().StringF( HTTP::EProxyAddress, RHTTPSession::GetTable() ), proxyAddr );
        proxyAddress.Close();
        }
    else
        {
        if ( address != NULL )
            {
#ifdef _DEBUG
            HBufC8* temp = HBufC8::NewL( address->Length() );
            temp->Des().Copy( *address );
            TPtrC8 proxy( temp->Des() );
            WriteToLog( _L8( "  Improper proxy (%S) defined, ignore" ), &proxy );
            delete temp;
            temp = NULL;
#endif
            CleanupStack::PopAndDestroy();  //address
            }
        else
            {
#ifdef _DEBUG
            WriteToLog( _L8( "  No proxy defined for this Access Point" ) );
#endif
            }
        }
    }

// ----------------------------------------------------
// CHttpTransportAdapter::SetMimeTypeL
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::SetMimeTypeL( const TDesC8& aMimeType )
    {
    if ( aMimeType.Length() > 0 )
        {
        delete iMimeBuffer;
        iMimeBuffer = NULL;
        iMimeBuffer = HBufC8::NewL( aMimeType.Length() );
        iMimeBuffer->Des().Copy( aMimeType );
#ifdef _DEBUG
        TPtr8 ptr( iMimeBuffer->Des() );
        WriteToLog( _L8( "CHttpTransportAdapter::SetMimeTypeL(): %S" ), &ptr );
#endif
        }
    }

// ----------------------------------------------------
// CHttpTransportAdapter::SetUrlL
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::SetUrlL( const TDesC& aUrl )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CHttpTransportAdapter::SetUrl()" ) );
#endif
    _LIT( KHttp, "http://" );
    delete iUrlBuffer;
    iUrlBuffer = NULL;
    if ( aUrl.FindF( KHttp ) == 0 )
        iUrlBuffer = HBufC8::NewL( aUrl.Length() );
    else
        {
        iUrlBuffer = HBufC8::NewL( aUrl.Length() + 7 );
        iUrlBuffer->Des().Copy( KHttp );
        }
    iUrlBuffer->Des().Append( aUrl );
    iDefaultURL.Parse( iUrlBuffer->Des() );
    }

// ----------------------------------------------------------
// CHttpTransportAdapter::HandleTransportTimerEventL
//
// ----------------------------------------------------------
//
void CHttpTransportAdapter::HandleTransportTimerEventL( TImpsTimingRequester* aTransaction,
                                                        const TInt /*aStatus*/ )
    {
    CImpsHttpTransaction* transaction = ( CImpsHttpTransaction* ) aTransaction;
    TInt httpStatus = transaction->HttpStatus();
#ifdef _DEBUG
    WriteToLog( _L8( "CHttpTransportAdapter::HandleTransportTimerEventL()  Request: %d" ), transaction->TID() );
#endif
    iReceiver.TransportResponse( transaction->TID(), KErrTimedOut, httpStatus, NULL );
    RemoveFromQueue( transaction );
    }

// ----------------------------------------------------
// CHttpTransportAdapter::MHFRunL
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent )
    {
    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
            RHTTPResponse response = aTransaction.Response();
            TInt contentLength = ContentLengthL( response );
            action->SetContentLength( contentLength );
#ifdef _DEBUG
            TInt status = response.StatusCode();
            RStringF statusStr = response.StatusText();
            TBuf<32> statusStr16;
            statusStr16.Copy( statusStr.DesC() );
            WriteToLog( _L( "CHttpTransportAdapter::MHFRunL(), Status: %d (%S), TID: %d, Round-Trip: %d ms" ),
                        status, &statusStr16, action->TID(),
                        action->TimeL() - action->SendTime() );
            DumpResponseHeadersL( aTransaction, action->TID() );
#endif
            //If this transaction has been cancelled, it must be handled now.
            if ( action->IsCancelled() )
                {
                //This does cancelling automagically
#ifdef _DEBUG
                WriteToLog( _L( "CHttpTransportAdapter::MHFRunL(), TID: \"%d\" was cancelled" ), action->TID() );
#endif
                RemoveFromQueue( action );
                break;
                }
            switch ( contentLength )
                {
                case 0:
                    //Content-Length of the response may or may not be 0.
                    //If it is, the response is most likely a poll response.
#ifdef _DEBUG
                    WriteToLog( _L8( "CHttpTransportAdapter::MHFRunL(), Content-Length 0, no body to wait for." ) );
                    WriteToLog( _L8( "CHttpTransportAdapter::MHFRunL(), Calling receiver with a TID \"%d\"" ), action->TID() );
#endif
                    iReceiver.TransportResponse( action->TID(), KErrNone,
                                                 action->HttpStatus(), NULL );
                    RemoveFromQueue( action );
                    break;
                case KMaxTInt:
                    /*
                    It may be, however, that the Transfer-Encoding = "chunked",
                    in which case missing Content-Length is perfectly acceptable.
                    Thus, if the response body is supposed to arrive in chunks,
                    ignore this event and wait for the next one. In other words,
                    let the HTTP stack decide what to do next.
                    This case is essentially identical to the default case, but
                    rendering it this way might make the fact a bit more obvious and,
                    thus, the code also a bit more readable. The compiler probably
                    knows how to optimise this, anyway.
                    */
                    break;
                default:
                    break;
                }
            }
        break;
        case THTTPEvent::EGotResponseBodyData:
            {
            TPtrC8 bodyData;
            CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
#ifdef _DEBUG
            WriteToLog( _L8( "MHFRunL(): a body part received - TID: %d" ), action->TID() );
#endif
            //The request data can now be deleted
            action->DoReleaseData();
            // Get the body data supplier
            iRespBody = aTransaction.Response().Body();
            TBool lastChunk = iRespBody->GetNextDataPart( bodyData );
            action->AppendDataL( bodyData, lastChunk );
            // Done with that bit of body data
            iRespBody->ReleaseData();
            if ( lastChunk )
                {
#ifdef _DEBUG
                WriteToLog( _L8( "  Last chunk - TID: %d" ), action->TID() );
#endif
                action->FinaliseRequestL( KErrNone );
                }
            }
        break;
        case THTTPEvent::EResponseComplete:
            {
            //The response of the transaction is complete
#ifdef _DEBUG
            WriteToLog( _L( "CHttpTransportAdapter::MHFRunL(), Transaction Complete" ) );
#endif
            }
        break;
        case THTTPEvent::ESucceeded:
            {
            CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
            //If iLastChunk is not ETrue, the framework has not changed it when
            //it delivered the payload data. For some reason the framework
            //does not return ETrue when GetNextDataPart() is called while the
            //value of the Transfer-Encoding header is "chunked". Hence we must
            //call the client application now.
            if ( !action->LastChunk() )
                {
#ifdef _DEBUG
                WriteToLog( _L8( "CHttpTransportAdapter::MHFRunL(), Transaction finished, but iLastChunk = EFalse." ) );
                WriteToLog( _L8( "  Complete the transaction %d" ), action->TID() );
#endif
                action->FinaliseRequestL( KErrNone );
                }
            else
                {
#ifdef _DEBUG
                WriteToLog( _L( "CHttpTransportAdapter::MHFRunL(), Transaction Successful. TID: %d" ), action->TID() );
#endif
                }
            RemoveFromQueue( action );
            }
        break;
        case THTTPEvent::EFailed:
            {
            CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
#ifdef _DEBUG
            WriteToLog( _L( "Transaction Failed. TID: %d" ), action->TID() );
#endif
            if ( !action->LastChunk() )
                action->FinaliseRequestL( KErrGeneral );
            RemoveFromQueue( action );
            }
        break;
        case THTTPEvent::ERedirectedPermanently:
            {
#ifdef _DEBUG
            CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
            WriteToLog( _L( "  SAP redirected permanently - TID: %d" ), action->TID() );
#endif
            //For the time being, both redirections are handled the same way.
            //Which is that they're not handled here at all, but in the HTTP Stack...
            //HandleTemporaryRedirectionL( aTransaction );
            //HandlePermanentRedirectionL( aTransaction );
            }
        break;
        case THTTPEvent::ERedirectedTemporarily:
            {
#ifdef _DEBUG
            CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
            WriteToLog( _L( "  SAP redirected temporarily - TID: %d" ), action->TID() );
#endif
            //HandleTemporaryRedirectionL( aTransaction );
            }
        break;
        default:
            {
#ifdef _DEBUG
            WriteToLog( _L( "<Unrecognised event: %d>" ), aEvent.iStatus );
#endif
            TInt error = ConvertUndefinedError( aEvent.iStatus );
#ifdef _DEBUG
            WriteToLog( _L( "  Error %d converted to %d" ), aEvent.iStatus, error );
#endif
            CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
            //Should we try to resend the request in case it fails?
            //This has been an issue of some controversy, so just in case
            //let's try the following procedure:
            if ( error < 0 )
                {
                //Right, it failed for some reason. These two guys are the
                //errors that we see most often, so give them a special
                //treatment - try to send the request again. There is a
                //const value in CImpsHttpTransaction KMaxNumberOfRetries
                //which is followed at this point; if Resend() returns with EFalse,
                //the request has spent all its "lives" and it gets the boot.
                if ( action != NULL &&
                     ( error == KErrEof ||
                       error == KErrDisconnected ) )
                    {
                    if ( !action->ResendL() )
                        {
                        iReceiver.TransportResponse( action->TID(), error,
                                                     action->HttpStatus(), NULL );
                        RemoveFromQueue( action );
                        }
                    }
                else
                    {
                    //There is little that can be done: the status is none of the ones
                    //we're interested in, neither is it a system-wide error that might
                    //be meaningful to the receiver. Forward the status to the receiver
                    //nevertheless and delete the transaction, it is quite useless now.
                    iReceiver.TransportResponse( action->TID(), error,
                                                 action->HttpStatus(), NULL );
                    RemoveFromQueue( action );
                    }
                }
            else
                {
                //We don't know what this is... A positive number, but none of
                //the HTTP specific ones. Simply ignore the response.
                if ( action != NULL && error != 10 )
                    {
                    iReceiver.TransportResponse( action->TID(), error,
                                                 action->HttpStatus(), NULL );
                    RemoveFromQueue( action );
                    }
                else if ( error == 10 )
                    {
                    //Do nothing, this will return once more in THTTPEvent::EFailed.
                    //The meaning of this status is a bit unclear.
                    }
                }
            }
        break;
        }
    }

// ----------------------------------------------------------
// CImpsHttpTransactionSender::HandleTemporaryRedirectionL
//
// ----------------------------------------------------------
//
void CHttpTransportAdapter::HandleTemporaryRedirectionL( RHTTPTransaction aTransaction )
    {
    RHTTPResponse response = aTransaction.Response();
    TPtrC8 newURL( LocationL( response ) );
    CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
    if ( newURL.Length() > 0 && action != NULL )
        {
        delete iUrlBuffer;
        iUrlBuffer = NULL;
        iUrlBuffer = HBufC8::NewL( newURL.Length() );
        iUrlBuffer->Des().Copy( newURL );
        action->ResendL();
#ifdef _DEBUG
        TPtr8 ptr( iUrlBuffer->Des() );
        WriteToLog( _L8( "CHttpTransportAdapter::HandleTemporaryRedirectionL() - TID %d" ), action->TID() );
        WriteToLog( _L8( "   New SAP address assigned: %S" ), &ptr );
        WriteToLog( _L8( "   Transaction retransmitted to the new destination" ) );
#endif
        }
    else
        {
#ifdef _DEBUG
        WriteToLog( _L8( "CHttpTransportAdapter::HandleTemporaryRedirectionL() - TID %d" ), action->TID() );
        WriteToLog( _L8( "   The SAP Server did not return a redirect address, cannot continue." ) );
#endif
        iReceiver.TransportResponse( action->TID(), KErrCouldNotConnect,
                                     action->HttpStatus(), NULL );
        RemoveFromQueue( action );
        }
    }

// ----------------------------------------------------------
// CImpsHttpTransactionSender::HandlePermanentRedirectionL
//
// ----------------------------------------------------------
//
void CHttpTransportAdapter::HandlePermanentRedirectionL( RHTTPTransaction /*aTransaction*/ )
    {

    }

// ----------------------------------------------------------
// CImpsHttpTransactionSender::ConvertUndefinedError
//
// ----------------------------------------------------------
//
TInt CHttpTransportAdapter::ConvertUndefinedError( const TInt aError )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "ConvertUndefinedError(): %d" ), aError );
#endif
    TInt error = 0;
    switch ( aError )
        {
        case -5120:
            error = KErrIfDNSNotFound;
            break;
        default:
            error = aError;
        }
    return error;
    }

// ----------------------------------------------------------
// CImpsHttpTransactionSender::ImpsTransaction
//
// ----------------------------------------------------------
//
CImpsHttpTransaction* CHttpTransportAdapter::ImpsTransaction( const RHTTPTransaction& aTransaction )
    {
    TInt count = iTransactionQueue.Count();
    CImpsHttpTransaction* traverse = NULL;
    CImpsHttpTransaction* ret = NULL;
    for ( TInt i = 0;i < count;i++ )
        {
        traverse = iTransactionQueue[i];
        if ( traverse->Transaction() == aTransaction )
            {
            ret = traverse;
            break;
            }
        }
    return ret;
    }

// ----------------------------------------------------------
// CImpsHttpTransactionSender::ContentLengthL
//
// ----------------------------------------------------------
//
TInt CHttpTransportAdapter::ContentLengthL( RHTTPResponse aResponse ) const
    {
    TInt retVal = KMaxTInt;
    _LIT8( KContentLength, "Content-Length" );
    THTTPHdrVal fieldValue;
    RStringPool stringPool = iHttpSession.StringPool();
    RHTTPHeaders headers = aResponse.GetHeaderCollection();
    RStringF lengthString = stringPool.OpenFStringL( KContentLength );
    headers.GetField( lengthString, 0, fieldValue );
    lengthString.Close();
    if ( fieldValue.Type() == THTTPHdrVal::KTIntVal )
        {
#ifdef _DEBUG
        WriteToLog( _L8( "CHttpTransportAdapter::ContentLengthL(): %d" ), fieldValue.Int() );
#endif
        retVal = fieldValue.Int();
        }
    return retVal;
    }

// ----------------------------------------------------------
// CImpsHttpTransactionSender::LocationL
//
// ----------------------------------------------------------
//
TPtrC8 CHttpTransportAdapter::LocationL( RHTTPResponse aResponse ) const
    {
    TPtrC8 sapAddress;
    _LIT8( KLocation, "Location" );
    THTTPHdrVal fieldValue;
    RStringPool stringPool = iHttpSession.StringPool();
    RHTTPHeaders headers = aResponse.GetHeaderCollection();
    RStringF location = stringPool.OpenFStringL( KLocation );
    headers.GetField( location, 0, fieldValue );
    location.Close();
    if ( fieldValue.Type() == THTTPHdrVal::KStrVal ||
         fieldValue.Type() == THTTPHdrVal::KStrFVal )
        {
        RString address = fieldValue.Str();
        sapAddress.Set( address.DesC() );
#ifdef _DEBUG
        TPtrC8 dump( address.DesC() );
        WriteToLog( _L8( "CHttpTransportAdapter::LocationL(): %S" ), &dump );
#endif
        }
    return sapAddress;
    }

// ----------------------------------------------------------
// CHttpTransportAdapter::RemoveFromQueue
//
// ----------------------------------------------------------
//
void CHttpTransportAdapter::RemoveFromQueue( const CImpsHttpTransaction* aRemove )
    {
    TInt index = iTransactionQueue.Find( aRemove );
    if ( index >= 0 )
        {
#ifdef _DEBUG
        WriteToLog( _L8( "CHttpTransportAdapter::RemoveFromQueue( transaction* ), TID: \"%d\"" ), aRemove->TID() );
#endif
        iTransactionQueue.Remove( index );
        delete aRemove;
        aRemove = NULL;
        }
    }

// ----------------------------------------------------
// CHttpTransportAdapter::MHFRunError
// HTTP Stack callback interface
// ----------------------------------------------------
//
TInt CHttpTransportAdapter::MHFRunError( TInt aInt, RHTTPTransaction aTransaction,
                                         const THTTPEvent& /*aEvent*/ )
    {
    CImpsHttpTransaction* action = ImpsTransaction( aTransaction );
#ifdef _DEBUG
    WriteToLog( _L( "CHttpTransportAdapter::MHFRunError(): %d" ), aInt );
    WriteToLog( _L( "Calling receiver with a TID \"%d\" and closing the corresponding request." ), action->TID() );
#endif
    iReceiver.TransportResponse( action->TID(), aInt,
                                 action->HttpStatus(), NULL );
    RemoveFromQueue( action );
    return KErrNone;
    }

// ----------------------------------------------------------
// CHttpTransportAdapter::HandleBearerEventL
//
// ----------------------------------------------------------
//
void CHttpTransportAdapter::HandleBearerEventL( TBool aIsAuthClose, TMsgBearerEvent aBearerEvent )
    {
#ifdef _DEBUG
    WriteToLog( _L( "CHttpTransportAdapter::HandleBearerEventL() - Event: %d  AuthClose: %d" ),
                aBearerEvent, aIsAuthClose );
#endif
    switch ( aBearerEvent )
        {
        case EMsgBearerSuspended:
#ifdef _DEBUG
            WriteToLog( _L( "  Bearer suspended" ) );
#endif
            iSuspend = ETrue;
            break;
        case EMsgBearerActive:
#ifdef _DEBUG
            WriteToLog( _L( "  Bearer active" ) );
#endif
            iSuspend = EFalse;
            break;
        case EMsgBearerLost:
#ifdef _DEBUG
            WriteToLog( _L( "  Bearer lost, close data channel" ) );
#endif
            Close();
            break;
        default:
            break;
        }
    }

#ifdef _DEBUG

// ----------------------------------------------------
// CHttpTransportAdapter::WriteToLog
//
// ----------------------------------------------------
//
/*TPtrC8 CHttpTransportAdapter::Name() const
    {
    return TPtrC8( _L8( "DataChannel" ) );
    }*/

// ----------------------------------------------------
// CHttpTransportAdapter::WriteToLog
// Writes to the log, 8-bit version
// ----------------------------------------------------
//
void CHttpTransportAdapter::WriteToLog( TRefByValue<const TDesC8> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KLogDir, KTransportLogFile, EFileLoggingModeAppend, buf );
    }

// ----------------------------------------------------
// CHttpTransportAdapter::RunL
// Writes to the log, UNICODE version
// ----------------------------------------------------
//
void CHttpTransportAdapter::WriteToLog( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KLogDir, KTransportLogFile, EFileLoggingModeAppend, buf );
    }

// ----------------------------------------------------------
// CHttpTransportAdapter::DeleteLogFiles
//
// ----------------------------------------------------------
//
void CHttpTransportAdapter::DeleteLogFilesL()
    {
    RFs session;
    User::LeaveIfError( session.Connect() );
    CFileMan* manager = CFileMan::NewL( session );
    manager->Delete( _L( "C:\\logs\\ImpsDataChannel\\*.*" ) );
    session.Close();
    delete manager;
    manager = NULL;
    }

// ----------------------------------------------------
// CHttpTransportAdapter::DumpResponseHeadersL
//
// ----------------------------------------------------
//
void CHttpTransportAdapter::DumpResponseHeadersL( RHTTPTransaction& aTrans,
                                                  const TInt aTransactionID )
    {
    WriteToLog( _L( "--------------------" ) );
    WriteToLog( _L( "Headers of the transaction %d: " ), aTransactionID );
    RHTTPResponse resp = aTrans.Response();
    RStringPool strP = aTrans.Session().StringPool();
    RHTTPHeaders hdr = resp.GetHeaderCollection();
    THTTPHdrFieldIter it = hdr.Fields();

    TBuf<KMaxHeaderNameLen>  fieldName16;
    TBuf<KMaxHeaderValueLen> fieldVal16;

    while ( it.AtEnd() == EFalse )
        {
        RStringTokenF fieldName = it();
        RStringF fieldNameStr = strP.StringF( fieldName );
        THTTPHdrVal fieldVal;
        if ( hdr.GetField( fieldNameStr, 0, fieldVal ) == KErrNone )
            {
            const TDesC8& fieldNameDesC = fieldNameStr.DesC();
            fieldName16.Copy( fieldNameDesC.Left( KMaxHeaderNameLen ) );
            switch ( fieldVal.Type() )
                {
                case THTTPHdrVal::KTIntVal:
                    WriteToLog( _L( "%S: %d\n" ), &fieldName16, fieldVal.Int() );
                    break;
                case THTTPHdrVal::KStrFVal:
                    {
                    RStringF fieldValStr = strP.StringF( fieldVal.StrF() );
                    const TDesC8& fieldValDesC = fieldValStr.DesC();
                    fieldVal16.Copy( fieldValDesC.Left( KMaxHeaderValueLen ) );
                    WriteToLog( _L( "%S: %S\n" ), &fieldName16, &fieldVal16 );
                    }
                break;
                case THTTPHdrVal::KStrVal:
                    {
                    RString fieldValStr = strP.String( fieldVal.Str() );
                    const TDesC8& fieldValDesC = fieldValStr.DesC();
                    fieldVal16.Copy( fieldValDesC.Left( KMaxHeaderValueLen ) );
                    WriteToLog( _L( "%S: %S\n" ), &fieldName16, &fieldVal16 );
                    }
                break;
                case THTTPHdrVal::KDateVal:
                    {
                    TDateTime date = fieldVal.DateTime();
                    TBuf<40> dateTimeString;
                    TTime t( date );
                    t.FormatL( dateTimeString, KDateFormat );
                    WriteToLog( _L( "%S: %S\n" ), &fieldName16, &dateTimeString );
                    }
                break;
                default:
                    WriteToLog( _L( "%S: <unrecognised value type>\n" ), &fieldName16 );
                    break;
                }

            // Display realm for WWW-Authenticate header
            RStringF wwwAuth = strP.StringF( HTTP::EWWWAuthenticate, RHTTPSession::GetTable() );
            if ( fieldNameStr == wwwAuth )
                {
                // check the auth scheme is 'basic'
                RStringF basic = strP.StringF( HTTP::EBasic, RHTTPSession::GetTable() );
                RStringF realm = strP.StringF( HTTP::ERealm, RHTTPSession::GetTable() );
                THTTPHdrVal realmVal;
                if ( ( fieldVal.StrF() == basic ) &&
                     ( !hdr.GetParam( wwwAuth, realm, realmVal ) ) )
                    {
                    RStringF realmValStr = strP.StringF( realmVal.StrF() );
                    fieldVal16.Copy( realmValStr.DesC() );
                    WriteToLog( _L( "Realm is: %S\n" ), &fieldVal16 );
                    }
                }
            }
        ++it;
        }
    WriteToLog( _L( "--------------------" ) );
    }

#endif

// ----------------------------------------------------
// CHttpTransportAdapter::NewImpsSenderL
// Returns a sender instance
// ----------------------------------------------------
//
EXPORT_C MImpsSender* NewImpsSenderL( MImpsDataReceiver& aReceiver,
                                      MMsgConnManager& aConnManager,
                                      const TDesC8& aMimeType )
    {
    return CHttpTransportAdapter::NewL( aReceiver, aConnManager, aMimeType );
    }


// End of File
