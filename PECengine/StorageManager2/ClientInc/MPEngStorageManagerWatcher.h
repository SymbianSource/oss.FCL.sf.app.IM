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
* Description:  It is used to liste Storage Ids and session state
*				 changes
*
*/


#ifndef __MPENGSTORAGEMANAGERWATCHER_H__
#define __MPENGSTORAGEMANAGERWATCHER_H__

//  INCLUDES
#include    <e32base.h>
#include    <bamdesca.h>


// PEC Engine constants
#include    "PEngPresenceEngineConsts2.h"


// FORWARD DECLARATIONS
class MPEngGlobalEventObserver;
class MPEngSIDChangeObserver;
class MPEngStorageManager;
class CPEngStoreEntry;
class CPEngSessionSlotId;


// CLASS DECLARATION
/**
 *  Storage manager watcher
 *  It is used to liste Storage Ids and session state changes.
 *	State changes are listened only of the session of the
 *	storage watcher.
 *	All observer Storage Id are also located in the session slot
 *	storage watcher was loaded
 *
 *  @lib PEngStoreMan2.dll
 *  @since 3.0
 */

class MPEngStorageManagerWatcher
    {
    public:
        // Priority of the observers
        enum TPEngObserverPriority
            {
            /**
             *  This value is meant only for internal components
             *  so notifications will get first there and store
             *  entries can be refreshed before clients are informed
             *  about it changes
             */
            EPEngObserverInternal           = 1,

            /**
             *  High priority for the client observer,
             *  These observers are informed about change as first after
             *  internal observers
             */
            EPEngObserverHighPriority       = 2,
            /**
             *  Normal priority for the client observer, used as default
             */
            EPEngObserverNormalPriority     = 3,
            /**
             *  Low priority for the client observer
             *  These observers are informed about change as last
             */
            EPEngObserverLowPriority        = 4
            };
    public: // New functions

        /**
         *  Openes handle to the Storage Manager watcher
         *
         *  @since 3.0
         */
        virtual void Open() = 0;

        /**
         *  Closes handle to the Storage Manager watcher
         *
         *  @since 3.0
         */
        virtual void Close() = 0;

        /**
         *  Create Storage Manager
         *  It creates storage manager instance for the Session of this
         *  Storage Manager watcher
         *  Ownership is transfered to the caller
         *
         *  @since 3.0
         *  @return returns Instance of the Storage manager
         */
        virtual MPEngStorageManager* CreateStorageManagerL() = 0;

        /**
         *  Register new event observer
         *  Notify factory will notify observer about every new event
         *  which will occur. This function does the same as previous
         *  StartNotifyFactoryL, but it also register observer for
         *  notifications. Observing is ended when observer is
         *  unregistered.
         *  This also increase reference count for the Notify factory
         *  singleton. Notify factory is deleted when there is no more
         *  observers, that inclused, also new event and
         *  SID change observers.
         *
         *  @since 3.0
         *  @param aEventObserver event observer where to notify
         *          about new event
         *  @return returns current Presence server state
         */
        virtual TPEngNWSessionSlotState RegisterListenEventObserverL(
            MPEngGlobalEventObserver& aEventObserver ) = 0;

        /**
         *  Un Register event observer
         *  This unregisters event observer.
         *  Notify factory is deleted when there is no more observers,
         *  that inclused all kind of observers
         *
         *  @since 3.0
         *  @param aEventObserver event observer where to notify
         *  about new event
         */
        virtual void UnregisterListenEventObserver(
            MPEngGlobalEventObserver& aEventObserver ) = 0;

        /**
         *  Returns current state of the presence server
         *
         *  @since 3.0
         *  @return current state of the presence server
         */
        virtual TPEngNWSessionSlotState CurrentStateOfSessionSlot() = 0;

        /**
         *  Registers Storage IDs listener
         *  If Storage IDs listener does not exists it is created.
         *  If lister already exists, set of the Storage Ids is
         *  updated.
         *  Notify factory will notify observer about every change of
         *  the SIDs listener has subscribed
         *
         *	@since 3.0
         *	@param aInterestedSIDs Storage IDs listener is interested in
         *	@param aSIDChangeObserver observer call back interface
         *	@param aPriority priority of the observer
         *	@param aKeepOldIDs flag if current Sids are kept
         *			ETrue will keep previously registered Store Ids
         */
        virtual void RegisterListenSIDsObserverL(
            const MDesCArray& aInterestedSIds,
            MPEngSIDChangeObserver& aSIdChangeObserver,
            TPEngObserverPriority aPriority,
            TBool aKeepOldIds = ETrue ) = 0;


        /**
         *  Remove engaged SIDs from existing Storage IDs listener
         *  If Storage IDs listener does not exists this call is ignored
         *  Notify factory will notify observer about every change of
         *  remaining SIDs listener has subscribed.
         *
         *	@since 3.0
         *	@param aSIDsToRemove Storage IDs to be removed
         *	@param aSIDChangeObserver observer call back interface
         */
        virtual void RemoveSIDsFromSIDsObserver(
            const MDesCArray& aSIdsToRemove,
            const MPEngSIDChangeObserver& aSIdChangeObserver ) = 0;

        /**
         *  Restart Storage Id observer
         *  It removes all engaged SIDs from existing Storage IDs listener
         *  If Storage IDs listener does not exists this call is ignored
         *
         *	@since 3.0
         *	@param aSIDChangeObserver observer call back interface
         */
        virtual void RestartSIDsObserver(
            const MPEngSIDChangeObserver& aSIdChangeObserver ) = 0;


        /**
         *  Un Register SID change observer
         *  This unregisters SID change observer.
         *  Notify factory is deleted when there is no more observers,
         *  that inclused all kind of observers
         *
         *  @since 3.0
         *	@param aSIDChangeObserver observer call back interface
         */
        virtual void UnregisterListenSIDsObserver(
            const MPEngSIDChangeObserver& aSIdChangeObserver ) = 0;

        /**
         *  Suspend existing SIDs listener
         *  If listener is not found, function will return KErrNotFound
         *
         *  @since 3.0
         *  @param aSIDChangeObserver observer where notify factory
         *                                  will notify about SID change
         *  @return KErrNone or KErrNotFound
         */
        virtual TInt SuspendListenSIDsObserver(
            const MPEngSIDChangeObserver& aSIdChangeObserver ) = 0;

        /**
         *  Resume existing suspended SIDs listener
         *  If listener is not found, function will return KErrNotFound
         *  if listener was not suspended, function will be ignored
         *
         *  @since 3.0
         *  @param aSIDChangeObserver observer where notify factory
         *                                  will notify about SID change
         *  @return KErrNone or KErrNotFound
         */
        virtual TInt ResumeListenSIDsObserver(
            const MPEngSIDChangeObserver& aSIdChangeObserver ) = 0;

        /*
         *  Get Session slot Id class
         *  Returned class carries information about the session of this
         *  instance of the storage manager watcher
         *
         *  @since 3.0
         *  @return CPEngSessionSlotId& Session Id slot reference
         */
        virtual const CPEngSessionSlotId& SessionSlotId() const = 0;

    protected: // Constructors and destructor

        /**
         * Destructor. cannot be destroyed using this interface,
         * use Close Instead
         */
        virtual ~MPEngStorageManagerWatcher() {};

    };


#endif      // __MPENGSTORAGEMANAGERWATCHER_H__

// End of File
