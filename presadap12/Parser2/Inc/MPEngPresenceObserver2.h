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
* Description:  Observer interface to receive presence change notifications.
*
*/

#ifndef __MPENGPRESENCEOBSERVER2_H
#define __MPENGPRESENCEOBSERVER2_H

//  INCLUDES
#include <E32Std.h>


//FORWARD DECLARATIONS
class CPEngPresenceNotifier2;
class CPEngTrackedPresenceIDs2;



// CLASS DECLARATION

/**
 * Observer interface to receive presence change notifications.
 * Observer is registered to CPEngPresenceNotifier2.
 *
 * With this API clients can
 * 1. Receive notifications from changes in cached
 *    presence attributes. Cached presence attributes
 *    are subscribed or fetched presence attributes
 *    stored to presence cache.
 *
 * 2. Receive notifications from user own attribute changes.
 *    Notification from user own attributes is requested with
 *    zero length PresenceID and they are also delivered with
 *    zero length PresenceID.
 *
 * @since 3.0
 */
class MPEngPresenceObserver2
    {

    public: //Observer methods

        /**
         * Presence change handler.
         *
         * Called by CPEngPresenceNotifier2 to handle presence
         * changed events. If this method leaves, error code is
         * reported back to HandlePresenceError().
         *
         * @since 3.0
         * @param aNotifier The notifier source where notification
         *        is coming from. No ownership transferred.
         * @param aChangedPresenceIDs The list of tracked Presence IDs.
         *        List contains both all tracked Presence IDs and changed
         *        Presence IDs. See CPEngTrackedPresenceIDs2 how to iterate
         *        through wanted Presence IDs. No ownership transferred.
         */
        virtual void HandlePresenceChangeL(
            CPEngPresenceNotifier2& aNotifier,
            CPEngTrackedPresenceIDs2& aChangedPresenceIDs ) = 0;


        /**
         * Notification failure handler.
         *
         * @since 3.0
         *
         * @param aError The error resulting from Presence Engine internal
         *        presence change event handling (e.g. out of memory) or
         *         leave error from HandlePresenceChangeL().
         * @param aNotifier The notifier of which event handling failed.
         *        No ownership transferred.
         */
        virtual void HandlePresenceError(
            TInt aError,
            CPEngPresenceNotifier2& aNotifier ) = 0;



    private: //Extensions

        /**
         * Reserved virtual table slots for future use.
         * Do not implement this.
         *
         * @since 3.0
         */
        virtual TInt PresenceObserverReserved() {
            return KErrNotSupported;
            }



    protected:  //Destructor

        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngPresenceObserver2() { }


    };

#endif //__MPENGPRESENCEOBSERVER2_H


// End of File


