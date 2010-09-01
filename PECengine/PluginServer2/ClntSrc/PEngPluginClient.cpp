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
* Description:  PEngPluginServer - object factory
*
*/



// INCLUDE FILES
#include <e32base.h>

#include "PEngPluginClient.h"
#include "RPEngPluginClient.h"

//MACROS
#define RETURN_IF_ERROR( aErrorCode )\
    TInt eC(  aErrorCode );\
    if ( eC != KErrNone )\
        {\
        return eC;\
        }

// ============================ MEMBER FUNCTIONS ===============================

EXPORT_C TInt PEngPluginClient2::LaunchPluginServer()
    {
    // open plugin client and connect to server
    RPEngPluginClient client;
    RETURN_IF_ERROR( client.Connect() );

    return client.SetOnlineState( EFalse );
    }

EXPORT_C TInt PEngPluginClient2::OpenFirstSession()
    {
    RPEngPluginClient client;
    RETURN_IF_ERROR( client.Connect() );

    return client.SetOnlineState( ETrue );
    }

EXPORT_C TInt PEngPluginClient2::CloseLastSession()
    {
    RPEngPluginClient client;
    RETURN_IF_ERROR( client.Connect() );

    return client.SetOnlineState( EFalse );
    }

EXPORT_C TInt PEngPluginClient2::ShutdownPluginServer()
    {
    // open plugin client and unload all plugins
    RPEngPluginClient client;

    // do not start server if it is not running
    RETURN_IF_ERROR( client.ConnectWithoutStarting() );

    // shutdown the plugin server
    return client.ShutdownServer();
    }

// End of file

