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
* Description:  Static interface of the storage manager
*
*/


#ifndef __PENGSTORAGEMANAGER_H__
#define __PENGSTORAGEMANAGER_H__

//  INCLUDES
#include	<e32base.h>
#include	"RObjectArray.h"


// FORWARD DECLARATIONS
class MPEngStorageManager;
class MPEngStorageManagerWatcher;
class CPEngSessionSlotId;
class MPEngGlobalEventObserver;
class CPEngSessionSlotState;

//  Local Data Structures

/**
 *  Presence Server notify factory to register observers for listening
 *	of new events, SIDs changes or notifying about SID  change
 *	when ever there is any observer, SAP settings store is always observer
 *	and active folder in the storage manager is changed
 *	does not matter on the nature of the observer, SID, Event, Stolid
 *
 *  @lib PEngClient
 *  @since 3.0
 */
class PEngStorageManager
    {

    public: // New functions

        /**
         *	Retrieves Storage Manager instance
         *	Ownership is passed to the caller
         *	To get Storage manager which has handle to the correct
         *	sub-folder of the Storage, Server address and user name
         *	has to be specified
         *
         *	@since 3.0
         *	@param aSessionSlot session slot Class which caries
         *				unique information about the session
         *	@return MPEngStorageManager type pointer, ownership is
         *						passed to the caller
          */
        IMPORT_C static MPEngStorageManager* GetStorageManagerL(
            const CPEngSessionSlotId& aSessionSlot );


        /**
         *	Retrieves Storage Manager Watcher reference
         *	When this method is called, Reference counter is increased
         *	and when user does not need watcher, close shall be called

         *	To get Storage manager Watcher which has handle to the
         *	correct sub-folder of the Storage, Server address
         *	and user name has to be specified
         *
         *	@since 3.0
         *	@param aSessionSlot session slot Class which caries
         *				unique information about the session
         *	@return MPEngStorageManagerWatcher type pointer,
         *						ownership is passed to the caller
          */
        IMPORT_C static MPEngStorageManagerWatcher* GetStorageManagerWatcherL(
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
         *	@param aApplicationId calling application Id
         */
        IMPORT_C static void CreateSessionFolderL(
            const CPEngSessionSlotId& aSessionSlot,
            const TDesC& aApplicationId );

        /**
         *	Remove Session folder from the storage, with the session folder
         *	Session is deleted from the storage, only if the calling
         *	application was the last one registered (CreateSessionFolderL())
         *
         *	@since 3.0
         *	@param aSessionSlot session slot Class which caries
         *				unique information about the session
         *	@param aApplicationId calling application Id
         */
        IMPORT_C static void RemoveSessionFolderL(
            const CPEngSessionSlotId& aSessionSlot,
            const TDesC& aApplicationId );

        /**
         *	Get Session slot state.
         *
         *	@since 3.0
         *	@param aSessionSlot session slot Class which caries
         *				unique information about the session
         *  @param aSessionSlotState will be filled on return with the
         *			    state of the session slot
         */
        IMPORT_C static void SessionSlotStateL(
            const CPEngSessionSlotId& aSessionSlot,
            CPEngSessionSlotState& aSessionSlotState );

        /**
         *	Get list of existing Session slots
         *	Returns array of all existing session slots, no matter if
         *	session is opened or closed.
         *
         *	@since 3.0
         *	@param aSessionSlots array where session slots states will be
         *			stored
         */
        IMPORT_C static void AllSessionSlotsStatesL(
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
        IMPORT_C static void RegisterGlobalEventObserverL(
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
        IMPORT_C static void UnregisterGlobalObserver(
            MPEngGlobalEventObserver& aGlobalObserver );


    protected: // Constructors and destructor

        /**
         * C++ default constructor.
         */
        PEngStorageManager();

        /**
         * Destructor.
         */
        virtual ~PEngStorageManager();

        // Prohibit copy constructor if not deriving from CBase.
        PEngStorageManager( const PEngStorageManager& );

        // Prohibit assignments operator if not deriving from CBase.
        PEngStorageManager& operator=( const PEngStorageManager& );

    };


#endif      // __PENGSTORAGEMANAGER_H__

// End of File
