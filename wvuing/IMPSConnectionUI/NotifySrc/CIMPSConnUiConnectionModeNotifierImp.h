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
* Description:  Connection mode notifier implementation.
*
*/

#ifndef __CIMPSCONNUICONNECTIONMODENOTIFIERIMP_H
#define __CIMPSCONNUICONNECTIONMODENOTIFIERIMP_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include <PEngPresenceEngineConsts2.h>
#include "MCnUiConnModeHandler.h"
#include "RGenericObserverArray.h"


// FORWARD DECLARATIONS
class MIMPSConnUiConnectionModeObserver;
class CIMPSConnUiConnectionModeNotifier;
class CIMPSConnUiConnectionModeNotifierImp;


// CLASS DECLARATION
/**
 * CIMPSConnUiConnectionModeNotifierImp.
 * Implements the event notifier features.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CIMPSConnUiConnectionModeNotifierImp ) :
        public CBase,
        public MCnUiConnModeObserver,
        public MGenObserverNotifyMediator < MIMPSConnUiConnectionModeObserver,
        TIMPSConnectionModeEvent >
    {
public:  // Two-phased constructor and destructor

    /**
     * Two-phased constructor.
     * Constructs client filtered version of
     * event notifier implementation.
     */
    static CIMPSConnUiConnectionModeNotifierImp* NewL(
        CIMPSConnUiConnectionModeNotifier& aInterface,
        TIMPSConnectionClient aClient );


    /**
     * Destructor.
     */
    ~CIMPSConnUiConnectionModeNotifierImp();


private:

    /**
     * C++ constructor.
     */
    CIMPSConnUiConnectionModeNotifierImp( CIMPSConnUiConnectionModeNotifier& aInterface,
                                          TIMPSConnectionClient aClient );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public:  //Implementation methods for public APIs

    /**
     * Implements notify start.
     *
     * @since 3.0
     */
    void StartL();


    /**
     * Implements notify stop.
     *
     * @since 3.0
     */
    void Stop();


    /**
     * Implements add observer.
     *
     * @since 3.0
     * @param aObserver The observer to add.
     */
    void AddObserverL( MIMPSConnUiConnectionModeObserver* aObserver );


    /**
     * Implements remove observer.
     *
     * @since 3.0
     * @param aObserver The observer to remove.
     * @return KErrNone if observer was found, else KErrNotFound.
     */
    TInt RemoveObserver( MIMPSConnUiConnectionModeObserver* aObserver );


    /**
     * Implements connection mode get.
     *
     * @since 3.0
     * @param aModeEventCateqory event category
     * @return Current connection mode.
     */
    TIMPSConnectionModeEvent ConnectionModeByCategoryL(
        TIMPSConnectionModeEvent aModeEventCateqory );


public: //Observation method from MCnUiConnModeObserver


    /**
     * Observation method to get events from client
     * specific connection mode changes.
     *
     * Forwards events to reqistered
     * MIMPSConnUiConnectionModeObserver observers.
     *
     * @since 3.0
     */
    void HandleConnModeChange( TIMPSConnectionClient aRequestedClient,
                               TIMPSConnectionModeEvent aConnMode );


private:  // Helper methods

    /**
     * Helper method implementing
     * real notify start.
     *
     * @since 3.0
     */
    void DoStartL();


    /**
     * Helper method to implementing
     * real notify stop.
     *
     * @since 3.0
     */
    void DoStop();


    /**
     * Called in notifier destructor.
     *
     * Sets the dying flag on (prevents further starting the
     * notifier and error and event notifying) Stops the notifier
     * if running.
     *
     * @since 3.0
     */
    void Dying();


    /**
     * Notifies registered observers from given presence event.
     * If notifier isn't running,
     * ignores given presence event.
     *
     * @since 3.0
     * @param aEvent The event to notify.
     */
    void NotifyObserversFromEvent( TIMPSConnectionModeEvent aEvent );


    /**
     * Notifies registered observers from given error.
     *
     * If notifier isn't running, ignores given error.
     *
     * Before notifying the error,
     * the notifier is stopped. (Client can restart
     * the notifier in the error callback.)
     *
     * @since 3.0
     * @param aError The error to report.
     */
    void StopAndNotifyObserversFromError( TInt aError );


private: //Notify mediators from MGenObserverNotifyMediator

    /**
     * Implements the observer notification.
     *
     * @since 3.0
     * @param aObserverToNotify The observer to notify.
     * @param aNotifyData The notify data.
     */
    void MediateNotifyL( MIMPSConnUiConnectionModeObserver& aObserverToNotify,
                         TIMPSConnectionModeEvent& aNotifyData );


    /**
     * Observer notification from error.
     *
     * @since 3.0
     * @param aObserverToNotify The notified observer which leaved.
     * @param aLeaveError The propagated leave code.
     */
    void MediateNotifyError( MIMPSConnUiConnectionModeObserver& aObserverToNotify,
                             TInt aLeaveError );


    /**
     * Observer notification from error.
     *
     * @since 3.0
     * @param aObserverToNotify The observer to notify.
     * @param aError The error.
     */
    void MediateError( MIMPSConnUiConnectionModeObserver& aObserverToNotify,
                       TInt aError );



private: //Data

    ///< Reference to owning interface, interface not owned
    CIMPSConnUiConnectionModeNotifier&  iInterface;

    /// Client for which to deliver mode events, owned
    const TIMPSConnectionClient         iClient;

    /// Connection mode handler to get notifications, owned
    MCnUiConnModeHandler*               iConnModeHandler;

    ///< Observer array, array owned, observers not
    RGenericObserverArray < MIMPSConnUiConnectionModeObserver,
    TIMPSConnectionModeEvent > iObserverArray;

    ///< Flags describing the notifier state, owned
    ///< Flag contents are either EFalse or ETrue
    TInt8   iStarted;
    TInt8   iDying;
    TInt8   iReserved1;
    TInt8   iReserved2;
    };

#endif      //__CIMPSCONNUICONNECTIONMODENOTIFIERIMP_H
//  End of File


