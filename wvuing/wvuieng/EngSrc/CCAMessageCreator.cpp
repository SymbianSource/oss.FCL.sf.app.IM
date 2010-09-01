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
* Description:  Creator class for messages.
*
*/


#include "CCAMessageCreator.h"
#include "CCAContentMessage.h"
#include "CCATextMessage.h"

#include "ChatDebugPrint.h"

const TInt KSystemOperationCode = -1;

//-----------------------------------------------------------------------------
// CCAMessageCreator::CCAMessageCreator
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageCreator::CCAMessageCreator()
    {
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::~CCAMessageCreator
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageCreator::~CCAMessageCreator()
    {
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageCreator* CCAMessageCreator::NewL()
    {
    CCAMessageCreator* self = new ( ELeave ) CCAMessageCreator();
    return self;
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::NewLC
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageCreator* CCAMessageCreator::NewLC()
    {
    CCAMessageCreator* self = new ( ELeave ) CCAMessageCreator();
    CleanupStack::PushL( self );
    return self;
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::CreateMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage* CCAMessageCreator::CreateMessageL( const SMessageData& aData ) const
    {
    CHAT_DP( D_CHAT_LIT( "CCAMessageCreator::CreateMessageL" ) );
    if ( aData.iContentType == KNullDesC8 ) // Text message
        {
        CHAT_DP( D_CHAT_LIT( "Text message" ) );
        return CreateTextMessageL( aData );
        }
    else
        {
        return CreateContentMessageL( aData );
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::CreateFailedMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage* CCAMessageCreator::CreateFailedMessageL(
    const MCAMessage* pSource ) const
    {
    return CreateFailedTextMessageL( pSource );
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::CreateSystemMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage* CCAMessageCreator::CreateSystemMessageL( MCAMessage::TSystemMessage aType,
                                                     const TDesC& aText ) const
    {
    CCATextMessage* message = CCATextMessage::NewLC( KSystemOperationCode,
                                                     KNullDesC, KNullDesC, NULL,
                                                     NULL, aText );
    message->SetSystem( aType );
    CleanupStack::Pop( message );
    return message;
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::CreateTextMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage* CCAMessageCreator::CreateTextMessageL(
    const SMessageData& aData ) const
    {
    CCATextMessage* message = CCATextMessage::NewL( aData.iOpId,
                                                    aData.iSender,
                                                    aData.iTargetId,
                                                    aData.iRecipients,
                                                    aData.iScreenNames,
                                                    aData.iText );
    message->SetMessagerType( aData.iMessager );
    return message;
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::CreatePictureMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage* CCAMessageCreator::CreateContentMessageL(
    const SMessageData& aData ) const
    {
    CCAContentMessage* message = CCAContentMessage::NewL( 	aData.iOpId,
                                                           aData.iSender,
                                                           aData.iTargetId,
                                                           aData.iRecipients,
                                                           aData.iScreenNames,
                                                           aData.iContentType,
                                                           aData.iContentData,
                                                           aData.iText );
    message->SetMessagerType( aData.iMessager );
    return message;
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::CreateFailedTextMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage* CCAMessageCreator::CreateFailedTextMessageL(
    const MCAMessage* pSource ) const
    {
    CCATextMessage* message = CCATextMessage::NewLC( pSource->OperationCode(),
                                                     pSource->Sender(),
                                                     pSource->Recipient(),
                                                     pSource->Recipients(),
                                                     pSource->ScreenNames(),
                                                     pSource->Text() );
    message->SetFailedL();
    CleanupStack::Pop( message );
    message->SetMessagerType( pSource->MessagerType() );
    return message;
    }

//-----------------------------------------------------------------------------
// CCAMessageCreator::CloneMessageL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage* CCAMessageCreator::CloneMessageL( const MCAMessage* aSource ) const
    {
    MCAMessage* message = NULL;
    if ( aSource->ContentType() == MCAMessage::EContentText )
        {
        // Text message
        message = CCATextMessage::NewLC( aSource->OperationCode(),
                                         aSource->Sender(),
                                         aSource->Recipient(),
                                         aSource->Recipients(),
                                         aSource->ScreenNames(),
                                         aSource->Text() );
        message->SetForwardPartL( aSource->ForwardPart() );
        }
    else
        {
        // Content message
        message = CCAContentMessage::NewLC( aSource->OperationCode(),
                                            aSource->Sender(),
                                            aSource->Recipient(),
                                            aSource->Recipients(),
                                            aSource->ScreenNames(),
                                            aSource->MimeType(),
                                            aSource->ContentData(),
                                            aSource->Text() );
        }
    static_cast< CCAMessageBase* >( message )->
    SetMessagerType( aSource->MessagerType() );

    CleanupStack::Pop(); // message
    return message;
    }

// end of file
