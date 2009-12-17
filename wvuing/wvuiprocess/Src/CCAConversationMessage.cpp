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
* Description:  Interface for handling one message
*
*/



// INCLUDE FILES
#include "CCAConversationMessage.h"
#include "MCAMessage.h"
#include "CCAContentMessage.h"
#include "MCAProcessManager.h"
#include "CCAEngine.h"
#include "CCAProcessManagerFactory.h"
#include "MCAMessageUtils.h"
#include "MCAContentProcessor.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CCAConversationMessage::NewL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAConversationMessage* CCAConversationMessage::NewL( MCAMessage& aMessage,
                                                      MCAMessage* aFwdMessage /*= NULL*/ )
    {
    CCAConversationMessage* self =
        new( ELeave ) CCAConversationMessage( aMessage, aFwdMessage );

    return self;
    }


// ---------------------------------------------------------
// CCAConversationMessage::CCAConversationMessage()
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAConversationMessage::CCAConversationMessage( MCAMessage& aMessage,
                                                MCAMessage* aFwdMessage )
        : iMessage( aMessage ),
        iFwdMessage( aFwdMessage )
    {

    }


// ---------------------------------------------------------
// CCAConversationMessage::TimeStamp()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TTime& CCAConversationMessage::TimeStamp() const
    {
    return iMessage.TimeStamp();
    }


// ---------------------------------------------------------
// CCAConversationMessage::MessageType()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TEnumsPC::TMessageType CCAConversationMessage::MessageType() const
    {

    TEnumsPC::TMessageType msgType = TEnumsPC::EMessageInvalid;

    switch ( iMessage.MessageType() )
        {
        case MCAMessage::EMessagePTOP:
            {
            msgType = TEnumsPC::EMessagePTOP;
            break;
            }
        case MCAMessage::EMessageWhisper:
            {
            msgType = TEnumsPC::EMessageWhisper;
            break;
            }

        case MCAMessage::EMessageGroup:
            {
            msgType = TEnumsPC::EMessageGroup;
            break;
            }
        case MCAMessage::EMessageSystem:
            {
            msgType = TEnumsPC::EMessageSystem;
            break;
            }
        default:
            {
            break;
            }

        }

    return 	msgType;

    }



// ---------------------------------------------------------
// CCAConversationMessage::MessagerType()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TEnumsPC::TMessagerType CCAConversationMessage::MessagerType() const
    {
    TEnumsPC::TMessagerType msgrType = TEnumsPC::EMessageOther;

    switch ( iMessage.MessagerType() )
        {
        case MCAMessage::EMessageSent:
            {
            msgrType = TEnumsPC::EMessageSent;
            break;
            }
        case MCAMessage::EMessageReceived:
            {
            msgrType = TEnumsPC::EMessageReceived;
            break;
            }
        default:
            {
            break;
            }

        }

    return 	msgrType;
    }



// ---------------------------------------------------------
// CCAConversationMessage::ContentType()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TEnumsPC::TContentType CCAConversationMessage::ContentType() const
    {
    TEnumsPC::TContentType contentType = TEnumsPC::EContentInvalid;

    switch ( iMessage.ContentType() )
        {
        case MCAMessage::EContentText:
            {
            contentType = TEnumsPC::EContentText;
            break;
            }
        case MCAMessage::EContentPicture:
            {
            contentType = TEnumsPC::EContentPicture;
            break;
            }
        case MCAMessage::EContentOther:
            {
            contentType = TEnumsPC::EContentOther;
            break;
            }
        default:
            {
            break;
            }

        }

    return 	contentType;

    }



