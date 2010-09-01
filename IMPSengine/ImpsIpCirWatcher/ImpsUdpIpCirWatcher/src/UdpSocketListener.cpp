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
* Description: Listener for Udp socket.
*
*
*/

#include "UdpCirWatcher.h"
#include "UdpSocketListener.h"
#include "ImpsIpCirWatcherAPI.h"

// ----------------------------------------------------
// CUdpSocketListener::ServicePort
//
// ----------------------------------------------------
//
CUdpSocketListener::CUdpSocketListener( const TInt aServicePort ) :
        CActive( EPriorityStandard ),
        iServicePort( aServicePort ),
        iDestructed( EFalse )
    {
    }

// ----------------------------------------------------
// CUdpSocketListener::NewL
//
// ----------------------------------------------------
//
CUdpSocketListener* CUdpSocketListener::NewL( const TInt aServicePort,
                                              const MImpsCirReceiver& aReceiver )
    {
    CUdpSocketListener* self = new ( ELeave ) CUdpSocketListener( aServicePort );
    CleanupStack::PushL( self );
    self->ConstructL( aReceiver );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CUdpSocketListener::ServicePort
//
// ----------------------------------------------------
//
void CUdpSocketListener::ConstructL( const MImpsCirReceiver& aReceiver )
    {
    User::LeaveIfError( iReceiverQueue.Append( CONST_CAST( MImpsCirReceiver*, &aReceiver ) ) );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
// CUdpSocketListener::ServicePort
//
// ----------------------------------------------------
//
const TInt CUdpSocketListener::ServicePort() const
    {
    return iServicePort;
    }

// ----------------------------------------------------
// CUdpSocketListener::~CUdpSocketListener
//
// ----------------------------------------------------
//
CUdpSocketListener::~CUdpSocketListener()
    {
    Cancel();
    iListenerSocket.Close();
    iReceiverQueue.Close();
    iDestructed = ETrue;
    }

// ----------------------------------------------------
// CUdpSocketListener::Contains
//
// ----------------------------------------------------
//
TInt CUdpSocketListener::Contains( const MImpsCirReceiver& aReceiver ) const
    {
    return iReceiverQueue.Find( &aReceiver );
    }

// ----------------------------------------------------
// CUdpSocketListener::RemoveReceiver
//
// ----------------------------------------------------
//
TBool CUdpSocketListener::RemoveReceiver( const MImpsCirReceiver& aReceiver )
    {
    TInt index = iReceiverQueue.Find( &aReceiver );
    iReceiverQueue.Remove( index );
    return iReceiverQueue.Count() > 0;
    }

// ----------------------------------------------------
// CUdpSocketListener::AppendReceiver
//
// ----------------------------------------------------
//
void CUdpSocketListener::AppendReceiverL( const MImpsCirReceiver& aReceiver )
    {
    User::LeaveIfError( iReceiverQueue.Append( &aReceiver ) );
    }

// ----------------------------------------------------
// CUdpSocketListener::ServicePort
//
// ----------------------------------------------------
//
TInt CUdpSocketListener::StartUdpSocketListen( RSocketServ& aSocketSession )
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::StartUdpSocketListen()" ) );
#endif
    TInt error = iListenerSocket.Open( aSocketSession, KAfInet, KSockDatagram, KProtocolInetUdp );
    if ( error == KErrNone )
        {
        iInetAddress = TInetAddr( KInetAddrAny, iServicePort );
        error = iListenerSocket.Bind( iInetAddress );
        if ( error == KErrNone )
            Activate();
        }
    else
        {
#ifdef _DEBUG
        CUdpCirWatcher::WriteToLog( _L8( "  Socket opening failed with error %d" ), error );
#endif
        }
    return error;
    }

// ----------------------------------------------------
// CUdpSocketListener::Activate
//
// ----------------------------------------------------
//
void CUdpSocketListener::Activate()
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::Activate() - Destructed: %d" ), iDestructed );
#endif
    if ( !iDestructed )
        {
        iSocketReceiveBuffer.Zero();
        iListenerSocket.RecvFrom( iSocketReceiveBuffer, iInetAddress, 0, iStatus );
#ifdef _DEBUG
        CUdpCirWatcher::WriteToLog( _L8( " UDP Listener started at port %d" ), iServicePort );
#endif
        SetActive();
        }
    }

// ----------------------------------------------------
// CUdpSocketListener::BroadcastMessageL
//
// ----------------------------------------------------
//
void CUdpSocketListener::BroadcastMessageL()
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpSocketListener::BroadcastMessageL()" ) );
#endif
    TInt count = iReceiverQueue.Count();
    if ( count > 0 )
        {
        for ( TInt i = 0;i < count;i++ )
            iReceiverQueue[i]->CirReceivedL( iSocketReceiveBuffer, EImpsUdpCir );
        }
    }

// ----------------------------------------------------
// CUdpSocketListener::BroadcastErrorL
//
// ----------------------------------------------------
//
void CUdpSocketListener::BroadcastErrorL( const TInt aError  )
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpSocketListener::BroadcastErrorL()" ) );
#endif
    TInt count = iReceiverQueue.Count();
    if ( count > 0 )
        {
        for ( TInt i = 0;i < count;i++ )
            iReceiverQueue[i]->HandleErrorL( aError, EImpsUdpCir );
        }
    }

// ----------------------------------------------------
// CUdpSocketListener::ServicePort
//
// ----------------------------------------------------
//
void CUdpSocketListener::RunL()
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpSocketListener::RunL() - iStatus: %d" ), iStatus.Int() );
#endif
    if ( iStatus == KErrCancel )
        {
#ifdef _DEBUG
        CUdpCirWatcher::WriteToLog( _L8( " Socket read cancelled." ) );
#endif
        }
    else
        {
        if ( iSocketReceiveBuffer.Length() > 0 )
            {
            if ( iSocketReceiveBuffer.Find( _L8( "WVCI" ) ) == 0 )
                {
#ifdef _DEBUG
                TBuf8<100> remoteAddr8;
                TBuf<100> remoteAddr;
                iInetAddress.Output( remoteAddr );
                remoteAddr8.Copy( remoteAddr );
                CUdpCirWatcher::WriteToLog( _L8( " CIR received from %S  Data: %S" ),
                                            &remoteAddr8, &iSocketReceiveBuffer );
#endif
                BroadcastMessageL();
                }
            else
                {
#ifdef _DEBUG
                CUdpCirWatcher::WriteToLog( _L8(
                                                " Receive buffer contains data, but the protocol was corrupted: %S" ),
                                            &iSocketReceiveBuffer );
#endif
                BroadcastErrorL( KErrCorrupt );
                }
            }
        else
            {
#ifdef _DEBUG
            CUdpCirWatcher::WriteToLog( _L8( "  No data, re-start listener" ) );
#endif
            }
        Activate();
        }
    }

// ----------------------------------------------------
// CUdpSocketListener::ServicePort
//
// ----------------------------------------------------
//
void CUdpSocketListener::DoCancel()
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpSocketListener::DoCancel()" ) );
#endif
    iListenerSocket.CancelRecv();
    }



