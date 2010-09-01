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
* Description: Inline function assemble for IMPS Cir watcher
*    
*/


#include <ecom/ecom.h>

// ----------------------------------------------------
// CImpsTcpCirWatcher::CImpsTcpCirWatcher
// 
// ----------------------------------------------------
//
inline CImpsTcpCirWatcher::CImpsTcpCirWatcher() : CActive( EPriorityStandard )
	{
	}

// ----------------------------------------------------
// CImpsTcpCirWatcher::~CImpsTcpCirWatcher
// 
// ----------------------------------------------------
//
inline CImpsTcpCirWatcher::~CImpsTcpCirWatcher()
	{
	REComSession::DestroyedImplementation( iDestructorKey );
    }

// ----------------------------------------------------
// CImpsTcpCirWatcher::NewL
// Create a new TCP/IP CIR Watcher
// ----------------------------------------------------
//
inline CImpsTcpCirWatcher* CImpsTcpCirWatcher::NewL( const TInt aPingInterval,
    MImpsCirReceiver& aCirReceiver, MMsgConnManager& aConnManager )
    {
    TUid KDefaultImplemantationUid = { 0x101FB0EB };
    TTcpWatcherParameters params( aPingInterval, aCirReceiver, aConnManager );
    TAny* ptr = REComSession::CreateImplementationL( KDefaultImplemantationUid,
                                                     _FOFF( CImpsTcpCirWatcher,
                                                     iDestructorKey ),
                                                     ( TAny* )&params );
    return reinterpret_cast<CImpsTcpCirWatcher*>( ptr );
    }

//************** UDP Watcher **************

// ----------------------------------------------------
// CImpsUdpCirWatcher::CImpsUdpCirWatcher
// 
// ----------------------------------------------------
//
inline CImpsUdpCirWatcher::CImpsUdpCirWatcher()
	{
	}

// ----------------------------------------------------
// CImpsUdpCirWatcher::~CImpsUdpCirWatcher
// 
// ----------------------------------------------------
//
inline CImpsUdpCirWatcher::~CImpsUdpCirWatcher()
	{	    
    }
    
// ----------------------------------------------------
// CImpsUdpCirWatcher* CImpsUdpCirWatcher::NewL
// Create a new UDP/IP CIR Watcher
// ----------------------------------------------------
//
inline CImpsUdpCirWatcher* CImpsUdpCirWatcher::NewL()
    {
    TUid destructorKey;
    TUid KDefaultImplemantationUid = { 0x101FB0EC };
    TAny* ptr = REComSession::CreateImplementationL( KDefaultImplemantationUid, destructorKey );
    CImpsUdpCirWatcher* self = reinterpret_cast<CImpsUdpCirWatcher*>( ptr );
    self->StoreDestructorKeyL( destructorKey );
    return self;
    }
    
// ----------------------------------------------------
// CUdpCirWatcher::Destroy
// 
// ----------------------------------------------------
//
inline void CImpsUdpCirWatcher::Destroy()
	{
	if( iDestructorKeyArray.Count() > 0 )
	    {
	    REComSession::DestroyedImplementation( iDestructorKeyArray[0] );
	    iDestructorKeyArray.Remove( 0 );
	    if( iDestructorKeyArray.Count() <= 0 )
	        iDestructorKeyArray.Close();
	    }
    }

// ----------------------------------------------------
// CUdpCirWatcher::DeleteSingleton
// 
// ----------------------------------------------------
//
inline void CImpsUdpCirWatcher::DeleteSingleton()
	{
    while( iDestructorKeyArray.Count() > 0 )
        {
        REComSession::DestroyedImplementation( iDestructorKeyArray[0] );
        iDestructorKeyArray.Remove( 0 );
        }
    iDestructorKeyArray.Close(); 
    }
	
	