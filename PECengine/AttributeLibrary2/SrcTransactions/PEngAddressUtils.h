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

#ifndef PENGADDRESSUTILS_H__
#define PENGADDRESSUTILS_H__


//  INCLUDES
#include <e32base.h>
#include <BamDescA.h>


// CLASS DECLARATION

/**
 * Tools for matching & handling presence addresses & user id's.
 *
 * Generic WV address syntax is defined as follows (in WV CSP 1.1):
 *
 * Address = "wv:" [User-ID] ["/" Resource] ["@" Domain]
 * Resource = Group-ID | Contact-List-ID | Content-ID
 * Domain = sub-domain *("." sub-domain)
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( PEngAddressUtils )
    {
public: //Fragment extractors

    /**
     * Match mode for for locating / matching WV addresses.
     *
     * ENormal: Normal matching with match domain.
     * EMatchToUniqueId: With this mode, extended uniqued
     * id matching is performed if normal find fails.
     *
     * @since 3.0
     */
    enum TPEngAddressMatchdMode { ENormal, EMatchToUniqueId };




public: //Fragment extractors

    /**
     * Extracts user id part from possibly complete WV address.
     * E.g. gets "john.doe" from "wv:john.doe/friends@imps.com"
     *
     * Possible user ID address structures are:
     *  - "wv:john/colleagues@imps.com"
     *  - "wv:john/friends"
     *  - "wv:john
     *
     * @since 3.0
     * @param aWVAddress WV address.
     * @return Plain user ID part without starting schema prefix
     * or without trailing resource and domain definition.
     * If no user id present in given address, returns empty descriptor.
     */
    static TPtrC WVUserId( const TDesC& aWVAddress );



    /**
     * Extracts resource id part from possibly complete WV address.
     * E.g. gets "friends" from "wv:john.doe/friends@imps.com"
     *
     * Possible resource ID address structures are:
     *  - "wv:john/colleagues@imps.com"
     *  - "wv:/managers"
     *  - "wv:john/friends"
     *  - "wv:/managers@imps.com"
     *
     * @since 3.0
     * @param aWVAddress WV address.
     * @return Resource id part without starting user / schema prefix
     * or without trailing domain definition.
     * If no resource part present in given address, returns empty descriptor.
     */
    static TPtrC WVResourceId( const TDesC& aWVAddress );



    /**
     * Extracts domain part from possibly complete WV address.
     * E.g. gets "imps.com" from "wv:john.doe/friends@imps.com"
     *
     * Possible domain address structures are:
     *  - "wv:john/colleagues@imps.com"
     *  - "wv:john@imps.com"
     *  - "wv:/managers@imps.com"
     *
     * @since 3.0
     * @param aWVAddress WV address.
     * @return Domain part without starting user / resource parts.
     * If no resource part present in given address, returns empty descriptor.
     */
    static TPtrC WVDomain( const TDesC& aWVAddress );



    /**
     * Strips possible wv-schema part away from complete
     * WV address and returns the rest.
     *
     * @since 3.0
     * @param aWVAddress Full WV address to reduce.
     * @return Plain presence address without wv-schema prefix.
     */
    static TPtrC ReducedWVAddress( const TDesC& aWVAddress );



public: //Address generator


    /**
     * Composes canonic WV address from
     * given WV addres parts.
     *
     * If some given part is zero length,
     * then corresponding part is not present
     * in generated full WV address.
     *
     * Either user id part or resource id part must be defined.
     * In other cases a leave with KErrArgument is raised.
     *
     * @since 3.0
     * @param aUserId User id
     * @param aResourceId
     * @param aDomain
     * @return Canonic WV address.
     */
    static HBufC* CanonicWVAddressLC( const TDesC& aUserId,
                                      const TDesC& aResourceId,
                                      const TDesC& aDomain );



public: //Match & find

    /**
     * Checks does two WV address match.
     * (Do they refer same WV user / resource.)
     *
     * Matching is case insensitive and possible "wv:" schema
     * prefixes are ignored in matching.
     *
     * Example 1: No match domain information available
     *  - "wv:User" == "wv:User" (Direct match.)
     *  - "wv:User" == "User"    (Direct match, schema prefix ignored.)
     *  - "wv:User@Server.com" == "wv:User@Server.com"  (Direct match.)
     *
     *
     * Example 2: Match domain "Server.com"
     *  - "wv:User" == "wv:User" (Direct match.)
     *  - "wv:User" == "User"    (Direct match, schema prefix ignored.)
     *  - "wv:User@Server.com" == "wv:User@Server.com"  (Direct match.)
     *
     *  - "wv:User" == "wv:User@Server.com"         (Domain extended match.)
     *  - "wv:User@Server.com" == "wv:User"         (Domain extended match.)
     *
     * @since 3.0
     * @param aWVAddress1 First WV address.
     * @param aWVAddress2 Second WV address.
     * @param aMatchDomain Match domain. If no match domain available
     * pass empty descriptor.
     * @return ETrue if passed addresses refer same WV user / resource.
     * Else EFalse.
     */
    static TBool MatchWVAddressesF( const TDesC& aWVAddress1,
                                    const TDesC& aWVAddress2,
                                    const TDesC& aMatchDomain );



    /**
     * Finds matching WV address from array of WV
     * addresses. (Finds a WV address which refers
     * to same user / resource than given address.)
     *
     * Gets the index for matching address from
     * passed descriptor array. Matching is case
     * insensitive and possible "wv:" schema prefixes are
     * ignored in matching.
     *
     * If non empty match domain is provided, local addresses
     * (addresses without domain part) are matched to users or
     * resources with matching aMatchDomain domain.
     *
     * If no matching presence id is found, returns KErrNotFound
     * If there is matching entry, returns index of it.
     * (In case there is several matches, returns the index of
     * first found.)
     *
     * If match mode EMatchToUniqueId is used, standard matching is
     * extended  with unique id matching if normal find fails.
     * Extended unique id matching is performed only if:
     * - empty match domain is provided
     * - also the searched address is local one (missing the domain part)
     *
     * Extended unique id matching succeeds if the given searched
     * user ID/resource ID matches only one WV address's user ID/resource ID
     * part. If there is several possible user ID / resource ID matches ion
     * the array, then KErrNotFound is returned, because the match is not
     * anymore confident.
     *
     * @since 3.0
     * @param aWVAddress The WV address to locate.
     * @param aWVAddresses The array of WV addresses to search.
     * @param aMatchDomain The match domain to use.
     * @param aMode The mode for matching.
     * @return Returns find operation result.
     * KErrNotFound if no matching address was found.
     * Else the index of matching entry.
     */
    static TInt FindWVAddressF( const TDesC& aWVAddress,
                                const MDesCArray& aWVAddresses,
                                const TDesC& aMatchDomain,
                                TPEngAddressMatchdMode aMode = ENormal );

private: //Prohibited constructor and destructor

    PEngAddressUtils();
    ~PEngAddressUtils();
    };

#endif      //  PENGADDRESSUTILS_H__

//  End of File

