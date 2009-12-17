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
* Description:  Global interface for the UI to implement to get notifications (call back)
*				 when theres a change in contact, list, invite, settings, conversation,
*				 group etc
*
*/



#ifndef MCAUIGLOBALNOTIFICATIONOBSERVERPC_H
#define MCAUIGLOBALNOTIFICATIONOBSERVERPC_H

#include <e32std.h>

#include "TEnumsPC.h"


// CLASS DECLARATION

/**
 *  Interface for observer
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */

class MCAUiGlobalNotificationObserverPC
    {
    public: // New functions

        /**
         * Called when a contact is deleted.
         * @param aId The ID of the deleted contact
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        virtual void HandleDelete( const TDesC& aContactId , TEnumsPC::TItem type ) = 0;

        /**
         * Called when a contact is added.
         * @param aList. List for contact that was added.
         * @param aContact The contact which was added
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        virtual void HandleAddition( TEnumsPC::TItem type ) = 0;

        /**
         * Called when a contact or list is about to change, or has been changed.
         * @param aType: the itemtype is type of list item at the hand.
         * @param aChangeType:the changetype is event that has occured due to call from engine
         */

        virtual void HandleChange( TEnumsPC::TItem aType, TEnumsPC::TChange aChangeType ) = 0;


        /**
        * Gets called when invite event occurs.
        * @param aEvent Type of event.
        * @param aUserId userid of the sender
        * @param aMessage invite message
        */
        virtual void HandleInvitationEventL( TEnumsPC::TInviteType aInviteType,
                                             const TDesC& aUserId,
                                             const TDesC& aMessage ) = 0;

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

        /**
        * Handles setting changes.
        * @param aChangedSettingEnum specifies the changed setting.
        */
        virtual void HandleSettingsChangeL( TInt aChangedSettingEnum ) = 0;

        /**
         * Handle chat events.
         * @param aWvid - WVID
         * @param aContactId - Contact identification
         * @param aIndex - The index of the conversation in the local store of PC
         * @param aChatInList - if new chat then ETrue
         * @param aMessage - The message text
         */
        virtual void HandleChatEvent( const TEnumsPC::TChatEventType aEvent,
                                      const TDesC& aWvid,
                                      const TDesC& aContactId,
                                      const TDesC& aMessage,
                                      const TEnumsPC::TMessagerType aMsgrType,
                                      const TEnumsPC::TMessageType aMsgType ) = 0;

        /**
        * To handle error condition in message related event
        * aError: error id
        * aInfo: error detail
        */
        virtual void HandleMessageError( TInt aError, const TDesC& aInfo ) = 0;

        /**
         * Virtual destructor
         */
        virtual ~MCAUiGlobalNotificationObserverPC() {};
    };

#endif      // MCAUIGLOBALNOTIFICATIONOBSERVERPC_H

// End of File
