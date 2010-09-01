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
* Description:  Interface for getting access to messages.
*
*/


#ifndef __MCAMESSAGESREADINTERFACE_H__
#define __MCAMESSAGESREADINTERFACE_H__

#include <e32base.h>

//	FORWARD CLASS DECLERATIONS
class MCAMessage;
class MCAMessageObserver;
class MCAMessageContainerInfo;
class MCALoggerMessageHeader;

// CLASS DECLARATION

/**
 *  Interface for getting access to messages.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessagesReadInterface
    {

    public: // Definitions

        /**
         * How many messages we want to change from unread to read.
         */
        enum TReadType
            {
            EReadAll,
            EReadAmount,
            EReadOne,
            EReadInvalid
            };

        /**
         * Type of container
         */
        enum TContainerType
            {
            ENoneContainer,
            EGroupContainer,
            EChatContainer,
            ESendContainer
            };

        /**
         *
         */
        enum TUnreadFilter
            {
            EUnreadAll,
            EUnreadReceived
            };

    public: // Interface

        /**
         * Get count of unread messages.
         * @param aUnreadFilter Show only received messages as unread or all.
         * @return Amount of unread messages.
         */
        virtual TInt UnreadCount( TUnreadFilter aUnreadFilter = EUnreadAll ) const = 0;

        /**
         * Get count of readable messages.
         * @return Amount of readable messages. Does not include unread messages.
         */
        virtual TInt MessageCount() const = 0;

        /**
         * Move unread messages to read.
         * @param aReadType Type of read. EReadAll, EReadAmount, EReadOne
         * @param aReadAmount Amount of messages to move from unread to read.
         * 		  Default 0, Needed if TReadAmount is used.
         */
        virtual void Read( TReadType aReadType, TInt aReadAmount = 0 ) = 0;

        /**
         * Get message.
         * @param aIndex, Index of message in messagearray.
         * @return Reference to message.
         */
        virtual MCAMessage& Message( TInt aIndex ) const = 0;

        /**
         * Get next unread message
         * @return Reference to next unread message.
         */
        virtual MCAMessage& ReadNextUnread() = 0;

        /**
         * Read one unread message from index aIndex.
         * @since S60 v3.2
         * @param aIndex Index to wanted message.
         * @return Reference to last unread message.
         */
        virtual MCAMessage& ReadUnreadFromIndex( TInt aIndex ) = 0;

        /**
         * Set observer for observing changes in messages container.
         * Observer can handle different situations like e.g.
         * containers destruction, messages arrival etc.
         * @param aObserver Observer class to get notifications.
         * 		  Uninstall observer with passing NULL as parameter.
         */
        virtual void SetObserver( MCAMessageObserver* aObserver ) = 0;

        /**
         * Get name of chat.
         * @return Name of chat.
         */
        virtual TPtrC Name() const = 0;

        /**
         * Delete message in index.
         * @param aIndex, Index of message to be deleted.
         * @param aFreeMemory, ETrue if message should be deleted. EFalse if ownership of message
         * is transferred to caller. Remember to achieve
         * @return Pointer to message if not ownership is tranferred to caller. NULL if not.
         */
        virtual void DeleteMessage( TInt aIndex ) = 0;

        /**
         * Starts logging in a specified group.
         * @param aLogFileName is the file name which will be shown in the
         *        first line of the notepad memo (after end logging).
         */
        virtual void StartLoggingL( const TDesC& aLogFilename ) = 0;

        /**
         * Ends the logging in a specified group.
         */
        virtual void EndLoggingL() = 0;

        /**
         * Tests whether the logging is on or not.
         * @return ETrue if on EFalse if not.
         */
        virtual TBool IsLoggingOn() const = 0;

        /**
         * Return index of message identified by operation code.
         * @param aOperationCode Operation code to identify message
         * @return Index of message. KErrNotFound, if not found.
         */
        virtual TInt FindIndex( TInt aOperationCode ) const = 0;

        /**
         * Return index of message identified by operation code.
         * @param aMessage Message which index wanted to find.
         * @return Index of message. KErrNotFound, if not found.
         */
        virtual TInt FindIndex( MCAMessage& aMessage ) const = 0;

        /**
         * Id information of message container.
         * @return Container information.
         */
        virtual MCAMessageContainerInfo& TargetId() = 0;

        /**
         * Lock/unlock messages from deletion
         * @param aLocked, ETrue locks, EFalse unlocks.
         */
        virtual void Lock( TBool aLocked ) = 0;


        //fix - refer Ui Spec Approved Version 1.0
        //(Instant Messaging NG 001 151006.pdf)
        //Section 10.2.10 Pg 131 -
        //"In case user has saved the image already or sent it
        //by himself, this option(save) is not available."

        /**
         * Note : Please dont delete while merging the wvui20 changes
         * Relogs the file and saves all the messages
         * only if the file has become dirty.
         * New implementation - Used if user saves an image
         * in saved conversations that info is stored back on
         * to the file and subsequently we dont allow resaving of the
         * same image

        /**
         * Starts relogging. Sets logging on.
         * @since 3.2
         * @param aFile Header information.
         */
        virtual void ReLoggingL( MCALoggerMessageHeader& aFile ) = 0;

        /**
         *	Ends Relogging. Close the file.
         *	Sets logging off.
         *	@since 3.2
         */
        virtual void EndReLoggingL() = 0;

    protected: // For protection
        /**
         * Destructor for protection
         */
        virtual ~MCAMessagesReadInterface() {}
    };

#endif      // __MCAMESSAGESREADINTERFACE_H__

// End of File
