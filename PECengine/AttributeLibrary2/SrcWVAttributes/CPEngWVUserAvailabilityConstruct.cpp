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
* Description:  WV User Availability implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVUserAvailabilityConstruct.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <E32Base.h>
#include <S32Strm.h>



//  DATA TYPES
/**
 * Availability CSP name construct.
 *
 * @since 3.0
 */
const TWVCspAttributeNameEntry KUserAvailabilityCspNameTable[] =
    {
        {
        EWVCspV11,
        LIT_AS_DESC8_PTR( KUserAvailabilityXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS )
        },
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KUserAvailabilityXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS_CSP12 )
        }
    };


const TInt KUserAvailabilityCspNameTableCount = sizeof( KUserAvailabilityCspNameTable ) / sizeof( TWVCspAttributeNameEntry );




// =============== CPEngWVUserAvailabilityConstruct MEMBER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KUserAvailabilityCspNameTable,
                                         KUserAvailabilityCspNameTableCount )
    {
    }


// Destructor
CPEngWVUserAvailabilityConstruct::~CPEngWVUserAvailabilityConstruct()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVUserAvailabilityConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVUserAvailabilityModel* model = new ( ELeave ) CPEngWVUserAvailabilityModel(
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }




// ================= CPEngWVUserAvailabilityModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::CPEngWVUserAvailabilityModel(
    TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::~CPEngWVUserAvailabilityModel()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::SetDataL( TInt* aIntData,
                                                                                const TDesC8* /*a8Data*/,
                                                                                const TDesC16* /*a16Data*/,
                                                                                TInt aField,
                                                                                TInt aGroup )
    {
    if ( aIntData &&
         ( aGroup == KPEngDefaultAttrValueGroup ) &&
         ( aField == EPEngUsrAvailability ) )
        {
        //check the data value
        switch ( *aIntData )
            {
            case EPEngUsrAvailabilityOffline:
            case EPEngUsrAvailabilityDiscreet:
            case EPEngUsrAvailabilityOnline:
                {
                iAvailability = static_cast<TPEngWVUserAvailabilityValues> ( *aIntData );
                break;
                }

            default:
                {
                __AssertAttrEnumValueOutOfBoundsL();
                break;
                }
            }

        return ETrue;
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::GetData( TInt* aIntData,
                                                                               TPtrC8* /*a8Data*/,
                                                                               TPtrC16* /*a16Data*/,
                                                                               TInt aField,
                                                                               TInt aGroup ) const
    {
    if ( aIntData &&
         ( aGroup == KPEngDefaultAttrValueGroup ) &&
         ( aField == EPEngUsrAvailability ) )
        {
        *aIntData = iAvailability;
        return ETrue;
        }

    return EFalse;
    }



// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::EncodeDataToXmlL(
    MPEngXMLSerializer& aSerializer ) const
    {
    aSerializer.StartTagL( KPresenceValueXMLTag );
        {
        switch ( iAvailability )
            {
            case EPEngUsrAvailabilityDiscreet:
                {
                aSerializer.RawValueL( KAvailabilityValueDiscreet );
                break;
                }
            case EPEngUsrAvailabilityOnline:
                {
                aSerializer.RawValueL( KAvailabilityValueAvailable );
                break;
                }
            case EPEngUsrAvailabilityOffline:
            default:
                {
                aSerializer.RawValueL( KAvailabilityValueNotAvailable );
                break;
                }
            }
        }
    aSerializer.EndTagL( KPresenceValueXMLTag );
    }



// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::DecodeDataFromXmlL(
    MPEngXMLParser& aParser,
    const TDesC8& aValueBlock )
    {
    if ( aParser.DecodeL( aValueBlock, KPresenceValueXMLTag, EFalse ) )
        {
        if ( aParser.ResultL().CompareF( KAvailabilityValueAvailable ) == 0 )
            {
            iAvailability = EPEngUsrAvailabilityOnline;
            }
        else if ( aParser.ResultL().CompareF( KAvailabilityValueDiscreet ) == 0 )
            {
            iAvailability = EPEngUsrAvailabilityDiscreet;
            }
        else
            {
            iAvailability = EPEngUsrAvailabilityOffline;
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::ResetDataL()
    {
    iAvailability = EPEngUsrAvailabilityOffline;
    }


// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::DataExternalizeSize() const
    {
    return 1; //iAvailability is stored to 8 bit
    }



// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::ExternalizeDataL(
    RWriteStream& aStream ) const
    {
    aStream.WriteInt8L( iAvailability );
    }


// -----------------------------------------------------------------------------
// CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVUserAvailabilityConstruct::CPEngWVUserAvailabilityModel::InternalizeDataL(
    RReadStream& aStream )
    {
    iAvailability = static_cast<TPEngWVUserAvailabilityValues> ( aStream.ReadInt8L() );
    }



//  End of File


