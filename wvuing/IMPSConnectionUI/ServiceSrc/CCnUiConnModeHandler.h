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
* Description:  Connection mode handler implementation.
*
*/

#ifndef __CCNUICONNMODEHANDLER_H
#define __CCNUICONNMODEHANDLER_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include "CCnUiGroupChannel.h"
#include "MCnUiConnModeHandler.h"


//FORWARD DECLARATION
class MCnUiConnModeObserver;



// CLASS DECLARATION
/**
 * Connection mode handler implementation.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiConnModeHandler ) : public CBase,
        public MCnUiGroupChannelListener,
        public MCnUiConnModeHandler

    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiConnModeHandler* NewL();


    /**
     * Destructor.
     */
    virtual ~CCnUiConnModeHandler();

private:

    /**
     * C++ constructor.
     */
    CCnUiConnModeHandler();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // New functions from MCnUiConnModeHandler

    /**
     * From MCnUiConnModeHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void SendUserSelectionL( TIMPSUserConnectionSelection aUserSelection,
                             TIMPSConnectionClient aClient );

    /**
     * From MCnUiConnModeHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TInt RegisterAsSignificantSchedulingClientL( TIMPSConnectionClient aClient );


    /**
     * From MCnUiConnModeHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void CancelRegisteringAsSSClient();


    /**
     * From MCnUiConnModeHandler.
     * See it for details.
     *
     * @since 2.1
     */
    TIMPSConnectionModeEvent ConnectionModeL( TIMPSConnectionClient aClient,
                                              TIMPSConnectionModeEvent aModeEventCateqory );

    /**
     * From MCnUiConnModeHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void NotifyConnectionModeChangesL( MCnUiConnModeObserver* aObserver,
                                       TIMPSConnectionClient aClientToNotify );

    /**
     * From MCnUiConnModeHandler.
     * See it for details.
     *
     * @since 2.1
     */
    void CancelConnectionModeNotify();



public: // from MCnUiGroupChannelListener

    /**
     * Channel listener.
     * @since 2.1
     */
    void HandleChannelMsg( TInt aGroupId,
                           TGCChannelID aChannelId,
                           TInt aChannelMsg );


private: // Private helpers


    /**
     * Helper to start the notifying.
     *
     * @param aClientToNotify The client for which to start the notify.
     */
    void DoStartNotifyL( TIMPSConnectionClient aClientToNotify );


    /**
     * Event category checkers.
     *
     * @param aEvent The event to check.
     * @return ETrue if given event value belongs to
     * category. Else EFalse.
     */
    TBool LoginLogoutModeCategoryEvent( TInt aEvent );
    TBool SSClientStartStopCategoryEvent( TInt aEvent );



private: // data

    ///<Listeners to listen channel activity, owned, lazy initialized
    CCnUiGroupChannel*            iLoginLogoutEventChannelListener;
    CCnUiGroupChannel*            iSSReqistrationChannelListener;

    ///<SS registration channel, owned
    CCnUiGroupChannel*            iSSReqistration;

    //Cached SS registration client count, owned.
    TInt                           iSSClientCountPrev;

    ///<Connection mode observer, not owned
    MCnUiConnModeObserver*        iConnModeObserver;

    ///<Observed client type, owned
    TIMPSConnectionClient         iObserverClient;

    };

#endif      //  __CCNUICONNMODEHANDLER_H

//  End of File

