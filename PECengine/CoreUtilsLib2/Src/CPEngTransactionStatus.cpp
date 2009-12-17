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
* Description:  Transaction status implementation.
*
*/

// INCLUDE FILES
#include "CPEngDetailedResultEntry.h"
#include "CPEngDataResultEntry.h"
#include "CPEngTransactionStatus.h"
#include <S32Strm.h>
#include <s32mem.h>


//DATA TYPES

/**
 * Transaction status panic reasons.
 * @since 3.0
 */
enum TPEngTransactionStatusPanicReasons
    {
    EPEngDetailedAccessOutOfBounds = 1,
    EPEngNoCriteriaForNextDetailByOperation = 2,
    EPEngNoCriteriaForNextDataByOperation = 3
    };


/**
 * Transaction status panic category.
 * @since 3.0
 */
_LIT( KPEngTransStatPanicCategory, "PEngTransStat" );




// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPEngTransactionStatus::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngTransactionStatus* CPEngTransactionStatus::NewL()
    {
    CPEngTransactionStatus* self = new ( ELeave ) CPEngTransactionStatus();
    return self;
    }


// Destructor
CPEngTransactionStatus::~CPEngTransactionStatus()
    {
    Reset();

#if _BullseyeCoverage
    cov_write();
#endif
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::CPEngTransactionStatus
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus::CPEngTransactionStatus()
        : iDetailedResultsQ( _FOFF( CPEngDetailedResultEntry, iLink ) ), // CSI: 33 #
        iDetailedResultsQIter( iDetailedResultsQ ),
        iDetailedResultsCount( 0 ),
        iDetResCurrentOperationId( KErrUnknown ),
        iDetResCurrentIndex( 0 ),
        iDataResultsQ( _FOFF( CPEngDataResultEntry, iLink ) ),         // CSI: 33 #
        iDataResultsQIter( iDataResultsQ ),
        iDataResultsCount( 0 ),
        iDataResCurrentOperationId( KErrUnknown ),
        iDataResCurrentIndex( 0 )
    {
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::Status()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::Status() const
    {
    return iStatusCode;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::DetailedResultCount()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::DetailedResultCount() const
    {
    return iDetailedResultsCount;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::DetailedResult()
// -----------------------------------------------------------------------------
//
const MPEngDetailedResultEntry2& CPEngTransactionStatus::DetailedResult(
    TInt aIndex ) const
    {
    //a pointer to detailed result entry
    //If passed index is out of bounds, panics with
    //KPEngTransStatPanicCategory, EPEngDetailedAccessOutOfBounds.

    __ASSERT_ALWAYS( aIndex < iDetailedResultsCount,
                     CPEngTransactionStatus::Panic( EPEngDetailedAccessOutOfBounds ) );


    //Details
    CPEngTransactionStatus* self = const_cast<CPEngTransactionStatus*>( this );
    self->iDetailedResultsQIter.SetToFirst();

    TInt index = 0;
    while ( index < aIndex )
        {
        self->iDetailedResultsQIter++;
        index++;
        }

    const CPEngDetailedResultEntry* entry = self->iDetailedResultsQIter;
    return *entry;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::GetFirstDetailedResultByOperation()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::GetFirstDetailedResultByOperation(
    TInt aOperationID,
    const MPEngDetailedResultEntry2*& aDetailedResult )
    {
    //a pointer to "first" detailed result entry of asked operation.
    //If there isn't any detailed result of asked operation, returns KErrNotFound and NULL.
    //Else returns KErrNone.

    iDetResCurrentOperationId = aOperationID;
    iDetResCurrentIndex = 0; // start searching entries from first element

    return LocateNextDetailByOperation( aDetailedResult );
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::GetNextDetailedResultByOperation()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::GetNextDetailedResultByOperation(
    const MPEngDetailedResultEntry2*& aDetailedResult )
    {
    //A pointer to next found detailed result entry.
    //If operation criteria hasn't been set previously, panics with
    //KPEngTransStatPanicCategory, EPEngNoCriteriaForNextByOrigin.

    __ASSERT_ALWAYS( iDetResCurrentOperationId != KErrUnknown,
                     CPEngTransactionStatus::Panic( EPEngNoCriteriaForNextDetailByOperation ) );

    // returns KErrNone if a detailed result of the asked operation ID is found.
    // Else KErrNotFound and NULL.
    return LocateNextDetailByOperation( aDetailedResult );
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::GetDesc()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::GetDesc(
    TPtrC& /*aDescriptionText*/,
    TPEngTransStatusDescriptionText2 /*aDescriptionID*/ ) const
    {
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::GetInt()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::GetInt(
    TUint32& /*aDescriptionInt*/,
    TPEngTransStatusDescriptionInteger2 /*aDescriptionID*/ ) const
    {
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::CloneLC()
// -----------------------------------------------------------------------------
//
MPEngTransactionStatusCopy2* CPEngTransactionStatus::CloneLC() const
    {
    return BaseCloneLC();
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::StatusExtension()
// -----------------------------------------------------------------------------
//
MPEngTransactionStatusExtension2* CPEngTransactionStatus::StatusExtension()
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::Close()
// from MPEngTransactionStatusCopy
// -----------------------------------------------------------------------------
//
void CPEngTransactionStatus::Close()
    {
    delete this;
    }


// -----------------------------------------------------------------------------
// From MPEngAdvTransactionStatus
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::SetStatus()
// -----------------------------------------------------------------------------
//
void CPEngTransactionStatus::SetStatus( TInt aStatus )
    {
    iStatusCode = aStatus;
    }




// -----------------------------------------------------------------------------
// CPEngTransactionStatus::AddDetailedResultL()
// -----------------------------------------------------------------------------
//
void CPEngTransactionStatus::AddDetailedResultL( TInt aOperationID,
                                                 TInt aErrorCode,
                                                 const TUint32* aAttributeTypeID,
                                                 const TDesC* aPresenceID,
                                                 const TDesC* aContactListID,
                                                 const TDesC* aDescription )
    {
    CPEngDetailedResultEntry* entry = CPEngDetailedResultEntry::NewLC( aOperationID,
                                                                       aErrorCode,
                                                                       aAttributeTypeID,
                                                                       aPresenceID,
                                                                       aContactListID,
                                                                       aDescription );



    iDetailedResultsQ.AddLast( *entry );
    iDetailedResultsCount++;
    CleanupStack::Pop(); //entry
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::AddDataResultL()
// -----------------------------------------------------------------------------
//
void CPEngTransactionStatus::AddDataResultL( TInt aOperationID,
                                             HBufC16* aDataResult )

    {
    //This function behaves like array append.
    //It takes the aDataResult ownership only in full success.
    CPEngDataResultEntry* entry = CPEngDataResultEntry::NewLC( aOperationID,
                                                               aDataResult );



    iDataResultsQ.AddLast( *entry );
    iDataResultsCount++;
    CleanupStack::Pop(); //entry
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::GetFirstDataResultByOperation()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::GetFirstDataResultByOperation( TInt aOperationID,
                                                            TPtrC16& aDataResult )
    {
    //a pointer to "first" detailed result entry of asked operation.
    //If there isn't any detailed result of asked operation, returns KErrNotFound and
    //empty descriptor. Else returns KErrNone.

    iDataResCurrentOperationId = aOperationID;
    iDataResCurrentIndex = 0; // start searching entries from first element

    return LocateNextDataByOperation( aDataResult );
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::GetNextDataResultByOperation()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::GetNextDataResultByOperation( TPtrC16& aDataResult )
    {
    //A pointer to next found detailed result entry.
    //If operation criteria hasn't been set previously, panics with
    //KPEngTransStatPanicCategory, EPEngNoCriteriaForNextByOrigin.

    __ASSERT_ALWAYS( iDataResCurrentOperationId != KErrUnknown,
                     CPEngTransactionStatus::Panic( EPEngNoCriteriaForNextDataByOperation ) );

    // returns KErrNone if a detailed result of the asked operation ID is found.
    // Else KErrNotFound and NULL.
    return LocateNextDataByOperation( aDataResult );
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::Reset()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngTransactionStatus::Reset()
    {
    CPEngDetailedResultEntry* detailEntry;
    iDetailedResultsQIter.SetToFirst();

    while ( ( detailEntry = iDetailedResultsQIter++ ) != NULL ) // CSI: 64 #
        {
        iDetailedResultsQ.Remove( *detailEntry );
        delete detailEntry;
        }



    CPEngDataResultEntry* dataEntry;

    iDataResultsQIter.SetToFirst();
    while ( ( dataEntry = iDataResultsQIter++ ) != NULL )       // CSI: 64 #
        {
        iDataResultsQ.Remove( *dataEntry );
        delete dataEntry;
        }


    iDetailedResultsCount = 0;
    iStatusCode = KErrNone;
    iDetResCurrentOperationId = KErrUnknown;
    iDetResCurrentIndex = 0;
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::ImportStatusFrom()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngTransactionStatus::ImportStatusFrom(
    CPEngTransactionStatus& aSource )
    {
    MergeStatusCodes( aSource );

    CPEngDetailedResultEntry* detailEntry;
    aSource.iDetailedResultsQIter.SetToFirst();

    while ( ( detailEntry = aSource.iDetailedResultsQIter++ ) != NULL ) // CSI: 64 #
        {
        aSource.iDetailedResultsQ.Remove( *detailEntry );

        iDetailedResultsQ.AddLast( *detailEntry );
        iDetailedResultsCount++;
        }



    CPEngDataResultEntry* dataEntry;
    aSource.iDataResultsQIter.SetToFirst();
    while ( ( dataEntry = aSource.iDataResultsQIter++ ) != NULL )       // CSI: 64 #
        {
        aSource.iDataResultsQ.Remove( *dataEntry );

        iDataResultsQ.AddLast( *dataEntry );
        iDataResultsCount++;
        }

    delete &aSource;
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::FullCloneLC()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus* CPEngTransactionStatus::FullCloneLC() const
    {
    CPEngTransactionStatus* self = const_cast<CPEngTransactionStatus*>( this );

    CPEngTransactionStatus* copy = BaseCloneLC();


    //Copy internal details
    self->iDataResultsQIter.SetToFirst();
    CPEngDataResultEntry* dataEntry;

    while ( ( dataEntry = self->iDataResultsQIter++ ) != NULL )     // CSI: 64 #
        {
        CPEngDataResultEntry* newEntry = CPEngDataResultEntry::NewLC( *dataEntry );
        copy->iDataResultsQ.AddLast( *newEntry );
        copy->iDataResultsCount++;
        CleanupStack::Pop(); //newEntry
        }

    return copy;
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::PackResultsL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPEngTransactionStatus::PackResultsL() const
    {
    TInt size = ExternalizeSize();
    CBufFlat* dynBuffer = CBufFlat::NewL( size );
    CleanupStack::PushL( dynBuffer );

    RBufWriteStream wstream;
    wstream.Open( *dynBuffer );
    CleanupClosePushL( wstream );

    ExternalizeL( wstream );

    wstream.CommitL();
    CleanupStack::PopAndDestroy(); //wstream

    TPtrC8 dynBufferPtr( dynBuffer->Ptr( 0 ) );
    HBufC8* heapBuffer = dynBufferPtr.AllocL();
    CleanupStack::PopAndDestroy(); //dynBuffer

    return heapBuffer;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::UnpackResultsL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngTransactionStatus::UnpackResultsL( const TDesC8& aResultPkg )
    {
    RDesReadStream rstream;
    rstream.Open( aResultPkg );
    CleanupClosePushL( rstream );

    InternalizeL( rstream );

    CleanupStack::PopAndDestroy(); //rs
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::LocateNextDetailByOperation()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::LocateNextDetailByOperation(
    const MPEngDetailedResultEntry2*& aDetailedResult )
    {
    iDetailedResultsQIter.SetToFirst();
    CPEngDetailedResultEntry* entry;
    TInt index = 0;
    while ( index < iDetResCurrentIndex )
        {
        iDetailedResultsQIter++;
        index++;
        }


    while ( ( entry = iDetailedResultsQIter++ ) != NULL )  // CSI: 64 #
        {
        index++;
        if ( entry->Operation() == iDetResCurrentOperationId )
            {
            //match found, update position and return entry
            iDetResCurrentIndex = index;
            aDetailedResult = entry;
            return KErrNone;
            }
        }


    aDetailedResult = NULL;
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::LocateNextDataByOperation()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::LocateNextDataByOperation( TPtrC16& aDataResult )
    {
    iDataResultsQIter.SetToFirst();
    CPEngDataResultEntry* entry;
    TInt index = 0;
    while ( index < iDataResCurrentIndex )
        {
        iDataResultsQIter++;
        index++;
        }

    while ( ( entry = iDataResultsQIter++ ) != NULL )  // CSI: 64 #
        {
        index++;
        if ( entry->Operation() == iDataResCurrentOperationId )
            {
            //match found, update position and return entry
            iDataResCurrentIndex = index;
            aDataResult.Set( entry->Data() );
            return KErrNone;
            }
        }

    aDataResult.Set( NULL, 0 );
    return KErrNotFound;
    }




// -----------------------------------------------------------------------------
// CPEngTransactionStatus::MergeStatusCodes()
// -----------------------------------------------------------------------------
//
void CPEngTransactionStatus::MergeStatusCodes( const CPEngTransactionStatus& aSource )
    {
    //If no error yet in this container, take the one from imported one
    if ( iStatusCode == KErrNone )
        {
        iStatusCode = aSource.iStatusCode;
        return;
        }
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::BaseCloneLC()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus* CPEngTransactionStatus::BaseCloneLC() const
    {
    CPEngTransactionStatus* self = const_cast<CPEngTransactionStatus*>( this );


    CPEngTransactionStatus* copy = CPEngTransactionStatus::NewL();
    CleanupStack::PushL( copy );

    //Main status code
    copy->iStatusCode = iStatusCode;


    //Clone only base details - e.g. only those visible to clients
    self->iDetailedResultsQIter.SetToFirst();

    CPEngDetailedResultEntry* entry;
    while ( ( entry = self->iDetailedResultsQIter++ ) != NULL ) // CSI: 64 #
        {
        CPEngDetailedResultEntry* newEntry = CPEngDetailedResultEntry::NewLC( *entry );
        copy->iDetailedResultsQ.AddLast( *newEntry );
        copy->iDetailedResultsCount++;
        CleanupStack::Pop(); //newEntry
        }

    return copy;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::ExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngTransactionStatus::ExternalizeSize() const
    {
    CPEngTransactionStatus* self = const_cast<CPEngTransactionStatus*>( this );

    TInt size = 12; //iStatusCode: 4 bytes
    //iDetailedResultsCount: 4 bytes
    //iDataResultsCount: 4 bytes


    self->iDetailedResultsQIter.SetToFirst();
    CPEngDetailedResultEntry* detailedEntry;
    while ( ( detailedEntry = self->iDetailedResultsQIter++ ) != NULL ) // CSI: 64 #
        {
        size += detailedEntry->ExternalizeSize();
        }


    self->iDataResultsQIter.SetToFirst() ;
    CPEngDataResultEntry* dataEntry;
    while ( ( dataEntry = self->iDataResultsQIter++ ) != NULL ) // CSI: 64 #
        {
        size += dataEntry->ExternalizeSize();
        }

    return size;
    }


// -----------------------------------------------------------------------------
// CPEngTransactionStatus::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngTransactionStatus::ExternalizeL( RWriteStream& aStream ) const
    {
    CPEngTransactionStatus* self = const_cast<CPEngTransactionStatus*>( this );

    aStream.WriteInt32L( iStatusCode );


    //Details
    aStream.WriteInt32L( iDetailedResultsCount );
    self->iDetailedResultsQIter.SetToFirst();
    CPEngDetailedResultEntry* detailedEntry;
    while ( ( detailedEntry = self->iDetailedResultsQIter++ ) != NULL )  // CSI: 64 #
        {
        detailedEntry->ExternalizeL( aStream );
        }


    //Datas
    aStream.WriteInt32L( iDataResultsCount );

    self->iDataResultsQIter.SetToFirst();
    CPEngDataResultEntry* dataEntry;
    while ( ( dataEntry = self->iDataResultsQIter++ ) != NULL ) // CSI: 64 #
        {
        dataEntry->ExternalizeL( aStream );
        }
    }



// -----------------------------------------------------------------------------
// CPEngTransactionStatus::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngTransactionStatus::InternalizeL( RReadStream& aStream )
    {
    Reset();

    iStatusCode = aStream.ReadInt32L();
    TInt ii;

    const TInt detailedCount = aStream.ReadInt32L();
    for ( ii = 0; ii < detailedCount; ii++ )
        {
        CPEngDetailedResultEntry* entry = CPEngDetailedResultEntry::NewLC( aStream );

        iDetailedResultsQ.AddLast( *entry );
        iDetailedResultsCount++;

        CleanupStack::Pop(); //entry
        }


    const TInt dataCount = aStream.ReadInt32L();
    for ( ii = 0; ii < dataCount; ii++ )
        {
        CPEngDataResultEntry* entry = CPEngDataResultEntry::NewLC( aStream );

        iDataResultsQ.AddLast( *entry );
        iDataResultsCount++;

        CleanupStack::Pop(); //entry
        }
    }

// -----------------------------------------------------------------------------
// CPEngTransactionStatus::Panic()
// -----------------------------------------------------------------------------
void CPEngTransactionStatus::Panic( TInt aReason )
    {
    User::Panic( KPEngTransStatPanicCategory, aReason );
    }



//  End of File

