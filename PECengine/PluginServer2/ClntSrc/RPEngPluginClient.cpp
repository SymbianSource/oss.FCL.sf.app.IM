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
* Description:  Implementation of class RPEngPluginClient
*
*/


//  Include Files
#include <eikdll.h>

#include "RPEngPluginClient.h"
#include "PEngPlgSrvCommon.h"
#include "PresenceDebugPrint.h"
#include "PEngServerStarter.h"
#include <f32file.h>
#include <e32uid.h>

//MACROS
#define RETURN_IF_ERROR( aErrorCode )\
    TInt eC(  aErrorCode );\
    if ( eC != KErrNone )\
        {\
        return eC;\
        }

/*
 * How many message slots you need?
 * One for every asynchronous service, one for all synchronous services and one for cancel request.
 * For example if you have two asynchronous services and you have also synchronous services, you would need
 * 2 + 1 + 1 = 4 message slots.
 */
const TUint KMessageSlotCount = 4;

// DESTRUCTION
EXPORT_C RPEngPluginClient::~RPEngPluginClient()
    {
    Close();
    }

// CONSTRUCTION
EXPORT_C RPEngPluginClient::RPEngPluginClient()
    {
#if _BullseyeCoverage

#if defined(__DLL__)
    // This is needed only for coverage builds to solve bullseye atexit problem.
    // __DLL__ guards are needed because in test cases this code is compiled as exe.
    Dll::SetTls( NULL );
#endif

#endif
    }

EXPORT_C TInt RPEngPluginClient::Connect( TInt aOOMRate )
    {
    PENG_DP( D_PENG_LIT( "RPEngPluginClient::Connect start" ) );
    //Connects this RPEngStorageClient the server.
    //Launches new server process if one not yet running.
    return PEngServerStarter::ConnectServer( *this,
                                             KPEngPluginServerExe,
                                             TVersion( KClientVersionMajor,
                                                       KClientVersionMinor,
                                                       KClientVersionBuild ),
                                             KMessageSlotCount,
                                             KPEngPluginServerExe,
                                             aOOMRate );
    }

TInt RPEngPluginClient::ConnectWithoutStarting()
    {
    PENG_DP( D_PENG_LIT( "RPEngPluginClient::ConnectWithoutStarting start" ) );
    TInt err = RSessionBase::CreateSession( KPEngPluginServerExe,
                                            TVersion( KClientVersionMajor,
                                                      KClientVersionMinor,
                                                      KClientVersionBuild ) );

    PENG_DP( D_PENG_LIT( "RPEngPluginClient::ConnectWithoutStarting end [%d]" ), err );
    return err;
    }

EXPORT_C void RPEngPluginClient::Close()
    {
    //Do whatever is needed during cleanup.
    RSessionBase::Close();
    }

TInt RPEngPluginClient::SetOnlineState( TBool aOnlineStatus )
    {
    PENG_DP( D_PENG_LIT( "RPEngPluginClient::SetOnlineState start" ) );
    // if we are online, load all plugins, otherways load only offline ones
    if ( !aOnlineStatus )
        {
        RETURN_IF_ERROR( Send( EPEngPlgSetStateOffline ) );
        }
    else
        {
        RETURN_IF_ERROR( Send( EPEngPlgSetStateOnline ) );
        }

    PENG_DP( D_PENG_LIT( "RPEngPluginClient::SetOnlineState end" ) );
    return KErrNone;
    }

EXPORT_C TUint RPEngPluginClient::PluginCountL()
    {
    TInt pluginCount = SendReceive( EPEngPlgPluginCount );
    User::LeaveIfError( pluginCount );

    return pluginCount;
    }

EXPORT_C TInt RPEngPluginClient::PluginUidL( TInt aIndex )
    {
    TIpcArgs msgArgs;
    msgArgs.Set( 0, aIndex ); // use slot 0 for the index
    TInt pluginUid = SendReceive( EPEngPlgPluginUid, msgArgs );
    User::LeaveIfError( pluginUid );

    return pluginUid;
    }

TInt RPEngPluginClient::ShutdownServer()
    {
    PENG_DP( D_PENG_LIT( "RPEngPluginClient::ShutdownServer start" ) );
    TInt retVal = Send( EPEngPlgShutdownServer );
    Close();

    return retVal;
    }

//  End of File
