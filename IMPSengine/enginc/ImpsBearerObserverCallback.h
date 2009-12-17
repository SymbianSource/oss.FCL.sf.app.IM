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
* Description: A connection manager object uses this callback function to indicate
* that a change in the status of the bearer has occurred.
*
*/


#ifndef __IMPSBEAREROBSERVERCALLBACK_H__
#define __IMPSBEAREROBSERVERCALLBACK_H__

#include <e32base.h>

//This must be kept as low as possible
const TInt KMaxNumberOfSubscribers         = 3;

//Possible events
enum TImpsBearerEvent
    {
    EImpsBearerActive = 0,
    EImpsBearerSuspended,
    EImpsBearerLost
    };

// CLASS DECLARATION

class MImpsBearerObserverCallback
    {
    public:

        /**
        * A connection manager object uses this callback function to indicate
        * that a change in the status of the bearer has occurred. It is important
        * to note that a call to the notification handler (HandleBearerEventL())
        * takes place inside the RunL() method of a Connection Manager, so the
        * listening object MUST return the control to the Manager AS SOON AS
        * POSSIBLE in order not to clog the scheduler.
        *
        * @param TImpsBearerEvent The event that took place
        * @param TBool In case of EImpsBearerLost, indicates whether connection
        *              was authoritatively closed (i.e. by the user)
        */
        virtual void HandleBearerEventL( const TImpsBearerEvent aBearerEvent, TBool aIsAuthClose ) = 0;
    };

#endif    // __IMPSBEAREROBSERVERCALLBACK_H__

// End of File
