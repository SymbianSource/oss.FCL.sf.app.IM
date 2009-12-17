/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface to observe events in text control

*
*/




#ifndef MCATAPEVENTOBSERVER_H

#define MCATAPEVENTOBSERVER_H



#include <e32def.h>



/**

 *  Interface to observe events in touchable controls

 *  @lib chat.exe

 *  @since S60 v3.2

 */

class MCATapEventObserver

    {

    public:



        /**  Tap event types */

        enum TTapEvent

            {

            ESingleTap = 0,  // This pure single tap on one item.

            // User needs to start and release the

            // tap on same item

            EDoubleTap,      // Double tap on single item.

            // There's no time limit between the taps

            EStartTap,       // When user puts stylus on control

            EReleaseTap,     // When user raises the stylus off the control



            // NOTE: When DoubleTap event happens, the observer

            // is also notified about SingleTap and Release.

            };



        /**

         * This method is called when observed object is tapped

         * @since S60 v3.2

         * @param aEvent Event which happened in touchable control

         * @param aControlId in which control the event occured

         * @return none

         */

        virtual void HandleTapEventL( TTapEvent aEvent, TUint aControlId ) = 0;



    protected:



        ~MCATapEventObserver() {}

    };





#endif // MCATAPEVENTOBSERVER_H

