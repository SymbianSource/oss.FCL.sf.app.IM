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
* Description:  Tools for matching & handling presence addresses & user id's.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include "PEngAddressUtils.h"
#include "PresenceDebugPrint.h"


// CONSTANTS
/**
 * WV schema prefix & length.
 */
_LIT( KWVSchemaPrefix, "wv:" );
const TInt KWVSchemaPrefixLength = 3;


/**
 * Resource separator "/" & length.
 */
_LIT( KWVResourceSeparator, "/" );
const TInt KWVResourceSeparatorLength = 1;


/**
 * Domain separator "@" & length.
 */
_LIT( KWVDomainSeparator, "@" );
const TInt KWVDomainSeparatorLength = 1;




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngAddressUtils::WVUserId()
// -----------------------------------------------------------------------------
//
TPtrC PEngAddressUtils::WVUserId( const TDesC& aWVAddress )
    {
    TPtrC userId( aWVAddress );
    if ( aWVAddress.Left( KWVSchemaPrefixLength ).CompareF( KWVSchemaPrefix ) == 0 )
        {
        userId.Set( aWVAddress.Mid( KWVSchemaPrefixLength ) );
        }


    TInt resourceStartPos = userId.Find( KWVResourceSeparator );
    if ( resourceStartPos != KErrNotFound )
        {
        userId.Set( userId.Left( resourceStartPos ) );
        }

    else
        {
        TInt domainStartPos = userId.Find( KWVDomainSeparator );
        if ( domainStartPos != KErrNotFound )
            {
            userId.Set( userId.Left( domainStartPos ) );
            }
        }


    return userId;
    }


// -----------------------------------------------------------------------------
// PEngAddressUtils::WVResourceId()
// -----------------------------------------------------------------------------
//
TPtrC PEngAddressUtils::WVResourceId( const TDesC& aWVAddress )
    {
    TPtrC resourceId( NULL, 0 );

    TInt resourceStartPos = aWVAddress.Find( KWVResourceSeparator );
    if ( resourceStartPos != KErrNotFound )
        {
        resourceId.Set( aWVAddress.Mid( resourceStartPos + KWVResourceSeparatorLength ) );


        TInt domainStartPos = resourceId.Find( KWVDomainSeparator );
        if ( domainStartPos != KErrNotFound )
            {
            resourceId.Set( resourceId.Left( domainStartPos ) );
            }
        }


    return resourceId;
    }


// -----------------------------------------------------------------------------
// PEngAddressUtils::WVDomain()
// -----------------------------------------------------------------------------
//
TPtrC PEngAddressUtils::WVDomain( const TDesC& aWVAddress )
    {
    TInt domainStartPos = aWVAddress.Find( KWVDomainSeparator );
    if ( domainStartPos == KErrNotFound )
        {
        return KNullDesC();
        }

    else
        {
        return aWVAddress.Mid( domainStartPos + KWVDomainSeparatorLength );
        }
    }



// -----------------------------------------------------------------------------
// PEngAddressUtils::ReducedWVAddress()
// -----------------------------------------------------------------------------
//

TPtrC PEngAddressUtils::ReducedWVAddress( const TDesC& aWVAddress )
    {
    TPtrC prefix( aWVAddress.Left( KWVSchemaPrefixLength ) );
    if ( prefix.CompareF( KWVSchemaPrefix ) == 0 )
        {
        //has prefix
        return aWVAddress.Mid( KWVSchemaPrefixLength );
        }
    else
        {
        return aWVAddress;
        }
    }


// -----------------------------------------------------------------------------
// PEngAddressUtils::CanonicWVAddressLC()
// -----------------------------------------------------------------------------
//
HBufC* PEngAddressUtils::CanonicWVAddressLC( const TDesC& aUserId,
                                             const TDesC& aResourceId,
                                             const TDesC& aDomain )
    {
    const TInt staticAddressLength = KWVSchemaPrefixLength +
                                     KWVResourceSeparatorLength +
                                     KWVDomainSeparatorLength;

    const TInt dynIdPartLength = aUserId.Length() +
                                 aResourceId.Length();

    __ASSERT_ALWAYS( dynIdPartLength > 0, User::Leave( KErrArgument ) );



    HBufC* addressBuf = HBufC::NewLC( staticAddressLength + dynIdPartLength + aDomain.Length() );
    TPtr address = addressBuf->Des();

    address.Append( KWVSchemaPrefix );
    address.Append( aUserId );      //empty user id can be always added into start

    if ( aResourceId.Length() > 0 )
        {
        address.Append( KWVResourceSeparator );
        address.Append( aResourceId );
        }

    if ( aDomain.Length() > 0 )
        {
        address.Append( KWVDomainSeparator );
        address.Append( aDomain );
        }

    return addressBuf;
    }


