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
* Description:  Contact Database observer
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include	"CPEngListObserver.h"

#include	"CPEngNWSessionSlotID2.h"
#include	"CPEngContactListNotifier2.h"
#include	"CPEngContactListStore2.h"
#include	"MPEngContactList2.h"
#include	"MPEngContactListProperties.h"
#include	"MPEngContactItem.h"
#include	"PEngWVContactListExtensions2.h"

#include	"MPEngContactMapper.h"


// LOCAL CLASS DEFINITION
NONSHARABLE_CLASS( TPEngContactListNameWrapper ) : public MDesC16Array
    {
public:
    TPEngContactListNameWrapper(
        RPointerArray<MPEngContactList2>& aContactLists )
            : iContactLists( aContactLists )
        {
        }
public: // from MDesC16Array
    TInt MdcaCount() const
        {
        return iContactLists.Count();
        }

    TPtrC16 MdcaPoint( TInt aIndex ) const
        {
        return iContactLists[ aIndex ]->ListProperties().Name();
        }

public: // data
    RPointerArray<MPEngContactList2>& iContactLists;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngListObserver::CPEngListObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngListObserver::CPEngListObserver(
    MPEngContactMapper& aMapper )
        : iContactMapper( aMapper )

    {
    }

// -----------------------------------------------------------------------------
// CPEngListObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngListObserver::ConstructL(
    const CPEngNWSessionSlotID2& aSessionId )
    {
    iSessionId = aSessionId.CloneL();
    iListStore = CPEngContactListStore2::NewL( aSessionId );
    iListNotifier = CPEngContactListNotifier2::NewL( aSessionId );
    User::LeaveIfError( iListNotifier->AddObserver( *this ) );
    RefreshSupportedListsL();
    }

// -----------------------------------------------------------------------------
// CPEngListObserver::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngListObserver* CPEngListObserver::NewLC(
    MPEngContactMapper& aMapper,
    const CPEngNWSessionSlotID2& aSessionId )

    {
    CPEngListObserver* self = new( ELeave ) CPEngListObserver( aMapper );

    CleanupStack::PushL( self );
    self->ConstructL( aSessionId );

    return self;
    }

// Destructor
CPEngListObserver::~CPEngListObserver()
    {
    iContactLists.Reset();
    delete iListNotifier;
    delete iListStore;
    delete iSessionId;
    }

// =============================================================================
// ===============Functions of MPEngContactListObserver2 class ========================
// =============================================================================
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngListObserver::HandleContactListChangeL()
// Contact list change handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngListObserver::HandleContactListChangeL(
    CPEngContactListNotifier2& /* aNotifier */,
    const TDesC& aContactListName )
    {
    // was it contact list store changed?
    if ( aContactListName == KNullDesC )
        {
        RefreshSupportedListsL();
        return;
        }
    RemapContactListL( ContactListL( aContactListName ) );
    }


// -----------------------------------------------------------------------------
// CPEngListObserver::HandleContactListError()
// Notification failure handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngListObserver::HandleContactListError(
    TInt /*aError*/,
    CPEngContactListNotifier2& /*aNotifier*/ )
    {
    // no error handling
    }

// =============================================================================
// ===============Functions of the base class ==================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngListObserver::UpdateMappingL()
// Update mapping for gived WV Id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngListObserver::UpdateMappingL(
    const TDesC& aWVId,
    const CContactIdArray* aContactIds )
    {
    TInt count( iContactLists.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        MPEngContactList2& contactList = *( iContactLists[ x ] );
        TInt index( contactList.FindContact( aWVId ) );
        if ( index != KErrNotFound )
            {
            RemapContactItemL( contactList.ContactItem( index ), aContactIds );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngListObserver::ReMappAllContactsL()
// Do full re-mapping of all contact list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngListObserver::ReMappAllContactsL()
    {
    TInt count( iContactLists.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        RemapContactListL( *iContactLists[ x ], ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CPEngListObserver::SessionId()
// Session slot Id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CPEngNWSessionSlotID2& CPEngListObserver::SessionId() const
    {
    return *iSessionId;
    }

// -----------------------------------------------------------------------------
// CPEngListObserver::RefreshSupportedListsL
// Build list of supported contact lists
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngListObserver::RefreshSupportedListsL()
    {
    iContactLists.Reset();
    iListNotifier->Stop();
    const MDesC16Array& lists = iListStore->AllContactListsL();
    TInt count( lists.MdcaCount() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( EFalse != iListStore->ContactListPropertiesL(
                 lists.MdcaPoint( x )
             ).Property(
                 KPEngContactToContactDbMapping,
                 KPEngCntLstPropertyExternalPermanent )
           )
            {
            MPEngContactList2& contactList = iListStore->ContactListL(
                                                 lists.MdcaPoint( x ) );
            iContactLists.AppendL( &contactList );
            RemapContactListL( contactList );
            }
        }
    TPEngContactListNameWrapper listNames( iContactLists );
    User::LeaveIfError( iListNotifier->Start( listNames ) );
    }

// -----------------------------------------------------------------------------
// CPEngListObserver::ContactListL
// Find Contact list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MPEngContactList2& CPEngListObserver::ContactListL(
    const TDesC& aContactList )
    {
    TInt count( iContactLists.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( KErrNone
             ==
             aContactList.CompareF(
                 iContactLists[ x ]->ListProperties().Name() )
           )
            {
            return *( iContactLists[ x ] );
            }
        }
    User::Leave( KErrNotFound );
    // just for compiler
    return *( iContactLists[ 0 ] );
    }


// -----------------------------------------------------------------------------
// CPEngListObserver::RemapContactListL
// Remap whole contact list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngListObserver::RemapContactListL(
    MPEngContactList2& aContactList,
    TBool aForceUpdate /* EFalse */ )
    {
    aContactList.SuspendUpdateNotification();
    TInt count ( aContactList.Count() );
    for ( TInt x( 0 ) ;  x < count ; ++x )
        {
        MPEngContactItem& contact = aContactList.ContactItem( x );
        TPtrC8 value;
        if ( aForceUpdate
             ||
             ( KErrNotFound == contact.GetProperty(
                   KPEngCntItemExtensionMappedContactDbIds,
                   KPEngCntItemPropertyExternalCached,
                   value ) )
           )
            {
            // not mapped yet, map
            CContactIdArray* contactIds =
                iContactMapper.MapWvIdToContactIdLC( contact.Id() );
            RemapContactItemL( contact, contactIds );
            CleanupStack::PopAndDestroy( contactIds );
            }
        }
    aContactList.ReleaseUpdateNotification();
    }

// -----------------------------------------------------------------------------
// CPEngListObserver::RemapContactListL
//  Remap one Contact Item with given contact db ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngListObserver::RemapContactItemL(
    MPEngContactItem& aContactItem,
    const CContactIdArray* aContactIds )
    {
    if ( aContactIds )
        {
        HBufC8* packedIds =
            PEngContactItemExtensionCodec2::PackContactIdsLC(
                *aContactIds );
        aContactItem.SetPropertyL( KPEngCntItemExtensionMappedContactDbIds,
                                   KPEngCntItemPropertyExternalCached,
                                   *packedIds );

        CleanupStack::PopAndDestroy( packedIds );
        }
    else
        {
        aContactItem.SetPropertyL( KPEngCntItemExtensionMappedContactDbIds,
                                   KPEngCntItemPropertyExternalCached,
                                   KNullDesC8 );
        }
    aContactItem.DeletePropertyL( KPEngCntItemExtensionMappingCleaning,
                                  KPEngCntItemPropertyExternalCached );
    }

//  End of File
