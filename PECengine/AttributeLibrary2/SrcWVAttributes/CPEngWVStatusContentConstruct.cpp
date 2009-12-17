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
* Description:  WV Status content construct.
*
*/

// INCLUDE FILES
#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVAttributeModelBase.h"
#include "CPEngWVStatusContentConstruct.h"
#include "CPEngWVStatusContentModel.h"


#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include <MPEngPresenceAttrModel2.h>
#include <E32Base.h>
#include <S32Strm.h>




//  DATA TYPES
/**
 * Status content CSP name table.
 *
 * @since 3.0
 */
const TWVCspAttributeNameEntry KStatusContentCspNameTable[] =
    {
        {
        EWVCspV11,
        LIT_AS_DESC8_PTR( KStatusContentXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS )
        },
        {
        EWVCspV12,
        LIT_AS_DESC8_PTR( KStatusContentXMLTag ),
        LIT_AS_DESC8_PTR( KPresenceSubListAttributesNS_CSP12 )
        }
    };


const TInt KStatusContentCspNameTableCount = sizeof( KStatusContentCspNameTable ) / sizeof( TWVCspAttributeNameEntry );



// =============== CPEngWVStatusContentConstruct MEMBER FUNCTIONS ===============


// -----------------------------------------------------------------------------
// CPEngWVStatusContentConstruct::CPEngWVStatusContentConstruct
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVStatusContentConstruct::CPEngWVStatusContentConstruct( TPEngWVCspVersion aCurrentCspVer )
        : CPEngWVAttributeConstructBase( aCurrentCspVer,
                                         KStatusContentCspNameTable,
                                         KStatusContentCspNameTableCount )
    {
    }


// Destructor
CPEngWVStatusContentConstruct::~CPEngWVStatusContentConstruct()
    {
    }



// -----------------------------------------------------------------------------
// CPEngWVStatusContentConstruct::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModelTypeImp* CPEngWVStatusContentConstruct::NewAttributeInstanceLC(
    TBool aUserOwnAttribute ) const
    {
    CPEngWVStatusContentModel* model = new ( ELeave ) CPEngWVStatusContentModel(
        aUserOwnAttribute );
    CleanupStack::PushL( model );
    model->ResetDataL();
    return model;
    }


//  End of File


