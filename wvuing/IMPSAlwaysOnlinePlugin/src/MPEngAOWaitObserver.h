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


#ifndef __MPENGAOWAITOBSERVER_H
#define __MPENGAOWAITOBSERVER_H

#include <e32std.h>

class MPEngAOWaitObserver
    {
    public:

        /**
        * This call is made when the waiting completes
        */
        virtual void HandleTimeWaited() = 0;

        /**
        * This call is made when the waiting completes with an error
        * @param aError error code for waiting error
        */
        virtual void HandleTimeWaitedError( TInt aError ) = 0;

        /**
        * Virtual destructor
        */
        virtual ~MPEngAOWaitObserver() {};
    };

#endif // __MPENGAOWAITOBSERVER_H
