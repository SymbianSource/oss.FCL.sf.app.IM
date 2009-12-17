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



#ifndef __CPENGSTORAGEMANAGERWATCHER_H__
#define __CPENGSTORAGEMANAGERWATCHER_H__

//  INCLUDES
#include	<e32base.h>
#include	<bamdesca.h>
#include	"RPEngStorageSubFolderClient.h"
#include	"MPEngStorageManagerWatcher.h"


// FORWARD DECLARATIONS
class MPEngGlobalEventObserver;
class MPEngSIDChangeObserver;
class CPEngSIdChangeListener;
class MPEngStorageFactory;
class CPEngSessionSlotId;
class CPEngSessionSlotEvent;
class CPEngSessionSlotState;

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
NONSHARABLE_CLASS( CPEngStorageManagerWatcher ):
        public CBase,
        public MPEngStorageManagerWatcher
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngStorageManagerWatcher* NewL(
        const CPEngSessionSlotId& aSessionSlot,
        RPEngStorageClient& aStorageClient,
        MPEngStorageFactory& aStorageFactory );

    /**
     * Two-phased constructor.
     */
    static CPEngStorageManagerWatcher* NewLC(
        const CPEngSessionSlotId& aSessionSlot,
        RPEngStorageClient& aStorageClient,
        MPEngStorageFactory& aStorageFactory );

    /**
     * Destructor.
     */
    virtual ~CPEngStorageManagerWatcher();

public: // New functions of main class

    /**
     *	Notify new Global event connected to this session slot
     *	Notifies about Global event all the listeners
     *
     *  @since 3.0
     *  @param	aNewEvent new global event
     */
    void NotifyGlobalEvent( const CPEngSessionSlotEvent& aNewEvent );

    /**
     *	Handler error of the listener
     *	It notifies about error all listeners
     *
     *  @since 3.0
     */
    void NotifyListenerError( TInt aError );


public: // New functions of MPEngStorageManagerWatcher

    /**
     *	Openes handle to the Storage Manager watcher
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
     */
    void Open();

    /**
     *	Closes handle to the Storage Manager watcher
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
     */
    void Close();

    /**
     *	Create Storage Manager
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
      */
    MPEngStorageManager* CreateStorageManagerL();

    /**
     *	Register new event observer
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
      */
    TPEngNWSessionSlotState RegisterListenEventObserverL(
        MPEngGlobalEventObserver& aEventObserver );

    /**
     *	Un Register event observer
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
      */
    void UnregisterListenEventObserver(
        MPEngGlobalEventObserver& aEventObserver );

    /**
     *	Returns current state of the presence server
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
      */
    TPEngNWSessionSlotState CurrentStateOfSessionSlot();

    /**
     *	Registers Storage IDs listener
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
     */
    void RegisterListenSIDsObserverL(
        const MDesCArray& aInterestedSIds,
        MPEngSIDChangeObserver& aSIdChangeObserver,
        TPEngObserverPriority aPriority,
        TBool aKeepOldIds );


    /**
     *	Remove engaged SIds from existing Storage IDs listener
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
     */
    void RemoveSIDsFromSIDsObserver(
        const MDesCArray& aSIdsToRemove,
        const MPEngSIDChangeObserver& aSIdChangeObserver );

    /**
     *  Restart Storage Id observer
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
     */
    void RestartSIDsObserver(
        const MPEngSIDChangeObserver& aSIdChangeObserver );

    /**
     *	Un Register SId change observer
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
      */
    void UnregisterListenSIDsObserver(
        const MPEngSIDChangeObserver& aSIdChangeObserver );

    /**
     *	Suspend existing SIDs listener
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
      */
    TInt SuspendListenSIDsObserver(
        const MPEngSIDChangeObserver& aSIdChangeObserver );

    /**
     *	Resume existing suspended SIDs listener
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
      */
    TInt ResumeListenSIDsObserver(
        const MPEngSIDChangeObserver& aSIdChangeObserver );

    /*
     *	Get Session slot Id class
     *  @see <MPEngStorageManagerWatcher.h>
     *	@since 3.0
     */
    const CPEngSessionSlotId& SessionSlotId() const;

private: // New Private functions of the main class

    /**
     *	Clean Storage Id Listener
     *	@since 3.0
     */
    void CleanStorageIdListener();


    /**
     *	Load Storage Id listener
     *	@since 3.0
     */
    void LoadSIdListenerL();

private:

    /**
     * C++ default constructor.
     */
    CPEngStorageManagerWatcher( MPEngStorageFactory& aStorageFactory );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const CPEngSessionSlotId& aSessionSlot,
                     RPEngStorageClient& aStorageClient );

private:    // Data
    /// OWN: Notify client
    RPEngStorageSubFolderClient					iNotifyClient;

    /// REF: Storage Manager Factory
    MPEngStorageFactory&						iStorageManagerFactory;

    /// OWN: Store Id listener, owned
    CPEngSIdChangeListener*						iStorageIdListener;

    /// Access count for reference counted object
    TInt										iAccessCount;

    /// OWN: Session Slot Id
    CPEngSessionSlotId*							iSessionSlotId;

    /// FER: Observers of the events of this particular session
    RPointerArray<MPEngGlobalEventObserver>		iEventObservers;

    /// Session Slot state
    TPEngNWSessionSlotState						iSessionState;

    };

#endif      // __CPENGSTORAGEMANAGERWATCHER_H__

// End of File
