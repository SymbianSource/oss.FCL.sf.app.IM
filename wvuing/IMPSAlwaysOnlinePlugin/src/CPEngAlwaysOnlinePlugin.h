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
* Description:  Implements the CAlwaysOnlineEComInterface interface
*
*/


#ifndef __PENGALWAYSONLINEPLUGIN_H__
#define __PENGALWAYSONLINEPLUGIN_H__

#include <e32base.h>
#include <alwaysonlineecominterface.h>

// FORWARD DECLARATIONS
class CAlwaysOnlineManagerServer;
class CIMPSAlwaysOnlinePluginImp;
class CPEngAOCallObserver;
class CIMPSReconnectNoteHandler;

/**
*  Implements the CAlwaysOnlineEComInterface interface
*
*  @lib PEngAOPlugin.lib
*  @since 2.5
*/
class CPEngAlwaysOnlinePlugin : public CAlwaysOnlineEComInterface
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CPEngAlwaysOnlinePlugin* NewL();

        /**
        * Default destructor
        */
        virtual ~CPEngAlwaysOnlinePlugin();

        /**
        * Interface function from CAlwaysOnlineEComInterface
        * Command ID's can be found in alwaysonlinemanagercommon.h
        * Handles the command coming from the Always Online Manager
        * @param aCommand The command as an integer
        * @param aParameters A Buffer containing parameters
        * @return TAny A return value, KErrNone if successfull
        *							   KErrNotSupported if command wasn't supported
        */
        TAny* HandleServerCommandL( TInt aCommand, TDesC8* aParameters );

    private:
        /**
        * Default constructor.
        */
        CPEngAlwaysOnlinePlugin();

        /**
        * Default Symbian constructor
        */
        void ConstructL();

        /**
         * Cleanup method for releasing feature manager library
         * @param aPtr, pointer to object which is target of cleanup operation
         */
        static void CleanupFeatureManager( TAny *aPtr );

    private:
        /// the Instant Messaging always online plugin implementation, owned
        CIMPSAlwaysOnlinePluginImp* iIMAOPlugin;
    };

#endif //__PENGALWAYSONLINEPLUGIN_H__
