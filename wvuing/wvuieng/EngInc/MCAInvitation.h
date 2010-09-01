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
* Description:  Used when inviting to chats.
*
*/


#ifndef MCAINVITATION_H
#define MCAINVITATION_H

//  INCLUDES
#include <e32base.h>
// CLASS DECLARATION

// Helper class for observing if invite is read or not.
class MCAInviteReadObserver
    {
    public: // Interface

        /**
         * Inform observer that invite is read.
         */
        virtual void HandleInviteReadL() const = 0;
    };

/**
*  Describes interface to one invitation .
*  Used when inviting to chats.
*
*  @lib CAEngine.lib
*  @since 3.0
*/
class MCAInvitation
    {
    public: // New interface

        /**
        * Gets the invitation ID.
        * @return Invite ID.
        */
        virtual const TDesC& InviteID() const = 0;

        /**
        * Gets the user ID.
        * @return User ID ( WV ID ).
        */
        virtual const TDesC& UserID() const = 0;

        /**
        * Gets the screen name.
        * @return Screen name.
        */
        virtual const TDesC& ScreenName() const = 0;

        /**
        * Gets the group name.
        * @return Group name.
        */
        virtual const TDesC& GroupName() const = 0;

        /**
        * Gets the group ID.
        * @return Group name.
        */
        virtual const TDesC& GroupId() const = 0;

        /**
        * Gets the invitation message.
        * @return Message.
        */
        virtual const TDesC& Message() const = 0;

        /**
        * Gets the timeout value.
        * @return Timeout value.
        */
        virtual TInt Timeout() const = 0;

        /**
         * Gets the time this invite was received (created)
         * @since 2.1
         * @return Time and date of creation
         */
        virtual TTime ReceivedAt() const = 0;

        /**
         * Set invitation as read.
         * @since 3.0
         */
        virtual void SetInvitationAsReadL() = 0;

        /**
         * Is invitation read or not.
         * @return Read status on invitation message. ETrue, read, EFalse not.
         */
        virtual TBool IsRead() const = 0;

        /**
         * Invite can be deleted through this interface
         */
        virtual ~MCAInvitation() {};

    };

#endif      // MCAINVITATION_H

// End of File
