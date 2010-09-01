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
* Description:  Represents the search data for UI
*
*/


#ifndef CCAINVITATIONPC_H
#define CCAINVITATIONPC_H


// INCLUDES
#include	"MCAInvitationPC.h"
#include 	"MCAInviteObserver.h"
//Platform includes
#include	"e32base.h"

//Forward Declaration
class MCAInvitationObserverPC;
class MCAChatInterface;
class MCAStoredContacts;
class MCAMainViewArrayPC;
class MCAInvitation;
class CCAEngine;
class MCAInvite;
//CLASS DECLARATION
/**
*  Invitation dialog  representation for the UI.
*
*  @lib wvuiprocessng.lib
*  @since 3.2
*/
class CCAInvitationPC: public CBase,
            public MCAInvitationPC,
            public MCAInviteObserver

    {

    public:

        /**
         * Symbian Two Phase Constructor
         *
         */
        static CCAInvitationPC* NewL( CCAEngine& aEngine, MCAChatInterface& aChatInterface );

        /**
         * Symbian Two Phase Constructor
         *
         */
        void ConstructL( );
        /**
         * virtual destructor
         */
        virtual ~CCAInvitationPC( );

    public:// From MCAInviteObserver

        /**
         * Gets called when invite event occurs.
         * @param aEvent Type of event.
         * @param aInvitation specifies the invitation message.
         */
        virtual void HandleInvitationEventL( TInviteEventType aEvent,
                                             const MCAInvitation* aInvitation );

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
                                               const TDesC& aResponse );
    public://New Methods
        /**
         *
         *
         */
        void RegisterObserver( MCAInvitationObserverPC* aObserver );

        /**
         *
         *
         */
        void UnRegisterObserver();

        /**
         * Gets the const reference to the UserID of the give invitaton index
         * @param aIndex: invitation index
         */
        const TDesC& InvitationUserIDL( TInt aIndex );

        /**
         * Gets the time this invite was received
         * @param aIndex is the invitation index.
         * @return Time and date of creation
         */
        TTime ReceivedAt( TInt aIndex );

        /**
         * Gets the invitation message.
         * @param aIndex is the invitation index.
         * @return Message.
         */
        const TDesC& Message( TInt aIndex );

        /**
         * Gets the group name.
         * @return Group name.
         */
        const TDesC& GroupName( TInt aIndex );

        /**
         * Sets the active invitation index
         */
        void SetActiveInvitation( const TInt aIndex );

        /**
         * Gets the active invitation index
         */
        TInt GetActiveInvitationIndex( );


        TBool IsInvitationUnread( TInt aIndex );

        /**
         * Set invitation as read.
         * @param aIndex is the tabindex of
         * the current active invitation
         */
        void SetInvitationAsReadL( TInt aIndex );

        /**
         * Maps the given tabindex to invitationindex
         * in mainviewarraypc
         * @return invitationindex
         */
        TInt GetInvitationIndex( TInt aTabIndex );

        /**
         * Returns the Count of Invitations.
         */
        TInt InvitationCount( );

        /**
         * Deletes the invitation
         * @param aInvitationIndex: index of the
         * invitation to be deleted
         */
        void DeleteInvitationL( TInt aInvitationIndex );

        /**
         * This is the reply to an invitation.
         * Note that invitation gets removed by this call, so do not try to use
         * it after calling this!!!
         * @param aInvitationIndex is the invitation index in teh array.
         * @param aInviteAccepted specifies if the invite was accepted or not.
         * @param aResponse is the response
         * @param aScreenName is our own screenname
         */
        void ReplyInvitationL( TInt aInvitationIndex,
                               const TBool aInviteAccepted,
                               const TDesC& aResponse,
                               const TDesC& aScreenName );

        /**
         * Gets the buffer holding the reject reason
         * This is stored in the engine side because the last written
         * reason must be remembered
         * @return The buffer to reject reason, which can be modified
         */
        HBufC* RejectReason();

        /**
         * Deletes the invitation at aIndex
         * @param aIndex Index of the invitation to be deleted
         */
        void DeleteSingleListViewInvitationL( const TDesC& aInviteID/*TInt aIndex*/ );

        /**
        * Finds an invitation
        * @param aWvId - contatc wv
        * @param aInviteTime - Invite Time
        * @return TInt - Index of the found invitation else -1
        */
        TInt FindInvitationIndex( const TDesC& aWvId, TTime aInviteTime );

        /**
        * Finds an invitation
        * @param aInviteId: id of the invitation
        * @return TInt - Index of the found invitation else -1
        */
        TInt FindInvitationIndex( const TDesC& aInviteId );

        /**
        * Unread Invites Counts
           * return no of unread inviation
        */
        TInt UnreadInvitesCount() const;

        /**
        * Unresd Invitation Info
           * return Invitation Id
        */

        const TDesC& UnresdInvitationInfo() ;
        const TDesC& GetInviteID( TInt aInvitationIndex );

        /**
         * @param aIndex : invitation index
        	* return identification of the sender of the invitation
         */
        TPtrC InvUserIdentificationL( TInt aIndex );
    private:

        /**
         * default constructor
         * @param
         */
        CCAInvitationPC( CCAEngine& aEngine, MCAChatInterface& aChatInterface );

        /**
         * Returns index for given invitation
         * @param aInvitation
         * @return index of invitation
         */
        TInt FindIndexForInvitation( const MCAInvitation* aInvitation ) const;

        /**
         * Deletes item from Invitationarray
         * @param aIndex Item index
         */
        void Delete( TInt aIndex );
    private:

        //DoesNot own.
        CCAEngine& iEngine;

        MCAChatInterface& iChatInterface;

        // Doesn't own. Handle to contacts interface
        MCAStoredContacts* iContacts;

        //Owns. Contact id
        HBufC* iWvId;

        //Doesnot own
        MCAInvite& iInviteInterface;

        //Owns. Maintains the invitations
        RPointerArray<MCAInvitation> iInvitationArray;

        TInt iCurrentActiveInvitation;

        MCAInvitationObserverPC* iObserver;

    };

#endif      // CCAINVITATIONPC_H

// End of File
