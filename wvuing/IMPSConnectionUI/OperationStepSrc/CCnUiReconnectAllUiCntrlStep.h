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
* Description:  Network connection restore UI control.
*
*/

#ifndef __CCNUIRECONNECTALLUICNTRLSTEP_H
#define __CCNUIRECONNECTALLUICNTRLSTEP_H


//  INCLUDES
#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>
#include "CCnUiLoginUiCntrlStep.h"


//FORWARD DECLARATION
class CIMPSSAPSettings;
class CPEngNWSessionSlotID2;


// CLASS DECLARATION
/**
 * Network connection restore UI control.
 * Restores network connection to all
 * given clients.
 *
 * Extends CCnUiLoginUiCntrlStep behaviour
 * by looping back from HandleCompleteL()
 * after each client login.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiReconnectAllUiCntrlStep ) : public CCnUiLoginUiCntrlStep
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     * @param aCCntxt UI control context.
     * @param aLoginSap The sap to reconnect.
     * @param aReconnectedClients The clients to reconnect.
     */
    static CCnUiReconnectAllUiCntrlStep* NewLC( MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLoginSap,
    TIMPSConnectionClient aReconnectedClient,
    CPEngNWSessionSlotID2& aNWSessionSlotID );

    /**
     * Destructor.
     */
    virtual ~CCnUiReconnectAllUiCntrlStep();


protected: //protected to allow derivation

    /**
     * C++ constructor.
     */
    CCnUiReconnectAllUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                                  CIMPSSAPSettings& aLoginSap,
                                  TIMPSConnectionClient aReconnectedClient,
                                  CPEngNWSessionSlotID2& aNWSessionSlotID );

    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // Overridden functions from MCnUiCntrlStep


    /**
     * Overridden HandleCompleteL().
     * If there is still more clients to login,
     * issues a new login round.
     *
     * @since 2.1
     * @return ECnUiRestartStep if there is another clients to reconnect.
     * Else ECnUiStepContinueTeardown.
     */
    TCnUiHandleCompleteStatus HandleCompleteL();


    /**
     * Undo step.
     *
     * @since 2.1
     */
    void UndoStepL();


protected: // New helper functions

    /**
     * Over written framework method.
     *
     * Setups the reconnect step as needed.
     *
     * @since 2.1
     * @param aClient Init has to initialize the aClient
     * with the client id to log out.
     *
     * @return Error status from init. If init returns something
     * else  than ECnUiLoginInitContinue login operation RunStepL()
     * is breaked with return value.
     */
    TInt DoInitLoginStepL( TIMPSConnectionClient& aClient );


    /**
     * Over written framework method.
     *
     * Shows proper wait note for
     * reconnect state.
     *
     * @since 2.1
     * @param aStateId State id from CCnUiConnOpener.
     * @param aClient The accessed client.
     */
    void DoShowLoginStateSpecificWaitnoteL( TInt aStateId,
                                            TIMPSConnectionClient aClient );



protected: // New helper functions

    /**
     * Initializes the disconnect.
     * Gets the SAP o disconnect and
     * populates the clients to logout.
     *
     * @since 2.1
     */
    void InitNWReConnectL();


    /**
     * Shows error note from simultaneous
     * login operation.
     *
     * @since 2.1
     */
    void ShowSimultaneousLoginOperationNoteL();


private: // private data

    ///<Cached reference to aReconnectedClients parameter, not owned
    TIMPSConnectionClient                   iRequestedReconnClient;

    ///<Has the reconnect initialization done, owned
    TBool                                   iInitDone;

    ///<Clients to reconnect, owned
    RArray< TIMPSConnectionClient >         iClientsToReConn;

    CPEngNWSessionSlotID2&                  iNWSessionSlotID;

    };

#endif      //  __CCNUIRECONNECTALLUICNTRLSTEP_H

//  End of File

