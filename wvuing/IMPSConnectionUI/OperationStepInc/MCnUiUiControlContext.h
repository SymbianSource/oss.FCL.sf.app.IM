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
* Description:  UI controll context.
*
*/

#ifndef __MCNUIUICONTROLCONTEXT_H
#define __MCNUIUICONTROLCONTEXT_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include "MCnUiBaseControlContext.h"



//FORWARD DECLARATIONS
class MCnUiUiFacade;
class MCnUiConnProcessObserverProxy;
class MCnUiConnModeRewaker;
class CIMPSSAPSettings;
class CPEngNWSessionSlotID2;



/**
 * UI controll context.
 *
 * Extends base control context features by offering
 * full UI services.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiUiControlContext ) : public MCnUiBaseControlContext
    {
public: // New static UI context access methods


    /**
     * Gets UI facade.
     *
     * @since 2.1
     * @return UI facade.
     */
    virtual MCnUiUiFacade& Ui() = 0;


    /**
     * Gets process observer proxy.
     *
     * @since 2.1
     * @return Process observer proxy.
     */
    virtual MCnUiConnProcessObserverProxy& ProcessObserverProxy() = 0;


    /**
     * Gets connection mode rewaker.
     *
     * @since 2.1
     * @return Connection mode rewaker.
     */
    virtual MCnUiConnModeRewaker& ConnModeRewaker() = 0;



public: // New sub operation methods

    /**
     * Disconnects all currently connected clients.
     *
     * @since 2.1
     *
     * @param aDisconnectedSap On the return contains disconnected SAP details.
     * @param aDisconnectedClients On the return contains disconnected clients.
     * @return Error status from disconnect all operation.
     * KCnUiErrorNoClientsToDisconnect if there wasn't any client to disconnect.
     */
    virtual TInt SubOpDisconnectAllL( CIMPSSAPSettings& aDisconnectedSap,
                                      RArray< TIMPSConnectionClient >& aDisconnectedClients,
                                      const CPEngNWSessionSlotID2& aNWSessionSlotID ) = 0;


    /**
     * Reconnects all listed clients to given SAP.
     *
     * @since 2.1
     *
     * @param aReconnectedSap The SAP to reconnect.
     * @param aReconnectedClients The clients to reconnect.
     * @return Error status from reconnect all operation.
     */
    virtual TInt SubOpReconnectAllL( CIMPSSAPSettings& aReconnectedSap,
                                     CPEngNWSessionSlotID2& aNWSessionSlotID ) = 0;


protected:  //Destructor

    /**
     * Inline destructor.
     *
     * Destruction using this
     * interface isn't possible.
     */
    ~MCnUiUiControlContext() {};
    };



/**
 * Process observer proxy.
 * Controls access to process
 * observer.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiConnProcessObserverProxy )
    {
public: //

    /**
     * Notifies possibly reqistered
     * process observer from SAP access.
     *
     * @since 2.1
     */
    virtual void NotifyAccessingSapL( const CIMPSSAPSettings& aAccessedSap ) = 0;



protected:  //Destructor

    /**
     * Inline destructor.
     *
     * Destruction using this
     * interface isn't possible.
     */
    virtual ~MCnUiConnProcessObserverProxy() {};
    };



#endif      //  __MCNUIUICONTROLCONTEXT_H

//  End of File

