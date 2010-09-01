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
* Description:  Tracked Presence ID collection.
*
*/

// INCLUDE FILES
#include <CPEngTrackedPresenceIDs2.h>
#include <CPEngTrackedPresenceID2.h>
#include <MPEngPresenceAttrModel2.h>
#include <E32Std.h>

#include "CPEngTrackedPresenceIDCollection.h"
#include "CPEngTrackedPresenceIDEntry.h"
#include "CPEngTrackedPresenceIDEntryLookUp.h"
#include "TPEngTrackedPresenceEntryFilter.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDCollection* CPEngTrackedPresenceIDCollection::NewL()
    {
    CPEngTrackedPresenceIDCollection* self = new ( ELeave ) CPEngTrackedPresenceIDCollection;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CPEngTrackedPresenceIDCollection::~CPEngTrackedPresenceIDCollection()
    {
    delete iInterface;
    delete iPresenceIdLookup;
    delete iEntryListFilter;
    // Ondra, remove all tracked Ids
    RemoveAllTrackedIds();
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::CPEngTrackedPresenceIDCollection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDCollection::CPEngTrackedPresenceIDCollection()
    {
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::ConstructL()
// Symbian OS constructor.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::ConstructL()
    {
    iInterface = new ( ELeave ) CPEngTrackedPresenceIDs2( *this );
    iPresenceIdLookup = CPEngTrackedPresenceIDEntryLookUp::NewL();
    iEntryListFilter = new ( ELeave ) TPEngTrackedPresenceEntryFilter( iTrackedIDs,
                                                                       EIncludedToList );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::Interface()
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDs2& CPEngTrackedPresenceIDCollection::Interface()
    {
    return *iInterface;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::TrackedPresenceIDsCount()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
TInt CPEngTrackedPresenceIDCollection::TrackedPresenceIDsCount() const
    {
    return iTrackedIDs.Count();
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::FindTrackedPresenceID()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
const CPEngTrackedPresenceIDEntry* CPEngTrackedPresenceIDCollection::FindTrackedPresenceID(
    const TDesC& aPresenceID ) const
    {
    TLinearOrder< CPEngTrackedPresenceIDEntry > order ( CPEngTrackedPresenceIDEntry::Compare );
    iPresenceIdLookup->SetLookupKey( aPresenceID );
    TInt index = iTrackedIDs.FindInOrder( iPresenceIdLookup, order );
    iPresenceIdLookup->Reset();

    const CPEngTrackedPresenceIDEntry* presenceID = NULL;
    if ( index != KErrNotFound )
        {
        presenceID = iTrackedIDs[ index ];
        }

    return presenceID;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::TrackedPresenceID()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
const CPEngTrackedPresenceIDEntry& CPEngTrackedPresenceIDCollection::TrackedPresenceID(
    TInt aIndex ) const
    {
    return *iTrackedIDs[ aIndex ];
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::NextTrackedPresenceID()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceID2* CPEngTrackedPresenceIDCollection::NextTrackedPresenceID(
    TInt& aIterIx )
    {
    CPEngTrackedPresenceID2* idInterface = NULL;

    if ( iTrackedIDs.Count() > aIterIx )
        {
        idInterface = &iTrackedIDs[ aIterIx ]->Interface();
        aIterIx++;
        }

    return idInterface;
    }



// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::AddTrackedIdsL()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::AddTrackedIdsL( const MDesCArray& aPresenceIDs,
                                                       const TArray<TUint32>& aTypes )
    {
    __ASSERT_ALWAYS( aTypes.Count() > 0, User::Leave( KErrArgument ) );


    const TInt idCount = aPresenceIDs.MdcaCount();
    for ( TInt idIx = 0; idIx < idCount; idIx++ )
        {
        TPtrC presenceID( aPresenceIDs.MdcaPoint( idIx ) );
        TLinearOrder< CPEngTrackedPresenceIDEntry > order
        ( CPEngTrackedPresenceIDEntry::Compare );
        TInt entryIx = KErrNotFound;

        iPresenceIdLookup->SetLookupKey( presenceID );
        TInt entryExists = iTrackedIDs.FindInOrder( iPresenceIdLookup, entryIx, order );
        iPresenceIdLookup->Reset();

        if ( entryExists == KErrNotFound )
            {
            CPEngTrackedPresenceIDEntry* entry = CPEngTrackedPresenceIDEntry::NewLC(
                                                     presenceID,
                                                     aTypes,
                                                     ENotIncludedToList );
            User::LeaveIfError( iTrackedIDs.Insert( entry, entryIx ) );
            CleanupStack::Pop( entry );
            }
        else
            {
            CPEngTrackedPresenceIDEntry* entry = iTrackedIDs[ entryIx ];
            entry->AddTrackedAttributesL( aTypes );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::RemoveTrackedIds()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::RemoveTrackedIds( const MDesCArray& aPresenceIDs )
    {
    const TInt idCount = aPresenceIDs.MdcaCount();
    for ( TInt idIx = 0; idIx < idCount; idIx++ )
        {
        TPtrC presenceID( aPresenceIDs.MdcaPoint( idIx ) );
        TLinearOrder< CPEngTrackedPresenceIDEntry > order
        ( CPEngTrackedPresenceIDEntry::Compare );
        TInt entryIx = KErrNotFound;

        iPresenceIdLookup->SetLookupKey( presenceID );
        TInt entryExists = iTrackedIDs.FindInOrder( iPresenceIdLookup, entryIx, order );
        iPresenceIdLookup->Reset();

        if ( entryExists == KErrNone )
            {
            delete iTrackedIDs[ entryIx ];
            iTrackedIDs.Remove( entryIx );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::RemoveTrackedIds()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::RemoveTrackedIds( const TArray<TUint32>& aTypes )
    {
    //go from end to start ==> remove is safe
    for ( TInt entryIx = ( iTrackedIDs.Count() - 1 ) ; entryIx >= 0; entryIx-- )
        {
        CPEngTrackedPresenceIDEntry* entry = iTrackedIDs[ entryIx ];
        entry->RemoveTrackedAttributes( aTypes );
        if ( !entry->AttributesTracked() )
            {
            delete entry;
            iTrackedIDs.Remove( entryIx );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::RemoveTrackedIds()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::RemoveTrackedIds( const MDesCArray& aPresenceIDs,
                                                         const TArray<TUint32>& aTypes )
    {
    const TInt idCount = aPresenceIDs.MdcaCount();
    for ( TInt idIx = 0; idIx < idCount; idIx++ )
        {
        TPtrC presenceID( aPresenceIDs.MdcaPoint( idIx ) );
        TLinearOrder< CPEngTrackedPresenceIDEntry > order
        ( CPEngTrackedPresenceIDEntry::Compare );
        TInt entryIx = KErrNotFound;

        iPresenceIdLookup->SetLookupKey( presenceID );
        TInt entryExists = iTrackedIDs.FindInOrder( iPresenceIdLookup, entryIx, order );
        iPresenceIdLookup->Reset();

        if ( entryExists == KErrNone )
            {
            CPEngTrackedPresenceIDEntry* entry = iTrackedIDs[ entryIx ];
            entry->RemoveTrackedAttributes( aTypes );
            if ( !entry->AttributesTracked() )
                {
                delete entry;
                iTrackedIDs.Remove( entryIx );
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::RemoveTrackedIds()
// Tracked presence IDs.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::RemoveAllTrackedIds()
    {
    iTrackedIDs.ResetAndDestroy();
    }



// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::ResetPresenceChangeMarks()
// List of changed presences
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::ResetPresenceChangeMarks()
    {
    const TInt entryCount = iTrackedIDs.Count();
    for ( TInt entryIx = 0; entryIx < entryCount; entryIx++ )
        {
        CPEngTrackedPresenceIDEntry* entry = iTrackedIDs[ entryIx ];
        entry->ResetChangeMarks();
        }
    }



// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::MarkPresenceChange()
// List of changed presences
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::MarkPresenceChange( const TDesC& aPresenceID )
    {
    TLinearOrder< CPEngTrackedPresenceIDEntry > order
    ( CPEngTrackedPresenceIDEntry::Compare );
    TInt entryIx = KErrNotFound;

    iPresenceIdLookup->SetLookupKey( aPresenceID );
    TInt entryExists = iTrackedIDs.FindInOrder( iPresenceIdLookup, entryIx, order );
    iPresenceIdLookup->Reset();

    if ( entryExists == KErrNone )
        {
        iTrackedIDs[ entryIx ]->MarkAllAttributesChanged();
        }
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::NextChangeMarkedPresenceID()
// List of changed presences
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceID2* CPEngTrackedPresenceIDCollection::NextChangeMarkedPresenceID(
    TInt& aIterIx )

    {
    CPEngTrackedPresenceID2* idInterface = NULL;

    while ( aIterIx < iTrackedIDs.Count()  )
        {
        CPEngTrackedPresenceIDEntry* trackedId = iTrackedIDs[ aIterIx ];

        if ( trackedId->AttributesChanged() )
            {
            idInterface = &trackedId->Interface();
            aIterIx++;
            break;
            }

        aIterIx++;
        }

    return idInterface;
    }




// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::ResetFilteredList()
// Filtered list.
// -----------------------------------------------------------------------------
//

void CPEngTrackedPresenceIDCollection::ResetFilteredList()
    {
    const TInt entryCount = iTrackedIDs.Count();
    for ( TInt entryIx = 0; entryIx < entryCount; entryIx++ )
        {
        CPEngTrackedPresenceIDEntry* entry = iTrackedIDs[ entryIx ];
        entry->SetFilterFlag( ENotIncludedToList );
        }
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::IncludeToFilteredList()
// Filtered list.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::IncludeToFilteredList( TInt aIndex )
    {
    iTrackedIDs[ aIndex ]->SetFilterFlag( EIncludedToList );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::FilteredList()
// Filtered list.
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngTrackedPresenceIDCollection::FilteredList() const
    {
    return *iEntryListFilter;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDCollection::ResetTrackedEntriesIterators()
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDCollection::ResetTrackedEntriesIterators()
    {
    const TInt trackedIDsCount = iTrackedIDs.Count();
    for ( TInt ii = 0; ii < trackedIDsCount; ii++ )
        {
        CPEngTrackedPresenceID2& idInterface = iTrackedIDs[ ii ]->Interface();
        idInterface.ResetTrackedIterator();
        idInterface.ResetChangedIterator();
        }
    }




//  End of File



