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
* Description:  Presence attribute store proxy.
*
*/

//  INCLUDES
#include "CPEngPresenceAttrModelStorageProxy.h"
#include "CPEngPresenceAttrModel.h"
#include "CPEngModelStoreEntryAdapter.h"
#include "MPEngStorageManager.h"
#include "PEngAttributeStoreIDCodec.h"
#include "PEngWVPresenceAttributes2.h"
#include <E32Base.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrModelStorageProxy* CPEngPresenceAttrModelStorageProxy::NewL(
    CPEngPresenceAttrModel& aAttributeModel )
    {
    CPEngPresenceAttrModelStorageProxy* self = new ( ELeave ) CPEngPresenceAttrModelStorageProxy(
        aAttributeModel );
    return self;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::ConnectStorageL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModelStorageProxy::ConnectStorageL( TBool aUserModel,
                                                          HBufC* aStoreId,
                                                          MPEngStorageManager& aStorageManager )

    {
    __ASSERT_ALWAYS( !iStoreEntry, User::Leave( KErrAlreadyExists ) );


    TPEngStorageType storageType = EPEngStorageBasicCached;
    if ( aUserModel )
        {
        storageType = EPEngStorageBasicPermanent;
        }


    iStoreEntry = CPEngModelStoreEntryAdapter::NewL( aStoreId,
                                                     aStorageManager,
                                                     storageType,
                                                     iAttributeModel );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::CPEngPresenceAttrModelStorageProxy()
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrModelStorageProxy::CPEngPresenceAttrModelStorageProxy(
    CPEngPresenceAttrModel& aAttributeModel )
        : iAttributeModel( aAttributeModel ),
        iEditLockOwned( EFalse ),
        iForceNotifySidChange( ETrue )
    {
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::~CPEngPresenceAttrModelStorageProxy()
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrModelStorageProxy::~CPEngPresenceAttrModelStorageProxy()
    {
    StopEditSupport();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::RetrieveFromStorageL()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrModelStorageProxy::RetrieveFromStorageL()
    {
    //KErrArgument is used here because client is trying
    //perform storage operation for wrong kind of attribute model
    __ASSERT_ALWAYS( iStoreEntry, User::Leave( KErrArgument ) );

    return iStoreEntry->StorageManager().RetrieveL( *iStoreEntry );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::StoreToStorageL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModelStorageProxy::StoreToStorageL( TBool aForceStore ) const
    {
    __ASSERT_ALWAYS( iStoreEntry, User::Leave( KErrArgument ) );

    MPEngStorageManager& sm = iStoreEntry->StorageManager();
    if ( aForceStore )
        {
        //Updating the force notify flag from const storage method
        //is ok, because the value is internal storage advice,
        //and thus the data isn't directly visible to attribute clients
        CPEngPresenceAttrModelStorageProxy* nonConstSelf =
            const_cast<CPEngPresenceAttrModelStorageProxy*> ( this );

        //Force notification from SID change is sent if the storing fails
        //If storing succeds, no need to send SID change event "manually"
        nonConstSelf->iForceNotifySidChange = ETrue;
        sm.StoreExclusiveL( *iStoreEntry );
        nonConstSelf->iForceNotifySidChange = EFalse;
        }
    else
        {
        sm.StoreL( *iStoreEntry );
        }

    ForceSendAttrEventIfNeededL();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::RemoveFromStorageL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModelStorageProxy::RemoveFromStorageL()
    {
    __ASSERT_ALWAYS( iStoreEntry, User::Leave( KErrArgument ) );

    User::LeaveIfError( iStoreEntry->StorageManager().Delete( *iStoreEntry ) );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::IsFromThisStorage()
// -----------------------------------------------------------------------------
//
TBool CPEngPresenceAttrModelStorageProxy::IsFromThisStorage(
    const MPEngStorageManager& aStorageManager ) const
    {
    if ( iStoreEntry &&
         ( &iStoreEntry->StorageManager() == &aStorageManager ) )
        {
        return ETrue;
        }


    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::GainEditLockL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModelStorageProxy::GainEditLockL( TBool aCreationAllowed )
    {
    __ASSERT_ALWAYS( iStoreEntry, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iEditLockOwned, User::Leave( KErrLocked ) );

    if ( iStoreEntry->StorageManager().LockedL( *iStoreEntry ) )
        {
        User::Leave( KErrLocked );
        }

    User::LeaveIfError( iStoreEntry->Lock( EStorageLockLevelBasic,
                                           aCreationAllowed ) );
    iEditLockOwned = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::StopEditSupport()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrModelStorageProxy::StopEditSupport()
    {
    TInt err = KErrNone;

    if ( iStoreEntry && iEditLockOwned )
        {
        iStoreEntry->Release();
        iEditLockOwned = EFalse;

        TRAP( err, ForceSendAttrEventIfNeededL() );
        }

    delete iStoreEntry;
    iStoreEntry = NULL;

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::IsEditLocked()
// -----------------------------------------------------------------------------
//
TBool CPEngPresenceAttrModelStorageProxy::IsEditLocked() const
    {
    return iEditLockOwned;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModelStorageProxy::ForceSendAttrEventIfNeededL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModelStorageProxy::ForceSendAttrEventIfNeededL() const
    {
    if ( iForceNotifySidChange &&
         ( iAttributeModel.Type() != KUidPrAttrOnlineStatus ) )
        {
        HBufC* sid = PEngAttributeStoreIDCodec::GenerateStoreIdL( KUidPrAttrOnlineStatus,
                                                                  iAttributeModel.PresenceID() );
        CleanupStack::PushL( sid );
        MPEngStorageManager& m = iStoreEntry->StorageManager();
        User::LeaveIfError( m.NotifyChangedSId( *sid, iStoreEntry->StorageType() ) );
        CleanupStack::PopAndDestroy( sid );
        }
    }


// End of File
