/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Tcp CirWatcher
*
*
*/


// INCLUDE FILES
#include <ecom/ecom.h>
#include <f32file.h>
#ifdef _DEBUG
#include <flogger.h>
#endif
#include <msgconnmanagerapi.h>
#include <msgbearerObscallback.h>
#include "TcpCirWatcher.h"
#include "CirWatcherDefines.h"
#include "TcpCirWatcherTimer.h"

// ================= MEMBER FUNCTIONS =======================
//


// ----------------------------------------------------------
// CTcpCirWatcher::CTcpCirWatcher
//
// ----------------------------------------------------------
//
CTcpCirWatcher::CTcpCirWatcher( const TInt aPingInterval,
                                MImpsCirReceiver& aCirReceiver,
                                MMsgConnManager& aConnManager ) :
        iPingInterval( aPingInterval ),
        iCirReceiver( aCirReceiver ),
        iConnectionManager( aConnManager ),
        iClientRequestPending( EFalse ),
        iWatcherStatus( EImpsCIRWatcherIdle ),
        iLastGoodWatcherStatus( EImpsCIRWatcherUnknown )

    {
    }

// ----------------------------------------------------------
// CTcpCirWatcher::FinishConstructL
//
// ----------------------------------------------------------
//
CTcpCirWatcher* CTcpCirWatcher::NewL( TTcpWatcherParameters* aParameters )
    {
    CTcpCirWatcher* self = new ( ELeave ) CTcpCirWatcher( aParameters->iPingInterval,
                                                          aParameters->iCirReceiver,
                                                          aParameters->iConnManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CImpsConnectionMonitor::ConstructL
//
// ----------------------------------------------------------
//
void CTcpCirWatcher::ConstructL()
    {
#ifdef _DEBUG
    DeleteLogFileL();
#endif
    iWatcherTimer = CTcpCirWatcherTimer::NewL( this );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
// CTcpCirWatcher::~CTcpCirWatcher
// Destructor
// ----------------------------------------------------
//
CTcpCirWatcher::~CTcpCirWatcher()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::~CTcpCirWatcher()" ) );
#endif
    Cancel();
    delete iPacket;
    delete iWatcherTimer;
    delete iCSPSessionID;
    delete iSocketSendBuffer;
    delete iAddressBuffer;
    iTCPCIRSocket.Close();
#ifdef _DEBUG
    WriteToLog( _L8( "  Destructor ended" ) );
#endif
    }

// ----------------------------------------------------------
// CTcpCirWatcher::DeleteLogFileL()
//
// ----------------------------------------------------------
//
void CTcpCirWatcher::DeleteLogFileL()
    {
    RFs session;
    User::LeaveIfError( session.Connect() );
    CFileMan* manager = CFileMan::NewL( session );
    manager->Delete( _L( "C:\\Logs\\ImpsTcpWatcher\\*.*" ) );
    session.Close();
    delete manager;
    manager = NULL;
    }

// ----------------------------------------------------
// CTcpCirWatcher::SetPingInterval
//
// ----------------------------------------------------
//
void CTcpCirWatcher::SetPingInterval( const TInt aPingInterval )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::SetPingInterval()  Interval: %d" ), aPingInterval );
#endif
    if ( aPingInterval > 0 )
        iPingInterval = aPingInterval;
    }

// ----------------------------------------------------
// CTcpCirWatcher::StartTCPWatcherL
//
// ----------------------------------------------------
//
void CTcpCirWatcher::StartTCPWatcherL( TRequestStatus& aStatus,
                                       const TUint aServerPort,
                                       const TDesC& aServerAddress,
                                       const TDesC8& aCSPSessionID )
    {
#ifdef _DEBUG
    HBufC8* id = HBufC8::NewL( aCSPSessionID.Length() );
    TPtr8 idPtr( id->Des() );
    idPtr.Copy( aCSPSessionID );
    TBuf8<200> eightBuf;
    eightBuf.Copy( aServerAddress );
    WriteToLog( _L8( "CTcpCirWatcher::StartTCPWatcher()" ) );
    WriteToLog( _L8( "  Server:     %S" ), &eightBuf );
    WriteToLog( _L8( "  Port:       %d" ), aServerPort );
    WriteToLog( _L8( "  Session ID: %S" ), &idPtr );
    delete id;
    id = NULL;
#endif
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    if ( iWatcherStatus == EImpsCIRWatcherIdle )
        {
        Reset();
        SetConnectionDetailsL( aServerAddress, aCSPSessionID, aServerPort );
        if ( iConnectionManager.Status() )
            {
            iClientRequestPending = ETrue;
            iConnectionManager.AddEventSubscriberL( this );
            //Might be DNS name, must make sure we get an IP address
            User::LeaveIfError( iHostResolver.Open( iConnectionManager.SocketSession(),
                                                    KAfInet, KProtocolInetUdp,
                                                    iConnectionManager.Connection() ) );
            iHostResolver.GetByName( *iAddressBuffer, iHostAddressDnsEntry, iStatus );
            iWatcherStatus = EImpsCIRWatcherAwaitingDNS;
            SetActive();
#ifdef _DEBUG
            WriteToLog( _L8( "  Waiting for host resolver to complete" ) );
#endif
            }
        else
            {
#ifdef _DEBUG
            WriteToLog( _L8( "  Bearer is not ready, complete with KErrCouldNotConnect." ) );
#endif
            User::RequestComplete( iClientStatus, KErrCouldNotConnect );
            }
        }
    else
        {
#ifdef _DEBUG
        WriteToLog( _L8( "  Watcher is not ready, complete with KErrNotReady." ) );
#endif
        User::RequestComplete( iClientStatus, KErrNotReady );
        }
    }

// ----------------------------------------------------
// CTcpCirWatcher::SetConnectionDetails
//
// ----------------------------------------------------
//
void CTcpCirWatcher::SetConnectionDetailsL( const TDesC& aServerAddress,
                                            const TDesC8& aCSPSessionID,
                                            const TUint aServerPort )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::SetConnectionDetails" ) );
#endif
    iServerPort = aServerPort;
    iCSPSessionID = HBufC8::NewL( aCSPSessionID.Length() );
    iCSPSessionID->Des().Copy( aCSPSessionID );
    iAddressBuffer = HBufC::NewL( aServerAddress.Length() );
    iAddressBuffer->Des().Copy( aServerAddress );
    }


