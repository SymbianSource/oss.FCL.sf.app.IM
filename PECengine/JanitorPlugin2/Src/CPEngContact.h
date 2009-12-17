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
* Description:  Container of one contact item. It includes contact ID and WV IDs
*
*
*/



#ifndef __CPENGCONTACT_H__
#define __CPENGCONTACT_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <cntitem.h>
#include <cntdef.h>

// CONSTANTS
static const TUid KPbkFieldWVIdMapping = { KIntContactFieldVCardMapWV };
static const TUid KPbkFieldIMAddress   = { KUidContactFieldIMAddressValue };


// FORWARD DECLARATIONS
class CPEngNWSessionSlotID2;
class CPEngContactListNotifier2;
class CPEngContactListStore2;

// CLASS DECLARATION
/**
 *  Container of the one contact item
 *  Container of one contact item. It includes, contact Id and wv IDs
 *  Which are in for that Contact ID in the database
 *
 *  @lib PEngJanitor2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngContact ) : public CBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CPEngContact* NewL(
        CContactItem* aContactItem );

    /**
     * Two-phased constructor.
     */
    static CPEngContact* NewLC(
        CContactItem* aContactItem );

    /**
     * Two-phased constructor - creates empty object, for Find purposes
     */
    static CPEngContact* NewL(
        TContactItemId& aContactId );

    /**
     * Destructor.
     */
    virtual ~CPEngContact();

public: // Function to manipulate with contact

    /**
     * Get Contact Item ID
     *
     * @since 3.0
     * @return TContactItemId of the contact
     */
    TContactItemId ContactId() const;

    /**
     * Get list of WV Ids of the contact
     *
     * @since 3.0
     * @return reference to the array of the WV IDs
     */
    const MDesCArray& WvIds() const ;

    /**
     * Compare two Contacts based on the Contact Item ID
     *
     * @since 3.0
     * @param aContactA contact to compare with
     * @param aContactB contact to compare with
     * @return -1 if contactID B is bigger than contactID A,
                0 if it match,
                1 if contactID B is smaller than contactID A
     */
    static TInt Compare(
        const CPEngContact& aContactA,
        const CPEngContact& aContactB );

    /**
     * Compare Contact Id if it match to the passed one
     *
     * @since 3.0
     * @param aContact contact to compare with
     * @return -1 if it is bigger
     *          0 if it match
     *          1 if passed Contact ID is smaller,
     */
    TInt Compare( const CPEngContact& aContact ) const;

    /**
     * Search if WV ID is presented in the contact
     *
     * @since 3.0
     * @param aWVId WV ID to look for
     * @return ETrue if WV Id is in the contact, otherwise EFalse
     */
    TBool ValidateWVId( const TDesC& aWVId ) const;

    /**
     * Count of the WV IDs in the contact
     *
     * @since 3.0
     * @param
     * @return number on the WV IDs in the contact
     */
    TInt WVIdsCount();


    /**
     * Update contact content from the contact database
     *
     * @since 3.0
     * @param aContactDatabase contact database
     */
    void UpdateContactL( CContactDatabase& aContactDatabase );

private:

    /**
    * C++ default constructor.
    */
    CPEngContact( TContactItemId& aContactId );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( CContactItem* aContactItem );

private:    // Data

    /// OWN: Array of the WV IDs
    CDesCArrayFlat                  iWVIds;

    // OWN: Contat db Id
    TContactItemId                  iContactItemId;
    };

#endif      // __CPENGCONTACT_H__

// End of File
