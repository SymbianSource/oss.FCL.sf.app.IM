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


#ifndef __MPENGAONWSTATUSOBSERVER_H
#define __MPENGAONWSTATUSOBSERVER_H

#include <e32std.h>

class MPEngAONwStatusObserver
    {
    public:

        /**
        * Callback method to inform that the network status
        * has been changed
        * @param aNwAvailable True if network is available, False if not
        */
        virtual void HandleNwStatusChange( TBool aNwAvailable ) = 0;

        virtual ~MPEngAONwStatusObserver() {};
    };

#endif // __MPENGAONWSTATUSOBSERVER_H
