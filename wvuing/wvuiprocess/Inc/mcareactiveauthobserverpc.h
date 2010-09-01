/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for reactive authorization observer
*
*/


#ifndef MCAREACTIVEAUTHOBSERVERPC_H
#define MCAREACTIVEAUTHOBSERVERPC_H

//INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
 *  Reactive Authorization observer interface to the UI.
 *  Signalling goes from the Engine -> UI.
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAReactiveAuthObserverPC
    {
    public:

        /**
         * Incoming pending reactive authorization request.
         */
        virtual void HandleRAPendingReqL() = 0;

        /**
         * Incoming reactive authorization status message
         */
        virtual void HandleRAStatusL() = 0;

        /* Error handler, if any of the observer methods leave.
         * @param aError The error code
         */
        virtual void HandleRAError( TInt aError ) = 0;

    protected:
        /**
         * Destructor.
         */
        virtual ~MCAReactiveAuthObserverPC() {};
    };

#endif      // MCAREACTIVEAUTHOBSERVERPC_H

// End of File
