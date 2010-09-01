/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer interface to get notifications from presence events.
*
*/

#ifndef __MIMPSCONNUIPRESENCEEVENTOBSERVERNG_H
#define __MIMPSCONNUIPRESENCEEVENTOBSERVERNG_H

#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATION
class CIMPSConnUiPresEventNotifier;
class CPEngNWSessionSlotID2;

/**
 * Observer interface to get notifcations from
 * presence events.
 *
 * This interface can be reqisterd both standard
 * event notifier (CIMPSConnUiPresEventNotifier)
 * and the client filtered event notifier
 * (CIMPSConnUiClntFilteredPresEventNotifier).
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MIMPSConnUiPresenceEventObserver )
    {
public: //notify methods

    /**
     * Notifies from presence event.
     *
     * If event handling causes a leave, leave is
     * reported back to HandlePresenceEventNotifyError()
     * callback.
     *
     * @since 2.1
     * @param aNotifier The presence notifier where this
     * event is coming from.
     * @param aEvent The presence event.
     */
    virtual void HandlePresenceEventL( CIMPSConnUiPresEventNotifier* aNotifier,
                                       const CPEngNWSessionSlotID2& aSessionSlotID,
                                       TIMPSConnectionClient aClient,
                                       TIMPSPresenceServiceEvent aEvent ) = 0;


    /**
     * Called when presence event listening or event
     * handling fails.
     *
     * Here is reported both errors from handling the
     * actual event and also asynchronously the errors
     * coming from starting the presence notify.
     *
     * When reporting the error, the notifier is stopped.
     * However, the observer may restart the notifier
     * during the notify callback if desired so.
     *
     * @since 2.1
     * @param aNotifier The presence notifier where this
     * error is coming from.
     * @param aError is the error code.
     */
    virtual void HandlePresenceEventNotifyError( CIMPSConnUiPresEventNotifier* aNotifier,
                                                 TInt aError ) = 0;
    };


#endif //__MIMPSCONNUIPRESENCEEVENTOBSERVERNG_H
