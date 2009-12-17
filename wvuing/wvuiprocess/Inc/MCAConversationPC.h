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
* Description:  Interface for Conversation Process Component
*
*/



#ifndef MCACONVERSATIONPC_H
#define MCACONVERSATIONPC_H

// INCLUDES
#include <e32std.h>
#include <badesca.h>

#include "MCAMessageRWInterfacePC.h"
#include "TEnumsPC.h"

class MCAConversationObserverPC;
class MCAMessage;
class MCAMessagesReadInterface;
class MCAMessagesWriteInterface;
class MCAMessageUtils;
class MCAServerContactsArrayPC;

// CLASS DECLARATION

/**
 *  Interface for conversations
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */
class MCAConversationPC
    {
    public: // Interface


        /**
        * AddConversationContactsObserverL: To add the UI component as an observer
        *  to listen to the contact related events
        * @param MCAConversationChatObserverPC - Ptr to the object to be added
        */
        virtual void AddObserver( MCAConversationObserverPC* aObserver ) = 0;


        /**
        * RemoveConversationObserver  : To remove the added observer
        */
        virtual void RemoveObserver() = 0;



        /**
         * Delete identified chat container
         * @param aServerAddress Part of identifier to identify container.
         * @param aUserId Part of identifier to identify container.
         * @param aTargetId Part of identifier to identify container.
         */
        virtual void DeleteChatL( const TDesC& aServerAddress,
                                  const TDesC& aUserId,
                                  const TDesC& aTargetId ) = 0;


        /**
         * InitialiseResourcesL
         * @param aContactId - the current conversation wvid
         */
        virtual void InitialiseResourcesL( const TDesC& aContactId ) = 0;

        /**
         * ReleaseResourcesL
         *
         */
        virtual void ReleaseResources() = 0;


        /**
         * GetIdentification
         * @param aContactId - wvid
         * @return Identification of contact.
         *      Alias if nickname is not set and alias is enabled.
         *      Wvid without domain and prefix if nickname does not
                exists and alias is not enabled or not exists.
         */
        virtual const TPtrC GetIdentification( const TDesC& aContactId ) = 0;


        /**
         * Part of container identifier which contains this message
         * @return Identifier to identify container which contains this message
         */
        virtual const TDesC& GetReadInterfaceId()	= 0;


        /**
         * GetLogFileName
         * @param ref to TPtr - This will be set within this function and
         *						returned to the caller
         */
        virtual void GetLogFileName( TPtr& aLogFileNamePtr ) = 0;

        /**
         * Starts logging in a specified group.
         * @param aLogFileName is the file name which will be shown in the
         *        first line of the notepad memo (after end logging).
         */
        virtual void RecordChatL( const TDesC& aLogFilename ) = 0;


        /**
         * SendMessageL
         * @param aMessageBuffer.
         */
        virtual void SendMessageL( const TDesC& aMessageBuffer ) = 0;

        /**
         * PopulateGroupDetailsLC
         * @return - Fills and returns groupname along with the ID
         * Ownership is transferred to the caller
         */
        virtual MCAServerContactsArrayPC* PopulateGroupDetailsLC() = 0;


        /**
         * Get name of chat.
         * @return Name of chat.
         */
        virtual TPtrC GetReadInterfaceName() = 0;


        /**
         * Set the current conversation
         * @param aIndex of the chat in the local list.
         */
        virtual void SetActiveConversationL( const TDesC& aWvId ) = 0;

        /**
         * Find any contact which id is aContactId
         * @param aContactId Contactid to recognize contact.
         * @return ETrue if found else EFalse
         */
        virtual TBool FindAnyContact( const TDesC& aContactId ) = 0;

        /**
         *  Count of stored groups
         *  @return Count of groups
         */
        virtual TInt GroupCount( TBool aOnlyVisible ) = 0;

        /**
         * Count of stored contacts
         * @return Count of contacts
         */
        virtual TInt ContactCount() = 0;

        /**
         * CheckReply
         * @param aWvid - contact id
         * @param aContactInlist - also checks whether the contact is in contact list
         */
        virtual void CheckReply( TPtrC& aWvid, TBool& aContactInlist ) = 0;



        /**
         * IsActiveConversation
         * @param aContactId - contact id
         * @return ETrue - if the passed contactID is the current active conversation
         *         EFalse - Otherwise
         */
        virtual TBool IsActiveConversation( const TDesC& aContactId ) = 0;

        /**
         * SetMessageReadInterfaceL: Calls the MessageReadInterface() of engine component
         * to set the read interface
         * @param aWvId - wv user id
         * @return void
         */
        virtual void SetMessageReadInterfaceL( const TDesC& aWvId ) = 0;

        /**
         * SetMessageWriteInterfaceL: Calls the MessageWriteInterface() of engine component
         * to set the write interface
         * @return void
         */
        virtual void SetMessageWriteInterfaceL() = 0;


        /**
         * Closes all open conversations
         */
        virtual void CloseConversations() = 0;

        /**
         * Leaves every joined group.
         */
        virtual void LeaveJoinedGroupsL() = 0;

        /**
         * Gets the message count from ReadInterface
         */
        virtual TInt MessageCount() = 0;

        /**
         * Destructor
         */
        virtual ~MCAConversationPC() {}


    };


#endif // MCACONVERSATIONPC_H

// End of File