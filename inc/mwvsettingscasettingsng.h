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
* Description:  Service Settings UI observer
*
*/


#ifndef __MWVSETTINGSCASETTINGSNG_H
#define __MWVSETTINGSCASETTINGSNG_H

#include <e32base.h>


// FORWARD DECLARATIONS
class CIMPSSAPSettings;

// CLASS DECLARATION

/**
*  Interface for the CA settings launching
*
*  @lib WVServiceSettingsUi.dll
*  @since 3.1u
*/
class MWVSettingsCASettings
    {

    public:

        /**
        * This method is called when CA settings should be launched
        * @since 3.1u
        * @param aServer the server whose settings should be modified
        */
        virtual TInt LaunchSettingsL ( CIMPSSAPSettings& aServer ) = 0;


    protected:

        /**
        * Destructor
        */
        virtual ~MWVSettingsCASettings() {};

    };

#endif      //  __MWVSETTINGSCASETTINGSNG_H
