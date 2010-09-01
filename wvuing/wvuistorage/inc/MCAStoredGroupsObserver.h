/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Group data container observer interface
*
*/



#ifndef MCASTOREDGROUPSOBSERVER_H
#define MCASTOREDGROUPSOBSERVER_H

#include <e32std.h>
#include "TStorageManagerGlobals.h"

// CLASS DECLARATION

/**
 *  Interface for group data container observer
 *
 *  @lib CAStorage.dll
 *  @since 2.1
 */
class MCAStoredGroupsObserver
    {
    public: // New functions

        /**
         * Called when a group is deleted.
         * @since 2.1
         * @param aId The ID of the deleted group
         */
        virtual void HandleDelete( const TDesC& aId ) = 0;

        /**
         * Called when a group is added.
         * @since 2.1
         * @param aId The ID of the added group
         */
        virtual void HandleAddition( const TDesC& aId ) = 0;

        /**
         * Called when a group is changed.
         * @since 2.1
         * @param aId The ID of the changed group
         */
        virtual void HandleChange( const TDesC& aId ) = 0;

        /**
         * Called when backup/restore starts/stops.
         * There is no distinction between backup/restore (Symbian limitation).
         * However, the called party should fetch new list(s) as they
         * may have changed if the operation was restore operation.
         * @since 2.1
         * @param aEventType Type of backup/restore event
         */
        virtual void HandleBackupRestoreEvent(
            const TStorageManagerGlobals::TCAObserverEventType aEventType ) = 0;

        /**
         * Virtual destructor
         */
        virtual ~MCAStoredGroupsObserver() {};
    };

#endif      // MCASTOREDGROUPSOBSERVER_H

// End of File
