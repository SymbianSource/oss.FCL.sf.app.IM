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
* Description:  Contact lists manager base
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "CPEngContactListManagerBase.h"
#include "PEngListLibTools.h"

#include "CPEngContactListModBase.h"
#include "CPEngWatcherList.h"
#include "CPEngContactListSettings.h"

#include "MPEngContactListAdvance.h"
#include "MPEngContactListManager.h"

#include "PEngStorageGlobals.h"
#include "MPEngStorageManager.h"
#include "PEngStorageManager.h"
#include "CPEngSessionSlotId.h"
#include "PresenceDebugPrint.h"

// Minimum store ocupation
//counts the settings count, syncFlag, domain length, each 4 bytes
static const TInt KMinimumStoreEntrySize = 12;



// ========================== HELPERS ============================================

/**
 * Abstract array entry id accessor.
 */
class MPEngEntryIdAccessor
    {
    public:

        /**
         * Gets descriptor id identifying the array item.
         * Derived implementation should cast the given
         * entry pointer to concrete type and return
         * descriptor identifying the entry.
         *
         * @param aEntry The array element.
         * @return The element descriptor id.
         */
        virtual const TDesC& DesId( const TAny* aEntry ) const = 0;
    };


/**
 * Concrete array entry id accessor to be used with
 * CPEngContactListSettings.
 */
NONSHARABLE_CLASS ( TPEngContactListSettingsIdAccessor ) : public MPEngEntryIdAccessor
    {
public:
    inline TPEngContactListSettingsIdAccessor() {}

    /**
     * Gets descriptor id identifying the
     * CPEngContactListSettings item.
     */
    inline const TDesC& DesId( const TAny* aEntry ) const
        {
        return ( ( CPEngContactListSettings* )aEntry )->Name();
        }
    };



/**
 * Concrete array entry id accessor to be used with
 * CPEngContactListModBase.
 */
NONSHARABLE_CLASS( TPEngContactListModelIdAccessor )
        : public MPEngEntryIdAccessor
    {
public:
    inline TPEngContactListModelIdAccessor() {}

    /**
     * Gets descriptor id identifying the
     * CPEngContactListModBase item.
     */
    inline const TDesC& DesId( const TAny* aEntry ) const
        {
        return ( ( CPEngContactListModBase* )aEntry )->Settings().Name();
        }
    };




/**
 * Templated find method.
 */
