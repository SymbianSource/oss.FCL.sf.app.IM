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
* Description:  Connection settings notifier implementation.
*
*/

#ifndef __CIMPSCONNUICONNECTIONSETTINGSNOTIFIERIMP_H
#define __CIMPSCONNUICONNECTIONSETTINGSNOTIFIERIMP_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include "RGenericObserverArray.h"


// FORWARD DECLARATIONS
class MIMPSConnUiConnectionSettingsObserver;
class CIMPSConnUiConnectionSettingsNotifier;
class CIMPSConnUiConnectionSettingsNotifierImp;


// CLASS DECLARATION
/**
 * CIMPSConnUiConnectionSettingsNotifierImp.
 * Implements the event notifier features.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CIMPSConnUiConnectionSettingsNotifierImp ) :
        public CBase,
        public MGenObserverNotifyMediator < MIMPSConnUiConnectionSettingsObserver,
        TIMPSConnectionSettingsEvent >
    {
public:  // Two-phased constructor and destructor

    /**
     * Two-phased constructor.
     * Constructs client filtered version of
     * event notifier implementation.
     */
    static CIMPSConnUiConnectionSettingsNotifierImp* NewL(
        CIMPSConnUiConnectionSettingsNotifier& aInterface,
        TIMPSConnectionClient aClient );


    /**
     * Destructor.
     */
    ~CIMPSConnUiConnectionSettingsNotifierImp();


private:

    /**
     * C++ constructor.
     */
    CIMPSConnUiConnectionSettingsNotifierImp(
        CIMPSConnUiConnectionSettingsNotifier& aInterface,
        TIMPSConnectionClient aClient );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public:  //Implementation methods for public APIs

    /**
     * Implements notify start.
     *
     * @since 2.1
     */
    void StartL();


    /**
     * Implements notify stop.
     *
     * @since 2.1
     */
    void Stop();


    /**
     * Implements add observer.
     *
     * @since 2.1
     * @param aObserver The observer to add.
     */
    void AddObserverL( MIMPSConnUiConnectionSettingsObserver* aObserver );


    /**
     * Implements remove observer.
     *
     * @since 2.1
     * @param aObserver The observer to remove.
     * @return KErrNone if observer was found, else KErrNotFound.
     */
    TInt RemoveObserver( MIMPSConnUiConnectionSettingsObserver* aObserver );


    /**
     * Implements connection mode get.
     *
     * @since 2.1
     * @return Current connection mode.
     */
    TIMPSConnectionSettingsEvent ConnectionSettingByCategoryL(
        TIMPSConnectionSettingsEvent aModeEventCateqory );


public: //Observation method from MCnUiConnModeObserver


    /**
     * Observation method to get events from client
     * specific connection mode changes.
     *
     * Forwards events to registered
     * MIMPSConnUiConnectionSettingsObserver observers.
     *
     * @since 2.1
     */
    void HandleConnModeChange( TIMPSConnectionClient aRequestedClient,
                               TIMPSConnectionSettingsEvent aConnMode );


private:  // Helper methods

    /**
     * Helper method implementing
     * real notify start.
     *
     * @since 2.1
     */
    void DoStartL();


    /**
     * Helper method to implementing
     * real notify stop.
     *
     * @since 2.1
     */
    void DoStop();


    /**
     * Called in notifier destructor.
     *
     * Sets the dying flag on (prevents further starting the
     * notifier and error and event notifying) Stops the notifier
     * if running.
     *
     * @since 2.1
     */
    void Dying();


    /**
     * Notifies registered observers from given presence event.
     * If notifier isn't running,
     * ignores given presence event.
     *
     * @since 2.1
     * @param aEvent The event to notify.
     */
    void NotifyObserversFromEvent( TIMPSConnectionSettingsEvent aEvent );


    /**
     * Notifies registered observers from given error.
     *
     * If notifier isn't running, ignores given error.
     *
     * Before notifying the error,
     * the notifier is stopped. (Client can restart
     * the notifier in the error callback.)
     *
     * @since 2.1
     * @param aError The error to report.
     */
    void StopAndNotifyObserversFromError( TInt aError );


private: //Notify mediators from MGenObserverNotifyMediator

    /**
     * Implements the observer notification.
     *
     * @since 2.1
     * @param The observer to notify.
     * @param aNotifyData The notify data.
     */
    void MediateNotifyL( MIMPSConnUiConnectionSettingsObserver& aObserverToNotify,
                         TIMPSConnectionSettingsEvent& aNotifyData );


    /**
     * Observer notification from error.
     *
     * @since 2.1
     * @param aObserverToNotify The notified observer which leaved.
     * @param aLeaveError The propagated leave code.
     */
    void MediateNotifyError( MIMPSConnUiConnectionSettingsObserver& aObserverToNotify,
                             TInt aLeaveError );


    /**
     * Observer notification from error.
     *
     * @since 2.1
     * @param aObserverToNotify The observer to notify.
     * @param aError The error.
     */
    void MediateError( MIMPSConnUiConnectionSettingsObserver& aObserverToNotify,
                       TInt aError );



private: //Data

    ///< Reference to owning interface, interface not owned
    CIMPSConnUiConnectionSettingsNotifier&  iInterface;

    /// Client for which to deliver mode events, owned
    const TIMPSConnectionClient             iClient;

    ///< Observer array, array owned, observers not
    RGenericObserverArray < MIMPSConnUiConnectionSettingsObserver,
    TIMPSConnectionSettingsEvent > iObserverArray;

    ///< Flags describing the notifier state, owned
    ///< Flag contents are either EFalse or ETrue
    TInt8   iStarted;
    TInt8   iDying;
    TInt8   iReserved1;
    TInt8   iReserved2;
    };

#endif      //__CIMPSCONNUICONNECTIONSETTINGSNOTIFIERIMP_H
//  End of File


