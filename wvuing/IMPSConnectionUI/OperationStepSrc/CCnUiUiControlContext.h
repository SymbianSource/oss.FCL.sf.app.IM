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
* Description:  UI control context implementation.
*
*/

#ifndef __CCNUIUICONTROLCONTEXT_H
#define __CCNUIUICONTROLCONTEXT_H

//  INCLUDES
#include "CCnUiBaseControlContext.h"
#include "MCnUiUiControlContext.h"


// FORWARD DECLARATIONS
class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;
class CCnUiOpContext;
class CCnUiSapStoreProxy;
class CCnUiConnModeRewaker;
class CCnUiCntrlStepDriver;
class MCnUiClientPluginAdv;
class MCnUiSignaller;
class MCnUiConnModeHandler;
class MCnUiConnectionHandler;
class MCnUiUiFacade;
class MCnUiUiFacade;
class MIMPSConnProcessUi;
class MIMPSConnProcessObserver;
class TProcessObserverProxy;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * UI control context implementation.
 *
 * Extends CCnUiBaseControlContext by offering
 * UI level features.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiUiControlContext ) : public CCnUiBaseControlContext,
        public MCnUiUiControlContext
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aClient The connection client.
     * @param aSapStoreProxy SAP store proxy to use.
     * @param aAAModeReWaker The AA mode rewaker to use.
     * @param aConnHandler Connection handler to use.
     * @param aObserver Process observer to notify. Can be NULL.
     */
    static CCnUiUiControlContext* NewLC( TIMPSConnectionClient aClient,
    CCnUiSapStoreProxy& aSapStoreProxy,
    MCnUiConnModeRewaker& aConnModeRewaker,
    MCnUiConnectionHandler& aConnHandler,
    MIMPSConnProcessObserver* aObserver );

    /**
     * Destructor.
     */
    ~CCnUiUiControlContext();

private:

    /**
     * C++ constructor.
     */
    CCnUiUiControlContext( TIMPSConnectionClient aClient,
                           CCnUiSapStoreProxy& aSapStoreProxy,
                           MCnUiConnModeRewaker& aConnModeRewaker,
                           MCnUiConnectionHandler& aConnHandler );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( MIMPSConnProcessObserver* aObserver );




public: // New methods

    /**
     * Performs login for in construction
     * given client.
     *
     * @since 2.1
     * @param aLoginType The login type.
     * @return Login status code.
     */
    TInt DoLoginL( TIMPSLoginType aLoginType, CPEngNWSessionSlotID2& aNWSessionSlotID,
                   CIMPSSAPSettings* aSap = NULL
                                            , TBool aLastLoginSap = EFalse );


    /**
     * Performs logout for in construction
     * given client.
     *
     * @since 2.1
     * @return Logout status code.
     */
    TInt DoLogoutL( const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Handles application exit according the
     * given exit type parameter.
     *
     * @since 2.1
     * @return Disconnect status code.
     */
    TInt DoHandleApplicationExitL( TIMPSExitType aExitType,
                                   const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: // New methods from MCnUiUiControlContext


    /**
     * From MCnUiUiControlContext
     * @see MCnUiUiControlContext
     * @since 2.1
     */
    MCnUiUiFacade& Ui();

    /**
     * From MCnUiUiControlContext
     * @see MCnUiUiControlContext
     * @since 2.1
     */
    MCnUiConnProcessObserverProxy& ProcessObserverProxy();


    /**
     * From MCnUiUiControlContext
     * @see MCnUiUiControlContext
     * @since 2.1
     */
    MCnUiConnModeRewaker& ConnModeRewaker();


    /**
     * From MCnUiUiControlContext
     * @see MCnUiUiControlContext
     * @since 2.1
     */
    TInt SubOpDisconnectAllL( CIMPSSAPSettings& aDisconnectedSap,
                              RArray< TIMPSConnectionClient >& aDisconnectedClients,
                              const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * From MCnUiUiControlContext
     * @see MCnUiUiControlContext
     * @since 2.1
     */
    TInt SubOpReconnectAllL( CIMPSSAPSettings& aReconnectedSap,
                             CPEngNWSessionSlotID2& aNWSessionSlotID );

public: //Methods from MCnUiBaseControlContext

    /**
     * From MCnUiBaseControlContext
     *
     * Forwards request to CCnUiBaseControlContext
     * base class.
     *
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiConnectionHandler& ConnHandler();


    /**
     * From MCnUiBaseControlContext
     *
     * Forwards request to CCnUiBaseControlContext
     * base class.
     *
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiSignaller& Signaller();


    /**
     * From MCnUiBaseControlContext
     *
     * Forwards request to CCnUiBaseControlContext
     * base class.
     *
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiConnModeHandler& ConnModeHandler();


    /**
     * From MCnUiBaseControlContext
     *
     * Forwards request to CCnUiBaseControlContext
     * base class.
     *
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    TIMPSConnectionClient ControlledClient();


    /**
     * From MCnUiBaseControlContext
     *
     * Forwards request to CCnUiBaseControlContext
     * base class.
     *
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    CIMPSSAPSettingsStore& SapStoreL();


    /**
     * From MCnUiBaseControlContext
     *
     * Forwards request to CCnUiBaseControlContext
     * base class.
     *
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiClientPlugin& ClientPluginL( TIMPSConnectionClient aClient,
                                      CPEngNWSessionSlotID2& aNWSessionSlotID,
                                      TBool aRefreshPlugin = EFalse );


    /**
     * From MCnUiBaseControlContext
     *
     * Forwards request to CCnUiBaseControlContext
     * base class.
     *
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiGlobalNotificationUiFacade& GlobalNotificationUiL();

    /**
     * Gets the active network session slot id
     * @param aClient the client type
     * @since 3.0
     */
    CPEngNWSessionSlotID2* GetActiveNWSessionSlotIDL(
        TIMPSConnectionClient /*aClient*/ ) {
        return NULL;
        };

public:  // Other new functions

    /**
     * Displays a list query for selecting a domain ("@domain.com" etc.)
     * @param aSelectedDomain Here is copied the domain user selected.
     * @param aSap Current SAP settings, previously selected domain is
     *             stored per SAP.
     * @return If user cancelled the query return 0,
     *         otherwise the dialog dismiss key code
     * @since 3.1u
     */
    static TInt DisplayDomainSelectionQueryL( TDes& aSelectedDomain,
                                              CIMPSSAPSettings* aSap );

private: //Local helpers


    /**
     * Private helper method to execute driver
     *
     * @since 2.1
     * @param aDriver
     * @param aPopAndDestroyCount How many objects to
     * PopAndDestroy() after driver execution.
     @ return Return value from driver execution.
     */
    TInt ExecuteDriverInUiLX( CCnUiCntrlStepDriver& aDriver, TInt aPopAndDestroyCount );


    /**
     * Reads integer resource value from resources
     * @param aResourceId resource id which is read
     * @return value of wanted integer resource flag
     */
    TInt IntResourceValueL( TInt aResourceId );


private: //Data

    //NOTE!! control context may not share operation data objects
    //as member variables between individual operations,
    //because this would mess up things in operation nesting...

    ///<Ui facade, owned
    MCnUiUiFacade*                  iUi;


    ///<Process observer proxy, owned
    TProcessObserverProxy*          iProcessObserverProxy;


    ///<The conn mode rewaker to use, not owned
    MCnUiConnModeRewaker&           iConnModeRewaker;

    };

#endif      //__CCNUIUICONTROLCONTEXT_H
//  End of File


