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
* Description: this is an observer class handling session status change
*
*/

#ifndef __MPENGSESSIONSTATUSOBSERVER_H
#define __MPENGSESSIONSTATUSOBSERVER_H


//  INCLUDES
#include    <E32Std.h>
#include    "PEngPresenceEngineConsts2.h"

// DATA TYPES



// FORWARD DECLARATIONS


// CLASS DECLARATION
/**
 *  Access handler of one network session
 *
 *  It is used to open/close network session
 *  and register its observers
 *
 *  @since 3.0
 */
class MPEngSessionStatusObserver
    {

    public: //

        /**
         *  Handle Session status change
         *
         *  @since 3.0
         *
         *  @param new session slot state
         *  @param new session slot event
         */
        virtual void StatusChangedL( TPEngNWSessionSlotState aNewState,
                                     TPEngNWSessionSlotEvent aNewEvent ) = 0;


    protected:  //Destructor

        /**
         *  Virtual inline destructor.
         *  Protected destructor to prohibits deletion trough interface.
         */
        virtual ~MPEngSessionStatusObserver() {};
    };


#endif      //  __MPEngSessionStatusObserver_H

//  End of File



