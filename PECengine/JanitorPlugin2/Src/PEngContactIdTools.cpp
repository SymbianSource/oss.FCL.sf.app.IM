/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  WV ID handling tools
*
*/

// INCLUDE FILES

#include <e32std.h>
#include "PEngContactIdTools.h"

// CONSTANTS
_LIT( KPengWVIDPrefix, "wv:" );
const TInt KPengWVIDPrefixLength( 3 );
_LIT( KCharAdd, "@" );

// -----------------------------------------------------------------------------
// FindContactIdInArray()
// Find Contact Id in the aray
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
GLREF_C  TInt FindContactIdInArray(
    const MDesCArray& aArray,
    const TDesC& aId )
    {
    TInt count ( aArray.MdcaCount() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        TInt err ( CompareContactIds( aArray.MdcaPoint( x ), aId ) );
        if ( err == KErrNone )
            {
            return x;
            }
        if ( err > 0 ) // no sense to look further in the array
            {
            return KErrNotFound;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// AddContactIToArrayL()
// Add contact Id to the array of the contact Ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
GLREF_C TInt AddContactIdToArrayL(
    CDesCArray& aArray,
    const TDesC& aId )
    {
    // insert the id in sequence
    TInt count ( aArray.MdcaCount() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        TInt err ( CompareContactIds( aArray.MdcaPoint( x ), aId ) );
        if ( err > 0 )
            {
            aArray.InsertL( x, aId  );
            return x;
            }
        if ( err == 0 )
            {
            return KErrAlreadyExists;
            }
        // go for another loop
        }
    // was not inserted, then add it to the end of the array
    aArray.AppendL( aId );
    return count;
    }

// -----------------------------------------------------------------------------
// CompareContactIs()
// Compare two contact Ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
GLREF_C TInt CompareContactIds(
    const TDesC& aId1,
    const TDesC& aId2 )
    {
    // get name without wv: part on the beginning
    TPtrC id1( aId1 );
    TPtrC id2( aId2 );

    if ( aId1.Left( KPengWVIDPrefixLength ).CompareF( KPengWVIDPrefix ) == KErrNone )
        {
        id1.Set( aId1.Mid( KPengWVIDPrefixLength ) );
        }

    if ( aId2.Left( KPengWVIDPrefixLength ).CompareF( KPengWVIDPrefix ) == KErrNone )
        {
        id2.Set( aId2.Mid( KPengWVIDPrefixLength ) );
        }

    // fix 21.04.2005 OK:
    // check how it goes with the domains, if they are different
    // no match, if one missing and base id is same, then same ids
    TInt err ( 	id1.CompareF( id2 ) );

    if ( err == KErrNone )
        {
        return err;
        }

    // try to compare them without domain
    // if home domain info is missing
    // or wv id domain is same as home one, remove it from the wv id
    TInt offset1( id1.Find( KCharAdd ) );
    TInt offset2( id2.Find( KCharAdd ) );

    // if both are with domain, then they are not same
    if ( ( offset1 != KErrNotFound ) && ( offset2 != KErrNotFound ) )
        {
        return err;
        }
    // remove domains from wv id1
    TInt domainCount( 0 );
    if ( KErrNotFound != offset1 )
        {
        id1.Set( id1.Left( offset1 ) ); // cause we need length
        domainCount++;
        }
    // remove domains from wv id2
    if ( KErrNotFound != offset2 )
        {
        id2.Set( id2.Left( offset2 ) ); // cause we need length
        domainCount++;
        }

    // if none of then or both have domain, no need to compare
    if ( ( !domainCount ) || ( domainCount == 2 ) )
        {
        return err;
        }
    return id1.CompareF( id2 );
    }

//  End of File
