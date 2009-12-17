/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Globally used enumerations
*
*/



#ifndef TSTORAGEMANAGERGLOBALS_H
#define TSTORAGEMANAGERGLOBALS_H

class TStorageManagerGlobals
    {
    public: // enumerations

        enum TPresenceStatus
            {
            EUnknown = -1,
            EOffline,
            EOnline,
            EInvisible,
            EAway,
            EBusy
            };

        enum TClientType
            {
            EUnknownClient = 0,
            EMobile,
            EPC
            };

        enum TCAStorageType
            {
            EUnknownStorageType = -1,
            EStoragePersistent,
            EStorageMemory
            };

        // properties
        enum TCAStorageProperty
            {
            EStoragePropNickname = 0,
            EStoragePropAlias,
            EStoragePropUserId
            };

        // observer event types
        enum TCAObserverEventType
            {
            EStorageEventBackupRestoreStarted = 0,
            EStorageEventBackupRestoreEnded,
            EStorageEventPreChange,
            EStorageEventPostChange,
            EStorageEventGroupChange,
            EStorageEventContactAddition,
            EStorageEventContactDelete,
            EStorageEventListAddition,
            EStorageEventListChanged,
            EStorageEventListDelete,
            EStorageEventOwnStatusChange,
            EStorageEventMultipleChanges
            };

        // Filter type for getting right contacts
        // Tells us what to show; for example EFilterAll
        // does not filter out any statuses: all contacts are shown
        enum TFilterType
            {
            EFilterNotDefined = -1,
            EFilterAll = 0,
            EFilterAndOperation = 1,
            EFilterOnline = 2,
            EFilterNonBlocked = 4,
            EFilterAway = 8,
            EFilterNonOffline = 16 // this means online and away
            };
    };

#endif // TSTORAGEMANAGERGLOBALS_H
