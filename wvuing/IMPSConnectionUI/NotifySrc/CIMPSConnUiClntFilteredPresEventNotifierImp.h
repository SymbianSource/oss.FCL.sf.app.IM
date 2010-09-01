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
* Description:  Client filtered event notifier implementation
*
*/

#ifndef __CIMPSCONNUICLNTFILTEREDPRESEVENTNOTIFIERIMP_H
#define __CIMPSCONNUICLNTFILTEREDPRESEVENTNOTIFIERIMP_H


//  INCLUDES
#include <E32Base.h>
#include "CIMPSConnUiPresEventNotifierImpBase.h"

#include <MPEngNWSessionSlotObserver2.h>
#include "MCnUiClientStatusHandler.h"

class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * Client filtered event notifier
 * implementation. Implements the
 * CIMPSConnUiClntFilteredPresEventNotifier
 * features.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CIMPSConnUiClntFilteredPresEventNotifierImp ) :
        public CIMPSConnUiPresEventNotifierImpBase,
        public MPEngNWSessionSlotObserver2,
        public MCnUiClientStatusObserver
    {
public:  // Two-phased constructor and destructor

    /**
     * Two-phased constructor.
     * Constructs client filtered version of
     * event notifier implementation.
     */
    static CIMPSConnUiClntFilteredPresEventNotifierImp* NewL(
        CIMPSConnUiPresEventNotifier& aInterface,
        TIMPSConnectionClient aClient );


    /**
     * Destructor.
     */
    ~CIMPSConnUiClntFilteredPresEventNotifierImp();

private:

    /**
     * C++ constructor.
     */
    CIMPSConnUiClntFilteredPresEventNotifierImp( CIMPSConnUiPresEventNotifier& aInterface,
                                                 TIMPSConnectionClient aClient );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public:  // Methods from CIMPSConnUiPresEventNotifierImpBase

    /**
     * Implements the notify start described
     * in CIMPSConnUiClntFilteredPresEventNotifier.
     * @see CIMPSConnUiClntFilteredPresEventNotifier
     * @since 2.1
     */
    void DoStartL( const TArray<TPEngNWSessionSlotEvent>& aEvents );


    /**
     * Implements notify stop as described
     * in CIMPSConnUiClntFilteredPresEventNotifier.
     * @see CIMPSConnUiClntFilteredPresEventNotifier
     * @since 2.1
     */
    void DoStop();


    /**
     * Implements service status get as described
     * in CIMPSConnUiClntFilteredPresEventNotifier.
     * @see CIMPSConnUiClntFilteredPresEventNotifier
     * @since 2.1
     */
    const CPEngNWSessionSlotID2* AppServiceStatusL(
        TIMPSConnectionClient aClient );


public:  // Methods from MPEngNWSessionSlotObserver2

    /**
     * From MPEngNWSessionSlotObserver2.
     * Converts event to application enumeration and
     * forwards it to observers according the current
     * client online status.
     *
     * @since 3.0
     */
    void HandleNWSessionSlotChangeL(
        CPEngNWSessionSlotNotifier2& aNotifier,
        CPEngNWSessionSlotEvent2& aEvent );

    /**
     * From MPEngNWSessionSlotObserver2.
     * Forwards error to observers.
     *
     * @since 3.0
     */
    void HandleNWSessionSlotError(
        TInt aError,
        CPEngNWSessionSlotNotifier2& aNotifier );




public:  // Methods from MCnUiClientStatusObserver


    /**
     * From MCnUiClientStatusObserver
     *
     * Changes the internal client login status cache
     * and also notifies the observers from pseudo
     * service event.
     *
     * @since 3.0
     */
    void HandleClientLoginStatusChange( TIMPSConnectionClient aRequestedClient,
                                        TBool aClientNowLoggedIn );


private:  // new helper methods

    /**
     * Checks if the given client is logged in
     * @param aClient the client to check
     * @since 3.0
     */
    TBool IsLoggedInL( TIMPSConnectionClient aClient );

    /**
     * Does the actual changing of the internal client
     * login status cache and also notifies the
     * observers from pseudo service event.
     *
     * @since 3.0
     */
    void DoHandleClientLoginStatusChangeL( TIMPSConnectionClient aRequestedClient,
                                           TBool aClientNowLoggedIn );

    /**
    * Calls ResetAndDestroy to given array
    * @since 3.0
    * @param aObject Array (RPointerArray<CPEngNWSessionSlotID2>)
    */
    static void DestroyCloseModelArray( TAny* aObject );

private: //Data

    /// Client according which to filter events, owned
    const TIMPSConnectionClient       iClientToFilter;


    ///< Cached client login status, owned
    TBool                             iClientLoggedIn;


    ///< PEC Engine event notifier, owned
    CPEngNWSessionSlotNotifier2*               iPecEventNotifier;


    ///< Client login status handler, owned
    MCnUiClientStatusHandler*         iClientStatusHandler;

    CPEngNWSessionSlotID2*          iSessionSlotID;

    };

#endif      //__CIMPSCONNUICLNTFILTEREDPRESEVENTNOTIFIERIMP_H
//  End of File


