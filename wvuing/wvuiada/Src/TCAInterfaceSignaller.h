/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides interface to register and unregister all interfaces.
*
*/



#ifndef TCAINTERFACESIGNALLER_H
#define TCAINTERFACESIGNALLER_H

// CLASS DECLARATION

/**
*  Used to signal interfaces in certain situations.
*
*  @lib CAAdapter
*  @since 1.2
*/
class TCAInterfaceSignaller
    {
    public: // New functions

        /**
        * Connects to IMPS engine (server)
        * Leaves with KErrNotFound if factory is not in TLS
        * @since 2.1
        */
        static void ConnectL();

        /**
        * Register all interfaces to WV Engine.
        * Leaves with KErrNotFound if factory is not in TLS
        */
        static void RegisterAllL();

        /**
        * Unregister all interfaces to WV Engine.
        * Leaves with KErrNotFound if factory is not in TLS
        */
        static void UnregisterAllL();

    private:

        /**
        * C++ default constructor.
        */
        TCAInterfaceSignaller();
    };

#endif      // TCAINTERFACESIGNALLER_H

// End of File
