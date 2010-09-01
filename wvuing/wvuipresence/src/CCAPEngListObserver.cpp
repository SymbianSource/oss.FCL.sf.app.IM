/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manager for presence-list handling
 *               (Contact-list updater/ Attribute-list publisher ).
 *
*/



// INCLUDE FILES
#include "CCAPEngListObserver.h"
#include "ChatDebugPrint.h"
#include "CAPresenceDefinitions.h"
#include "CAPresenceUtils.h"
#include "CCAPresenceErrors.h"
#include "CAPresenceConst.h"

#include "CCAStorageManagerFactory.h"
#include "MCAStoredContacts.h"
#include "MCAStoredContact.h"
#include "MCAContactList.h"
#include "MCAPresenceUpdater.h"

#include <CPengPresenceNotifier2.h>
#include <CPEngContactListStore2.h>
#include <MPEngContactList2.h>
#include <MPEngContactListObserver2.h>
#include <MPEngContactListProperties.h>
#include <PEngWVPresenceAttributes2.h>
#include <CPEngContactListNotifier2.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAPEngListObserver::CCAPEngListObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAPEngListObserver::CCAPEngListObserver(
    CPEngPresenceNotifier2& aAttributeNotifier,
    RArray<TUint32>& aHandledAttributes,
    MCAPresenceUpdater& aUpdater )
        : iAttributeNotifier( aAttributeNotifier ),
        iAttributes( aHandledAttributes ),
        iPresenceUpdater( aUpdater )
    {
    CHAT_DP_FUNC_ENTER( "CCAPEngListObserver" );
    CHAT_DP_FUNC_DONE( "CCAPEngListObserver" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::ConstructL( CPEngNWSessionSlotID2* aSessionSlotID )
    {
    CHAT_DP_FUNC_ENTER( "ConstructL" );

    iStoredContacts = CCAStorageManagerFactory::ContactListInterfaceL();
    iActiveListId = KNullDesC().AllocL();

    iPEngContactListNotifier =
        CPEngContactListNotifier2::NewL( *aSessionSlotID );
    iPEngContactListNotifier->AddObserver( *this );

    iContactListStore = CPEngContactListStore2::NewL( *aSessionSlotID );

    CHAT_DP_FUNC_DONE( "ConstructL" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAPEngListObserver* CCAPEngListObserver::NewL(
    CPEngNWSessionSlotID2* aSessionSlotID,
    CPEngPresenceNotifier2& aAttributeNotifier,
    RArray<TUint32>& aHandledAttributes,
    MCAPresenceUpdater& aUpdater )
    {
    CHAT_DP_FUNC_ENTER( "NewL" );

    CCAPEngListObserver* self = new( ELeave ) CCAPEngListObserver(
        aAttributeNotifier,
        aHandledAttributes,
        aUpdater );

    CleanupStack::PushL( self );
    self->ConstructL( aSessionSlotID );
    CleanupStack::Pop( self );

    CHAT_DP_FUNC_DONE( "NewL" );

    return self;
    }

// Destructor
CCAPEngListObserver::~CCAPEngListObserver()
    {
    CHAT_DP_FUNC_ENTER( "~CCAPEngListObserver" );

    if ( iPEngContactListNotifier )
        {
        iPEngContactListNotifier->Stop();
        iPEngContactListNotifier->RemoveObserver( *this );
        }

    delete iContactListStore;
    delete iPEngContactListNotifier;
    delete iActiveListId;

    CHAT_DP_FUNC_DONE( "~CCAPEngListObserver" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::SetActiveListIdL
// Setter for active contact list identifier
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::SetActiveListIdL( const TDesC& aListId )
    {
    CHAT_DP_FUNC_ENTER( "SetActiveListId" );

    HBufC* tmp = aListId.AllocL();
    delete iActiveListId;
    iActiveListId = tmp;

    // restart notifier
    iPEngContactListNotifier->Stop();
    iPEngContactListNotifier->Start( *iActiveListId );

    // this method is needed as we might move the getter to here also

    CHAT_DP_FUNC_DONE( "SetActiveListId" );
    }


// -----------------------------------------------------------------------------
// CCAPEngListObserver::SetListIdsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::SetListIds( const MDesCArray& aListIds )
    {
    // restart notifier
    iPEngContactListNotifier->Stop();
    iPEngContactListNotifier->Start( aListIds );
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::ClearListNotifier
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::ClearListNotifier()
    {
    iPEngContactListNotifier->Stop();
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::StartNotifier
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::StartNotifier( const TDesC& aListId )
    {
    iPEngContactListNotifier->Stop();
    iPEngContactListNotifier->Start( aListId );
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::AddL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::AddL( const TDesC& aListId )
    {
    if ( iPEngContactListNotifier->IsActive() )
        {
        TInt err = iPEngContactListNotifier->Add( aListId );
        if ( err != KErrAlreadyExists )
            {
            User::LeaveIfError( err );
            }
        }
    else
        {
        User::LeaveIfError( iPEngContactListNotifier->Start( aListId ) );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::Remove
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::Remove( const TDesC& aListId )
    {
    iPEngContactListNotifier->Remove( aListId );
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::HandleContactListChangeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::HandleContactListChangeL(
    CPEngContactListNotifier2& /*aNotifier*/,
    const TDesC& aContactListName  )
    {
    CHAT_DP_FUNC_ENTER( "HandleContactListChangeL" );

    MPEngContactList2& list =
        iContactListStore->ContactListL( aContactListName );

    // fetch list id and display name
    MPEngContactListProperties& prop = list.ListProperties();
    TPtrC id( aContactListName );
    TPtrC name( KNullDesC );
    prop.DisplayName( name );
    CHAT_DP( D_CHAT_LIT( "**listchange: %S [%S]" ), &id, &name );

    MCAContactList* storedList = iStoredContacts->FindContactList( id );
    if ( storedList )
        {
        // list found, update displayname if it's changed
        if ( name.Length() > 0 &&
             storedList->DisplayName().Compare( name ) != 0 )
            {
            storedList->SetDisplayNameL( name );
            }
        }
    else
        {
        // list not found, nothing to update
        return;
        }

    // check removed contacts
    const MDesCArray& removed = list.RemovedContacts();
    TInt removeCount( removed.MdcaCount() );
    CHAT_DP( D_CHAT_LIT( "removed wvids count %d" ), removeCount );
    for ( TInt i( 0 ); i < removeCount; ++i )
        {
        TPtrC id( removed.MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "HandleContactListChangeL - removed id %S" ), &id );
        iStoredContacts->RemoveContact( aContactListName, id );

        // remove from attribute notifier
        iAttributeNotifier.Remove( id );
        }

    // check added contacts
    const MDesCArray& added = list.AddedContacts();
    TInt addCount( added.MdcaCount() );

    CHAT_DP( D_CHAT_LIT( "added wvids count %d" ), addCount );
    for ( TInt ii( 0 ); ii < addCount; ++ii )
        {
        TInt i( list.FindContact( added.MdcaPoint( ii ) ) );
        // might be already removed
        if ( i != KErrNotFound )
            {
            const MPEngContactItem& contact = list.ContactItem( i );
            TPtrC id( contact.Id() );
            TPtrC nick( contact.NickName() );
            CHAT_DP( D_CHAT_LIT( "HandleContactListChangeL - contact id %S, \
                                  nick %S" ), &id, &nick );

            // add to attribute notifier
            TInt err = iAttributeNotifier.Add( id, iAttributes.Array() );

            // updates existing contact or creates new
            iStoredContacts->CreateContactL( aContactListName, nick, id );
            // make sure to update the presence
            iPresenceUpdater.UpdatePresenceL( id );

            // leave with error.
            // Ignore inactive notifier (manual refresh mode)
            User::LeaveIfError( err == KErrNotReady ? KErrNone : err );
            }
        }

    CHAT_DP_FUNC_DONE( "HandleContactListChangeL" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::HandleContactListError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::HandleContactListError( TInt aError,
                                                  CPEngContactListNotifier2& /*aNotifier*/ )
    {
    // ignored
    CHAT_DP_FUNC_ENTER( "HandleContactListNotifyError" );

    // but print some debug information
    CHAT_DP( D_CHAT_LIT( "CCAPEngListObserver::HandleContactListNotifyError - \
                          Got error %d" ), aError );

    CHAT_DP_FUNC_DONE( "HandleContactListNotifyError" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListObserver::SkipStorageRemoval
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListObserver::SkipStorageRemoval()
    {
    iSkipRemoval = ETrue;
    }

//  End of File
