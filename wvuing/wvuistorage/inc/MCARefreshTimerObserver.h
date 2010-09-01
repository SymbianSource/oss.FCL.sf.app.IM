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
* Description:  This interface is used to observer refresh timer lauch
*
*/


#ifndef __MCAREFRESHTIMEROBSERVER_H
#define __MCAREFRESHTIMEROBSERVER_H

#include <e32std.h>

class MCARefreshTimerObserver
    {
    public:

        /**
        * This call is made when the waiting completes
        * @param aError error code for waiting error, KErrNone if completes without error
        *
        */
        virtual void HandleTimeWaited( const TInt aError ) = 0;

        /**
        * Virtual destructor
        */
        virtual ~MCARefreshTimerObserver() {};
    };

#endif // __MCAREFRESHTIMEROBSERVER_H
