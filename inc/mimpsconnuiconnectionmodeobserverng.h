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
* Description:  Connection mode observer.
*
*/

#ifndef __MIMPSCONNUICONNECTIONMODEOBSERVERNG_H
#define __MIMPSCONNUICONNECTIONMODEOBSERVERNG_H

#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATION
class CIMPSConnUiConnectionModeNotifier;


/**
 * Observer interface to get notifcations from
 * connection mode events.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MIMPSConnUiConnectionModeObserver )
    {
public: //notify methods

    /**
     * Notifies from connection mode event.
     *
     * If event handling causes a leave, leave is
     * reported back to HandleConnectionModeEventNotifyError()
     * callback.
     *
     * @since 2.1
     * @param aNotifier The connection mode notifier where this
     * event is coming from.
     * @param aEvent The connection mode event.
     */
    virtual void HandleConnectionModeEventL( CIMPSConnUiConnectionModeNotifier* aNotifier,
                                             TIMPSConnectionModeEvent aEvent ) = 0;


    /**
     * Called when connection mode event listening or event
     * handling fails.
     *
     * Here is reported both errors from handling the
     * actual connection mode event and also asynchronously
     * the errors coming from starting (requesting) the
     * connection mode notify.
     *
     * When reporting the error, the notifier is
     * automaticly stopped. However, the observer may
     * restart the notifier during the notify callback
     * if desired so.
     *
     * @since 2.1
     * @param aNotifier The presence notifier where this
     * error is coming from.
     * @param aError is the error code.
     */
    virtual void HandleConnectionModeEventNotifyError(
        CIMPSConnUiConnectionModeNotifier* aNotifier,
        TInt aError ) = 0;
    };


#endif //__MIMPSCONNUICONNECTIONMODEOBSERVERNG_H
