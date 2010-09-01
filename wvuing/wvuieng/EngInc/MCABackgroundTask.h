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
* Description:  Callback for CIdle based background task
*
*/


#ifndef MCABACKGROUNDTASK_H
#define MCABACKGROUNDTASK_H

// INCLUDES
#include "PublicEngineDefinitions.h"
#include <e32std.h>

// CLASS DECLARATION

/**
*	Callback for CIdle based background task
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCABackgroundTask
    {
    public:

        /**
        * Its called by the CIdle
        * @param aSubTask is the number of subtask, its counting down
        */
        virtual void HandleBackgroundTaskL( TInt aSubTask ) = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCABackgroundTask() {};
    };

#endif      // MCABACKGROUNDTASK_H

// End of File
