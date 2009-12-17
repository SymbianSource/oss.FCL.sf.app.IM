/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handler class for incoming and outgoing messages
*
*/


#include "CCAMessageHandler.h"
#include "MCAChatInterface.h"
#include "MCAMessagesWriteInterface.h"
#include "MCAMessageUtils.h"
#include "MCAImpsImClient.h"
#include "MCASettings.h"
#include "MCAMessageErrorObserver.h"
#include "MCAImpsFactory.h"
#include "PublicEngineDefinitions.h"
#include "impsbuilddefinitions.h"
#include "mcabuffermemoryhandler.h"
#include "camessageutil.h"

#include <apmstd.h> // KMaxDataTypeLength
#include <utf.h>    // CnvUtfConverter

#include "ChatDebugPrint.h"

#include "ImpsCSPAllErrors.h"

// "test character identity and accents, ignore case"
const TInt KCollationLevel = 1;
const TInt KMemorySafeValue = 1024; // One kbyte for safe memory allocation.

//-----------------------------------------------------------------------------
// CCAMessageHandler::CCAMessageHandler
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageHandler::CCAMessageHandler( MCAChatInterface& aChatInterface,
                                      MCAMessageUtils& aMessageUtils,
                                      MCAImpsFactory* aIMPSFactory )
        : iMessageUtils( aMessageUtils ),
        iChatInterface( aChatInterface ),
        iImpsFactory( aIMPSFactory )
    {
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::~CCAMessageHandler
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageHandler::~CCAMessageHandler()
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::~CCAMessageHandler" );
    delete iIdle;
    if ( iSendBuffer )
        {
        iSendBuffer->SetObserver( NULL );
        }
    iErrorObservers.Close();
    iHoldingMessages.Close();
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::~CCAMessageHandler" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageHandler* CCAMessageHandler::NewL(
    MCAChatInterface& aChatInterface,
    MCAMessageUtils& aMessageUtils,
    MCAImpsFactory* aIMPSFactory )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::NewL" );
    CCAMessageHandler* self = CCAMessageHandler::NewLC(
                                  aChatInterface,
                                  aMessageUtils,
                                  aIMPSFactory );
    CleanupStack::Pop( self );
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::NewL" );
    return self;
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::NewLC
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageHandler* CCAMessageHandler::NewLC(
    MCAChatInterface& aChatInterface,
    MCAMessageUtils& aMessageUtils,
    MCAImpsFactory* aIMPSFactory )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::NewLC" );
    CCAMessageHandler* self = new ( ELeave ) CCAMessageHandler(
        aChatInterface,
        aMessageUtils,
        aIMPSFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::NewLC" );
    return self;
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::ConstructL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::ConstructL()
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::ConstructL" );

    iImpsImClient = iImpsFactory->CreateImClientL();

    iSendBuffer = &iChatInterface.MessageReadInterfaceL( KNullDesC, KNullDesC );
    iSendBuffer->SetObserver( this );
    iIdle = CIdle::NewL( CActive::EPriorityIdle );
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::ConstructL" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleNewTextMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleNewTextMessageL(  TInt aOpId,
                                                const TDesC& /*aMessageId*/,
                                                const TDesC& aSender,
                                                const TDesC& aGroupId,
                                                const MDesCArray& aRecipients,
                                                const MDesCArray& aScreenNames,
                                                const TDesC& aText,
                                                TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::HandleNewTextMessageL" );

    // Don't use memory handling if this is local echo message
    MCAMessagesWriteInterface& messageContainer =
        iChatInterface.MessageWriteInterfaceL(
            aCspId.Sap(),
            aCspId.UserId(),
            ( aGroupId == KNullDesC ? aSender : aGroupId ) );

    if ( !( aGroupId != KNullDesC &&
            iLocalEchoInGroup &&
            messageContainer.OwnScreenName().CompareC( aSender, KCollationLevel, NULL ) == 0 ) )
        {
        if ( !iMessageUtils.MemoryHandler().FreeMemoryIfNeededL( KMemorySafeValue + aText.Size() ) )
            {
            NotifyMessageError( KErrNoMemory, NULL );
            User::Leave( KErrNoMemory );
            }
        }

    HBufC* sap = aCspId.Sap().AllocLC();
    HBufC* userId = aCspId.UserId().AllocLC();

    MCAMessageCreator::SMessageData data =
        {
        KMessageDataVersion,
        aOpId,
        *sap,
        *userId,
        aSender,
        ( aGroupId == KNullDesC ? aSender : aGroupId ),
        &aRecipients,
        &aScreenNames,
        aText,
        KNullDesC8,
        KNullDesC8,
        MCAMessage::EMessageReceived
        };
    HandleNewMessageL( data );

    CleanupStack::PopAndDestroy( 2, sap );
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::HandleNewTextMessageL" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleNewContentMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleNewContentMessageL(
    TInt aOpId,
    const TDesC& /*aMessageId*/,
    const TDesC& aSender,
    const TDesC& aGroupId,
    const MDesCArray& aRecipients,
    const MDesCArray& aScreenNames,
    const TDesC& aContentType,
    const TDesC8& aContent,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::HandleNewContentMessageL" );

    // Don't use memory handling if this is local echo message
    MCAMessagesWriteInterface& messageContainer =
        iChatInterface.MessageWriteInterfaceL(
            aCspId.Sap(),
            aCspId.UserId(),
            ( aGroupId == KNullDesC ? aSender : aGroupId ) );

    if ( !( aGroupId != KNullDesC &&
            iLocalEchoInGroup &&
            messageContainer.OwnScreenName().CompareC( aSender, KCollationLevel, NULL ) == 0 ) )
        {
        if ( !iMessageUtils.MemoryHandler().FreeMemoryIfNeededL( KMemorySafeValue + aContent.Size() ) )
            {
            NotifyMessageError( KErrNoMemory, NULL );
            User::Leave( KErrNoMemory );
            }
        }

    HBufC* sap = aCspId.Sap().AllocLC();
    HBufC* userId = aCspId.UserId().AllocLC();

    TBuf8< KMaxDataTypeLength > mimeType;
    CnvUtfConverter::ConvertFromUnicodeToUtf8( mimeType, aContentType );

    MCAMessageCreator::SMessageData data =
        {
        KMessageDataVersion,
        aOpId,
        *sap,
        *userId,
        aSender,
        ( aGroupId == KNullDesC ? aSender : aGroupId ),
        &aRecipients,
        &aScreenNames,
        KNullDesC,
        mimeType,
        aContent,
        MCAMessage::EMessageReceived
        };
    HandleNewMessageL( data );

    CleanupStack::PopAndDestroy( 2, sap );
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::HandleNewContentMessageL" );
    }


