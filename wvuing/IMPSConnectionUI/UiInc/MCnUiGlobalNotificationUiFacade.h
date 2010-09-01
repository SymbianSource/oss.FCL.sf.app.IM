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
* Description:  Global notification UI Facade for Connection UI.
*
*/

#ifndef __MCNUIGLOBALNOTIFICATIONUIFACADE_H
#define __MCNUIGLOBALNOTIFICATIONUIFACADE_H

//  INCLUDES
#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATION
class MCnUiGlobalNotificationUiFacade;


/**
 * Factory method for global notification UI facade construction.
 *
 * Global factory method to create concrete
 * global notification UI facade.
 *
 * @since 2.1
 * @return New MCnUiGlobalNotificationUiFacade instance.
 */
GLREF_D MCnUiGlobalNotificationUiFacade* CreateGlobalNotificationUiFacadeL();



/**
 * Global notification UI facade interface for Connection UI.
 *
 * Global notification UI facade interface for Connection UI.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiGlobalNotificationUiFacade )
    {
public: // Global notification features

    /**
     * Shows global username / password error note.
     * Method is synchronous, and returns after
     * login timeout / user confirmation.
     *
     * @since 2.1
     * @param aServerName The server where the login failed.
     */
    virtual void ShowLoginUserDataErrorL( const TDesC& aServerName ) = 0;


    /**
     * Shows global detailed login error note.
     *
     * Method is synchronous, and returns after
     * login timeout / user confirmation.
     *
     * @since 2.1
     * @param aServerName The server where the login failed.
     * @param aErrDetDescription The detailed error description.
     * If passed error description is empty string, defaults to
     * generic connection error.
     */
    virtual void ShowLoginDetailedErrorL( const TDesC& aServerName,
                                          const TDesC& aErrDetDescription ) = 0;


    /**
     * Shows global detailed login error note.
     *
     * Method is synchronous, and returns after
     * login timeout / user confirmation.
     *
     * @since 2.1
     * @param aServerName The server where the login failed.
     * @param aNoteTextResourceId The detailed error description resource id.
     * If passed error description id as KErrNone, defaults to
     * generic connection error.
     */
    virtual void ShowLoginDetailedErrorL( const TDesC& aServerName,
                                          TInt aNoteTextResourceId ) = 0;

    /**
     * Shows active connection scheduled close confirmation query
     * to in requested client applications
     *
     * Method is synchronous, and returns after
     * query timeout / user confirmation.
     *
     * @since 2.1
     * @param aClientsToShow The client groups where to
     * show the query. If no clients passed, leaves with KErrArgument.
     * @return ETrue if user accepted the question,
     * else EFalse. Query timeout is considered as acceptance.
     */
    virtual TBool CGQActiveConnectionScheduledCloseL(
        const TArray<TIMPSConnectionClient>& aClientsToShow ) = 0;


    /**
     * Shows confirmation query about active connection close for
     * scheduled connection in requested client applications
     *
     * Method is synchronous, and returns after
     * query timeout / user confirmation.
     *
     * @since 2.1
     * @param aServerName The server where the connection would be done
     * @param aClientsToShow The client groups where to
     * show the query. If no clients passed, leaves with KErrArgument.
     * @return ETrue if user accepted the question,
     * else EFalse. Query timeout is considered as acceptance.
     */
    virtual TBool CGQActiveConnectionSuppressForScheduledConnectionL(
        const TDesC& aServerName,
        const TArray<TIMPSConnectionClient>& aClientsToShow ) = 0;



public:  //Destructor

    /**
     * Virtual inline destructor.
     * Allows destruction using this
     * interface
     */
    virtual ~MCnUiGlobalNotificationUiFacade() {};

    };


#endif      //  __MCNUIGLOBALNOTIFICATIONUIFACADE_H


//  End of File

