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



// INCLUDE FILES
#include    "IMMessageUtilsPC.h"
#include	"ChatDebugPrint.h"

#include    "imutils.h"

#include    "MCAMessagesWriteInterface.h"
#include    "MCAMessagesReadInterface.h"

#include    "MCAMessageCreator.h"
#include    "MCAMessageContainerInfo.h"

#include    "impsbuilddefinitions.h"

#include	"CCAConversationPC.h"
#include	"CCAEngine.h"
#include 	"MCAProcessManager.h"
#include 	"CCAProcessManagerFactory.h"
#include	"MCAMessage.h"
#include	"MCAConversationMessage.h"
#include	"CCAConversationMessage.h"

#include	"MCARecordedChatsPC.h"
#include	"CCARecordedChatsPC.h"
#include	"MCAMessageContainer.h"
#include	"CCAGroupPC.h"

#include	"MCAMessageFlowHandlerPC.h"
#include	"CCAMessageFlowHandlerPC.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// IMMessageUtilsPC: SendFileL
// -----------------------------------------------------------------------------
//
EXPORT_C void IMMessageUtilsPC::SendFileL(
    MCAConversationPC& aMessageRWInterfacePC,
    const MDesCArray* aSelectedNames,
    const TBool aFileProtected,
    const TDesC& aFileName,
    const TDesC8& aContentType,
    const TDesC8& aContentData,
    const TDesC& aSender /*= KNullDesC*/,
    const TDesC& aSapId /*= KNullDesC*/,
    const TDesC& aUserId /*= KNullDesC*/,
    const MDesCArray* aRecipients /*NULL*/ )
    {

    CCAEngine* engine = CCAProcessManagerFactory::InstanceL()->
                        GetProcessManager()->GetEngine();

    MCAMessageCreator& creator =
        engine->MessageUtils().MessageCreator();

    CCAConversationPC& msgRWInterface =
        static_cast <CCAConversationPC&>( aMessageRWInterfacePC );
    const MCAMessageContainerInfo& msgContainerInfo =
        msgRWInterface.ReadInterfaceL().TargetId();



    if ( !aFileProtected )
        {

        CHAT_DP( D_CHAT_LIT( "Sending file %S.." ), &aFileName );

        MCAMessageCreator::SMessageData data =
            {
            KMessageDataVersion,
            0,
            aSapId,
            aUserId,
            aSender,
            msgContainerInfo.Target(), // Recipient
            aRecipients,
            aSelectedNames,
            aFileName,
            aContentType,
            aContentData,
            MCAMessage::EMessageSent
            };

        MCAMessage* message = creator.CreateMessageL( data );
#ifdef RD_SEND_NOT_SUPPORTED_CONTENT
        if ( err != KErrNone )
            {
            message->SetProcessState( MCAMessage::EContentCorrupted );
            }
#endif //RD_SEND_NOT_SUPPORTED_CONTENT
        // Lock write interface buffer memory handling for sending
        // file, memory check is done when message is appended to
        // read container after possible image processing.
        msgRWInterface.WriteInterfaceL().LockBufferMemoryHandling( ETrue );
        msgRWInterface.WriteInterfaceL().AppendL( message );
        // Open interface
        msgRWInterface.WriteInterfaceL().LockBufferMemoryHandling( EFalse );
        }
#ifdef RD_SEND_NOT_SUPPORTED_CONTENT
    else
        {

        CHAT_DP( D_CHAT_LIT( "Sending file %S.." ), &aFileName );

        MCAMessageCreator::SMessageData data =
            {
            KMessageDataVersion,
            0,
            aSapId,
            aUserId,
            aSender,
            msgContainerInfo.Target(), // Recipient
            aRecipients,
            aSelectedNames,
            aFileName,
            aContentType,
            aContentData,
            MCAMessage::EMessageSent
            };
        MCAMessage* message = creator.CreateMessageL( data );
        message->SetProcessState( MCAMessage::EContentNotSupportedDrm );
        // Lock write interface buffer memory handling for sending
        // file, memory check is done when message is appended to
        // read container after possible image processing.
        msgRWInterface.WriteInterface().LockBufferMemoryHandling( ETrue );
        msgRWInterface.WriteInterface().AppendL( message );
        // Open interface
        msgRWInterface.WriteInterface().LockBufferMemoryHandling( EFalse );
        }
#endif //RD_SEND_NOT_SUPPORTED_CONTENT        
    }




// -----------------------------------------------------------------------------
// CCAMessageUtilsPC: SendMessageL
// -----------------------------------------------------------------------------
//

