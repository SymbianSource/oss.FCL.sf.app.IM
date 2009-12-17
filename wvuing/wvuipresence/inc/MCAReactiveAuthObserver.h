/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Internal reactive authorization observer
*
*/


#ifndef MCAREACTIVEAUTHOBSERVER_H
#define MCAREACTIVEAUTHOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CCABlockingUI;

class CPEngReactAuthNotifier;
class MPEngAuthorizationRequest;
class MPEngAuthorizationStatus;

// CLASS DECLARATION

/**
 * Reactive authorization observer
 *
 * Start some CActive from the callbacks and handle the logic in a callback, we
 * must not block the original PEC Engine calls.
 *
 * The CPEngReactAuthNotifier can be obtained via IM Presence. From there the
 * count of pending requests and status messages can be obtained. These are
 * then handled in the UI.
 *
 * The user must respond with the id given here (through MCAPresence methods).
 *
 * @lib chat.app
 * @since 3.0
 */
class MCAReactiveAuthObserver
    {
    public:  // new methods

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
    };

#endif      // MCAREACTIVEAUTHOBSERVER_H

// End of File
