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
* Description:  Interface for Process Manager
*
*/


#ifndef MCAINVITATIONPC_H
#define MCAINVITATIONPC_H

//INCLUDES
//Platform includes
#include	"e32base.h"

//FORWARD DECLARATION

class MCAInvitationObserverPC;

//CLASS DECLARATION

class MCAInvitationPC
    {

    public:

        virtual void RegisterObserver( MCAInvitationObserverPC* aObserver ) = 0;

        virtual void UnRegisterObserver() = 0;

        /**
         * Gets the UserID of the give invitaton index
         * @param aIndex: invitation index
         */
        virtual const TDesC& InvitationUserIDL( TInt aIndex ) = 0;

        /**
         * Gets the time this invite was received
         * @param aIndex is the invitation index.
         * @since 3.2
         * @return Time and date of creation
         */
        virtual TTime ReceivedAt( TInt aIndex ) = 0;

        /**
         * Gets the invitation message.
         * @param aIndex is the invitation index.
         * @return Message.
         */
        virtual const TDesC& Message( TInt aIndex ) = 0;

        /**
         * Gets the group name.
         * @return Group name.
         */
        virtual const TDesC& GroupName( TInt aIndex ) = 0;

        /**
         * Sets the active invitation index
         */
        virtual void SetActiveInvitation( const TInt aIndex ) = 0;

        /**
         * Gets the active invitation index
         */
        virtual TInt GetActiveInvitationIndex( ) = 0;

        /**
         * checks if the given invitation is read or not
         * @returns ETrue if the invitation is read. Else EFalse.
         */
        virtual TBool IsInvitationUnread( TInt aIndex ) = 0;

        /**
         * Set invitation as read.
         * @param aIndex is the tabindex of
         * the current active invitation
         */
        virtual void SetInvitationAsReadL( TInt aIndex ) = 0;

        /**
         * Maps the given tabindex to invitationindex
         * in mainviewarraypc
         * @return invitationindex
         */
        virtual TInt GetInvitationIndex( TInt aTabIndex ) = 0;



        /**
         * Deletes the invitation
         * @param aInvitationIndex: index of the
         * invitation to be deleted
         */
        virtual void DeleteInvitationL( TInt aInvitationIndex ) = 0;

        /**
         * This is the reply to an invitation.
         * Note that invitation gets removed by this call, so do not try to use
         * it after calling this!!!
         * @param aInvitationIndex is the invitation index in teh array.
         * @param aInviteAccepted specifies if the invite was accepted or not.
         * @param aResponse is the response
         * @param aScreenName is our own screenname
         */
        virtual void ReplyInvitationL( TInt aInvitationIndex,
                                       const TBool aInviteAccepted,
                                       const TDesC& aResponse,
                                       const TDesC& aScreenName ) = 0;

        /**
        * Gets the buffer holding the reject reason
        * This is stored in the engine side because the last written
        * reason must be remembered
        * @return The buffer to reject reason, which can be modified
        */
        virtual HBufC* RejectReason() = 0;

        /**
         * Deletes the invitation at aIndex
         * @param aIndex Index of the invitation to be deleted
         */
        virtual void DeleteSingleListViewInvitationL( const TDesC& aInviteID/*TInt aIndex*/ ) = 0;

        /**
        * Finds an invitation
        * @param aWvId - contatc wv
        * @param aInviteTime - Invite Time
        * @return TInt - Index of the found invitation else -1
        */
        virtual TInt FindInvitationIndex( const TDesC& aWvId, TTime aInviteTime ) = 0 ;


        /**
        * Finds an invitation
        * @param aInviteId: id of the invitation
        * @return TInt - Index of the found invitation else -1
        */
        virtual TInt FindInvitationIndex( const TDesC& aInviteId ) = 0;

        /**
        * Unread Invites Counts
           * return no of unread inviation
        */
        virtual TInt UnreadInvitesCount() const = 0;

        /**
        * Unresd Invitation Info
           * return Invitation Id
        */


        virtual const TDesC& UnresdInvitationInfo() = 0 ;

        virtual const TDesC& GetInviteID( TInt aInvitationIndex ) = 0;

        /**
        * @param aIndex : invitation index
           * return identification of the sender of the invitation
        */
        virtual TPtrC InvUserIdentificationL( TInt aIndex ) = 0;

        /**
         * Destructor
         */
        virtual ~MCAInvitationPC() {};

    };

#endif    //MCAINVITATIONPC_H

//End of File