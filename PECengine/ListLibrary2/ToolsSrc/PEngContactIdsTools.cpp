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
* Description:  Contact Id handling tools
*
*/

// INCLUDE FILES

#include <e32std.h>
#include "PEngContactIdsTools.h"


// ================ LOCAL FUNCTIONS ============================================

// -----------------------------------------------------------------------------
// AssertDomain()
// Checks domain of the contact id
// If domain is same as passed, it is removed from the id.
//
//
// @param aContactId The contact id from where to check the domain.
// @param aUserDomain The user domain.
// @param aDomain Return paramater for extracted domain.
// @return 1 if the domain was updated. Else 0.
// -----------------------------------------------------------------------------
//
TInt DoAssertDomain( TPtrC& aContactId,
                     const TDesC& aUserDomain,
                     TPtrC& aDomain )
    {
    TChar domainSeparator( '@' );

    TInt offset( aContactId.Locate( domainSeparator ) );
    if ( ( KErrNotFound != offset ) &&
         ( !aUserDomain.Length()    ||
           ( KErrNone == aContactId.Mid( offset ).CompareF( aUserDomain ) ) ) )
        {
        aDomain.Set( aContactId.Mid( offset + 1 ) ); // storing the domain too
        aContactId.Set( aContactId.Left( offset ) ); // cause we need length
        return 1; // as described in description
        }

    return 0; // nothing was updated
    }


// -----------------------------------------------------------------------------
// CleanContactListName()
// Removes slash from the beginning of the contact list id if it is there
//
// @param aCntListId Contact list id to clean.
// -----------------------------------------------------------------------------
//
void DoCleanContactListName( TPtrC& aCntListId )
    {
    // look for the slash as first character->1
    if ( aCntListId.Left( 1 ).Compare( KPEngWVResourceSeparator ) == KErrNone )
        {
        // ignore slash on the beginning, 1 character
        aCntListId.Set( aCntListId.Mid( 1 ) );
        }
    }



