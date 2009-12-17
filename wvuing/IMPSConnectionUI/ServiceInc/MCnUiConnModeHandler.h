/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for Connection mode handling.
*
*/

#ifndef __MCNUICONNMODEHANDLER_H
#define __MCNUICONNMODEHANDLER_H

//  INCLUDES
#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATION
class MCnUiConnModeHandler;
class MCnUiConnModeObserver;


/**
 * Global factory method to create connection mode handler.
 *
 * @since 2.1
 * @return New MCnUiConnModeHandler instance.
  */
GLREF_D MCnUiConnModeHandler* CreateConnModeHandlerL();



/**
 * Raw mode events that the clients can send trough this interface
 *
 * @since 2.1
 */
enum TIMPSUserConnectionSelection
    {
    EUserLevelLogin,                  ///<manual login
    EUserLevelLogout,                 ///<manual logout
    EUserLevelAAConnectionStart       ///<AA connection started
    };



// CLASS DECLARATION
/**
 * Interface for Connection mode handling.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiConnModeHandler )
    {
public: // New connection mode related functions.


    /**
     * Sends connection mode connection related event.
     *
     * @since 2.1
     * @param aConnMode New connection mode event.
     * @param aClient The client for which to send.
     */
    virtual void SendUserSelectionL( TIMPSUserConnectionSelection aUserSelection,
                                     TIMPSConnectionClient aClient ) = 0;

    /**
     * Registers this MCnUiConnModeHandler instance as
     * significant scheduling client.
     *
     * When first client is counted as significant connection client
     * "EIMPSCMEFirstSSClientStart" is notified to registered
     * MCnUiConnModeObserver observers. When last client
     * cancels the counting, the "EIMPSCMELastSSClientStop"
     * event is posted to MCnUiConnModeObserver observers.
     *
     * @since 2.1
     * @param aClient The client for which to count.
     * @return KErrNone if could successfully register.
     *         KErrInUse if counting already in use.
     */
    virtual TInt RegisterAsSignificantSchedulingClientL( TIMPSConnectionClient aClient ) = 0;


    /**
     * Cancels any previously issued significant client
     * registering. Cancellation is also done automaticly
     * in the MCnUiConnModeHandler destructor.
     *
     * @since 2.1
     */
    virtual void CancelRegisteringAsSSClient() = 0;


    /**
     * Gets the connection mode.
     *
     * Gets the connection mode for given client.
     *
     * @since 2.1
     * @param aClient The client for which to get the event
     * @param aClient
     * @return Client current connection mode from requested event category.
     */
    virtual TIMPSConnectionModeEvent ConnectionModeL(
        TIMPSConnectionClient aClient,
        TIMPSConnectionModeEvent aModeEventCateqory ) = 0;


    /**
     * Requests notification from given client connection
     * mode changes.
     *
     * @since 2.1
     * @param aObserver The observer to notify.
     * @param aClientToNotify For which client to report
     * mode changes.
     */
    virtual void NotifyConnectionModeChangesL( MCnUiConnModeObserver* aObserver,
                                               TIMPSConnectionClient aClientToNotify ) = 0;

    /**
     * Cancels any previously issued connection mode notify.
     *
     * @since 2.1
     */
    virtual void CancelConnectionModeNotify() = 0;

public:  //Destructor

    /**
     * Virtual inline destructor.
     *
     * Concrete object can be destructed
     * using this interface.
     *
     * Cancels any previously issued connection
     * mode notify.
     */
    virtual ~MCnUiConnModeHandler() {};
    };



/**
 * Connection mode observer.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiConnModeObserver )
    {
public:  // New observation methods

    /**
     * Method called to handle connection mode notifications.
     *
     * @since 2.1
     * @param aRequestedClient The clientfor which the connection
     * mode changes was requested.
     * @param aConnMode The new connection mode for client.
     */
    virtual void HandleConnModeChange( TIMPSConnectionClient aRequestedClient,
                                       TIMPSConnectionModeEvent aConnMode ) = 0;

protected:  //Destructor

    /**
     * Virtual inline destructor.
     *
     * Concrete observer can't be destroyed
     * using this interface.
     */
    virtual ~MCnUiConnModeObserver() {};
    };


#endif      //  __MCNUICONNMODEHANDLER_H

//  End of File

