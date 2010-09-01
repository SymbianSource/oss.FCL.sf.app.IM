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
* Description:  Represents the searched data
*
*/


// INCLUDE FILES
#include    "CCASearchData.h"
#include    "PrivateEngineDefinitions.h"

#include    "ChatDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCASearchData* CCASearchData::NewL()
    {
    CCASearchData* self = new ( ELeave ) CCASearchData;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCASearchData::~CCASearchData()
    {
    delete iResults;
    }

// ---------------------------------------------------------
// CCASearchData::NoMoreResultsAvailable
// ---------------------------------------------------------
//
EXPORT_C TBool CCASearchData::NoMoreResultsAvailable() const
    {
    return iCompleted;
    }

// ---------------------------------------------------------
// CCASearchData::SearchResultType
// ---------------------------------------------------------
//
EXPORT_C TImpsSearchResultType CCASearchData::SearchResultType() const
    {
    return iSearchResultType;
    }

// ---------------------------------------------------------
// CCASearchData::SearchData
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCASearchData::SearchData( TInt aIndex ) const
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchData::SearchData with index %d" ), aIndex );
    CHAT_DP( D_CHAT_LIT( "SearchDataCount is %d" ), iResults->MdcaCount() );

    return iResults->MdcaPoint( aIndex );
    }

// ---------------------------------------------------------
// CCASearchData::SearchDataCount
// ---------------------------------------------------------
//
EXPORT_C TInt CCASearchData::SearchDataCount() const
    {
    CHAT_DP( D_CHAT_LIT( " CCASearchData::SearchDataCount is %d " ),
             iResults->MdcaCount() );

    return iResults->MdcaCount();
    }

// ---------------------------------------------------------
// CCASearchData::HowManyResultsAvailable
// ---------------------------------------------------------
//
EXPORT_C TInt CCASearchData::HowManyResultsAvailable() const
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchData::HowManyResultsAvailable is %d" ),
             iMaximumResultsAvailable );

    return iMaximumResultsAvailable;
    }

// ---------------------------------------------------------
// CCASearchData::NextSearchIndex
// ---------------------------------------------------------
//
TInt CCASearchData::NextSearchIndex() const
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchData::NextSearchIndex is %d" ),
             iServerNextIndex );

    return iServerNextIndex;
    }

// ---------------------------------------------------------
// CCASearchData::UpdateDataL
// ---------------------------------------------------------
//
void CCASearchData::UpdateDataL( TInt                    aIndex,
                                 TBool                   aCompleted,
                                 TImpsSearchResultType   aType,
                                 MDesCArray*             aResults,
                                 TInt                    /*aSearchLimit*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchData::UpdateDataL()" ) );

    iServerNextIndex = aIndex;

    iCompleted = aCompleted;

    iSearchResultType = aType;

    if ( aResults )
        {
        TInt count( aResults->MdcaCount() );

        CHAT_DP( D_CHAT_LIT( "New results     %d" ), aResults->MdcaCount() );

        for ( TInt arrayIndex( 0 ); arrayIndex < count; ++arrayIndex )
            {
            TPtrC searchData( aResults->MdcaPoint( arrayIndex ) );
            // We don't care about index
            TInt ignore;
            // Don't add duplicates
            if ( iResults->Find( searchData, ignore ) != 0 )
                {
                iResults->AppendL( searchData );
                }
            }

        // Check iServerNextIndex against really received results
        // If server for some reason gives wrong index it
        // messes up our logic and causes crashes
        TInt resultCount = iResults->MdcaCount();
        if ( iServerNextIndex > resultCount && !iCompleted )
            {
            iServerNextIndex = resultCount;
            }

        CHAT_DP( D_CHAT_LIT( "We have results %d" ), resultCount );
        }
    else // Search was succesfull, but there were no results so clear the array
        {
        iResults->Reset();
        }
    }

// ---------------------------------------------------------
// CCASearchData::ClearResults
// ---------------------------------------------------------
//
void CCASearchData::ClearResults()
    {
    CHAT_DP_TXT( "CCASearchData::ClearResults()...starts" );
    iServerNextIndex = 0;

    iMaximumResultsAvailable = 0;

    iCompleted = EFalse;

    iResults->Reset();

    CHAT_DP_TXT( "CCASearchData::ClearResults()...over" );
    }

// ---------------------------------------------------------
// CCASearchData::WeHaveData
// ---------------------------------------------------------
//
TBool CCASearchData::WeHaveData( TInt aNextIndex  ) const
    {
    TBool haveData( aNextIndex < iServerNextIndex );
    CHAT_DP( D_CHAT_LIT( "CCASearchData::WeHaveData()...Havedata: %d" ),
             haveData );
    return haveData;
    }

// ---------------------------------------------------------
// CCASearchData::UpdateMaximumPossibleResults
// ---------------------------------------------------------
//
void CCASearchData::UpdateMaximumPossibleResults( TInt aMaxResults )
    {
    iMaximumResultsAvailable = aMaxResults;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCASearchData::CCASearchData()
        : iCompleted( EFalse ),
        iServerNextIndex( 0 ),
        iMaximumResultsAvailable( 0 )
    {
    }

// Symbian OS default constructor can leave.
void CCASearchData::ConstructL()
    {
    CHAT_DP_TXT( "CCASearchData::ConstructL()...starts" );

    iResults = new ( ELeave )CDesCArrayFlat( KSearchDataGranularity );

    CHAT_DP_TXT( "   ConstructL()...ends" );
    }

//  End of File
