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
* Description:  Utils for message handling
*
*/



#ifndef IMMESSAGEUTILSPC_H
#define IMMESSAGEUTILSPC_H

//  INCLUDES
#include    <e32base.h>
#include    <apgcli.h>             // RApaLsSession

// FORWARD DECLARATIONS

class MCAConversationPC;
class MCAConversationMessage;
class MCARecordedChatsPC;
class MCAMessageContainer;
class MCAMessageFlowHandlerPC;
class MCAGroupPC;



// CLASS DECLARATION

/**
*  Utils for message handling
*
*  @lib wvuiprocessng.lib
*  @since 3.2
*/
class IMMessageUtilsPC
    {
    public:

        IMPORT_C static void SendFileL(
            MCAConversationPC& aMessageRWInterfacePC,
            const MDesCArray* aSelectedNames,
            const TBool aFileProtected,
            const TDesC& aFileName,
            const TDesC8& aContentType,
            const TDesC8& aContentData,
            const TDesC& aSender = KNullDesC,
            const TDesC& aSapId = KNullDesC,
            const TDesC& aUserId = KNullDesC,
            const MDesCArray* aRecipients = NULL ) ;

        IMPORT_C static TBool SendMessageL(
            const MCAConversationMessage& aMessage,
            const TDesC& aRecipient,
            MCAConversationPC& aMessageRWInterfacePC,
            const MDesCArray* aSelectedNames,
            const TDesC& aSender = KNullDesC,
            const TDesC& aSapId = KNullDesC,
            const TDesC& aUserId = KNullDesC,
            const TDesC& aFileName = KNullDesC,
            const MDesCArray* aRecipients = NULL ) ;

        /**
         * Create a clone from source message
         * @param aFwdSource, source message
         * @return New cloned message - transfers ownership to the caller
         */
        IMPORT_C static MCAConversationMessage* CloneMessageL(
            const MCAConversationMessage& aFwdSource );


        /**
         * Creates and returns the instance to Message Flow handler
         * The caller has to free this
         * @param aMessageContainer - ref to message container
         * @param aConversationPC - ref to conversation PC
         * @param aRecordedChatHandler Is this handler for recorded chat
         *                             or not.
         */
        IMPORT_C static MCAMessageFlowHandlerPC* CreateInstanceL(
            MCAMessageContainer& aMessageContainer,
            MCAConversationPC& aConversationPC,
            TBool aRecordedChatHandler );


        /**
        * Creates and returns the instance to Message Flow handler
        * The caller has to free this
        * @param aMessageContainer - ref to message container
        * @param aGroupPC - ref to group PC
        * @param aRecordedChatHandler Is this handler for recorded chat
        *                             or not.
        */
        IMPORT_C static MCAMessageFlowHandlerPC* CreateInstanceL(
            MCAMessageContainer& aMessageContainer,
            MCAGroupPC& aGroupPC,
            TBool aRecordedChatHandler );
        /**
         * Creates and returns the instance to Message Flow handler
         * The caller has to free this
         * @param aMessageContainer - ref to message container
         * @param aRecordedChatsPC - ref to RecordedChats PC
         * @param aRecordedChatHandler Is this handler for recorded chat
         *                             or not.
         */
        IMPORT_C static MCAMessageFlowHandlerPC* CreateInstanceL(
            MCAMessageContainer& aMessageContainer,
            MCARecordedChatsPC& aRecordedChatsPC,
            TBool aRecordedChatHandler );

        IMPORT_C static void SendFileL(
            MCAGroupPC& aMessageRWInterfacePC,
            const MDesCArray* aSelectedNames,
            const TBool aFileProtected,
            const TDesC& aFileName,
            const TDesC8& aContentType,
            const TDesC8& aContentData,
            const TDesC& aSender = KNullDesC,
            const TDesC& aSapId = KNullDesC,
            const TDesC& aUserId = KNullDesC,
            const MDesCArray* aRecipients = NULL ) ;

        IMPORT_C static TBool SendMessageL(
            const MCAConversationMessage& aMessage,
            const TDesC& aRecipient,
            MCAGroupPC& aMessageRWInterfacePC,
            const MDesCArray* aSelectedNames,
            const TDesC& aSender = KNullDesC,
            const TDesC& aSapId = KNullDesC,
            const TDesC& aUserId = KNullDesC,
            const TDesC& aFileName = KNullDesC,
            const MDesCArray* aRecipients = NULL ) ;

    };

#endif      // IMMESSAGEUTILSPC_H

// End of File
