/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Offers information regarding a given header to the UI.
*
*/


#ifndef __MCALOGGERHEADERSINTERFACE_H__
#define __MCALOGGERHEADERSINTERFACE_H__

// INCLUDES
#include    "PublicEngineDefinitions.h"

#include    <e32std.h>

// FORWARD DECLARATIONS
class RWriteStream;
class MCAMessagesReadInterface;
class MCALoggerMessageHeader;

// CLASS DECLARATION

/**
*  Interface class for UI->Engine communication ( UI retrieves information ).
*  Offers information regarding a given header to the UI.
*
*  @lib CAEngine.lib
*  @since 2.1
*/
class MCALoggerHeadersInterface
    {
    public: // New functions

        /**
        * Returns the number data items ( headers count ).
        * @return Number of data items.
        * @since 2.5
        */
        virtual TInt HeaderCount() const = 0;

        /**
        * Returns name of recorded chat
        * @param aIndex is index of recorded chat
        * @return Reference to name of recorded chat
        */
        virtual const TDesC& Name( TInt aIndex ) const = 0;

        /**
        * Returns the type of the chat data.
        * @param aIndex is the index to the header array.
        * @return Message type.
        */
        virtual TChatType ChatType( TInt aIndex ) const = 0;

        /**
        * Returns time when history save started.
        * @param aIndex is the index to the header array.
        * @return Start time
        */
        virtual TTime StartTime( TInt aIndex ) const = 0;

        /**
        * Returns time when history save ended.
        * @param aIndex is the header index.
        * @return End time.
        */
        virtual TTime EndTime( TInt aIndex ) const = 0;

        /**
         * Returns identification information of history data
         * @param aIndex is the header index
         * @return Identication information of history data.
         */
        virtual const TDesC& Identification( TInt aIndex ) const = 0;

        /**
         *	Delete one history file recognized by index.
         *	@since 2.5
         *	@param aIndex Index of file to be deleted from db.
         */
        virtual void DeleteHistoryFileL( TInt aIndex  ) = 0;

        /**
         * Get message for sending it via MTM
         * @param aIndex Index of message to send
         * @param aStream Stream where message is written.
         * @since 2.5
         */
        virtual void GetMessageForSendingViaMTML(
            TInt aIndex, RWriteStream& aStream ) = 0;

        /**
         * Interface to access messages
         * @param aIndex Index of header whichs messages are accessed with interface
         * @return Access to messages.
         * @since 2.5
         */
        virtual MCAMessagesReadInterface& MessagesInterfaceL( TInt aIndex ) = 0;

        /**
         * Deletes message container of current recorded chat
         * from engine. Used to free memory when exiting current
         * recorded chat view i.e. called from DoDeactivate method
         * of view class.
         * @since 3.1
         */
        virtual void DeleteRecordedChatContainerL() = 0;

        /**
         * Finds index of recorded chat by name.
         * @since S60 v3.2
         * @param aName Name of recorded chat.
         * @return Index of chat or KErrNotFound if
         *         chat is not found.
         */
        virtual TInt Index( const TDesC& aName ) const = 0;

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
         * @since S60 v3.2
         * @param aIndex Index of header whichs messages are accessed with interface
         * @return void
         */
        virtual void ReLoggingL( TInt aIndex ) = 0;
    public:

        /**
         *	Destructor
         */
        virtual ~MCALoggerHeadersInterface() {};
    };

#endif      // __MCALOGGERHEADERSINTERFACE_H__

// End of File
