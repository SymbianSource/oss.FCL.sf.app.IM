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
* Description:  WV Comm Cap implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVCommCapConstruct.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <E32Base.h>
#include <S32Strm.h>



//  DATA TYPES
/**
 * CommCap CSP name table.
 *
 * @since 3.0
 */
const TWVCspAttributeNameEntry KCommCapCspNameTable[] =
    {
        {
        EWVCspV11,
        LIT_AS_DESC8_PTR( KCommCapXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS )
        },
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KCommCapXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS_CSP12 )
        }
    };


const TInt KCommCapCspNameTableCount = sizeof( KCommCapCspNameTable ) / sizeof( TWVCspAttributeNameEntry );




// =============== CPEngWVCommCapConstruct MEMBER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVCommCapConstruct::CPEngWVCommCapConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KCommCapCspNameTable,
                                         KCommCapCspNameTableCount )
    {
    }


// Destructor
CPEngWVCommCapConstruct::~CPEngWVCommCapConstruct()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVCommCapConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVCommCapModel* model = new ( ELeave ) CPEngWVCommCapModel(
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }




// ================= CPEngWVCommCapModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapConstruct::CPEngWVCommCapModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVCommCapConstruct::CPEngWVCommCapModel::CPEngWVCommCapModel(
    TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVCommCapConstruct::CPEngWVCommCapModel::~CPEngWVCommCapModel()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVCommCapConstruct::CPEngWVCommCapModel::SetDataL( TInt* aIntData,
                                                              const TDesC8* /*a8Data*/,
                                                              const TDesC16* /*a16Data*/,
                                                              TInt aField,
                                                              TInt aGroup )
    {
    TBool handled = EFalse;

    if ( aIntData && ( aField == EPEngCommCapStatus ) )
        {
        switch ( aGroup ) //from TPEngCommunicationCapabilitiesFieldGroups
            {
            case EPEngCommCapIMClient:
                {
                DoSetCapabilityStatusL( iIMCapability, *aIntData );
                handled = ETrue;
                break;
                }
            }
        }

    return handled;
    }


// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVCommCapConstruct::CPEngWVCommCapModel::GetData( TInt* aIntData,
                                                             TPtrC8* /*a8Data*/,
                                                             TPtrC16* /*a16Data*/,
                                                             TInt aField,
                                                             TInt aGroup ) const
    {
    TBool handled = EFalse;

    if ( aIntData && ( aField == EPEngCommCapStatus ) )
        {
        switch ( aGroup ) //from TPEngCommunicationCapabilitiesFieldGroups
            {
            case EPEngCommCapIMClient:
                {
                *aIntData = iIMCapability;
                handled = ETrue;
                break;
                }
            }
        }

    return handled;
    }



// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVCommCapConstruct::CPEngWVCommCapModel::EncodeDataToXmlL(
    MPEngXMLSerializer& aSerializer ) const
    {
    aSerializer.StartTagL( KCommCXMLTag );
        {
        aSerializer.StartTagL( KCommCapXMLTagCap ).RawValueL( KCommCapIMXMLTag ).EndTagL( KCommCapXMLTagCap );

        aSerializer.StartTagL( KCommCapStatusXMLTag );
            {
            switch ( iIMCapability )
                {
                case EPEngCommCapStatusOpen:
                    {
                    aSerializer.RawValueL( KCommCapStatusOpen );
                    break;
                    }
                case EPEngCommCapStatusClosed:
                default:
                    {
                    aSerializer.RawValueL( KCommCapStatusClosed );
                    break;
                    }
                }
            }
        aSerializer.EndTagL( KCommCapStatusXMLTag );

        aSerializer.StartTagL( KCommCapNoteXMLTag ).EndTagL( KCommCapNoteXMLTag );
        }
    aSerializer.EndTagL( KCommCXMLTag );
    }



// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVCommCapConstruct::CPEngWVCommCapModel::DecodeDataFromXmlL(
    MPEngXMLParser& aParser,
    const TDesC8& aValueBlock )
    {
    RArray<TPtrC8> commCaps;
    CleanupClosePushL( commCaps );

    // parse the commcap attribute values
    if ( aParser.DecodeL( aValueBlock, KCommCXMLTag, ETrue ) )
        {
        TInt count( aParser.Count() );
        for ( TInt resCount( 0 ); resCount < count; resCount++ )
            {
            User::LeaveIfError( commCaps.Append( aParser.ResultL( resCount ) ) );
            }

        count = commCaps.Count();
        for ( TInt i( 0 ); i < count; i++ )
            {
            if ( aParser.DecodeL( commCaps[i], KCommCapXMLTagCap, EFalse ) )
                {
                if ( aParser.ResultL().CompareF( KCommCapIMXMLTag ) == 0 )
                    {
                    if ( aParser.DecodeL( commCaps[i], KCommCapStatusXMLTag, EFalse ) )
                        {
                        if ( aParser.ResultL().CompareF( KCommCapStatusOpen ) == 0 )
                            {
                            iIMCapability = EPEngCommCapStatusOpen;
                            }
                        else
                            {
                            iIMCapability = EPEngCommCapStatusClosed;
                            }
                        }
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(); //commCaps
    }



// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVCommCapConstruct::CPEngWVCommCapModel::ResetDataL()
    {
    iIMCapability = EPEngCommCapStatusClosed;
    }


// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVCommCapConstruct::CPEngWVCommCapModel::DataExternalizeSize() const
    {
    return 1; //iIMCapability is stored to 8 bit
    }



// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVCommCapConstruct::CPEngWVCommCapModel::ExternalizeDataL(
    RWriteStream& aStream ) const
    {
    aStream.WriteInt8L( iIMCapability );
    }


// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVCommCapConstruct::CPEngWVCommCapModel::InternalizeDataL(
    RReadStream& aStream )
    {
    iIMCapability = static_cast<TPEngWVCommunicationCapabilityValues> ( aStream.ReadInt8L() );
    }


// -----------------------------------------------------------------------------
// CPEngWVCommCapConstruct::CPEngWVCommCapModel::DoSetCapabilityStatusL()
// -----------------------------------------------------------------------------
//
void CPEngWVCommCapConstruct::CPEngWVCommCapModel::DoSetCapabilityStatusL(
    TPEngWVCommunicationCapabilityValues& aCapabilityHolder,
    TInt aNewStatus )
    {
    //validate the data
    switch ( aNewStatus )
        {
            //from TPEngCommunicationCapabilityStatus
        case EPEngCommCapStatusOpen:
        case EPEngCommCapStatusClosed:
            {
            aCapabilityHolder = static_cast<TPEngWVCommunicationCapabilityValues> ( aNewStatus );
            break;
            }
        default:
            {
            __AssertAttrEnumValueOutOfBoundsL();
            break;
            }
        }
    }



//  End of File


