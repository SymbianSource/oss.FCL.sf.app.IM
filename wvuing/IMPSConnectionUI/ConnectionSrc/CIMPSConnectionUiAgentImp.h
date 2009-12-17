/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS Connection UI Agent implementation.
*
*/

#ifndef __CIMPSCONNECTIONUIAGENTIMP_H__
#define __CIMPSCONNECTIONUIAGENTIMP_H

//  INCLUDES
#include <E32Base.h>
#include <mimpsconnuipresenceeventobserverng.h>
#include "cimpsconnectionuingagent.h"


// FORWARD DECLARATIONS
class CIMPSConnUiClntFilteredPresEventNotifier;
class MCnUiUiFacade;
class CCnUiClientGroupUiNotifyHandler;
class CCnUiClientGroupUiConnectionLostHandler;
class MCnUiConnModeHandler;



// CLASS DECLARATION
/**
 * CIMPSConnectionUiAgentImp.
 * Implements the CIMPSConnectionUiAgent features.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CIMPSConnectionUiAgentImp ) : public CBase,
        public MIMPSConnUiPresenceEventObserver
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CIMPSConnectionUiAgentImp* NewL( TIMPSConnectionClient aClient );


    /**
     * Destructor.
     */
    ~CIMPSConnectionUiAgentImp();

private:

    /**
     * C++ constructor.
     */
    CIMPSConnectionUiAgentImp( TIMPSConnectionClient aClient );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public:  // New service handling methods


    /**
     * From CIMPSConnectionUiAgentImp
     * @see CIMPSConnectionUiAgentImp
     * @since 2.1
     */
    TInt StartServiceL( TIMPSConnectionUiAgentService aService );


    /**
     * From CIMPSConnectionUiAgentImp
     * @see CIMPSConnectionUiAgentImp
     * @since 2.1
     */
    TInt StopService( TIMPSConnectionUiAgentService aService );


    /**
     * From CIMPSConnectionUiAgentImp
     * @see CIMPSConnectionUiAgentImp
     * @since 2.1
     */
    void StopAllServices();

    /**
     * Sets the new resource file to use in Connection UI.
     *
     * @param aNewResourceFile the new resourcefile to be used
     * @since 2.6
     */
    void UseResourceFileL( const TDesC& aNewResourceFile );


public:  //From MIMPSConnUiPresenceEventObserver

    /**
     * From MIMPSConnUiPresenceEventObserver
     * Notifies from presence event.
     *
     * @since 2.1
     * @param aNotifier The presence notifier where this
     * event is coming from.
     * @param aEvent The presence event.
     */
    void HandlePresenceEventL( CIMPSConnUiPresEventNotifier* aNotifier,
                               const CPEngNWSessionSlotID2& aSessionSlotID,
                               TIMPSConnectionClient aClient,
                               TIMPSPresenceServiceEvent aEvent );


    /**
     * From MIMPSConnUiPresenceEventObserver
     * Called when presence event listening or event
     * handling fails.
     *
     * @since 2.1
     * @param aNotifier The presence notifier where this
     * error is coming from.
     * @param aError is the error code.
     */
    void HandlePresenceEventNotifyError( CIMPSConnUiPresEventNotifier* aNotifier,
                                         TInt aError );


private: //Data

    ///<Client for which behalf this agent is working. Owned.
    const TIMPSConnectionClient                     iClient;

    ///<Client filtered event notifier. Owned
    CIMPSConnUiClntFilteredPresEventNotifier*       iClntNotifier;

    ///<Connection UI facade. Owned
    MCnUiUiFacade*                                  iUi;


    ///<
    CCnUiClientGroupUiNotifyHandler*                iClientGroupRemoteNotifyHandler;

    ///<
    CCnUiClientGroupUiConnectionLostHandler*        iClientGroupConnectionLostHandler;

    ///<
    MCnUiConnModeHandler*                           iClientConnModeHandler;

    ///<Resource file name, owned
    HBufC*											iBrandedResourceFileName;
    };

#endif      //__CIMPSCONNECTIONUIAGENTIMP_H
//  End of File


