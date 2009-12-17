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
* Description:  WV Client Info implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVClientInfoConstruct.h"
#include "PEngWVAttributeModelDataHelper.h"
#include "PresenceAttributeDefValueCRKeys.h"


#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <E32Base.h>
#include <S32Strm.h>
#include <centralrepository.h>




//  DATA TYPES
/**
 * Client Info CSP name table,
 *
 * @since 3.0
 */
const TWVCspAttributeNameEntry KClientInfoCspNameTable[] =
    {
        {
        EWVCspV11,
        LIT_AS_DESC8_PTR( KClientInfoXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS )
        },
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KClientInfoXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS_CSP12 )
        }
    };

const TInt KClientInfoCspNameTableCount = sizeof( KClientInfoCspNameTable ) / sizeof( TWVCspAttributeNameEntry );




/**
 * Implemention restriction for maximum device detail
 * field length. Used when streaming the device details.
 */
const TInt KDeviceDetailDynamicMaxLength = 512;




// =============== LOCAL HELPER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CenRepValueL()
// Loads device detail value from CenRep.
// -----------------------------------------------------------------------------
//
HBufC* CenRepValueL( CRepository& aCenRep, TInt aId )
    {
    HBufC* valueBuffer = HBufC::NewLC( KDeviceDetailDynamicMaxLength );
    TPtr value( valueBuffer->Des() );

    //CenRep load status can be ignored
    //If the value isn't found or it doesn't fit to max size buffer
    //==> empty value is used
    aCenRep.Get( aId, value );

    HBufC* tmp = value.AllocL();
    CleanupStack::PopAndDestroy( valueBuffer );

    return tmp;
    }



// =============== CPEngWVClientInfoConstruct MEMBER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngWVClientInfoConstruct* CPEngWVClientInfoConstruct::NewL( TPEngWVCspVersion aCurrentCspVer )
    {
    CPEngWVClientInfoConstruct* self = new ( ELeave ) CPEngWVClientInfoConstruct(
        aCurrentCspVer );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }



// Destructor
CPEngWVClientInfoConstruct::~CPEngWVClientInfoConstruct()
    {
    delete iUserDeviceManufacturer;
    delete iUserDeviceModel;
    delete iUserClientProducer;
    delete iUserClientVersion;
    delete iUserClientLanguage;
    }


// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVClientInfoConstruct::CPEngWVClientInfoConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KClientInfoCspNameTable,
                                         KClientInfoCspNameTableCount )
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngWVClientInfoConstruct::ConstructL()
    {
    CRepository* cenRep = CRepository::NewLC( KCRUIDPresenceAttributeDefValue );

    iUserDeviceManufacturer = CenRepValueL( *cenRep, KPAWVClientInfoDevManufacturer );
    iUserDeviceModel = CenRepValueL( *cenRep, KPAWVClientInfoModel );

    iUserClientProducer = CenRepValueL( *cenRep, KPAWVClientInfoClientProducer );

    iUserClientVersion = CenRepValueL( *cenRep, KPAWVClientInfoClientVersion );
    iUserClientLanguage = CenRepValueL( *cenRep, KPAWVClientInfoLanguage );

    CleanupStack::PopAndDestroy(); //cenRep
    }




// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVClientInfoConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVClientInfoModel* model = new ( ELeave ) CPEngWVClientInfoModel(
        *this,
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }




// ================= CPEngWVClientInfoModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoConstruct::CPEngWVClientInfoModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::CPEngWVClientInfoModel(
    const CPEngWVClientInfoConstruct& aConstructor,
    TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute ),
        iConstructor( aConstructor )
    {
    }


// Destructor
CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::~CPEngWVClientInfoModel()
    {
    delete iDeviceManufacturer;
    delete iClientProducer;
    delete iDeviceModel;
    delete iClientVersion;
    delete iClientLanguage;
    }


// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::GetData( TInt* aIntData,
                                                                   TPtrC8* /*a8Data*/,
                                                                   TPtrC16* a16Data,
                                                                   TInt aField,
                                                                   TInt aGroup ) const
    {
    TBool handled = EFalse;

    if ( aGroup == KPEngDefaultAttrValueGroup )
        {
        if ( aIntData && ( aField == EPEngCliInfDeviceType ) )
            {
            *aIntData = iDeviceType;
            handled = ETrue;
            }


        else if ( a16Data )
            {
            switch ( aField ) //from TPEngWVClientInfoDesC16Fields
                {
                case EPEngCliInfDeviceManufacturer:
                    {
                    a16Data->Set( *iDeviceManufacturer );
                    handled = ETrue;
                    break;
                    }

                case EPEngCliInfClientProducer:
                    {
                    a16Data->Set( *iClientProducer );
                    handled = ETrue;
                    break;
                    }

                case EPEngCliInfDeviceModel:
                    {
                    a16Data->Set( *iDeviceModel );
                    handled = ETrue;
                    break;
                    }

                case EPEngCliInfClientVersion:
                    {
                    a16Data->Set( *iClientVersion );
                    handled = ETrue;
                    break;
                    }

                case EPEngCliInfClientLanguage:
                    {
                    a16Data->Set( *iClientLanguage );
                    handled = ETrue;
                    break;
                    }
                };
            }
        }

    return handled;
    }


// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::SetDataL( TInt* /*aIntData*/,
                                                                    const TDesC8* /*a8Data*/,
                                                                    const TDesC16* a16Data,
                                                                    TInt aField,
                                                                    TInt aGroup )
    {
    TBool handled = EFalse;

    if ( aGroup == KPEngDefaultAttrValueGroup )
        {
        if ( a16Data )
            {
            //check the data length
            if ( a16Data->Length() > KPEngMaxWVStatusTextLength )
                {
                User::Leave( KErrArgument );
                }

            switch ( aField ) //from TPEngWVClientInfoDesC16Fields
                {
                case EPEngCliInfDeviceManufacturer:
                    {
                    DoSetDescL( iDeviceManufacturer, *a16Data );
                    handled = ETrue;
                    break;
                    }

                case EPEngCliInfClientProducer:
                    {
                    DoSetDescL( iClientProducer, *a16Data );
                    handled = ETrue;
                    break;
                    }

                case EPEngCliInfDeviceModel:
                    {
                    DoSetDescL( iDeviceModel, *a16Data );
                    handled = ETrue;
                    break;
                    }

                case EPEngCliInfClientVersion:
                    {
                    DoSetDescL( iClientVersion, *a16Data );
                    handled = ETrue;
                    break;
                    }

                case EPEngCliInfClientLanguage:
                    {
                    DoSetDescL( iClientLanguage, *a16Data );
                    handled = ETrue;
                    break;
                    }
                };
            }
        }

    return handled;
    }


// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::EncodeDataToXmlL(
    MPEngXMLSerializer& aSerializer ) const
    {
    // Client type
    aSerializer.StartTagL( KClientTypeXMLTag );
        {
        switch ( iDeviceType )
            {
            case EPEngCliDevTypeMobilePhone:
                {
                aSerializer.RawValueL( KClientTypeMobilePhoneXMLTag );
                break;
                }
            case EPEngCliDevTypeComputer:
                {
                aSerializer.RawValueL( KClientTypeComputerXMLTag );
                break;
                }
            case EPEngCliDevTypePDA:
                {
                aSerializer.RawValueL( KClientTypePDAXMLTag );
                break;
                }
            case EPEngCliDevTypeCLI:
                {
                aSerializer.RawValueL( KClientTypeCLIXMLTag );
                break;
                }
            case EPEngCliDevTypeOther:
            default:
                {
                aSerializer.RawValueL( KClientTypeOtherXMLTag );
                break;
                }
            }
        }
    aSerializer.EndTagL( KClientTypeXMLTag );

    // Device manufacturer
    aSerializer.StartTagL( KDevManufacturerXMLTag )
    .UnicodeTextL( *iDeviceManufacturer )
    .EndTagL( KDevManufacturerXMLTag );

    // Client Producer
    aSerializer.StartTagL( KClientProducerXMLTag )
    .UnicodeTextL( *iClientProducer )
    .EndTagL( KClientProducerXMLTag );

    // Device Model
    aSerializer.StartTagL( KDeviceModelXMLTag )
    .UnicodeTextL( *iDeviceModel )
    .EndTagL( KDeviceModelXMLTag );

    // Client version
    aSerializer.StartTagL( KClientVersionXMLTag )
    .UnicodeTextL( *iClientVersion )
    .EndTagL( KClientVersionXMLTag );

    // Client language
    // Client languages are actually enumerations
    // ==> empty string in XML not allowed
    if ( iClientLanguage->Length() > 0 )
        {
        aSerializer.StartTagL( KClientLanguageXMLTag )
        .UnicodeTextL( *iClientLanguage )
        .EndTagL( KClientLanguageXMLTag );
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::DecodeDataFromXmlL(
    MPEngXMLParser& aParser,
    const TDesC8& aValueBlock )
    {
    // parse client type first
    if ( aParser.DecodeL( aValueBlock, KClientTypeXMLTag, EFalse ) )
        {
        if ( aParser.ResultL().CompareF( KClientTypeMobilePhoneXMLTag ) == 0 )
            {
            iDeviceType = EPEngCliDevTypeMobilePhone;
            }
        else if ( aParser.ResultL().CompareF( KClientTypeComputerXMLTag ) == 0 )
            {
            iDeviceType = EPEngCliDevTypeComputer;
            }
        else if ( aParser.ResultL().CompareF( KClientTypePDAXMLTag ) == 0 )
            {
            iDeviceType = EPEngCliDevTypePDA;
            }
        else if ( aParser.ResultL().CompareF( KClientTypeCLIXMLTag ) == 0 )
            {
            iDeviceType = EPEngCliDevTypeCLI;
            }
        else
            {
            iDeviceType = EPEngCliDevTypeOther;
            }
        }

    // Parse device manufacturer
    if ( aParser.DecodeL( aValueBlock, KDevManufacturerXMLTag, EFalse ) )
        {
        HBufC* tmpData = aParser.ResultAsUnicodeTextL();
        delete iDeviceManufacturer;
        iDeviceManufacturer = tmpData;
        }

    // Parse client producer
    if ( aParser.DecodeL( aValueBlock, KClientProducerXMLTag, EFalse ) )
        {
        HBufC* tmpData = aParser.ResultAsUnicodeTextL();
        delete iClientProducer;
        iClientProducer = tmpData;
        }

    // Parse device model
    if ( aParser.DecodeL( aValueBlock, KDeviceModelXMLTag, EFalse ) )
        {
        HBufC* tmpData = aParser.ResultAsUnicodeTextL();
        delete iDeviceModel;
        iDeviceModel = tmpData;
        }

    // Parse client version
    if ( aParser.DecodeL( aValueBlock, KClientVersionXMLTag, EFalse ) )
        {
        HBufC* tmpData = aParser.ResultAsUnicodeTextL();
        delete iClientVersion;
        iClientVersion = tmpData;
        }

    // Parse client language
    if ( aParser.DecodeL( aValueBlock, KClientLanguageXMLTag, EFalse ) )
        {
        HBufC* tmpData = aParser.ResultAsUnicodeTextL();
        delete iClientLanguage;
        iClientLanguage = tmpData;
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::ResetDataL()
    {
    HBufC* tmpDeviceManufacturer;
    HBufC* tmpDeviceModel;
    HBufC* tmpClientProducer;
    HBufC* tmpClientVersion;
    HBufC* tmpClientLanguage;


    if ( iUserOwnModel )
        {
        tmpDeviceManufacturer = iConstructor.iUserDeviceManufacturer->AllocLC();
        tmpDeviceModel = iConstructor.iUserDeviceModel->AllocLC();
        tmpClientProducer = iConstructor.iUserClientProducer->AllocLC();
        tmpClientVersion = iConstructor.iUserClientVersion->AllocLC();
        tmpClientLanguage = iConstructor.iUserClientLanguage->AllocLC();
        iDeviceType = EPEngCliDevTypeMobilePhone;
        }
    else
        {
        tmpDeviceManufacturer = HBufC::NewLC( 0 );
        tmpDeviceModel = HBufC::NewLC( 0 );
        tmpClientProducer = HBufC::NewLC( 0 );
        tmpClientVersion = HBufC::NewLC( 0 );
        tmpClientLanguage = HBufC::NewLC( 0 );
        iDeviceType = EPEngCliDevTypeOther;
        }


    delete iDeviceManufacturer;
    delete iClientProducer;
    delete iDeviceModel;
    delete iClientVersion;
    delete iClientLanguage;



    iClientLanguage = tmpClientLanguage;
    CleanupStack::Pop();  //tmpClientLanguage

    iClientVersion = tmpClientVersion;
    CleanupStack::Pop();  //tmpClientVersion

    iDeviceModel = tmpDeviceModel;
    CleanupStack::Pop();  //tmpDeviceModel

    iClientProducer = tmpClientProducer;
    CleanupStack::Pop();  //tmpClientProducer

    iDeviceManufacturer = tmpDeviceManufacturer;
    CleanupStack::Pop();  //tmpDeviceManufacturer
    }


// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::DataExternalizeSize() const
    {
    TInt size = 1; //iDeviceType is stored in one byte

    size += iDeviceManufacturer->Size();
    size += 4;       //4 bytes for descriptor length

    size += iClientProducer->Size();
    size += 4;       //4 bytes for descriptor length

    size += iDeviceModel->Size();
    size += 4;       //4 bytes for descriptor length

    size += iClientVersion->Size();
    size += 4;       //4 bytes for descriptor length

    size += iClientLanguage->Size();
    size += 4;       //4 bytes for descriptor length

    return size;
    }



// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::ExternalizeDataL(
    RWriteStream& aStream ) const
    {
    aStream.WriteInt8L( iDeviceType );  // device type

    aStream << iDeviceManufacturer->Left( KDeviceDetailDynamicMaxLength );
    aStream << iClientProducer->Left( KDeviceDetailDynamicMaxLength );
    aStream << iDeviceModel->Left( KDeviceDetailDynamicMaxLength );
    aStream << iClientVersion->Left( KDeviceDetailDynamicMaxLength );
    aStream << iClientLanguage->Left( KDeviceDetailDynamicMaxLength );
    }


// -----------------------------------------------------------------------------
// CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVClientInfoConstruct::CPEngWVClientInfoModel::InternalizeDataL(
    RReadStream& aStream )
    {
    iDeviceType = static_cast<TPEngWVCliInfDeviceTypeValues> ( aStream.ReadInt8L() );

    HBufC* tmpDeviceManufacturer = HBufC::NewLC( aStream, KDeviceDetailDynamicMaxLength );
    HBufC* tmpClientProducer = HBufC::NewLC( aStream, KDeviceDetailDynamicMaxLength );
    HBufC* tmpDeviceModel = HBufC::NewLC( aStream, KDeviceDetailDynamicMaxLength );
    HBufC* tmpClientVersion = HBufC::NewLC( aStream, KDeviceDetailDynamicMaxLength );
    HBufC* tmpClientLanguage = HBufC::NewLC( aStream, KDeviceDetailDynamicMaxLength );

    delete iDeviceManufacturer;
    delete iClientProducer;
    delete iDeviceModel;
    delete iClientVersion;
    delete iClientLanguage;

    iClientLanguage = tmpClientLanguage;
    CleanupStack::Pop();  //tmpClientLanguage

    iClientVersion = tmpClientVersion;
    CleanupStack::Pop();  //tmpClientVersion

    iDeviceModel = tmpDeviceModel;
    CleanupStack::Pop();  //tmpDeviceModel

    iClientProducer = tmpClientProducer;
    CleanupStack::Pop();  //tmpClientProducer

    iDeviceManufacturer = tmpDeviceManufacturer;
    CleanupStack::Pop();  //tmpDeviceManufacturer
    }



//  End of File


