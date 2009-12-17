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
* Description:  One Session manager of the presence server
* 				 Handles all management concerning one network session
*  				 Opening, closing, ownership handling, updating
*  				 of Session related data to network
*
*/



#ifndef __CPENGCSPSESSMANAGER_H__
#define __CPENGCSPSESSMANAGER_H__

//  INCLUDES
#include	<e32base.h>

#include	"MPEngRequestHandlerObserver.h"
#include	"MPEngSessionStatusObserver.h"

#include	"PEngPresenceEngineConsts2.h"
#include	"RPEngStorageAdminSubClient.h"


// FORWARD DECLARATIONS
class	MPEngRequestHandler;
class	CPEngIncomingDataHandler;
class	MPEngServer;
class	CPEngSessionSlotId;
class	CPEngAppAccessContainer;
class	MPEngSubSession;
class	MPEngTransactionFactory;
class   RPEngStorageAdminClient;
class   MPEngAccessHandler;
class   MPEngPureDataHandler;
class   RPEngMessage;

// CLASS DECLARATION
/**
 *  One Session manager of the presence server
 *  Handles all management concerning one network session
 *  Opening, closing, ownership handling, updating
 *  of Session related data to network
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngCSPSessManager ):
        public CBase,
        public MPEngRequestHandlerObserver,
        public MPEngSessionStatusObserver
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngCSPSessManager* NewLC( MPEngServer& aServer,
    const CPEngSessionSlotId& aSessId,
    RPEngStorageAdminClient& aAdmin );

    /**
     * Destructor.
     */
    ~CPEngCSPSessManager();

private: // new function of MPEngRequestHandlerObserver
    /**
     *  Remove calling request handler from array of pending requests
     *
     *  @see <MPEngRequestHandlerObserver.h>
     *  @since 3.0
     */
    void CompleteRequestHandler(
        MPEngRequestHandler* aRequestHandler );

    /**
     *  Set Session slot state to open
     *
     *  @see <MPEngRequestHandlerObserver.h>
     *  @since 3.0
     */
    void SetCSPSessionOpenedL();

private: // ne functions from MPEngSessionStatusObserver

    /**
     *	Handle Session status change
     *
     *  @since 3.0
     *
     *  @param new session slot state
     *  @param session slot event
     */
    void StatusChangedL( TPEngNWSessionSlotState aNewState,
                         TPEngNWSessionSlotEvent aNewEvent );

public: // Slot management support

    /**
     *  Open method for clean up stack support
     *
     *  @since 3.0
     */
    void Open();

    /**
     *  Close method for clean up stack support
     *
     *  @since 3.0
     */
    void Close();

    /**
     *  Open method for sub-sessions
     *	Reference to the Sub-session is stored
     *	for future notifications
     *
     *  @since 3.0
     *	@param aServSubSession server sub-session
     */
    void OpenL( MPEngSubSession& aServSubSession );

    /**
     *  Close method for the sub-session
     *
     *  @since 3.0
     * 	@param aServSubSession server sub-session
     */
    void Close( MPEngSubSession* aServSubSession );

    /**
     *	Open reference to the session manager by the app Id
     *
     *  @since 3.0
     *  @param aAppId application Id to open reference
     */
    void AttachAppIdL( const TDesC& aAppId );

    /**
     *	Close reference to the session manager by the app Id
     *
     *  @since 3.0
     *  @param aAppId application Id to open reference
     */
    void DetachAppId( const TDesC& aAppId );

    /**
     *  Check if application Id is active
     *
     *  @since 3.0
     *  @param aAppId application Id
     */
    void AppIdActiveL( const TDesC& aAppId );

    /**
     *	Reserve Session slot for Process Id
     *
     *  @since 3.0
     *  @param aAppId application Id
     *  @param aProcessId process Id to reserve
     */
    void ReserveProcessL( const TDesC& aAppId,
                          const TDesC& aProcessId );

    /**
     *	Activate Process id
     *  Function leaves if there is not such a combination of
     *  process Id and application id registered
     *
     *  @since 3.0
     *  @param aAppId application Id
     *  @param aProcessId process Id to activate
     */
    void ActivateProcessL( const TDesC& aAppId,
                           const TDesC& aProcessId );

    /**
     *  Session slot Id
     *
     *  @since 3.0
     *  @return session slot Id
     */
    const CPEngSessionSlotId& SessionId() const;

