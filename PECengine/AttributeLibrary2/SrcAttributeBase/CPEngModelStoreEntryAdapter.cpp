/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Store entry adapter for presence attributes.
*
*/

//  INCLUDES
#include "CPEngModelStoreEntryAdapter.h"
#include "CPEngPresenceAttrModel.h"
#include <E32Base.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngModelStoreEntryAdapter::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngModelStoreEntryAdapter* CPEngModelStoreEntryAdapter::NewL(
    HBufC* aStoreId,
    MPEngStorageManager& aStorageManager,
    TPEngStorageType aStorageType,
    CPEngPresenceAttrModel& aAttributeModel )
    {
    CPEngModelStoreEntryAdapter* self =
        new CPEngModelStoreEntryAdapter( aStorageType,      // CSI: 62 #
                                         aStoreId,
                                         aAttributeModel );

    if ( !self )
        {
        delete aStoreId;
        User::Leave( KErrNoMemory );
        }

    CleanupStack::PushL( self );
    self->BaseConstructL( aStorageManager, EFalse );
    CleanupStack::Pop(); //self

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngModelStoreEntryAdapter::CPEngModelStoreEntryAdapter()
// -----------------------------------------------------------------------------
//
CPEngModelStoreEntryAdapter::CPEngModelStoreEntryAdapter(
    TPEngStorageType aStorageType,
    HBufC* aStoreId,
    CPEngPresenceAttrModel& aAttributeModel )
        : CPEngStoreEntry( aStorageType ),
        iStoreId( aStoreId ),
        iAttributeModel( aAttributeModel )
    {
    }


// -----------------------------------------------------------------------------
// CPEngModelStoreEntryAdapter::~CPEngModelStoreEntryAdapter()
// -----------------------------------------------------------------------------
//
CPEngModelStoreEntryAdapter::~CPEngModelStoreEntryAdapter()
    {
    delete iStoreId;
    }


// -----------------------------------------------------------------------------
// CPEngModelStoreEntryAdapter::StorageManager()
// -----------------------------------------------------------------------------
//
MPEngStorageManager& CPEngModelStoreEntryAdapter::StorageManager()
    {
    return *iStorageManager;
    }

const MPEngStorageManager& CPEngModelStoreEntryAdapter::StorageManager() const
    {
    return *iStorageManager;
    }






// -----------------------------------------------------------------------------
// CPEngModelStoreEntryAdapter::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngModelStoreEntryAdapter::ExternalizeL(
    RWriteStream& aStream,
    TPEngStorageType /*aStorageType*/ ) const
    {
    iAttributeModel.ExternalizeL( aStream );
    }


// -----------------------------------------------------------------------------
// CPEngModelStoreEntryAdapter::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngModelStoreEntryAdapter::InternalizeL(
    RReadStream& aStream,
    TPEngStorageType /*aStorageType*/ )
    {
    iAttributeModel.InternalizeL( aStream );
    }


// -----------------------------------------------------------------------------
// CPEngModelStoreEntryAdapter::StorageId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngModelStoreEntryAdapter::StorageId() const
    {
    return *iStoreId;
    }

// -----------------------------------------------------------------------------
// CPEngModelStoreEntryAdapter::EntrySize()
// -----------------------------------------------------------------------------
//
TUint32 CPEngModelStoreEntryAdapter::EntrySize() const
    {
    return iAttributeModel.ExternalizeSize();
    }



// End of File
