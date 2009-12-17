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
* Description:  Contact data container observer interface
*
*/



#ifndef MCASYNCOBSERVERPC_H
#define MCASYNCOBSERVERPC_H

#include <e32std.h>

// CLASS DECLARATION

/**
 *  Interface for observer
 *
 *  @lib wvuiprocess.dll
 *  @since 3.0
 */

class MCASyncObserverPC
    {
    public: // New functions

        virtual void NotifySyncCompleted() = 0;
        /**
         * Virtual destructor
         */
        virtual ~MCASyncObserverPC() {};
    };

#endif      // MCASYNCOBSERVERPC_H

// End of File
