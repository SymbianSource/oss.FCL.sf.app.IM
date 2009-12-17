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
* Description:  Basic control context.
*
*/

#ifndef __CCNUIBASECONTROLCONTEXT_H
#define __CCNUIBASECONTROLCONTEXT_H

//  INCLUDES
#include <E32Base.h>
#include "MCnUiBaseControlContext.h"



// FORWARD DECLARATIONS
class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;
class CCnUiOpContext;
class CCnUiSapStoreProxy;
class CCnUiCntrlStepDriver;
class MCnUiClientPluginAdv;
class MCnUiSignaller;
class MCnUiConnectionHandler;
class MCnUiConnModeHandler;
class MCnUiGlobalNotificationUiFacade;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlot2;

// CLASS DECLARATION
/**
 * Basic control context.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiBaseControlContext ) : public CBase,
        public MCnUiBaseControlContext
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiBaseControlContext* NewLC( TIMPSConnectionClient aClient,
    CCnUiSapStoreProxy& aSapStoreProxy,
    MCnUiConnectionHandler& aConnHandler );


    /**
     * Destructor.
     */
    ~CCnUiBaseControlContext();

protected: //Protected constructors to allow derivation

    /**
     * C++ constructor.
     */
    CCnUiBaseControlContext( TIMPSConnectionClient aClient,
                             CCnUiSapStoreProxy& aSapStoreProxy,
                             MCnUiConnectionHandler& aConnHandler );

    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public: // New public methods

    /**
     * Executes the AA login operation.
     *
     * @param aShowDetailedError Should we show error note to user
     * in case an error occurs during login.
     * @return AA login status.
     * One of following:
     */
    TInt DoAALoginL( TBool aShowDetailedError );


    /**
     * Executes the AA logout operation.
     *
     * @param aIsScheduled Tells if this is a normal scheduled
     * scheduled logout (Need to ask confirmation from user)
     * @return AA login status.
     * One of following:
     */
    TInt DoAALogoutL( TBool aIsScheduled );



protected: // New methods from MCnUiBaseControlContext

    /**
     * From MCnUiBaseControlContext
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiConnectionHandler& ConnHandler();


    /**
     * From MCnUiBaseControlContext
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiSignaller& Signaller();


    /**
     * From MCnUiBaseControlContext
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiConnModeHandler& ConnModeHandler();


    /**
     * From MCnUiBaseControlContext
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    TIMPSConnectionClient ControlledClient();


    /**
     * From MCnUiBaseControlContext
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    CIMPSSAPSettingsStore& SapStoreL();


    /**
     * From MCnUiBaseControlContext
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiClientPlugin& ClientPluginL( TIMPSConnectionClient aClient,
                                      CPEngNWSessionSlotID2& aNWSessionSlotID,
                                      TBool aRefreshPlugin = EFalse );


    /**
     * From MCnUiBaseControlContext
     * @see MCnUiBaseControlContext
     * @since 2.1
     */
    MCnUiGlobalNotificationUiFacade& GlobalNotificationUiL();

    /**
     * Gets the active network session slot id
     * @param aClient the client type
     * @since 3.0
     */
    CPEngNWSessionSlotID2* GetActiveNWSessionSlotIDL( TIMPSConnectionClient aClient );

private:

    /**
    * Calls ResetAndDestroy to given array
    * @since 3.0
    * @param aObject Array (RPointerArray<CPEngNWSessionSlotID2>)
    */
    static void DestroyCloseModelArray( TAny* aObject );

private: //Data

    //NOTE!! control context may not share operation data objects
    //as member variables between individual operations,
    //because this would mess up things in operation nesting...


    ///<Controlled client, owned
    TIMPSConnectionClient           iControlledClient;

    ///<Proxy to access SAP Store, not owned
    CCnUiSapStoreProxy&             iSapStoreProxy;

    ///<Connection handler, owned
    MCnUiConnectionHandler&         iConnHandler;

    ///<Signaller, owned
    MCnUiSignaller*                 iSignaller;

    ///<Connection mode handler, owned
    MCnUiConnModeHandler*           iConnModeHandler;

    ///<PEC Client plug-in, lazy initialized, owned
    MCnUiClientPlugin*              iPluginPEC;

    ///<IM Client plug-in, lazy initialized, owned
    MCnUiClientPlugin*              iPluginIM;

    ///<Global notification UI, lazy initialized, owned
    MCnUiGlobalNotificationUiFacade* iGNUI;

    // Owns: network session slot ID for Always online connections
    CPEngNWSessionSlotID2*          iAASessionSlotID;

    // Owns: network session slot for Always online connections
    CPEngNWSessionSlot2*            iAASessionSlot;
    };

#endif      //__CCNUIBASECONTROLCONTEXT_H
//  End of File


