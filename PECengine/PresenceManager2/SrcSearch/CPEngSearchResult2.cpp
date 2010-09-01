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
* Description:  Search results.
*
*/

// INCLUDE FILES
#include "CPEngSearchResultEntry.h"

#include <CPEngSearchResult2.h>
#include <E32Std.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSearchResult2::CPEngSearchResult2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSearchResult2::CPEngSearchResult2( CPEngSearchResultEntry& aImp )
        : iImp( aImp )
    {
    }


// Destructor
CPEngSearchResult2::~CPEngSearchResult2()
    {
    //Implementation not owned
    }


// -----------------------------------------------------------------------------
// CPEngSearchResult2::CurrentFindingsCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSearchResult2::CurrentFindingsCount() const
    {
    return iImp.iCurrentFindingsCount;
    }


// -----------------------------------------------------------------------------
// CPEngSearchResult2::ContinuationIndex()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSearchResult2::ContinuationIndex() const
    {
    return iImp.iContinuationIndex;
    }


// -----------------------------------------------------------------------------
// CPEngSearchResult2::SearchCompleted()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngSearchResult2::SearchCompleted() const
    {
    return iImp.iSearchCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngSearchResult2::UserResults()
// -----------------------------------------------------------------------------
//
EXPORT_C const MDesCArray& CPEngSearchResult2::UserResults() const
    {
    return iImp.iUserResults;
    }


//  End of File


