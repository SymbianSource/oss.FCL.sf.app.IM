/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  holds the partial success information
*
*/

// INCLUDE FILES
#include    "CCAPartialSuccessData.h"
#include    "CCAErrorData.h"

#include    "ChatDebugPrint.h"

#include    <impsdetailed.h>

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCAPartialSuccessData* CCAPartialSuccessData::NewL()
    {
    CCAPartialSuccessData* self = new ( ELeave ) CCAPartialSuccessData();

    // no need for ConstructL

    return self;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAPartialSuccessData::CCAPartialSuccessData()
    {
    }

// Destructor
CCAPartialSuccessData::~CCAPartialSuccessData()
    {
    iErrorData.ResetAndDestroy();
    iErrorData.Close();
    }

// ---------------------------------------------------------
// CCAPartialSuccessData::SetPartialSuccessDataL
// ---------------------------------------------------------
//
void CCAPartialSuccessData::SetPartialSuccessDataL(
    const CImpsDetailed& aDetailedResults )
    {
    iErrorData.ResetAndDestroy();

    TInt count( aDetailedResults.Count() );

    for ( TInt arrayIndex( 0 ); arrayIndex < count; ++arrayIndex )
        {
        CCAErrorData* errorData = CCAErrorData::NewL(
                                      *( aDetailedResults.At( arrayIndex ) ) );

        // NOTE: iErrorData takes ownership
        TInt err( iErrorData.Append( errorData ) );
        if ( err != KErrNone )
            {
            delete errorData;
            User::Leave( err );
            }
        }
    }

// ---------------------------------------------------------
// CCAPartialSuccessData::PartialSuccessData
// ---------------------------------------------------------
//
const RPointerArray<MCAError>* CCAPartialSuccessData::PartialSuccessData()
const
    {
    return &iErrorData;
    }

//  End of File
