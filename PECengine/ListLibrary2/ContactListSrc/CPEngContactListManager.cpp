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
* Description:  Contact lists manager
*
*/

// INCLUDE FILES
#include <e32std.h>

#include "CPEngContactListManager.h"

#include "CPEngContactListModBase.h"
#include "CPEngContactListSettings.h"
#include "CPEngSessionSlotId.h"

#include "MPEngListLibFactory.h"

#include "PresenceDebugPrint.h"

#include "PEngContactIdsTools.h"

#include "MPEngStorageManager.h"


//Default granurality for list of lists
const TInt KListOfListsGranurality = 3;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListManager::CPEngContactListManager()
// -----------------------------------------------------------------------------
//
CPEngContactListManager::CPEngContactListManager( MPEngListLibFactory& aFactory )
        : CPEngContactListManagerBase( aFactory ),
        iAllLists( KListOfListsGranurality ),
        iSynchronizedLists( KListOfListsGranurality )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManager::ConstructL( const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngContactListManagerBase::ConstructL( aSessionSlot );
    HandleSettingsUpdateL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::NewL()
// -----------------------------------------------------------------------------
//
CPEngContactListManager* CPEngContactListManager::NewL(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngContactListManager* self = NewLC( aFactory, aSessionSlot );
    CleanupStack::Pop();  //self
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContactListManager* CPEngContactListManager::NewLC(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngContactListManager* self =
        new ( ELeave ) CPEngContactListManager( aFactory );

    CleanupClosePushL( *self );
    self->ConstructL( aSessionSlot );
    return self;
    }


// Destructor
CPEngContactListManager::~CPEngContactListManager()
    {
    iAllLists.Reset();
    iSynchronizedLists.Reset();
    }



// =============================================================================
// ===============Function from MPEngContactListManager =======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListManager::Close()
// -----------------------------------------------------------------------------
//
void CPEngContactListManager::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 )
        {
        iFactory.RemoveCntListManager( this );
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::ContactListL()
// -----------------------------------------------------------------------------
//
MPEngContactListAdvance& CPEngContactListManager::ContactListL(
    const TDesC& aContactList )
    {
    return LoadContactListModelL( aContactList, EFalse, EFalse );
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::PEngActiveContactLists()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListManager::AllContactListsL() const
    {
    return iAllLists;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::PEngActiveContactLists()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListManager::SynchronizedContactLists() const
    {
    return iSynchronizedLists;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::ContactListSettingsL()
// -----------------------------------------------------------------------------
//
MPEngContactListProperties& CPEngContactListManager::ContactListSettingsL(
    const TDesC& aContacList )
    {
    CPEngContactListSettings* settings = ContactListSettingsOrNull( aContacList );
    if ( !settings )
        {
        User::Leave( KErrNotFound );
        }

    return *settings;
    }

// -----------------------------------------------------------------------------
// CPEngContactListManager::SetAsDefaultCntListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManager::SetAsDefaultCntListL(
    const TDesC& aContactList )
    {
    // find contact list and set it as new default contact list
    MPEngContactListProperties& settings = ContactListSettingsL( aContactList );


    // is it already default contact list
    if ( settings.IsDefault() )
        {
        settings.DeletePropertyL( KPEngNewDefaultContactList,
                                  KPEngCntLstPropertyNativeCached );
        return;
        }


    // update settings
    settings.SetPropertyL( KPEngNewDefaultContactList,
                           KPEngCntLstPropertyNativeCached,
                           ETrue );

    settings.SetPropertyL( KPEngPropertiesUpdateNeeded,
                           KPEngCntLstPropertyNativeCached,
                           ETrue );

    // update settings in the current default contact list
    MPEngContactListProperties* defaultCntList = DefaultCntListSettingsOrNull();
    if ( defaultCntList )
        {
        defaultCntList->SetPropertyL( KPEngNewDefaultContactList,
                                      KPEngCntLstPropertyNativeCached,
                                      KErrCancel );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::CreateContactListL()
// -----------------------------------------------------------------------------
//
MPEngContactListProperties& CPEngContactListManager::CreateContactListL(
    const TDesC& aContactList,
    TPEngContactListBaseSettings& aBaseSettings )
    {
    // check if such a list not exist already
    if ( ContactListSettingsOrNull( aContactList ) )
        {
        User::Leave( KErrAlreadyExists );
        }


    // add new contact list settings
    CPEngContactListSettings* newList = CPEngContactListSettings::NewLC(
                                            aContactList,
                                            aBaseSettings,
                                            *this );
    if ( aBaseSettings.iContactListNameAutoUpdate )
        {
        // get user name
        TChar domainSeparator( '@' );
        TInt offset( iSessionId->UserId().LocateF( domainSeparator ) );
        TPtrC userName( KErrNotFound == offset ? iSessionId->UserId()
                        : iSessionId->UserId().Left( offset ) );

        newList->UpdateListServerNameL( userName, iUserDomain );
        }


    InsertContactListSettingsL( *newList );
    CleanupStack::Pop(); // newList

    // update array of lists and store settings
    HandleSettingsUpdateL();
    StoreSettingsL();

    return *newList;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::DeleteContactListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManager::DeleteContactListL( const TDesC& aContactList )
    {
    if ( ContactListModelOrNull( aContactList ) )
        {
        User::Leave( KErrInUse );
        }


    CPEngContactListSettings* cntList = ContactListSettingsOrNull( aContactList );
    if ( !cntList )
        {
        User::Leave( KErrNotFound );
        }


    cntList->SetPropertyL( KPEngContactListDeleteNeeded,
                           KPEngCntLstPropertyNativeCached,
                           ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::ChangeContactListTypeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManager::ChangeContactListTypeL(
    const TDesC& aContactList,
    TPEngContactListType aContactListType )
    {
    // find contact list settings
    CPEngContactListSettings* settings = ContactListSettingsOrNull( aContactList );
    if ( !settings )
        {
        User::Leave( KErrNotFound );
        }

    if ( settings->BaseSettings().iContactListType == aContactListType )
        {
        // type is same, nothing to do
        return;
        }

    // load model
    CPEngContactListModBase* model = &LoadContactListModelL( aContactList,
                                                             EFalse,
                                                             ETrue );
    // update storage type of the model
    model->ChangeStorageTypeL( CPEngContactListModBase::StoreTypeForListType( aContactListType ) );
    settings->UpdateListTypeL( aContactListType );
    }


// -----------------------------------------------------------------------------
// CPEngContactListManager::ReleaseCntListModel()
// -----------------------------------------------------------------------------
//
void CPEngContactListManager::ReleaseCntListModel( const TDesC& aContactList )
    {
    CPEngContactListModBase* model = ContactListModelOrNull( aContactList );
    if ( model )
        {
        model->Close();
        }
    }



// =============================================================================
// =============== from CPEngContactListManagerBase ============================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngContactListManager::HandleSettingsUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManager::HandleSettingsUpdateL( void )
    {
    CPEngContactListManagerBase::HandleSettingsUpdateL();


    iAllLists.Reset();
    iSynchronizedLists.Reset();


    TInt count ( iContactListSettings.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        CPEngContactListSettings* listSettings = iContactListSettings[ x ];

        iAllLists.AppendL( listSettings->Name() );

        if ( listSettings->Synchronized() )
            {
            iSynchronizedLists.AppendL( listSettings->Name() );
            }
        }


    // look for models of the contact lists which has been deleted and remove them
    count = iContactLists.Count() - 1;
    for ( ; count >= 0 ; --count )
        {
        if ( !( & ( iContactLists[ count ]->Settings() ) ) )
            {
            // intance removes itself from array in its destructor
            iContactLists[ count ]->Close();
            }
        }
    }

//  End of File














































