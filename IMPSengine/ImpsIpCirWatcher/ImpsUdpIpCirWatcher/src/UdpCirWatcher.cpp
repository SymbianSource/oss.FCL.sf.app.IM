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
* Description: CirWatcher for UDP
*
*
*/


// INCLUDE FILES
#include <ecom/ecom.h>
#include <f32file.h>
#include <flogger.h>
#include <msgconnmanagerapi.h>
#include "UdpCirWatcher.h"
#include "UdpSocketListener.h"
#include "CirWatcherDefines.h"
#include "ImpsIpCirWatcherAPI.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CUdpCirWatcher::CUdpCirWatcher
//
// ----------------------------------------------------------
//
CUdpCirWatcher::CUdpCirWatcher() : iReferenceCount( 1 )
    {
    }

// ----------------------------------------------------------
// CUdpCirWatcher::NewL
//
// ----------------------------------------------------------
//
CUdpCirWatcher* CUdpCirWatcher::NewL()
    {
    CUdpCirWatcher* self = NULL;
    if ( Dll::Tls() == NULL )
        {
        self = new ( ELeave ) CUdpCirWatcher();
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop();
        Dll::SetTls( self );
        }
    else
        {
        self = static_cast<CUdpCirWatcher*>( Dll::Tls() );
        self->IncreaseReferenceCount();
        }
    return self;
    }

// ----------------------------------------------------
// CUdpCirWatcher::Destroy
// Destructor
// ----------------------------------------------------
//
void CUdpCirWatcher::Destroy()
    {
    DecreaseReferenceCount();
    CImpsUdpCirWatcher::Destroy();
    if ( ReferenceCount() == 0 )
        {
        Dll::FreeTls();
        delete this;
        }
    }

// ----------------------------------------------------
// CUdpCirWatcher::DeleteSingleton
// Destructor
// ----------------------------------------------------
//
void CUdpCirWatcher::DeleteSingleton()
    {
    CImpsUdpCirWatcher::DeleteSingleton();
    Dll::FreeTls();
    delete this;
    }

// ----------------------------------------------------
// CUdpCirWatcher::Destroy
//
// ----------------------------------------------------
//
void CUdpCirWatcher::StoreDestructorKeyL( const TUid aDestructorKey )
    {
    User::LeaveIfError( iDestructorKeyArray.Append( aDestructorKey ) );
    }

// ----------------------------------------------------------
// CUdpCirWatcher::ConstructL
//
// ----------------------------------------------------------
//
void CUdpCirWatcher::ConstructL()
    {
#ifdef _DEBUG
    DeleteLogFileL();
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::ConstructL()" ) );
#endif
    User::LeaveIfError( iSocketSession.Connect() );
    }

// ----------------------------------------------------------
// CTcpCirWatcher::DeleteLogFileL
//
// ----------------------------------------------------------
//
void CUdpCirWatcher::DeleteLogFileL()
    {
    RFs session;
    User::LeaveIfError( session.Connect() );
    CFileMan* manager = CFileMan::NewL( session );
    manager->Delete( _L( "C:\\Logs\\ImpsUdpWatcher\\*.*" ) );
    session.Close();
    delete manager;
    manager = NULL;
    }

// ----------------------------------------------------
// CUdpCirWatcher::~CUdpCirWatcher
// Destructor
// ----------------------------------------------------
//
CUdpCirWatcher::~CUdpCirWatcher()
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::~CUdpCirWatcher()" ) );
#endif
    iListenerQueue.ResetAndDestroy();
    iListenerQueue.Close();
    iSocketSession.Close();
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( " Destructor ends" ) );
#endif
    }

// ----------------------------------------------------------
// CUdpCirWatcher::FindListener
//
// ----------------------------------------------------------
//
TBool CUdpCirWatcher::FindListenerL( const TInt aServicePort,
                                     const MImpsCirReceiver& aCirReceiver )
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::FindListener()" ) );
#endif
    TBool found = EFalse;
    TInt count = iListenerQueue.Count();
    CUdpSocketListener* listener = NULL;
    for ( TInt i = 0;!found && i < count;i++ )
        {
        listener = iListenerQueue[i];
        if ( listener->ServicePort() == aServicePort )
            {
#ifdef _DEBUG
            CUdpCirWatcher::WriteToLog( _L8(
                                            "  Port %d already on watch, attach the new listener to it" ), aServicePort );
#endif
            found = ETrue;
            listener->AppendReceiverL( aCirReceiver );
            }
        }
    return found;
    }

