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
* Description:  Presence Server class
*
*/


#ifndef __CPENGSERVER_H__
#define __CPENGSERVER_H__

//  INCLUDES
#include <e32def.h>
#include <s32std.h>
#include <e32svr.h>
#include <s32file.h>	// file server

#include	"MPEngServer.h"
#include	"RPEngStorageAdminClient.h"

// FORWARD DECLARATION
class TPEngServerParams;

// CLASS DECLARATION
/**
 *  Presence Server server class
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngServer ):
        public CPolicyServer,
        public MPEngServer
    {
public:		// Constructors and destructors

    /**
     *  Two-phased constructor.
     *  @since 3.0
     *  @param aPriority priority of the server
     */
    static CPEngServer* NewL( TInt aPriority );

    static CPEngServer* NewLC( TInt aPriority );

    /**
     * Destructor.
     */
    virtual ~CPEngServer();

public:	// new functions from MPEngServer

    /**
     *  Inform server about created session,
     *  @see <MPEngServer.h>
     */
    void SessionCreated();

    /**
     *  Inform server about closing of the session
     *  @see <MPEngServer.h>
     */
    void SessionDied();

    /**
     *  Add subsession to the container
     *  @see <MPEngServer.h>
     */
    void AddSubSessionL( CPEngSubSession& aSubSession );

    /**
     *  Remove subsession from the container
     *  @see <MPEngServer.h>
     */
    void RemoveSubSessionL( CPEngSubSession& aSubSession );

    /**
     *  Session Manager
     *  @see <MPEngServer.h>
     */
    CPEngCSPSessManager* CSPSessionManagerLC(
        CPEngSessionSlotId& aSessionId );

    /**
     *  Remove Session Manager from the server
     *  @see <MPEngServer.h>
     */
    void RemoveCSPSessManager( CPEngCSPSessManager* aManager );

    /**
     *  Session is opened
     *  @see <MPEngServer.h>
     */
    void SessionOpened();

    /**
     *  Session is closed
     *  @see <MPEngServer.h>
     */
    void SessionClosed();

public:		// New methods

    /*
     *  Starts the server
     *  @since 3.0
     *  @param aParams parameters for server start up
     */
    static TInt ExecuteServerL( TPEngServerParams& aParams );

    /*
     *	Stops the server,
     *  actually just calls CActiveScheduler::Stop();
     *  @since 3.0
     */
    void StopServer();

    /*
     *	Run server
     *  @since 3.0
     *  @param aParams parameters to run server with
     */
    static void RunServerL( TPEngServerParams& aParams );

public:		// Methods derived from CServer2

    /*
     *	Create new session handler
     *  @see <CServer2.h>
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

private:	// New methods

    /*
     *  Start server listening
     *	Calls CActiveScheduler::Start()
     *  @since 3.0
     */
    void StartServer();

    /*
     *  Check server access count
     *  @since 3.0
     */
    void CheckServerAccess();

protected:	// Constructors and destructors

    /**
     * C++ default constructor.
     */
    CPEngServer( TInt aPriority );

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();		// Second phase construct

private:	// Data

    /// OWN: Object Container of Sub-Sessions, owned over iContainerIx
    CObjectCon*								iContainer;

    /// OWN: Flag if server is running
    TBool									iRunning;

    /// OWN: Session count
    TInt									iSessCount;

    /// OWN: Opened sessions count
    TInt									iOpenedSessions;

    /// OWN: Container of the CSP Managers
    ///      Elements OWNed
    RPointerArray<CPEngCSPSessManager>		iSessionsManagers;

    /// OWN: Storage Admin main client
    RPEngStorageAdminClient					iStoreAdmin;
    };

#endif      //  __CPENGSERVER_H__

