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
* Description:  Global interface for the UI to get notifications when theres a
*				 change in contact, list, invite, settings, conversation, group etc
*
*/



#ifndef CCAGLOBALNOTIFICATIONOBSERVERPC_H
#define CCAGLOBALNOTIFICATIONOBSERVERPC_H

#include <e32std.h>
//#include "TStorageManagerGlobals.h"

#include "TEnumsPC.h"

#include "MCAGlobalNotificationObserverPC.h"
#include "MCAStoredContactsObserver.h"
#include "MCAInviteObserver.h"
#include "MCASettingsObserver.h"
#include "MCAChatObserver.h"
#include "MCAMessageErrorObserver.h"

class CCAEngine;
class MCAChatInterface;
class MCAContactListModel;
class MCAStoredContacts;
class MCASettings;
class MCAInvite;
class MCAMessage;
class MCAUiGlobalNotificationObserverPC;
class MCAMessageUtils;
// CLASS DECLARATION

/**
 *  Interface for observer
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */

class CCAGlobalNotificationObserverPC: public CBase,
            public MCAGlobalNotificationObserverPC,
            public MCAStoredContactsObserver,
            public MCAInviteObserver,
            public MCASettingsObserver,
            public MCAChatObserver,
            public MCAMessageErrorObserver
    {

    public: //Constructor and Destructors
        /**
        * construction
        */
        void ConstructL();
        /**
        * construction
        * @param aContactListModel : a reference to the contact list model object at the chat engine
        */
        static CCAGlobalNotificationObserverPC* NewL(
            MCAContactListModel& aContactListModel,
            MCAChatInterface& aChatInterface,
            MCAInvite& aInviteInterface,
            MCASettings& aSettingsInterface,
            const MCAMessageUtils& aMessageUtils );
        /**
         * destructor
         */
        ~CCAGlobalNotificationObserverPC();
    public:
        TEnumsPC::TCASettingValues ConvertToSettingsEnumsPC( TInt aStatus );

    public://new functions, called from ccasyncchecker

        /*
        * @see MCAGlobalNotificationObserverPC
        */
        virtual TBool IsContactListsSyncDoneL();

        /*
        * @see MCAGlobalNotificationObserverPC
        */
        virtual TBool IsValidListInterfaceL();

        /*
        * @see MCAGlobalNotificationObserverPC
        */
        virtual TBool StoredContactsIsAllSynchronised( TBool& aFailed );

        /*
         * @see MCAGlobalNotificationObserverPC
         */
        virtual TBool IsValidStoredContactsInterface();

    public: //MCAChatObserver
        /**
        * Handle events.
        * @param aEvent Event to handle.
        * @param aMessage, Event occurred because of message
        */
        virtual void HandleChatEvent( TChatEventType aEvent, MCAMessage* aMessage = NULL );

    public: // MCAMessageErrorObserver

        /**
         * Handle events.
         * @param aMessage, Event occurred because of message
         */
        virtual void HandleMessageError( TInt aError, MCAMessage* aMessage );

    public: // MCAStoredContactsObserver

        /**
         * Called when a contact is deleted.
         * @param aId The ID of the deleted contact
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        virtual void HandleContactDelete( const TDesC& aContactId );

        /**
         * Called when a contact is added.
         * @param aList. List for contact that was added.
         * @param aContact The contact which was added
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        virtual void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact );

        /**
         * Called when a contact or list is about to change, or has been changed.
         * @param aList List that is changed. Can be NULL if contact is changed.
         * @param aContact Contact that is changed. Can be NULL if list is changed
         * @param aEventType The type of event: EStorageEventPreChange before
         * the change (contact still has the old information), or
         * EStorageEventPostChange after the change took place
         * (contact has new information)
         */
        virtual void HandleChange( MCAContactList* aList, MCAStoredContact* aContact,
                                   TStorageManagerGlobals::TCAObserverEventType aEventType,
                                   TBool aUserIdChanged );

    public: // From MCAInviteObserver

        /**
         * @see MCAInviteObserver
         */
        virtual void HandleInvitationEventL( TInviteEventType aEvent,
                                             const MCAInvitation* aInvitation );

        /**
         * @see MCAInviteObserver
         */
        virtual void HandleInvitationResponse( TBool aAcceptance,
                                               const TDesC& aUserId,
                                               const TDesC& aGroupName,
                                               const TDesC& aResponse );


    public: //From MCASettingsObserver

        /**
        * HandleSettingsChangeL: To handle the change events in settings
        * @see MCASettingsObserver
        * @param aChangedSettingEnum: enumeration of the type of settings change
        * @return void
        */
        virtual void HandleSettingsChangeL( TInt aChangedSettingEnum );

    public: //MCAUiGlobalNotificationObserverPC
        /**
        * Add the UI class as an observer for call backs
        * @param aGlobalObserver: reference to the UI class which implements
        * the MCAUiGlobalNotificationObserverPC interface
        * @return void
        */
        virtual void AddGlobalObserverL( MCAUiGlobalNotificationObserverPC* aGlobalObserver );

        /**
        * Add the UI class as an observer for call backs
        * @param aGlobalObserver: reference to the UI class which implements
        * the MCAUiGlobalNotificationObserverPC interface
        * @return void
        */
        virtual void RemoveGlobalObserver( MCAUiGlobalNotificationObserverPC* aGlobalObserver );

    private:
        /**
        * Get the current WVID from the Message
        * @param aMessage - Ptr to the message
        * @return Reference to the wvid, if aMessage is Not NULL else
        *         returns KNullDesC
        */
        const TDesC& GetWvId( MCAMessage* aMessage );

    private:
        /**
        * default constructor
        * @param aContactListModel : a reference to the contact list model object at the chat engine
        */
        CCAGlobalNotificationObserverPC(
            MCAContactListModel& aContactListModel,
            MCAChatInterface& aChatInterface,
            MCAInvite& aInviteObserver,
            MCASettings& aSettingsInterface,
            const MCAMessageUtils& aMessageUtils );

    private:

        /*
        * reference to the contact list model
        */
        MCAContactListModel& iContactListModel;

        /*
        * Reference to chat interface
        */
        MCAChatInterface& iChatInterface;

        /**
        * Reference to Invite Interface
        */
        MCAInvite& iInviteInterface;

        /**
        * Reference to Invite Interface
        */
        MCASettings& iSettingsInterface;

        const MCAMessageUtils& iMessageUtils;

        /*
        * reference to the contact list interface
        */
        MCAStoredContacts* iContactInterface;



        /* we need to maintain the list of observers here.whenever any event is
        * happening then we need call back the UI
           */
        RPointerArray< MCAUiGlobalNotificationObserverPC > iGlobalNotificationObservers;


    };

#endif      // CCAGLOBALNOTIFICATIONOBSERVERPC_H

// End of File
