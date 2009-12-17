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
* Description:  Factory of Storage Manager
*
*/



#ifndef __CPENGSTORAGEMANAGERFACTORY_H__
#define __CPENGSTORAGEMANAGERFACTORY_H__

//  INCLUDES
#include <e32base.h>
#include "RPEngStorageClient.h"
#include "MPEngStorageFactory.h"

#include "RObjectArray.h"


// FORWARD DECLARATIONS
class CPEngStorageManagerWatcher;
class CPEngStorageManager;
class CPEngSessionSlotId;
class CPEngSessionSlotState;
class MPEngGlobalEventObserver;


/**
 *  Factory of Storage Manager
 *	It holds all active Watchers of the storage manager
 *	There is always one watcher per each session slot which is
 *	shared by all storage manager instances accessing this session slot
 *
 *  @lib PEngStoreMan2.dll
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngStorageManagerFactory ):
        public CActive,
        public MPEngStorageFactory
    {
public:  // Constructors and destructor

    /**
     *	Instance of the storage factory, it is created if needed
     *
     *  @since 3.0
     *  @param
     *  @return
     */
    static CPEngStorageManagerFactory* InstanceLC();

    /**
     *	Gets pointer to the Manager factory it is exists
     *	if not then NULL pointer
     *
     *  @since 3.0
     *  @param
     *  @return
     */
    static CPEngStorageManagerFactory* Instance();

    /**
     * Destructor.
     */
    virtual ~CPEngStorageManagerFactory();

public:	// New functions for reference counting support

    /**
     *	Openes handle new handle to Storage Manager Factory
     *
     *	@since 3.0
     */
    void Open();

    /**
     *	Closes handle to the Storage Manager Factory
     *
     *	@since 3.0
     */
    void Close();

public: // virtual Functions of the CActive

    /**
     *	Handles an active object’s request completion event.
     *	@since 3.0
     */
    void RunL();

    /**
     *	Handles a leave occurring in the request completion
     *	event handler RunL()
     *	@since 3.0
     */
    TInt RunError( TInt aError );

    /**
     *	Implements cancellation of an outstanding request.
     *	This function is called as part of the active object’s Cancel().
     *	@since 3.0
     */
    void DoCancel();

public: // New Functions from MPEngStorageFactory

    /**
     *  Remove Storage watcher,
     *	@see <MPEngStorageFactory.h>
     */
    void CloseStorageWatcher(
        CPEngStorageManagerWatcher* aStorageWatcher );

    /**
     *  Increases reference counter of Factory
     *	@see <MPEngStorageFactory.h>
     */
    void OpenWatcher();

    /**
     *  Storage Manager main client
     *	@see <MPEngStorageFactory.h>
     */
    RPEngStorageClient& MainClient();

