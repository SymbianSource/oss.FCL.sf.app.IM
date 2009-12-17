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
* Description:  Concrete client login status handler.
*
*/

#ifndef __CCNUICLIENTSTATUSHANDLER_H
#define __CCNUICLIENTSTATUSHANDLER_H

//  INCLUDES
#include <E32Base.h>
#include "CCnUiGroupChannel.h"
#include "MCnUiClientStatusHandler.h"


// CLASS DECLARATION
/**
 * Concrete client login status handler.
 * Implements MCnUiClientStatusHandler features
 * using shared data.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiClientStatusHandler ) : public CBase,
        public MCnUiClientStatusHandler,
        public MCnUiGroupChannelListener
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiClientStatusHandler* NewLC();


    /**
     * Destructor.
     */
    virtual ~CCnUiClientStatusHandler();

private:

    /**
     * C++ constructor.
     */
    CCnUiClientStatusHandler();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // New functions from MCnUiClientStatusHandler

    /**
     * From MCnUiClientStatusHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TBool AnyClientLoggedIn();


    /**
     * From MCnUiClientStatusHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TBool ClientLoggedIn( TIMPSConnectionClient aClient );


    /**
     * From MCnUiClientStatusHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void SetNoClientsLoggedInL();


    /**
     * From MCnUiClientStatusHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void SetClientLoggedInL( TIMPSConnectionClient aClient );


    /**
     * From MCnUiClientStatusHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void SetClientLoggedOutL( TIMPSConnectionClient aClient );


    /**
     * From MCnUiClientStatusHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void NotifyClientLoginStatusChangesL( MCnUiClientStatusObserver* aObserver,
                                          TIMPSConnectionClient aClientToNotify );

    /**
     * From MCnUiClientStatusHandler..
     * See it for details.
     *
     * @since 2.1
     */
    void CancelClientStatusNotify();



public: // from MCnUiGroupChannelListener

    void HandleChannelMsg( TInt aGroupId,
                           TGCChannelID aChannelId,
                           TInt aRetMsg );



private: // data

    // Owns: status channel
    CCnUiGroupChannel*            iLoginStastusChannel;

    ///<Observed client, owned
    TIMPSConnectionClient         iObservedClient;

    ///<Client status observer, not owned
    MCnUiClientStatusObserver*    iClntStatusObserver;
    };

#endif      //  __CCNUICLIENTSTATUSHANDLER_H

//  End of File

