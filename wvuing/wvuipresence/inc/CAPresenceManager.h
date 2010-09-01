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
* Description:  Factory for presence manager
*
*/



#ifndef CAPRESENCEMANAGER_H
#define CAPRESENCEMANAGER_H

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MCAPresence;
class MCASettings;

// CLASS DECLARATION

/**
*  Factory for presence manager.
*
*  @lib CAPresence.dll
*  @since 2.1
*/
class CAPresenceManager // CSI: 51 # This is not a C-class
            // although it begins with C.
    {
    public: // New functions

        /**
        * Creates instance of presence manager
        * @since 2.1
        * @param aSettingsAPI Pointer to settings API. First call of InstanceL-
        * method must have valid settings API pointer, otherwise InstanceL will
        * leave. Next calls can be made without aSettingsAPI parameter.
        * @return MCAPresence pointer
        */
        IMPORT_C static MCAPresence* InstanceL( MCASettings* aSettingsAPI = NULL );

        /**
         * Release the instance from TLS
         * @since 2.1
         */
        IMPORT_C static void Release();
    };

#endif      // CAPRESENCEMANAGER_H

// End of File
