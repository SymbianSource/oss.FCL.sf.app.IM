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
* Description:  Search results entry.
*
*/

// INCLUDE FILES
#include "CPEngSearchResultEntry.h"
#include <CPEngSearchResult2.h>
#include <E32Std.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSearchResultEntry::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSearchResultEntry* CPEngSearchResultEntry::NewL()
    {
    CPEngSearchResultEntry* self = CPEngSearchResultEntry::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CPEngSearchResultEntry* CPEngSearchResultEntry::NewLC()
    {
    CPEngSearchResultEntry* self = new ( ELeave ) CPEngSearchResultEntry;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }




// Destructor
CPEngSearchResultEntry::~CPEngSearchResultEntry()
    {
    delete iInterface;
    }


// -----------------------------------------------------------------------------
// CPEngSearchResultEntry::CPEngSearchResultEntry
// C++ default constructor can NOT contain any code, that
// might leave.
//
// UserResults amount can differ a lot, 5 should be quite optimal
// -----------------------------------------------------------------------------
//
CPEngSearchResultEntry::CPEngSearchResultEntry()
        : iUserResults( 5 )
    {
    }


// -----------------------------------------------------------------------------
// CPEngSearchResultEntry::ConstructL()
// Symbian OS constructor.
// -----------------------------------------------------------------------------
//
void CPEngSearchResultEntry::ConstructL()
    {
    iInterface = new ( ELeave ) CPEngSearchResult2( *this );
    }


// -----------------------------------------------------------------------------
// CPEngSearchResultEntry::Interface()
// -----------------------------------------------------------------------------
//
CPEngSearchResult2& CPEngSearchResultEntry::Interface()
    {
    return *iInterface;
    }


// -----------------------------------------------------------------------------
// CPEngSearchResultEntry::UserResults()
// -----------------------------------------------------------------------------
//
CDesC16Array& CPEngSearchResultEntry::UserResults()
    {
    return iUserResults;
    }


// -----------------------------------------------------------------------------
// CPEngSearchResultEntry::SetDetails()
// -----------------------------------------------------------------------------
//
void CPEngSearchResultEntry::SetDetails( TInt aFindingsCount,
                                         TInt aContinuationIndex,
                                         TBool aSearchCompleted )
    {
    iCurrentFindingsCount = aFindingsCount;
    iContinuationIndex = aContinuationIndex;
    iSearchCompleted = aSearchCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngSearchResultEntry::Reset()
// -----------------------------------------------------------------------------
//
void CPEngSearchResultEntry::Reset()
    {
    iCurrentFindingsCount = 0;
    iContinuationIndex = 0;
    iSearchCompleted = ETrue;
    iUserResults.Reset();
    }



//  End of File