// ---------------------------------------------------------
// CTcpCirWatcher::Reset
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::Reset()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::Reset()" ) );
#endif
    delete iCSPSessionID;
    iCSPSessionID = NULL;
    delete iAddressBuffer;
    iAddressBuffer = NULL;
    iPingRetries = 0;
    iConnRetries = 0;
    }

// ----------------------------------------------------
// CTcpCirWatcher::StopTCPWatcher
// Destructor
// ----------------------------------------------------
//
void CTcpCirWatcher::StopTCPWatcher()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "StopTCPWatcher()" ) );
#endif
    if ( iWatcherStatus == EImpsCIRWatcherAwaitingDNS )
        {
#ifdef _DEBUG
        WriteToLog( _L8( "  Status is EImpsCIRWatcherAwaitingDNS, cancel resolver" ) );
#endif
        iHostResolver.Cancel();
        //If the client requests another start inside the scheduler
        //loop that completes the previous Start() request, the second
        //request will fail with KErrNotReady, since this component must
        //wait for the host resolver to return in RunL(). The method will
        //alter the state of this component to EImpsCIRWatcherIdle.
        //iClientRequestPending = EFalse;
        //iWatcherStatus = EImpsCIRWatcherStopping;
        //User::RequestComplete( iClientStatus, KErrCancel );
#ifdef _DEBUG
        WriteToLog( _L8( "  Resolver cancelled, request not completed" ) );
#endif
        }
    else
        {
        if ( iWatcherStatus != KImpsCIRWatcherStopping &&
             iWatcherStatus != EImpsCIRWatcherIdle )
            {
            iWatcherTimer->Cancel();
            if ( iConnectionManager.NumberOfActiveConns() > 0 )
                {

                // Socket Connection is getting disconnected and closed when Ending the PacketDataConnection
                // if active connection is not exists not required to cancel.
                iTCPCIRSocket.CancelAll();
                }
            if ( iClientRequestPending &&
                 ( iWatcherStatus == EImpsCIRWatcherConnectingTCPSocket ||
                   iWatcherStatus == EImpsCIRWatcherSendingHELOMessage ||
                   iWatcherStatus == EImpsCIRWatcherReceivingHELOResponse ) )
                {
#ifdef _DEBUG
                WriteToLog( _L8( "  A request is pending, complete with KErrCancel" ) );
#endif
                User::RequestComplete( iClientStatus, KErrCancel );
                iWatcherStatus = EImpsCIRWatcherIdle;
                }
            else
                iWatcherStatus = EImpsCIRWatcherStopping;
            iConnectionManager.RemoveEventSubscriber( this );
            }
        else
            {
#ifdef _DEBUG
            WriteToLog( _L8( "  Status is %d" ), iWatcherStatus );
#endif
            }
        }
    }

