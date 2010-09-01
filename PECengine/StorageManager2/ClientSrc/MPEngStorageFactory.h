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
* Description:  It is used by the watchers to signal its state
*
*/



#ifndef __MPENGSTORAGEFACTORY_H__
#define __MPENGSTORAGEFACTORY_H__

//  INCLUDES

#include	<e32def.h>

// FORWARD DECLARATIONS
class CPEngStorageManagerWatcher;
class RPEngStorageClient;

/**
 *  Storage Manager Factory abstract interface
 *	It is used by the Watcher to signal its removal
 *
 *  @lib PEngStorMan2.dll
 *  @since 3.0
 */

// CLASS DECLARATION
class MPEngStorageFactory
    {
    public: // New functions

        /**
         *  Remove Storage watcher
         *
         *  @since 3.0
         *  @param Storage watcher
         */
        virtual void CloseStorageWatcher(
            CPEngStorageManagerWatcher* aStorageWatcher ) = 0;

        /**
         *  Increases reference counter of Factory
         *
         *  @since 3.0
         *  @param Storage watcher
         */
        virtual void OpenWatcher() = 0;

        /**
         *  Returns main client of the Storage Manager
         *
         *  @since 3.0
         *  @param reference to the storage manager main client
         */
        virtual RPEngStorageClient& MainClient() = 0;
    };

#endif      // __MPENGSTORAGEFACTORY_H__

// End of File
