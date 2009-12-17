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
* Description:  Interface for Chat observer
*
*/


#ifndef MCAINVITATIONOBSERVERPC_H
#define MCAINVITATIONOBSERVERPC_H


//INCLUDES
#include <e32std.h>
#include "TEnumsPC.h"
// CLASS DECLARATION

/**
 *  Search observer interface to the UI.
 *  Signalling goes from the Engine -> UI.
 *
 *  @lib wvprocessng.lib
 *  @since 3.2
 */
class MCAInvitationObserverPC
    {
    public:

        /**
         * Gets called when invite event occurs.
         * @param aEvent Type of event.
         * @param aUserId userid of the sender
         * @param aMessage invite message
         */
        virtual void HandleInvitationEventL( TEnumsPC::TInviteType aInviteType, TInt aIndex ) = 0;

        /**
        * Gets called when we received response to our invitation
        * @param aAcceptance Accepted or rejected
        * @param aUserId User Id of the response sender
        * @param aGroupName group name
        * @param aResponse response text
        */
        virtual void HandleInvitationResponse( TBool aAcceptance,
                                               const TDesC& aUserId,
                                               const TDesC& aGroupName,
                                               const TDesC& aResponse ) = 0;

    public:
        /**
         * Destructor.
         */
        virtual ~MCAInvitationObserverPC() {};
    };

#endif      // MCASEARCHOBSERVERINTERFACE_H

// End of File
