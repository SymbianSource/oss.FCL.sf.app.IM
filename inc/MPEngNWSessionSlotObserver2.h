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
* Description:  Observer interface to receive NWSessionSlot change notifications.
*
*/

#ifndef __MPENGNWSESSIONSLOTOBSERVER2_H
#define __MPENGNWSESSIONSLOTOBSERVER2_H

//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS
class CPEngNWSessionSlotNotifier2;
class CPEngNWSessionSlotEvent2;


// CLASS DECLARATION


/**
 * Observer interface to get NWSessionSlot change notifications.
 * Observer is registered to CPEngNWSessionSlotNotifier2.
 *
 * @since 3.0
 */
class MPEngNWSessionSlotObserver2
    {
    public: //Observing methods


        /**
         * NWSessionSlot change handler.
         *
         * Called by CPEngNWSessionSlotNotifier2 to report NWSessionSlot
         * changed events. If this method leaves, error code is
         * reported back to HandleNWSessionSlotError().
         *
         * @since 3.0
         * @param aNotifier The notifier source where notification
         *        is coming from. No ownership transferred.
         * @param aEvent The event container.  No ownership transferred.
         */
        virtual void HandleNWSessionSlotChangeL(
            CPEngNWSessionSlotNotifier2& aNotifier,
            CPEngNWSessionSlotEvent2& aEvent ) = 0;


        /**
         * Notification failure handler.
         *
         * @since 3.0
         * @param aError The error resulting from Presence Engine internal
         *        change event handling (e.g. out of memory) or
         *        leave error from HandleNWSessionSlotChangeL().
         * @param aNotifier The notifier of which event handling failed.
         *        No ownership transferred.
         */
        virtual void HandleNWSessionSlotError(
            TInt aError,
            CPEngNWSessionSlotNotifier2& aNotifier ) = 0;


    private: //Extensions

        /**
         * Reserved virtual table slots for future use.
         * Do not implement this.
         *
         * @since 3.0
         */
        virtual TInt NWSessionSlotObserverReserved() {
            return KErrNotSupported;
            }



    protected:  //Destructor
        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngNWSessionSlotObserver2() { }

    };


#endif //__MPENGNWSESSIONSLOTOBSERVER2_H


//End of file


