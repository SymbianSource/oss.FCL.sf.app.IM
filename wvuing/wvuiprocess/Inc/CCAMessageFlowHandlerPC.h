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
* Description:  Flow controller for messages
*
*/



#ifndef CCAMESSAGEFLOWHANDLERPC_H
#define CCAMESSAGEFLOWHANDLERPC_H

//  INCLUDES
#include <e32base.h>
#include "MCAMessageObserver.h"
#include "MCASettingsObserver.h"
#include "MCAMessageFlowHandlerPC.h"

// FORWARD DECLARATIONS
class MCAMessagesReadInterface;
class MCASettings;
class MCAConversationManagerPC;
class MCARecordedChatsPC;


// CLASS DECLARATION

/**
*  Flow controller for messages
*
*  @lib wvuiprocessng.dll
*  @since Series 60 3 dot 2
*/
class CCAMessageFlowHandlerPC : public CTimer, public MCAMessageObserver,
            public MCASettingsObserver, public MCAMessageFlowHandlerPC
    {



    public:

        /**
         * Should we fetch new messages or not
         * @param aFetch ETrue if this chat is in foreground and should
         *               receive messages, EFalse otherwise.
         */
        void FetchMessages( TBool aFetch );



    protected: // Functions from MCAMessageObserver

        /**
         * Handle events.
         * @param aEvent Event to handle.
         * @param aIndex, Index of message which is affected by event.
         */
        void HandleMessageEvent( TMessageEventType aEvent, TInt aIndex );

    protected:  // Functions from CTimer

        /**
         * @see CTimer
         */
        void RunL();

        /**
         * @see CActive
         */
        TInt RunError( TInt aError );

    protected:  // Functions from MCASettingsObserver

        /**
         * Handles setting changes.
         * @param aChangedSettingEnum specifies the changed setting.
         */
        void HandleSettingsChangeL( TInt aChangedSettingEnum );


    private:  // New functions

        /**
         * Get flow control value from settings
         */
        void UpdateTimeIntervalL();

        /**
         * Is opening on going or not.
         * @since Series 60 v3.2
         * @return ETrue if opening on going, EFalse otherwise.
         */
        TBool IsOpening() const;

        /**
         * Corrects the real index to current index of messages.
         * This is needed when opening a chat, because messages
         * are not added to container in additional order.
         * @param aIndex Real index of message in container
         * @return current index of message in container.
         */
        TInt MessageIndexCorrection( TInt aIndex ) const;

    private:

        /**
        * C++ default constructor.
        */
        CCAMessageFlowHandlerPC( MCAMessageContainer& aMessageContainer,
                                 MCAMessagesReadInterface& aReadInterface,
                                 MCASettings& aSettings );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TBool aRecordedChatHandler );

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aMessageContainer Interface to UI side message container
         * @param aReadInterface Message read interface
         * @param aSettings Interface to settings manager
         * @param aRecordedChatHandler Is this handler for recorded chat
         *                             or not.
         */
        static CCAMessageFlowHandlerPC* NewL(
            MCAMessageContainer& aMessageContainer,
            MCAMessagesReadInterface& aReadInterface,
            MCASettings& aSettings,
            TBool aRecordedChatHandler );

        /**
         * Destructor.
         */
        virtual ~CCAMessageFlowHandlerPC();


    private:    // Data

        // Interface for UI side message container
        // Not owned
        MCAMessageContainer& iMessages;

        // Interface for fetching messages
        // Not owned
        MCAMessagesReadInterface& iReadInterface;

        // interface for settings manager
        MCASettings& iSettings;

        // time interval for receiving messages
        TTimeIntervalMicroSeconds32 iTimeInterval;

        // Is chat deleted already
        TBool iChatDeleted;

        // Should we fetch new messages
        TBool iFetchMessages;

        // Index for fetching messages
        TInt iMsgIndex;

        // Initial count for messages
        TInt iInitialMsgCount;

        // Is new message received while opening the chat
        TBool iNewMsgWhileOpening;

        // Flag is read from CR variation in class construction
        // ETrue in releases >= 3.2
        // EFalse in releases < 3.2
        // Set to EFalse if this handler is used to handle
        // recorded chat
        TBool iBgOpeningMode;

        TInt iAddedUnreadMsgs;

        // allow unit tests to access protected member functions
        friend class UT_CCAMessageFlowHandler; // CSI: 36 # see above

    };

#endif      // CCAMESSAGEFLOWHANDLERPC_H

// End of File
