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
* Description:  Storage Server Abstract interface
*
*/



#ifndef __MPENGSTORAGESERVER_H__
#define __MPENGSTORAGESERVER_H__

//  INCLUDES

#include <e32def.h>
#include <bamdesca.h>
#include "TPEngStorageServerMessages.h"

// FORWARD DECLARATIONS
class CPEngStorageFolder;
class RPEngMessage;


/**
*  Storage Server Abstract interface
*
*  @since 3.0
*/
class MPEngStorageServer
    {
    public: // New functions

        /**
         *  Add subsession to the container
         *
         *  @since 3.0
         *  @param aSubSession sub session instance
         */
        virtual void AddSubSessionL( CObject& aSubSession ) = 0;

        /**
         *  Remove subsession from the container
         *
         *  @since 3.0
         *  @param aSubSession subsession instance
         */
        virtual void RemoveSubSessionL( CObject& aSubSession ) = 0;

        /**
         *	Start Listening of the Global events
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         *	@param aSessionId Id of the session request came from
         */
        virtual void RegisterGlobEventListenerL( const RPEngMessage& aMessage,
                                                 TUint32 aSessionId ) = 0;

        /**
         *	Cancel specified asynchronous request of the client
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         *	@param aSessionId Id of the session request came from
         */
        virtual void CancelRequestL( const RPEngMessage& aMessage,
                                     TUint32 aSessionId ) = 0;

        /**
         *  Cancel all session's asynchronous request
         *
         *  @since 3.0
         *	@param aSessionId Id of the session request came from
         */
        virtual void CancelAllSessionRequests( TUint32 aSessionId ) = 0;

        /**
         *	Reload Asynchronous scout of the async observer
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         *	@param aSessionId Id of the session request came from
         */
        virtual void ReloadAsynchronousScoutL( const RPEngMessage& aMessage,
                                               TUint32 aSessionId ) = 0;

        /**
         *	Create Session slot Folder
         *	If session Slot folder exists, new Application ID is just
         *	registered there.
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         *	@param aSessionId Id of the session request came from
         */
        virtual void CreateSessionFolderL(
            const RPEngMessage& aMessage ) = 0;

        /**
         *	Remove Session slot Folder
         *	If session Slot folder has registered more Application Ids
         *	it stays in the storage, till last one is unregistered
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         *	@param aSessionId Id of the session request came from
         */
        virtual void RemoveSessionFolderL(
            const RPEngMessage& aMessage ) = 0;

        /**
         *	Get session slot state
         *	Returns to the client buffer with the session slot state
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         */
        virtual void SessionStateL( const RPEngMessage& aMessage ) = 0;

        /**
         *	Get All Session slots states
         *	Returns to the client buffer with all session slot states
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         */
        virtual void AllSessionStatesL( const RPEngMessage& aMessage ) = 0;

        /**
         *	Get Storage Folder Instance and return its pointer
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         */
        virtual CPEngStorageFolder* StorageFolderL(
            const RPEngMessage& aMessage ) = 0;

        /**
         *	Notify new global event to the listeners
         *
         *  @since 3.0
         *  @param aGlobalEvent global event des buffer
         */
        virtual void NotifyGlobalEvent( const TDesC8& aGlobalEvent ) = 0;

        /**
         *  Notify Error to the listeners of new events
         *
         *  @since 3.0
         *  @param aError error code
         */
        virtual void NotifyError( TInt aError ) = 0;

        /**
         *	Check if passed Application Id is registered for
         *	defined session slot
         *	Content of the client message is defined in the
         *	<TPEngStorageServerMessages.h>
         *
         *  @since 3.0
         *  @param aMessage message from the client
         */
        virtual void IsAppIdRegisteredL( const RPEngMessage& aMessage ) = 0;

        /**
         *	Is back up operation active
         *
         *  @since 3.0
         *  @return ETrue if back up is active
         */
        virtual TBool BackUpActive() = 0;


    public: // testing support

        /**
         *  Wipe Session Slot folder
         *	Nothing is checked, folder is wipe out with force
         *
         *  @since 3.0
         *  @param aMessage message from the client
         */
        virtual void WipeSessionSlotFolderL(
            const RPEngMessage& aMessage ) = 0;

    public: // Reference counted access related functions

        /**
         *	Session has been created
         *	More in the file <MPEngStorageServer.h>
         *
         *	@since 3.0
         */
        virtual void SessionCreated() = 0;

        /**
         *	Session is dying, check if server is needed
         *	More in the file <MPEngStorageServer.h>
         *
         *	@since 3.0
         */
        virtual void SessionDied() = 0;

    public: // used by server timer

        /**
         *  Timer expired
         *
         *	@since 3.0
         */
        virtual void TimeExpired() = 0;

    protected:  //Destructor

        /**
          * Virtual inline destructor.
          *
          * Server cannot be destroyed through this interface
          * destroyed using this interface.
          */
        virtual ~MPEngStorageServer() {};

    };

#endif      // __MPENGSTORAGESERVER_H__

// End of File
