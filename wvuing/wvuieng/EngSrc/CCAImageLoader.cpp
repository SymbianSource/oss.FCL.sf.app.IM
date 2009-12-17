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
* Description:  Image loader and scaler
*
*/


#include "CCAImageLoader.h"
#include "MCAChatInterface.h"
#include "CCAContentMessage.h"

#include "ChatDebugPrint.h"
#include "chatdebugassert.h"
#include "impsbuilddefinitions.h"

#include "mcaimpsfactory.h"
#include "mcaimpsimclient.h"
#include "wvengutils.h"
#include "imnginternalcrkeys.h"
#include "imprivatecrkeys.h"
#include "imvariant.hrh"

#include <ImageConversion.h>
#include <BitmapTransforms.h>
#include <s32file.h>

// CONSTANTS
const TInt KChatCustomBitmapWidth = 320;
const TInt KChatCustomBitmapHeight = 240;

// From ICL documentation:
// "all decoder plugins also support thumbnail decodes with ratios of 1:2, 1:4 and 1:8."
const TInt KDecodeScaleRatioMin = 2;
const TInt KDecodeScaleRatioMax = 8;


//-----------------------------------------------------------------------------
// CCAImageLoader::CCAImageLoader
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAImageLoader::CCAImageLoader(
    MCAChatInterface& aChatInterface, MCAImpsFactory& aImpsFactory )
        : CActive( CActive::EPriorityLow ),
        iChatInterface( aChatInterface ),
        iImpsFactory( aImpsFactory )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::CCAImageLoader" );
    CActiveScheduler::Add( this );
    CHAT_DP_FUNC_DONE( "CCAImageLoader::CCAImageLoader" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::CCAImageLoader
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAImageLoader::~CCAImageLoader()
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::~CCAImageLoader" );
    iFs.Close();

    delete iDecoder;
    delete iScaler;
    delete iEncoder;
    delete iBitmap;
    delete iContent;

    iMessages.Close();

    CHAT_DP_FUNC_DONE( "CCAImageLoader::~CCAImageLoader" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAImageLoader* CCAImageLoader::NewL( MCAChatInterface& aChatInterface,
                                      MCAImpsFactory& aImpsFactory )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::NewL" );
    CCAImageLoader* self =
        new ( ELeave ) CCAImageLoader( aChatInterface, aImpsFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    CHAT_DP_FUNC_DONE( "CCAImageLoader::NewL" );
    return self;
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::ConstructL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::ConstructL()
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::ConstructL" );
    User::LeaveIfError( iFs.Connect() );

    iSendNotScalable = WVEngUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
                       & EIMFeatSendNotScalable;
    CHAT_DP_FUNC_DONE( "CCAImageLoader::ConstructL" );
    }


//-----------------------------------------------------------------------------
// CCAImageLoader::RunL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::RunL()
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::RunL" );
    TInt status = iStatus.Int();
    CHAT_DP( D_CHAT_LIT( "Status %d" ), status );
    switch ( status )
        {
        case KErrUnderflow:
            {
            if ( iDecoder )
                {
                CHAT_DP( D_CHAT_LIT( "Continue converting." ) );
                iStatus = KRequestPending;
                iDecoder->ContinueConvert( &iStatus );
                }
            else
                {
                CompleteRequestL();
                }
            break;
            }
        case KErrNone:
            {
            if ( iDecoder )
                {
                delete iDecoder;
                iDecoder = NULL;
                switch ( iCurrentMessage->ContentProcessState() )
                    {
                    case MCAMessage::EContentNotProcessed:
                        {
                        CHAT_DP( D_CHAT_LIT( "-> Start scaling." ) );
                        // Decode ready -> Start scaling.
                        iCurrentMessage->SetProcessState(
                            MCAMessage::EBitmapDecoded );
                        StartProcessingL( *iCurrentMessage );
                        break;
                        }
                    case MCAMessage::EContentReady:
                        {
                        CHAT_DP(
                            D_CHAT_LIT( "-> Start scaling to thumbnail." ) );
                        // Decode ready -> Start making thumbnail.
                        iCurrentMessage->SetProcessState(
                            MCAMessage::EContentDecoded );
                        StartProcessingL( *iCurrentMessage );
                        break;
                        }
                    default:
                        {
                        iStatus = KErrNotSupported;
                        CompleteRequestL();
                        break;
                        }
                    }

                }
            else if ( iScaler )
                {
                // scaling ready -> start encoding
                delete iScaler;
                iScaler = NULL;
                switch ( iCurrentMessage->ContentProcessState() )
                    {
                    case MCAMessage::EBitmapDecoded:
                        {
                        CHAT_DP( D_CHAT_LIT( "-> Start encoding." ) );
                        iCurrentMessage->SetProcessState(
                            MCAMessage::EBitmapScaled );
                        StartProcessingL( *iCurrentMessage );
                        break;
                        }
                    case MCAMessage::EContentDecoded:
                        {
                        CHAT_DP( D_CHAT_LIT( "Thumbnail ready." ) );
                        CCAContentMessage* message =
                            static_cast< CCAContentMessage* >( iCurrentMessage );
                        message->SetThumbnail( iBitmap );
                        iBitmap = NULL;
                        iCurrentMessage->SetProcessState(
                            MCAMessage::EThumbnailReady );
                        CompleteRequestL();
                        break;
                        }
                    default:
                        {
                        iStatus = KErrNotSupported;
                        CompleteRequestL();
                        break;
                        }
                    }
                }
            else if ( iEncoder )
                {
                CHAT_DP( D_CHAT_LIT( "Content ready. -> Stop processing." ) );
                delete iEncoder;
                iEncoder = NULL;
                iCurrentMessage->SetProcessState( MCAMessage::EContentReady );
                iCurrentMessage->SetContentData( iContent );
                iContent = NULL;
                delete iBitmap;
                iBitmap = NULL;
                CompleteRequestL();
                }
            else
                {
                iStatus = KErrNotSupported;
                CompleteRequestL();
                }
            break;
            }
        case KErrNoMemory:
            {
            // RunError will handle this case
            User::Leave( KErrNoMemory );
            }
        default:
            {
            CompleteRequestL();
            break;
            }
        }
    CHAT_DP_FUNC_DONE( "CCAImageLoader::RunL" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::DoCancel
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::DoCancel()
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::DoCancel" );

    ClearState();
    iCurrentMessage->HandleProcessingComplete( *this, iStatus.Int() );
    CHAT_DP_FUNC_DONE( "CCAImageLoader::DoCancel" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::RunError
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAImageLoader::RunError( TInt aError )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::RunError" );
    if ( IsActive() )
        {
        iStatus = aError;
        Cancel();
        }

    if ( aError == KErrNoMemory )
        {
        CActiveScheduler::Current()->Error( KErrNoMemory );
        }

    // Clean up and reset all pending messages
    ClearState();
    TRAP_IGNORE( ResetProcessingL( aError ) );

    CHAT_DP_FUNC_DONE( "CCAImageLoader::RunError" );
    return KErrNone;
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::RequestProcessing
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::RequestProcessingL( MCAMessage& aMessage )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::RequestProcessingL" );
    // Append to scheduler
    iMessages.AppendL( &aMessage );
    LaunchProcessingL();
    CHAT_DP_FUNC_DONE( "CCAImageLoader::RequestProcessingL" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::StartProcessing
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::StartProcessingL( MCAMessage& aMessage )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::StartProcessingL" );
    // File -> bitmap
    // bitmap -> scaled bitmap
    // scaled bitmap -> encoded content

    iCurrentMessage = &aMessage;
    switch ( aMessage.ContentProcessState() )
        {
        case MCAMessage::EContentNotProcessed:  // File -> bitmap
            {
            __CHAT_ASSERT_DEBUG( !iDecoder );
            iDecoder = CImageDecoder::FileNewL( iFs, aMessage.Text() );

            TSize origSize( iDecoder->FrameInfo().iOverallSizeInPixels );
            ScaleSize( origSize, iTargetSize );
            TBool fullyScaleable( iDecoder->FrameInfo().iFlags & TFrameInfo::EFullyScaleable );

            TInt frameCount = iDecoder->FrameCount();
            if ( frameCount > 1 && iSendNotScalable )
                {
                // Animated gif, try to send file as it is
                // Animation would be lost in decoding/scaling process

                delete iDecoder;    // Delete -> unlocks image file
                iDecoder = NULL;

                // Open file
                RFile file;
                User::LeaveIfError( file.Open( iFs, aMessage.Text(), EFileRead ) );
                CleanupClosePushL( file );

                // Get file size
                TInt fileSize = 0;
                User::LeaveIfError( file.Size( fileSize ) );

                // Check file size, if bigger than max transaction content -> leave
                if ( fileSize >
                     iImpsFactory.CreateImClientL()->MaxTransactionContentLengthL() )
                    {
                    User::Leave( KErrOverflow );
                    }

                // Get file content
                HBufC8* content = HBufC8::NewLC( fileSize );
                TPtr8 ptr( content->Des() );
                User::LeaveIfError( file.Read( ptr, fileSize ) );

                // Set content to message, ownership is transferred
                CleanupStack::Pop( content );
                CleanupStack::PopAndDestroy();  // file.Close()
                iCurrentMessage->SetProcessState( MCAMessage::EContentReady );
                iCurrentMessage->SetContentData( content );

                // Complete
                CompleteRequestL();
                break;
                }

            TSize decodeSize( DecodeSize( origSize, iTargetSize, fullyScaleable ) );

            __CHAT_ASSERT_DEBUG( !iBitmap );
            iBitmap = new ( ELeave ) CFbsBitmap();
            User::LeaveIfError( iBitmap->Create( decodeSize,
                                                 iDecoder->FrameInfo().iFrameDisplayMode ) );
            iDecoder->Convert( &iStatus, *iBitmap );
            if ( !IsActive() )
                {
                SetActive();
                }
            break;
            }
        case MCAMessage::EBitmapDecoded:        // scale bitmap to size 320x240
            {
            TSize currentSize( iBitmap ? iBitmap->SizeInPixels() : iTargetSize );

            if ( currentSize != iTargetSize )
                {
                // bitmap exists and the size is wrong,
                // we have to scale the bitmap
                __CHAT_ASSERT_DEBUG( !iScaler );
                iScaler = CBitmapScaler::NewL();
                iScaler->Scale( &iStatus, *iBitmap, iTargetSize );
                if ( !IsActive() )
                    {
                    SetActive();
                    }
                }
            else
                {
                // the size is already correct => skip this step
                aMessage.SetProcessState( MCAMessage::EBitmapScaled );
                StartProcessingL( aMessage );
                }
            break;
            }
        case MCAMessage::EBitmapScaled:         // encode bitmap for sending
            {
            __CHAT_ASSERT_DEBUG( !iEncoder );
            iEncoder = CImageEncoder::DataNewL( iContent, aMessage.MimeType() );
            iEncoder->Convert( &iStatus, *iBitmap );
            if ( !IsActive() )
                {
                SetActive();
                }
            break;
            }
        case MCAMessage::EContentReady:         // decode bitmap for creating thumbnail
            {
            __CHAT_ASSERT_DEBUG( !iDecoder );
            iDecoder = CImageDecoder::DataNewL( iFs, aMessage.ContentData() );

            CCAContentMessage* message =
                static_cast< CCAContentMessage* >( iCurrentMessage );
            iThumbSize = message->ThumbnailSize();

            TSize origSize( iDecoder->FrameInfo().iOverallSizeInPixels );
            TBool fullyScaleable( iDecoder->FrameInfo().iFlags & TFrameInfo::EFullyScaleable );

            TSize decodeSize( DecodeSize( origSize, iThumbSize, fullyScaleable ) );

            __CHAT_ASSERT_DEBUG( !iBitmap );
            iBitmap = new ( ELeave ) CFbsBitmap();
            User::LeaveIfError( iBitmap->Create( decodeSize,
                                                 iDecoder->FrameInfo().iFrameDisplayMode ) );
            iDecoder->Convert( &iStatus, *iBitmap );
            if ( !IsActive() )
                {
                SetActive();
                }
            break;
            }
        case MCAMessage::EContentDecoded:       // scale bitmap to thumbnail size.
            {
            TSize currentSize( iBitmap ? iBitmap->SizeInPixels() : iThumbSize );

            if ( currentSize != iThumbSize )
                {
                // bitmap exists and the size is wrong,
                // we have to scale the bitmap
                __CHAT_ASSERT_DEBUG( !iScaler );
                iScaler = CBitmapScaler::NewL();
                iScaler->Scale( &iStatus, *iBitmap, iThumbSize );
                if ( !IsActive() )
                    {
                    SetActive();
                    }
                }
            else
                {
                // bitmap size is already correct,
                // set the thumbnail and complete request
                CCAContentMessage* message =
                    static_cast< CCAContentMessage* >( iCurrentMessage );
                message->SetThumbnail( iBitmap );
                iBitmap = NULL;
                aMessage.SetProcessState( MCAMessage::EThumbnailReady );
                CompleteRequestL();
                }
            break;
            }
#ifdef RD_SEND_NOT_SUPPORTED_CONTENT
        case MCAMessage::EContentNotSupported:
        case MCAMessage::EContentNotSupportedDrm:
        case MCAMessage::EContentCorrupted:
            {
            RFileReadStream fileStream;
            User::LeaveIfError( fileStream.Open(	iFs,
                                                 aMessage.Text(),
                                                 EFileRead ) );
            CleanupClosePushL( fileStream );
            TInt length = fileStream.Source()->SizeL();
            HBufC8* temp = HBufC8::NewLC( length );
            TPtr8 ptr( temp->Des() );
            fileStream.ReadL( ptr, length );
            CleanupStack::Pop( temp );
            iCurrentMessage->SetContentData( temp );
            CleanupStack::PopAndDestroy();  // fileStream
            CompleteRequestL();
            break;
            }
#endif //RD_SEND_NOT_SUPPORTED_CONTENT
        default:
            {
            // State is not known for processing. Complete it.
            CompleteRequestL();
            break;
            }
        }
    CHAT_DP_FUNC_DONE( "CCAImageLoader::StartProcessingL" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::CancelProcessing
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::CancelProcessing( MCAMessage& aMessage )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::CancelProcessing" );
    if ( iCurrentMessage == &aMessage )
        {
        if ( IsActive() )
            {
            Cancel();
            }
        iCurrentMessage = NULL;
        }
    CHAT_DP_FUNC_DONE( "CCAImageLoader::CancelProcessing" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::CancelProcessing
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::RemoveProcessingL( MCAMessage& aMessage )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::RemoveProcessingL" );
    // Cancel if message is processed at the moment.
    CancelProcessing( aMessage );
    // Remove message from scheduler
    TInt messageIndex( iMessages.Find( &aMessage ) );
    if ( messageIndex >= 0 )
        {
        iMessages.Remove( messageIndex );
        LaunchProcessingL();
        }
    CHAT_DP_FUNC_DONE( "CCAImageLoader::RemoveProcessingL" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::LaunchProcessing
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::LaunchProcessingL()
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::LaunchProcessingL" );
    TInt count( iMessages.Count() );
    if ( count > 0 && !IsActive() )
        {
        TRAPD( error, StartProcessingL( *iMessages[ 0 ] ) );
        if ( error == KErrNoMemory )
            {
            // if no memory: stop the whole process
            TRAP_IGNORE( ResetProcessingL( error ) );
            }
        else if ( error < 0 )
            {
            iMessages[ 0 ]->SetProcessState( MCAMessage::EContentCorrupted );
            TInt ignore;
            TRAP( ignore, CompleteRequestL() );
            User::Leave( error );
            }
        }
    CHAT_DP_FUNC_DONE( "CCAImageLoader::LaunchProcessingL" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::DecodeSize
// Scale to nearest (larger or equal) size with ratio 1:1, 1:2, 1:4 or 1:8
//-----------------------------------------------------------------------------
TSize CCAImageLoader::DecodeSize( const TSize& aSize, const TSize& aTargetSize,
                                  TBool aAnyRatio )
    {
    if ( aAnyRatio || aSize == aTargetSize )
        {
        // decoder can scale to any ratio or the size is already correct
        return aTargetSize;
        }

    // 1:1 is always valid ratio for decode scaling
    TInt lastValidRatio( 1 );
    for ( TInt ratio( KDecodeScaleRatioMin ); ratio <= KDecodeScaleRatioMax; ratio <<= 1 )
        {
        if ( aSize.iWidth % ratio + aSize.iHeight % ratio == 0 )
            {
            // this ratio is valid
            if ( aSize.iWidth / ratio < aTargetSize.iWidth ||
                 aSize.iHeight / ratio < aTargetSize.iHeight )
                {
                // the decoded size was smaller in some dimension,
                // the last valid ratio should be used
                break;
                }

            // this scale ratio results to greater or equal size
            lastValidRatio = ratio;
            }
        }

    // return the size scaled with correct ratio
    return TSize( aSize.iWidth / lastValidRatio,
                  aSize.iHeight / lastValidRatio );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::ScaleSize
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TBool CCAImageLoader::ScaleSize( const TSize& aSize, TSize& aNewSize )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::ScaleSize" );
    if ( Max( aSize.iHeight, aSize.iWidth ) > KChatCustomBitmapWidth )
        {
        // scaling needed
        if ( aSize.iHeight > aSize.iWidth )
            {
            aNewSize.SetSize( KChatCustomBitmapHeight, KChatCustomBitmapWidth );
            }
        else
            {
            aNewSize.SetSize( KChatCustomBitmapWidth, KChatCustomBitmapHeight );
            }
        return ETrue;
        }

    // old size is correct
    aNewSize = aSize;
    CHAT_DP_FUNC_DONE( "CCAImageLoader::ScaleSize" );
    return EFalse;
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::CompleteRequestL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::CompleteRequestL()
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::CompleteRequestL" );
    iCurrentMessage->HandleProcessingComplete( *this, iStatus.Int() );
    TInt index = iMessages.Find( iCurrentMessage );
    if ( index >= 0 )
        {
        iMessages.Remove( index );
        }
    iCurrentMessage = NULL;
    LaunchProcessingL();
    CHAT_DP_FUNC_DONE( "CCAImageLoader::CompleteRequestL" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::ResetProcessingL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::ResetProcessingL( TInt aReason )
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::ResetProcessingL" );
    // cancel operation
    if ( IsActive() )
        {
        iStatus = aReason;
        Cancel();
        }

    // show note
    if ( aReason == KErrNoMemory )
        {
        CActiveScheduler::Current()->Error( KErrNoMemory );
        }

    // reset all pending messages
    TInt count = iMessages.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        iMessages[ i ]->SetProcessState( MCAMessage::EContentCorrupted );
        iMessages[ i ]->HandleProcessingComplete( *this, aReason );
        iMessages.Remove( i );
        }
    CHAT_DP_FUNC_DONE( "CCAImageLoader::ResetProcessingL" );
    }

//-----------------------------------------------------------------------------
// CCAImageLoader::ClearState
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAImageLoader::ClearState()
    {
    CHAT_DP_FUNC_ENTER( "CCAImageLoader::ClearState" );
    if ( iScaler )
        {
        CHAT_DP_TXT( "CCAImageLoader::ClearState - Cancel scaler" );
        iScaler->Cancel();
        delete iScaler;
        iScaler = NULL;
        }
    if ( iDecoder )
        {
        CHAT_DP_TXT( "CCAImageLoader::ClearState - Cancel decoder" );
        iDecoder->Cancel();
        delete iDecoder;
        iDecoder = NULL;
        }
    if ( iEncoder )
        {
        CHAT_DP_TXT( "CCAImageLoader::ClearState - Cancel encoder" );
        iEncoder->Cancel();
        delete iEncoder;
        iEncoder = NULL;
        }
    if ( iBitmap )
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    CHAT_DP_FUNC_DONE( "CCAImageLoader::ClearState" );
    }

// End of file
