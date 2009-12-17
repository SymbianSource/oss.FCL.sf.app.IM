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
* Description:  Connection handler interface.
*
*/

#ifndef __MCNUICONNECTIONHANDLER_H
#define __MCNUICONNECTIONHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <PEngPresenceEngineConsts2.h>
#include <impspresenceconnectionuiconstsng.h>



//FORWARD DECLARATION
class CCnUiSapStoreProxy;
class CIMPSSAPSettings;
class MCnUiConnectionHandler;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlot2;
class MPEngNWSessionOperationObserver2;

//DATA TYPES
/**
 * Sap connection status.
 *
 * @since 2.1
 */
enum TCnUiSapCnStatus
    {
    ECnUiSCS_NotConnected = 0,
    ECnUiSCS_SapConnected = 1,
    ECnUiSCS_SapConnected_PwdMissMatch = 2,
    ECnUiSCS_AnotherSapConnected = 3,
    ECnUiSCS_AnotherSapConnected_ClientsNotKnown = 4
    };


/**
 * Factory method for connection handler construction.
 *
 * Global factory method to create concrete
 * connection handler.
 *
 * @since 2.1
 * @param aSapProxy The SAP store proxy to use.
 * @return New MCnUiConnectionHandler instance.
  */
GLREF_D MCnUiConnectionHandler* CreateConnHandlerL( CCnUiSapStoreProxy& aSapProxy );




// CLASS DECLARATION
/**
 * Connection Handler.
 *
 * Abstract interface to to handle connections
 * and client logins.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiConnectionHandler )
    {
public: // SAP connection mutators

    /**
     * Opens the SAP connection.
     *
     * @since 2.1
     * @param aSapToLogin The SAP where to login.
     * @param aStatus Request status to complete
     * when connection open is done.
     */
    virtual void OpenSapConnectionL(
        const CIMPSSAPSettings& aSapToLogin,
        CPEngNWSessionSlotID2& aNWSessionSlotID,
        MPEngNWSessionOperationObserver2& aSlotOperationObserver ) = 0;

    /**
     * Cancels the SAP connection open.
     *
     * @since 2.1
     */
    virtual void CancelSapConnectionOpen( CPEngNWSessionSlotID2& aNWSessionSlotID ) = 0;


    /**
     * Closes the SAP connection.
     *
     * @since 2.1
     */
    virtual void CloseSapConnectionL(
        CPEngNWSessionSlotID2& aNWSessionSlotID,
        MPEngNWSessionOperationObserver2& aSlotOperationObserver ) = 0;


    /**
     * Cancels the SAP connection close.
     *
     * @since 2.1
     */
    virtual void CancelSapConnectionClose( CPEngNWSessionSlotID2& aNWSessionSlotID ) = 0;


public: // Client login mutators

    /**
     * Sets the given client as logged in.
     *
     * @since 2.1
     * @param aClient The client to login.
     */
    virtual void LoginTheClientL( TIMPSConnectionClient aClient ) = 0;


    /**
     * Sets the given client as logged out.
     *
     * @since 2.1
     * @param aClient The client to logout.
     */
    virtual void LogoutTheClientL( TIMPSConnectionClient aClient ) = 0;


    /**
     * Sets the session slot.
     * Note, that if successfull, ownership is transferred away from caller.
     *
     * @since 3.0
     * @param aClient The client to logout.
     */
    virtual void SetSessionSlotL( CPEngNWSessionSlot2* aSlot ) = 0;

public: // Status getters

    /**
     * Checks is the client logged in.
     *
     * @since 2.1
     * @param aClient The client to check.
     * @return ETrue if the client is logged in. Else EFalse.
     */
    virtual TBool TheClientLoggedInL( TIMPSConnectionClient aClient ) = 0;


    /**
     * Gets the logged in clients.
     *
     * @since 2.1
     * @param aClients The array to put logged in clients.
     */
    virtual void GetLoggedInClientsL( RArray< TIMPSConnectionClient >& aClients ) = 0;


    /**
     * Getter for currently logged in SAP.
     *
     * Gets currently logged in SAP. If not
     * currently logged in returns EFalse.
     * Else returns ETrue and aSap is set to
     * contain currently logged SAP's data.
     *
     * @since 2.1
     * @param aSap When logged in, the SAP is filled
     * with logged in SAP's details.
     * @return Login status.
     */
    virtual TBool GetLoggedInSapL( CIMPSSAPSettings& aSap, TIMPSConnectionClient aClient ) = 0;


    /**
     * Gets SAP connection status.
     *
     * @since 2.1
     * @param aSap The SAP to check.
     * @return SAP connection status from TCnUiSapCnStatus.
     */
    virtual TCnUiSapCnStatus SapConnectionStatusL( const CIMPSSAPSettings& aSap,
                                                   TIMPSConnectionClient aClient ) = 0;


    /**
     * Checks is there active network connection
     * or not. Uses the pattern matching of
     * network session slot ID. Assumes either
     * full ID or just application ID.
     *
     * @since 2.1
     * @return ETrue if there is a active NW connection.
     * Else EFalse.
     */
    virtual TBool NwConnectionActiveL( CPEngNWSessionSlotID2& aIdToMatch ) = 0;


    /**
     * Gets service status from PEC Engine.
     *
     * @since 2.1
     * @return Current service status.
     */
    virtual TPEngNWSessionSlotState ServiceStatusL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID ) = 0;


public:  //Destructor

    /**
     * Virtual inline destructor.
     * Allows destruction using this
     * interface
     */
    virtual ~MCnUiConnectionHandler() {};
    };


#endif      //  __MCNUICONNECTIONHANDLER_H

//  End of File