// ----------------------------------------------------
// CTcpCirWatcher::WatcherStatus
//
// ----------------------------------------------------
//
TUint CTcpCirWatcher::WatcherStatus() const
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::WatcherStatus()" ) );
#endif
    return iWatcherStatus;
    }

#ifdef _DEBUG

// ----------------------------------------------------
// CTcpCirWatcher::WriteToLog
//
// ----------------------------------------------------
//
void CTcpCirWatcher::WriteToLog( TRefByValue<const TDesC8> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KTcpLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KTcpLogDir, KTcpWatcherLogFile, EFileLoggingModeAppend, buf );
    }

#endif

// ---------------------------------------------------------
// CTcpCirWatcher::WaitForHELOResponse
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::WaitForHELOResponse()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::WaitForHELOResponse()" ) );
#endif
    iSocketReceiveBuffer.Zero();
    iStatus = KRequestPending;
    iWatcherStatus = EImpsCIRWatcherReceivingHELOResponse;
    iTCPCIRSocket.RecvOneOrMore( iSocketReceiveBuffer, 0, iStatus, iRecvLength );
    iWatcherTimer->ActivateTimer( 10000000, KWaitForHELOResponse );
    SetActive();
    }

// ---------------------------------------------------------
// CTcpCirWatcher::ConnectTCPSocket
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::ConnectTCPSocketL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::ConnectTCPSocket()  iConnRetries: %d" ), iConnRetries );
#endif
    iWatcherStatus = EImpsCIRWatcherConnectingTCPSocket;
    User::LeaveIfError( iTCPCIRSocket.Open( iConnectionManager.SocketSession(),
                                            KAfInet, KSockStream, KProtocolInetTcp,
                                            iConnectionManager.Connection() ) );
    if ( iConnRetries < KMaxNumberOfConnAttempts )
        {
        iTCPCIRSocket.Connect( iSocketAddress, iStatus );
        iConnRetries++;
        SetActive();
        iWatcherTimer->ActivateTimer( 20000000, KWaitForFirstConnectAttempt );
        }
    else
        {
        iTCPCIRSocket.Close();
        iCirReceiver.HandleErrorL( KErrCouldNotConnect, EImpsTcpCir );
        iWatcherStatus = EImpsCIRWatcherIdle;
        iConnectionManager.RemoveEventSubscriber( this );
#ifdef _DEBUG
        WriteToLog( _L8( "  Connect retry limit encountered, stop." ) );
#endif
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::SendHELOMessageL
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::SendHELOMessageL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::SendHELOMessageL()" ) );
#endif
    iWatcherStatus = EImpsCIRWatcherSendingHELOMessage;
    delete iSocketSendBuffer;
    iSocketSendBuffer = NULL;
    iSocketSendBuffer = HBufC8::NewL( 7 + iCSPSessionID->Length() );
    iSocketSendBuffer->Des().Copy( KHELOMessage );
    iSocketSendBuffer->Des().Append( *iCSPSessionID );
    iSocketSendBuffer->Des().Append( KMessageTermination );
    iStatus = KRequestPending;
    iTCPCIRSocket.Write( iSocketSendBuffer->Des(), iStatus );
    iWatcherTimer->ActivateTimer( 10000000, KWaitForHELOSendAttempt );
    SetActive();
    }