//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleNewMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleNewMessageL(
    MCAMessageCreator::SMessageData& aData )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::HandleNewMessageL" );
    if ( aData.iTargetId == KNullDesC )
        {
        // Invalid message.
        User::Leave( KErrArgument );
        }
    MCAMessagesWriteInterface& messageContainer =
        iChatInterface.MessageWriteInterfaceL( aData.iSapId,
                                               aData.iUserId,
                                               aData.iTargetId );

    MCAMessage* message = iMessageUtils.MessageCreator().CreateMessageL( aData );

    if ( iLocalEchoInGroup
         && ( message->MessageType() == MCAMessage::EMessageGroup )
         && ( messageContainer.OwnScreenName().CompareC( aData.iSender,
                                                         KCollationLevel, NULL ) == 0 ) )
        {
        // Do not show message because it is already shown with local echo.
        CHAT_DP( D_CHAT_LIT( "Group message not shown, because local echo." ) );
        delete message;
        return;
        }

    // set iTargetId to ScreenName if current received message type is EMessageWhisper
    if ( message->MessageType() == MCAMessage::EMessageWhisper || message->MessageType() == MCAMessage::EMessageGroup )
        {
        TInt count = aData.iScreenNames->MdcaCount();
        MCAMessageCreator::SMessageData data2 =
            {
            aData.iVersion,
            aData.iOpId,
            aData.iSapId,
            aData.iUserId,
            aData.iSender,
            ( count > 0 ) ? aData.iScreenNames->MdcaPoint( 0 ) : aData.iTargetId,
            aData.iRecipients,
            aData.iScreenNames,
            aData.iText,
            aData.iContentType,
            aData.iContentData,
            aData.iMessager
            };

        MCAMessage* message2 = iMessageUtils.MessageCreator().CreateMessageL( data2 );

        delete message;
        message = message2;
        }

    // Message can get different process state during creation. For example
    // EContentNotSupported
    if ( message->ContentProcessState() == MCAMessage::EContentNotProcessed )
        {
        message->SetProcessState( MCAMessage::EContentReady );
        }

    // Add message
    CAMessageUtil::AppendMessageWithDateStampL(
        *message,
        messageContainer,
        iMessageUtils.MessageCreator() );

    CHAT_DP_FUNC_DONE( "CCAMessageHandler::HandleNewMessageL" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleSendCompleteL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleSendCompleteL(	TInt aOpId,
                                             TBool aDeliveryReportOrdered,
                                             TImpsCspIdentifier& /*aCspId*/ )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::HandleSendCompleteL" );

    CHAT_DP( D_CHAT_LIT( "Operationcode %d. Delivery ordered %d" ),
             aOpId, aDeliveryReportOrdered );
    HandleMessageSentL( KErrNone, aOpId, ETrue );
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::HandleSendCompleteL" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleDeliveryReportL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleDeliveryReportL(	const TDesC& /*aMessageId*/,
                                               TInt /*aResult*/,
                                               const TDesC* /*aDescription*/,
                                               TImpsCspIdentifier& /*aCspId*/ )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::HandleDeliveryReportL" );
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::HandleDeliveryReportL" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleMessageEvent
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleMessageEvent( TMessageEventType aEvent,
                                            TInt /*aIndex*/ )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::HandleMessageEvent" );
    switch ( aEvent )
        {
        case ENewMessage:
            {
            if ( !iIdle->IsActive() )
                {
                iIdle->Start( TCallBack( SendMessage, this ) );
                }
            break;
            }
        case EChatDeleted:
            {
            // send buffer was deleted
            iSendBuffer = NULL;
            break;
            }
        default:
            break;
        }
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::HandleMessageEvent" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::SendMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageHandler::SendMessage( TAny *aInstance )
    {
    CCAMessageHandler* handler = static_cast< CCAMessageHandler* >( aInstance );
    handler->iSendLaunchLock = ETrue;
    TInt retVal = handler->DoSendMessage();
    handler->iSendLaunchLock = EFalse;
    return retVal;
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::DoSendMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageHandler::DoSendMessage()
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::DoSendMessage" );

    if ( !iSendBuffer )
        {
        // send buffer was deleted, we're shutting down
        CHAT_DP( D_CHAT_LIT( "There is no send buffer." ) );
        return EFalse;
        }

    // 1. Check if there is message in holding.
    MCAMessage* holdingMessage = NextHoldingMessage();
    if ( !holdingMessage && !iSendBuffer->UnreadCount() )
        {
        CHAT_DP( D_CHAT_LIT( "There is not ready messages for sending." ) );
        return EFalse;
        }

    // 2. Choose new message from sendbuffer or use holding message if exists.
    MCAMessage& message = ( !holdingMessage ? iSendBuffer->ReadNextUnread()
                            : *holdingMessage );

    // 3. Add message to holding if needed
    TBool appendedToHolding( EFalse );
    TRAPD( error, appendedToHolding = AppendedToHoldingL( message ) );
#ifndef RD_SEND_NOT_SUPPORTED_CONTENT
    if ( error ) // Corrupted message, inform upstairs
        {
        NotifyMessageError(
            error == KErrOverflow ? error : ECorruptedContent, &message );
        // Remove sent message from send buffer, because answer will not come.
        TInt index = iSendBuffer->FindIndex( message );
        if ( index >= 0 )
            {
            iSendBuffer->DeleteMessage( index );
            }
        return ETrue;
        }
#endif //RD_SEND_NOT_SUPPORTED
    if ( appendedToHolding )
        {
        return ETrue;
        }

    // 4. Send message
    TInt opCode( 0 );
    TRAP( error, opCode = SendMessageToServerL( message ) );
    CHAT_DP( D_CHAT_LIT( "Send retval ( %d )" ), error );

    if ( error == KErrServerBusy )
        {
        TRAPD( err, iHoldingMessages.AppendL( &message ) );
        if ( err )
            {
            CActiveScheduler::Current()->Error( err );
            }
        return ETrue;
        }
    // 5. Local echo message
    TRAPD( error2, LocalEchoMessageL( message ) );
    if ( error2 != KErrNone )
        {
        NotifyMessageError( error2, &message );
        }

    // 6. Handle error
    if ( error != KErrNone )
        {
        TRAP( error2, HandleMessageSentFailedL( message, error ) );
        if ( error2 )
            {
            NotifyMessageError( error2, &message );
            }
        // Remove sent message from send buffer, because answer will not come.
        TInt index = iSendBuffer->FindIndex( message );
        if ( index >= 0 )
            {
            iSendBuffer->DeleteMessage( index );
            }
        }
    else // Send succesfull. -> Local echo
        {
        CHAT_DP( D_CHAT_LIT( "Address: %s, UserId: %s, Recipient %s" ),
                 &message.ServerAddress(),
                 &message.UserId(),
                 &message.Recipient() );
        message.SetOperationCode( opCode );
        }

    // 7. Check need of
    TInt unreadCount = iSendBuffer->UnreadCount();
    TInt holdingMessages( iHoldingMessages.Count() );
    CHAT_DP( D_CHAT_LIT( "Still %d messages to go. Send buffer has %d messages. \
                          Holding %d messages" ),
             unreadCount, iSendBuffer->MessageCount(), holdingMessages );

    CHAT_DP_FUNC_DONE( "CCAMessageHandler::DoSendMessage" );
    return Max( unreadCount, holdingMessages );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleMessageSentL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleMessageSentL( TInt aStatus, TInt aOperationCode,
                                            TBool aSuccess )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::HandleMessageSentL" );
    CHAT_DP( D_CHAT_LIT( "OpCode = %d, Success = %d" ), aOperationCode, aSuccess );

    if ( !iSendBuffer )
        {
        // send buffer was deleted, we're shutting down
        CHAT_DP( D_CHAT_LIT( "There is no send buffer." ) );
        return;
        }

    TInt index = User::LeaveIfError( iSendBuffer->FindIndex( aOperationCode ) );

    if ( !aSuccess )
        {
        MCAMessage& message = iSendBuffer->Message( index );

        message.SetContainerInfo( NULL );

        MCAMessage* failedMessage =
            iMessageUtils.MessageCreator().CreateFailedMessageL( &message );
        CleanupDeletePushL( failedMessage );
        MCAMessagesWriteInterface& messageContainer =
            iChatInterface.MessageWriteInterfaceL( message.ServerAddress(),
                                                   message.UserId(),
                                                   message.Recipient() );
        CleanupStack::Pop( failedMessage );
        messageContainer.AppendL( failedMessage );
        NotifyMessageError( aStatus, &message );
        }
    iSendBuffer->DeleteMessage( index );
    CHAT_DP( D_CHAT_LIT( "Send buffer has %d messages." ),
             iSendBuffer->MessageCount() );
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::HandleMessageSentL" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::SetLocalEchoInGroup
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::SetLocalEchoInGroup( TBool aLocalEchoInGroup )
    {
    iLocalEchoInGroup = aLocalEchoInGroup;
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::NotifyMessageError
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::NotifyMessageError( TInt aStatus, MCAMessage* aMessage )
    {
    TInt count( iErrorObservers.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        iErrorObservers[ a ]->HandleMessageError( aStatus, aMessage );
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::RegisterChatObserver
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::RegisterObserver( MCAMessageErrorObserver* aObserver )
    {
    TInt index = iErrorObservers.Find( aObserver );
    if ( index == KErrNotFound )
        {
        iErrorObservers.Append( aObserver );
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::RegisterChatObserver
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::UnregisterObserver( MCAMessageErrorObserver* aObserver )
    {
    TInt index = iErrorObservers.Find( aObserver );
    if ( index >= 0 )
        {
        iErrorObservers.Remove( index );
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleProcessingComplete
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleProcessingComplete(
    MCAContentProcessor& /*aProcessor*/,
    MCAMessage& /*aMessage*/,
    TInt aStatus )
    {
    if ( !iIdle->IsActive() && aStatus == KErrNone && !iSendLaunchLock )
        {
        iIdle->Start( TCallBack( SendMessage, this ) );
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::NextHoldingMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage* CCAMessageHandler::NextHoldingMessage()
    {
    TInt holdingMessages = iHoldingMessages.Count();
    MCAMessage* message = NULL;
    TInt index( 0 );
    // Check if holding messages hold one ready message
    for ( ; index < holdingMessages && !message; ++index )
        {
        MCAMessage* msg = iHoldingMessages[ index ];
        if ( ( msg->ContentProcessState() >= MCAMessage::EContentReady ) ||
             ( msg->ContentType() == MCAMessage::EContentText ) )
            {
            CHAT_DP( D_CHAT_LIT( "Ready holding message found. Index( %d )" ),
                     index );
            message = msg;
            iHoldingMessages.Remove( index );
            }
        }
    return message;
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::NextHoldingMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TBool CCAMessageHandler::AppendedToHoldingL( MCAMessage& aMessage )
    {
    switch ( aMessage.ContentType() )
        {
        case MCAMessage::EContentPicture: // add scaler
            {
            if ( aMessage.ContentProcessState() < MCAMessage::EContentReady )
                {
                CHAT_DP( D_CHAT_LIT( "Picture message not yet ready for \
                                  sending. Put to holding." ) );
                TInt added = User::LeaveIfError( aMessage.AddContentProcessor(
                                                     iMessageUtils.ImageScaler(), this ) );
                if ( added )
                    {
                    iHoldingMessages.AppendL( &aMessage );
                    return ETrue;
                    }
                }
            break;
            }
        default: // Flowthrough other types
            {
#ifdef RD_SEND_NOT_SUPPORTED_CONTENT
            if ( aMessage.ContentProcessState() < MCAMessage::EContentReady )
                {
                CHAT_DP( D_CHAT_LIT( "Picture message not yet ready for \
                                  sending. Put to holding." ) );
                TInt added = User::LeaveIfError( aMessage.AddContentProcessor(
                                                     iMessageUtils.ImageScaler(), this ) );
                if ( added )
                    {
                    iHoldingMessages.AppendL( &aMessage );
                    return ETrue;
                    }
                }
#endif //RD_SEND_NOT_SUPPORTED_CONTENT            
            break;
            }
        }
    return EFalse;
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::SendMessageToServerL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageHandler::SendMessageToServerL( MCAMessage& aMessage )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::SendMessageToServerL" );

    TInt opCode( 0 );
    TBuf< KMaxDataTypeLength > mimeType;
    CnvUtfConverter::ConvertToUnicodeFromUtf8( mimeType, aMessage.MimeType() );

    // granularity of 1 because only one item appended to array
    CDesCArrayFlat* tempArray = NULL;
    const MDesCArray* recipients = NULL;

    const TDesC* sender = NULL;
    const TDesC* groupId = NULL;

    if ( aMessage.ScreenNames() )
        {
        groupId = &aMessage.Recipient();
        if ( aMessage.ScreenNames()->MdcaCount() > 0 )
            {
            recipients = aMessage.ScreenNames();
            }
        }
    else
        {
        tempArray = new ( ELeave ) CDesCArrayFlat( 1 );
        CleanupStack::PushL( tempArray );
        tempArray->AppendL( aMessage.Recipient() );
        }

    if ( aMessage.ContentType() == MCAMessage::EContentText )
        {
        CHAT_DP( D_CHAT_LIT( "Send text message" ) );
        opCode = iImpsImClient->SendTextMessageL(
                     sender,
                     tempArray,
                     groupId,
                     recipients,
                     aMessage.Text(),
                     EFalse );
        }
    else
        {
        CHAT_DP( D_CHAT_LIT( "Send content message" ) );
        TInt size( aMessage.ContentData().Size() );
        TInt maxSize( iImpsImClient->MaxTransactionContentLengthL() );
        if ( size > maxSize )
            {
            // content too big => can't send
            CHAT_DP( D_CHAT_LIT( "content too big: %d/%d" ), size, maxSize );
            User::Leave( KErrOverflow );
            }

        opCode = iImpsImClient->SendContentMessageL(
                     sender,
                     tempArray,
                     groupId,
                     recipients,
                     mimeType,
                     aMessage.ContentData(),
                     EFalse );
        }

    if ( tempArray )
        {
        CleanupStack::PopAndDestroy( tempArray );
        }

    CHAT_DP_FUNC_DONE( "CCAMessageHandler::SendMessageToServerL" );
    return opCode;
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::HandleMessageSentFailedL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::HandleMessageSentFailedL( MCAMessage& aMessage,
                                                  TInt aError )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::HandleMessageSentFailedL" );
    MCAMessage* failMessage =
        iMessageUtils.MessageCreator().CreateFailedMessageL( &aMessage );
    CleanupDeletePushL( failMessage );
    MCAMessagesWriteInterface& messageContainer =
        iChatInterface.MessageWriteInterfaceL( aMessage.ServerAddress(),
                                               aMessage.UserId(),
                                               aMessage.Recipient() );
    CleanupStack::Pop( failMessage );

    // Notify observers for error.
    NotifyMessageError( aError, &aMessage );
    messageContainer.AppendL( failMessage );
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::HandleMessageSentFailedL" );
    }

//-----------------------------------------------------------------------------
// CCAMessageHandler::LocalEchoMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageHandler::LocalEchoMessageL( MCAMessage& aMessage )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageHandler::LocalEchoMessageL" );

    if ( aMessage.MessageType() == MCAMessage::EMessageGroup
         && !iLocalEchoInGroup )
        {
        // Local echo is not allowed in groups.
        CHAT_DP( D_CHAT_LIT( " Message not shown because local echo is not \
                    allowed." ) );
        return;
        }

    MCAMessagesWriteInterface& messageContainer =
        iChatInterface.MessageWriteInterfaceL( aMessage.ServerAddress(),
                                               aMessage.UserId(),
                                               aMessage.Recipient() );

    if ( aMessage.MessageType() == MCAMessage::EMessageWhisper )
        {
        // Local echo ones for all recipients
        const MDesCArray* recipients = aMessage.ScreenNames();
        TInt count( recipients->MdcaCount() );
        for ( TInt a( 0 ); a < count; ++a )
            {
            MCAMessageCreator::SMessageData data =
                {
                KMessageDataVersion,
                aMessage.OperationCode(),
                aMessage.ServerAddress(),
                aMessage.UserId(),
                aMessage.Sender(),
                recipients->MdcaPoint( a ),
                aMessage.Recipients(),
                recipients,
                aMessage.Text(),
                aMessage.MimeType(),
                aMessage.ContentData(),
                aMessage.MessagerType()
                };
            MCAMessage* message =
                iMessageUtils.MessageCreator().CreateMessageL( data );
            message->SetProcessState( MCAMessage::EContentReady );

            // Append message
            CAMessageUtil::AppendMessageWithDateStampL(
                *message,
                messageContainer,
                iMessageUtils.MessageCreator() );
            }
        }
    else
        {
        // Append message
        CAMessageUtil::AppendMessageWithDateStampL(
            aMessage,
            messageContainer,
            iMessageUtils.MessageCreator(),
            ETrue );
        }
    CHAT_DP_FUNC_DONE( "CCAMessageHandler::LocalEchoMessageL" );
    }


// end of file
