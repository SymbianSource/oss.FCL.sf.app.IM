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
* Description:  Storage Manager interface
*
*/


#ifndef __MPENGSTORAGEMANAGER_H__
#define __MPENGSTORAGEMANAGER_H__

//class CPEngStoreEntry;
#include <badesca.h>
#include "CPEngStoreEntry.h"

// FORWARD DECLARATIONS
class MPEngStorageManagerWatcher;
class CPEngSessionSlotId;

// CLASS DECLARATION
/**
*  MPEngStorageManager defines the interface for Storing / Retrieving.
*  Through this interface Storage Entries can be stored / retrieved.
*  Storage entries implement CPENGSTOREENTRY interface. For more
*  information about storage entries see CPEngStoreEntry class.
*
*  @lib PEngStorMan.lib
*  @since 3.0
*/
class MPEngStorageManager
    {
        // Storage Manager can be deleted by this interface
    public:

        virtual ~MPEngStorageManager() {};

    public: // New functions

        /**
        *	Opens one more reference to this storage Manager
        *
        *	@since 3.0
        */
        virtual void Open() = 0;

        /**
        *	Closes one reference to this storage manager and deletes
        *	if it is no more needed
        *
        *	@since 3.0
        */
        virtual void Close() = 0;

        /**
         *	Stores StoreEntry given as an argument.
         *	If StoreEntry has been locked, lock is obeyed
         *
         *	@since 3.0
         *	@param aStoreEntry Store entry to store.
         */
        virtual void StoreL( const CPEngStoreEntry& aStoreEntry ) = 0;

        /**
         *	Stores Exclusively StoreEntry given as an argument.
         *	This store function does NOT obey Lock of the store entry
         *
         *	@since 3.0
         *	@param aStoreEntry Store entry to store.
         */
        virtual void StoreExclusiveL(
            const CPEngStoreEntry& aStoreEntry ) = 0;


        /**
         *	Retrieves the state of Storage Entry given as an argument.
         *  If obey version flag is on, before InternalizeL is called
         *  on the Store Entry, version is checked, if version from
         *  store and one Store Entry was already internalized with
         *  is same, InternalizeL is not called on the Store Entry.
         *  This feature is by default on.
         *
         *	@since 3.0
         *	@param aStoreEntry Store entry to retrieve.
         *  @param aObeyVersion flag to obey version numbers
         *	@return KErrNone if successful or KErrNotFound if store
         *		entry does not exists in the storage
         */
        virtual TInt RetrieveL( CPEngStoreEntry& aStoreEntry,
                                TBool aObeyVersion = ETrue ) = 0;

        /**
         *	Deletes Storage Entry from store.
         *
         *	@since 3.0
         *	@param aId id of Store entry to delete.
         *	@param aStorageType storage type of Store entry to delete.
         *	@return KErrNone if successful.
         */
        virtual TInt Delete( const CPEngStoreEntry& aStoreEntry ) = 0;

        /**
         *	Deletes Storage Entry from store.
         *
         *	@since 3.0
         *	@param aStoreEntryId Storage ID of entry to delete
         *	@return KErrNone if successful.
         */
        virtual TInt Delete( const TDesC& aStoreEntryId ) = 0;

        /*
         *	Used to check if certain storage entry is locked
         *  Function leaves if some error while communicating with
         *  server occures.
         *
         *	@since 3.0
         *	@param aStoreEntry store entry to be locked
         *  @param aPriority lock priority
         *	@return ETrue if entry is locked
         */
        virtual TBool LockedL( const CPEngStoreEntry& aStoreEntry,
                               TPengStorageLockPriority aPriority =
                                   EStorageLockLevelBasic ) const = 0;

        /**
         *	Used to check if certain storage entry is locked
         *  Function returns EFalse if some error while communicating with
         *  server occures.
         *
         *	@since 3.0
         *	@param aStoreID is id of the storage entry
         *  @param aPriority lock priority
         *	@return ETrue if entry is locked
         *          EFalse if not entry is not locked or error has occured
         *          while communicating with the server.
         */
        virtual TBool Locked( const CPEngStoreEntry& aStoreEntry,
                              TPengStorageLockPriority aPriority =
                                  EStorageLockLevelBasic ) const = 0;


        /*
         *	Used to lock certain storage entry
         *	Lock is reference counted, so in order to release lock
         *	same number of Unlock() calls needs to be done as Lock()
         *	If Store Entry is already locked with lower priority, no matter
         *	by which client, previous lock is removed and replaced by
         *	the new lock with the new higher priority.
         *	If passed priority is lower or same as already active of the
         *	store entry, this will fail with KErrLocked
         *
         *	@since 3.0
         *	@param aStoreEntry store entry to be locked
         *	@param aPriority priority of the Lock to be done
         *  @param aCreateEntry if store entry does not exists, it is created
         *         default value is ETrue = store entry is created if needed
         *	@return count of the lock on the server side, KErrDisconected or
         *       KErrNotFound if aCreateEntry = EFalse and entry does not exist.
         */
        virtual TInt Lock( const CPEngStoreEntry& aStoreEntry,
                           TPengStorageLockPriority aPriority =
                               EStorageLockLevelBasic,
                           TBool aCreateEntry = ETrue  ) const = 0;

        /*
         *	Used to unlock certain storage entry
         *	UnLock is reference counted and need to be called as many
         *	times ad Lock was called
         *
         *	@since 3.0
         *	@param aStoreEntry store entry to be un-locked
         *	@return Reference count of the lock, or if negative
         *			then some system wide error code
         */
        virtual TInt Unlock( const CPEngStoreEntry& aStoreEntry ) const = 0;

        /**
         *  Notifies about storage IDs change
         *  This call returns without waiting if notification was delivered
         *  to the Presence Server.
         *  Only one SID can be notified at the time, when same function
         *  is called in sequence multiple times client shall keep passed
         *  descriptors valid, if those descriptors will be deleted after
         *  call, notification will not be delivered to the listeners,
         *  Since Storage server was not able to read Store Id from client
         *
         *	@since 3.0
         *	@param aChangedSId changed Store Id to be notified,
         *  @param aSidStorageType storage type of the storage entry
         *	@return KErrDisconnected if there is not any observer registered
         *			KErrNone if it went OK
         */
        virtual TInt NotifyChangedSId( const TDesC& aChangedSId,
                                       TPEngStorageType aSidStorageType ) = 0;


        /*
         *	Stops sending of SID changes done through this Storage Manager
         *	Instance, how ever all notification will be buffered on the
         *	server side and will be sent all when ReleaseServerSideBuffering
         *	is called
         *	Multiple calling of this function increases counter on the
         *	client side and by specified counter
         *	Notifications are resumbed by the ReleaseServerSideBuffering()
         *
         *	@since 3.0
         *	@param	aCount count by which counter of the server will be
         *			increased, by default set to 1
         *	@return positive number represents number on the server side
         *		counter, negative some system wide error
         */
        virtual TInt BufferServerSideNotifications( TInt aCount = 1 ) = 0;


        /*
         *	Decreases counter of the Server Side Notification buffering,
         *	when counter reaches zero, all buffered notifications are
         *	notified to the listeners.
         *	If Bufferering of the SID changes is not active, this function
         *	Does nothing
         *
         *	@since 3.0
         *	@param	aCount count by which counter of the server will be
         *			decreased, by default set to 1
         *	@return positive number represents number on the server side
         *		ounter, negative some system wide error
         */
        virtual TInt ReleaseServerSideBuffering( TInt aCount = 1 ) = 0 ;

        /*
         *	Used to unlock certain storage entry
         *
         *	@since 3.0
         *	@return reference to the Storage Manager Watcher
         */
        virtual MPEngStorageManagerWatcher& StorageManagerWatcher() = 0;

        /*
         *	Get Session slot Id class
         *	Returned class carries information about the session of this
         *	instance of the storage manager
         *
         *	@since 3.0
         *	@return CPEngSessionSlotId& Session ID slot reference
         */
        virtual const CPEngSessionSlotId& SessionSlotId() const = 0;

    };

#endif      // __MPENGSTORAGEMANAGER_H__

// End of File