// ----------------------------------------------------------
// CUdpCirWatcher::FindListener
//
// ----------------------------------------------------------
//
CUdpSocketListener* CUdpCirWatcher::FindListener( TInt& aIndex,
                                                  const TInt aServicePort ) const
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::FindListener()" ) );
#endif
    TBool found = EFalse;
    TInt count = iListenerQueue.Count();
    CUdpSocketListener* listener = NULL;
    for ( TInt i = 0;!found && i < count;i++ )
        {
        listener = iListenerQueue[i];
        if ( listener->ServicePort() == aServicePort )
            {
            found = ETrue;
            aIndex = i;
            }
        }
    return listener;
    }

// ----------------------------------------------------------
// CUdpCirWatcher::CreateListenerL
//
// ----------------------------------------------------------
//
TInt CUdpCirWatcher::CreateListenerL( const TInt aServicePort,
                                      const MImpsCirReceiver& aCirReceiver )
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::CreateListenerL()" ) );
#endif
    CUdpSocketListener* listener = CUdpSocketListener::NewL( aServicePort, aCirReceiver );
    CleanupStack::PushL( listener );
    User::LeaveIfError( iListenerQueue.Append( listener ) );
    CleanupStack::Pop();  //listener
    return listener->StartUdpSocketListen( iSocketSession );
    }

// ----------------------------------------------------------
// CUdpCirWatcher::RegisterCirReceiverL
//
// ----------------------------------------------------------
//
void CUdpCirWatcher::RegisterCirReceiverL( const MImpsCirReceiver& aCirReceiver,
                                           const TInt aServicePort )
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::RegisterCirReceiverL() - Port: %d" ), aServicePort );
#endif
    TInt count = iListenerQueue.Count();
    if ( count > 0 )
        {
        if ( !FindListenerL( aServicePort, aCirReceiver ) )
            User::LeaveIfError( CreateListenerL( aServicePort, aCirReceiver ) );
        }
    else
        User::LeaveIfError( CreateListenerL( aServicePort, aCirReceiver ) );
    }

// ----------------------------------------------------------
// CUdpCirWatcher::UnregisterCirReceiverL
//
// ----------------------------------------------------------
//
void CUdpCirWatcher::UnregisterCirReceiverL( const MImpsCirReceiver& aCirReceiver )
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::UnregisterCirReceiverL()" ) );
#endif
    TBool found = EFalse;
    TInt count = iListenerQueue.Count();
    if ( count > 0 )
        {
        CUdpSocketListener* listener = NULL;
        for ( TInt i = 0;!found && i < count;i++ )
            {
            listener = iListenerQueue[i];
            if ( listener->Contains( aCirReceiver ) >= 0 )
                {
                found = ETrue;
                if ( !listener->RemoveReceiver( aCirReceiver ) )
                    {
                    iListenerQueue.Remove( i );
                    delete listener;
                    listener = NULL;
                    }
                }
            }
        if ( !found )
            User::Leave( KErrNotFound );
        }
    }

// ----------------------------------------------------------
// CUdpCirWatcher::Reset
//
// ----------------------------------------------------------
//
void CUdpCirWatcher::Reset( const TInt aServicePort )
    {
#ifdef _DEBUG
    CUdpCirWatcher::WriteToLog( _L8( "CUdpCirWatcher::Reset() - Port: %d" ), aServicePort );
#endif
    CUdpSocketListener* listener = NULL;
    if ( aServicePort != KErrArgument )
        {
        TInt index = KErrNotFound;
        listener = FindListener( index, aServicePort );
        if ( listener != NULL && index >= 0 )
            {
            iListenerQueue.Remove( index );
            delete listener;
            listener = NULL;
            }
        }
    else
        {
        TInt count = iListenerQueue.Count();
        for ( TInt i = 0;i < count;i++ )
            {
            listener = iListenerQueue[i];
            iListenerQueue.Remove( i );
            delete listener;
            listener = NULL;
            }
        }
    }

// ---------------------------------------------------------
// CUdpCirWatcher::IncreaseReferenceCount
//
// ---------------------------------------------------------
//
void CUdpCirWatcher::IncreaseReferenceCount()
    {
    iReferenceCount++;
    }

// ---------------------------------------------------------
// CUdpCirWatcher::DecreaseReferenceCount
//
// ---------------------------------------------------------
//
void CUdpCirWatcher::DecreaseReferenceCount()
    {
    iReferenceCount--;
    }

// ----------------------------------------------------------
// CUdpCirWatcher::ReferenceCount
//
// ----------------------------------------------------------
//
const TInt CUdpCirWatcher::ReferenceCount() const
    {
    return iReferenceCount;
    }

#ifdef _DEBUG

// ----------------------------------------------------
// CCIRWatcher::WriteToLog
//
// ----------------------------------------------------
//
void CUdpCirWatcher::WriteToLog( TRefByValue<const TDesC8> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KUdpLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KUdpLogDir, KUdpWatcherLogFile, EFileLoggingModeAppend, buf );
    }

#endif

// End of File