// ---------------------------------------------------------
// CTcpCirWatcher::SendPing
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::SendPingL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::SendPing()  iPingRetries: %d/%d" ), iPingRetries, KMaxNumberOfPingAttempts );
#endif
    if ( iPingRetries < KMaxNumberOfPingAttempts )
        {
        iWatcherStatus = EImpsCIRWatcherSendingPINGRequest;
        iTCPCIRSocket.Write( KImpsPingData, iStatus );
        iPingRetries++;
        SetActive();
        }
    else
        {
        //The server is down, close the socket and report error
        iTCPCIRSocket.Close();
        iCirReceiver.HandleErrorL( KErrDisconnected, EImpsTcpCir );
        iWatcherStatus = EImpsCIRWatcherIdle;
        iConnectionManager.RemoveEventSubscriber( this );
#ifdef _DEBUG
        WriteToLog( _L8( "  Ping retry limit encountered, stop." ) );
#endif
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::RunL
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::RunL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::RunL(), iStatus: %d" ), iStatus );
#endif
    iWatcherTimer->Cancel();
    switch ( iWatcherStatus )
        {
        case EImpsCIRWatcherAwaitingDNS:
            if ( iStatus == KErrNone )
                {
                iSocketAddress = TInetAddr( iHostAddressDnsEntry().iAddr );
#ifdef _DEBUG
                TBuf<200> buffer;
                TBuf8<200> eightBuffer;
                iSocketAddress.Output( buffer );
                eightBuffer.Copy( buffer );
                WriteToLog( _L8( "  IP address resolved (%S), starting to connect." ), &eightBuffer );
#endif
                iSocketAddress.SetPort( iServerPort );
                iHostResolver.Close();
                ConnectTCPSocketL();
                }
            else
                {
#ifdef _DEBUG
                WriteToLog( _L8( "  IP address resolving failed, complete with %d." ), iStatus.Int() );
#endif
                iConnectionManager.RemoveEventSubscriber( this );
                User::RequestComplete( iClientStatus, iStatus.Int() );
                iWatcherStatus = EImpsCIRWatcherIdle;
                }
            break;
        default:
            if ( iStatus == KErrNone )
                HandleRequestCompletionL();
            else
                HandleErrorL( iStatus.Int() );
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::HandleRequestCompletionL
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::HandleRequestCompletionL()
    {
    switch ( iWatcherStatus )
        {
        case EImpsCIRWatcherConnectingTCPSocket:
#ifdef _DEBUG
            WriteToLog( _L8( "  TCP Socket is connected, starting to send HELO." ) );
#endif
            iConnRetries = 0;
            SendHELOMessageL();
            break;
        case EImpsCIRWatcherSendingHELOMessage:
#ifdef _DEBUG
            WriteToLog( _L8( "  HELO Message sent, waiting for response." ) );
#endif
            WaitForHELOResponse();
            break;
        case EImpsCIRWatcherReceivingHELOResponse:
#ifdef _DEBUG
            WriteToLog( _L8( "  Server responded with the message: %S" ), &iSocketReceiveBuffer );
#endif
            if ( iClientRequestPending )
                {
#ifdef _DEBUG
                WriteToLog( _L8( "  Completing request & starting to PING the server in %d second intervals." ), iPingInterval );
#endif
                User::RequestComplete( iClientStatus, KErrNone );
                iClientRequestPending = EFalse;
                }
            WaitForNextPing();
            break;
        case EImpsCIRWatcherSendingPINGRequest:
#ifdef _DEBUG
            WriteToLog( _L8( "  Ping sent succesfully, wait for response" ) );
#endif
            iWatcherStatus = EImpsCIRWatcherWaitingPINGResponse;
            iWatcherTimer->ActivateTimer( 5000000, KWaitForPingReply );
            ReceiveFromSocket();
            break;
        case EImpsCIRWatcherWaitingPINGResponse:
#ifdef _DEBUG
            WriteToLog( _L8( "  Server responded to ping with the message: %S" ), &iSocketReceiveBuffer );
#endif
            if ( iSocketReceiveBuffer.Find( KImpsPingResponseWVCI ) == 0 )
                ReportCirL();
            else if ( iSocketReceiveBuffer.Find( KImpsPingResponseOK ) == 0 )
#ifdef _DEBUG
                WriteToLog( _L8( "  Server is OK." ) )
#endif
                ;
            else
                //Should this case include a reset/closure of the connection?
                //Clearly the received data does not comply to any of
                //the rules set by the OMA IMPS specification.
#ifdef _DEBUG
                WriteToLog( _L8( "  Do not talk to me like that.. learn some manners!" ) );
#endif
            ;
            iPingRetries = 0;
            WaitForNextPing();
            break;
        case KImpsCIRWatcherReceivingFromSocket:
#ifdef _DEBUG
            WriteToLog( _L8( "  Server sent: %S" ), &iSocketReceiveBuffer );
#endif
            if ( iSocketReceiveBuffer.Find( KImpsPingResponseWVCI ) == 0 )
                ReportCirL();
            if ( iWatcherStatus == KImpsCIRWatcherReceivingFromSocket )
                WaitForNextPing();
            break;
        default:
#ifdef _DEBUG
            WriteToLog( _L8( "  Default case, something's wrong." ) );
#endif
            break;
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::HandleErrorL
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::HandleErrorL( const TInt aError )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "  CTcpCirWatcher::HandleErrorL(), error: %d  iWatcherStatus: %d" ), aError, iWatcherStatus );
#endif
    switch ( iWatcherStatus )
        {
        case EImpsCIRWatcherConnectingTCPSocket:
        case EImpsCIRWatcherSendingHELOMessage:
        case EImpsCIRWatcherReceivingHELOResponse:
#ifdef _DEBUG
            WriteToLog( _L8( "    Error when establishing connection to CIR Server: %d  Client pending: %d" ), iStatus.Int(), iClientRequestPending );
#endif
            if ( iClientRequestPending )
                {
                iCompletionCode = iStatus == KErrCancel ? iCompletionCode : aError;
                CompleteWithError( iCompletionCode );
                }
            else
                iWatcherTimer->ActivateTimer( 10000000, KWaitForNextConnectAttempt );
            break;
        case EImpsCIRWatcherReceivingFromSocket:
            if ( aError == KErrCancel )
                {
#ifdef _DEBUG
                WriteToLog( _L8( "    Time to send the next ping request" )  );
#endif
                SendPingL();
                }
            else
                {
#ifdef _DEBUG
                WriteToLog( _L8( "    Socket read returned with error" )  );
#endif
                DetermineSocketStateL( aError );
                }
            break;
        case EImpsCIRWatcherSendingPINGRequest:
#ifdef _DEBUG
            WriteToLog( _L8( "  Error when sending PING: %d, try again." ), aError );
#endif
            SendPingL();
            break;
        case EImpsCIRWatcherWaitingPINGResponse:
#ifdef _DEBUG
            WriteToLog( _L8( "  Error when receiving PING response: %d, check socket state" ), aError );
#endif
            DetermineSocketStateL( aError );
            break;
        case EImpsCIRWatcherStopping:
            //Client has called StopTCPWatcher()
            if ( iStatus == KErrCancel )
                {
#ifdef _DEBUG
                WriteToLog( _L8( "  Watcher shutdown requested" )  );
#endif
                if ( iClientRequestPending )
                    {
#ifdef _DEBUG
                    WriteToLog( _L8( "  A request is pending, complete" )  );
#endif
                    User::RequestComplete( iClientStatus, KErrCancel );
                    }
                iTCPCIRSocket.Close();
                iHostResolver.Close();
                iWatcherStatus = EImpsCIRWatcherIdle;
                }
            break;
        default:
#ifdef _DEBUG
            WriteToLog( _L8( "  Default case, do nothing" ), aError );
#endif
            break;
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::DetermineSocketStateL
// NOTE: Close()s the current socket and opens a new one in
// each case. The aError parameter is used only for debug info.
// Change this feature if needed, since for instance a socket
// receive operation may fail/time out, in which case the socket
// is most probably still as good as new. Closing a perfectly
// good socket should not consume resources that prodigiously.
// That might not, however, be the most elegant solution, but it
// works fine just the same...
// ---------------------------------------------------------
//
void CTcpCirWatcher::DetermineSocketStateL( const TInt aError )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "    CTcpCirWatcher::DetermineSocketStateL()" )  );
    switch ( aError )
        {
            //The socket has been closed for some reason, try to connect again
        case KErrEof:
        case KErrDisconnected:
            WriteToLog( _L8( "    The socket was disconnected, re-open." )  );
            break;
            //Something seriously wrong, close the socket & open a new one
        case KErrNotReady:
            WriteToLog( _L8( "    Socket cannot be re-used anymore, close & re-open." )  );
            break;
            //Something timed out.
        case KErrTimedOut:
            WriteToLog( _L8( "    Socket operation timed out, close & re-open." )  );
            break;
        case KErrConnectionTerminated:
            WriteToLog( _L8( "    Socket Connection Terminated (Received KErrConnectionTerminated (-17210) )" )  );
            break;
        default:
            WriteToLog( _L8( "    Default case - Error: %d" ), aError  );
            break;
        }
#endif
    if ( aError == KErrConnectionTerminated )
        {
        // KErrConnectionTerminated = -17210 is declared in es_sock.h
        // Received this error when ending the packed data connection. in this case not required to connect again.
        // if received in other situations other than EndPacketDataConnection, may need to connect again
        iTCPCIRSocket.Close();
#ifdef _DEBUG
        WriteToLog( _L8( "Received Socket Connection Terminated Error - Closed the Socket Connection " )  );
#endif
        }
    else
        {
        iTCPCIRSocket.Close();
        ConnectTCPSocketL();
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::WaitForNextPing
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::WaitForNextPing()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::WaitForNextPing()" ) );
#endif
    iWatcherTimer->ActivateTimer( iPingInterval * 1000000, KWaitForNextPing );
    iWatcherStatus = EImpsCIRWatcherReceivingFromSocket;
    ReceiveFromSocket();
    }

// ---------------------------------------------------------
// CTcpCirWatcher::CheckAddressValidity
//
// ---------------------------------------------------------
//
TBool CTcpCirWatcher::CheckAddressValidity()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::CheckAddressValidity()" ) );
#endif
    TInetAddr destination;
    iTCPCIRSocket.RemoteName( destination );
    if ( destination == iSocketAddress )
        return ETrue;
    else return EFalse;
    }

