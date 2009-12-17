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
* Description:  Abstract interface to handle client plug-ins.
*
*/

#ifndef __MCNUICLIENTPLUGIN_H
#define __MCNUICLIENTPLUGIN_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATION
class CIMPSSAPSettings;
class MCnUiClientPlugin;
class CPEngNWSessionSlotID2;

/**
 * Factory method for client plug-in construction.
 *
 * Global factory method to create concrete
 * client plug-ins.
 *
 * @since 2.1
 * @param aClientId Which plug-in to instantiate.
 * @return New plug-in instance.
 */
GLREF_D MCnUiClientPlugin* CreateClientPluginL( TIMPSConnectionClient aClientId,
                                                CPEngNWSessionSlotID2& aNWSessionSlotID );


/**
 * Connection settings mode values
 *
 * @since 2.1
 */
enum TCnUiConnectionMode
    {
    ECnUiCMAutomatic = 0x1, ///<Connection managing mode automatic.
    ECnUiCMManual = 0x2     ///<Connection managing mode manual.
    };


// CLASS DECLARATION
/**
 * Abstract interface to handle client plug-ins.
 *
 * Connection UI calls these client plug-in methods
 * to get certain client settings or to allow the client
 * plug-in to some connection state specific processing.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiClientPlugin )
    {
public: //New settings getters / setters

    /**
     * For future:
     * Change these getters / setters to
     * generic typeless GetParam() / SetParam() mutators.
     *
     * Both numeric and descriptor mutators are needed to
     * preserve API. And also TIMPSConnectionClient ClientId() is needed.
     *
     * Open: How to handle client specific errors / error notes?
     * Maybe it should be possible to retrieve a error text
     * for last error. It might be a problem that plug-ins are executed
     * both application / server context... ?
     */


    /**
     * Gets the plug-ins client id.
     *
     * @since 2.1
     * @return The plug-in client ID.
     */
    virtual TIMPSConnectionClient ClientID() = 0;


    /**
     * Releases dynamic resources owned by the MCnUiClientPlugin.
     *
     * This method is called by the plug-in host to free
     * MCnUiClientPlugin owned "heavy resources" like GPRS engine
     * handles etc.  It is up to client plug-in to decide how to
     * proceed with the request.
     *
     * Plug-in host uses this method to free resources
     * when Connection UI and also plug-in processing is stopped
     * for long time to wait user input.
     *
     * In practice method implementation will first cancel
     * all outstanding requests and then free resources that it can.
     *
     * @since 2.1
     */
    virtual void ReleaseDynamicResources() = 0;

    /**
     * Called in various connection processing stages to
     * determine what is client current connection mode
     * setting.
     *
     * @since 2.1
     * @return Current client connection mode from TCnUiConnectionMode.
     */
    virtual TCnUiConnectionMode CurrentConnectionModeSettingL() = 0;

    /**
     * Called to re-wake client automatic connection mode.
     *
     * @since 2.1
     */
    virtual void ReWakeAutomaticConnectionModeL() = 0;



public: //New connection open related methods

    /**
     * Called to initialize the plug-in for connection open.
     *
     * Called just before opening the network connection.
     * (aSapConnectionOpen is reported as EFalse)
     *
     * Or if the network connection is already open,
     * called before issuing the client asynchronous
     * processing (now the aSapConnectionOpen is ETrue)
     *
     * NOTE!! This initialization request might
     * come several times during one login try
     * depending the Presence Engine behaviour.
     *
     * @since 2.1
     * @param aSapConnectionOpen ETrue if the SAP connection is
     * already open. Else EFalse.
     * @param aSap The SAP settings details.
     */
    virtual void ConnectionOpenInitL( TBool aSapConnectionOpen,
                                      const CIMPSSAPSettings& aSap ) = 0;


    /**
     * Called to determine has the network server enough
     * capabilities for client type. This method is called
     * after the network connection to SAP
     * is succesfully opened.
     *
     * If client returns EFalse (IMPS Network Server doesn't
     * have enough capabilities), login procedure is immediately
     * stopped to insufficient capabilities error, and no
     * further plug-in processing methods is called.
     *
     * @since 2.1
     * @param ETrue if currently connected SAP (IMPS Network Server)
     * has enough capabilities for this client type.
     * Else EFalse.
     */
    virtual TBool VerifySapCapabilitiesL() = 0;


    /**
     * Issues the plug-in to do asynchronous processing
     * related to connection open. This is issued after
     * the plug-in has reported that the network server has
     * enough capabilities.
     *
     * Plug-in has to initialize the given request status
     * to KRequestPending state if it starts the asynchronous
     * processing.
     *
     * If Plug-in leaves from starting the asynchronous
     * processing, it is considered to mean that starting the
     * asynchronous operation failed and there isn't any
     * asynchronous operation to wait. (So leaves are not allowed
     * between real operation start and method call return.)
     *
     * @since 2.1
     * @param aStatus The request status to complete when
     * asynchronous processing is done. Needs to be
     * initialized to KRequestPending state when
     * asynchronous processing is started.
     */
    virtual void DoPostLoginProsessingL( TRequestStatus& aStatus ) = 0;


    /**
     * This method must cancels the previously started
     * post login asynchronous processing.
     *
     * NOTE!! This method implementation must be such that
     * it copes from canceling not started operations /
     * partly constructed object.
     *
     * @since 2.1
     */
    virtual void CancelPostLoginProsessing() = 0;


public: //New connection close related methods


    /**
     * Called to initialize the plug-in for connection close.
     *
     * Called just before issuing the plug-in to do
     * its pre logout processing.
     *
     * @since 2.1
     * @param aSap The SAP settings details.
     */
    virtual void ConnectionCloseInitL( const CIMPSSAPSettings& aSap ) = 0;


    /**
     * Issues the plug-in to do asynchronous processing
     * related to connection close. This processing takes
     * place before closing that actual network connection.
     *
     * Plug-in has to initialize the given request status
     * to KRequestPending state if it starts the asynchronous
     * processing.
     *
     * If Plug-in leaves from starting the asynchronous
     * processing, it is considered to mean that starting the
     * asynchronous operation failed and there isn't any
     * asynchronous operation to wait. (So leaves are not allowed
     * between real operation start and method call return.)
     *
     * @since 2.1
     * @param aStatus The request status to complete when
     * asynchronous processing is done. Needs to be
     * initialized to KRequestPending state when
     * asynchronous processing is started.
     */
    virtual void DoPreLogoutProsessingL( TRequestStatus& aStatus ) = 0;


    /**
     * This method must cancels the previously started
     * pre logout asynchronous processing.
     *
     * NOTE!! This method implementation must be such that
     * it copes from canceling not started operations /
     * partly constructed object.
     *
     * @since 2.1
     */
    virtual void CancelPreLogoutProsessing() = 0;

    /**
     * From MCnUiClientPlugin
     * Publishes commcap attribute closed
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    virtual void LoginCancelledL() = 0;

public: //Destructor

    /**
     * Virtual destructor.
     *
     * Allows destruction using
     * this interface.
     */
    virtual ~MCnUiClientPlugin() {};
    };



#endif      //  __MCNUICLIENTPLUGIN_H

//  End of File

