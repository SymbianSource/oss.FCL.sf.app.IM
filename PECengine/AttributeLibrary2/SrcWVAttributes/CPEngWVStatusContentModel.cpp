/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  WV Status content attribute implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVStatusContentModel.h"
#include "CAsync2Sync.h"
#include "PEngWVAttributeModelDataHelper.h"



#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <E32Base.h>
#include <S32Strm.h>
#include <imcvcodc.h>



//LOCAL constants
namespace
    {
    _LIT( KPEngMyLogoModelPanic, "MyLogoMod" );
    enum TPEngMyLogoModelPanicReasons
        {
        EModAlreadyHasNwStatusContent,
        EModInvalidParamsHandleNewMyLogo
        };

    void PanicStatusContModel( TPEngMyLogoModelPanicReasons aPanicReason )
        {
        User::Panic( KPEngMyLogoModelPanic, aPanicReason );
        }
    }


// ================= CPEngWVStatusContentModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVStatusContentModel::CPEngWVStatusContentModel( TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVStatusContentModel::~CPEngWVStatusContentModel()
    {
    delete iMyLogoData;
    delete iMyLogoMimeType;
    delete iMyLogoSoureFileName;

    delete iStatusContentNW;
    delete iMyLogoConverter;    //cancels the active convert if any
    }




// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVStatusContentModel::GetData( TInt* /*aIntData*/,
                                          TPtrC8* a8Data,
                                          TPtrC16* a16Data,
                                          TInt aField,
                                          TInt aGroup ) const
    {
    TBool handled = EFalse;

    if ( aGroup == KPEngDefaultAttrValueGroup )
        {
        if ( a16Data && ( aField == EPEngMyLogoDirectContentSourceFileName ) )
            {
            DoGetDesc( iMyLogoSoureFileName, *a16Data );
            handled = ETrue;
            }


        if ( a8Data && ( aField == EPEngMyLogoDirectContentBuffer ) )
            {
            DoGetDesc( iMyLogoData, *a8Data );
            handled = ETrue;
            }
        }

    return handled;
    }




// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVStatusContentModel::SetDataL( TInt* /*aIntData*/,
                                           const TDesC8* a8Data,
                                           const TDesC16* a16Data,
                                           TInt aField,
                                           TInt aGroup )
    {
    CAsync2Sync* wait = CAsync2Sync::NewLC();

    TBool handled = CPEngWVStatusContentModel::SetDataAsyncL( a8Data,
                                                              a16Data,
                                                              wait->RequestStatus(),
                                                              aField,
                                                              aGroup );
    if ( handled )
        {
        wait->WaitToComplete();

        //handle all other errors than the cancel by leaving
        TInt status = wait->RequestStatus().Int();
        User::LeaveIfError( status );
        }

    CleanupStack::PopAndDestroy( wait );

    return handled;
    }


// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::SetDataAsyncL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVStatusContentModel::SetDataAsyncL( const TDesC8* a8Data,
                                                const TDesC16* a16Data,
                                                TRequestStatus& aRequestStatus,
                                                TInt aField,
                                                TInt aGroup )
    {
    TBool handled = EFalse;

    if ( aGroup == KPEngDefaultAttrValueGroup )
        {
        if ( a16Data && ( aField == EPEngMakeMyLogoDirectContentFromFile ) )
            {
            __ASSERT_ALWAYS( !iMyLogoConverter, User::Leave( KErrInUse ) );
            iMyLogoConverter = CPEngMyLogoConverter::FileNewL( *a16Data, *this );
            iMyLogoConverter->IssueConvert( aRequestStatus );
            handled = ETrue;
            }


        else if ( a8Data && ( aField == EPEngMakeMyLogoDirectContentFromBuffer ) )
            {
            __ASSERT_ALWAYS( !iMyLogoConverter, User::Leave( KErrInUse ) );
            iMyLogoConverter = CPEngMyLogoConverter::DataNewL( *a8Data, *this );
            iMyLogoConverter->IssueConvert( aRequestStatus );
            handled = ETrue;
            }
        }

    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::CancelSetDataAsync()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::CancelSetDataAsync()
    {
    delete iMyLogoConverter; //Deleting converter cancels it
    iMyLogoConverter = NULL;
    }


// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::EncodeDataToXmlL(
    MPEngXMLSerializer& aSerializer ) const
    {
    TPtrC8 logoData( NULL, 0 );
    TPtrC8 logoMimeType( NULL, 0 );

    DoGetDesc( iMyLogoData, logoData );
    DoGetDesc( iMyLogoMimeType, logoMimeType );


    if ( ( logoData.Length() > 0 ) && ( logoMimeType.Length() > 0 ) )
        {
        //Image & mime type for image are available
        //==>serialize them to XML
        aSerializer.StartTagL( KDirectContentXMLTag ).
        Base64DataL( logoData ).
        EndTagL( KDirectContentXMLTag );

        aSerializer.StartTagL( KContentTypeXMLTag )
        .RawValueL( logoMimeType )
        .EndTagL( KContentTypeXMLTag );
        }

    else
        {
        //write empty data
        aSerializer.StartTagL( KDirectContentXMLTag ).EndTagL( KDirectContentXMLTag );
        aSerializer.StartTagL( KContentTypeXMLTag ).EndTagL( KContentTypeXMLTag );
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::DecodeDataFromXmlL( MPEngXMLParser& aParser,
                                                    const TDesC8& aValueBlock )
    {
    __ASSERT_ALWAYS( !iStatusContentNW ,
                     PanicStatusContModel( EModAlreadyHasNwStatusContent ) );


    TPtrC8 mimeType( NULL, 0 );
    TPtrC8 statusContent( NULL, 0 );

    if ( aParser.DecodeL( aValueBlock, KContentTypeXMLTag, EFalse ) )
        {
        mimeType.Set( aParser.ResultL() );

        if ( aParser.DecodeL( aValueBlock, KDirectContentXMLTag, EFalse ) )
            {
            statusContent.Set( aParser.ResultL() );
            }
        }

    //Is status content MyLogo?
    if ( ( mimeType.CompareF( KPEngWVMyLogoMimeJpeg ) == 0 ) ||
         ( mimeType.CompareF( KPEngWVMyLogoMimeImageGif ) == 0 ) )
        {

        //statusContent length is big enough for decoded image
        //data, because in BASE64 decoding the result shrinks
        iStatusContentNW = HBufC8::NewL( statusContent.Length() );
        TPtr8 imageDataNW = iStatusContentNW->Des();

        TImCodecB64 base64Decoder;
        base64Decoder.Initialise();

        // Decode returns EFalse, if no more data is needed for decoding
        //==> in that case the decoding was succesful
        TBool decodeOk = !base64Decoder.Decode( statusContent, imageDataNW );
        if ( decodeOk )
            {
            iStatusContentNW = iStatusContentNW->ReAllocL( iStatusContentNW->Length() );
            }
        else
            {
            delete iStatusContentNW;
            iStatusContentNW = NULL;
            }
        }


    if ( !iStatusContentNW )
        {
        ResetDataL();
        }
    else
        {
        //there is some raw content. Verify & convert it to valid MyLogo
        TRAPD( err, CheckReceivedStatusContentL() );
        if ( err == KErrNoMemory )
            {
            User::Leave( KErrNoMemory );
            }

        else if ( ( err != KErrNone ) &&
                  ( err != KErrCancel ) )
            {
            ResetDataL();
            }
        }

    delete iStatusContentNW;
    iStatusContentNW = NULL;
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::ResetDataL()
    {
    delete iMyLogoSoureFileName;
    delete iMyLogoMimeType;
    delete iMyLogoData;

    iMyLogoSoureFileName = NULL;
    iMyLogoMimeType = NULL;
    iMyLogoData = NULL;
    }


// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVStatusContentModel::DataExternalizeSize() const
    {
    TInt size = DoGetBufExtSize( iMyLogoData );
    size += DoGetBufExtSize( iMyLogoMimeType );
    size += DoGetBufExtSize( iMyLogoSoureFileName );

    return size;
    }


// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::ExternalizeDataL( RWriteStream& aStream ) const
    {
    DoExtrBufL( iMyLogoData, aStream );
    DoExtrBufL( iMyLogoMimeType, aStream );
    DoExtrBufL( iMyLogoSoureFileName, aStream );
    }


// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::InternalizeDataL( RReadStream& aStream )
    {
    TPtr8 typeStrait8( NULL, 0 );
    DoIntrBufL( iMyLogoData, typeStrait8, aStream );
    DoIntrBufL( iMyLogoMimeType, typeStrait8, aStream );

    TPtr16 typeStrait16( NULL, 0 );
    DoIntrBufL( iMyLogoSoureFileName, typeStrait16, aStream );
    }


// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::CheckReceivedStatusContentL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::CheckReceivedStatusContentL()
    {
    CAsync2Sync* wait = CAsync2Sync::NewLC();
    iMyLogoConverter = CPEngMyLogoConverter::DataNewL( *iStatusContentNW, *this );
    iMyLogoConverter->IssueConvert( wait->RequestStatus() );
    wait->WaitToComplete();
    User::LeaveIfError( wait->RequestStatus().Int() );
    CleanupStack::PopAndDestroy( wait );
    }


// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::HandleNewMyLogo()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::HandleNewMyLogo( HBufC8* aImageData,
                                                 HBufC8* aImageMimeType,
                                                 HBufC* aImageSoureFileName )
    {
    __ASSERT_ALWAYS( aImageData && aImageMimeType && aImageSoureFileName,
                     PanicStatusContModel( EModInvalidParamsHandleNewMyLogo ) );

    //This must take ownership of the given HBufC's in all cases
    //so no leaving allowed
    delete iMyLogoData;
    delete iMyLogoMimeType;
    delete iMyLogoSoureFileName;

    iMyLogoData = aImageData;
    iMyLogoMimeType = aImageMimeType;
    iMyLogoSoureFileName = aImageSoureFileName;
    }


// -----------------------------------------------------------------------------
// CPEngWVStatusContentModel::HandleConverterDestroy()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusContentModel::HandleConverterDestroy()
    {
    iMyLogoConverter = NULL;
    }



//  End of File