// ---------------------------------------------------------
// CCAConversationMessage::SystemMessageType()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TEnumsPC::TSystemMessage CCAConversationMessage::SystemMessageType()
    {

    TEnumsPC::TSystemMessage systemMessage = TEnumsPC::ESystemMessageNone;

    switch ( iMessage.SystemMessageType() )
        {
        case MCAMessage::ESystemMessageWelcome:
            {
            systemMessage = TEnumsPC::ESystemMessageWelcome;
            break;
            }
        case MCAMessage::ESystemMessageNewUsers:
            {
            systemMessage = TEnumsPC::ESystemMessageNewUsers;
            break;
            }
        case MCAMessage::ESystemMessageTopicChanged:
            {
            systemMessage = TEnumsPC::ESystemMessageTopicChanged;
            break;
            }
        case MCAMessage::ESystemMessagePrivatChanged:
            {
            systemMessage = TEnumsPC::ESystemMessagePrivatChanged;
            break;
            }
        case MCAMessage::ESystemMessageLeftUsers:
            {
            systemMessage = TEnumsPC::ESystemMessageLeftUsers;
            break;
            }
        case MCAMessage::ESystemMessageDateChange:
            {
            systemMessage = TEnumsPC::ESystemMessageDateChange;
            break;
            }
        default:
            {
            break;
            }

        }

    return 	systemMessage;
    }



// ---------------------------------------------------------
// CCAConversationMessage::Recipient()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CCAConversationMessage::Recipient() const
    {
    return iMessage.Recipient();
    }


// ---------------------------------------------------------
// CCAConversationMessage::Sender()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CCAConversationMessage::Sender() const
    {
    return iMessage.Sender();
    }



// ---------------------------------------------------------
// CCAConversationMessage::Text()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CCAConversationMessage::Text() const
    {
    return iMessage.Text();
    }



// ---------------------------------------------------------
// CCAConversationMessage::MimeType()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC8& CCAConversationMessage::MimeType() const
    {
    return iMessage.MimeType();
    }



// ---------------------------------------------------------
// CCAConversationMessage::ContentData()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC8& CCAConversationMessage::ContentData() const
    {
    return iMessage.ContentData();
    }



// ---------------------------------------------------------
// CCAConversationMessage::SetContentData()
// (other items were commented in a header).
// aContent could have been a const, since this is calling the engine
// all the engine calls has to be modified, hence left it like that.
// ---------------------------------------------------------
//
void CCAConversationMessage::SetContentData( HBufC8* aContent )
    {
    iMessage.SetContentData( aContent );
    }


// ---------------------------------------------------------
// CCAConversationMessage::ContentProcessState()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TEnumsPC::TContentProcessState CCAConversationMessage::ContentProcessState() const
    {

    TEnumsPC::TContentProcessState processState = TEnumsPC::EContentNotProcessed;

    switch ( iMessage.ContentProcessState() )
        {
        case MCAMessage::EContentNotSupported:
            {
            processState = TEnumsPC::EContentNotSupported;
            break;
            }
        case MCAMessage::EContentNotSupportedDrm:
            {
            processState = TEnumsPC::EContentNotSupportedDrm;
            break;
            }
        case MCAMessage::EContentCorrupted:
            {
            processState = TEnumsPC::EContentCorrupted;
            break;
            }
        case MCAMessage::EBitmapDecoded:
            {
            processState = TEnumsPC::EBitmapDecoded;
            break;
            }
        case MCAMessage::EBitmapScaled:
            {
            processState = TEnumsPC::EBitmapScaled;
            break;
            }
        case MCAMessage::EContentReady:
            {
            processState = TEnumsPC::EContentReady;
            break;
            }

        case MCAMessage::EContentDecoded:
            {
            processState = TEnumsPC::EContentDecoded;
            break;
            }
        case MCAMessage::EThumbnailReady:
            {
            processState = TEnumsPC::EThumbnailReady;
            break;
            }

        default:
            {
            break;
            }

        }

    return 	processState;


    }



