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
* Description:  Connection handler implementation.
*
*/

#ifndef __CCNUICONNECTIONHANDLER_H
#define __CCNUICONNECTIONHANDLER_H

//  INCLUDES
#include <E32Base.h>
#include "MCnUiConnectionHandler.h"



//FORWARD DECLARATION
class CIMPSSAPSettings;
class MCnUiClientStatusHandler;
class CCnUiSapStoreProxy;
class CPEngNWSessionSlot2;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * Connection handler implementation.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiConnectionHandler ) : public CBase,
        public MCnUiConnectionHandler
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     * @param aSapProxy The SAP store proxy to use.
     */
    static CCnUiConnectionHandler* NewL( CCnUiSapStoreProxy& aSapProxy );


    /**
     * Destructor.
     */
    virtual ~CCnUiConnectionHandler();


private:

    /**
     * C++ constructor.
     */
    CCnUiConnectionHandler( CCnUiSapStoreProxy& aSapProxy );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // New SAP connection related functions from MCnUiConnectionHandler

    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void OpenSapConnectionL( const CIMPSSAPSettings& aSapToLogin,
                             CPEngNWSessionSlotID2& aNWSessionSlotID,
                             MPEngNWSessionOperationObserver2& aSlotOperationObserver );

    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void CancelSapConnectionOpen( CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void CloseSapConnectionL( CPEngNWSessionSlotID2& aNWSessionSlotID,
                              MPEngNWSessionOperationObserver2& aSlotOperationObserver );


    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void CancelSapConnectionClose( CPEngNWSessionSlotID2& aNWSessionSlotID );


public: // New client login related functions from MCnUiConnectionHandler

    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void LoginTheClientL( TIMPSConnectionClient aClient );

    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void LogoutTheClientL( TIMPSConnectionClient aClient );


public: // New status getters from MCnUiConnectionHandler

    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TBool TheClientLoggedInL( TIMPSConnectionClient aClient );


    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void GetLoggedInClientsL( RArray< TIMPSConnectionClient >& aClients );

    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TBool GetLoggedInSapL( CIMPSSAPSettings& aSap, TIMPSConnectionClient aClient );


    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TCnUiSapCnStatus SapConnectionStatusL( const CIMPSSAPSettings& aSap,
                                           TIMPSConnectionClient aClient );


    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TBool NwConnectionActiveL( CPEngNWSessionSlotID2& aIdToMatch );


    /**
     * From MCnUiConnectionHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TPEngNWSessionSlotState ServiceStatusL( const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Sets the given client as logged out.
     *
     * @since 3.0
     * @param aClient The client to logout.
     */
    void SetSessionSlotL( CPEngNWSessionSlot2* aSlot );


private: // New helpers
    /**
      * Gets the corresponding network session slot for a slot ID
      *
      * @since 3.0
      * @param aNWSessionSlotID the network session slot ID
      * @return the network session slot
      */
    CPEngNWSessionSlot2* NetworkSessionSlotForIDL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID );

    /**
    * Calls ResetAndDestroy to given array
    * @since 3.0
    * @param aObject Array (RPointerArray<CPEngNWSessionSlotID2>)
    */
    static void DestroyCloseModelArray( TAny* aObject );

private: // data

    ///<Client login status handler, owned
    MCnUiClientStatusHandler*               iClientStatusHandler;

    ///<SAP stettings store proxy, not owned
    CCnUiSapStoreProxy&                     iSapProxy;

    // owns: the network session slot ID
    CPEngNWSessionSlotID2*                  iNWSessionSlotID;

    CPEngNWSessionSlot2*                    iNWSessionSlot;

    };

#endif      //  __CCNUICONNECTIONHANDLER_H

//  End of File

