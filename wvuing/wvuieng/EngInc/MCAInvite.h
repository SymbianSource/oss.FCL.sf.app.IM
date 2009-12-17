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
* Description:  invitation interface for UI
*
*/


#ifndef MCAINVITE_H
#define MCAINVITE_H

// INCLUDES
#include <e32std.h>

// INCLUDES
#include "CCAInvitationRequest.h"
#include "MCAInvitation.h"

// FORWARD DECLARATIONS
class MCAInviteObserver;

// CLASS DECLARATION

/**
*  Invitation handling interface for UI.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCAInvite
    {
    public:

        /**
        * Sends an invitation for joining to a group.
        * @param aInvitation specifies the invitation (texts etc.).
        */
        virtual void SendInvitationL(
            const CCAInvitationRequest& aInvitation ) = 0;

        /**
        * This is the reply to an invitation.
        * Note that invitation gets removed by this call, so do not try to use
        * it after calling this!!!
        * @param aInvitation is the invitation.
        * @param aInviteAccepted specifies if the invite was accepted or not.
        * @param aResponse is the response
        * @param aScreenName is our own screenname
        */
        virtual void ReplyInvitationL( const MCAInvitation* aInvitation,
                                       const TBool aInviteAccepted,
                                       const TDesC& aResponse,
                                       const TDesC& aScreenName ) = 0;

        /**
        * Adds an invite observer. Invitation events are guided to this
        * observer.
        * @param aObserver specifies the observer.
        * @return KErrNone if successful, otherwise one of the system error codes
        */
        virtual TInt AddInviteObserver( MCAInviteObserver* aObserver ) = 0;

        /**
        * Removes an invite observer.
        * @param aObserver specifies the observer.
        */
        virtual void RemoveInviteObserver( MCAInviteObserver* aObserver ) = 0;

        /**
         * Populates the list with currently unhandled invitations
         * @since 2.1
         */
        virtual void PopulateInviteList( RPointerArray<MCAInvitation>& aList ) = 0;

        /**
         * Returns the number of unhandled invitations
         * @since 2.1
         * @return The number of received active
         */
        virtual TInt ActiveInvitations() = 0;

        /**
         * Removes the invitation
         * @since 2.1
         * @param aInvitation
         */
        virtual void RemoveInvitationL( MCAInvitation* aInvitation ) = 0;

        /**
         * Gets the pointer to buffer holding the reject reason
         * This is stored in the engine side because the last written
         * reason must be remembered
         * @since 2.1
         * @return The pointer to reject reason, which can be modified
         */
        virtual HBufC* RejectReasonPtr() = 0;

        /**
         * Amount of unread invite messages
         * @return Amount of unread invite messages
         */
        virtual TInt UnreadInvitesCount() const = 0;

    protected:

        /**
        * Deletion through this class is not wanted.
        */
        virtual ~MCAInvite() {};
    };

#endif      // MCAINVITE_H

// End of File
