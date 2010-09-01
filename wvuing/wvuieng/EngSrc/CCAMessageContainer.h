/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for messages.
*
*/


#ifndef __CCAMESSAGECONTAINER_H__
#define __CCAMESSAGECONTAINER_H__

#include "MCAMessagesReadInterface.h"
#include "MCAMessagesWriteInterface.h"
#include "MCAMessageContainerInfo.h"
#include "MCAContentProcessObserver.h"

//	FORWARD CLASS DECLERATIONS
class MCABufferMemoryHandler;
class MCAChatObserver;
class MCALoggerWriteInterface;
class MCALoggerMessage;

// CLASS DECLARATION

/**
 *  Container class for messages.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */

class CCAMessageContainer : public CBase,
            public MCAMessagesReadInterface,
            public MCAMessagesWriteInterface,
            public MCAMessageContainerInfo,
            public MCAContentProcessObserver
    {
    public: // Construction

        /**
         * Construction
         * @param aIdentifier, Identifier of container
         * @param aMemoryHandler Handles memory
         * @param aType Type of container. EGroupContainer, EChatContainer
         * @return Pointer to new created instance of CCAMessageContainer
         */
        static CCAMessageContainer* NewL( const TDesC& aServerAddress,
                                          const TDesC& aUserId,
                                          const TDesC& aTargetId,
                                          MCABufferMemoryHandler& aMemoryHandler,
                                          TContainerType aType );

        /**
         * Construction
         * @param aIdentifier, Identifier of container
         * @param aMemoryHandler Handles memory
         * @param aType Type of container. EGroupContainer, EChatContainer
         * @return Pointer to new created instance of CCAMessageContainer
         */
        static CCAMessageContainer* NewLC( const TDesC& aServerAddress,
                                           const TDesC& aUserId,
                                           const TDesC& aTargetId,
                                           MCABufferMemoryHandler& aMemoryHandler,
                                           TContainerType aType  );

        /**
         * Destruction
         */
        ~CCAMessageContainer();

    private: // Construction

        /**
         * Constructor
         * @param aMemoryHandler Handles memory
         */
        CCAMessageContainer( MCABufferMemoryHandler& aMemoryHandler );

        /**
         * Constructor
         * @param aIdentifier, Identifier of container
         * @param aType Type of container. EGroupContainer, EChatContainer
         */
        void ConstructL( const TDesC& aServerAddress,
                         const TDesC& aUserId,
                         const TDesC& aTargetId,
                         TContainerType aType );

    private: // Methods from MCAMessagesReadInterface

        /**
         * @see MCAMessagesReadInterface
         */
        TInt UnreadCount( TUnreadFilter aUnreadFilter = EUnreadAll ) const;

        /**
         * @see MCAMessagesReadInterface
         */
        TInt MessageCount() const;

        /**
         * @see MCAMessagesReadInterface
         */
        void Read( TReadType aReadType, TInt aReadAmount = 0 );

        /**
         * @see MCAMessagesReadInterface
         */
        MCAMessage& Message( TInt aIndex ) const;

        /**
         * @see MCAMessagesReadInterface
         */
        MCAMessage& ReadNextUnread();

        /**
         * @see MCAMessagesReadInterface
         */
        MCAMessage& ReadUnreadFromIndex( TInt aIndex );

        /**
         * @see MCAMessagesReadInterface
         */
        void SetObserver( MCAMessageObserver* aObserver );

        /**
         * @see MCAMessagesReadInterface
         */
        TPtrC Name() const;

        /**
         * @see MCAMessagesReadInterface
         */
        void DeleteMessage( TInt aIndex );

        /**
         * @see MCAMessagesReadInterface
         */
        void StartLoggingL( const TDesC& aLogFilename );

        /**
         * @see MCAMessagesReadInterface
         */
        void EndLoggingL();

        /**
         * @see MCAMessagesReadInterface
         */
        TBool IsLoggingOn() const;

        /**
         * @see MCAMessagesReadInterface
         */
        TInt FindIndex( TInt aOperationCode ) const;

        /**
         * @see MCAMessagesReadInterface
         */
        TInt FindIndex( MCAMessage& aMessage ) const;

        /**
         * @see MCAMessagesReadInterface
         */
        MCAMessageContainerInfo& TargetId();

        /**
         * @see MCAMessagesReadInterface
         */
        void Lock( TBool aLocked );

        //fix - refer Ui Spec Approved Version 1.0
        //(Instant Messaging NG 001 151006.pdf)
        //Section 10.2.10 Pg 131 -
        //"In case user has saved the image already or sent it
        //by himself, this option(save) is not available."

        /**
         * Added newly - Plz dont remove while merging
         * @see MCAMessagesReadInterface
         */
        void ReLoggingL( MCALoggerMessageHeader& aFile );

        /**
         * Added newly - Plz dont remove while merging
         * @see MCAMessagesReadInterface
         */
        void EndReLoggingL();

    private: // Methods from MCAMessagesWriteInterface

        /**
         * @see MCAMessagesWriteInterface
         */
        void AppendL( MCAMessage* aMessage, TBool aSharedOwnership );

        /**
         * @see MCAMessagesWriteInterface
         */
        void SetScreenNameL( const TDesC& aScreenName );

        /**
         * @see MCAMessagesWriteInterface
         */
        const TDesC& OwnScreenName() const;

        /**
         * @see MCAMessagesWriteInterface
         */
        void LockBufferMemoryHandling( TBool aLocked );

        /**
         * @see MCAMessagesWriteInterface
         */
        TTime Time();

    private: // From MCAMessageContainerInfo

        /**
         * @see MCAMessageContainerInfo
         */
        const TDesC& ServerAddress() const;

        /**
         * @see MCAMessageContainerInfo
         */
        const TDesC& UserId() const;

        /**
         * @see MCAMessageContainerInfo
         */
        const TDesC& Target() const;

        /**
         * @see MCAMessageContainerInfo
         */
        const TDesC& ScreenName() const;

        /**
         * @see MCAMessageContainerInfo
         */
        void SetAddToContactAsked( TBool aIsAsked );

        /**
         * @see MCAMessageContainerInfo
         */
        TBool IsAddToContactAsked() const;

    private: // From MCAContentProcessObserver

        /**
         * @see MCAContentProcessObserver
         */
        void HandleProcessingComplete( MCAContentProcessor& aProcessor,
                                       MCAMessage& aMessage, TInt aStatus );

    public: // New methods

        /**
         * Ordering function for ordering Message containers
         * @param aSourceA First compare item.
         * @param aSourceB Second compare item.
         * @return -1, 0 or +1, like CompareC.
         */
        static TInt OrderUid( const CCAMessageContainer& aSourceA,
                              const CCAMessageContainer& aSourceB );

        /**
         * Type of container
         * @return Type of container. EGroupContainer, EChatContainer
         */
        TContainerType ContainerType() const;

        /**
         * Change identifier
         * @param aNewId, new identifier
         * @param aType, new type. If ENoneContainer, Resolved automaticly.
         */
        void ChangeIdL( const TDesC& aServerAddress,
                        const TDesC& aUserId,
                        const TDesC& aTargetId,
                        MCAMessagesReadInterface::TContainerType aType );

        /**
         * Get recipient of message identified by operation code.
         * aStatus updated to KErrNone if success. KErrNotFound if not.
         * @param aOperationCode, Identifier for getting right message.
         * @param aStatus. Status of operation. KErrNone/KErrNotFound
         * @return Message recipient
         */
        const TDesC& MessageRecipient( TInt aOperationCode, TInt& aStatus ) const;

        /**
         * Set chat observer.
         * @param aObserver. Observer to set.
         */
        void SetChatObserver( MCAChatObserver* aObserver );

    private: // New helper methods

        /**
         * Resolve type of container by identifier.
         * @param aType, new type. If ENoneContainer, Resolved automaticly.
         */
        void ResolveType( TContainerType aType );

        /**
         * Make log.
         * @param aMessage Message to log.
         */
        void MakeLogL( MCALoggerMessage& aMessage );

        /**
         * Notify contact for pending count change.
         */
        void NotifyContactForPending() const;

        /**
         * Returns target id to be used for
         * comparisons.
         * @since S60 v3.1
         * @return Target id.
         */
        const TDesC& ComparableTarget() const;

    public:

        /**
         * Get count of all messages. Unread and read.
         * @return Count of messages
         */
        TInt AllMessagesCount() const;

        /**
         * Access to any message. Also unread.
         * @param aIndex Index of message
         * @return Reference to message.
         */
        MCAMessage& MessageFromAll( TInt aIndex ) const;

        /**
         * Delete from all messages. Unread and read messages available.
         * @param aIndex. Index of message.
         */
        void DeleteMessageFromAll( TInt aIndex );

        /**
         * Is container locked or not. ETrue locked, EFalse not.
         * @return Locked status.
         */
        TBool IsLocked() const;

        /**
         * Sizes of messages in container in bytes.
         * @since S60 v3.1
         * @return Size in bytes.
         */
        TInt ContainerSizeInBytes() const;

    private: // Member data

        /// Chat group id / conversation party id
        HBufC* iServerAddress;
        HBufC* iUserId;
        HBufC* iTargetId;
        HBufC* iDecodedTargetId;

        /// Screen name of user corresponding the container.
        HBufC* iScreenName;

        /// Offset to last unread message
        TInt iUnreadOffset;

        /// Array of messages.
        RPointerArray< MCAMessage > iMessageContainer;

        /// Reference to memory handler
        MCABufferMemoryHandler& iMemoryHandler;

        /// Observer class
        MCAMessageObserver* iObserver;

        /// Type of container
        TContainerType iContainerType;

        /// Not own. Chat observer
        MCAChatObserver* iChatObserver;

        // Owns. Write interface for logging.
        MCALoggerWriteInterface* iLogger;

        // Tells if add to contacts is asked
        TBool iAddToContactAsked;

        // Locked status
        TBool iLocked;

        // Buffer memory handling locked
        TBool iMemoryHandlingLocked;

        // Size of contained messages in bytes
        TInt iContainerSizeInBytes;
    };

#endif      // __CCAMESSAGECONTAINER_H__

// End of File
