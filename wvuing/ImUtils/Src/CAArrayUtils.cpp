/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Array utils
 *
*/


// INCLUDE FILES
#include    "CAArrayUtils.h"
#include    "ChatDefinitions.h"
#include    "ChatDebugPrint.h"
#include    "CAUtils.h"

const TInt KSelectionArrayGranularity = 5;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CAArrayUtils::SetSelectionsLC()
// Returns selection array and leaves it on stack
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CArrayFixFlat<TInt>* CAArrayUtils::SetSelectionsLC(
    const CDesCArray& aSource1,
    const CDesCArray& aSource2 )
    {
    CArrayFixFlat<TInt>* returnArray = new ( ELeave )
    CArrayFixFlat<TInt>( KSelectionArrayGranularity );
    CleanupStack::PushL( returnArray );
    TInt source1Count( aSource1.MdcaCount() );
    TInt source2Count( aSource2.MdcaCount() );
    for ( TInt i( 0 ); i < source1Count; ++i )
        {
        for ( TInt j( 0 ); j < source2Count; ++j )
            {
            if ( aSource1.MdcaPoint( i ).CompareC( aSource2.MdcaPoint( j ) ) == 0 )
                {
                returnArray->AppendL( j );
                }
            }
        }
    return returnArray;
    }



// -----------------------------------------------------------------------------
// CAArrayUtils::CloneArrayLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDesCArray* CAArrayUtils::CloneArrayLC( const MDesCArray& aSource )
    {
    TInt count( aSource.MdcaCount() );
    CDesCArray* array = new( ELeave )CDesCArrayFlat( count == 0 ? 1 : count );
    CleanupStack::PushL( array );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC text( aSource.MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( " CloneArrayLC [%d] %S" ), i, &text );
        array->AppendL( text );
        }
    CHAT_DP_TXT( "----------------------[endClone]----------------------" );
    return array;
    }

// -----------------------------------------------------------------------------
// CAArrayUtils::CreateDiffLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDesCArray* CAArrayUtils::CreateDiffLC( const CDesCArray& aArrayDiff,
                                                 const CDesCArray& aArrayTest )
    {
    CDesCArray* array = new( ELeave )CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( array );
    TInt count( aArrayDiff.MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC id( aArrayDiff.MdcaPoint( i ) );
        TInt pos( 0 );
        if ( aArrayTest.Find( id, pos, ECmpCollated ) != 0 )
            {
            // item that is in aArrayDiff, but not in aArrayTest
            // add to resulting array
            CHAT_DP( D_CHAT_LIT( " CreateDiffLC [%d] %S" ), i, &id );
            array->AppendL( id );
            }
        }
    CHAT_DP_TXT( "-----------------------[endDiff]----------------------" );
    return array;
    }

// -----------------------------------------------------------------------------
// CAArrayUtils::AppendArrayL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAArrayUtils::AppendArrayL( const CDesCArray& aSource,
                                          CDesCArray& aDest )
    {
    TInt count( aSource.MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        aDest.AppendL( aSource.MdcaPoint( i ) );
        }
    }
// -----------------------------------------------------------------------------
// CAArrayUtils::AppendIfNotFoundL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TBool CAArrayUtils::AppendIfNotFoundL( CDesCArray& aDest, const TDesC& aPtr )
    {
    TInt pos( KErrNotFound );
    if ( aDest.Find( aPtr, pos, ECmpCollated ) != 0 )
        {
        // not found
        aDest.AppendL( aPtr );
        return ETrue;
        }
    return EFalse;
    }
// -----------------------------------------------------------------------------
// CAArrayUtils::DeleteIfFound
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TBool CAArrayUtils::DeleteIfFound( CDesCArray& aDest, const TDesC& aPtr )
    {
    TInt pos( KErrNotFound );
    if ( aDest.Find( aPtr, pos, ECmpCollated ) == 0 )
        {
        aDest.Delete( pos );
        return EFalse;
        }
    return ETrue;
    }


//  End of File
