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
* Description:  ContactId and ContactList ID handling tools
*
*/

#ifndef __PENGCONTACTIDSTOOLS_H__
#define __PENGCONTACTIDSTOOLS_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// CONSTANTS
_LIT( KPEngWVIDPrefix, "wv:" );
const TInt KPEngWVIDPrefixLength( 3 );

_LIT( KPEngWVResourceSeparator, "/" );
const TInt KPEngWVResourceSeparatorLength = 1;

_LIT( KPEngWVDomainSeparator, "@" );
const TInt KPEngWVDomainSeparatorLength = 1;




// NAMESPACE DEFINITION

/**
 * Namespace encapsulating a collection of
 * ContactIds utility functions.
 */
namespace NContactIdsTools
    {

    /**
     * Find Contact Id in the array.
     *
     * @since 3.0
     * @param aArray WV IDs array
     * @param aContactId Contact Id to look for
     * @param aIndex The return paramater for the index.
     * @param aUserDomain user domain
     * @return KErrNotFound if it is not found otherwise index in array
     */
    TInt FindContactIdInArray( const MDesCArray& aArray,
                               const TDesC& aContactId,
                               TInt& aIndex,
                               const TDesC& aUserDomain = KNullDesC );


    /**
     * Find Contact Id in the array.
     *
     * @since 3.0
     * @param aArray WV IDs array
     * @param aContactId Contact Id to look for
     * @param aUserDomain user domain
     * @return KErrNotFound if it is not found otherwise index in array
     */
    TInt FindContactIdInArray( const MDesCArray& aArray,
                               const TDesC& aContactId,
                               const TDesC& aUserDomain = KNullDesC );



    /**
     * Add Contact Id to the array.
     *
     * @since 3.0
     * @param aArray Contact Ids array
     * @param aContactId Contact Id to look for
     * @return index of the added element in array, or KErrAlreadyExists
     */
    TInt AddContactIdToArrayL( CDesCArray& aArray,
                               const TDesC& aContactId,
                               const TDesC& aUserDomain = KNullDesC );


    /**
     * Remove Contact Id from the array,
     *
     * @since 3.0
     * @param aArray WV IDs array
     * @param aPresenceID presence ID to look for
     * @return KErrNotFound if it is not found otherwise KErrNone
     */
    TInt RemoveContactIdFromArray( CDesCArray& aArray,
                                   const TDesC& aContactId,
                                   const TDesC& aUserDomain = KNullDesC );

    /**
     * Compare two Contact Ids
     *
     * @since 3.0
     * @param aContactId1 Contact Id 1
     * @param aContactId2 Contact Id 2
     * @param aUserDomain user domain
     * @return KErrNone if they are same, other non zero number if not
     */
    TInt CompareContactIds( const TDesC& aContactId1,
                            const TDesC& aContactId2,
                            const TDesC& aUserDomain = KNullDesC );


    } //namespace NContactIdsTools



/**
 * Namespace encapsulating a collection of
 * ContactListIds utility functions.
 */
namespace NContactListIdsTools
    {

    /**
      * Extract pure contact list name
      *
      * wv prefix, contact id and domain are removed from the contact
      * list id
      *
      * @since 3.0
      * @param aContactListId contact list id, on the end contains pure id
      * @param aPlainContactId plain part of contact id of active user
      */
    void ExtractPureContactListName( TPtrC& aContactListId,
                                     const TDesC& aPlainContactId );

    /**
      * Compare two Contact list Ids
      * Compare ignores "/" on the beginning of the lists
      *
      * @since 3.0
      * @param aContactListId1 contact list id 1
      * @param aContactListId2 contact list id 2
      * @return KErrNone if they are same, non zero number if not.
      */
    TInt CompareContactListIds( const TDesC& aContactListId1,
                                const TDesC& aContactListId2 );


    } //namespace NContactListIdsTools



#endif // __PENGCONTACTIDSTOOLS_H__
//  End of File