EXPORT_C TBool IMMessageUtilsPC::SendMessageL(
    const MCAConversationMessage& aMessage,
    const TDesC& aRecipient,
    MCAConversationPC& aMessageRWInterfacePC,
    const MDesCArray* aSelectedNames,
    const TDesC& aSender /*= KNullDesC*/,
    const TDesC& aSapId /*= KNullDesC*/,
    const TDesC& aUserId /*= KNullDesC*/,
    const TDesC& aFileName /*= KNullDesC*/,
    const MDesCArray* aRecipients /*NULL*/ )
    {

    CCAEngine* engine =
        CCAProcessManagerFactory::InstanceL()->
        GetProcessManager()->GetEngine();


    MCAMessageCreator& creator =
        engine->MessageUtils().MessageCreator();
    CCAConversationPC& msgRWInterface =
        static_cast <CCAConversationPC&>(
            aMessageRWInterfacePC );


    MCAMessageCreator::SMessageData data =
        {
        KMessageDataVersion,
        0,
        aSapId,
        aUserId,
        aSender,
        aRecipient,
        aRecipients,
        aSelectedNames,
        aFileName,
        aMessage.MimeType(),
        aMessage.ContentData(),
        MCAMessage::EMessageSent
        };

    // Create and write
    MCAMessage* newMessage = creator.CreateMessageL( data );
    newMessage->SetProcessState( MCAMessage::EContentReady );
    msgRWInterface.WriteInterfaceL().AppendL( newMessage );

    // message handled succesfully
    return ETrue;

    }


// ---------------------------------------------------------
// IMMessageUtilsPC::CloneMessageL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C MCAConversationMessage* IMMessageUtilsPC::CloneMessageL(
    const MCAConversationMessage& aFwdSource )
    {

    MCAConversationMessage& FwdMessageTmp =
        const_cast <MCAConversationMessage&>( aFwdSource );


    CCAConversationMessage* FwdMessage =
        dynamic_cast <CCAConversationMessage*>( &FwdMessageTmp );

    if ( !FwdMessage )
        {
        return NULL;
        }

    MCAProcessManager* processManager =
        CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine&	engine ( *processManager->GetEngine() );

    MCAMessage* Msg =
        engine.MessageUtils().MessageCreator().CloneMessageL(
            &FwdMessage->GetMessage() );

    //transfer the ownership of Msg to the CCAConversationMessage
    CCAConversationMessage* NewFwdMsg = CCAConversationMessage::NewL( *Msg, Msg );

    return NewFwdMsg;

    }



// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::CreateInstanceL
//
// -----------------------------------------------------------------------------
//
EXPORT_C MCAMessageFlowHandlerPC* IMMessageUtilsPC::CreateInstanceL(
    MCAMessageContainer& aMessageContainer,
    MCAConversationPC& aConversationPC,
    TBool aRecordedChatHandler )
    {

    CCAConversationPC& conversationPC =
        *static_cast<CCAConversationPC*>( &aConversationPC );

    MCAProcessManager* processManager =
        CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine&	engine ( *processManager->GetEngine() );

    CCAMessageFlowHandlerPC* FlowHandler =
        CCAMessageFlowHandlerPC::NewL(
            aMessageContainer,
            conversationPC.ReadInterfaceL(),
            *engine.GetInternalSettingsInterface(),
            aRecordedChatHandler );

    return 	FlowHandler;
    }

// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::CreateInstanceL
//
// -----------------------------------------------------------------------------
//
EXPORT_C MCAMessageFlowHandlerPC* IMMessageUtilsPC::CreateInstanceL(
    MCAMessageContainer& aMessageContainer,
    MCAGroupPC& aGroupPC,
    TBool aRecordedChatHandler )
    {

    CCAGroupPC& groupPC =
        *static_cast<CCAGroupPC*>( &aGroupPC );

    MCAProcessManager* processManager =
        CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine&	engine ( *processManager->GetEngine() );

    CCAMessageFlowHandlerPC* FlowHandler =
        CCAMessageFlowHandlerPC::NewL(
            aMessageContainer,
            groupPC.ReadInterfaceL(),
            *engine.GetInternalSettingsInterface(),
            aRecordedChatHandler );

    return 	FlowHandler;
    }



// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::CreateInstanceL
//
// -----------------------------------------------------------------------------
//
EXPORT_C MCAMessageFlowHandlerPC* IMMessageUtilsPC::CreateInstanceL(
    MCAMessageContainer& aMessageContainer,
    MCARecordedChatsPC& aRecChatsPC,
    TBool aRecordedChatHandler )
    {

    CCARecordedChatsPC& recordedChatsPC =
        *static_cast<CCARecordedChatsPC*>( &aRecChatsPC );

    MCAProcessManager* processManager =
        CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine&	engine ( *processManager->GetEngine() );

    CCAMessageFlowHandlerPC* FlowHandler = CCAMessageFlowHandlerPC::NewL(
                                               aMessageContainer,
                                               recordedChatsPC.ReadInterfaceL(),
                                               *engine.GetInternalSettingsInterface(),
                                               aRecordedChatHandler );

    return FlowHandler;
    }

