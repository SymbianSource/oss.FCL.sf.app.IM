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
* Description:  Editor for invite creation
*
*/


#ifndef CCAINVITEEDITOR_H
#define CCAINVITEEDITOR_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class MCAInviteConflictResolver;
class MCAServerContactsArrayPC;
class MCAInviteEditorPC;


// CLASS DECLARATION

/**
*  Editor for invite creation.
*
*  @lib chatng.app
*  @since 1.2
*/
class CCAInviteEditor : public CBase
    {
    public: // Enums

        enum TInviteResult // result of invitation
            {
            ECreationCancelled = 1,
            EInviteCancelled,
            ERecipientAcceptedInvite,
            ERecipientCancelledInvite
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aGroup Reference of GroupId
        * @return Instance of this class
        */
        static CCAInviteEditor* NewL( const TDesC& aGroup );

        /**
        * Two-phased constructor.
        * @param aGroup Reference of GroupId
        * @return Instance of this class
        */
        static CCAInviteEditor* NewLC( const TDesC& aGroup );

        /**
        * Destructor.
        */
        virtual ~CCAInviteEditor();

    public: // New methods

        /**
        * Creates and sends invitation
        * @param aResolver for conflicts with banned users.
        * @param aReceiver Receiver of invitation. If not given, then user
        * will be asked for recipient.
        * @return Did system managed to create and send invitation
        */
        TBool CreateAndSendInvitation( MCAInviteConflictResolver& aResolver,
                                       const TDesC& aReceiver = KNullDesC
                                     );

    private:

        /**
        * Populates list of wvids to whom user can send invitation
        * @param aInviteList List of user-ids
        * @param aRawOnlineList Raw data online-list
        * @param aSelectionArray Array of selected indexes
        */
        void PopulateInviteListL( CDesCArray& aInviteList,
                                  const MCAServerContactsArrayPC& aRawOnlineList,
                                  const CArrayFixFlat<TInt>& aSelectionArray );

        /**
         * Gets online friends from Presence API and fetch nicknames also
         * for contact ID's from Storage.
         * @return contacts array with wvid's and nicknames
         */
        MCAServerContactsArrayPC* GetOnlineFriendsLC();

        /**
        * Queries invitation message
        * Ownership is transferred to caller
        * @return Message
        */
        HBufC* QueryInviteMessageL();

        /**
        * Shows error note
        */
        void ShowErrorNoteL();

    private:
        /**
        * C++ default constructor.
        * @param aInviteAPI Reference to invite API
        * @param aPresenceAPI Reference to presence API
        */
        CCAInviteEditor( );

        /**
        * By default Symbian OS constructor is private.
        * @param aGroup GroupId
        */
        void ConstructL( const TDesC& aGroup );

        /**
        * "Real" logic for creating and sending invitation
        */
        void CreateAndSendInvitationL( MCAInviteConflictResolver& aResolver,
                                       const TDesC& aReceiver = KNullDesC
                                     );

    private:    // Data
        // Owns.
        HBufC* iGroupID;

        // Own's iInviteEditorPC
        MCAInviteEditorPC* iInviteEditorPC;
    };

#endif      // CCAINVITEEDITOR_H

// End of File
