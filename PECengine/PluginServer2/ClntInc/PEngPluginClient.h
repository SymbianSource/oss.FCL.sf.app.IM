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

#ifndef __PENGPLUGINCLIENT_H
#define __PENGPLUGINCLIENT_H

//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * Plugin Server Factory
 *
 * @lib PEngPluginClient2
 * @since 3.0
 */

class PEngPluginClient2
    {

    public:

        /**
        * Launch plugin server process
        *
        * @since 3.0
        */
        IMPORT_C static TInt LaunchPluginServer();

        /**
        * Informs the plugin server that a connection
        * to network has been made.
        *
        * @since 3.0
        */
        IMPORT_C static TInt OpenFirstSession();

        /**
        * Informs the plugin server that the last
        * connection to network was closed
        *
        * @since 3.0
        */
        IMPORT_C static TInt CloseLastSession();

        /**
        * Shutdown plugin server process
        *
        * @since 3.0
        */
        IMPORT_C static TInt ShutdownPluginServer();

    private: // Prohibited constructors and destructor

        PEngPluginClient2();
        ~PEngPluginClient2();
    };

#endif // __PENGPLUGINCLIENT_H



