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
* Description:  Container for contact list settings.
*
*/


// INCLUDE FILES
#include    "CPEngContactListSettings.h"
#include    "MPEngAttributeList2.h"
#include    "PEngContactIdsTools.h"
#include    "PEngStorageGlobals.h"
#include    "MPEngContactListSettingsManager.h"
#include    "CPEngContactListProperty.h"

#include    <e32std.h>
#include    <s32strm.h>


// Minimal size of the store entry for contact list settings
// 7 x 4 bytes (32bit) to store
static const TInt KListSettingsStoreMinSize = 7 * 4;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListSettings::CPEngContactListSettings()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings::CPEngContactListSettings(
    const TPEngContactListBaseSettings& aBaseSettings,
    MPEngContactListSettingsManager& aManager )
        : iSettingsManager( aManager ),
        iBehavior ( aBaseSettings )
    {
    }

// -----------------------------------------------------------------------------
// CPEngContactListSettings::CPEngContactListSettings()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings::CPEngContactListSettings(
    MPEngContactListSettingsManager& aManager )
        : iSettingsManager( aManager )
    {
    }

// -----------------------------------------------------------------------------
// CPEngContactListSettings::ConstructServerL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::ConstructServerL( const TDesC& aContactList,
                                                 const TDesC& aServerName )
    {
    iName = aContactList.AllocL();
    iServerName = aServerName.AllocL();
    iDisplayName = KNullDesC().AllocL();

    iSettingsManager.StoreSize() += aContactList.Length() +
                                    aServerName.Length() + KListSettingsStoreMinSize;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::ConstructL( const TDesC& aContactList )
    {
    iName = aContactList.AllocL();
    iName->Des().Fold();
    iDisplayName = KNullDesC().AllocL();

    iSettingsManager.StoreSize() += aContactList.Length() + KListSettingsStoreMinSize;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings* CPEngContactListSettings::NewLC(
    const TDesC& aContactList,
    const TPEngContactListBaseSettings& aBaseSettings,
    MPEngContactListSettingsManager& aManager )
    {
    CPEngContactListSettings* self = new( ELeave )
    CPEngContactListSettings( aBaseSettings,
                              aManager );

    CleanupStack::PushL( self );
    self->ConstructL( aContactList );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::NewServerLC()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings* CPEngContactListSettings::NewServerLC(
    const TDesC& aContactList,
    const TDesC& aServerName,
    MPEngContactListSettingsManager& aManager )
    {
    CPEngContactListSettings* self = new( ELeave ) CPEngContactListSettings(
        aManager );

    CleanupStack::PushL( self );
    self->ConstructServerL( aContactList, aServerName );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::NewServerLC()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings* CPEngContactListSettings::NewStreamLC(
    RReadStream& aStream,
    MPEngContactListSettingsManager& aManager )
    {
    CPEngContactListSettings* self = new( ELeave ) CPEngContactListSettings( aManager );

    CleanupStack::PushL( self );
    self->InternalizeL( aStream, EPEngStorageBasicPermanent );

    return self;
    }


// Destructor
CPEngContactListSettings::~CPEngContactListSettings()
    {
    delete iName;
    delete iServerName;
    delete iDisplayName;
    iProperties.ResetAndDestroy();
    iSubscriptionAttrs.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::Synchronized()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListSettings::Synchronized() const
    {
    return Property( KPEngHasBeenSynchronized,
                     KPEngCntLstPropertyNativeCached );
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::ListName()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListSettings::Name() const
    {
    return *iName;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::ServerName()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListSettings::ServerName() const
    {
    if ( ( iServerName ) && ( iServerName->Compare( KNullDesC ) != KErrNone ) )
        {
        return *iServerName;
        }
    // no server name, then it same as local one
    return *iName;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::DisplayName()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListSettings::DisplayName( TPtrC& aDispName ) const
    {
    if ( Synchronized()  ||
         Property( KPEngCntLstSyncMaster, KPEngCntLstPropertyNativePermanent ) )
        {
        aDispName.Set( *iDisplayName );
        return KErrNone;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::SetDisplayNameL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::SetDisplayNameL( const TDesC& aDisplayName )
    {
    if ( KErrNone == iDisplayName->Compare( aDisplayName ) )
        {
        // nothing to do
        return;
        }

    // update display name
    HBufC* temp = aDisplayName.AllocL();
    TInt dif( iDisplayName->Length() - aDisplayName.Length() );
    delete iDisplayName;
    iDisplayName = temp;
    iSettingsManager.StoreSize() += dif;

    // set update needed flag, stores data internally
    SetPropertyL( KPEngPropertiesUpdateNeeded,
                  KPEngCntLstPropertyNativeCached,
                  ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::IsDefault()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListSettings::IsDefault() const
    {
    TInt x( PEngPropertyManager::FindProperty( iProperties,
                                               KPEngCntLstDefault,
                                               KPEngCntLstPropertyNativePermanent ) );
    if ( x == KErrNotFound )
        {
        return EFalse;
        }

    return iProperties[ x ]->ValueInt();
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::SubscriptionAttributes()
// -----------------------------------------------------------------------------
//
const RArray<TUint32>& CPEngContactListSettings::SubscriptionAttributes() const
    {
    return iSubscriptionAttrs;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::SetSubscriptionAttributesL()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListSettings::SetSubscriptionAttributesL(
    const MPEngAttributeList2& aAttributelist )
    {
    TArray<TUint32> attrs( aAttributelist.ListOfAttributes() );
    return SetSubscriptionAttributesL( attrs );
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::SetSubscriptionAttributesL()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListSettings::SetSubscriptionAttributesL(
    const TArray<TUint32>& aAttributelist )
    {
    // test if arrays are same
    TInt count( aAttributelist.Count() );
    TBool same( iSubscriptionAttrs.Count() == count );
    if ( same )
        {
        for ( TInt x( 0 ) ; x < count ; ++x )
            {
            if ( KErrNotFound == iSubscriptionAttrs.Find( aAttributelist[ x ] ) )
                {
                same = EFalse;
                }
            }
        }


    // is update needed?
    if ( same &&
         Property( KPEngCntLstSubscribe, KPEngCntLstPropertyNativePermanent ) )
        {
        return EFalse;
        }


    // 4 bytes per one attribute
    TInt dif( ( iSubscriptionAttrs.Count() - count ) * 4 );
    iSubscriptionAttrs.Reset();

    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iSubscriptionAttrs.AppendL( aAttributelist[ x ] );
        }
    iSettingsManager.StoreSize() += dif;


    // set Subscription property On
    ActivateSubscriptionL();
    SetPropertyL( KPEngSubcriptionUpdate,
                  KPEngCntLstPropertyNativeCached,
                  ETrue );

    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::ActivateSubscriptionL()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListSettings::ActivateSubscriptionL()
    {

    if ( Property( KPEngCntLstSubscribe, KPEngCntLstPropertyNativePermanent ) )
        {
        return EFalse;
        }

    // set Subscription property On
    SetPropertyL( KPEngCntLstSubscribe,
                  KPEngCntLstPropertyNativePermanent,
                  ETrue );

    SetPropertyL( KPEngSubcriptionUpdate,
                  KPEngCntLstPropertyNativeCached,
                  ETrue );
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::DeActivateSubscriptionL()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListSettings::DeActivateSubscriptionL()
    {
    if ( ! Property( KPEngCntLstSubscribe, KPEngCntLstPropertyNativePermanent ) )
        {
        return EFalse;
        }


    // set Subscription property Off
    DeletePropertyL( KPEngCntLstSubscribe,
                     KPEngCntLstPropertyNativePermanent );

    SetPropertyL( KPEngSubcriptionUpdate,
                  KPEngCntLstPropertyNativeCached,
                  ETrue );

    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::BaseSettings()
// -----------------------------------------------------------------------------
//
const TPEngContactListBaseSettings& CPEngContactListSettings::BaseSettings() const
    {
    return iBehavior;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListSettings::GetProperty( TUint aName,
                                            TUint aLevel,
                                            TInt& aValue ) const

    {
    return PEngPropertyManager::GetProperty( iProperties,
                                             aName,
                                             aLevel,
                                             aValue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListSettings::GetProperty( TUint aName,
                                            TUint aLevel,
                                            TPtrC8& aValue ) const
    {
    return PEngPropertyManager::GetProperty( iProperties,
                                             aName,
                                             aLevel,
                                             aValue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListSettings::GetProperty( TUint aName,
                                            TUint aLevel,
                                            TPtrC16& aValue ) const
    {
    return PEngPropertyManager::GetProperty( iProperties,
                                             aName,
                                             aLevel,
                                             aValue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::Property()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListSettings::Property( TUint aName,
                                         TUint aLevel ) const
    {
    return PEngPropertyManager::PropertyOrZero( iProperties,
                                                aName,
                                                aLevel );
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::SetPropertyL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::SetPropertyL( TUint aName,
                                             TUint aLevel,
                                             TInt aValue )
    {
    PEngPropertyManager::SetPropertyL( iProperties,
                                       aName,
                                       aLevel,
                                       aValue,
                                       iSettingsManager.StoreSize() );
    iSettingsManager.StoreSettingsL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::SetPropertyL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::SetPropertyL( TUint aName,
                                             TUint aLevel,
                                             const TDesC8& aValue )
    {
    PEngPropertyManager::SetPropertyL( iProperties,
                                       aName,
                                       aLevel,
                                       aValue,
                                       iSettingsManager.StoreSize() );
    iSettingsManager.StoreSettingsL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::SetPropertyL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::SetPropertyL( TUint aName,
                                             TUint aLevel,
                                             const TDesC16& aValue )
    {
    PEngPropertyManager::SetPropertyL( iProperties,
                                       aName,
                                       aLevel,
                                       aValue,
                                       iSettingsManager.StoreSize() );
    iSettingsManager.StoreSettingsL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::DeletePropertyL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::DeletePropertyL( TUint aName,
                                                TUint aLevel )
    {
    if ( PEngPropertyManager::DeletePropertyL( iProperties,
                                               aName,
                                               aLevel,
                                               iSettingsManager.StoreSize() ) )
        {
        iSettingsManager.StoreSettingsL();
        }
    }



// =============================================================================
// ===============Functions of main class ======================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListSettings::UpdateListServerNameL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::UpdateListServerNameL(
    const TDesC& aUserName,
    const TDesC& aDomain )
    {
    // allocate buffer as big as we need
    HBufC* newSrvNameBuf;
    newSrvNameBuf = HBufC::NewLC( aUserName.Length() +
                                  iName->Length() +
                                  aDomain.Length() +
                                  KPEngWVResourceSeparatorLength );
    TPtr newSrvName( newSrvNameBuf->Des() );
    newSrvName.Append( aUserName );

    // check if first character of the contact list name is '/'
    if ( KErrNone != iName->Left( 1 ).CompareF( KPEngWVResourceSeparator ) )
        {
        newSrvName.Append( KPEngWVResourceSeparator );
        }

    newSrvName.Append( *iName );
    newSrvName.Append( aDomain );

    delete iServerName;
    iServerName = newSrvNameBuf;
    CleanupStack::Pop(); //newSrvNameBuf
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::DispName()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListSettings::DispName() const
    {
    return *iDisplayName;
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::PushUpdateDisplayNameL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::PushUpdateDisplayNameL( HBufC* aDispName )   // CSI: 60 #
    {
    if ( KErrNone == iDisplayName->Compare( *aDispName ) )
        {
        delete aDispName;
        return;
        }

    // update display name
    iSettingsManager.StoreSize() += ( iDisplayName->Length() -
                                      aDispName->Length() );
    delete iDisplayName;
    iDisplayName = aDispName;
    iSettingsManager.StoreSettingsL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::UpdateListTypeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::UpdateListTypeL(
    TPEngContactListType aContactListType )
    {
    iBehavior.iContactListType = aContactListType;
    iSettingsManager.StoreSettingsL();
    }



// -----------------------------------------------------------------------------
// CPEngContactListSettings::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::ExternalizeL( RWriteStream& aStream,
                                             TInt aStorageType ) const
    {
    switch ( aStorageType )
        {
        case EPEngStorageBasicPermanent:
            {
            // Store permanent part

            aStream.WriteInt32L( iName->Length() );
            aStream << *iName;
            aStream.WriteInt32L( iDisplayName->Length() );
            aStream << *iDisplayName;

            if ( iServerName )
                {
                aStream.WriteInt32L( iServerName->Length() );
                aStream << *iServerName;
                }
            else
                {
                aStream.WriteInt32L( 0 ); // no server name -zero
                }

            aStream.WriteInt32L( iBehavior.iContactListType );
            aStream.WriteInt32L( iBehavior.iContactListNameAutoUpdate );


            // subscription attributes
            TInt attrCount( iSubscriptionAttrs.Count() );
            aStream.WriteInt32L( attrCount );
            for ( TInt x ( 0 ) ; x < attrCount ; ++x )
                {
                aStream.WriteInt32L( iSubscriptionAttrs[ x ] );
                }


            // Detailed properties
            PEngPropertyManager::ExternalizePropertiesL( iProperties,
                                                         aStream,
                                                         aStorageType );
            break;
            };


        case EPEngStorageBasicCached:
            {
            // store cached part
            // Detailed properties
            PEngPropertyManager::ExternalizePropertiesL( iProperties,
                                                         aStream,
                                                         aStorageType );
            break;
            }


        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListSettings::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListSettings::InternalizeL( RReadStream& aStream,
                                             TInt aStorageType )
    {
    // write asked part
    switch ( aStorageType )
        {
        case EPEngStorageBasicPermanent:
            {
            // reset property array
            iProperties.ResetAndDestroy();

            // read permanent part
            delete iName;
            iName = NULL;
            TInt length ( aStream.ReadInt32L() );
            iName = HBufC::NewL( aStream, length );
            iSettingsManager.StoreSize() += length + 4; // 4 as TInt size


            delete iDisplayName;
            iDisplayName = NULL;
            length = aStream.ReadInt32L();
            iDisplayName = HBufC::NewL( aStream, length );
            iSettingsManager.StoreSize() += length + 4; // 4 as TInt size


            // read server name only if length is greater than 0
            delete iServerName;
            iServerName = NULL;
            length = aStream.ReadInt32L();
            if ( length > 0 )
                {
                iServerName = HBufC::NewL( aStream, length );
                iSettingsManager.StoreSize() += length + 4; // 4 as TInt size
                }
            else
                {
                iSettingsManager.StoreSize() += 4; // 4 as TInt size
                }


            iBehavior.iContactListType = static_cast<TPEngContactListType>
                                         ( aStream.ReadInt32L() );

            iBehavior.iContactListNameAutoUpdate = aStream.ReadInt32L();


            // subscription attributes
            TInt count( aStream.ReadInt32L() );
            iSubscriptionAttrs.Reset();
            for ( TInt x ( 0 ) ; x < count ; ++x )
                {
                iSubscriptionAttrs.AppendL( aStream.ReadInt32L() );
                }

            // type(4), auto update(4), count(4), each attribute 4x
            // first properties mark(4)
            iSettingsManager.StoreSize() += ( 16 + 4 * count );

            // Detailed properties
            PEngPropertyManager::InternalizePropertiesL( iProperties,
                                                         aStream,
                                                         iSettingsManager.StoreSize() );
            break;
            };


        case EPEngStorageBasicCached:
            {
            // read cached part
            // 4 for first property mark

            iSettingsManager.StoreSize() += 4;
            // Detailed properties
            PEngPropertyManager::InternalizePropertiesL( iProperties,
                                                         aStream,
                                                         iSettingsManager.StoreSize() );
            break;
            }


        default:
            {
            break;
            }
        }
    }

//  End of File
