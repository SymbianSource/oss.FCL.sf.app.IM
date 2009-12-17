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
* Description:  See class description below.
*
*/


#ifndef __CPENGSTORAGEMANAGER_H__
#define __CPENGSTORAGEMANAGER_H__

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

#include "MPEngStorageManager.h" // base class
#include "RPEngStorageSubFolderClient.h"


// FORWARD DECLARATIONS
class CPEngStoreEntry;


enum TPEngPermanentStorageType
    {
    EPEngNormal,
    EPEngTemp,
    EPEngGlobal
    };

/**
 *  Implementation of Presence Storage Manager.
 *  CPEngStorageManager implements the Storage Manager presented at
 *  MPEngStorageManager. This version supports two different saving
 *  methods: to memory cache and disc drive.
 *  Usage:   1. construct
 *           2. set store base (SetStorageBase()) if not set before use, raises panic.
 *           3. Store/Retrieve
 *
 *	See MPEngStorageManager class for method descriptions.
 */
NONSHARABLE_CLASS( CPEngStorageManager ) :
        public CBase,
        public MPEngStorageManager
    {
public:  // Constructors and destructor

    /**
     *	Two-phased constructor.
     */
    static CPEngStorageManager* NewL(
        MPEngStorageManagerWatcher& aWatcher,
        RPEngStorageClient& aStorageMainClient );

    /**
      *	Two-phased constructor.
      */
    static CPEngStorageManager* NewLC(
        MPEngStorageManagerWatcher& aWatcher,
        RPEngStorageClient& aStorageMainClient );

    /**
     *	Destructor.
     */
    virtual ~CPEngStorageManager();

protected: // Protected contructors

    /**
     *	C++ default constructor.
     */
    CPEngStorageManager( MPEngStorageManagerWatcher& aWatcher );

    /**
     *	Symbian 2nd phase constructor
     */
    void ConstructL( RPEngStorageClient& aStorageMainClient );

public: // Functions from the MPEngStorageManager base classes

    /**
    *	Opens one more reference to this storage Manager
    *	@see <MPEngStorageManager.h>
    *	@since 3.0
    */
    void Open();

    /**
    *	Closes one reference to this storage manager
    *	@see <MPEngStorageManager.h>
    *	@since 3.0
    */
    void Close();

    /**
     *	Stores Storeable Entry given as an argument.
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    void StoreL( const CPEngStoreEntry& aStoreEntry );

    /**
     *	Stores Exclusively StoreEntry given as an argument.
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    void StoreExclusiveL( const CPEngStoreEntry& aStoreEntry );

    /**
     *	Retrieves the state of Storeable Entry given as an argument.
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    TInt RetrieveL( CPEngStoreEntry& aStoreEntry,
                    TBool aObeyVersion );

    /**
     *	Deletes Storeable Entry from store.
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    TInt Delete( const CPEngStoreEntry& aStoreEntry );
    TInt Delete( const TDesC& aStoreEntryId );


    /**
     *	Used to check if certain storage entry is locked
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    TBool LockedL( const CPEngStoreEntry& aStoreEntry,
                   TPengStorageLockPriority aPriority ) const;

    TBool Locked( const CPEngStoreEntry& aStoreEntry,
                  TPengStorageLockPriority aPriority ) const;


    /**
     *	Used to lock certain storage entry
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    TInt Lock( const CPEngStoreEntry& aStoreEntry,
               TPengStorageLockPriority aPriority,
               TBool aCreateEntry ) const;

    /**
     *	Used to unlock certain storage entry
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    TInt Unlock( const CPEngStoreEntry& aStoreEntry ) const;


    /**
     *	Blind call to notify about storage IDs change
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
     */
    TInt NotifyChangedSId( const TDesC& aChangedSId,
                           TPEngStorageType aSidStorageType );


    /**
     * 	Stops sending of SID changes done through this Storage Manager
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    TInt BufferServerSideNotifications( TInt aCount );

    /**
     *	Decreases counter of the Server Side Notification buffering,
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    TInt ReleaseServerSideBuffering( TInt aCount );

    /**
     *	Used to unlock certain storage entry
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    MPEngStorageManagerWatcher& StorageManagerWatcher();

    /**
     *	Get Session slot Id class
     *	@see <MPEngStorageManager.h>
     *	@since 3.0
     */
    const CPEngSessionSlotId& SessionSlotId() const;


private:	// Ne private functions

    /**
      *	Stores Store Entry
      *
      *	@since 3.0
      *	@param aStoreEntry Store entry to store.
      *	@param aExlusiveLock if ETrue, lock is obeyed
      */
    void StoreEntryL( const CPEngStoreEntry& aStoreEntry,
                      TBool aObeyLock  );

private:    // Data

    /// Access counter
    TInt									iAccessCount;

    /// OWN: Storage Manager Watcher, reference owned
    MPEngStorageManagerWatcher&				iStorageManagerWatcher;

    /// OWN: Sub Session client to Storage Server
    RPEngStorageSubFolderClient				iSubFolderClient;

    ///	REF: Session Slot Id class
    const CPEngSessionSlotId&				iSessionSlot;

    };

#endif      // __CPENGSTORAGEMANAGER_H__
