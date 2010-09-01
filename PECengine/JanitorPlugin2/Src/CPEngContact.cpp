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



// INCLUDE FILES
#include    <e32std.h>
#include	<cntfldst.h>
#include	"CPEngContact.h"
#include	"PEngContactIdTools.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContact::CPEngContact
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngContact::CPEngContact(
    TContactItemId& aContactId )
        : iWVIds( 1 ), // usually only one WV per contact
        iContactItemId( aContactId )
    {
    }

// -----------------------------------------------------------------------------
// CPEngContact::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngContact::ConstructL(
    CContactItem* aContactItem )
    {
    iWVIds.Reset();
    // if there is no WV ID field then the array of the WV IDs will be empty
    CContactItemFieldSet& cardFieldSet = aContactItem->CardFields();

    TInt fieldIndex ( cardFieldSet.FindNext( KPbkFieldIMAddress,
                                             KPbkFieldWVIdMapping,
                                             KContactFieldSetSearchAll ) );
    // now read all wv ID fields
    while ( fieldIndex != KErrNotFound )
        {
        CContactTextField* textField = ( cardFieldSet[fieldIndex] ).TextStorage(); // does not take ownership
        // read contact only if it's length is more then 0
        TPtrC wvId ( textField->Text() );
        if ( wvId.Length() != 0 )
            {
            // if it leaves, it is cause of KErrNoMemory
            AddContactIdToArrayL( iWVIds, wvId );
            }
        // +1 is to start from next field
        fieldIndex = cardFieldSet.FindNext( KPbkFieldIMAddress, KPbkFieldWVIdMapping, fieldIndex + 1 );
        }
    }

// -----------------------------------------------------------------------------
// CPEngContact::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContact* CPEngContact::NewL(
    CContactItem* aContactItem )
    {
    CPEngContact* self = NewLC( aContactItem );

    CleanupStack::Pop( self ); // self

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngContact::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContact* CPEngContact::NewLC(
    CContactItem* aContactItem )
    {
    TContactItemId contactID( aContactItem->Id() );
    CPEngContact* self = new( ELeave ) CPEngContact( contactID );

    CleanupStack::PushL( self );
    self->ConstructL( aContactItem );

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngContact::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContact* CPEngContact::NewL(
    TContactItemId& aContactID )
    {
    CPEngContact* self = new( ELeave ) CPEngContact( aContactID );

    // no need to call ConstructL

    return self;
    }

// Destructor
CPEngContact::~CPEngContact()
    {
    iWVIds.Reset();
    }
// =============================================================================
// ===============Function of base class =======================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContact::ContactId()
// Return contact Item ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TContactItemId CPEngContact::ContactId() const
    {
    return iContactItemId;
    }

// -----------------------------------------------------------------------------
// CPEngContact::WvIds()
// Get List of the WV IDs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContact::WvIds() const
    {
    return iWVIds;
    }

// -----------------------------------------------------------------------------
// CPEngContact::Compare()
// Compare two Contacts if they match, based on Contact Item ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngContact::Compare(
    const CPEngContact& aContactA,
    const CPEngContact& aContactB )
    {
    return aContactA.Compare( aContactB );
    }

// -----------------------------------------------------------------------------
// CPEngContact::CompareContactID()
// Compare two Contacts if they match, based on Contact Item ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngContact::Compare(
    const CPEngContact& aContact ) const
    {
    if ( aContact.ContactId() == iContactItemId )
        {
        return KErrNone;
        }
    if ( aContact.ContactId() < iContactItemId )
        {
        return 1; // does not match
        }
    return -1; // does not match
    }

// -----------------------------------------------------------------------------
// CPEngContact::ValidateWVId()
// Search if WV Id is presented in the contact
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEngContact::ValidateWVId(
    const TDesC& aWVId ) const
    {
    return !( KErrNotFound == FindContactIdInArray( iWVIds, aWVId ) );
    }

// -----------------------------------------------------------------------------
// CPEngContact::WVIdsCount()
// Count of the WV IDs in the contact
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngContact::WVIdsCount()
    {
    return iWVIds.Count();
    }

// -----------------------------------------------------------------------------
// CPEngContact::UpdateContactL()
// Update contact from the contact db
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContact::UpdateContactL(
    CContactDatabase& aContactDatabase )
    {
    // update content of the Contact from the database
    CContactItem* contactItem =
        aContactDatabase.ReadMinimalContactLC( iContactItemId );

    iWVIds.Reset();
    ConstructL( contactItem );
    CleanupStack::PopAndDestroy(); // contactItem
    }


//  End of File