public: // New Functions of the base class

    /**
     *	Retrieves Storage Manager instance
     *	Ownership is passed to the caller
     *	To get Storage manager which has handle to the correct
     *	sub-folder of the Storage,
     *	Server address and user name has to be specified.
     *
     *	@since 3.0
     *	@param aSessionSlot session slot Class which caries
     *				unique information about the session
     *	@return MPEngStorageManager type pointer,
     *                  ownership is passed to the caller
      */
    CPEngStorageManager* StorageManagerL(
        const CPEngSessionSlotId& aSessionSlot );


    /**
     *	Retrieves Storage Manager Watcher pointer
     *	When this method is called,
     *  Reference counter is increased and when user does not
     *	need watcher, close shall be called
     *
     *	To get Storage manager Watcher which has handle to
     *	the correct sub-folder of the Storage,
     *	Server address and user name has to be specified
     *
     *	@since 3.0
     *	@param aSessionSlot session slot Class which caries
     *				unique information about the session
     *	@return MPEngStorageManagerWatcher type pointer,
     *				ownership is passed to the caller
      */
    CPEngStorageManagerWatcher* StorageManagerWatcherL(
        const CPEngSessionSlotId& aSessionSlot );

    /**
     *	Create Session folder in the storage
     *	This will ensure existence of the session slot folder till the
     *	moment calling application will not unregister from the
     *	session slot folder ( RemoveSessionFolderL)
     *
     *	@since 3.0
     *	@param aSessionSlot session slot Class which caries
     *				unique information about the session
     *	@param aApplicationId application Id
     */
    void CreateSessionFolderL(
        const CPEngSessionSlotId& aSessionSlot,
        const TDesC& aApplicationId );

    /**
     *	Remove Session folder from the storage,
     *	Session folder Session is deleted from the storage,
     *	only if the calling	application was the last one registered
     *	@see CreateSessionFolderL()
     *
     *	@since 3.0
     *	@param aSessionSlot session slot Class which caries
     *				unique information about the session
     *	@param aApplicationId application Id
     */
    void RemoveSessionFolderL(
        const CPEngSessionSlotId& aSessionSlot,
        const TDesC& aApplicationId );

    /**
     *	Get Session slot state
     *	returns class where is described state of the session slot
     *	ownership is passed
     *
     *	@since 3.0
     *	@param aSessionSlot session slot Class which caries
     *				unique information about the session
     *	@param aSessionSlotState state class is updated on the
     *			return with actual state
     */
    void SessionSlotStateL( const CPEngSessionSlotId& aSessionSlot,
                            CPEngSessionSlotState& aSessionSlotState );

    /**
     *	Get list of existing Session slots
     *	Returns array of all existing session slots, no matter if
     *	session is opened or closed.
     *
     *	@since 3.0
     *	@param aSessionSlot session slot Class which caries
     *				unique information about the session
     */
    void AllSessionSlotsStatesL(
        RObjectArray<CPEngSessionSlotState>& aSessionSlots );


    /**
     *	Register Global event observer
     *	Global event observer will be notified about all events
     *	of the PEC engine
     *
     *	@since 3.0
     *	@param aGlobalObserver call back interface implemented by the
     *				observer
     *		Function leaves with KErrAlreadyExists
     *		, if same observer is already registered
     */
    void RegisterGlobalEventObserverL(
        MPEngGlobalEventObserver& aGlobalObserver );

    /**
     *	Unregister Global event observer
     *	Global observer is unregistered, if observer is not found
     *	among the registered observers, function is ignored
     *
     *	@since 3.0
     *	@param aGlobalObserver call back interface implemented by the
     *				observer
     */
    void UnregisterGlobalObserver(
        MPEngGlobalEventObserver& aGlobalObserver );

private:  // New private Functions

    /**
     *	Start global event listener
     *	@since 3.0
     */
    void StartGlobalEventListenerL();


private:	// private constructors

    /**
    * C++ default constructor.
    */
    CPEngStorageManagerFactory();

    /**
    * Symbian 2nd phase constructor
    */
    void ConstructL();

private:    // Data

    /// Listener of global events is activated
    TBool											iListening;

    /// Is listening scout created on the server side
    TInt											iScoutCreated;

    /// Access count of the reference counted object
    TInt											iAccessCount;

    /// OWN: Storage manager main client
    RPEngStorageClient								iStoreManagerClient;

    /// REF: Container with all Storage ID watchers
    RPointerArray<CPEngStorageManagerWatcher>		iStorageWatchers;

    /// REF: Global Events Observers
    RPointerArray<MPEngGlobalEventObserver>			iObservers;

    /// OWN:New Global Event receive Buffer
    HBufC8*											iGlobalEventBuff;

    /// New Global Event
    TPtr8											iGlobalEventDes;

    /// Last All states buffer size
    TInt											iAllStatesBuffSize;

    /// OWN: Flag is object is still alive
    TBool* 											iAliveFlag;
    };

#endif      // __CPENGSTORAGEMANAGERFACTORY_H__

// End of File
