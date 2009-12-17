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
* Description:  Holds error message information
*
*/

// INCLUDE FILES
#include    "CCAErrorData.h"
#include    "ChatDebugPrint.h"

#include    <impsdetailed.h>
#include    <impserrors.h>
#include    <badesca.h>

const TInt KErrorDataArrayGranularity = 3;

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCAErrorData* CCAErrorData::NewL( const CImpsDetailedResult& aDetailedResult )
    {
    CCAErrorData* self = new ( ELeave ) CCAErrorData();

    CleanupStack::PushL( self );
    self->ConstructL( aDetailedResult );
    CleanupStack::Pop( self );

    return self;
    }

// Two-phased constructor.
CCAErrorData* CCAErrorData::NewL(
    TInt                aCode,
    const TDesC&        aDescription,
    const CPtrC16Array* aUserIDs,
    const CPtrC16Array* aGroupIDs,
    const CPtrC16Array* aMessageIDs,
    const CPtrC16Array* aScreenNames,
    const CPtrC16Array* aScreenNameGroup )
    {
    CCAErrorData* self = new ( ELeave ) CCAErrorData();

    CleanupStack::PushL( self );
    self->ConstructL(
        aCode,
        aDescription,
        aUserIDs,
        aGroupIDs,
        aMessageIDs,
        aScreenNames,
        aScreenNameGroup );

    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCAErrorData::~CCAErrorData()
    {
    delete iDescription;

    if ( iUserIDs )
        {
        iUserIDs->Reset();
        }
    delete iUserIDs;

    if ( iGroupIDs )
        {
        iGroupIDs->Reset();
        }
    delete iGroupIDs;

    if ( iMsgIDs )
        {
        iMsgIDs->Reset();
        }
    delete iMsgIDs;

    if ( iScreenNames )
        {
        iScreenNames->Reset();
        }
    delete iScreenNames;

    if ( iScreenNameGroup )
        {
        iScreenNameGroup->Reset();
        }
    delete iScreenNameGroup;
    }

// ---------------------------------------------------------
// CCAErrorData::Code
// ---------------------------------------------------------
//
EXPORT_C TInt CCAErrorData::Code() const
    {
    // In case we get error code in specification format => 531 e.g.
    if ( iCode > 0 )
        {
        // Imps_ERROR_BASE is negative so "+"
        return ( - iCode + Imps_ERROR_BASE );
        }

    return iCode;
    }

// ---------------------------------------------------------
// CCAErrorData::Description
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CCAErrorData::Description() const
    {
    return *iDescription;
    }

// ---------------------------------------------------------
// CCAErrorData::UserIDs
// ---------------------------------------------------------
//
EXPORT_C const CDesCArray& CCAErrorData::UserIDs() const
    {
    return *iUserIDs;
    }

// ---------------------------------------------------------
// CCAErrorData::GroupIDs
// ---------------------------------------------------------
//
EXPORT_C const CDesCArray& CCAErrorData::GroupIDs() const
    {
    return *iGroupIDs;
    }

// ---------------------------------------------------------
// CCAErrorData::MessageIDs
// ---------------------------------------------------------
//
EXPORT_C const CDesCArray& CCAErrorData::MessageIDs() const
    {
    return *iMsgIDs;
    }

// ---------------------------------------------------------
// CCAErrorData::ScreenNames
// ---------------------------------------------------------
//
EXPORT_C const CDesCArray& CCAErrorData::ScreenNames() const
    {
    return *iScreenNames;
    }

// ---------------------------------------------------------
// CCAErrorData::ScreenNameGroup
// ---------------------------------------------------------
//
EXPORT_C const CDesCArray& CCAErrorData::ScreenNameGroup() const
    {
    return *iScreenNameGroup;
    }


// Symbian OS default constructor can leave.
void CCAErrorData::ConstructL( const CImpsDetailedResult& aDetailedResult )
    {
    // Set all the data available
    SetCode( aDetailedResult.Code() );

    iDescription = aDetailedResult.Descriptor().AllocL();

    SetUserIDsL( aDetailedResult.UserIds() );

    SetGroupIDsL( aDetailedResult.GroupIds() );

    SetMessageIDsL( aDetailedResult.MessageIds() );

    SetScreenNamesL( aDetailedResult.SNames() );

    SetScreenNamesGroupsL( aDetailedResult.SNameGroups() );

    }

// Symbian OS default constructor can leave.
void CCAErrorData::ConstructL(
    TInt                aCode,
    const TDesC&        aDescription,
    const CPtrC16Array* aUserIDs,
    const CPtrC16Array* aGroupIDs,
    const CPtrC16Array* aMessageIDs,
    const CPtrC16Array* aScreenNames,
    const CPtrC16Array* aScreenNameGroup )
    {
    // Set all the data available
    SetCode( aCode );

    iDescription = aDescription.AllocL();

    SetUserIDsL( aUserIDs );

    SetGroupIDsL( aGroupIDs );

    SetMessageIDsL( aMessageIDs );

    SetScreenNamesL( aScreenNames );

    SetScreenNamesGroupsL( aScreenNameGroup );
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAErrorData::CCAErrorData()
    {
    }

// ---------------------------------------------------------
// CCAErrorData::SetCode
// ---------------------------------------------------------
//
void CCAErrorData::SetCode( TInt aCode )
    {
    iCode = aCode;
    }

// ---------------------------------------------------------
// CCAErrorData::SetUserIDsL
// ---------------------------------------------------------
//
void CCAErrorData::SetUserIDsL( const CPtrC16Array* aUserIDs )
    {
    iUserIDs = new ( ELeave ) CDesCArrayFlat( KErrorDataArrayGranularity );
    // Get User WV IDs
    if ( aUserIDs )
        {
        TInt count( aUserIDs->MdcaCount() );
        for ( TInt arrayIndex( 0 ); arrayIndex < count; ++arrayIndex )
            {
            iUserIDs->AppendL( aUserIDs->MdcaPoint( arrayIndex ) );
            }
        }
    }


// ---------------------------------------------------------
// CCAErrorData::SetGroupIDsL
// ---------------------------------------------------------
//
void CCAErrorData::SetGroupIDsL( const CPtrC16Array* aGroupIDs )
    {
    iGroupIDs = new ( ELeave ) CDesCArrayFlat( KErrorDataArrayGranularity );
    // Get group WV IDs
    if ( aGroupIDs )
        {
        TInt count( aGroupIDs->MdcaCount() );
        for ( TInt arrayIndex( 0 ); arrayIndex < count; ++arrayIndex )
            {
            iGroupIDs->AppendL( aGroupIDs->MdcaPoint( arrayIndex ) );
            }
        }
    }


// ---------------------------------------------------------
// CCAErrorData::SetMessageIDsL
// ---------------------------------------------------------
//
void CCAErrorData::SetMessageIDsL( const CPtrC16Array* aMessageIDs )
    {
    iMsgIDs = new ( ELeave ) CDesCArrayFlat( KErrorDataArrayGranularity );
    // Get message ids
    if ( aMessageIDs )
        {
        TInt count( aMessageIDs->MdcaCount() );
        for ( TInt arrayIndex( 0 ); arrayIndex < count; ++arrayIndex )
            {
            iMsgIDs->AppendL( aMessageIDs->MdcaPoint( arrayIndex ) );
            }
        }
    }

// ---------------------------------------------------------
// CCAErrorData::SetScreenNamesL
// ---------------------------------------------------------
//
void CCAErrorData::SetScreenNamesL( const CPtrC16Array* aScreenNames )
    {
    iScreenNames = new ( ELeave ) CDesCArrayFlat( KErrorDataArrayGranularity );
    // Get screen names
    if ( aScreenNames )
        {
        TInt count( aScreenNames->MdcaCount() );
        for ( TInt arrayIndex( 0 ); arrayIndex < count; ++arrayIndex )
            {
            iScreenNames->AppendL( aScreenNames->MdcaPoint( arrayIndex ) );
            }
        }
    }


// ---------------------------------------------------------
// CCAErrorData::SetScreenNamesGroupsL
// ---------------------------------------------------------
//
void CCAErrorData::SetScreenNamesGroupsL(
    const CPtrC16Array* aScreenNameGroups )
    {
    iScreenNameGroup =
        new ( ELeave ) CDesCArrayFlat( KErrorDataArrayGranularity );
    // Get groups of screen names
    if ( aScreenNameGroups )
        {
        TInt count( aScreenNameGroups->MdcaCount() );
        for ( TInt arrayIndex( 0 ); arrayIndex < count; ++arrayIndex )
            {
            iScreenNameGroup->AppendL(
                aScreenNameGroups->MdcaPoint( arrayIndex ) );
            }
        }
    }
//  End of File
