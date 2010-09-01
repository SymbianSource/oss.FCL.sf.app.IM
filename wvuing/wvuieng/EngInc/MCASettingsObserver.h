/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer interface for settings changes
*
*/

#ifndef MCASETTINGSOBSERVER_H
#define MCASETTINGSOBSERVER_H

// INCLUDES

#include <e32std.h>

// CLASS DECLARATION

/**
*  Interface for internal settings observer.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCASettingsObserver
    {
    public:

        /**
        * Handles setting changes.
        * @param aChangedSettingEnum specifies the changed setting.
        */
        virtual void HandleSettingsChangeL(
            TInt aChangedSettingEnum ) = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCASettingsObserver() {};
    };

#endif      // MCASETTINGSOBSERVER_H

// End of File
