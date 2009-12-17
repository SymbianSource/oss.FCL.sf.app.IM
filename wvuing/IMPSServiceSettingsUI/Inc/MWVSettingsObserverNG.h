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


#ifndef __MWVSETTINGSOBSERVERNG_H
#define __MWVSETTINGSOBSERVERNG_H

#include <e32base.h>

// CLASS DECLARATION

/**
*  Interface for the clients of service settings UI
*  for observing server changes
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
class MWVSettingsObserver
    {

    public:



        /**
        * This method is called when the current server changes
        * @since 2.1
        * @param aNewServerName the name of the new server
        */
        virtual void ServerChangedL ( const TDesC& aNewServerName, TBool aDefaultServerChanged = EFalse ) = 0;

        /**
         * @since 3.0
         * @return the currently used server name (ownership is transferred)
         */
        virtual HBufC* CurrentServerNameLC() = 0;

    protected:

        /**
        * Destructor
        */
        virtual ~MWVSettingsObserver() {};

    };

#endif      //  __MWVSETTINGSOBSERVERNG_H
