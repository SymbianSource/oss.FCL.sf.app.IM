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
* Description:  WV InfoLink implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVInfoLinkConstruct.h"
#include "PEngWVAttributeModelDataHelper.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <PEngWVPresenceAttributes2.h>

#include <E32Base.h>
#include <S32Strm.h>



//  DATA TYPES
/**
 * InfoLink WV CSP name table and size.
 */
const TWVCspAttributeNameEntry KInfoLinkCspNameTable[] =
    {
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KInfoLinkXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS_CSP12 )
        }
    };

const TInt KInfoLinkCspNameTableCount = sizeof( KInfoLinkCspNameTable ) / sizeof( TWVCspAttributeNameEntry );




// =============== CPEngWVInfoLinkConstruct MEMBER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVInfoLinkConstruct::CPEngWVInfoLinkConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KInfoLinkCspNameTable,
                                         KInfoLinkCspNameTableCount )
    {
    }


// Destructor
CPEngWVInfoLinkConstruct::~CPEngWVInfoLinkConstruct()
    {
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVInfoLinkConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVInfoLinkModel* model = new ( ELeave ) CPEngWVInfoLinkModel(
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }







// ================= CPEngWVInfoLink MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngWVInfoLinkConstruct::CPEngWVInfoLink*
CPEngWVInfoLinkConstruct::CPEngWVInfoLink::NewLC()
    {
    CPEngWVInfoLink* self = new ( ELeave ) CPEngWVInfoLink();
    CleanupStack::PushL( self );
    return self;
    }

CPEngWVInfoLinkConstruct::CPEngWVInfoLink*
CPEngWVInfoLinkConstruct::CPEngWVInfoLink::NewLC( RReadStream& aStream )
    {
    CPEngWVInfoLink* self = new ( ELeave ) CPEngWVInfoLink();
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::CPEngWVInfoLink
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVInfoLinkConstruct::CPEngWVInfoLink::CPEngWVInfoLink()
    {
    }


// Destructor
CPEngWVInfoLinkConstruct::CPEngWVInfoLink::~CPEngWVInfoLink()
    {
    delete iInfoLink;
    delete iInfoLinkDescr;
    delete iInfoLinkMimeType;
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::SetUrlL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLink::SetUrlL( const TDesC& aUrl )
    {
    DoSetDescL( iInfoLink, aUrl );
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::SetDescriptionL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLink::SetDescriptionL( const TDesC& aDescr )
    {
    DoSetDescL( iInfoLinkDescr, aDescr );
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::SetMimeTypeL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLink::SetMimeTypeL( const TDesC& aMimeType )
    {
    DoSetDescL( iInfoLinkMimeType, aMimeType );
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::GetUrl()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLink::GetUrl( TPtrC16& aUrl ) const
    {
    DoGetDesc( iInfoLink, aUrl );
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::GetDescription()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLink::GetDescription( TPtrC16& aDescr ) const
    {
    DoGetDesc( iInfoLinkDescr, aDescr );
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::GetMimeType()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLink::GetMimeType( TPtrC16& aMimeType ) const
    {
    DoGetDesc( iInfoLinkMimeType, aMimeType );
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::ExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVInfoLinkConstruct::CPEngWVInfoLink::ExternalizeSize() const
    {
    TInt size = DoGetBufExtSize( iInfoLink );
    size += DoGetBufExtSize( iInfoLinkDescr );
    size += DoGetBufExtSize( iInfoLinkMimeType );
    return size;
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLink::ExternalizeL(
    RWriteStream& aStream ) const
    {
    DoExtrBufL( iInfoLink, aStream );
    DoExtrBufL( iInfoLinkDescr, aStream );
    DoExtrBufL( iInfoLinkMimeType, aStream );
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLink::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLink::InternalizeL( RReadStream& aStream )
    {
    TPtr16 dataStrait( NULL, 0 );
    DoIntrBufL( iInfoLink, dataStrait, aStream );
    DoIntrBufL( iInfoLinkDescr, dataStrait, aStream );
    DoIntrBufL( iInfoLinkMimeType, dataStrait, aStream );
    }






// ================= CPEngWVInfoLinkModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::CPEngWVInfoLinkModel(
    TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::~CPEngWVInfoLinkModel()
    {
    iInfoLinks.ResetAndDestroy();
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::SetDataL( TInt* aIntData,
                                                                const TDesC8* /*a8Data*/,
                                                                const TDesC16* a16Data,
                                                                TInt aField,
                                                                TInt aGroup )
    {
    TBool handled = EFalse;

    if ( a16Data )
        {
        //Updating infoLink struct contents

        if ( aGroup == iInfoLinks.Count() )
            {
            // initialize the cell where the new data is about to be put
            CPEngWVInfoLink* link = CPEngWVInfoLink::NewLC();
            handled = SetInfoLinkDataL( *link, *a16Data, aField );
            if ( handled )
                {
                iInfoLinks.AppendL( link );
                CleanupStack::Pop( link );
                }
            else
                {
                CleanupStack::PopAndDestroy( link );
                }
            }
        else
            {
            CPEngWVInfoLink* link = iInfoLinks[ aGroup ];
            handled = SetInfoLinkDataL( *link, *a16Data, aField );
            }
        }



    else if ( aIntData )
        {
        //Editing infoLink struct amount
        if ( ( aGroup == EPEngInfoLinkOperations ) &&
             ( aField == EPEngInfoLinkOpDeleteGroup ) )
            {
            TInt index = *aIntData;

            //If client gives special index -1, all existing infolinks are deleted
            if ( index == -1 )
                {
                iInfoLinks.ResetAndDestroy();
                }
            else
                {
                delete iInfoLinks[ *aIntData ];
                iInfoLinks.Remove( *aIntData );
                }
            handled = ETrue;
            }
        }

    return handled;
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::GetData( TInt* aIntData,
                                                               TPtrC8* /*a8Data*/,
                                                               TPtrC16* a16Data,
                                                               TInt aField,
                                                               TInt aGroup ) const
    {
    TBool handled = EFalse;

    if ( a16Data )
        {
        const CPEngWVInfoLink& infoLink = *iInfoLinks[ aGroup ];
        handled = GetInfoLinkData( infoLink, aField, *a16Data );
        }

    else if ( aIntData )
        {
        if ( ( aGroup == EPEngInfoLinkOperations ) &&
             ( aField == EPEngInfoLinkOpGetGroupCount ) )
            {
            *aIntData = iInfoLinks.Count();
            handled = ETrue;
            }
        }


    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::EncodeDataToXmlL(
    MPEngXMLSerializer& aSerializer ) const
    {
    const TInt infLinkCount = iInfoLinks.Count();
    for ( TInt ix = 0; ix < infLinkCount; ix++ )
        {
        const CPEngWVInfoLink& infoLink = *iInfoLinks[ ix ];
        TPtrC16 data( NULL, 0 );

        aSerializer.StartTagL( KInfoLinkXMLTagInf );

        infoLink.GetUrl( data );
        aSerializer.StartTagL( KInfoLinkXMLTagLink )
        .UnicodeTextL( data )
        .EndTagL( KInfoLinkXMLTagLink );

        infoLink.GetDescription( data );
        aSerializer.StartTagL( KInfoLinkXMLTagText )
        .UnicodeTextL( data )
        .EndTagL( KInfoLinkXMLTagText );

        infoLink.GetMimeType( data );
        aSerializer.StartTagL( KContentTypeXMLTag )
        .UnicodeTextL( data )
        .EndTagL( KContentTypeXMLTag );


        aSerializer.EndTagL( KInfoLinkXMLTagInf );
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::DecodeDataFromXmlL(
    MPEngXMLParser& aParser,
    const TDesC8& aValueBlock )
    {
    RArray<TPtrC8> infoLinkDataBlocks;
    CleanupClosePushL( infoLinkDataBlocks );

    iInfoLinks.ResetAndDestroy();

    // parse the InfoLink attribute values
    if ( aParser.DecodeL( aValueBlock, KInfoLinkXMLTagInf, ETrue ) )
        {
        const TInt resCount( aParser.Count() );
        TInt ix;
        for ( ix = 0; ix < resCount; ix++ )
            {
            User::LeaveIfError( infoLinkDataBlocks.Append( aParser.ResultL( ix ) ) );
            }


        for ( ix = 0; ix < resCount; ix++ )
            {
            TPtrC8 infoLinkBlock( infoLinkDataBlocks[ ix ] );
            CPEngWVInfoLink* infoLink = CPEngWVInfoLink::NewLC();

            TBool mandatoryFieldsFound = EFalse;
            if ( aParser.DecodeL( infoLinkBlock, KInfoLinkXMLTagLink, EFalse ) )
                {
                HBufC* tmp = aParser.ResultAsUnicodeTextL();

                CleanupStack::PushL( tmp );
                infoLink->SetUrlL( *tmp );
                CleanupStack::PopAndDestroy(); // tmp

                mandatoryFieldsFound = ETrue;
                }


            if ( aParser.DecodeL( infoLinkBlock, KInfoLinkXMLTagText, EFalse ) )
                {
                HBufC* tmp = aParser.ResultAsUnicodeTextL();

                CleanupStack::PushL( tmp );
                infoLink->SetDescriptionL( *tmp );
                CleanupStack::PopAndDestroy(); // tmp
                }


            if ( aParser.DecodeL( infoLinkBlock, KContentTypeXMLTag, EFalse ) )
                {
                HBufC* tmp = aParser.ResultAsUnicodeTextL();

                CleanupStack::PushL( tmp );
                infoLink->SetMimeTypeL( *tmp );
                CleanupStack::PopAndDestroy(); // tmp
                }

            if ( mandatoryFieldsFound )
                {
                iInfoLinks.AppendL( infoLink );
                CleanupStack::Pop( infoLink );
                }
            else
                {
                CleanupStack::PopAndDestroy( infoLink );
                }
            }
        }

    CleanupStack::PopAndDestroy(); // infoLinks
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::ResetDataL()
    {
    iInfoLinks.ResetAndDestroy();
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::DataExternalizeSize() const
    {
    TInt dataSize = 4; // 4 is the max size of the count

    const TInt infLinkCount( iInfoLinks.Count() );
    for ( TInt ix = 0; ix < infLinkCount; ix++ )
        {
        dataSize += iInfoLinks[ ix ]->ExternalizeSize();
        }

    return dataSize;
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::ExternalizeDataL(
    RWriteStream& aStream ) const
    {
    const TInt infLinkCount( iInfoLinks.Count() );
    aStream << TCardinality( infLinkCount );

    for ( TInt ix = 0; ix < infLinkCount; ix++ )
        {
        iInfoLinks[ ix ]->ExternalizeL( aStream );
        }
    }


// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::InternalizeDataL(
    RReadStream& aStream )
    {
    iInfoLinks.ResetAndDestroy();

    TCardinality c;
    aStream >> c;   //read the length bits
    const TInt infLinkCount = c;
    for ( TInt ix = 0 ; ix < infLinkCount; ix++ )
        {
        CPEngWVInfoLink* infoLink = CPEngWVInfoLink::NewLC( aStream );
        User::LeaveIfError( iInfoLinks.Append( infoLink ) );
        CleanupStack::Pop( infoLink );
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::SetInfoLinkDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::SetInfoLinkDataL(
    CPEngWVInfoLink& aInfoLink,
    const TDesC16& aData,
    TInt aInfoLinkFieldId )
    {
    TBool handled = EFalse;
    switch ( aInfoLinkFieldId )
        {
        case EPEngInfoLinkLink:
            {
            aInfoLink.SetUrlL( aData );
            handled = ETrue;
            break;
            }

        case EPEngInfoLinkDesc:
            {
            aInfoLink.SetDescriptionL( aData );
            handled = ETrue;
            break;
            }

        case EPEngInfoLinkMimeType:
            {
            aInfoLink.SetMimeTypeL( aData );
            handled = ETrue;
            break;
            }
        }

    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::GetInfoLinkData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVInfoLinkConstruct::CPEngWVInfoLinkModel::GetInfoLinkData(
    const CPEngWVInfoLink& aInfoLink,
    TInt aInfoLinkFieldId,
    TPtrC16& aData ) const
    {
    TBool handled = EFalse;
    switch ( aInfoLinkFieldId )
        {
        case EPEngInfoLinkLink:
            {
            aInfoLink.GetUrl( aData );
            handled = ETrue;
            break;
            }

        case EPEngInfoLinkDesc:
            {
            aInfoLink.GetDescription( aData );
            handled = ETrue;
            break;
            }

        case EPEngInfoLinkMimeType:
            {
            aInfoLink.GetMimeType( aData );
            handled = ETrue;
            break;
            }
        }

    return handled;
    }


//  End of File



