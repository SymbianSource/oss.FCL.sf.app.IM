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
* Description:  Invitation handling interface for UI
*
*/

#ifndef MCAINVITEOBSERVER_H
#define MCAINVITEOBSERVER_H

// INCLUDES
#include "MCAInvitation.h"

#include <e32std.h>

// CLASS DECLARATION

/**
*  Invitation handling interface for UI. Receiving of invites is serialized
*  in engine, so client will receive only one invite per time through this
*  interface. If engine receives several invites, it will cache those invites
*  and call appropriate methods from this interface after client has returned
*  control from HandleInvitation.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCAInviteObserver
    {
    public:
        enum TInviteEventType
            {
            ENewInvite,
            EInviteExpired,
            EInviteCancel,
            EInviteRead
            };

        /**
         * Gets called when invite event occurs.
         * @param aEvent Type of event.
         * @param aInvitation specifies the invitation message.
         */
        virtual void HandleInvitationEventL( TInviteEventType aEvent,
                                             const MCAInvitation* aInvitation ) = 0;

        /**
         * Gets called when we received response to our invitation
         * @since 2.1
         * @param aAcceptance Accepted or rejected
         * @param aUserId User Id of the response sender
         * @param aGroupName group name
         * @param aResponse response text
         */
        virtual void HandleInvitationResponse( TBool aAcceptance,
                                               const TDesC& aUserId,
                                               const TDesC& aGroupName,
                                               const TDesC& aResponse ) = 0;
    protected:

        /**
        * Destructor.
        */
        virtual ~MCAInviteObserver() {};
    };

#endif      // MCAINVITEOBSERVER_H

// End of File
