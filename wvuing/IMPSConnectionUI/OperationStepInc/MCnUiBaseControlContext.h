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
* Description:  Base control context.
*
*/

#ifndef __MCNUIBASECONTROLCONTEXT_H
#define __MCNUIBASECONTROLCONTEXT_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATIONS
class MCnUiConnectionHandler;
class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;
class MCnUiSignaller;
class MCnUiConnModeHandler;
class MCnUiClientPlugin;
class MCnUiGlobalNotificationUiFacade;
class CPEngNWSessionSlotID2;


// CLASS DECLARATION
/**
 * Base control context.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiBaseControlContext )
    {
public: // New static control context access methods

    /**
     * Getter for connection handler.
     * @since 2.1
     * @return A reference to MCnUiConnectionHandler.
     */
    virtual MCnUiConnectionHandler& ConnHandler() = 0;


    /**
     * Gets the signaller.
     *
     * @since 2.1
     * @return The signaller.
     */
    virtual MCnUiSignaller& Signaller() = 0;


    /**
     * Gets the conn mode handler.
     *
     * @since 2.1
     * @return The conn mode handler.
     */
    virtual MCnUiConnModeHandler& ConnModeHandler() = 0;


    /**
     * Gets the controlled client.
     *
     * @since 2.1
     * @return The controlled client.
     */
    virtual TIMPSConnectionClient ControlledClient() = 0;



public: // New dynamic control context access methods

    /**
     * Gets the SAP Settings Store.
     *
     * NOTE!! Dynamic handles might be invalidated
     * with ReleaseDynamicResources() call so
     * long lasting pointer caching shouldn't be used
     * with them.
     *
     * @since 2.1
     * @return The SAP Settings Store.
     */
    virtual CIMPSSAPSettingsStore& SapStoreL() = 0;


    /**
     * Gets the client plug-in.
     *
     * @since 2.1
     * @return The client plug-in.
     */
    virtual MCnUiClientPlugin& ClientPluginL( TIMPSConnectionClient aClient,
                                              CPEngNWSessionSlotID2& aNWSessionSlotID,
                                              TBool aRefreshPlugin = EFalse ) = 0;


    /**
     * Gets the global notify UI.
     *
     * @since 2.1
     * @return The global notify UI.
     */
    virtual MCnUiGlobalNotificationUiFacade& GlobalNotificationUiL() = 0;

    /**
     * Gets the active network session slot id
     * @param aClient the client type
     * @since 3.0
     */
    virtual CPEngNWSessionSlotID2* GetActiveNWSessionSlotIDL(
        TIMPSConnectionClient aClient ) = 0;


protected:  //Destructor

    /**
     * Inline destructor.
     *
     * Destruction using this
     * interface isn't possible.
     */
    virtual ~MCnUiBaseControlContext() {};
    };

#endif      //  __MCNUIBASECONTROLCONTEXT_H

//  End of File