// -----------------------------------------------------------------------------
// DoCompareContactIdsCommon()
// Compare two Contact Ids
//
// @param aContactId1 Contact Id 1. Possible "WV:" prefix is stripped away.
// @param aContactId2 Contact Id 2. Possible "WV:" prefix is stripped away.
// @param aUserDomain user domain
// @return KErrNone if they contact IDs are same, other non zero number if not
// -----------------------------------------------------------------------------
//
TInt DoCompareContactIdsCommon( TPtrC& aContactId1,
                                TPtrC& aContactId2 )
    {
    // get name without wv: part on the beginning
    if ( KErrNone
         ==
         aContactId1.Left( KPEngWVIDPrefixLength ).CompareF( KPEngWVIDPrefix ) )
        {
        aContactId1.Set( aContactId1.Mid( KPEngWVIDPrefixLength ) );
        }

    if ( KErrNone
         ==
         aContactId2.Left( KPEngWVIDPrefixLength ).CompareF( KPEngWVIDPrefix ) )
        {
        aContactId2.Set( aContactId2.Mid( KPEngWVIDPrefixLength ) );
        }

    return aContactId1.CompareF( aContactId2 );
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NContactIdsTools::FindContactIdInArray()
// -----------------------------------------------------------------------------
//
TInt NContactIdsTools::FindContactIdInArray( const MDesCArray& aArray,
                                             const TDesC& aContactId,
                                             TInt& aIndex,
                                             const TDesC& aUserDomain )
    {
    TInt l( 0 );
    TInt r( aArray.MdcaCount() );
    TInt ret( KErrNotFound );
    while ( r > l )
        {
        TInt inx = ( l + r ) >> 1;
        TInt k ( CompareContactIds( aContactId,
                                    aArray.MdcaPoint( inx ),
                                    aUserDomain ) );
        if ( k == 0 )
            {
            aIndex = inx;
            return KErrNone;
            }
        else if ( k > 0 )
            l = inx + 1;
        else
            r = inx;
        }
    aIndex = r;
    return ret;
    }


// -----------------------------------------------------------------------------
// NContactIdsTools::FindContactIdInArray()
// -----------------------------------------------------------------------------
//
TInt NContactIdsTools::FindContactIdInArray( const MDesCArray& aArray,
                                             const TDesC& aContactId,
                                             const TDesC& aUserDomain )
    {
    TInt index( 0 );
    if ( KErrNone == NContactIdsTools::FindContactIdInArray( aArray,
                                                             aContactId,
                                                             index,
                                                             aUserDomain ) )
        {
        return index;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// NContactIdsTools::AddContactIdToArrayL()
// -----------------------------------------------------------------------------
//
TInt NContactIdsTools::AddContactIdToArrayL( CDesCArray& aArray,
                                             const TDesC& aContactId,
                                             const TDesC& aUserDomain )
    {
    TInt index( 0 );
    if ( KErrNone == NContactIdsTools::FindContactIdInArray( aArray,
                                                             aContactId,
                                                             index,
                                                             aUserDomain ) )
        {
        return KErrAlreadyExists;
        }

    aArray.InsertL( index , aContactId );
    return index;
    }


// -----------------------------------------------------------------------------
// NContactIdsTools::RemoveContactIdFromArray()
// -----------------------------------------------------------------------------
//
TInt NContactIdsTools::RemoveContactIdFromArray( CDesCArray& aArray,
                                                 const TDesC& aContactId,
                                                 const TDesC& aUserDomain )
    {
    TInt posToDelete( NContactIdsTools::FindContactIdInArray( aArray,
                                                              aContactId,
                                                              aUserDomain ) );
    if ( posToDelete != KErrNotFound )
        {
        aArray.Delete( posToDelete );
        return KErrNone;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// NContactIdsTools::CompareWVIDs()
// -----------------------------------------------------------------------------
//
TInt NContactIdsTools::CompareContactIds( const TDesC& aContactId1,
                                          const TDesC& aContactId2,
                                          const TDesC& aUserDomain )
    {
    // get contact ids without wv: prefix
    TPtrC contactId1( aContactId1 );
    TPtrC contactId2( aContactId2 );

    TInt err = DoCompareContactIdsCommon( contactId1, contactId2 );
    if ( err == KErrNone )
        {
        return KErrNone;
        }

    // try to compare them without domain if domain is missing or is same as user one
    // remove domain from contactId1
    TPtrC domain1;
    TPtrC domain2;
    TInt domainCount = DoAssertDomain( contactId1, aUserDomain, domain1 );
    domainCount += DoAssertDomain( contactId2, aUserDomain, domain2 );

    // if none of then or both have domain, no need to compare
    if ( ( domainCount == 0 ) ||
         ( domainCount == 2 ) )
        {
        // compare domains, if match continue, else return the error
        if ( domain1.CompareF( domain2 ) != KErrNone )
            {
            return err;
            }
        }

    return contactId1.CompareF( contactId2 );
    }


// -----------------------------------------------------------------------------
// NContactListIdsTools::ExtractPureContactListName()
// -----------------------------------------------------------------------------
//
void NContactListIdsTools::ExtractPureContactListName( TPtrC& aContactListId,
                                                       const TDesC& aPlainContactId )
    {
    TChar domainSeparator( '@' );
    TInt offset( aContactListId.Locate( domainSeparator ) );

    // remove domain
    if ( KErrNotFound != offset )
        {
        aContactListId.Set( aContactListId.Left( offset ) );
        }

    // remove wv prefix
    if ( KErrNone
         ==
         aContactListId.Left( KPEngWVIDPrefixLength ).CompareF( KPEngWVIDPrefix ) )
        {
        aContactListId.Set( aContactListId.Mid( KPEngWVIDPrefixLength ) );
        }


    // remove user part
    if ( KErrNone
         ==
         aContactListId.Left( aPlainContactId.Length() ).CompareF( aPlainContactId ) )
        {
        aContactListId.Set( aContactListId.Mid( aPlainContactId.Length() ) );
        }
    }


// -----------------------------------------------------------------------------
// NContactListIdsTools::CompareContactListIds()
// -----------------------------------------------------------------------------
//
TInt NContactListIdsTools::CompareContactListIds( const TDesC& aContactListId1,
                                                  const TDesC& aContactListId2 )
    {
    // extract pure list name, free from domain and possibly user part
    TPtrC id1( aContactListId1 );
    TPtrC id2( aContactListId2 );
    DoCleanContactListName( id1 );
    DoCleanContactListName( id2 );

    // do the compare
    return id1.CompareF( id2 );
    }

//  End of File