// -----------------------------------------------------------------------------
// CCAMessageUtilsPC: SendFileL
// -----------------------------------------------------------------------------
//
EXPORT_C void IMMessageUtilsPC::SendFileL(
    MCAGroupPC& aMessageRWInterfacePC,
    const MDesCArray* aSelectedNames,
    const TBool aFileProtected,
    const TDesC& aFileName,
    const TDesC8& aContentType,
    const TDesC8& aContentData,
    const TDesC& aSender /*= KNullDesC*/,
    const TDesC& aSapId /*= KNullDesC*/,
    const TDesC& aUserId /*= KNullDesC*/,
    const MDesCArray* aRecipients /*NULL*/ )
    {

    CCAEngine* engine = CCAProcessManagerFactory::InstanceL()->
                        GetProcessManager()->GetEngine();

    MCAMessageCreator& creator =
        engine->MessageUtils().MessageCreator();

    CCAGroupPC& msgRWInterface =
        static_cast <CCAGroupPC&>( aMessageRWInterfacePC );
    const MCAMessageContainerInfo& msgContainerInfo =
        msgRWInterface.ReadInterfaceL().TargetId();



    if ( !aFileProtected )
        {

        CHAT_DP( D_CHAT_LIT( "Sending file %S.." ), &aFileName );

        MCAMessageCreator::SMessageData data =
            {
            KMessageDataVersion,
            0,
            aSapId,
            aUserId,
            aSender,
            msgContainerInfo.Target(), // Recipient
            aRecipients,
            aSelectedNames,
            aFileName,
            aContentType,
            aContentData,
            MCAMessage::EMessageSent
            };

        MCAMessage* message = creator.CreateMessageL( data );
#ifdef RD_SEND_NOT_SUPPORTED_CONTENT
        if ( err != KErrNone )
            {
            message->SetProcessState( MCAMessage::EContentCorrupted );
            }
#endif //RD_SEND_NOT_SUPPORTED_CONTENT
        // Lock write interface buffer memory handling for sending
        // file, memory check is done when message is appended to
        // read container after possible image processing.
        msgRWInterface.WriteInterfaceL().LockBufferMemoryHandling( ETrue );
        msgRWInterface.WriteInterfaceL().AppendL( message );
        // Open interface
        msgRWInterface.WriteInterfaceL().LockBufferMemoryHandling( EFalse );
        }
#ifdef RD_SEND_NOT_SUPPORTED_CONTENT
    else
        {

        CHAT_DP( D_CHAT_LIT( "Sending file %S.." ), &aFileName );

        MCAMessageCreator::SMessageData data =
            {
            KMessageDataVersion,
            0,
            aSapId,
            aUserId,
            aSender,
            msgContainerInfo.Target(), // Recipient
            aRecipients,
            aSelectedNames,
            aFileName,
            aContentType,
            aContentData,
            MCAMessage::EMessageSent
            };
        MCAMessage* message = creator.CreateMessageL( data );
        message->SetProcessState(
            MCAMessage::EContentNotSupportedDrm );
        // Lock write interface buffer memory handling for sending
        // file, memory check is done when message is appended to
        // read container after possible image processing.
        msgRWInterface.WriteInterface().LockBufferMemoryHandling(
            ETrue );
        msgRWInterface.WriteInterface().AppendL( message );
        // Open interface
        msgRWInterface.WriteInterface().LockBufferMemoryHandling(
            EFalse );
        }
#endif //RD_SEND_NOT_SUPPORTED_CONTENT        
    }




// -----------------------------------------------------------------------------
// CCAMessageUtilsPC: SendMessageL
// -----------------------------------------------------------------------------
//

EXPORT_C TBool IMMessageUtilsPC::SendMessageL(
    const MCAConversationMessage& aMessage,
    const TDesC& aRecipient,
    MCAGroupPC& aMessageRWInterfacePC,
    const MDesCArray* aSelectedNames,
    const TDesC& aSender /*= KNullDesC*/,
    const TDesC& aSapId /*= KNullDesC*/,
    const TDesC& aUserId /*= KNullDesC*/,
    const TDesC& aFileName /*= KNullDesC*/,
    const MDesCArray* aRecipients /*NULL*/ )
    {

    CCAEngine* engine = CCAProcessManagerFactory::InstanceL()->
                        GetProcessManager()->GetEngine();


    MCAMessageCreator& creator =
        engine->MessageUtils().MessageCreator();
    CCAGroupPC& msgRWInterface =
        static_cast <CCAGroupPC&>( aMessageRWInterfacePC );


    MCAMessageCreator::SMessageData data =
        {
        KMessageDataVersion,
        0,
        aSapId,
        aUserId,
        aSender,
        aRecipient,
        aRecipients,
        aSelectedNames,
        aFileName,
        aMessage.MimeType(),
        aMessage.ContentData(),
        MCAMessage::EMessageSent
        };

    // Create and write
    MCAMessage* newMessage = creator.CreateMessageL( data );
    newMessage->SetProcessState( MCAMessage::EContentReady );
    msgRWInterface.WriteInterfaceL().AppendL( newMessage );

    // message handled succesfully
    return ETrue;

    }


//  End of File
