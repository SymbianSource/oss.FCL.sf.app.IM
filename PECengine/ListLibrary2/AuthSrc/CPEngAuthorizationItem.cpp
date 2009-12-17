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
* Description:  Container of one reactive authorization item
*
*/

// INCLUDE FILES
#include    "CPEngAuthorizationItem.h"
#include    "PEngListLibraryPanics.h"
#include    <e32std.h>
#include    <s32strm.h>


// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Compares two TUint32 numbers.
// Follows standard comparison rules.
// -----------------------------------------------------------------------------
//
TInt CompareTUint32( const TUint32& aA, const TUint32& aB )
    {
    return aA - aB;
    }


#define ARRAY_ORDER TLinearOrder<TUint32> ( CompareTUint32 )

// CONST
// Space required in stream to store one TInt32 - 4 bytes
const TInt KTIntStoreSize =  4;


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::CPEngAuthorizationItem
// -----------------------------------------------------------------------------
//
CPEngAuthorizationItem::CPEngAuthorizationItem( TInt& aSize )
        : iSize( aSize )
    {
    // length of the User Id => 4 bytes
    iSize += KTIntStoreSize;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationItem::ConstructL( const TDesC& aUserId )
    {
    iUserId = aUserId.AllocL();
    iSize += ( aUserId.Length() );
    }


// Destructor
CPEngAuthorizationItem::~CPEngAuthorizationItem()
    {
    if ( iUserId )
        {
        iSize -= ( KTIntStoreSize + iUserId->Length() );
        }
    else
        {
        iSize -= KTIntStoreSize;
        }
    delete iUserId;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::UserId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngAuthorizationItem::Id() const
    {
    return *iUserId;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::LocalFlags()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationItem::LocalFlags() const
    {
    return iLocalFlags;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::SetLocalFlags()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationItem::SetLocalFlags( TInt aLocalFlag )
    {
    iLocalFlags = aLocalFlag;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationItem::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iUserId->Length() );
    aStream << *iUserId;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationItem::InternalizeL( RReadStream& aStream )
    {
    __ASSERT_DEBUG( !iUserId, Panic( EReactiveAuthInternalizeMisused ) );

    // user Id
    TInt length( aStream.ReadInt32L() );
    iUserId = HBufC::NewL( aStream, length );
    iSize += length;
    }



// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::ExternalizeArrayL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationItem::ExternalizeArrayL( RWriteStream& aStream,
                                                const RArray<TUint32>& aArray )
    {
    TInt count( aArray.Count() );
    aStream.WriteInt32L( count );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        aStream.WriteInt32L( aArray[ x ] );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::InternalizeArrayL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationItem::InternalizeArrayL( RReadStream& aStream,
                                                RArray<TUint32>& aArray,
                                                TInt& aSize )
    {
    // 4 bytes per each number
    aSize -= ( KTIntStoreSize * aArray.Count() );
    TInt count( aStream.ReadInt32L() );
    aArray.Reset();
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        aArray.AppendL( aStream.ReadInt32L() );
        //  4 bytes per each attribute
        aSize += KTIntStoreSize;
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::SizeOfArray()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationItem::SizeOfArray( const RArray<TUint32>& aArray )
    {
    // 4 bytes per each element of array + count of the array
    return KTIntStoreSize * ( aArray.Count() + 1 );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::AddAttributeToArrayL()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationItem::AddAttributeToArrayL( RArray<TUint32>& aArray,
                                                   TUint32 aAttribute )
    {
    TInt err( aArray.InsertInOrder( aAttribute, ARRAY_ORDER ) );
    if ( err == KErrAlreadyExists )
        {
        return 0; //nothing was added => size change 0
        }

    User::LeaveIfError( err );
    return KTIntStoreSize;
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationItem::CopyArrayContentL()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationItem::CopyArrayContentL( RArray<TUint32>& aTarget,
                                                const TArray<TUint32>& aSource )
    {
    TInt sizeDelta( 0 );
    TInt count( aSource.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        sizeDelta += AddAttributeToArrayL( aTarget, aSource[ x ] );
        }

    return sizeDelta;
    }


//  End of File
