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
* Description:  Sub-client to managed particular session slot
*
*/


#ifndef __RPENGSTORAGESUBFOLDERCLIENT_H__
#define __RPENGSTORAGESUBFOLDERCLIENT_H__

//  INCLUDES
#include <e32def.h>
#include "PEngStorageGlobals.h"


// FORWARD DECLARATIONS
class RPEngStorageClient;

/**
 *  Storage sub-client
 *  It is used for management of particular session slot to which
 *	this sub-session client is connected.
 *
 *  @lib PEngStoreMan2.dll
 *  @since 3.0
 */
// CLASS DECLARATION
NONSHARABLE_CLASS(  RPEngStorageSubFolderClient ):
        public RSubSessionBase

    {

public:		// Constructors and destructors

    /**
     * C++ default constructor.
     */
    RPEngStorageSubFolderClient( );

    /**
     * Destructor.
     */
    ~RPEngStorageSubFolderClient( void );

public: 	// new methods

    /**
     * Connects to the presence server
     *
     *	@since 3.0
     *	@param aStorageMainClient storage main client
     *	@param aSessionName packed session slot Id to connect to
     *	@return KErrNone if connected or any of system wide error codes
     */
    TInt Connect( RPEngStorageClient& aStorageMainClient,
                  const TDesC8& aSesssionName );

    /**
     *	Close connection to the presence server
     *
     *	@since 3.0
     */
    void Close();

public:		// new Storage management Functions

    /**
     *  Stores Store Entry to the Server
     *
     *  @since 3.0
     *  @param aStoreId storage ID of the store entry
     *  @param aStoreType Type of the store entry
     *  @param aData data buffer of store entry
     *  @return KErrNone if client is connected and request went through
     *			otherwise system wide error code
    */
    TInt Store( const TDesC& aStoreId,
                TPEngStorageType aStoreType,
                const TDesC8& aData );

    /**
     *  Retrieves Store Entry from Server
     *
     *  @since 3.0
     *  @param aStoreId storage ID of the store entry
     *  @param aStoreType Type of the store entry
     *  @param aData data buffer of store entry
     *  @return KErrNone if client is connected and request went through,
     *		if positive Error code, then returned number specifies
     *		size of the buffer needed to write data to the client buffer,
     *		since buffer was not big enough.Otherwise system wide error
     */
    TInt Retrieve( const TDesC& aStoreId,
                   TPEngStorageType aStoreType,
                   TDes8& aData );

    /**
     *  Deletes Store entry from the storage
     *
     *  @since 3.0
     *  @param aStoreId storage ID of the store entry
     *  @param aStoreType Type of the store entry
     *  @return KErrNone if client is connected and request went through
     *			otherwise system wide error code
    */
    TInt Delete( const TDesC& aStoreId, TPEngStorageType aStoreType );

    /*
     *  Check is store entry is locked
     *
     *  @since 3.0
     *  @param aStoreId storage ID of the store entry
     *  @param aStoreType Type of the store entry
     *  @param aPriority lock priority
     *  @return KErrNone if client is connected and request went through
     *		and entry is not locked, ETrue if locked and call went through
     *		otherwise system wide error code
     */
    TInt StoreEntryLocked( const TDesC& aStoreId,
                           TPEngStorageType aStoreType,
                           TPengStorageLockPriority aPriority ) const;

    /*
     *  Lock defined store entry
     *
     *  @since 3.0
     *  @param aStoreId storage ID of the store entry
     *  @param aStoreType Type of the store entry
     *  @param aPriority lock priority
     *  @return KErrNone if client is connected and request went through
     *			otherwise system wide error code
     */
    TInt LockStoreEntry( const TDesC& aStoreId,
                         TPEngStorageType aStoreType,
                         TPengStorageLockPriority aPriority,
                         TBool aCreateEntry ) const;

    /*
     * Unlock defined store entry
     *
     * @since 3.0
     * @param aStoreId storage ID of the store entry
     * @param aStoreType Type of the store entry
     * @return KErrNone if client is connected and request went through
     *			otherwise system wide error code
     */
    TInt UnlockStoreEntry( const TDesC& aStoreId,
                           TPEngStorageType aStoreType ) const;


    /*
     * Starts buffering server side notifications from this sub session
     *
     * @since 3.0
     * @param aCount number to increase server counter by
     * @return KErrNone if client is connected and request went through
     *			otherwise system wide error code
    */
    TInt BufferServerSideNotifications( TInt aCount );


    /*
     * Release buffering of Server side notifications, from this
     * sub session, all buffered notifications are notified to other
     * clients
     *
     * @since 3.0
     * @param aCount number to decrease server counter by
     * @return KErrNone if client is connected and request went through
     *			otherwise system wide error code
    */
    TInt ReleaseServerSideBuffering( TInt aCount );

public:		// notify functions

    /**
     *	Synchronous request to start Storage ID listening.
     *	This request carries information about SIDs client
     *	is interested in, and also pointer to the return
     *	array which is used by the client to return information
     *	about changes SIDs.	This synchronous request has to be
     *	followed by the asynchronous request which will bring to
     *	the server TRequestStatus which will be used
     *	to signal changes to the client
     *
     *	@since 3.0
     *	@param aEngagedSIds packed Storage Ids to listen
     *	@return TInt KErrNone if operation is successful,
    			KErrDisconnected if server is disconnected,
     *			or system wide error code
     */
    TInt StartListeningSIdsChanges(	const TDesC& aEngagedSIds );

    /**
     *	Synchronous request to update listen storage IDs scout
     *	with new set of storage IDs	New set of SIDs which will be
     *	watched by the client is delivered to the Server while
     *	Existing Listen SIDs scout has to exists, if not,
     *	call is completed with KErrNotFound. If this function fails
     *	on the server side and completed with error code, existing
     *	listener is kept and also old set of SIDs client
     *	is interested in stays untouched
     *
     *	@since 3.0
     *	@param aEngagedSIds packed Storage Ids to listen
     *	@return TInt KErrNone if operation is successful,
     *			KErrDisconnected if server is disconnected,
     *			or system wide error code
     */
    TInt UpdateListenSIdsChangesScout( const TDesC& aEngagedSIds );

    /**
     *	Asynchronous request to the server which reload Storage ID
     *	listener with new TRequestStatus which is used to signal
     *	changes to the Client. When TRequestStatus is completed
     *	and client consumes SID changes, same request can be used
     *	to continue with listening of SID changes. SIDs which client
     *	is interested in are defined
     *	by the StartListeningSIDsChanges function call
     *
     *	@since 3.0
     *	@param aAnswerBuffer buffer server will answer indexes of
     *		    changed Store Ids
     *	@param aStatus - TRequestStatus, where server will signal
     *		completing of the request
     *	@return TInt KErrNone is server is connected,
     *		KErrDisconnected if server is disconnected
     */
    TInt ReloadListenSIdsChangesScout( /*TDes& aAnswerBuffer,*/
        TRequestStatus& aStatus );


    /**
     *	Synchronous request to fetch changed Storage IDs indexes
     *
     *	@since 3.0
     *	@param aChangedIds buffer where indexes of changed Ids will be stored
     *	@return TInt KErrNone is server is connected,
     *		KErrDisconnected if server is disconnected
     */
    TInt FetchChangedSIdsIndexes(
        TDes& aChangedIds );


    /**
     *	Synchronous request to stop listening of the SID changes
     *	This request also cancel pending asynchronous request
     *	from ReloadSIDsChangesScout call
     *
     *	@since 3.0
     *	@return TInt KErrNone is server is connected,
     *				KErrDisconnected if server is disconnected
     */
    TInt StopListeningSIdsChanges();

    /**
     *	SYNCHRONOUS request to the server to notify it
     *	about change of one storage ID.	Server then notifies all
     *	the clients which are listening change of this particular SID
     *	This issues blind message, so it returns to client
     *	without waiting how server will response
     *
     *	@since 3.0
     *	@param aChangedSId - changed Storage Id
     *	@return KErrDisconnected if server is disconnected,
     *								otherwise answer from the server
     */
    TInt NotifyChangedSIdBlind( const TDesC& aPackedChangedSId,
                                TPEngStorageType aStoreType );

private:	// Data

    /// Flag if sub Session is connected to the Server
    TBool										iConnected;


    };

#endif      //  __RPENGSTORAGESUBFOLDERCLIENT_H__





















