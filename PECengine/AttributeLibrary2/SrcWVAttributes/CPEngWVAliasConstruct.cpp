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
* Description:  WV Alias implementation.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVAliasConstruct.h"
#include "PEngWVAttributeModelDataHelper.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <PEngWVPresenceAttributes2.h>

#include <E32Base.h>
#include <S32Strm.h>



//  DATA TYPES
/**
 * Alias CSP name table.
 *
 * @since 3.0
 */
const TWVCspAttributeNameEntry KAliasCspNameTable[] =
    {
        {
        EWVCspV11,
        LIT_AS_DESC8_PTR( KAliasXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS )
        },
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KAliasXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS_CSP12 )
        }
    };


const TInt KAliasCspNameTableCount = sizeof( KAliasCspNameTable ) / sizeof( TWVCspAttributeNameEntry );




// =============== CPEngWVAliasConstruct MEMBER FUNCTIONS ===============

// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVAliasConstruct::CPEngWVAliasConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KAliasCspNameTable,
                                         KAliasCspNameTableCount )
    {
    }


// Destructor
CPEngWVAliasConstruct::~CPEngWVAliasConstruct()
    {
    }



// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVAliasConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVAliasModel* model = new ( ELeave ) CPEngWVAliasModel(
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }




// ================= CPEngWVAliasModel MEMBER FUNCTIONS =================

// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasConstruct::CPEngWVAliasModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVAliasConstruct::CPEngWVAliasModel::CPEngWVAliasModel(
    TBool aUserOwnAttribute )
        : CPEngWVAttributeModelBase( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVAliasConstruct::CPEngWVAliasModel::~CPEngWVAliasModel()
    {
    delete iAlias;
    }




// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasModel::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVAliasConstruct::CPEngWVAliasModel::SetDataL( TInt* /*aIntData*/,
                                                          const TDesC8* /*a8Data*/,
                                                          const TDesC16* a16Data,
                                                          TInt aField,
                                                          TInt aGroup )
    {
    if ( a16Data &&
         ( aGroup == KPEngDefaultAttrValueGroup ) &&
         ( aField == EPEngAlias ) )
        {
        DoSetDescL( iAlias, *a16Data );
        return ETrue;
        }

    return EFalse;
    }





// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasModel::GetData()
// -----------------------------------------------------------------------------
//
TBool CPEngWVAliasConstruct::CPEngWVAliasModel::GetData( TInt* /*aIntData*/,
                                                         TPtrC8* /*a8Data*/,
                                                         TPtrC16* a16Data,
                                                         TInt aField,
                                                         TInt aGroup ) const
    {
    if ( a16Data &&
         ( aGroup == KPEngDefaultAttrValueGroup ) &&
         ( aField == EPEngAlias ) )
        {
        DoGetDesc( iAlias, *a16Data );

        return ETrue;
        }


    return EFalse;
    }




// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasModel::EncodeDataToXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVAliasConstruct::CPEngWVAliasModel::EncodeDataToXmlL(
    MPEngXMLSerializer& aSerializer ) const
    {
    //iAlias might be NULL -> get a valid TPtrC

    TPtrC16 alias( NULL, 0 );
    DoGetDesc( iAlias, alias );

    aSerializer.StartTagL( KPresenceValueXMLTag )
    .UnicodeTextL( alias )
    .EndTagL( KPresenceValueXMLTag );
    }



// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasModel::DecodeDataFromXmlL()
// -----------------------------------------------------------------------------
//
void CPEngWVAliasConstruct::CPEngWVAliasModel::DecodeDataFromXmlL(
    MPEngXMLParser& aParser,
    const TDesC8& aValueBlock )
    {
    if ( aParser.DecodeL( aValueBlock, KPresenceValueXMLTag, EFalse ) )
        {
        HBufC* tmp = aParser.ResultAsUnicodeTextL();
        CleanupStack::PushL( tmp );
        DoSetDescL( iAlias, *tmp );
        CleanupStack::PopAndDestroy( tmp );
        }
    }



// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasModel::ResetDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVAliasConstruct::CPEngWVAliasModel::ResetDataL()
    {
    delete iAlias;
    iAlias = NULL;
    }


// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasModel::DataExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngWVAliasConstruct::CPEngWVAliasModel::DataExternalizeSize() const
    {
    return DoGetBufExtSize( iAlias );
    }



// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasModel::ExternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVAliasConstruct::CPEngWVAliasModel::ExternalizeDataL(
    RWriteStream& aStream ) const
    {
    DoExtrBufL( iAlias, aStream );
    }


// -----------------------------------------------------------------------------
// CPEngWVAliasConstruct::CPEngWVAliasModel::InternalizeDataL()
// -----------------------------------------------------------------------------
//
void CPEngWVAliasConstruct::CPEngWVAliasModel::InternalizeDataL(
    RReadStream& aStream )
    {
    TPtr16 typeStrait( NULL, 0 );
    DoIntrBufL( iAlias, typeStrait, aStream );
    }

//  End of File


