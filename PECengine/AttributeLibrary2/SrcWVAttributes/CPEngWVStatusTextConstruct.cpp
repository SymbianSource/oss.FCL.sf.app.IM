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
* Description:  WV Status Text implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVStatusTextConstruct.h"
#include "PresenceAttributeDefValueCRKeys.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <PEngWVPresenceAttributes2.h>

#include <E32Base.h>
#include <S32Strm.h>



//  DATA TYPES
/**
 * Status text CSP name table.
 *
 * @since 3.0
 */
const TWVCspAttributeNameEntry KStatusTextCspNameTable[] =
    {
        {
        EWVCspV11,
        LIT_AS_DESC8_PTR( KStatusTextXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS )
        },
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KStatusTextXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS_CSP12 )
        }
    };


const TInt KStatusTextCspNameTableCount = sizeof( KStatusTextCspNameTable ) / sizeof( TWVCspAttributeNameEntry );




// =============== CPEngWVStatusTextConstruct MEMBER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVStatusTextConstruct::CPEngWVStatusTextConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KStatusTextCspNameTable,
                                         KStatusTextCspNameTableCount )
    {
    }


// Destructor
CPEngWVStatusTextConstruct::~CPEngWVStatusTextConstruct()
    {
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVStatusTextConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVStatusTextModel* model = new ( ELeave ) CPEngWVStatusTextModel(
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }




// ================= CPEngWVStatusTextModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextConstruct::CPEngWVStatusTextModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::CPEngWVStatusTextModel(
    TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::~CPEngWVStatusTextModel()
    {
    delete iStatusText;
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::SetDataL( TInt* /*aIntData*/,
                                                                    const TDesC8* /*a8Data*/,
                                                                    const TDesC16* a16Data,
                                                                    TInt aField,
                                                                    TInt aGroup )
    {
    if ( a16Data &&
         ( aGroup == KPEngDefaultAttrValueGroup ) &&
         ( aField == EPEngStatusText ) )
        {
        //check the data length
        if ( a16Data->Length() > KPEngMaxWVStatusTextLength )
            {
            User::Leave( KErrArgument );
            }


        HBufC* tmp = a16Data->AllocL();
        delete iStatusText;
        iStatusText = tmp;

        return ETrue;
        }


    return EFalse;
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::GetData( TInt* /*aIntData*/,
                                                                   TPtrC8* /*a8Data*/,
                                                                   TPtrC16* a16Data,
                                                                   TInt aField,
                                                                   TInt aGroup ) const
    {
    if ( a16Data &&
         ( aGroup == KPEngDefaultAttrValueGroup ) &&
         ( aField == EPEngStatusText ) )
        {
        a16Data->Set( *iStatusText );
        return ETrue;
        }


    return EFalse;
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::EncodeDataToXmlL(
    MPEngXMLSerializer& aSerializer ) const
    {
    aSerializer.StartTagL( KPresenceValueXMLTag )
    .UnicodeTextL( *iStatusText )
    .EndTagL( KPresenceValueXMLTag );
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::DecodeDataFromXmlL(
    MPEngXMLParser& aParser,
    const TDesC8& aValueBlock )
    {
    if ( aParser.DecodeL( aValueBlock, KPresenceValueXMLTag, EFalse ) )
        {
        HBufC* tmp = aParser.ResultAsUnicodeTextL();

        if ( tmp->Length() > KPEngMaxWVStatusTextLength )
            {
            CleanupStack::PushL( tmp );
            HBufC* tmp2 = tmp->Left( KPEngMaxWVStatusTextLength ).AllocL();
            delete iStatusText;
            iStatusText = tmp2;
            CleanupStack::PopAndDestroy( tmp );
            }
        else
            {
            delete iStatusText;
            iStatusText = tmp;
            }
        }
    else
        {
        HBufC* tmpText = HBufC::NewL( 0 );  //by default status text is empty == 0 length
        delete iStatusText;
        iStatusText = tmpText;
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::ResetDataL()
    {
    HBufC* tmpText = HBufC::NewL( 0 );  //by default status text is empty == 0 length
    delete iStatusText;
    iStatusText = tmpText;
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::DataExternalizeSize() const
    {
    return iStatusText->Size() + 4;  //4 bytes for descriptor overhead
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::ExternalizeDataL(
    RWriteStream& aStream ) const
    {
    aStream << iStatusText->Left( KPEngMaxWVStatusTextLength );
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVStatusTextConstruct::CPEngWVStatusTextModel::InternalizeDataL(
    RReadStream& aStream )
    {
    HBufC* tmpText = HBufC::NewL( aStream, KPEngMaxWVStatusTextLength );
    delete iStatusText;
    iStatusText = tmpText;
    }


//  End of File


