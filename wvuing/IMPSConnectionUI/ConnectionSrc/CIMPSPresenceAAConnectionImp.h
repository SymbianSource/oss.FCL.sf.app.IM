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
* Description:  AA connection implementation.
*
*/

#ifndef __CIMPSPRESENCEAACONNECTIONIMP_H
#define __CIMPSPRESENCEAACONNECTIONIMP_H

//  INCLUDES
#include <E32Base.h>
#include <PEngPresenceEngineConsts2.h>
#include <impspresenceconnectionuiconstsng.h>

#include "CCnUiSapStoreProxy.h"



// FORWARD DECLARATIONS
class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;
class MCnUiConnectionHandler;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * CIMPSPresenceAAConnectionImp.
 * Implements the CIMPSPresenceAAConnection
 * features.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CIMPSPresenceAAConnectionImp ) : public CBase
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aSapStore The SAP settings store to use.
     * May be NULL.
     */
    static CIMPSPresenceAAConnectionImp* NewL( CIMPSSAPSettingsStore* aSapStore );


    /**
     * Destructor.
     */
    ~CIMPSPresenceAAConnectionImp();

private:

    /**
     * C++ constructor.
     */
    CIMPSPresenceAAConnectionImp( CIMPSSAPSettingsStore* aSapStore );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public:  // Connection methods for CIMPSPresenceAAConnection

    /**
     * Implements login as described in CIMPSPresenceAAConnection.
     * @see CIMPSPresenceAAConnection
     * @since 2.1
     */
    TInt LoginL( TIMPSConnectionClient aClient,
                 TBool aShowDetailedError = EFalse );


    /**
     * Implements logout as described in CIMPSPresenceAAConnection.
     * @see CIMPSPresenceAAConnection
     * @since 2.1
     */
    TInt LogoutL( TIMPSConnectionClient aClient,
                  TBool aIsScheduled = EFalse );


    /**
      * Implements loging state checking as described in
      * CIMPSPresenceAAConnection.
      * @see CIMPSPresenceAAConnection
      * @since 2.1
      */
    TBool LoggedInL( TIMPSConnectionClient aClient );


    /**
     * Implements pure connection status get from Presence
     * Engine as described in CIMPSPresenceAAConnection.
     *
     * @see CIMPSPresenceAAConnection
     * @since 2.1
     */
    TPEngNWSessionSlotState PureServiceStatusL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Implements the default server name get from SAP
     * settings store as described in CIMPSPresenceAAConnection.
     * !!!Note!!! Ownership is transferred to the caller!
     *
     * @see CIMPSPresenceAAConnection
     * @since 2.6
     */
    HBufC* DefaultServerNameL( TIMPSConnectionClient aClient );

    /**
     * Implements the fetching of active NWSessionSlotID as described in
     * CIMPSPresenceConnectionUi.
     * @see CIMPSPresenceConnectionUi
     * @since 3.0
     */
    CPEngNWSessionSlotID2* GetActiveNWSessionSlotIDL( TIMPSConnectionClient aClient );

private: // New helper methods

    /**
    * Calls ResetAndDestroy to given array
    * @since 3.0
    * @param aObject Array (RPointerArray<CPEngNWSessionSlotID2>)
    */
    static void DestroyCloseModelArray( TAny* aObject );

private: //Data

    ///<SapStore proxy, owned
    CCnUiSapStoreProxy              iSapStoreProxy;

    ///<Connection handler, owned
    MCnUiConnectionHandler*         iConnHandler;

    };

#endif      //__CIMPSPRESENCEAACONNECTIONIMP_H
//  End of File


