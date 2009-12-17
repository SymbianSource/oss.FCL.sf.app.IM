/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for message containers
*
*/


#ifndef CCACHATCONTAINER_H
#define CCACHATCONTAINER_H

#include "impsbuilddefinitions.h"
#include "MCABufferMemoryHandler.h"
#include "MCAChatInterface.h"
#include "MCAChatObserver.h"

//	FORWARD CLASS DECLERATIONS
class CCAMessageContainer;
class MCASettings;

// CLASS DECLARATION

/**
 *  Container for message containers
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class CCAChatContainer : public CBase,
            public MCAChatInterface,
            public MCABufferMemoryHandler,
            public MCAChatObserver
    {
    public: // Construction

        /**
         * Construction
         */
        static CCAChatContainer* NewL( MCASettings& aSettingsInterface );

        /**
         * Destruction
         */
        ~CCAChatContainer();

    private: // Construction

        /**
         * Constructor
         */
        CCAChatContainer( MCASettings& aSettingsInterface );

    private: // Methods from MCAChatInterface

        /**
         * @see MCAChatInterface
         */
        MCAMessagesReadInterface& MessageReadInterfaceL(
            const TDesC& aServerAddress,
            const TDesC& aUserId,
            const TDesC& aTargetId,
            MCAMessagesReadInterface::TContainerType aType );

        /**
         * @see MCAChatInterface
         */
        MCAMessagesWriteInterface& MessageWriteInterfaceL(
            const TDesC& aServerAddress,
            const TDesC& aUserId,
            const TDesC& aTargetId,
            MCAMessagesReadInterface::TContainerType aType );

        /**
         * @see MCAChatInterface
         */
        void DeleteChatL( const TDesC& aServerAddress,
                          const TDesC& aUserId,
                          const TDesC& aTargetId );

        /**
         * @see MCAChatInterface
         */
        void CloseAllContainers();

        /**
         * @see MCAChatInterface
         */
        MCAMessagesReadInterface* ChatExistsL( const TDesC& aServerAddress,
                                               const TDesC& aUserId,
                                               const TDesC& aTargetId );

        /**
         * @see MCAChatInterface
         */
        MCAMessagesReadInterface& ChangeChatIdL(
            const TDesC& aOldServerAddress,
            const TDesC& aOldUserId,
            const TDesC& aOldTargetId,
            const TDesC& aNewServerAddress,
            const TDesC& aNewUserId,
            const TDesC& aNewTargetId,
            MCAMessagesReadInterface::TContainerType aType );



        /**
         * @see MCAChatInterface
         */
        TInt GroupCount() const;

        /**
         * @see MCAChatInterface
         */
        TInt ChatCount() const;

        /**
         * @see MCAChatInterface
         */
        MCAMessagesReadInterface& GroupAt( TInt aIndex ) const;

        /**
         * @see MCAChatInterface
         */
        MCAMessagesReadInterface& ChatAt( TInt aIndex ) const;

        /**
         * @see MCAChatInterface
         */
        TInt RegisterChatObserver( MCAChatObserver* aObserver );

        /**
         * @see MCAChatInterface
         */
        TInt UnregisterChatObserver( MCAChatObserver* aObserver );

        /**
         * @see MCAChatInterface
         */
        void ResetPendingCount();

        /**
         * @see MCAChatInterface
         */
        TInt MessagesPendingCount( TInt& aCountOfChats,
                                   MCAMessagesReadInterface::TUnreadFilter aUnreadFilter =
                                       MCAMessagesReadInterface::EUnreadAll ) const;

        /**
         * @see MCAChatInterface
         */
        MCAMessageContainerInfo* PendingMessageInfo() const;

#ifdef RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
        /**
         * @see MCAChatInterface
         */
        TInt ChatGroupMessagesPendingCount( TInt& aCountOfChats ) const;

        /**
         * @see MCAChatInterface
         */
        MCAMessageContainerInfo* ChatGroupPendingMessageInfo() const;

