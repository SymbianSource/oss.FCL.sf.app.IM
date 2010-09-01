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
* Description:  Event notifier implementation base
*
*/

#ifndef __CIMPSCONNUIPRESEVENTNOTIFIERIMPBASE_H
#define __CIMPSCONNUIPRESEVENTNOTIFIERIMPBASE_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include <PEngPresenceEngineConsts2.h>
#include "RGenericObserverArray.h"


// FORWARD DECLARATIONS
class MIMPSConnUiPresenceEventObserver;
class CIMPSConnUiPresEventNotifier;
class CIMPSConnUiPresEventNotifierImpBase;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * CIMPSConnUiPresEventNotifierImpBase.
 * Implements the event notifier
 * base features.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CIMPSConnUiPresEventNotifierImpBase ) :
        public CBase,
        public MGenObserverNotifyMediator < MIMPSConnUiPresenceEventObserver,
        TIMPSPresenceServiceEvent >
    {
public: //destructor.

    /**
     * Destructor.
     * Concrete implementation object can be
     * destroyed using base class.
     */
    ~CIMPSConnUiPresEventNotifierImpBase();

protected: //constructor.

    /**
     * C++ constructor.
     * Takes a reference to implemented interface.
     * This interface is givent to notified observers.
     *
     * @param aInterface The implemented interface.
     * No ownership taken.
     */
    CIMPSConnUiPresEventNotifierImpBase( CIMPSConnUiPresEventNotifier& aInterface );


public:  //Implementation methods for public APIs

    /**
     * Implements notify start.
     *
     * Implements notify start by making sanity checks
     * for internal state and calling the virtual template
     * method DoStartL() to issue real notification.
     *
     * StartL() traps internally the DoStartL() call and
     * if the DoStartL() leaves, the partly constructed
     * notification service is stopped by calling DoStop().
     *
     * Starting fails if:
     * - already started
     * - no observers registered
     * @since 2.1
     */
    void StartL();


    /**
     * Implements notify stop.
     *
     * If notifier isn't running, does nothing.
     * If notifier is running, calls the virtual
     * template method DoStop() to stop the
     * real notify service.
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
    void AddObserverL( MIMPSConnUiPresenceEventObserver* aObserver );


    /**
     * Implements remove observer.
     *
     * @since 2.1
     * @param aObserver The observer to remove.
     * @return KErrNone if observer was found, else KErrNotFound.
     */
    TInt RemoveObserver( MIMPSConnUiPresenceEventObserver* aObserver );


    /**
     * Virtual template method to implement service status get.
     *
     * @since 2.1
     * @return Current service status as application level enumeration.
     */
    virtual const CPEngNWSessionSlotID2* AppServiceStatusL(
        TIMPSConnectionClient aClient ) = 0;



protected:  // Template methods for derived implementations

    /**
     * Virtual template method to implement
     * real notify start.
     *
     * This method implementation must
     * requested notification from
     * all qiven PEC presence events.
     *
     * @see CIMPSConnUiPresEventNotifierImp
     * @see CIMPSConnUiClntFilteredEventNotifierImp
     * @since 2.1
     *
     * @param aEvents The events where from to
     * get notifications
     */
    virtual void DoStartL( const TArray<TPEngNWSessionSlotEvent>& aEvents ) = 0;


    /**
     * Virtual template method to implement
     * real notify stop.
     *
     * This method implementation must
     * cancel any previous notification request.
     *
     * @see CIMPSConnUiPresEventNotifierImp
     * @see CIMPSConnUiClntFilteredEventNotifierImp
     * @since 2.1
     */
    virtual void DoStop() = 0;



protected:  // Helper methods for derived implementations

    /**
     * This must be called in derived notifier
     * implementations at the beginning of their
     * destructor.
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
    void NotifyObserversFromEvent( const CPEngNWSessionSlotID2& aSessionSlotID,
                                   TIMPSConnectionClient aClient,
                                   TIMPSPresenceServiceEvent aEvent );


    /**
     * Notifies registered observers from given error.
     *
     * If notifier isn't running, ignores given error.
     *
     * Before notifying the error,
     * the notifier.is stopped. (Client can restart
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
     * @param aNotifyData The structure holding notify data.
     */
    void MediateNotifyL( MIMPSConnUiPresenceEventObserver& aObserverToNotify,
                         TIMPSPresenceServiceEvent& aNotifyData );


    /**
     * Observer notification from error.
     *
     * @since 2.1
     * @param aObserverToNotify The notified observer which leaved.
     * @param aLeaveError The propagated leave code.
     */
    void MediateNotifyError( MIMPSConnUiPresenceEventObserver& aObserverToNotify,
                             TInt aLeaveError );

    /**
     * Observer notification from error.
     *
     * @since 2.1
     * @param aObserverToNotify The notified observer which leaved.
     * @param aLeaveError The propagated leave code.
     */
    void MediateError( MIMPSConnUiPresenceEventObserver& aObserverToNotify,
                       TInt aError );

private: //Data

    ///< Reference to owning interface, interface not owned
    CIMPSConnUiPresEventNotifier&                      iInterface;

    ///< Observer array, array owned, observers not
    RGenericObserverArray < MIMPSConnUiPresenceEventObserver,
    TIMPSPresenceServiceEvent > iObserverArray;


    CPEngNWSessionSlotID2* iSessionSlotID;
    TIMPSConnectionClient  iClient;
    ///< Flags describing the notifier state, owned
    ///< Flag contents are either EFalse or ETrue
    TInt8   iStarted;
    TInt8   iDying;
    TInt8   iReserved1;
    TInt8   iReserved2;

    };

#endif      //__CIMPSCONNUIPRESEVENTNOTIFIERIMPBASE_H
//  End of File


