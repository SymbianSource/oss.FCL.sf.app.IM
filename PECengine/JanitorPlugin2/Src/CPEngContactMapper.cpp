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
* Description:  Container of one contact list item. It includes, wv ID, nick name,
*				 List of contact IDs, flag if there is need to notify about change of the
*				 Presence change of the wv ID, and flag if wv ID: was checked by Janitor
*
*/



// INCLUDE FILES
#include	"CPEngContactMapper.h"

#include    <e32std.h>
#include	<bamdesca.h>
#include	<cntitem.h>

#include	"CPEngListObserver.h"
#include	"CPEngContact.h"

#include	"CPEngNWSessionSlotNotifier2.h"
#include	"CPEngNWSessionSlotEvent2.h"
#include	"CPEngNWSessionSlotID2.h"

// Debug prints
#include    "PresenceDebugPrint.h"


// CONSTANTS
#define ORDER TLinearOrder<CPEngContact> (CPEngContact::Compare)

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactMapper::CPEngContactMapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngContactMapper::CPEngContactMapper()
    {
    PENG_DP( D_PENG_LIT( "PEngJanitorPlugIn: CPEngContactMapper::CPEngContactMapper" ) );
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::ConstructL()
    {
    // create contact cache and register observer
    iCntDatabase = CContactDatabase::OpenL();
    iCntNotifier = CContactChangeNotifier::NewL( *iCntDatabase, this );
    RebuildContactDbCacheL();

    iSessionNotifier = CPEngNWSessionSlotNotifier2::NewL();
    User::LeaveIfError( iSessionNotifier->AddObserver( *this ) );
    User::LeaveIfError( iSessionNotifier->Start() );
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContactMapper* CPEngContactMapper::NewL()
    {
    CPEngContactMapper* self = NewLC( );

    CleanupStack::Pop(); // self

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContactMapper* CPEngContactMapper::NewLC()
    {
    CPEngContactMapper* self = new( ELeave ) CPEngContactMapper();

    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }

// Destructor
CPEngContactMapper::~CPEngContactMapper()
    {
    iContacts.ResetAndDestroy();
    iListObservers.ResetAndDestroy();
    delete iSessionNotifier;
    delete iCntNotifier;
    delete iCntDatabase;
    }

// =============================================================================
// ===============Function for MPEngContactMapper class ========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactMapper::MappWvIdL
// Map wv id to the contact db id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CContactIdArray* CPEngContactMapper::MapWvIdToContactIdLC(
    const TDesC& aWVId )
    {
    CContactIdArray* contactIds = NULL;
    TInt count ( iContacts.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( iContacts[ x ]->ValidateWVId( aWVId ) )
            {
            if ( !contactIds )
                {
                contactIds = CContactIdArray::NewLC();
                }
            contactIds->AddL( iContacts[ x ]->ContactId() );
            }
        }
    // even no contact id was created, push NULL to maintain LC continuity
    if ( !contactIds )
        {
        CleanupStack::PushL( contactIds );
        }
    return contactIds;
    }

// =============================================================================
// ===============Function for MContactDBObserver class ========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactMapper::HandleDatabaseEventL
// Handle Contact Database event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::HandleDatabaseEventL(
    TContactDbObserverEvent aEvent )
    {
    switch ( aEvent.iType )
        {
        case EContactDbObserverEventContactDeleted:
            {
            RemoveContactL( aEvent.iContactId );
            break;
            }
        case EContactDbObserverEventContactAdded:
            {
            AddContactL( aEvent.iContactId );
            break;
            }
        case EContactDbObserverEventContactChanged:
            {
            UpdateContactL( aEvent.iContactId );
            break;
            }
        case EContactDbObserverEventUnknownChanges:
            {
            // reload whole contact cache, since more changes has happened
            HandleMultipleChangesL();
            }
        default:
            {
            break;
            }
        }
    }


// =============================================================================
// ===============Function for MPEngNWSessionSlotObserver2 class ===============
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactMapper::HandleNWSessionSlotChangeL
// NWSessionSlot change handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::HandleNWSessionSlotChangeL(
    CPEngNWSessionSlotNotifier2& /*aNotifier*/,
    CPEngNWSessionSlotEvent2& aEvent )
    {
    TInt index( FindListObserver( aEvent.NWSessionSlotID() ) );
    if ( EPEngNWPresenceSessionOpen == aEvent.NWSessionSlotState() )
        {
        // make sure we are observing such a session
        if ( index != KErrNotFound )
            {
            return;
            }
        CPEngListObserver* observer = CPEngListObserver::NewLC(
                                          *this,
                                          aEvent.NWSessionSlotID() );
        iListObservers.AppendL( observer );
        CleanupStack::Pop( observer ); // observer
        }
    else
        {
        // stop observing of such a session
        if ( index != KErrNotFound )
            {
            delete iListObservers[ index ];
            iListObservers.Remove( index );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::HandleNWSessionSlotError
// Notification failure handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::HandleNWSessionSlotError(
    TInt /*aError*/,
    CPEngNWSessionSlotNotifier2& /*aNotifier */ )
    {
    // no error handling here
    }

// =============================================================================
// =============== New Function of base class =================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactMapper::AddContactL
// Inserts new contact to the contact db cache
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::AddContactL(
    TContactItemId& aContactId )
    {
    CContactItem* cntItem = iCntDatabase->ReadMinimalContactLC( aContactId );
    CPEngContact* contactEntry = CPEngContact::NewL( cntItem ); // careful no in CleanUpStack
    CleanupStack::PopAndDestroy( cntItem ); // cntItem
    const MDesCArray& wvIds = contactEntry->WvIds();
    if ( ! wvIds.MdcaCount() )
        {
        delete contactEntry;
        return;
        }
    CleanupStack::PushL( contactEntry );
    iContacts.AppendL( contactEntry );
    CleanupStack::Pop( contactEntry );
    // update mapping, for each wv id
    RemapWvIdsL( wvIds );
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::RemoveContactL
// Remove contact from contact db cache
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::RemoveContactL(
    TContactItemId& aContactId )
    {
    TInt index( FindContact( aContactId ) );
    // if contact is in our cache, we can leave, nothing to do
    if ( index == KErrNotFound )
        {
        return;
        }
    CPEngContact* contact = iContacts[ index ];
    CleanupStack::PushL( contact );
    iContacts.Remove( index );
    RemapWvIdsL( contact->WvIds() );
    CleanupStack::PopAndDestroy( contact );
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::UpdateContactL
// Update content of the contact in  contact db cache
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::UpdateContactL(
    TContactItemId& aContactId )
    {
    TInt index( FindContact( aContactId ) );
    CPEngContact* contactEntry = NULL;
    if ( index == KErrNotFound )
        {
        // contact is not in the cache, try to add it
        CContactItem* cntItem = iCntDatabase->ReadMinimalContactLC( aContactId );
        contactEntry = CPEngContact::NewL( cntItem ); // careful no in CleanUpStack
        CleanupStack::PopAndDestroy( cntItem ); // cntItem
        const MDesCArray& wvIds = contactEntry->WvIds();
        TInt wvIdCount( wvIds.MdcaCount() );
        if ( !wvIdCount )
            {
            delete contactEntry;
            return;
            }
        CleanupStack::PushL( contactEntry );
        iContacts.AppendL( contactEntry );
        CleanupStack::Pop( contactEntry );
        RemapWvIdsL( contactEntry->WvIds() );
        }
    else
        {
        // contact is in cache, update it, first store old wv ids
        contactEntry = iContacts[ index ];
        const MDesCArray& oldIds = contactEntry->WvIds();
        TInt count ( oldIds.MdcaCount() );
        CDesCArray* oldWVIds = new( ELeave ) CDesCArrayFlat( count );
        CleanupStack::PushL( oldWVIds );
        for ( TInt x( 0 ) ; x < count ; ++x )
            {
            oldWVIds->AppendL( oldIds.MdcaPoint( x ) );
            }
        contactEntry->UpdateContactL( *iCntDatabase );

        // does contact entry need have any wv ids or it can be removed
        const MDesCArray& newIds = contactEntry->WvIds();
        TInt newCount( newIds.MdcaCount() );
        if ( ! newCount )
            {
            iContacts.Remove( index );
            delete contactEntry;
            }
        else
            {
            // remove new wv ids from old ones if the are same
            // so we avoid double remapping
            for ( TInt x( 0 ) ;  x < newCount ; ++x )
                {
                TInt pos( 0 );
                if ( KErrNone == oldWVIds->Find( newIds.MdcaPoint( x ), pos ) )
                    {
                    oldWVIds->Delete( pos );
                    }
                }
            RemapWvIdsL( newIds );
            }
        RemapWvIdsL( *oldWVIds );
        CleanupStack::PopAndDestroy( oldWVIds ); //
        }
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::HandleMultipleChangesL
// More changes happened in the contact db
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::HandleMultipleChangesL()
    {
    // rebuild whole contact db and call on all contact list full re-mapping
    RebuildContactDbCacheL();
    TInt count( iListObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iListObservers[ x ]->ReMappAllContactsL();
        }
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::UpdateMappingOnContactListsL
// Notify all list observers about wv Ids mapping change
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::UpdateMappingOnContactListsL(
    const TDesC& aWVId,
    const CContactIdArray* aContactIds )
    {
    TInt count( iListObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iListObservers[ x ]->UpdateMappingL( aWVId, aContactIds );
        }
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::RebuildContactDbCacheL()
// Rebuild contact db cache
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::RebuildContactDbCacheL()
    {
    // check all contacts
    _LIT( KWVStart, "" );
    // Specify in which fields search should be performed
    CContactItemFieldDef* fieldToSearchIn = new ( ELeave ) CContactItemFieldDef();
    CleanupStack::PushL( fieldToSearchIn );
    fieldToSearchIn->AppendL( KPbkFieldWVIdMapping );
    fieldToSearchIn->AppendL( KPbkFieldIMAddress );
    // search in contact database
    CContactIdArray* contactIds = iCntDatabase->FindLC( KWVStart,
                                                        fieldToSearchIn );

    // OK now we have Contact Ids, time to add contacts to cache
    // reset our array
    iContacts.ResetAndDestroy();
    CPEngContact* contact = NULL;
    CContactItem* contactItem = NULL;
    TInt count ( contactIds->Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        contactItem = iCntDatabase->ReadMinimalContactLC( ( *contactIds )[ x ] );
        contact = CPEngContact::NewLC( contactItem );
        // store back up of the contact only if there was some id
        if ( contact->WvIds().MdcaCount() )
            {
            iContacts.AppendL( contact );
            CleanupStack::Pop( contact ); // contact
            }
        else
            {
            CleanupStack::PopAndDestroy( contact ); // contact
            }
        CleanupStack::PopAndDestroy( contactItem ); // contactItem
        }
    CleanupStack::PopAndDestroy( 2 ); // contactIDArray, fieldToSearchIn
    }


// -----------------------------------------------------------------------------
// CPEngContactMapper::FindContact
// Find Contact in contact cache
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngContactMapper::FindContact(
    TContactItemId& aContactId ) const
    {
    TInt count ( iContacts.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if (  iContacts[ x ]->ContactId() == aContactId )
            {
            return x;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CPEngContactMapper::RemapWvIdsL
// Remap given wv Ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactMapper::RemapWvIdsL( const MDesCArray& aWVIds )
    {
    TInt count( aWVIds.MdcaCount() );
    for ( TInt x( 0 ) ;  x < count ; ++x )
        {
        CContactIdArray* contactIds =
            MapWvIdToContactIdLC( aWVIds.MdcaPoint( x ) );
        UpdateMappingOnContactListsL( aWVIds.MdcaPoint( x ), contactIds );
        CleanupStack::PopAndDestroy( contactIds ); // contactIds
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactMapper::FindListObserver
// Find Contact list observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngContactMapper::FindListObserver(
    const CPEngNWSessionSlotID2& aSessionId ) const
    {
    TInt count( iListObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( KErrNone
             ==
             iListObservers[ x ]->SessionId().MatchBasePart( aSessionId )
           )
            {
            return x;
            }
        }
    return KErrNotFound;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
