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
* Description:  Online Status attribute model implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVOnlineStatusConstruct.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <E32Base.h>
#include <S32Strm.h>



//  DATA TYPES
/**
 * Online status CSP name table.
 *
 * @since 3.0
 */
const TWVCspAttributeNameEntry KOnlineStatusCspNameTable[] =
    {
        {
        EWVCspV11,
        LIT_AS_DESC8_PTR( KOnlineStatusXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS )
        },
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KOnlineStatusXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS_CSP12 )
        }
    };


const TInt KOnlineStatusCspNameTableCount = sizeof( KOnlineStatusCspNameTable ) / sizeof( TWVCspAttributeNameEntry );




// =============== CPEngWVOnlineStatusConstruct MEMBER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KOnlineStatusCspNameTable,
                                         KOnlineStatusCspNameTableCount )
    {
    }


// Destructor
CPEngWVOnlineStatusConstruct::~CPEngWVOnlineStatusConstruct()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVOnlineStatusConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVOnlineStatusModel* model = new ( ELeave ) CPEngWVOnlineStatusModel(
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }




// ================= CPEngWVOnlineStatusModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::CPEngWVOnlineStatusModel(
    TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::~CPEngWVOnlineStatusModel()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::SetDataL( TInt* aIntData,
                                                                        const TDesC8* /*a8Data*/,
                                                                        const TDesC16* /*a16Data*/,
                                                                        TInt aField,
                                                                        TInt aGroup )
    {
    if ( aIntData &&
         ( aGroup == KPEngDefaultAttrValueGroup ) &&
         ( aField == EPEngOnlineStatus ) )
        {
        //check the data value
        switch ( *aIntData )
            {
            case EPEngOnlineStatusOffline:
            case EPEngOnlineStatusOnline:
                {
                iOnlineStatus = static_cast<TPEngWVOnlineStatusValues> ( *aIntData );
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
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::GetData( TInt* aIntData,
                                                                       TPtrC8* /*a8Data*/,
                                                                       TPtrC16* /*a16Data*/,
                                                                       TInt aField,
                                                                       TInt aGroup ) const
    {
    if ( aIntData &&
         ( aGroup == KPEngDefaultAttrValueGroup ) &&
         ( aField == EPEngUsrAvailability ) )
        {
        *aIntData = iOnlineStatus;
        return ETrue;
        }

    return EFalse;
    }



// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::EncodeDataToXmlL(
    MPEngXMLSerializer& aSerializer ) const
    {
    aSerializer.StartTagL( KPresenceValueXMLTag );
        {
        switch ( iOnlineStatus )
            {
            case EPEngOnlineStatusOnline:
                {
                aSerializer.RawValueL( KXMLValueTrue );
                break;
                }
            case EPEngOnlineStatusOffline:
            default:
                {
                aSerializer.RawValueL( KXMLValueFalse );
                break;
                }
            }
        }
    aSerializer.EndTagL( KPresenceValueXMLTag );
    }



// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::DecodeDataFromXmlL(
    MPEngXMLParser& aParser,
    const TDesC8& aValueBlock )
    {
    if ( aParser.DecodeL( aValueBlock, KPresenceValueXMLTag, EFalse ) )
        {
        if ( aParser.ResultL().CompareF( KXMLValueTrue ) == 0 )
            {
            iOnlineStatus = EPEngOnlineStatusOnline;
            }
        else
            {
            iOnlineStatus = EPEngOnlineStatusOffline;
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::ResetDataL()
    {
    iOnlineStatus = EPEngOnlineStatusOffline;
    }


// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::DataExternalizeSize() const
    {
    return 1; //Online Status is stored to 8 bit
    }



// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::ExternalizeDataL(
    RWriteStream& aStream ) const
    {
    aStream.WriteInt8L( iOnlineStatus );
    }


// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::InternalizeDataL(
    RReadStream& aStream )
    {
    iOnlineStatus = static_cast<TPEngWVOnlineStatusValues> ( aStream.ReadInt8L() );
    }


// -----------------------------------------------------------------------------
// CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::HandleNetworkSessionStateChangeL()
// -----------------------------------------------------------------------------
//
void CPEngWVOnlineStatusConstruct::CPEngWVOnlineStatusModel::HandleNetworkSessionStateChangeL(
    TBool aSessionOpen )
    {
    if ( aSessionOpen )
        {
        iOnlineStatus = EPEngOnlineStatusOnline;
        }

    else
        {
        iOnlineStatus = EPEngOnlineStatusOffline;
        }
    }


//  End of File
