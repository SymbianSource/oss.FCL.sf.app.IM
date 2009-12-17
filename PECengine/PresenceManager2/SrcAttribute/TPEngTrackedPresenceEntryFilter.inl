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
* Description:  Tracked presence entry filter.
*
*/


// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
inline TPEngTrackedPresenceEntryFilter::TPEngTrackedPresenceEntryFilter(
                    RPointerArray< CPEngTrackedPresenceIDEntry >& aIDsToFilter,
                    TInt aIncludeFlag )
    : iIDsToFilter( aIDsToFilter ),
      iIncludeFlag( aIncludeFlag )
    {
    }

// -----------------------------------------------------------------------------
// TPEngTrackedPresenceEntryFilter::MdcaCount()
// -----------------------------------------------------------------------------
//
inline TInt TPEngTrackedPresenceEntryFilter::MdcaCount() const
    {
    TInt filteredCount = 0;
    const TInt entryCount( iIDsToFilter.Count() );
    for( TInt entryIx = 0; entryIx < entryCount; entryIx++ )
        {
        if( iIDsToFilter[ entryIx ]->FilterFlag() == iIncludeFlag )
            {
            filteredCount++;
            }
        }

    return filteredCount;
    }


// -----------------------------------------------------------------------------
// TPEngTrackedPresenceEntryFilter::MdcaPoint()
// -----------------------------------------------------------------------------
//
inline TPtrC TPEngTrackedPresenceEntryFilter::MdcaPoint( TInt aIndex ) const
    {
    TInt filteredCount = 0;
    const TInt entryCount( iIDsToFilter.Count() );
    for( TInt entryIx = 0; entryIx < entryCount; entryIx++ )
        {
        const CPEngTrackedPresenceIDEntry* entry = iIDsToFilter[ entryIx ];
        if( entry->FilterFlag() == iIncludeFlag )
            {
            if( filteredCount == aIndex )
                {
                return entry->PresenceID();
                }

            filteredCount++;
            }
        }

    return KNullDesC();
    }



//  End of File

