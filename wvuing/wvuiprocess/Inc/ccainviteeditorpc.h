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
* Description:  Implementation for PC InviteEditor Interface
*
*/


#ifndef CCAINVITEEDITORPC_H
#define CCAINVITEEDITORPC_H


// INCLUDES
//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "MCAInviteEditorPC.h"

// FORWARD DECLARATIONS
class MCAInvite;
class MCAPresence;
class MCAServerContactsArrayPC;
class MCAInviteConflictResolver;
//class declaration
/**
 *  Implementation class for InviteEditor Interface for the UI.
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class CCAInviteEditorPC:  public CBase,
            public MCAInviteEditorPC
    {

    public:

        /**
         * Symbian Two Phase Constructor
         * @param aEngine : a reference to chat engine
         */
        static CCAInviteEditorPC* NewL( MCAInvite& aInviteAPI,
                                        MCAPresence& aPresenceAPI );


        /**
         * Second phased symbian constructor.
         */
        void ConstructL();

        /**
         * virtual destructor
         */
        virtual ~CCAInviteEditorPC();

    private:

        /**
         * default constructor
         * @param aEngine : a reference to chat engine
         */

        CCAInviteEditorPC( MCAInvite& aInviteAPI,
                           MCAPresence& aPresenceAPI );

    public: //From MCAInviteEditorPC
        /**
         * Sends an invitation for joining to a group.
         * @param aUserIDs specifies the user IDs who are going to receive
         *        this invitation.
         * @param aGroupID is the group WV ID.
         * @param aMessage is the invitation message.
         */
        TInt SendInvitationL( const CDesCArray& aUserIDs,
                              const TDesC& aGroupID,
                              const TDesC& aMessage );
        /**
         * Gets online friends from Presence API and fetch nicknames also
         * for contact ID's from Storage.
         * @return contacts array with wvid's and nicknames
         */
        MCAServerContactsArrayPC* GetOnlineFriendsLC( const TDesC& aIconTabulator );

        /**
        * Populates list of wvids to whom user can send invitation
        * @since 2.1
        * @param aInviteList List of user-ids
        * @param aRawOnlineList Raw data online-list
        * @param aSelectionArray Array of selected indexes
        */
        void PopulateInviteListL( CDesCArray& aInviteList,
                                  const MCAServerContactsArrayPC& aRawOnlineList,
                                  const CArrayFixFlat<TInt>& aSelectionArray );
    private://Data

        MCAInvite& iInviteAPI;

        MCAPresence& iPresenceAPI;
    };

#endif      // CCAINVITEEDITORPC_H

// End of File
