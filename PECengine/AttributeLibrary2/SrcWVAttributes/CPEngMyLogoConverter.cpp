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
* Description:  MyLogo converter.
*
*/

// INCLUDE FILES
#include "CPEngMyLogoConverter.h"
#include "PresenceDebugPrint.h"

#include <E32Std.h>
#include <PEngWVPresenceAttributes2.h>
#include <ImageConversion.h>
#include <BitmapTransforms.h>



//LOCAL constants
namespace
    {
    //MyLogo image frame
    const TInt KPEngMyLogoFrame = 0;

    //MyLogo converter panics
    _LIT( KPEngMyLogoConverterPanic, "MyLogoConv" );

    //MyLogo converter panic reasons
    enum TPEngMyLogoConverterPanicReasons
        {
        EConvUnkownConvertState,
        EConvNoImageDecoder,
        EConvNoMyLogoBitmap,
        EConvMyLogoBitmapExists,
        EConvMyLogoEncoderExists,
        EConvMyLogoScalerExists,
        EConvParamsMissingNotifyNewLogo,
        EConvParamsMissingVerifyMyLogoRequirements1,
        EConvParamsMissingVerifyMyLogoRequirements2
        };
    }




// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::FileNewL()
// -----------------------------------------------------------------------------
//
CPEngMyLogoConverter* CPEngMyLogoConverter::FileNewL( const TDesC& aFileName,
                                                      MPEngMyLogoConversionObserver& aObserver )
    {
    CPEngMyLogoConverter* self = new ( ELeave ) CPEngMyLogoConverter( aObserver );
    CleanupStack::PushL( self );
    self->ConstructFromFileL( aFileName );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::DataNewL()
// -----------------------------------------------------------------------------
//
CPEngMyLogoConverter* CPEngMyLogoConverter::DataNewL( const TDesC8& aImageData,
                                                      MPEngMyLogoConversionObserver& aObserver )
    {
    CPEngMyLogoConverter* self = new ( ELeave ) CPEngMyLogoConverter( aObserver );
    CleanupStack::PushL( self );
    self->ConstructFromDataL( aImageData );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngMyLogoConverter::~CPEngMyLogoConverter()
    {
    CActive::Cancel();
    iObserver.HandleConverterDestroy();

    delete iImageDataBuffer;
    delete iImageMimeTypeBuffer;
    delete iImageFileNameBuffer;
    delete iImageDecoder;
    delete iImageEncoder;
    delete iBitmapScaler;
    delete iMyLogoBitmap;

    iFs.Close();
    }


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::CPEngMyLogoConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngMyLogoConverter::CPEngMyLogoConverter( MPEngMyLogoConversionObserver& aObserver )
        : CActive( EPriorityIdle ),
        iState( EConverterIdle ),
        iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::ConstructFromFileL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::ConstructFromFileL( const TDesC& aImageFile )
    {
    User::LeaveIfError( iFs.Connect() );
    iImageFileNameBuffer = aImageFile.AllocL();

    if ( aImageFile.Length() > 0 )
        {
        RFile imageFile;
        CleanupClosePushL( imageFile );


        //Try open source file with different access modes
        TInt openErr = imageFile.Open( iFs, aImageFile, EFileShareReadersOnly );
        if ( openErr != KErrNone )
            {
            openErr = imageFile.Open( iFs, aImageFile, EFileShareAny | EFileStream );
            }
        User::LeaveIfError( openErr );


        //And read file contents to buffer
        TInt fileSize;
        User::LeaveIfError( imageFile.Size( fileSize ) );
        iImageDataBuffer = HBufC8::NewL( fileSize );
        TPtr8 imageData( iImageDataBuffer->Des() );

        User::LeaveIfError( imageFile.Read( imageData ) );

        CleanupStack::PopAndDestroy(); // imageFile


        //And open image decoder to buffer
        iImageDecoder = CImageDecoder::DataNewL( iFs,
                                                 *iImageDataBuffer,
                                                 CImageDecoder::EOptionAlwaysThread );
        }

    else
        {
        iImageDataBuffer = HBufC8::NewL( 0 );
        iImageMimeTypeBuffer = HBufC8::NewL( 0 );
        }
    }



// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::ConstructFromDataL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::ConstructFromDataL( const TDesC8& aImageData )
    {
    iImageFileNameBuffer = HBufC::NewL( 0 );

    if ( aImageData.Length() > 0 )
        {
        //Make a local copy from image
        iImageDataBuffer = aImageData.AllocL();

        //And open image decoder to buffer
        iImageDecoder = CImageDecoder::DataNewL( iFs,
                                                 *iImageDataBuffer,
                                                 CImageDecoder::EOptionAlwaysThread );
        }

    else
        {
        iImageDataBuffer = HBufC8::NewL( 0 );
        iImageMimeTypeBuffer = HBufC8::NewL( 0 );
        }
    }

// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::IssueConvert()
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::IssueConvert( TRequestStatus& aRequestStatus )
    {
    //Issue first image processing round
    iStatus = KRequestPending;
    SetStateActive( EConverterIdle );

    TRequestStatus *s = &iStatus;
    User::RequestComplete( s, KErrNone );

    //And cache client request status so it can be signalled later
    //Client request status must be also set to pending state..
    iClientRequest = &aRequestStatus;
    *iClientRequest = KRequestPending;
    }


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::RunL()
// From CActive
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::RunL()
    {
    PENG_DP( D_PENG_LIT( "CPEngMyLogoConverter::RunL() iState[%d], iStatus[%d]" ),
             iState, iStatus.Int() );

    switch ( iState )
        {
        case EConverterIdle:
            {
            if ( VerifyMyLogoRequirementsL() )
                {
                NotifyNewLogo();
                ConversionCompletedD( KErrNone );
                }

            else
                {
                IssueImageOpenToBitmapL();
                SetStateActive( EImageOpenToBitmap );
                }

            break;
            }

        case EImageOpenToBitmap:
            {
            User::LeaveIfError( iStatus.Int() );

            delete iImageDecoder;
            iImageDecoder = NULL;

            IssueBitmapScaleDownL();
            SetStateActive( EBitmapScaleDown );
            break;
            }


        case EBitmapScaleDown:
            {
            User::LeaveIfError( iStatus.Int() );

            delete iBitmapScaler;
            iBitmapScaler = NULL;

            IssueBitmapStoreToImageL();
            SetStateActive( EBitmapStoreToImage );
            break;
            }

        case EBitmapStoreToImage:
            {
            User::LeaveIfError( iStatus.Int() );
            if ( iImageDataBuffer->Size() > KPEngWVMyLogoImageMaxByteSize )
                {
                User::Leave( KErrTooBig );
                }

            NotifyNewLogo();
            ConversionCompletedD( KErrNone );
            break;
            }


        default:
            {
            PanicConverter( EConvUnkownConvertState );
            break;
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::RunError()
// From CActive
// -----------------------------------------------------------------------------
//
TInt CPEngMyLogoConverter::RunError( TInt aError )
    {
    PENG_DP( D_PENG_LIT( "CPEngMyLogoConverter::RunError [%d]" ), aError );

    //RunL() has leaved. Stop the processing.
    ConversionCompletedD( aError );

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::DoCancel()
// From CActive
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::DoCancel()
    {
    //cancel the operation step currently running...
    switch ( iState )
        {
        case EConverterIdle:
            {
            //no issued request to cancel
            break;
            }

        case EImageOpenToBitmap:
            {
            iImageDecoder->Cancel();
            break;
            }

        case EBitmapScaleDown:
            {
            iBitmapScaler->Cancel();
            break;
            }

        case EBitmapStoreToImage:
            {
            iImageEncoder->Cancel();
            break;
            }

        default:
            {
            //nothing to do
            break;
            }
        }

    //and complete original client request
    User::RequestComplete( iClientRequest, KErrCancel );
    }


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::SetStateActive()
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::SetStateActive( TLogoConverterState aNewState )
    {
    SetActive();
    iState = aNewState;

    //Give other threads a chance
    User::After( 0 );       // CSI: 92 #
    }



// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::VerifyMyLogoRequirements()
// -----------------------------------------------------------------------------
//
TBool CPEngMyLogoConverter::VerifyMyLogoRequirementsL()
    {
    __ASSERT_DEBUG( iImageDataBuffer,
                    PanicConverter( EConvParamsMissingVerifyMyLogoRequirements1 ) );

    if ( iImageDataBuffer->Length() == 0 )
        {
        //Empty logo - no processing needed
        return ETrue;
        }


    //verify image properties
    __ASSERT_DEBUG( iImageDecoder && !iImageMimeTypeBuffer,
                    PanicConverter( EConvParamsMissingVerifyMyLogoRequirements2 ) );

    if ( iImageDecoder->FrameCount() == 0 )
        {
        User::Leave( KErrNotSupported );  //no frames in image
        }


    TFrameInfo fi;
    fi = iImageDecoder->FrameInfo( KPEngMyLogoFrame );


    TUid imageType;
    TUid imageSubType;
    iImageDecoder->ImageType( KPEngMyLogoFrame, imageType, imageSubType );


    //Check image type
    if ( ( imageType != KImageTypeGIFUid ) && ( imageType != KImageTypeJPGUid ) )
        {
        PENG_DP( D_PENG_LIT( "CPEngMyLogoConverter::VerifyMyLogoRequirementsL() - wrong image format [%08x]" ),
                 imageType.iUid );
        return EFalse;
        }

    //Check image pixel size
    if ( ( fi.iOverallSizeInPixels.iHeight > KPEngWVMyLogoImageMaxHeight ) ||
         ( fi.iOverallSizeInPixels.iWidth > KPEngWVMyLogoImageMaxWidth ) )
        {
        PENG_DP( D_PENG_LIT( "CPEngMyLogoConverter::VerifyMyLogoRequirementsL() - image pixel size too big h[%d] w[%d]" ),
                 fi.iOverallSizeInPixels.iHeight, fi.iOverallSizeInPixels.iWidth );
        return EFalse;
        }

    //Check image byte size
    if ( iImageDataBuffer->Size() > KPEngWVMyLogoImageMaxByteSize )
        {
        PENG_DP( D_PENG_LIT( "CPEngMyLogoConverter::VerifyMyLogoRequirementsL() - image byte size too big [%d]" ),
                 iImageDataBuffer->Size() );
        return EFalse;
        }


    //Perfectly OK logo image.
    if ( imageType == KImageTypeGIFUid )
        {
        iImageMimeTypeBuffer = KPEngWVMyLogoMimeImageGif().AllocL();
        }
    else
        {
        iImageMimeTypeBuffer = KPEngWVMyLogoMimeJpeg().AllocL();
        }

    return ETrue;
    }



// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::IssueImageOpenToBitmapL()
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::IssueImageOpenToBitmapL()
    {
    __ASSERT_DEBUG( !iMyLogoBitmap,
                    PanicConverter( EConvMyLogoBitmapExists ) );

    __ASSERT_DEBUG( iImageDecoder,
                    PanicConverter( EConvNoImageDecoder ) );


    //For future: This could be optimized to take just 1/8 sized image...
    TFrameInfo fi;
    fi = iImageDecoder->FrameInfo( KPEngMyLogoFrame );

    iMyLogoBitmap = new( ELeave ) CFbsBitmap;
    User::LeaveIfError( iMyLogoBitmap->Create( fi.iOverallSizeInPixels,
                                               fi.iFrameDisplayMode ) );

    iImageDecoder->Convert( &iStatus, *iMyLogoBitmap, KPEngMyLogoFrame );
    }



// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::IssueBitmapScaleDownL()
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::IssueBitmapScaleDownL()
    {
    __ASSERT_DEBUG( !iBitmapScaler,
                    PanicConverter( EConvMyLogoScalerExists ) );

    __ASSERT_DEBUG( iMyLogoBitmap,
                    PanicConverter( EConvNoMyLogoBitmap ) );


    iBitmapScaler = CBitmapScaler::NewL();
    iBitmapScaler->Scale( &iStatus,
                          *iMyLogoBitmap,
                          TSize( KPEngWVMyLogoImageMaxWidth, KPEngWVMyLogoImageMaxHeight ),
                          ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::IssueBitmapStoreToImageL()
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::IssueBitmapStoreToImageL()
    {
    __ASSERT_DEBUG( !iImageEncoder && !iImageMimeTypeBuffer,
                    PanicConverter( EConvMyLogoEncoderExists ) );

    __ASSERT_DEBUG( iMyLogoBitmap,
                    PanicConverter( EConvNoMyLogoBitmap ) );


    iImageMimeTypeBuffer = KPEngWVMyLogoMimeJpeg().AllocL();

    delete iImageDataBuffer;
    iImageDataBuffer = NULL;

    iImageEncoder = CImageEncoder::DataNewL( iImageDataBuffer,
                                             KPEngWVMyLogoMimeJpeg,
                                             CImageEncoder::EOptionAlwaysThread );
    iImageEncoder->SetThumbnail( EFalse );
    iImageEncoder->Convert( &iStatus, *iMyLogoBitmap );
    }



// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::NotifyNewLogo()
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::NotifyNewLogo()
    {
    __ASSERT_DEBUG( iImageDataBuffer && iImageMimeTypeBuffer && iImageFileNameBuffer,
                    PanicConverter( EConvParamsMissingNotifyNewLogo )  );



    iObserver.HandleNewMyLogo( iImageDataBuffer, iImageMimeTypeBuffer, iImageFileNameBuffer );

    //HandleNewMyLogo took the ownership of passed
    //iImageDataBuffer and iImageMimeTypeBuffer and iImageFileNameBuffer
    //==> NULL own member pointers
    iImageDataBuffer = NULL;
    iImageMimeTypeBuffer = NULL;
    iImageFileNameBuffer = NULL;
    }



// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::ConversionCompletedD()
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::ConversionCompletedD( TInt aStatus )
    {
    User::RequestComplete( iClientRequest, aStatus );
    delete this;
    }


// -----------------------------------------------------------------------------
// CPEngMyLogoConverter::PanicConverter()
// -----------------------------------------------------------------------------
//
void CPEngMyLogoConverter::PanicConverter( TInt aPanicReason ) const
    {
    User::Panic( KPEngMyLogoConverterPanic, aPanicReason );
    }



//  End of File


