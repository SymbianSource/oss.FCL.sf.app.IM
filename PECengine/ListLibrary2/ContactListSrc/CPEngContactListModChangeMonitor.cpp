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
* Description:  Monitors contact list changes.
*
*/

// INCLUDE FILES
#include "PEngContactIdsTools.h"
#include "CPEngContactListModChangeMonitor.h"
#include "MPEngContactListModStore.h"
#include "PEngListLibraryPanics.h"
#include <s32strm.h>
#include <e32std.h>


// ============================= LOCAL FUNCTIONS ==============================
/**
 * Helper to grow RBuf buffer if needed.
 */
void GrowBufferIfRequiredL( RBuf& aBuffer, TInt aRequiredLength )
    {
    if ( aRequiredLength > aBuffer.MaxLength() )
        {
        aBuffer.ReAllocL( aRequiredLength );
        }

    aBuffer.Zero();
    }


//Default granurality for change monitor arrays
const TInt KChangeMonitorGranurality = 5;

//Byte size needed to stream TInt type data => 4 bytes
const TInt KIntStreamBytes = 4;




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::CPEngContactListModChangeMonitor()
// -----------------------------------------------------------------------------
//
CPEngContactListModChangeMonitor::CPEngContactListModChangeMonitor(
    MPEngContactListModStore& aStoreEntry )
        : iAddedContactIds( KChangeMonitorGranurality ),
        iRemovedContactIds( KChangeMonitorGranurality ),
        iStoreEntry( aStoreEntry )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::NewL()
// -----------------------------------------------------------------------------
//
CPEngContactListModChangeMonitor* CPEngContactListModChangeMonitor::NewL(
    MPEngContactListModStore& aStoreEntry  )
    {
    CPEngContactListModChangeMonitor* self =
        new ( ELeave ) CPEngContactListModChangeMonitor( aStoreEntry );
    return self;
    }