public: // Request handling

    /**
     *  Handle asynchronous request
     *
     *  @since 3.0
     *  @param aMessage RPEngMessage of the request
     *  @param aServSubSession server sub-session
     *  @param aServSessionId server session Id
     *  @param aServSubSessionId server sub-session Id of the request
     */
    void HandleAsynchronousRequestL(
        const RPEngMessage& aMessage,
        MPEngSubSession& aServSubSession,
        TInt32 aServSessionId,
        TInt   aServSubSessionId );

    /**
     *  Handle synchronous request
     *
     *  @since 3.0
     *  @param aMessage RPEngMessage of the request
     *  @param aServSessionId server session Id
     *  @param aServSubSessionId server sub-session Id of the request
     */
    void HandleSynchronousRequestL(
        const RPEngMessage& aMessage,
        TInt32 aServSessionId,
        TInt   aServSubSessionId );

    /**
     *	Handle Log In Request
     *
     *	@since 3.0
     *  @param aMessage RPEngMessage of the request
     *  @param aServSubSession server sub-session
     *  @param aServSessionId server session Id
     *  @param aServSubSessionId server sub-session Id of the request
     */
    void HandleLogInRequestL(	const RPEngMessage& aMessage,
                              MPEngSubSession& aSubSession,
                              TInt32 aServSessionId,
                              TInt   aServSubSessionId );

    /**
     *	Handle Log Oout Request
     *
     *	@since 3.0
     *  @param aMessage RPEngMessage of the request
     *  @param aServSubSession server sub-session
     *  @param aServSessionId server session Id
     *  @param aServSubSessionId server sub-session Id of the request
     */
    void HandleForceLogOutRequestL( const RPEngMessage& aMessage,
                                    MPEngSubSession& aSubSession,
                                    TInt32 aServSessionId,
                                    TInt   aServSubSessionId );

    /**
     *  Close reference to the session manager by the app Id
     *	Asynchronous version
     *
     *  @since 3.0
     *  @param aMessage RPEngMessage of the request
     *  @param aServSubSession server sub-session
     *  @param aServSessionId server session Id
     *  @param aServSubSessionId server sub-session Id of the request
     */
    void HandleDetachAppIdL( const RPEngMessage& aMessage,
                             MPEngSubSession& aSubSession,
                             TInt32 aServSessionId,
                             TInt   aServSubSessionId );

    /**
     *  Cancel particular asynchronous request
     *	Cancel all asynchronous request from the Server Session
     *  Cancel all asynchronous request from the Server Sub-session
     *
     *  @since 3.0
     *  @param aMessage RPEngMessage of the request
     *  @param aServSessionId server session Id
     *  @param aServSubSessionId server sub-session Id of the request
     */
    void CancelAsynchronousRequest(
        const RPEngMessage& aMessage,
        TInt32 aServSessionId,
        TInt   aServSubSessionId );

    void CancelAllSessionRequests(
        TInt32 aServSessionId );

    void CancelAllSubSessionRquests(
        TInt32 aServSessionId,
        TInt   aServSubSessionId );

private:	// Function for handling of resources

    /**
     *  Load resources for the online state of the session
     *
     *  @since 3.0
     */
    void LoadResourcesL();

    /**
     *  Unloads resources for the offline state of Session slot
     *
     *  @since 3.0
     */
    void UnloadResources();

