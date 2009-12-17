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
* Description:  Static interface of the storage manager
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "PEngStorageManager.h"
#include	"CPEngStorageManagerFactory.h"

#include	"CPEngStorageManager.h"
#include	"CPEngStorageManagerWatcher.h"
#include	"MPEngGlobalEventObserver.h"

//	Debug prints
#include	"PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngStorageManager::GetStorageManagerL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngStorageManager* PEngStorageManager::GetStorageManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngStorageManagerFactory* storageFactory =
        CPEngStorageManagerFactory::InstanceLC();
    MPEngStorageManager* manager =
        storageFactory->StorageManagerL( aSessionSlot );
    CleanupStack::PopAndDestroy(); //storageFactory
    return manager;
    }


// -----------------------------------------------------------------------------
// PEngStorageManager::GetStorageManagerWatcherL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngStorageManagerWatcher* PEngStorageManager::GetStorageManagerWatcherL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngStorageManagerFactory* storageFactory =
        CPEngStorageManagerFactory::InstanceLC();
    MPEngStorageManagerWatcher* watcher =
        storageFactory->StorageManagerWatcherL( aSessionSlot );
    CleanupStack::PopAndDestroy(); //storageFactory
    return watcher;
    }


// -----------------------------------------------------------------------------
// PEngStorageManager::CreateSessionFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngStorageManager::CreateSessionFolderL(
    const CPEngSessionSlotId& aSessionSlot,
    const TDesC& aApplicationId )
    {
    CPEngStorageManagerFactory* storageFactory =
        CPEngStorageManagerFactory::InstanceLC();
    storageFactory->CreateSessionFolderL( aSessionSlot, aApplicationId );
    CleanupStack::PopAndDestroy(); // storageFactory
    }


// -----------------------------------------------------------------------------
// PEngStorageManager::RemoveSessionFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngStorageManager::RemoveSessionFolderL(
    const CPEngSessionSlotId& aSessionSlot,
    const TDesC& aApplicationId )
    {
    CPEngStorageManagerFactory* storageFactory =
        CPEngStorageManagerFactory::InstanceLC();

    storageFactory->RemoveSessionFolderL( aSessionSlot, aApplicationId );
    CleanupStack::PopAndDestroy(); // storageFactory
    }


// -----------------------------------------------------------------------------
// PEngStorageManager::SessionSlotStateL
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngStorageManager::SessionSlotStateL(
    const CPEngSessionSlotId& aSessionSlot,
    CPEngSessionSlotState& aSessionSlotState )
    {
    CPEngStorageManagerFactory* storageFactory =
        CPEngStorageManagerFactory::InstanceLC();

    storageFactory->SessionSlotStateL( aSessionSlot, aSessionSlotState );
    CleanupStack::PopAndDestroy(); // storageFactory
    }


// -----------------------------------------------------------------------------
// PEngStorageManager::AllSessionSlotsStatesL
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngStorageManager::AllSessionSlotsStatesL(
    RObjectArray<CPEngSessionSlotState>& aSessionSlots )
    {
    CPEngStorageManagerFactory* storageFactory =
        CPEngStorageManagerFactory::InstanceLC();

    storageFactory->AllSessionSlotsStatesL( aSessionSlots );
    CleanupStack::PopAndDestroy(); // storageFactory
    }


// -----------------------------------------------------------------------------
// PEngStorageManager::RegisterGlobalEventListenerL
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngStorageManager::RegisterGlobalEventObserverL(
    MPEngGlobalEventObserver& aGlobalObserver )
    {
    CPEngStorageManagerFactory* storageFactory =
        CPEngStorageManagerFactory::InstanceLC();

    storageFactory->RegisterGlobalEventObserverL( aGlobalObserver );
    CleanupStack::PopAndDestroy(); // storageFactory
    }


// -----------------------------------------------------------------------------
// PEngStorageManager::UnregisterGlobalObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngStorageManager::UnregisterGlobalObserver(
    MPEngGlobalEventObserver& aGlobalObserver )
    {
    CPEngStorageManagerFactory* storageFactory =
        CPEngStorageManagerFactory::Instance();
    if ( storageFactory )
        {
        storageFactory->UnregisterGlobalObserver( aGlobalObserver );
        }
    }

//  End of File
