/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Callback for background task, implemented in CCARequestMapper
*
*/


#ifndef MCABACKGROUNDCALLBACK_H
#define MCABACKGROUNDCALLBACK_H

// INCLUDES
#include "MCABackgroundInterface.h"
#include <e32std.h>

// CLASS DECLARATION

/**
*  Callback for background task status change notification.
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCABackgroundCallback
    {

    public:

        /**
        * Call if the given background task is activated by its CIdle
        * @param aTask is the identifier of the background task
        * @return ETrue if it has more work/subtask, EFalse otherwise
        */
        virtual TInt HandleBackgroundTask( MCABackgroundInterface::TCABackgroundTasks aTask ) = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCABackgroundCallback() {};
    };

#endif      // MCABACKGROUNDCALLBACK_H

// End of File
