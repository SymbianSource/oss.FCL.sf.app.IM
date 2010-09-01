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
* Description:  Interface for getting access to message containers.
*
*/


#ifndef MCACHATINTERFACE_H
#define MCACHATINTERFACE_H

#include "impsbuilddefinitions.h"
#include "MCAMessagesReadInterface.h"

//	FORWARD CLASS DECLERATIONS
class MCAMessagesWriteInterface;
class MCABufferMemoryHandler;
class MCAMessageCreator;
class MCAChatObserver;
class MCAMessageContainerInfo;

/**
 * Internal buffer ids, these must be specified so that they can't appear in
 * wireless village ids. And they must also be non-matchable to KNullDesC when
 * comparing with neutral compare.
 *
 * From WV_CSP_v1.1.pdf:
 *   As per URI [RFC2396], certain reserved characters must be escaped if they
 *   occur within the User-ID, Resource, or Domain portions of a Wireless
 *   Village address. This includes the characters
 *   ";", "?", ":", "&", "=", "+", "$" and ",".
 *
 * When using neutral compare, atleast ";", "?", ":", "&", "=", "+", "#" and ","
 * are invalid.
 */
_LIT( KSendBufferId, "$" );
_LIT( KLoggerBufferId, "$$" );

// CLASS DECLARATION

/**
 *  Interface for getting access to message containers.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAChatInterface
    {
    public: // Interface

        /**
         * Provide access to chat messages. If messageContainer does not exists
         * creates one.
         * @param aServerAddress Part of identifier to identify container.
         * @param aUserId Part of identifier to identify container.
         * @param aTargetId Part of identifier to identify container.
         * @param aType. Preferred type of container. If ENoneContainer,
         *               resolved by container.
         * @return Read interface to container
         */
        virtual MCAMessagesReadInterface& MessageReadInterfaceL(
            const TDesC& aServerAddress,
            const TDesC& aUserId,
            const TDesC& aTargetId = KSendBufferId,
            MCAMessagesReadInterface::TContainerType aType =
                MCAMessagesReadInterface::ENoneContainer ) = 0;

        /**
         * Provide access to append chat messages.
         * If messageContainer does not exists, creates one.
         * @param aServerAddress Part of identifier to identify container.
         * @param aUserId Part of identifier to identify container.
         * @param aTargetId Part of identifier to identify container.
         * @param aType. Preferred type of container. If ENoneContainer,
         *               resolved by container.
         * @return Write interface to container
         */
        virtual MCAMessagesWriteInterface& MessageWriteInterfaceL(
            const TDesC& aServerAddress,
            const TDesC& aUserId,
            const TDesC& aTargetId = KSendBufferId,
            MCAMessagesReadInterface::TContainerType aType =
                MCAMessagesReadInterface::ENoneContainer ) = 0;

        /**
         * Delete identified chat container
         * @param aServerAddress Part of identifier to identify container.
         * @param aUserId Part of identifier to identify container.
         * @param aTargetId Part of identifier to identify container.
         */
        virtual void DeleteChatL( const TDesC& aServerAddress,
                                  const TDesC& aUserId,
                                  const TDesC& aTargetId = KSendBufferId ) = 0;


        /**
         * Close all open containers.
         */
        virtual void CloseAllContainers() = 0;

        /**
         * Check if chat exists or not.
         * @param aServerAddress Part of identifier to identify container.
         * @param aUserId Part of identifier to identify container.
         * @param aTargetId Part of identifier to identify container.
         * @return Read interface if chat exists, NULL if not.
         */
        virtual MCAMessagesReadInterface* ChatExistsL(
            const TDesC& aServerAddress,
            const TDesC& aUserId,
            const TDesC& aTargetId = KSendBufferId ) = 0;

        /**
         * Change identifier of chat. Also type of container can be changed. If
         * old does not exists, creates new one.
         * @param aOldServerAddress Part of old identifier to identify container.
         * @param aOldUserId Part of old identifier to identify container.
         * @param aOldTargetId Part of old identifier to identify container.
         * @param aServerAddress Part of new identifier to identify container.
         * @param aUserId Part of new identifier to identify container.
         * @param aTargetId Part of new identifier to identify container.
         * @param aType. Preferred type of container. If ENoneContainer,
         *               Resolved by container.
         * @return Read interface to container
         */
        virtual MCAMessagesReadInterface& ChangeChatIdL(
            const TDesC& aOldServerAddress,
            const TDesC& aOldUserId,
            const TDesC& aOldTargetId,
            const TDesC& aNewServerAddress,
            const TDesC& aNewUserId,
            const TDesC& aNewTargetId = KSendBufferId,
            MCAMessagesReadInterface::TContainerType aType =
                MCAMessagesReadInterface::ENoneContainer  ) = 0;

        /**
         * Get count of group chats.
         * @return count of group chats.
         */
        virtual TInt GroupCount() const = 0;

        /**
         * Get count of ptop chats.
         * @return Count of ptop chats.
         */
        virtual TInt ChatCount() const = 0;

        /**
         * Get group chat
         * @param aIndex, Index of chat to get.
         * @return Read interface based on index.
         */
        virtual MCAMessagesReadInterface& GroupAt( TInt aIndex ) const = 0;

        /**
         * Get ptop chat
         * @param aIndex, Index of chat to get.
         * @return Read interface based on index.
         */
        virtual MCAMessagesReadInterface& ChatAt( TInt aIndex ) const = 0;

        /**
         * Register observer for chat events. Remember to unregister
         * when not needed
         * @param aObserver Observer which is registered.
         * @return KErrNone if succeed.
         */
        virtual TInt RegisterChatObserver( MCAChatObserver* aObserver ) = 0;

        /**
         * Unregister observer for chat events.
         * @param aObserver Observer which is unregistered
         * @return KErrNone if found and unregistered.
         */
        virtual TInt UnregisterChatObserver( MCAChatObserver* aObserver ) = 0;

        /**
         * Reset count of pending messages
         */
        virtual void ResetPendingCount() = 0;

        /**
         * Count of pending messages
         * @param aCountOfChats. Count of different chats having pending
         *                       messages
         * @param aUnreadFilter Count all messages or just received messages
         * @return count of messages.
         */
        virtual TInt MessagesPendingCount( TInt& aCountOfChats,
                                           MCAMessagesReadInterface::TUnreadFilter aUnreadFilter =
                                               MCAMessagesReadInterface::EUnreadAll ) const = 0;

        /**
         * First pending message containers info.
         * @return First pending message containers info.
         */
        virtual MCAMessageContainerInfo* PendingMessageInfo() const = 0;

#ifdef RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
        /**
         * Count of unread chat group messages.
         * @param aCountOfChats. Count of different chats having pending
         *                       messages
         * @since S60 v3.2
         * @return count of messages.
         */
        virtual TInt ChatGroupMessagesPendingCount( TInt& aCountOfChats ) const = 0;

        /**
         * First pending message containers info.
         * @return First pending message containers info of group Chat.
         */
        virtual MCAMessageContainerInfo* ChatGroupPendingMessageInfo() const = 0 ;

#endif  // RD_CHAT_GROUP_MESSAGE_INDICATION_NEW

    };

#endif      // MCACHATINTERFACE_H

// End of File
