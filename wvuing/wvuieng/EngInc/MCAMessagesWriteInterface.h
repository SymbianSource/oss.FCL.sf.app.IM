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
* Description:  Interface for write access to messages.
*
*/


#ifndef __MCAMESSAGESWRITEINTERFACE_H__
#define __MCAMESSAGESWRITEINTERFACE_H__

//	FORWARD CLASS DECLERATIONS
class MCAMessage;

// CLASS DECLARATION

/**
 *  Interface for write access to messages.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessagesWriteInterface
    {
    public: // Interface

        /**
         * Append one message to container.
         * @param aMessage. New message to container.
         * @param aSharedOwnerShip EFalse if ownership is tranferred, ETrue if shared.
         */
        virtual void AppendL( MCAMessage* aMessage, TBool aSharedOwnership = EFalse ) = 0;

        /**
         * Set screenname corresponding the container if needed.
         * @param aScreenName Screen name of user.
         */
        virtual void SetScreenNameL( const TDesC& aScreenName ) = 0;

        /**
         * Get screenname corresponding the container.
         * @return Own screenname in group.
         */
        virtual const TDesC& OwnScreenName() const = 0;

        /**
         * With this we can lock buffer memory handling for not
         * to free memory if memory runs out. Used for example
         * when opening recorded chats when we do not want
         * to clear active messages because there is not
         * enough memory to open recorded chats.
         */
        virtual void LockBufferMemoryHandling( TBool aLocked ) = 0;

        /**
         * Get the time and date of the last message written to this interface,
         * Time is used to determine the need of datestamp.
         * @return Date and time of the last message. if there are no
         *         previous messages, then returns Time::NullTTime()
         */
        virtual TTime Time() = 0;

        /**
         * Virtual destructor
         */
        virtual ~MCAMessagesWriteInterface() {}
    };

#endif      // __MCAMESSAGESWRITEINTERFACE_H__

// End of File
