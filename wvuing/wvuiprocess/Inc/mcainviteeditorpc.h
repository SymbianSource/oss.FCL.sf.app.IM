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
* Description:  Interface for Invite operations
*
*/


#ifndef MCAINVITEEDITORPC_H
#define MCAINVITEEDITORPC_H


// INCLUDES
#include "badesca.h"
#include "MCAServerContactsArrayPC.h"
// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 *  Inviteeditor interface for UI
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAInviteEditorPC

    {
    public:
        /**
         * Sends an invitation for joining to a group.
         * @param aUserIDs specifies the user IDs who are going to receive
         *        this invitation.
         * @param aGroupID is the group WV ID.
         * @param aMessage is the invitation message.
         */
        virtual TInt SendInvitationL( const CDesCArray& aUserIDs,
                                      const TDesC& aGroupID,
                                      const TDesC& aMessage ) = 0;

        /**
         * Gets online friends from Presence API and fetch nicknames also
         * for contact ID's from Storage.
         * @return contacts array with wvid's and nicknames
         */
        virtual MCAServerContactsArrayPC* GetOnlineFriendsLC( const TDesC& aIconTabulator ) = 0;

        /**
         * Populates list of wvids to whom user can send invitation
         * @since 2.1
         * @param aInviteList List of user-ids
         * @param aRawOnlineList Raw data online-list
         * @param aSelectionArray Array of selected indexes
         */
        virtual void PopulateInviteListL( CDesCArray& aInviteList,
                                          const MCAServerContactsArrayPC& aRawOnlineList,
                                          const CArrayFixFlat<TInt>& aSelectionArray ) = 0;
    public:

        /**
         * Destructor.
         */
        virtual ~MCAInviteEditorPC() {};

    };

#endif      // MCAGROUPPC_H

// End of File
