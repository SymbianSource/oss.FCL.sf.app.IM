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
* Description:  Message Base class.
*
*/


#include "CCAMessageBase.h"
#include "MCAMessageContainerInfo.h"
#include "PublicEngineDefinitions.h"
#include "PrivateEngineDefinitions.h"
#include "CAUtils.h"
#include "MCAContentProcessor.h"
#include "MCAContentProcessObserver.h"

#include "ChatDebugAssert.h"
#include "ChatDebugPrint.h"

#include <stringloader.h>
#include <caengineNG.rsg>
#include <avkon.rsg>
#include <txtetext.h>                    //for CEditableText::ELineBreak

_LIT( KLineFeedFormat, "%c" );
_LIT( KSpace, " " );
const TInt KTimeStampMaxLength = 20; // max timestamp string length

// Identification tool
TBool Compare( const CCAMessageBase::SProcessorObserverPair& aFirst,
               const CCAMessageBase::SProcessorObserverPair& aSecond )
    {
    if ( aFirst.iProcessor == aSecond.iProcessor )
        {
        return ETrue;
        }
    return EFalse;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::CCAMessageBase
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageBase::CCAMessageBase()
    {
    }


//-----------------------------------------------------------------------------
// CCAMessageBase::ConstructL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::ConstructL( TInt aOpCode,
                                 const TDesC& aSender,
                                 const TDesC& aRecipient,
                                 const MDesCArray* aRecipients,
                                 const MDesCArray* aScreenNames )
    {
    iOperationCode = aOpCode;

    iSender = aSender.AllocL();
    iRecipient = aRecipient.AllocL();

    // Sender & recipient
    if ( aRecipients )
        {
        CDesCArrayFlat* tempArray = new ( ELeave ) CDesCArrayFlat( 1 );
        CleanupStack::PushL( tempArray );
        TInt count( aRecipients->MdcaCount() );
        for ( TInt a( 0 ); a < count; ++a )
            {
            tempArray->AppendL( aRecipients->MdcaPoint( a ) );
            }
        CleanupStack::Pop( tempArray );
        delete iRecipients;
        iRecipients = tempArray;
        }

    if ( aScreenNames )
        {
        CDesCArrayFlat* tempArray = new ( ELeave ) CDesCArrayFlat( 1 );
        CleanupStack::PushL( tempArray );
        TInt count( aScreenNames->MdcaCount() );
        for ( TInt a( 0 ); a < count; ++a )
            {
            tempArray->AppendL( aScreenNames->MdcaPoint( a ) );
            }
        CleanupStack::Pop( tempArray );
        delete iScreenNames;
        iScreenNames = tempArray;
        }


    iMessagerType = EMessageOther;
    iSystemMessageType = ESystemMessageNone;
    iTime.HomeTime();
    iMessageVersion = KVersionNum;

    ResolveMessageType( aRecipients, aScreenNames );
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::~CCAMessageBase
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageBase::~CCAMessageBase()
    {
    __CHAT_ASSERT_DEBUG( iOwnerCount == 0 );
    delete iMimeType;
    delete iSender;
    delete iRecipient;
    delete iContentData;
    delete iText;
    delete iRecipients;
    delete iScreenNames;
    if ( iProcessors.Count() )
        {
        TInt ignore = KErrNone;
        TRAP( ignore, iProcessors[ 0 ].iProcessor->RemoveProcessingL( *this ) );
        }
    iProcessors.Close();
    delete iForwardPart;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::TimeStamp
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TTime& CCAMessageBase::TimeStamp() const
    {
    return iTime;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::MessageType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage::TMessageType CCAMessageBase::MessageType() const
    {
    return iMessageType;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::MessagerType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage::TMessagerType CCAMessageBase::MessagerType() const
    {
    return iMessagerType;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::OperationCode
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageBase::OperationCode() const
    {
    return iOperationCode;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Recipient
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::SetOperationCode( TInt aOpId )
    {
    iOperationCode = aOpId;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Recipient
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageBase::Recipient() const
    {
    if ( iRecipient )
        {
        return *iRecipient;
        }
    return KNullDesC;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Recipients
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const MDesCArray* CCAMessageBase::Recipients() const
    {
    return iRecipients;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::ScreenNames
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const MDesCArray* CCAMessageBase::ScreenNames() const
    {
    return iScreenNames;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::IncreaseOwnerCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::IncreaseOwnerCount()
    {
    ++iOwnerCount;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::DecreaseOwnerCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TBool CCAMessageBase::DecreaseOwnerCount()
    {
    return ( --iOwnerCount <= 0 );
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Target
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageBase::ServerAddress() const
    {
    if ( iInfo )
        {
        return iInfo->ServerAddress();
        }
    return KNullDesC;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Target
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageBase::UserId() const
    {
    if ( iInfo )
        {
        return iInfo->UserId();
        }
    return KNullDesC;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Target
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageBase::Target() const
    {
    if ( iInfo )
        {
        return iInfo->Target();
        }
    return KNullDesC;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::SetMessageContainerInfo
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::SetContainerInfo( MCAMessageContainerInfo* aInfo )
    {
    iInfo = aInfo;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Text
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCALoggerMessage& CCAMessageBase::LoggerMessage()
    {
    return *this;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::AddContentProcessor
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageBase::AddContentProcessor( MCAContentProcessor& aProcessor,
                                          MCAContentProcessObserver* aObserver )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageBase::AddContentProcessor" );

    SProcessorObserverPair pair =
        {
        &aProcessor,
        aObserver
        };

    TInt originalCountOfProcessors = iProcessors.Count();
    TInt error = iProcessors.Append( pair );
    if ( error == KErrNone )
        {
        CHAT_DP( D_CHAT_LIT( " Processor added." ) );
        if ( iProcessors.Count() == 1 )
            {
            TRAP( error, LaunchProcessingL() );
            CHAT_DP( D_CHAT_LIT( " Launch processing. Retval = %d" ), error );
            }
        }
    CHAT_DP_FUNC_DONE( "CCAMessageBase::AddContentProcessor" );
    if ( error < KErrNone )
        {
        return error;
        }
    return iProcessors.Count() > originalCountOfProcessors;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::HandleProcessingComplete
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::HandleProcessingComplete( MCAContentProcessor& aProcessor,
                                               TInt aStatus )
    {
    CHAT_DP_FUNC_ENTER( "CCAMessageBase::HandleProcessingComplete" );
    SProcessorObserverPair pair =
        {
        &aProcessor,
        NULL
        };
    TIdentityRelation< SProcessorObserverPair > rule( *Compare );
    TInt index = iProcessors.Find( pair, rule );
    if ( index >= 0 )
        {
        if ( iProcessors[ index ].iObserver )
            {
            CHAT_DP( D_CHAT_LIT( "Inform processing observer about completion \
                                  of processor." ) );
            iProcessors[ index ].iObserver->HandleProcessingComplete(
                aProcessor, *this, aStatus );
            }
        iProcessors.Remove( index );
        }
    if ( iDefaultContentObserver )
        {
        iDefaultContentObserver->HandleProcessingComplete( aProcessor, *this,
                                                           aStatus );
        }
    CHAT_DP_FUNC_DONE( "CCAMessageBase::HandleProcessingComplete" );
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::MimeType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC8& CCAMessageBase::MimeType() const
    {
    if ( iMimeType )
        {
        return *iMimeType;
        }
    return KNullDesC8;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::ContentData
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC8& CCAMessageBase::ContentData() const
    {
    if ( iContentData )
        {
        return *iContentData;
        }
    return KNullDesC8;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::SetContentData
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::SetContentData( HBufC8* aNewContent )
    {
    delete iContentData;
    iContentData = aNewContent;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::ContentProcessState
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage::TContentProcessState CCAMessageBase::ContentProcessState() const
    {
    return iContentProcessingState;
    }


//-----------------------------------------------------------------------------
// CCAMessageBase::SetProcessState
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::SetProcessState( TContentProcessState aNewState )
    {
    iContentProcessingState = aNewState;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::ReleaseTemporaryContentData
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::ReleaseTemporaryContentData()
    {
    // No temporary content data.
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Text
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageBase::Text() const
    {
    if ( iText )
        {
        return *iText;
        }
    return KNullDesC;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Sender
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageBase::Sender() const
    {
    if ( iSender )
        {
        return *iSender;
        }
    return KNullDesC;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::Sender
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageBase::SetContentProcessObserver(
    MCAContentProcessObserver* aObserver )
    {
    iDefaultContentObserver = aObserver;
    }

//-----------------------------------------------------------------------------
// CCAMessageBase::SizeInBytes
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageBase::SizeInBytes() const
    {
    return  iSender->Size() +
            iRecipient->Size();
    }

// ---------------------------------------------------------
// CCAMessageBase::FailedMessage()
// ---------------------------------------------------------
//
TBool CCAMessageBase::FailedMessage() const
    {
    return iFailed;
    }

// ---------------------------------------------------------
// CCAMessageBase::SetForwardPart()
// ---------------------------------------------------------
//
void CCAMessageBase::SetForwardPartL( const HBufC* aFwdPart )
    {
    // If NULL
    if ( !aFwdPart )
        {
        delete iForwardPart;
        iForwardPart = NULL;
        return;
        }

    HBufC* temp = aFwdPart->AllocL();
    // Delete old
    delete iForwardPart;
    iForwardPart = NULL;
    iForwardPart = temp;
    }

// ---------------------------------------------------------
// CCAMessageBase::ForwardPart()
// ---------------------------------------------------------
//
const HBufC* CCAMessageBase::ForwardPart() const
    {
    return iForwardPart;
    }

// ---------------------------------------------------------
// CCAMessageBase::FailedMessage()
// ---------------------------------------------------------
//
void CCAMessageBase::SetFailed( TBool aFailed /* = ETrue */  )
    {
    iFailed = aFailed;
    }

// ---------------------------------------------------------
// CCAMessageBase::InternalizeL()
// ---------------------------------------------------------
//
void CCAMessageBase::InternalizeL( RReadStream& aStream )
    {
    iMessageType  = static_cast< TMessageType  >( aStream.ReadUint16L() );
    iMessagerType = static_cast< TMessagerType >( aStream.ReadUint16L() );

    // This item does not exist in version 0
    if ( iMessageVersion > 0 )
        {
        iSystemMessageType =
            static_cast< TSystemMessage >( aStream.ReadUint16L() );
        }

    iFailed = static_cast< TBool >( aStream.ReadUint8L() );

    //this member was newly added.
    //read also this member from the file, because
    //the user could have saved the image before saving a conversation
    iSaved = static_cast< TBool >( aStream.ReadUint8L() );

    TUint32 high = aStream.ReadUint32L();
    TUint32 low = aStream.ReadUint32L();

    iTime = MAKE_TINT64( high, low );

    HBufC* tempSender = CAUtils::InternalizeBufferFromStreamL( aStream );
    delete iSender;
    iSender = tempSender;

    HBufC* tempRecipient = CAUtils::InternalizeBufferFromStreamL( aStream );
    delete iRecipient;
    iRecipient = tempRecipient;

    HBufC* tempMessage = CAUtils::InternalizeBufferFromStreamL( aStream );
    delete iText;
    iText = tempMessage;

    if ( ContentType() != EContentText && ContentType() != EContentInvalid )
        {
        HBufC8* tempContentType =
            CAUtils::InternalizeBuffer8FromStreamL( aStream );
        delete iMimeType;
        iMimeType = tempContentType;

        HBufC8* tempContent = CAUtils::InternalizeBuffer8FromStreamL( aStream );
        delete iContentData;
        iContentData = tempContent;
        }
    SetProcessState( EContentReady );
    }

// ---------------------------------------------------------
// CCAMessageBase::ExternalizeL()
// ---------------------------------------------------------
//
void CCAMessageBase::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint16L( static_cast< TUint16 >( ContentType() ) );
    aStream.WriteUint16L( static_cast< TUint16 >( KVersionNum ) );
    aStream.WriteUint16L( static_cast< TUint16 >( iMessageType ) );
    aStream.WriteUint16L( static_cast< TUint16 >( iMessagerType ) );
    aStream.WriteUint16L( static_cast< TUint16 >( iSystemMessageType ) );
    aStream.WriteUint8L( static_cast< TUint8 >( iFailed ) );

    //this member was newly added.
    //write also this member to the file, because
    //the user could have saved the image.
    aStream.WriteUint8L( static_cast< TUint8 >( iSaved ) );

    aStream.WriteUint32L( I64HIGH( iTime.Int64() ) );
    aStream.WriteUint32L( I64LOW( iTime.Int64() ) );

    CAUtils::ExternalizeBufferToStreamL( *iSender, aStream );
    // if the type of current received message are EMessageWhisper and EMessageGroup,
    // store recipient's screen name when try to save conversation.
    if ( iMessagerType == EMessageReceived && ( iMessageType == EMessageWhisper || iMessageType == EMessageGroup ) )
        {
        TInt count = iScreenNames->MdcaCount();
        if ( count )
            {
            // iScreenNames have only one element as current account's ScreenName.
            CAUtils::ExternalizeBufferToStreamL( iScreenNames->MdcaPoint( 0 ), aStream );
            }
        else
            {
            CAUtils::ExternalizeBufferToStreamL( *iRecipient, aStream );
            }
        }
    else
        {
        CAUtils::ExternalizeBufferToStreamL( *iRecipient, aStream );
        }

    CAUtils::ExternalizeBufferToStreamL( *iText, aStream );

    if ( ContentType() != EContentText && ContentType() != EContentInvalid )
        {
        CAUtils::ExternalizeBufferToStreamL( *iMimeType, aStream );
        CAUtils::ExternalizeBufferToStreamL( *iContentData, aStream );
        }
    }

// ---------------------------------------------------------
// CCAMessageBase::MessageSizeInBytesL()
// ---------------------------------------------------------
//
TInt CCAMessageBase::MessageSizeInBytesL() const
    {
    TInt memoryRequired( 0 );

    if ( iMessageType != EMessageSystem )
        {
        HBufC* separator =
            StringLoader::LoadLC( R_CHATCLIENT_HISTORY_MSG_SEPARATOR );
        if ( iMessagerType == EMessageSent )
            {
            memoryRequired += KFromMe().Size();
            memoryRequired += CAUtils::DisplayId( *iRecipient ).Size();
            }
        else
            {
            memoryRequired += CAUtils::DisplayId( *iSender ).Size();
            if ( iMessageType == EMessagePTOP )
                {
                memoryRequired += KWhiteSpace().Size();
                memoryRequired += KToMe().Size();
                }
            memoryRequired += separator->Size();
            memoryRequired += KWhiteSpace().Size();
            }
        CleanupStack::PopAndDestroy( separator );
        }

    memoryRequired += iText->Size();
    memoryRequired += KLineEnd().Size();

    return memoryRequired;
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::ExternalizeForMTML
// Externalize message to MTM send format.
// -----------------------------------------------------------------------------
//
void CCAMessageBase::ExternalizeForMTML( RWriteStream& aStream,
                                         TBool aShowTimeStamps ) const
    {
    if ( !aShowTimeStamps &&
         iSystemMessageType == ESystemMessageDateChange )
        {
        // Hide date change messages when we don't show timestamps
        return;
        }

    // TimeStamp
    if ( aShowTimeStamps &&
         iSystemMessageType != ESystemMessageDateChange )
        {
        HBufC* timeStamp =
            HBufC::NewLC( KTimeStampMaxLength );
        TPtr timePtr( timeStamp->Des() );

        HBufC* timeStampFormat =
            StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );

        // construct timestamp
        TRAPD( err, TimeStamp().FormatL( timePtr, *timeStampFormat ) );
        if ( err == KErrNone )
            {
            // Convert timestamp to correct number mode
            CAUtils::LanguageSpecificNumberConversion( timePtr );
            aStream.WriteL( timePtr );
            aStream.WriteL( KSpace );
            }

        //timeStamp, timeStampFormat
        CleanupStack::PopAndDestroy( 2, timeStamp );
        }

    if ( iMessageType != EMessageSystem )
        {
        HBufC* separator =
            StringLoader::LoadLC( R_CHATCLIENT_HISTORY_MSG_SEPARATOR );

        if ( iMessageType != EMessageGroup )
            {
            if ( iMessagerType == EMessageSent )
                {
                aStream.WriteL( KFromMe );
                aStream.WriteL( CAUtils::DisplayId( *iRecipient ) );
                }
            else
                {
                aStream.WriteL( CAUtils::DisplayId( *iSender ) );
                aStream.WriteL( KWhiteSpace );
                aStream.WriteL( KToMe );
                }
            }
        else
            {
            if ( iMessagerType == EMessageSent )
                {
                aStream.WriteL( CAUtils::DisplayId( *iSender ) );
                }
            else
                {
                aStream.WriteL( CAUtils::DisplayId( *iSender ) );
                }
            }
        aStream.WriteL( *separator );
        aStream.WriteL( KWhiteSpace );
        CleanupStack::PopAndDestroy( separator );
        }

    if ( ContentType() != EContentText ) // For media files.
        {
        HBufC* mediaFile = StringLoader::LoadLC( R_QTN_CHAT_LOG_MEDIA_FILE );
        aStream.WriteL( *mediaFile );
        CleanupStack::PopAndDestroy( mediaFile );
        }
    else if ( aShowTimeStamps &&
              iSystemMessageType == ESystemMessageDateChange )
        {
        // Date change system msg
        HBufC* dateChange = iText->AllocLC();
        TPtr dateChangePtr( dateChange->Des() );
        // Convert message to correct number mode
        CAUtils::LanguageSpecificNumberConversion( dateChangePtr );
        // Write to stream
        aStream.WriteL( dateChangePtr );
        CleanupStack::PopAndDestroy( dateChange );
        }
    else
        {
        aStream.WriteL( *iText );
        }

    TBuf<1> lineFeed;
    lineFeed.Format( KLineFeedFormat, CEditableText::ELineBreak );
    aStream.WriteL( lineFeed );
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::SetMessageType
// -----------------------------------------------------------------------------
//
void CCAMessageBase::SetMessageType( TMessageType aNewType )
    {
    iMessageType = aNewType;
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::SetMessagerType
// -----------------------------------------------------------------------------
//
void CCAMessageBase::SetMessagerType( TMessagerType aNewType )
    {
    iMessagerType = aNewType;
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::LaunchProcessing
// -----------------------------------------------------------------------------
//
void CCAMessageBase::LaunchProcessingL()
    {
    if ( iProcessors.Count() > 0 )
        {
        CHAT_DP( D_CHAT_LIT( "Request processing." ) );
        iProcessors[ 0 ].iProcessor->RequestProcessingL( *this );
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::ResolveMessageType
// -----------------------------------------------------------------------------
//
void CCAMessageBase::ResolveMessageType( const MDesCArray* aRecipients,
                                         const MDesCArray* aScreenNames )
    {
    // aData.iRecipients > 0 -> ptop
    // aData.iScreenNames > 0 -> whisper
    // other cases -> group

    TInt countOfRecipients( aRecipients ? aRecipients->MdcaCount() : 0 );
    TInt countOfScreenNames( aScreenNames ? aScreenNames->MdcaCount() : 0 );

    if ( countOfRecipients ) // received ptop
        {
        iMessageType = EMessagePTOP;
        }
    else if ( countOfScreenNames ) // sent / received whisper
        {
        iMessageType = EMessageWhisper;
        }
    else if ( !aRecipients && !aScreenNames ) // sent ptop
        {
        iMessageType = EMessagePTOP;
        }
    else
        {
        iMessageType = EMessageGroup;
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::SetSystem
// -----------------------------------------------------------------------------
//
void CCAMessageBase::SetSystem( TSystemMessage aType )
    {
    iSystemMessageType = aType;
    iMessagerType = EMessageSent;
    iMessageType = EMessageSystem;
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::SystemMessageType
// -----------------------------------------------------------------------------
//
MCAMessage::TSystemMessage CCAMessageBase::SystemMessageType()
    {
    if ( iMessageType != EMessageSystem )
        {
        // not a system message
        return ESystemMessageNone;
        }
    return iSystemMessageType;
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::SetVersionNumber
// -----------------------------------------------------------------------------
//
void CCAMessageBase::SetVersionNumber( TUint16 aVersion )
    {
    iMessageVersion = aVersion;
    }



//The following API's are used, if message content is a image
//This is part of new implementation required for wvuing
//While merging with wvui20, please make sure you preserve
//this change also.

// -----------------------------------------------------------------------------
// CCAMessageBase::SetImageAsSaved
// -----------------------------------------------------------------------------
//
void CCAMessageBase::SetImageAsSaved( const TBool aSaved )
    {
    iSaved = aSaved;
    }

// -----------------------------------------------------------------------------
// CCAMessageBase::IsImageSaved
// -----------------------------------------------------------------------------
//
TBool CCAMessageBase::IsImageSaved()
    {
    return iSaved;
    }


// end of file