private:	// request management

    /**
     *  Check if request is allowed
     *
     *
     *  @since 3.0
     *  @param aRequestFunction request function
     *  @param aServSessionId server session Id of the request
     *	@param aServSubSessionId server sub-session Id of the request
     */
    void CheckRequestValidityL (
        TInt aRequestFunction,
        TInt32 aServSessionId,
        TInt   aServSubSessionId );

    /**
     *  Find defined request in session manager
     *
     *  @since 3.0
     *  @param aRequestFunction request function to find
     *  @param aServSessionId session to look in
     *  @param aServSubSessionId sub-session to look in
     *  @return index or KerrNotFound
     */
    TInt FindRequest( TInt aRequestFunction );

    TInt FindRequestInSession(
        TInt aRequestFunction,
        TInt32 aServSessionId );

    TInt FindRequestInSubSession(
        TInt aRequestFunction,
        TInt32 aServSessionId,
        TInt   aServSubSessionId );

    /**
     *	Handle Send new data Request
     *
     *	@since 3.0
     *	@param aMessage RPEngMessage from Client
     *	@param aServSubSession server sub-session
     *	@param aServSessionId server session id
     *	@param aServSubSessionId server sub-session Id
     */
    void HandleUpdateDataRequestL( const RPEngMessage& aMessage,
                                   MPEngSubSession& aServSubSession,
                                   TInt32 aServSessionId,
                                   TInt   aServSubSessionId );

    /**
     *  Do Log In to the CSP session
     *
     *  @since 3.0
     *	@param aMessage RPEngMessage from Client
     *	@param aServSubSession server sub-session
     *	@param aServSessionId server session id
     *	@param aServSubSessionId server sub-session Id
     */
    void DoLogInL( const RPEngMessage& aMessage,
                   MPEngSubSession& aServSubSession,
                   TInt32 aServSessionId,
                   TInt   aServSubSessionId );

    /**
     *  Do log out from the CSP session
     *
     *  @since 3.0
     *	@param aMessage RPEngMessage from Client
     *	@param aServSubSession server sub-session
     *	@param aServSessionId server session id
     *	@param aServSubSessionId server sub-session Id
     */
    void DoLogOutL( const RPEngMessage& aMessage,
                    MPEngSubSession& aServSubSession,
                    TInt32 aServSessionId,
                    TInt   aServSubSessionId );

private: // Helping functions

    /**
     *  Find Access counter
     *
     *  @since 3.0
     *  @param aAppId application Id
     *  @return pointer to access counter if found, NULL if not found
     */
    CPEngAppAccessContainer* AccessCounter( const TDesC& aAppId );

    /**
     *  Check reference counter and delete this object if needed
     *  @since 3.0
     */
    void CheckReferenceAccessD();

    /**
     *  Unload Transaction factories
     *  @since 3.0
     */
    void UnloadTransactionFactories();

    /**
     *  Reset access Containers
     *  @since 3.0
     */
    void ResetAccessContainers();

    /**
     *  Inform sub-session about app Id disconnection
     *  @since 3.0
     *  @param aAppId disconnected app Id
     */
    void NotifyDisconnectedAppId( const TDesC& aAppId );

    /**
     *  Inform sub-sessions about CPS close state
     *  @since 3.0
     */
    void NotifyCPSClose();

    /**
     *  Notify Storage Server about new event
     *  @since 3.0
     *  @param aEvent new event to notify
     *  @param aAppId application id event is related
     *  @param aAppIdState state of the application id
     */
    void NotifyNewEventL( TPEngNWSessionSlotEvent aEvent,
                          const TDesC& aAppId,
                          TPEngNWSessionSlotState aAppIdState );

    /**
     *  Leaves with error code if not connected.
     */
    void LeaveIfNotConnectedL() const;




private:

    /**
     * C++ default constructor.
     */
    CPEngCSPSessManager( MPEngServer& aServer );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const CPEngSessionSlotId& aSessId,
                     RPEngStorageAdminClient& aAdmin );

private:    // Data

    /// OWN: Access Plain Access Count, used by cleanUp support
    TInt											iAccessCount;

    /// OWN: Session slot Id
    CPEngSessionSlotId*								iSessionSlotId;

    /// OWN: Session state
    TPEngNWSessionSlotState							iState;

    /// OWN: Container of Access Counters
    CObjectCon*										iAccessCounters;

    ///	REF: Server
    MPEngServer&									iServer;

    /// OWN: Server sub-sessions,
    ///      Elements REFerenced
    RPointerArray<MPEngSubSession>					iServSubSessions;

    /// OWN: Storage Manager admin client
    RPEngStorageAdminSubClient						iStoreAdmin;

    /// OWN: Array of active request handlers
    ///      Elements OWNed
    RPointerArray<MPEngRequestHandler>				iRequestHandlers;

    /// OWN: Transaction factories
    RPointerArray<MPEngTransactionFactory>			iTransFactories;

    /// OWN: Online Plugins loaded
    TBool 											iPluginsLoaded;

    /// OWN: Access handlers
    MPEngAccessHandler*								iAccessHandler;

    /// OWN: pure data handler
    MPEngPureDataHandler*							iPureDataHandler;

    /// OWN: list of Incoming Listen Data Handlers
    ///      Elements OWNed
    RPointerArray<CPEngIncomingDataHandler>			iIncomingHandlers;
    };

#endif      // __CPENGCSPSESSMANAGER_H__

// End of File
