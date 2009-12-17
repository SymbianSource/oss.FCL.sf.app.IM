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


#ifndef __CPENGSTORAGESERVER_H__
#define __CPENGSTORAGESERVER_H__

//  INCLUDES
#include <e32def.h>
#include <s32std.h>
#include <e32svr.h>
#include <s32file.h>	// file server

#include "MPEngStorageServer.h"
#include "PEngPresenceEngineConsts2.h"


// FORWARD DECLARATION
class CPEngStorageFolder;
class TPEngServerParams;
class CPEngSAPChangeObserver;
class CPEngSessionSlotId;
class CPEngSessionSlotState;
class CPEngSessionSlotEvent;
class CPEngHandlerListenEvents;
class CPEngTimer;

/**
 *  Main Server class of the presence server
 *  @since 3.0
 */
class   CPEngStorageServer
            : public CPolicyServer,
            public MPEngStorageServer
            //, public MBackupOperationObserver
    {
    public:		// Constructors and destructors

        /**
         * Two-phased constructor.
         */
        static CPEngStorageServer* NewL( TInt aPriority );	// Static constructor

        /**
         * Two-phased constructor.
         */
        static CPEngStorageServer* NewLC( TInt aPriority );	// Static constructor

        /**
         * Destructor.
         */
        virtual ~CPEngStorageServer();				// Destructor (virtual)

    protected:	// Constructors and destructors

        /**
         * C++ default constructor.
         */
        CPEngStorageServer( TInt aPriority );

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();		// Second phase construct

    public:		// New methods

        /*
         * Starts the server, called from clients Connect() method or from WinsMain()
         */
        static TInt ExecuteServerL( TPEngServerParams& aParams );

        /*
         *	Stops the server, actually just calls CActiveScheduler::Stop();
         */
        void StopServer();

        /*
         *	Run server
         */
        static void RunServerL( TPEngServerParams& aParams );

    public: // new functions from MPEngStorageServer

        /**
         *  Add subsession to the container
         *  @see <MPEngStorageServer.h>
         */
        void AddSubSessionL( CObject& aSubSession );

        /**
         *  Remove subsession from the container
         *  @see <MPEngStorageServer.h>
         */
        void RemoveSubSessionL( CObject& aSubSession );


        /**
         *	Start Listening of the Global events
         *	@see <MPEngStorageServer.h>
         */
        void RegisterGlobEventListenerL( const RPEngMessage& aMessage,
                                         TUint32 aSessionId );

        /**
         *	Cancel specified asynchronous request of the client
         *	@see <MPEngStorageServer.h>
         */
        void CancelRequestL( const RPEngMessage& aMessage,
                             TUint32 aSessionId );

        /**
         *  Cancel all session's asynchronous request
         *	@see <MPEngStorageServer.h>
         */
        void CancelAllSessionRequests( TUint32 aSessionId );

        /**
         *	Reload Asynchronous scout of the async observer
         *	@see <MPEngStorageServer.h>
         */
        void ReloadAsynchronousScoutL( const RPEngMessage& aMessage,
                                       TUint32 aSessionId );

        /**
         *	Create Session slot Folder
         *	@see <MPEngStorageServer.h>
         */
        void CreateSessionFolderL( const RPEngMessage& aMessage );

        /**
         *	Remove Session slot Folder
         *	@see <MPEngStorageServer.h>
         */
        void RemoveSessionFolderL( const RPEngMessage& aMessage );

        /**
         *	Get session slot state
         *	@see <MPEngStorageServer.h>
         */
        void SessionStateL( const RPEngMessage& aMessage );

        /**
         *	Get All Session slots states
         *	@see <MPEngStorageServer.h>
         */
        void AllSessionStatesL( const RPEngMessage& aMessage );

        /**
         *	Get Storage Folder Instance and return its pointer
         *	@see <MPEngStorageServer.h>
         */
        CPEngStorageFolder* StorageFolderL(
            const RPEngMessage& aMessage );

        /**
         *	Notify new global Session Slot event
         *	@see <MPEngStorageServer.h>
         */
        void NotifyGlobalEvent( const TDesC8& aGlobalEvent );

        /**
         *  Notify Error to the listeners of new events
         *	@see <MPEngStorageServer.h>
         */
        void NotifyError( TInt aError );

        /**
         *	Check if passed Application Id is registered for
         *	defined session slot
         *	@see <MPEngStorageServer.h>
         */
        void IsAppIdRegisteredL( const RPEngMessage& aMessage );

        /**
         *	Is back up operation active
         *	@see <MPEngStorageServer.h>
         */
        TBool BackUpActive();


        /**
         *  Wipe Session Slot folder
         *	@see <MPEngStorageServer.h>
         */
        void WipeSessionSlotFolderL( const RPEngMessage& aMessage );



    public: // Reference counted access related functions

        /**
         *	Session has been created
         *	More in the file <MPEngStorageServer.h>
         *
         *	@since 3.0
         */
        void SessionCreated();

        /**
         *	Session is dying, check if server is needed
         *	More in the file <MPEngStorageServer.h>
         *
         *	@since 3.0
         */
        void SessionDied();

        /**
         *  Timer expired, kill server if not needed anymore
         *
         *	@since 3.0
         */
        void TimeExpired();


    public:		// Methods derived from CServer2

        /*
         *	Called when new client is connecting, version number of the client is checked here.
         */
        CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;



    private:	// New methods

        /*
         *	Calls CActiveScheduler::Start()
         */
        void StartServer();

        /**
         *	Read Session ID from the client's message
         *
         *  @since 3.0
         *  @param aMessage client's message
         *  @return session slot ID class, ownership transferred
         */
        CPEngSessionSlotId* TranslateSessionIdLC( const RPEngMessage& aMessage );

        /**
         *	Try to find session Folder if it is active
         *
         *  @since 3.0
         *  @param aSessionId session Id to find folder for
         *  @return session Id class if folder is loaded, NULL if not
         */
        CPEngStorageFolder* FindStorageFolder(
            const CPEngSessionSlotId& aSessionId );

        /**
         *	Load storage folder,
         *	there is open handle to the returner pointer
         *
         *	@since 3.0
         */
        CPEngStorageFolder* LoadStorageFolderL(
            const CPEngSessionSlotId& aSessionId );

        /**
         *	Load session slot state
         *
         *  @since 3.0
         *  @param aSessId session slot Id
         *	@param aCreate flag if session shall be created if needed
         *	@param aSessState returned session slot state
         *	@param aStorageFolder returned storage folder, if loaded
         *	@param aStateFileName returned file name of session state
         *  @return number of items on clean up stack added by function
         */
        inline TInt LoadSessionStateLCX(
            CPEngSessionSlotId& aSessId,
            TBool aCreate,
            CPEngSessionSlotState*& aSessState,
            CPEngStorageFolder*& aStorageFolder,
            HBufC*& aStateFileName );
        /**
         *	Read State file from permanent store if it does exists
         *
         *  @since 3.0
         *  @param aSessionId session Identification
         *  @param aFileName file name of the state file
         *  @return state of the session folder
         */
        CPEngSessionSlotState* ReadSessionStatePermanentLC(
            const CPEngSessionSlotId& aSessionId,
            HBufC*& aFileName );

        /**
         *	Modify State file name to folder name
         *
         *  @since 3.0
         *  @param aFileName which will be altered to folder name
         */
        inline void UpdateFileToFolderName( TDes& aFileName );

        /**
         *	Create session directory in the file system
         *
         *  @since 3.0
         *  @param aFileName file name of the session slot state file
         */
        inline void CreateSessionDirectoryL( TDes& aFileName );

        /**
         *	Store data to the file
         *
         *  @since 3.0
         *  @param aFileName file name of the target file
         *	@param aData date to be stored into the file
         *	@return KErrNone if it went OK or system wide error code
         */
        inline TInt StoreFile( const TDesC& aFileName, const TDesC8& aData );

        /**
         *	Read data from the file
         *
         *  @since 3.0
         *  @param aFileName file name of the target file
         *	@return buffer with the data of the file
         */
        inline void ReadFileL( const TDesC& aFileName,
                               RBuf8& aBuff );

        /**
         *	Clean storage from session
         *
         *  @since 3.0
         *  @param file name of the session file
         *	@return KErrNone if cleaning goes fine
         */
        inline TInt CleanStorageFromSessionL( TDes& aFileName );

        /**
         *	Create Buffer with global event
         *
         *  @since 3.0
         *  @param aState session state
         *	@param aEvent event notification
         *  @return new session slot event
         */
        inline CPEngSessionSlotEvent* PackGlobEventLC(
            CPEngSessionSlotState& aState,
            TPEngNWSessionSlotEvent aEvent,
            const TDesC& aAppId );

        /**
         *	Update Session Slot state
         *
         *  @since 3.0
         *  @param aFolder folder of the session, can be NULL
         *	@param aState state of the session slot
         *	@param aEvent new session slot event
         *	@param aFileName file name of the state of the session
         *
         */
        inline void UpdateSessionSltStateL(
            CPEngStorageFolder* aFolder,
            CPEngSessionSlotState& aState,
            CPEngSessionSlotEvent& aEvent,
            TDes& aFileName );


    private:	// New notification engine functions

        /**
         *	Find Request Handler
         *
         *	@since 3.0
         *	@param aSessionID Session ID
         *	@param aFunction request function
         *	@return pointer to Request Handler it it exists or NULL
         */
        inline CPEngHandlerListenEvents* FindRequestHandler( TUint32 aSessionId,
                                                             TPEngStorageServerMessages aFunction );

    private:	// Data

        /// OWN: Container collection
        ///      All containers created by this collection are also
        ///      owned by this colletion
        CObjectConIx*						    iContainerIx;

        /// OWN: Container of Sub sesssions
        ///      Owned over iContainerIx
        CObjectCon*								iSubSessionCnt;

        /// OWN: Container of the Storage Cages
        ///      Owned over iContainerIx
        CObjectCon*								iFoldersCnt;

        /// OWN: Container with the Observers,
        ///      Owned over iContainerIx
        CObjectCon*								iObserversCnt;


        /// OWN: Flag if server is running
        TBool									iRunning;


        /// OWN: File server client on the Storage Server
        RFs										iFs;

        /// OWN: Session count
        TInt									iSessCount;

        /// OWN: Back up operation active
        TBool									iBackUpActive;

        /// OWN: Temp folder Name
        HBufC*									iTempFolder;

        /// OWN: Server shut down timer
        CPEngTimer*								iTimeOut;

        /// OWN: Communication buffer to minimize allocations
        RBuf16									iBuff16;

        /// OWN: Communication buffer to minimize allocations
        RBuf8									iBuff8;
    };

#endif      //  __CPENGSTORAGESERVER_H__

