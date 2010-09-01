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
* Description:  Observer interface to receive presence contact list notifications.
*
*/

#ifndef __MPENGCONTACTLISTOBSERVER2_H
#define __MPENGCONTACTLISTOBSERVER2_H

//  INCLUDES
#include <E32Std.h>


//FORWARD DECLARATIONS
class CPEngContactListNotifier2;



// CLASS DECLARATION

/**
 * Observer interface to receive contact list change notifications.
 * Observer is registered to CPEngContactListNotifier2.
 *
 * @lib
 * @since 3.0
 */
class MPEngContactListObserver2
    {
    public: //Observer methods

        /**
         * Contact list change handler.
         *
         * Called by CPEngContactListNotifier2 to handle contact list
         * changed events. If this method leaves, error code is
         * reported back to HandleContactListError().
         *
         * @since 3.0
         * @param aNotifier The notifier source where notification
         *        is coming from. No ownership transferred.
         * @param aContactListName Contact list which has changed.
         */
        virtual void HandleContactListChangeL(
            CPEngContactListNotifier2& aNotifier,
            const TDesC& aContactListName ) = 0;


        /**
         * Notification failure handler.
         *
         * @since 3.0
         *
         * @param aError The error from Presence Engine internal
         *        presence change event handling (e.g. out of memory) or
         *        leave error from HandleContactListChangeL().
         * @param aNotifier The notifier of which event handling failed.
         *        No ownership transferred.
         */
        virtual void HandleContactListError(
            TInt aError,
            CPEngContactListNotifier2& aNotifier ) = 0;



    private: //Extensions

        /**
         * Reserved virtual table slots for future use.
         * Do not implement this.
         *
         * @since 3.0
         */
        virtual TInt ContactListObserverReserved() {
            return KErrNotSupported;
            }



    protected:  //Destructor

        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngContactListObserver2() { }


    };



#endif  //__MPENGCONTACTLISTOBSERVER2_H


//End of file