template<class T>
inline TInt FindEntryInArray( const RPointerArray<T>& aArray,
                              const TDesC& aContactId,
                              TInt& aIndex,
                              const MPEngEntryIdAccessor& aAccessor,
                              const TDesC& aUserDomain = KNullDesC )
    {
    TInt l( 0 );
    TInt r( aArray.Count() );
    TInt ret( KErrNotFound );
    while ( r > l )
        {
        TInt inx = ( l + r ) >> 1;
        TInt k ( NContactIdsTools::CompareContactIds( aContactId,
                                                      aAccessor.DesId( aArray[ inx ] ),
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



//Default granurality for contact lists
const TInt KListObjectsGranurality = 3;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::CPEngContactListManagerBase()
// -----------------------------------------------------------------------------
//
CPEngContactListManagerBase::CPEngContactListManagerBase(
    MPEngListLibFactory& aFactory )
        : CPEngStoreEntry( static_cast<TPEngStorageType> ( EPEngMixedPermanentCached |
                                                           EPEngStorageFlagVersionChecked ) ),
        iAccessCount( 1 ), // init ref count on 1
        iFactory( aFactory ),
        iContactListSettings( KListObjectsGranurality ),
        iContactLists( KListObjectsGranurality )
    {
    iSize = KMinimumStoreEntrySize;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::ConstructL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    iSessionId = aSessionSlot.CloneL();
    MPEngStorageManager* storageManager = NULL;
    storageManager = PEngStorageManager::GetStorageManagerL( *iSessionId );
    CleanupClosePushL( *storageManager );
    CPEngStoreEntry::BaseConstructL( *storageManager );
    CleanupStack::PopAndDestroy(); //storageManager


    InitDomainNameL( *iSessionId );


    TInt err( iStorageManager->RetrieveL( *this ) );
    if ( err == KErrNotFound )
        {
        // store does not exist, create it
        InitWatcherListL();
        StoreL();
        }
    }


// Destructor
CPEngContactListManagerBase::~CPEngContactListManagerBase()
    {
    iContactListSettings.ResetAndDestroy();
    for ( TInt x( iContactLists.Count() - 1 ) ; x >= 0 ; --x )
        {
        iContactLists[ x ]->Close();
        }

    iContactLists.Reset();
    iCntLstSettingsObservers.Reset();
    delete iSessionId;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::CreateEmptyAttributeListL()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngContactListManagerBase::SessionId( ) const
    {
    return *iSessionId;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::Open()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::Open()
    {
    iAccessCount++;
    }



// =============================================================================
// =============== Functions from MPEngContactListSettingsManager ==============
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::ContactListSettingsOrNull()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings* CPEngContactListManagerBase::ContactListSettingsOrNull(
    const TDesC& aContactList )
    {
    TInt x ( FindContactListSettings( aContactList ) );
    if ( x == KErrNotFound )
        {
        return NULL;
        }

    return iContactListSettings[ x ];
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::AddContactListSettingsObserverL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::AddContactListSettingsObserverL(
    const MPEngContactListSettingsObserver* aObserver )
    {
    // add observer
    User::LeaveIfError( iCntLstSettingsObservers.InsertInAddressOrder( aObserver ) );
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::RemoveContactListSettingsObserver()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::RemoveContactListSettingsObserver(
    const MPEngContactListSettingsObserver* aObserver )
    {
    // remove observer
    TInt index ( iCntLstSettingsObservers.FindInAddressOrder( aObserver ) );
    if ( index != KErrNotFound )
        {
        iCntLstSettingsObservers.Remove( index );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::RemoveModel()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::RemoveModel( CPEngContactListModBase* aModel )
    {
    TInt index( iContactLists.Find( aModel ) );
    if ( index != KErrNotFound )
        {
        iContactLists.Remove( index );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::UserDomain()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListManagerBase::UserDomain() const
    {
    return iUserDomain;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::StoreSettingsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::StoreSettingsL()
    {
    StoreL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::StoreSize()
// -----------------------------------------------------------------------------
//
TInt& CPEngContactListManagerBase::StoreSize( )
    {
    return iSize;
    }



// =============================================================================
// ==================== Functions from CPresenceStoreEntry =====================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngContactListMainArray::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::ExternalizeL( RWriteStream& aStream,
                                                TPEngStorageType aStorageType ) const
    {
    switch ( aStorageType )
        {
        case EPEngStorageBasicPermanent:
            {
            TInt count ( iContactListSettings.Count() );
            aStream.WriteInt32L( count );
            for ( TInt x ( 0 ) ; x < count ; x++ )
                {
                iContactListSettings[x]->ExternalizeL( aStream,
                                                       aStorageType );
                }
            break;
            }


        case EPEngStorageBasicCached:
            {
            aStream.WriteInt32L( iEnviromentSynchronized );

            TInt count ( iContactListSettings.Count() );
            for ( TInt x ( 0 ) ; x < count ; x++ )
                {
                iContactListSettings[x]->ExternalizeL( aStream,
                                                       aStorageType );
                }
            break;
            }


        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::InternalizeL(
    RReadStream& aStream,
    TPEngStorageType aStorageType )
    {
    switch ( aStorageType )
        {
        case EPEngStorageBasicPermanent:
            {
            iContactListSettings.ResetAndDestroy();
            iSize = KMinimumStoreEntrySize;

            TInt count ( aStream.ReadInt32L() );
            for ( TInt x ( 0 ) ; x < count ; x++ )
                {
                CPEngContactListSettings* newSettings =
                    CPEngContactListSettings::NewStreamLC( aStream,
                                                           *this );
                iContactListSettings.AppendL( newSettings );
                CleanupStack::Pop(); // newSettings
                }

            iEnviromentSynchronized = EFalse;
            break;
            }


        case EPEngStorageBasicCached:
            {
            iEnviromentSynchronized = aStream.ReadInt32L();

            TInt count ( iContactListSettings.Count() );
            for ( TInt x( 0 ) ; x < count ; ++x )
                {
                iContactListSettings[ x ]->InternalizeL( aStream,
                                                         aStorageType );
                }
            break;
            }
        default:
            {

            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::StorageId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListManagerBase::StorageId() const
    {
    return KPEngContactListManagerSId;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::EntrySize()
// -----------------------------------------------------------------------------
//
TUint32 CPEngContactListManagerBase::EntrySize() const
    {
    return iSize;
    }




// =============================================================================
// ==================== Functions from MPEngSIDChangeObserver ==================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::HandleSIDsChangeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::HandleSIDsChangeL(
    CPtrCArray& /* aChangedSIDs */ )
    {
    TInt err( iStorageManager->RetrieveL( *this ) );
    if ( err == KErrNotFound )
        {
        InitWatcherListL();
        StoreL();
        }

    NotifyCntLstSettingsObservers();
    HandleSettingsUpdateL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::HandleSIDNotifyError()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::HandleSIDNotifyError( TInt /* aError */ )
    {
    NotifyCntLstSettingsObservers();
    }


// =============================================================================
// =============== protected functions for derived classes =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::FindContactListSettings()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListManagerBase::FindContactListSettings(
    const TDesC& aContactList ) const
    {
    TInt index( KErrNotFound );
    TPEngContactListSettingsIdAccessor settingsOrder;

    if ( KErrNone == FindEntryInArray( iContactListSettings,
                                       aContactList,
                                       index,
                                       settingsOrder,
                                       iUserDomain ) )
        {
        return index;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::InsertContactListSettingsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::InsertContactListSettingsL(
    CPEngContactListSettings& aSettings )
    {
    TInt index( KErrNotFound );
    TPEngContactListSettingsIdAccessor settingsOrder;

    if ( KErrNone == FindEntryInArray( iContactListSettings,
                                       aSettings.Name(),
                                       index,
                                       settingsOrder,
                                       iUserDomain ) )
        {
        delete iContactListSettings[ index ];
        iContactListSettings[ index ] = &aSettings;
        }

    else
        {
        iContactListSettings.InsertL( &aSettings, index );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::DeleteContactListSettingsL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::DeleteContactListSettingsL(
    const TDesC& aContactList )
    {
    TInt index( KErrNotFound );
    TPEngContactListSettingsIdAccessor settingsOrder;
    if ( KErrNone == FindEntryInArray( iContactListSettings,
                                       aContactList,
                                       index,
                                       settingsOrder,
                                       iUserDomain ) )
        {
        // delete contact list from store
        iStorageManager->Delete( aContactList );

        // delete contact list from the settings array
        delete iContactListSettings[ index ];
        iContactListSettings.Remove( index );

        //notify observers
        NotifyCntLstSettingsObservers();
        StoreL();
        }
    }

// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::DefaultCntListSettingsOrNull()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings* CPEngContactListManagerBase::DefaultCntListSettingsOrNull()
    {
    TInt count( iContactListSettings.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        CPEngContactListSettings* settings = iContactListSettings[ x ];
        if ( settings->IsDefault() )
            {
            return settings;
            }
        }

    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::HandleSettingsUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::HandleSettingsUpdateL()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::LoadContactListModelL()
// -----------------------------------------------------------------------------
//
CPEngContactListModBase& CPEngContactListManagerBase::LoadContactListModelL(
    const TDesC& aContactList,
    TBool aReferenceUpdate,
    TBool aAnyTime )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListManagerBase::LoadContactListModelL() [%S]" ),
             &aContactList );

    CPEngContactListModBase* model = ContactListModelOrNull( aContactList  );
    if ( model )
        {
        if ( aReferenceUpdate )
            {
            model->Open();              // CSI: 65 #
            }

        return *model;
        }


    // check if requested list is watcher list
    if ( KErrNone == aContactList.CompareF( KPEngWatcherList ) )
        {
        // Create watcher list and append it to array
        TInt x( FindContactListSettings( KPEngWatcherList ) );
        CPEngWatcherList* newList = CPEngWatcherList::NewLC(
                                        *iContactListSettings[ x ],
                                        *iStorageManager, *this );

        InsertContactListModelL( *newList );
        CleanupStack::Pop(); // newList
        return *newList;
        }


    // try to find settings for contact list to load it
    CPEngContactListSettings* settings = ContactListSettingsOrNull( aContactList );
    if ( settings )
        {
        if ( !aAnyTime
             &&
             !settings->Synchronized()
             &&
             !settings->Property( KPEngCntLstSyncMaster,
                                  KPEngCntLstPropertyNativePermanent ) )
            {
            User::Leave( KErrNotReady );
            }

        // create list and append it to array
        CPEngContactListModBase* newList = CPEngContactListModBase::NewLC(
                                               *settings,
                                               *iStorageManager,
                                               *this );

        InsertContactListModelL( *newList );
        CleanupStack::Pop(); // newList
        return *newList;
        }

    User::Leave( KErrNotFound );
    return *( iContactLists[0] );  //To keep compiler happy
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::ContactListModelOrNull()
// -----------------------------------------------------------------------------
//
CPEngContactListModBase* CPEngContactListManagerBase::ContactListModelOrNull(
    const TDesC& aContactList )
    {
    TInt index( KErrNotFound );
    TPEngContactListModelIdAccessor listModelOrder;
    if ( KErrNone == FindEntryInArray( iContactLists,
                                       aContactList,
                                       index,
                                       listModelOrder,
                                       iUserDomain ) )
        {
        return iContactLists[ index ];
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::InsertContactListModelL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::InsertContactListModelL(
    CPEngContactListModBase& aModel )
    {
    TInt index( KErrNotFound );
    TPEngContactListModelIdAccessor listModelOrder;
    if ( KErrNone == FindEntryInArray( iContactLists,
                                       aModel.Settings().Name(),
                                       index,
                                       listModelOrder,
                                       iUserDomain ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    iContactLists.InsertL( &aModel, index );
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::NotifyCntLstSettingsObservers()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::NotifyCntLstSettingsObservers( )
    {
    for ( TInt x ( iCntLstSettingsObservers.Count() - 1 ) ; x >= 0 ; x-- )
        {
        iCntLstSettingsObservers[ x ]->RefreshSettingsReference();
        }
    }


// =============================================================================
// =============== New private functions =======================================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::InitDomainNameL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::InitDomainNameL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    TChar domainSeparator( '@' );
    TInt offset( aSessionSlot.UserId().LocateF( domainSeparator ) );
    if ( KErrNotFound == offset )
        {
        iUserDomain.Set( KNullDesC );
        }
    else
        {
        // copy first part till '@'
        iUserDomain.Set( aSessionSlot.UserId().Mid( offset ) );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListManagerBase::InitDomainNameL()
// -----------------------------------------------------------------------------
//
void CPEngContactListManagerBase::InitWatcherListL()
    {
    TPEngContactListBaseSettings baseSettings;
    baseSettings.iContactListNameAutoUpdate = EFalse;
    baseSettings.iContactListType = EPEngCachedContactList;

    CPEngContactListSettings* watcherSettings =
        CPEngContactListSettings::NewLC( KPEngWatcherList,
                                         baseSettings,
                                         *this );
    watcherSettings->SetDisplayNameL( KPEngWatcherList );

    // reset list, we start from beginning
    iContactListSettings.Reset();
    iContactListSettings.AppendL( watcherSettings );

    CleanupStack::Pop(); // watcherSettings
    }


//  End of File