// Destructor
CPEngContactListModChangeMonitor::~CPEngContactListModChangeMonitor()

    {
    iAddedContactIds.Reset();
    iRemovedContactIds.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::AddedContactIds()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListModChangeMonitor::AddedContactIds() const
    {
    return iAddedContactIds;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::RemovedContactIds()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListModChangeMonitor::RemovedContactIds() const
    {
    return iRemovedContactIds;
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::CountAddedContactIds()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModChangeMonitor::CountAddedContactIds() const
    {
    return iAddedContactIds.Count();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::CountRemovedContactIds()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModChangeMonitor::CountRemovedContactIds() const
    {
    return iRemovedContactIds.Count();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::FindContactIdInAdded()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModChangeMonitor::FindContactIdInAdded(
    const TDesC& aContactId,
    const TDesC& aUserDomain ) const
    {
    return NContactIdsTools::FindContactIdInArray( iAddedContactIds,
                                                   aContactId,
                                                   aUserDomain );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::FindContactIdInRemoved()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModChangeMonitor::FindContactIdInRemoved(
    const TDesC& aContactId,
    const TDesC& aUserDomain ) const
    {
    return NContactIdsTools::FindContactIdInArray( iRemovedContactIds,
                                                   aContactId,
                                                   aUserDomain );
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModChangeMonitor::ExternalizeArrayL(
    RWriteStream& aStream ) const
    {
    // first list of added WV IDs
    TInt count( iAddedContactIds.MdcaCount() );
    aStream.WriteInt32L( count );
    for ( TInt i( 0 ); i < count; i++ )
        {
        aStream.WriteInt32L( iAddedContactIds.MdcaPoint( i ).Length() );
        aStream.WriteL( iAddedContactIds.MdcaPoint( i ) );
        }


    // now store removed WV IDs
    count = iRemovedContactIds.Count();
    aStream.WriteInt32L( count );
    for ( TInt ii( 0 ) ; ii < count ; ii++ )
        {
        aStream.WriteInt32L( iRemovedContactIds.MdcaPoint( ii ).Length() );
        aStream.WriteL( iRemovedContactIds.MdcaPoint( ii ) );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListModChangeMonitor::InternalizeArrayL(
    RReadStream& aStream )
    {
    iAddedContactIds.Reset();
    iRemovedContactIds.Reset();
    TInt& storeSize = iStoreEntry.StoreSizeCount();
    storeSize += KIntStreamBytes * 2 ; // two counts of the arrays


    RBuf buffer;
    buffer.CleanupClosePushL();


    // internalize array of Added WV IDs
    TInt count ( aStream.ReadInt32L() );
    for ( TInt x ( 0 ); x < count; x++ )
        {
        TInt size = aStream.ReadInt32L();
        GrowBufferIfRequiredL( buffer, size );
        aStream.ReadL( buffer, size );

        // array was stored in order
        iAddedContactIds.AppendL( buffer );
        storeSize += size + KIntStreamBytes; // length
        }


    // internalize array of Added WV IDs
    count = aStream.ReadInt32L();
    for ( TInt y( 0 ) ; y < count; y++ )
        {
        TInt size = aStream.ReadInt32L();
        GrowBufferIfRequiredL( buffer, size );
        aStream.ReadL( buffer, size );

        iRemovedContactIds.AppendL( buffer );
        storeSize += size + KIntStreamBytes; // length
        }


    CleanupStack::PopAndDestroy(); // buffer
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::InsertAddedContactIdL()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModChangeMonitor::InsertAddedContactIdL(
    const TDesC& aContactId )
    {
    RemoveRemovedContactId( aContactId );

    // insert WV ID into the array
    TInt position( NContactIdsTools::AddContactIdToArrayL( iAddedContactIds,
                                                           aContactId ) );

    // was WV already existing
    if ( position == KErrAlreadyExists )
        {
        return NContactIdsTools::FindContactIdInArray( iAddedContactIds,
                                                       aContactId );
        }
    else
        {
        // added successfully
        TInt& entrySize = iStoreEntry.StoreSizeCount();
        entrySize += aContactId.Size() + KIntStreamBytes; // length
        return position;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::RemoveAddedContactId()
// -----------------------------------------------------------------------------
//
void CPEngContactListModChangeMonitor::RemoveAddedContactId(
    const TDesC& aContactId )
    {
    TInt err = NContactIdsTools::RemoveContactIdFromArray( iAddedContactIds,
                                                           aContactId );
    if ( err == KErrNone )
        {
        TInt& entrySize = iStoreEntry.StoreSizeCount();
        entrySize -= aContactId.Size() - KIntStreamBytes; // length
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::ResetAddedContactId()
// -----------------------------------------------------------------------------
//
void CPEngContactListModChangeMonitor::ResetAddedContactId()
    {
    iAddedContactIds.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::InsertRemovedContactIdL()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListModChangeMonitor::InsertRemovedContactIdL(
    const TDesC& aContactId )
    {
    RemoveAddedContactId( aContactId );

    // insert WV ID into the array
    TInt position = NContactIdsTools::AddContactIdToArrayL( iRemovedContactIds,
                                                            aContactId );
    // was WV already existing
    if ( position == KErrAlreadyExists )
        {
        return NContactIdsTools::FindContactIdInArray( iRemovedContactIds,
                                                       aContactId );
        }

    else
        {
        // added successfully
        TInt& entrySize = iStoreEntry.StoreSizeCount();
        entrySize += aContactId.Size() + KIntStreamBytes; // length
        return position;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::RemoveRemovedContactId()
// -----------------------------------------------------------------------------
//
void CPEngContactListModChangeMonitor::RemoveRemovedContactId(
    const TDesC& aContactId )
    {
    TInt err = NContactIdsTools::RemoveContactIdFromArray( iRemovedContactIds,
                                                           aContactId );
    if ( err == KErrNone )
        {
        TInt& entrySize = iStoreEntry.StoreSizeCount();
        entrySize -= aContactId.Size() - KIntStreamBytes;// length
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::ResetRemovedContactIds()
// -----------------------------------------------------------------------------
//
void CPEngContactListModChangeMonitor::ResetRemovedContactIds()
    {
    iRemovedContactIds.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngContactListModChangeMonitor::Reset()
// -----------------------------------------------------------------------------
//
void CPEngContactListModChangeMonitor::Reset()
    {
    iAddedContactIds.Reset();
    iRemovedContactIds.Reset();
    }


//  End of File






















