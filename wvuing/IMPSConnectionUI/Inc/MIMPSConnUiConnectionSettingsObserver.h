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
* Description:  Connection settings observer.
*
*/

#ifndef __MIMPSCONNUICONNECTIONSETTINGSOBSERVER_H
#define __MIMPSCONNUICONNECTIONSETTINGSOBSERVER_H

#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATION
class CIMPSConnUiConnectionSettingsNotifier;


/**
 * Observer interface to get notifcations from
 * connection mode events.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MIMPSConnUiConnectionSettingsObserver )
    {
public: //notify methods

    /**
     * Notifies from connection settings event.
     *
     * If event handling causes a leave, leave is
     * reported back to HandleConnectionSettingsEventNotifyError()
     * callback.
     *
     * @since 2.1
     * @param aNotifier The connection settings notifier
     * where this event is coming from.
     * @param aEvent The connection settings event.
     */
    virtual void HandleConnectionSettingsEventL(
        CIMPSConnUiConnectionSettingsNotifier* aNotifier,
        TIMPSConnectionSettingsEvent aEvent ) = 0;


    /**
     * Called when connection mode event listening or event
     * handling fails.
     *
     * Here is reported both errors from handling the
     * actual connection settings event and also asynchronously
     * the errors coming from starting (requesting) the
     * connection settings notify.
     *
     * When reporting the error, the notifier is
     * automaticly stopped. However, the observer may
     * restart the notifier during the notify callback
     * if desired so.
     *
     * @since 2.1
     * @param aNotifier The connection settings notifier
     * where this error is coming from.
     * @param aError is the error code.
     */
    virtual void HandleConnectionSettingsEventNotifyError(
        CIMPSConnUiConnectionSettingsNotifier* aNotifier,
        TInt aError ) = 0;
    };


#endif //__MIMPSCONNUICONNECTIONSETTINGSOBSERVER_H
