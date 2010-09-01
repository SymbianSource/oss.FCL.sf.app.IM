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
* Description:  Connection UI implementation.
*
*/

#ifndef __CIMPSPRESENCECONNECTIONUIIMP_H
#define __CIMPSPRESENCECONNECTIONUIIMP_H

//  INCLUDES
#include <e32base.h>
#include "cimpspresenceconnectionuing.h"
#include "CCnUiSapStoreProxy.h"


// FORWARD DECLARATIONS
class CIMPSSAPSettingsStore;
class MIMPSConnProcessObserver;
class MCnUiConnectionHandler;
class CCnUiConnModeRewaker;



// CLASS DECLARATION
/**
 * CIMPSPresenceConnectionUiImp.
 * Implements the CIMPSPresenceConnectionUi features.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CIMPSPresenceConnectionUiImp ) : public CBase
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     * @param aSapStore The SAP Settings store to use.
     * If NULL pointer is used, creates a temporary one
     * when needed.
     */
    static CIMPSPresenceConnectionUiImp* NewL( TIMPSConnectionClient aClient,
    CIMPSSAPSettingsStore* aSapStore );


    /**
     * Destructor.
     */
    ~CIMPSPresenceConnectionUiImp();

private:

    /**
     * C++ constructor.
     */
    CIMPSPresenceConnectionUiImp( TIMPSConnectionClient aClient,
                                  CIMPSSAPSettingsStore* aSapStore );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public:  // Connection methods for CIMPSPresenceConnectionUi

    /**
     * Implements login as described in CIMPSPresenceConnectionUi.
     * @see CIMPSPresenceConnectionUi
     * @since 2.1
     */
    TInt LoginL( TIMPSConnectionClient aClient,
                 TIMPSLoginType aLoginType,
                 CPEngNWSessionSlotID2& aNWSessionSlotID,
                 MIMPSConnProcessObserver* aObserver,
                 CIMPSSAPSettings* aSap = NULL, TBool aLastLoginSap = EFalse );


    /**
     * Implements logout as described in CIMPSPresenceConnectionUi.
     * @see CIMPSPresenceConnectionUi
     * @since 2.1
     */
    TInt LogoutL( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                  MIMPSConnProcessObserver* aObserver );



    /**
     * Implements application exit handling as described
     * in CIMPSPresenceConnectionUi.
     *
     * @see CIMPSPresenceConnectionUi
     * @since 2.1
     */
    TInt HandleApplicationExitL( TIMPSConnectionClient aClient,
                                 TIMPSExitType aExitType,
                                 const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                 MIMPSConnProcessObserver* aObserver );

    /**
     * Implements the fetching of active NWSessionSlotID as described in
     * CIMPSPresenceConnectionUi.
     * @see CIMPSPresenceConnectionUi
     * @since 3.0
     */
    CPEngNWSessionSlotID2* GetActiveNWSessionSlotIDL( TIMPSConnectionClient aClient );


    /**
     * Implements logged in sap get as described in
     * CIMPSPresenceConnectionUi.
     * @see CIMPSPresenceConnectionUi
     * @since 2.1
     */
    TInt GetLoggedInSapL( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                          CIMPSSAPSettings& aSap );


    /**
     * Implements pure connection status get from Presence
     * Engine as described in CIMPSPresenceConnectionUi.
     *
     * @see CIMPSPresenceConnectionUi
     * @since 2.1
     */
    TPEngNWSessionSlotState PureServiceStatusL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Implements connection operation get as
     * described in CIMPSPresenceConnectionUi.
     *
     * @see CIMPSPresenceConnectionUi
     * @since 2.1
     */
    TIMPSConnectionOperation CurrentConnectionOperationL( HBufC*& aAddtionalDesc );

    /**
     * Implements client login status getting as
     * described in CIMPSPresenceConnectionUi.
     *
     * @see CIMPSPresenceConnectionUi
     * @since 3.0
     */
    TBool LoggedInL( TIMPSConnectionClient aClient );

public:  // Other methods for CIMPSPresenceConnectionUi

    /**
     * Displays a domain selection query
     * described in CIMPSPresenceConnectionUi.
     *
     * @see CIMPSPresenceConnectionUi
     * @since 3.1u
     */
    TInt DisplayDomainSelectionQueryL( TDes& aSelectedDomain,
                                       CIMPSSAPSettings* aSap );

public:  // New methods, left public to allow testing

    /**
     * Remaps internal login error codes to external ones as
     * defined in CIMPSPresenceConnectionUi interface.
     *
     * @see CIMPSPresenceConnectionUi
     * @since 2.1
     */
    static TInt MapToUiLoginReturnStatus( TInt aOperationResult );


private: // New helper methods

    /**
     * Stores network session slot ownership
     *
     * @return error code
     */
    void StoreNWSessionOwnershipL( const CPEngNWSessionSlotID2& aNWSessionSlotID );

    /**
     * Restores network session slot ownership
     *
     * @return error code
     */
    void ReStoreNWSessionOwnershipL();

    /**
    * Calls ResetAndDestroy to given array
    * @since 3.0
    * @param aObject Array (RPointerArray<CPEngNWSessionSlotID2>)
    */
    static void DestroyCloseModelArray( TAny* aObject );


private: //Data

    ///<SapStore proxy, owned
    CCnUiSapStoreProxy          iSapStoreProxy;

    ///<Synchronous connection handler, owned
    MCnUiConnectionHandler*     iConnHandler;

    ///<Connection mode rewaker, owned
    CCnUiConnModeRewaker*       iConnModeRewaker;

    ///<Client using Connection UI
    TIMPSConnectionClient       iClient;

    };

#endif      //__CIMPSPRESENCECONNECTIONUIIMP_H
//  End of File