// ---------------------------------------------------------
// CCAConversationMessage::SetProcessState()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationMessage::SetProcessState( TEnumsPC::TContentProcessState aNewState )
    {
    MCAMessage::TContentProcessState processState = MCAMessage::EContentNotProcessed;

    switch ( aNewState )
        {
        case TEnumsPC::EContentNotSupported:
            {
            processState = MCAMessage::EContentNotSupported;
            break;
            }
        case TEnumsPC::EContentNotSupportedDrm:
            {
            processState = MCAMessage::EContentNotSupportedDrm;
            break;
            }
        case TEnumsPC::EContentCorrupted:
            {
            processState = MCAMessage::EContentCorrupted;
            break;
            }
        case TEnumsPC::EBitmapDecoded:
            {
            processState = MCAMessage::EBitmapDecoded;
            break;
            }
        case TEnumsPC::EBitmapScaled:
            {
            processState = MCAMessage::EBitmapScaled;
            break;
            }
        case TEnumsPC::EContentReady:
            {
            processState = MCAMessage::EContentReady;
            break;
            }

        case TEnumsPC::EContentDecoded:
            {
            processState = MCAMessage::EContentDecoded;
            break;
            }
        case TEnumsPC::EThumbnailReady:
            {
            processState = MCAMessage::EThumbnailReady;
            break;
            }

        default:
            {
            break;
            }

        }

    iMessage.SetProcessState( processState )	;
    }



// ---------------------------------------------------------
// CCAConversationMessage::SetForwardPartL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationMessage::SetForwardPartL( const HBufC* aFwdPart )
    {
    iMessage.SetForwardPartL( aFwdPart )	;
    }



// ---------------------------------------------------------
// CCAConversationMessage::ForwardPart()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const HBufC* CCAConversationMessage::ForwardPart() const
    {
    return iMessage.ForwardPart();
    }



// ---------------------------------------------------------
// CCAConversationMessage::AddContentProcessorL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationMessage::AddContentProcessorL( TSize aThumbSize )
    {

    MCAProcessManager* processManager =  CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine&	engine ( *processManager->GetEngine() );

    const MCAMessageUtils& mUtils = engine.MessageUtils();

    // add thumbnail processor for message
    CCAContentMessage& msg = static_cast< CCAContentMessage& >( iMessage );
    msg.SetThumbnailSize( aThumbSize );
    iMessage.AddContentProcessor( mUtils.ImageScaler(), NULL );
    }



// ---------------------------------------------------------
// CCAConversationMessage::ReleaseTemporaryContentData()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationMessage::ReleaseTemporaryContentData()
    {
    iMessage.ReleaseTemporaryContentData();
    }



// ---------------------------------------------------------
// CCAConversationMessage::RemoveProcessingL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationMessage::RemoveProcessingL()
    {
    MCAProcessManager* processManager =  CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine&	engine ( *processManager->GetEngine() );

    const MCAMessageUtils& mUtils = engine.MessageUtils();

    mUtils.ImageScaler().RemoveProcessingL( iMessage );
    }



// ---------------------------------------------------------
// CCAConversationMessage::Thumbnail()
// (other items were commented in a header).
// ---------------------------------------------------------
//
CFbsBitmap* CCAConversationMessage::Thumbnail() const
    {
    CCAContentMessage& msg = static_cast< CCAContentMessage& >( iMessage );
    return msg.Thumbnail();
    }

// ---------------------------------------------------------
// CCAConversationMessage::GetMessage()
// (other items were commented in a header).
// ---------------------------------------------------------
//
MCAMessage& CCAConversationMessage::GetMessage() const
    {
    return iMessage;
    }


//fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
//Section 10.2.10 Pg 131 -
//"In case user has saved the image already or sent it by himself,
// this option(save) is not available."
// ---------------------------------------------------------
// CCAConversationMessage::SetImageAsSaved()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationMessage::SetImageAsSaved( const TBool aSaved )
    {
    iMessage.SetImageAsSaved( aSaved );
    }

//fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
//Section 10.2.10 Pg 131 -
//"In case user has saved the image already or sent it by himself,
// this option(save) is not available."
// ---------------------------------------------------------
// CCAConversationMessage::IsImageSaved()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAConversationMessage::IsImageSaved()
    {
    return iMessage.IsImageSaved();
    }

// ---------------------------------------------------------
// CCAConversationMessage::FailedMessage()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAConversationMessage::FailedMessage() const
    {
    return iMessage.FailedMessage();
    }


// ---------------------------------------------------------
// CCAConversationMessage::~CCAConversationMessage()
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAConversationMessage::~CCAConversationMessage()
    {
    //delete if there is any fwd message
    delete iFwdMessage;
    }


// End of File

