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
* Description:  Contact data container observer interface
*
*/



#ifndef MCAMAINVIEWOBSERVERPC_H
#define MCAMAINVIEWOBSERVERPC_H

#include <e32std.h>
//#include "TStorageManagerGlobals.h"

#include "TEnumsPC.h"


// CLASS DECLARATION

/**
 *  Interface for observer
 *
 *  @lib wvuiprocess.dll
 *  @since 3.0
 */

class MCAMainViewObserverPC
    {
    public: // New functions

        /**
         * Called when a contact is deleted.
         * @param aId The ID of the deleted contact
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        virtual void HandleDelete( const TDesC& aContactId ,
                                   TEnumsPC::TItem type ) = 0;

        /**
         * Called when a contact is added.
         * @param aList. List for contact that was added.
         * @param aContact The contact which was added
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        virtual void HandleAddition( TEnumsPC::TItem type,
                                     TInt aIndex ) = 0;

        /**
         * Called when a contact or list is about to change, or has been changed.
         * @param aList List that is changed. Can be NULL if contact is changed.
         * @param aContact Contact that is changed. Can be NULL if list is changed
         * @param aEventType The type of event: EStorageEventPreChange before
         * the change (contact still has the old information), or
         * EStorageEventPostChange after the change took place
         * (contact has new information)
         */
        // the itemtype is type of list item at the hand..
        // the changetype is event that has occured due to call from engine....

        virtual void HandleChange( TEnumsPC::TItem type, TInt aIndex,
                                   TEnumsPC::TChange changeType ) = 0;

        virtual void HandleWatcherEvent( const TDesC &  aString ,
                                         TEnumsPC::TOnlineStatus aOnlineStatus ) = 0;

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
         * Virtual destructor
         */
        virtual ~MCAMainViewObserverPC() {};
    };

#endif      // MCAMAINVIEWOBSERVERPC_H

// End of File
