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
* Description:  Client status handler.
*
*/

#ifndef __MCNUICLIENTSTATUSHANDLER_H
#define __MCNUICLIENTSTATUSHANDLER_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATION
class MCnUiClientStatusObserver;
class MCnUiClientStatusHandler;

/**
 * Factory method for client status
 * handler construction.
 *
 * Global factory method to create concrete
 * client status handler.
 *
 * @since 2.1
 * @return New MCnUiClientStatusHandler instance.
 * Leaves created object on CleanupStack.
 */
GLREF_D MCnUiClientStatusHandler* CreateClientStatusHandlerLC();



// CLASS DECLARATION
/**
 * Abstract interface to handle client login
 * status.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiClientStatusHandler )
    {
public: // Client status related functions


    /**
     * Checks is there any client marked as
     * logged in.
     *
     * @since 2.1
     * @return ETrue if there is atleast
     * one known client logged in.
     * Else EFalse.
     */
    virtual TBool AnyClientLoggedIn() = 0;


    /**
     * Checks is the given client marked as
     * logged in.
     *
     * @since 2.1
     * @param aClient The client to check.
     * @return ETrue if the client is logged in.
     * Else EFalse.
     */
    virtual TBool ClientLoggedIn( TIMPSConnectionClient aClient ) = 0;


    /**
     * Sets the client as logged in.
     *
     * @since 2.1
     * @param aClient The client to mark in.
     */
    virtual void SetClientLoggedInL( TIMPSConnectionClient aClient ) = 0;


    /**
     * Clear all logged in flags.
     *
     * @since 2.1
     */
    virtual void SetNoClientsLoggedInL() = 0;


    /**
     * Removes the client from loged in.
     *
     * @since 2.1
     * @param aClient The client to take out of logged in ones.
     */
    virtual void SetClientLoggedOutL( TIMPSConnectionClient aClient ) = 0;


    /**
     * Requests notifications from certain client login
     * staus changes. There can be just one this kind
     * of reqest active at time per one MCnUiClientStatusHandler.
     *
     * @since 2.1
     * @param aObserver The observer to notify. Must be a valid object.
     * @param aClientToNotify The client for which login status to notify.
     */
    virtual void NotifyClientLoginStatusChangesL( MCnUiClientStatusObserver* aObserver,
                                                  TIMPSConnectionClient aClientToNotify ) = 0;

    /**
     * Cancels any previously issued
     * client status notify.
     *
     * @since 2.1
     */
    virtual void CancelClientStatusNotify() = 0;


public:  //Destructor

    /**
     * Virtual inline destructor.
     *
     * Allows destruction using this
     * interface
     *
     * Cancels any previously issued
     * client status notify.
     */
    virtual ~MCnUiClientStatusHandler() {};
    };



/**
 * Observer interface to report client
 * status changes.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiClientStatusObserver )
    {
public:  // New observation methods

    /**
     * Method called to handle client
     * status changes.
     *
     * @since 2.1
     * @param aRequestedClient The client for which the
     * notifiaction was requested.
     * @param aClientNowLoggedIn ETrue if the client is
     * now logged in. EFalse if the client is now logged out.
     */
    virtual void HandleClientLoginStatusChange( TIMPSConnectionClient aRequestedClient,
                                                TBool aClientNowLoggedIn ) = 0;

protected:  //Destructor

    /**
     * Virtual inline destructor.
     *
     * Concrete observer can't be destroyed
     * using this interface.
     */
    virtual ~MCnUiClientStatusObserver() {};
    };


#endif      //  __MCNUICLIENTSTATUSHANDLER_H

//  End of File