// -----------------------------------------------------------------------------
// PEngAddressUtils::MatchWVAddressesF()
// -----------------------------------------------------------------------------
//
TBool PEngAddressUtils::MatchWVAddressesF( const TDesC& aWVAddress1,
                                           const TDesC& aWVAddress2,
                                           const TDesC& aMatchDomain )
    {
    //User ID's must match exactly
    TPtrC pureUserId1 = WVUserId( aWVAddress1 );
    TPtrC pureUserId2 = WVUserId( aWVAddress2 );
    if ( pureUserId1.CompareF( pureUserId2 ) != 0 )
        {
        return EFalse;
        }


    //Resource ID's must match exactly
    TPtrC pureResourceId1 = WVResourceId( aWVAddress1 );
    TPtrC pureResourceId2 = WVResourceId( aWVAddress2 );
    if ( pureResourceId1.CompareF( pureResourceId2 ) != 0 )
        {
        return EFalse;
        }


    //Domains _may_ match exactly
    TPtrC pureDomain1 = WVDomain( aWVAddress1 );
    TPtrC pureDomain2 = WVDomain( aWVAddress2 );
    if ( pureDomain1.CompareF( pureDomain2 ) == 0 )
        {
        return ETrue;
        }


    //Or they might match after domain extended / reduced match
    if ( aMatchDomain.Length() > 0 )
        {
            {
            //If first id is local address (from aMatchDomain)
            //And if second id is from aMatchDomain ==> match.
            TBool id1LocalAddress = ( pureDomain1.Length() == 0 );
            TBool id2FromMatchDomain = ( pureDomain2.CompareF( aMatchDomain ) == 0 );
            if ( id1LocalAddress && id2FromMatchDomain )
                {
                return ETrue;
                }
            }

            {
            //If second id is local address (from aMatchDomain)
            //And if first id is from aMatchDomain ==> match.
            TBool id2LocalAddress = ( pureDomain2.Length() == 0 );
            TBool id1FromMatchDomain = ( pureDomain1.CompareF( aMatchDomain ) == 0 );
            if ( id2LocalAddress && id1FromMatchDomain )
                {
                return ETrue;
                }
            }
        }

    //no match so far
    return EFalse;
    }


// -----------------------------------------------------------------------------
// PEngAddressUtils::FindWVAddressF()
// -----------------------------------------------------------------------------
//
TInt PEngAddressUtils::FindWVAddressF( const TDesC& aWVAddress,
                                       const MDesCArray& aWVAddresses,
                                       const TDesC& aMatchDomain,
                                       TPEngAddressMatchdMode aMode )
    {
    PENG_DP( D_PENG_LIT( "PEngAddressUtils::FindWVAddressF( [%S] from [%d] in domain [%S]" ),
             &aWVAddress, aWVAddresses.MdcaCount(), &aMatchDomain );


    //Try first direct match
    const TInt arrayCount( aWVAddresses.MdcaCount() );
    for ( TInt index( 0 ); index < arrayCount; index++ )
        {
        const TPtrC candidate( aWVAddresses.MdcaPoint( index ) );
        if ( MatchWVAddressesF( aWVAddress, candidate, aMatchDomain ) )
            {
#ifdef PENG_ENABLE_DEBUG_PRINT
            TPtrC matchedAddress = aWVAddresses.MdcaPoint( index );
            PENG_DP( D_PENG_LIT( "PEngAddressUtils::FindWVAddressF( normal match to %d: [%S]" ),
                     index, &matchedAddress );
#endif //PENG_ENABLE_DEBUG_PRINT



            return index;
            }
        }

    //conventional find didn't succeed
    TInt uniqueMatchIndex = KErrNotFound;
    if ( aMode == EMatchToUniqueId )
        {
        // Do the extended matching if possible:
        // - no domain in searched id
        // - no match domain available
        // - searched id / resource match to unique id / resource in array ==> match

        if ( ( WVDomain( aWVAddress ).Length() == 0 ) && ( aMatchDomain.Length() == 0 ) )
            {
            const TPtrC pureSearchUserId = WVUserId( aWVAddress );
            const TPtrC pureSearchResourceId = WVResourceId( aWVAddress );


            const TInt arrayCount( aWVAddresses.MdcaCount() );
            for ( TInt index( 0 ); index < arrayCount; index++ )
                {
                const TPtrC candidateUserId = WVUserId( aWVAddresses.MdcaPoint( index ) );
                const TPtrC candidateResourceId = WVResourceId( aWVAddresses.MdcaPoint( index ) );

                TBool userIdsMatch = ( pureSearchUserId.CompareF( candidateUserId ) == 0 );
                TBool resourceIdsMatch = ( pureSearchResourceId.CompareF( candidateResourceId ) == 0 );

                if ( userIdsMatch && resourceIdsMatch )
                    {
                    if ( uniqueMatchIndex == KErrNotFound )
                        {
                        //first match, remember the index
                        uniqueMatchIndex = index;
                        }
                    else
                        {
                        //subsequent match, previous match isn't unique anymore
                        return KErrNotFound;
                        }
                    }
                }

#ifdef PENG_ENABLE_DEBUG_PRINT
            if ( uniqueMatchIndex != KErrNotFound )
                {
                TPtrC matchedAddress = aWVAddresses.MdcaPoint( uniqueMatchIndex );
                PENG_DP( D_PENG_LIT( "PEngAddressUtils::FindWVAddressF( Unique match to %d: [%S]" ),
                         uniqueMatchIndex, &matchedAddress );
                }

#endif //PENG_ENABLE_DEBUG_PRINT
            }
        }


    //uniqueMatchIndex is either the index to unique match or KErrNotFound
    //if no match was found / or whole matching wasn't performed
    return uniqueMatchIndex;
    }


//  End of File

