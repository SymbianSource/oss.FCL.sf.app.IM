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
* Description:  WV Thumbnail implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVThumbnailConstruct.h"
#include "PEngWVAttributeModelDataHelper.h"



#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <PEngWVPresenceAttributes2.h>


#include <E32Base.h>
#include <S32Strm.h>



//  DATA TYPES
/**
 * Thumbnail CSP name table.
 * NOTE: Thumbnail isn't published (XML name and namespace
 * ar thus empty.) However, the CSP name table is needed
 * to get the attribute working in different protocol
 * configurations.
 *
 * @since 3.0
 */
const TWVCspAttributeNameEntry KThumbnailCspNameTable[] =
    {
        {
        EWVCspV11,
        LIT_AS_DESC8_PTR( KNullDesC ),
        LIT_AS_DESC8_PTR( KNullDesC )
        },
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KNullDesC ),
        LIT_AS_DESC8_PTR( KNullDesC )
        }
    };


const TInt KThumbnailCspNameTableCount = sizeof( KThumbnailCspNameTable ) / sizeof( TWVCspAttributeNameEntry );




// =============== CPEngWVThumbnailConstruct MEMBER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVThumbnailConstruct::CPEngWVThumbnailConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KThumbnailCspNameTable,
                                         KThumbnailCspNameTableCount )
    {
    }