// ---------------------------------------------------------
// CTcpCirWatcher::ReportCirL
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::ReportCirL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::ReportCirL()" ) );
#endif
    //Just a precaution: if the address has changed somewhere
    //along the line, our connection has been hijacked...
    //Though any serious attacker would probably spoof the IP
    //address, but it doesn't hurt to try.
    if ( CheckAddressValidity() )
        iCirReceiver.CirReceivedL( iSocketReceiveBuffer, EImpsTcpCir );
    else
        {
#ifdef _DEBUG
        WriteToLog( _L8( "  Wrong address, something's wrong. Close & report error" ) );
#endif
        iTCPCIRSocket.Close();
        iWatcherStatus = EImpsCIRWatcherIdle;
        iCirReceiver.HandleErrorL( KErrAbort, EImpsTcpCir );
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::ReceiveFromSocket
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::ReceiveFromSocket()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::ReceiveFromSocket()" ) );
#endif
    TUint flags = 0;
    iSocketReceiveBuffer.Zero();
    iStatus = KRequestPending;
    iTCPCIRSocket.RecvOneOrMore( iSocketReceiveBuffer, flags, iStatus, iRecvLength );
    SetActive();
    }

// ----------------------------------------------------
// CTcpCirWatcher::CompleteWithError
//
// ----------------------------------------------------
//
void CTcpCirWatcher::CompleteWithError( const TInt aError )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::CompleteWithError()" ) );
#endif
    iWatcherTimer->Cancel();
    User::RequestComplete( iClientStatus, aError );
    iConnectionManager.RemoveEventSubscriber( this );
    iWatcherStatus = EImpsCIRWatcherIdle;
    }

