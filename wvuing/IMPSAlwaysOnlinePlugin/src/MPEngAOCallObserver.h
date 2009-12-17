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
* Description:  This interface is used to inform about the change in network
*				 registration status.
*
*/


#ifndef __MPENGAOCALLOBSERVER_H
#define __MPENGAOCALLOBSERVER_H

#include <e32std.h>

class MPEngAOCallObserver
    {
    public:

        /**
        * This call is made when the waiting completes
        * @param TBool Whether the call ended already or not.
        */
        virtual void HandleCallEndedL( TBool aEnded ) = 0;

        /**
        * Virtual destructor
        */
        virtual ~MPEngAOCallObserver() {};
    };

#endif // __MPENGAOCALLOBSERVER_H