// Destructor
CPEngWVThumbnailConstruct::~CPEngWVThumbnailConstruct()
    {
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVThumbnailConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVThumbnailModel* model = new ( ELeave ) CPEngWVThumbnailModel(
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }




// ================= CPEngWVThumbnailModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailConstruct::CPEngWVThumbnailModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::CPEngWVThumbnailModel(
    TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::~CPEngWVThumbnailModel()
    {
    delete iDesC8Buf0;
    delete iDesC8Buf1;
    delete iDesC8Buf2;
    delete iDesC16Buf0;
    delete iDesC16Buf1;
    delete iDesC16Buf2;
    }


// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::SetDataL( TInt* /*aIntData*/,
                                                                  const TDesC8* a8Data,
                                                                  const TDesC16* a16Data,
                                                                  TInt aField,
                                                                  TInt aGroup )
    {
    TBool handled = EFalse;

    if ( aGroup == KPEngDefaultAttrValueGroup )
        {
        if ( a16Data )
            {
            handled = SetData16L( *a16Data, aField );
            }

        else if ( a8Data )
            {
            handled = SetData8L( *a8Data, aField );
            }
        }

    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::GetData( TInt* /*aIntData*/,
                                                                 TPtrC8* a8Data,
                                                                 TPtrC16* a16Data,
                                                                 TInt aField,
                                                                 TInt aGroup ) const
    {
    TBool handled = EFalse;


    if ( aGroup == KPEngDefaultAttrValueGroup )
        {
        if ( a16Data )
            {
            handled = GetData16( aField, *a16Data );
            }

        else if ( a8Data )
            {
            handled = GetData8( aField, *a8Data );
            }
        }


    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::EncodeDataToXmlL(
    MPEngXMLSerializer& /*aSerializer*/ ) const
    {
    //Thumbnail model doesn't support XML encode / decode
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::DecodeDataFromXmlL(
    MPEngXMLParser& /*aParser*/,
    const TDesC8& /*aValueBlock*/ )
    {
    //Thumbnail model doesn't support XML encode / decode
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::ResetDataL()
    {
    delete iDesC8Buf0;
    delete iDesC8Buf1;
    delete iDesC8Buf2;
    delete iDesC16Buf0;
    delete iDesC16Buf1;
    delete iDesC16Buf2;

    iDesC8Buf0 = NULL;
    iDesC8Buf1 = NULL;
    iDesC8Buf2 = NULL;
    iDesC16Buf0 = NULL;
    iDesC16Buf1 = NULL;
    iDesC16Buf2 = NULL;
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::DataExternalizeSize() const
    {
    TInt size = DoGetBufExtSize( iDesC8Buf0 );
    size += DoGetBufExtSize( iDesC8Buf1 );
    size += DoGetBufExtSize( iDesC8Buf2 );
    size += DoGetBufExtSize( iDesC16Buf0 );
    size += DoGetBufExtSize( iDesC16Buf1 );
    size += DoGetBufExtSize( iDesC16Buf2 );
    return size;
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::ExternalizeDataL(
    RWriteStream& aStream ) const
    {
    DoExtrBufL( iDesC8Buf0, aStream );
    DoExtrBufL( iDesC8Buf1, aStream );
    DoExtrBufL( iDesC8Buf2, aStream );
    DoExtrBufL( iDesC16Buf0, aStream );
    DoExtrBufL( iDesC16Buf1, aStream );
    DoExtrBufL( iDesC16Buf2, aStream );
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::InternalizeDataL(
    RReadStream& aStream )
    {
    TPtr8 typeStrait8( NULL, 0 );
    DoIntrBufL( iDesC8Buf0, typeStrait8, aStream );
    DoIntrBufL( iDesC8Buf1, typeStrait8, aStream );
    DoIntrBufL( iDesC8Buf2, typeStrait8, aStream );

    TPtr16 typeStrait16( NULL, 0 );
    DoIntrBufL( iDesC16Buf0, typeStrait16, aStream );
    DoIntrBufL( iDesC16Buf1, typeStrait16, aStream );
    DoIntrBufL( iDesC16Buf2, typeStrait16, aStream );
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::SetData8L()
// -----------------------------------------------------------------------------
//
TBool CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::SetData8L( const TDesC8& aData,
                                                                   TInt aField )
    {
    TBool handled = EFalse;
    switch ( aField ) //From TPEngMyLogoThumbnailDesC8Fields
        {
        case EPEngMyLogoThumbnailDesC8Buf0:
            {
            DoSetDescL( iDesC8Buf0, aData );
            handled = ETrue;
            break;
            }
        case EPEngMyLogoThumbnailDesC8Buf1:
            {
            DoSetDescL( iDesC8Buf1, aData );
            handled = ETrue;
            break;
            }

        case EPEngMyLogoThumbnailDesC8Buf2:
            {
            DoSetDescL( iDesC8Buf2, aData );
            handled = ETrue;
            break;
            }
        };

    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::GetData8()
// -----------------------------------------------------------------------------
//
TBool CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::GetData8( TInt aField,
                                                                  TPtrC8& aData ) const
    {
    TBool handled = EFalse;
    switch ( aField ) //From TPEngMyLogoThumbnailDesC8Fields
        {
        case EPEngMyLogoThumbnailDesC8Buf0:
            {
            DoGetDesc( iDesC8Buf0, aData );
            handled = ETrue;
            break;
            }
        case EPEngMyLogoThumbnailDesC8Buf1:
            {
            DoGetDesc( iDesC8Buf1, aData );
            handled = ETrue;
            break;
            }

        case EPEngMyLogoThumbnailDesC8Buf2:
            {
            DoGetDesc( iDesC8Buf2, aData );
            handled = ETrue;
            break;
            }
        };

    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::SetData16L()
// -----------------------------------------------------------------------------
//
TBool CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::SetData16L( const TDesC16& aData,
                                                                    TInt aField )
    {
    TBool handled = EFalse;
    switch ( aField ) //From TPEngMyLogoThumbnailDesC16Fields
        {
        case EPEngMyLogoThumbnailDesC16Buf0:
            {
            DoSetDescL( iDesC16Buf0, aData );
            handled = ETrue;
            break;
            }
        case EPEngMyLogoThumbnailDesC16Buf1:
            {
            DoSetDescL( iDesC16Buf1, aData );
            handled = ETrue;
            break;
            }

        case EPEngMyLogoThumbnailDesC16Buf2:
            {
            DoSetDescL( iDesC16Buf2, aData );
            handled = ETrue;
            break;
            }
        };

    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::GetData16()
// -----------------------------------------------------------------------------
//
TBool CPEngWVThumbnailConstruct::CPEngWVThumbnailModel::GetData16( TInt aField,
                                                                   TPtrC16& aData ) const
    {
    TBool handled = EFalse;
    switch ( aField ) //From TPEngMyLogoThumbnailDesC16Fields
        {
        case EPEngMyLogoThumbnailDesC16Buf0:
            {
            DoGetDesc( iDesC16Buf0, aData );
            handled = ETrue;
            break;
            }
        case EPEngMyLogoThumbnailDesC16Buf1:
            {
            DoGetDesc( iDesC16Buf1, aData );
            handled = ETrue;
            break;
            }

        case EPEngMyLogoThumbnailDesC16Buf2:
            {
            DoGetDesc( iDesC16Buf2, aData );
            handled = ETrue;
            break;
            }
        };

    return handled;
    }



//  End of File