// ----------------------------------------------------
// CTcpCirWatcher::HandleTimerEventL
//
// ----------------------------------------------------
//
void CTcpCirWatcher::HandleTimerEventL( const TUint aExpiredRequest )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::HandleTimerEventL()" ) );
#endif
    switch ( aExpiredRequest )
        {
        case KWaitForFirstConnectAttempt:
#ifdef _DEBUG
            WriteToLog( _L8( "  Socket connect timed out, complete with KErrTimedOut" ) );
#endif
            iCompletionCode = KErrTimedOut;
            iTCPCIRSocket.CancelConnect();
            break;
#ifdef _DEBUG
        case KWaitForNextPing:
            //This will return in RunL() with KErrCancel
            WriteToLog( _L8( "  Socket read timed out, time to send the next ping" ) );
            iTCPCIRSocket.CancelRecv();
            break;
        case KWaitForPingReply:
            //This will return in RunL() with KErrCancel
            WriteToLog( _L8( "  Server didn't reply to ping, try again" ) );
            iTCPCIRSocket.CancelRecv();
            break;
#else
        case KWaitForNextPing:
        case KWaitForPingReply:
            //This will return in RunL() with KErrCancel
            iTCPCIRSocket.CancelRecv();
            break;
#endif
        case KWaitForHELOResponse:
            //The server didn't reply to the HELO response
#ifdef _DEBUG
            WriteToLog( _L8( "  Server didn't reply to HELO request, quit." ) );
#endif
            iTCPCIRSocket.CancelRecv();
            break;
        case KWaitForHELOSendAttempt:
            //Socket server is fooked...
#ifdef _DEBUG
            WriteToLog( _L8( "  Socket server didn't reply, quit." ) );
#endif
            iTCPCIRSocket.CancelWrite();
            break;
        case KWaitForNextConnectAttempt:
            ConnectTCPSocketL();
            break;
        default:
#ifdef _DEBUG
            WriteToLog( _L8( "  Default case" ) );
#endif
            break;
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::DoCancel
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::DoCancel()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::DoCancel()  Status: %d" ), iWatcherStatus );
#endif
    iWatcherTimer->Cancel();
    switch ( iWatcherStatus )
        {
        case EImpsCIRWatcherConnectingTCPSocket:
        case EImpsCIRWatcherSendingHELOMessage:
        case EImpsCIRWatcherReceivingHELOResponse:
#ifdef _DEBUG
            WriteToLog( _L8( "    Cancelling all socket operations" ) );
#endif
            iTCPCIRSocket.CancelAll();
            iWatcherStatus = EImpsCIRWatcherIdle;
            break;
        case EImpsCIRWatcherBearerSuspended:
            iTCPCIRSocket.CancelAll();
            break;
        default:
#ifdef _DEBUG
            WriteToLog( _L8( "    Default case, cancels all socket operations." ) );
#endif
            iTCPCIRSocket.CancelAll();
            break;
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::HandleBearerSuspension
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::HandleBearerSuspension()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::HandleBearerSuspension()" ) );
#endif
    iLastGoodWatcherStatus = iWatcherStatus;
    iWatcherStatus = EImpsCIRWatcherBearerSuspended;
    Cancel();
    iWatcherTimer->Cancel();
    switch ( iLastGoodWatcherStatus )
        {
        case EImpsCIRWatcherConnectingTCPSocket:
        case EImpsCIRWatcherSendingHELOMessage:
        case EImpsCIRWatcherReceivingHELOResponse:
            //If we're connecting & client is pending, complete with error
            if ( iClientRequestPending )
                {
                User::RequestComplete( iClientStatus, KErrCouldNotConnect );
                iLastGoodWatcherStatus = EImpsCIRWatcherUnknown;
                }
            break;
        default:
#ifdef _DEBUG
            WriteToLog( _L8( "   Default case." ) );
#endif
            break;
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::HandleBearerActivationLL
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::HandleBearerActivationL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::HandleBearerActivationL()" ) );
#endif
    iWatcherStatus = iLastGoodWatcherStatus;
    switch ( iWatcherStatus )
        {
        case EImpsCIRWatcherUnknown:
#ifdef _DEBUG
            WriteToLog( _L8( "   No active CIR channel exists, ignore." ) );
#endif
            iWatcherStatus = EImpsCIRWatcherIdle;
            break;
        case EImpsCIRWatcherSendingPINGRequest:
        case EImpsCIRWatcherWaitingPINGResponse:
        case KImpsCIRWatcherReceivingFromSocket:
#ifdef _DEBUG
            WriteToLog( _L8( "   Last good state was sending a request, send another." ) );
#endif
            SendPingL();
            break;
        case EImpsCIRWatcherConnectingTCPSocket:
#ifdef _DEBUG
            WriteToLog( _L8( "   Last good state was opening a socket, re-open." ) );
#endif
            iTCPCIRSocket.Close();
            ConnectTCPSocketL();
            break;
        default:
#ifdef _DEBUG
            WriteToLog( _L8( "   Default case." ) );
#endif
            break;
        }
    }

// ---------------------------------------------------------
// CTcpCirWatcher::HandleBearerEventL
//
// ---------------------------------------------------------
//
void CTcpCirWatcher::HandleBearerEventL( TBool aIsAuthClose, TMsgBearerEvent aBearerEvent )
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CTcpCirWatcher::HandleBearerEventL()  Event: %d  AuthClose: %d" ),
                aBearerEvent, aIsAuthClose );
#endif
    switch ( aBearerEvent )
        {
        case EMsgBearerSuspended:
#ifdef _DEBUG
            WriteToLog( _L8( "   Bearer suspended - State: %d" ), iWatcherStatus );
#endif
            //Do nothing if already in suspend state
            if ( iWatcherStatus != EImpsCIRWatcherBearerSuspended )
                HandleBearerSuspension();
            break;
        case EMsgBearerActive:
#ifdef _DEBUG
            WriteToLog( _L8( "   Bearer activated - State: %d." ), iWatcherStatus );
#endif
            //Act only if in suspend state
            if ( iWatcherStatus == EImpsCIRWatcherBearerSuspended )
                HandleBearerActivationL();
            break;
        case EMsgBearerLost:
#ifdef _DEBUG
            WriteToLog( _L8( "   Bearer lost, call it a day." ) );
#endif
            Cancel();
            iWatcherStatus = EImpsCIRWatcherIdle;
            iCirReceiver.HandleErrorL( EMsgBearerLost, EImpsTcpCir );
            break;
        default:
#ifdef _DEBUG
            WriteToLog( _L8( "   Default case, shouldn't be here..." ) );
#endif
            break;
        }
    }


// End of File