#endif  // RD_CHAT_GROUP_MESSAGE_INDICATION_NEW

    private: // Methods from MCABufferMemoryHandler

        /**
         * @see MCABufferMemoryHandler
         */
        TBool FreeMemoryIfNeededL( TInt aSize );

        /**
         * @see MCABufferMemoryHandler
         */
        void SetAccessInterface( MCAChatInterface* aAccess );

    private: // Methods from MCAChatObserver

        /**
         * @see MCAChatObserver
         */
        void HandleChatEvent( TChatEventType aEvent,
                              MCAMessage* aMessage = NULL );

    public: // New methods

        /**
         * Resolve recipient of message identified by operation code.
         * @param aOperationCode, Identifier for getting right message.
         * @param aStatus. Status of operation. KErrNone/KErrNotFound
         * @return Message recipient
         */
        const TDesC& MessageRecipient( TInt aOperationCode,
                                       TInt& aStatus ) const;

        /**
         * Set external memoryhandler. Set with NULL,
         * if wanted to reset external handler.
         */
        void SetExternalMemoryHandler( MCABufferMemoryHandler* aMemoryHandler );

    private: // New helper methods

        /**
         * Return index to chat. Try to create if does not exists.
         * Leave if not possible. e.g. KErrNoMemory.
         * @param aServerAddress Part of identifier to identify container.
         * @param aUserId Part of identifier to identify container.
         * @param aTargetId Part of identifier to identify container.
         * @return Index of container.
         */
        TInt ChatL( const TDesC& aServerAddress,
                    const TDesC& aUserId,
                    const TDesC& aTargetId,
                    MCAMessagesReadInterface::TContainerType aType =
                        MCAMessagesReadInterface::ENoneContainer );

        /**
         * Returns latest found item by index.
         * FindChat or ChatL( id, type ) must be called before this one.
         * @param aIndex, Index of needed item.
         * @param aResetLatest. Reset type of latest found container.
         *                      ETrue, Reset, EFalse, No reseting.
         * @return Pointer to right container.
         */
        CCAMessageContainer* ChatL( TInt aIndex, TBool aResetLatest = ETrue );

        /**
         * FindChat or ChatL( id, type ) must be called before this one.
         * @param aIndex, Index of needed item.
         * @param aResetLatest. Reset type of latest found container.
         *                      ETrue, Reset, EFalse, No reset.
         */
        void RemoveChat( TInt aIndex, TBool aResetLatest = ETrue );

        /**
         * Finds if container exists and returns index.
         * @param aChat Reference chat.
         * @return Index of container. If container is not found,
         *                             KErrNotFound is returned.
         */
        TInt FindChat( const CCAMessageContainer& aChat );

        /**
         * Create new container. Change aServerAddress and userid to
         * current ones, if KNullDesC used.
         * @see CCAMessageContainer
         */
        CCAMessageContainer* CreateContainerLC(
            const TDesC& aServerAddress,
            const TDesC& aUserId,
            const TDesC& aTargetId,
            MCABufferMemoryHandler& aMemoryHandler,
            MCAMessagesReadInterface::TContainerType aType );
    private: // Member data

        /// Access to settings
        MCASettings& iSettingsInterface;

        /// Container of chats. Owns items.
        RPointerArray< CCAMessageContainer > iChatContainer;

        /// Container of groups. Owns items.
        RPointerArray< CCAMessageContainer > iGroupContainer;

        /// Container of send buffers. Owns items.
        RPointerArray< CCAMessageContainer > iSendContainer;

        /// Container of observers. Does not own items.
        RPointerArray< MCAChatObserver > iObservers;

        /// External memory handler if set.
        MCABufferMemoryHandler* iExternalMemoryHandler;

        /// Last found item type
        MCAMessagesReadInterface::TContainerType iLatestType;
    };


#endif      // CCACHATCONTAINER_H

// End of File
