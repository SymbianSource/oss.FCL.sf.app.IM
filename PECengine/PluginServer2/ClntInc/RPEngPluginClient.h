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
* Description:  See class description below.
*
*/


#ifndef __RPENGPLUGINCLIENT_H__
#define __RPENGPLUGINCLIENT_H__

//  INCLUDES
#include <e32std.h>

//  CONSTANTS

/**
*  Main class of plugin server client side.
*
*  Features:
*
*   - Load individual or all plugins
*       - Also load all plugins with exception list
*   - Unload individual or all plugins
*   - Observer plugin status
*   - Shutdown or restart plugin server
*   - Inform plugins about state changes
*   - Verify if plugins are running
*   - Get list of running plugins
*   - Send messages to plugins
*
*  @lib PEngPlgSrv2.lib
*  @since 3.0
*/
class RPEngPluginClient : public RSessionBase
    {

    public:     // Constructors and destructors

        IMPORT_C RPEngPluginClient();
        IMPORT_C ~RPEngPluginClient();

    public: //Server management methods

        /**
        * Connects to plugin server. If the server is not running, it is started.
        *
        * @since 3.0
        * @param aOOMRate is deterministic OOM failure rate. Works only in debug builds. For testing only!
        * @return TInt is error code which indicates if connecting was successfull.
        */
        IMPORT_C TInt Connect( TInt aOOMRate = 0 );
        TInt ConnectWithoutStarting();

        /**
        * Close connection to plugin server. Doesn't shutdown server session.
        *
        * @since 3.0
        */
        IMPORT_C void Close();

        /**
        * Informs plugin server about network state changes
        *
        * @param aOnlineStatus is ETrue if network session was opened
        * @return TInt KErrNone if everything went ok
        * @since 3.0
        */
        TInt SetOnlineState( TBool aOnlineStatus );

        /**
        * Shutdown plugin server process
        *
        * Close called automatically.
        *
        * @since 3.0
        */
        TInt ShutdownServer();

    public: //Plugin handling methods

        /**
        * Count of plugins running at the moment.
        *
        * @since 3.0
        * @return TUint is count of plugins.
        */
        IMPORT_C TUint PluginCountL();

        /**
        * Gets the plugin's uid by the index
        *
        * @since 3.0
        * @param aIndex The index of the plugin in server's
        *               held plugins
        * @return
        */
        IMPORT_C TInt PluginUidL( TInt aIndex );


    };

#endif      //  __RPENGPLUGINCLIENT_H__