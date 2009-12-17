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
* Description:  Collection of tools used to pack and unpack
*                data transferred over IPC.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include "PEngMessagePacker.h"


_LIT( KPEngMsgPckPanic, "PEngMsgPckr" );
enum TPEngMsgPckPanicReason
    {
    EResetIntArrayBufLength,
    EIntArrayCountBufLength,
    EIntFromArrayIndexNegative,
    EIntFromArrayIndexTooLarge
    };


// ====================== MEMBER FUNCTIONS ======================================

// =============================================================================
// ===================== DESCRIPTOR ARRAY PACKING ==============================
// =============================================================================


// -----------------------------------------------------------------------------
// PEngMessagePacker::DesArrayPackageSize()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::DesArrayPackageSize( const MDesCArray& aDesArray )
    {
    TInt count ( aDesArray.MdcaCount() );
    TInt len( count + 1 ); // once more count on the begining of buf
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        len += aDesArray.MdcaPoint( x ).Length();
        }

    return len;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::DesArrayPackageSize()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::DesArrayPackageSize( const TDesC& aDesAsArray )
    {
    return aDesAsArray.Length() + 2; // length and cout
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::PackDesArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC16* PEngMessagePacker::PackDesArrayL( const MDesCArray& aDesArray )
    {
    HBufC16* desBuffer = HBufC16::NewL( DesArrayPackageSize( aDesArray ) );
    TPtr des = desBuffer->Des();
    TInt count ( aDesArray.MdcaCount() );
    des.Append( ( TUint16* ) &count, 1 );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        TPtrC element( aDesArray.MdcaPoint( x ) );
        TInt length( element.Length() );
        des.Append( reinterpret_cast<TUint16*> ( &length ), 1 );
        des.Append( element );
        }

    return desBuffer;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::PackDesArray()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::PackDesArray( TDes& aBuffer,
                                               const MDesCArray& aDesArray )
    {
    aBuffer.Zero();
    if ( aBuffer.MaxLength() < DesArrayPackageSize( aDesArray ) )
        {
        return KErrArgument;
        }

    TInt count ( aDesArray.MdcaCount() );
    aBuffer.Append( reinterpret_cast<TUint16*> ( &count ), 1 );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        TPtrC element( aDesArray.MdcaPoint( x ) );
        TInt length( element.Length() );
        aBuffer.Append( reinterpret_cast<TUint16*> ( &length ), 1 );
        aBuffer.Append( element );
        }


    return KErrNone;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::PackOneDesAsArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC16* PEngMessagePacker::PackOneDesAsArrayL( const TDesC& aDes )
    {
    HBufC16* desBuffer = HBufC16::NewL( aDes.Length() + 2 ); // count and size
    TPtr des = desBuffer->Des();
    TInt count( 1 ); //Only one descriptor is packed
    des.Append( reinterpret_cast<TUint16*> ( &count ), 1 );
    TInt length( aDes.Length() );
    des.Append( reinterpret_cast<TUint16*> ( &length ), 1 );
    des.Append( aDes );
    return desBuffer;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::PackOneDesAsArray()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::PackOneDesAsArray( TDes& aBuffer,
                                                    const TDesC& aDes )
    {
    aBuffer.Zero();
    if ( aBuffer.MaxLength() < ( aDes.Length() + 2 ) ) // count and size
        {
        return KErrArgument;
        }

    TInt count( 1 );
    aBuffer.Append( reinterpret_cast<TUint16*> ( &count ), 1 );
    TInt length( aDes.Length() );
    aBuffer.Append( reinterpret_cast<TUint16*> ( &length ), 1 );
    aBuffer.Append( aDes );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::AppendToDesArray()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::AppendToDesArray( TDes& aBuffer,
                                                   const TDesC& aDes )
    {
    // is anything in the array?
    TInt count( 0 ); // if array will be empty
    if ( aBuffer.Length() == 0 )
        {
        // first element appent count first
        aBuffer.Append( reinterpret_cast<TUint16*> ( &count ), 1 );
        }

    // check space
    if ( ( aBuffer.Length() + aDes.Length() + 1 ) > aBuffer.MaxLength() )
        {
        return KErrBadDescriptor;
        }

    TInt length( aDes.Length() );
    aBuffer.Append( reinterpret_cast<TUint16*> ( &length ), 1 );
    aBuffer.Append( aDes );

    //  update count
    count = ( TInt ) ( aBuffer[ 0 ] ); // count is first
    count++;
    aBuffer[ 0 ] = static_cast<TUint16> ( count );
    return KErrNone;
    }




// =============================================================================
// =================== DESCRIPTOR ARRAY EXTRACTING =============================
// =============================================================================


// -----------------------------------------------------------------------------
// PEngMessagePacker::DesArrayCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::DesArrayCount( const TDesC& aBuffer )
    {
    if ( aBuffer.Length() == 0 )
        {
        return 0; // des is empty
        }

    return ( TInt ) ( aBuffer[ 0 ] );
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::UnpackDesArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::UnpackDesArrayL( const TDesC& aBuffer,
                                                  CDesCArray& aArray )
    {
    return UnpackDesTempArrayL( aBuffer, aArray );
    }

// -----------------------------------------------------------------------------
// PEngMessagePacker::UnpackDesArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::UnpackDesArrayL( const TDesC& aBuffer,
                                                  CPtrCArray& aArray )
    {
    return UnpackDesTempArrayL( aBuffer, aArray );
    }

// -----------------------------------------------------------------------------
// PEngMessagePacker::UnpackDesArrayLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CDesCArray* PEngMessagePacker::UnpackDesArrayLC( const TDesC& aBuffer )
    {
    TInt count ( DesArrayCount( aBuffer ) );
    CDesC16ArrayFlat* desArray = new( ELeave ) CDesC16ArrayFlat( ++count );
    CleanupStack::PushL( desArray );
    UnpackDesArrayL( aBuffer, *desArray );
    return desArray;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::UnpackFirstDesFromArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* PEngMessagePacker::UnpackFirstDesFromArrayL( const TDesC& aBuffer )
    {
    // Ignore count, read length of des
    TInt desL ( ( TInt ) ( aBuffer[1] ) ); // first was count

    // First two are info about count and length
    // Returns buffer ownership to caller
    return aBuffer.Mid( 2, desL ).AllocL();
    }



// =============================================================================
// ===================== INTEGER ARRAY HANDLING ================================
// =============================================================================


// -----------------------------------------------------------------------------
// AppendInt32ToDesc()
// -----------------------------------------------------------------------------
//
void AppendInt32ToDesc( TDes& aBuffer, TInt32 aValue )
    {
    aBuffer.Append( ( TUint16* ) &aValue, 2 );  //TInt32 ==> 2 x 16bytes
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::TIntArrayPackageSize()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::TIntArrayPackageSize( TInt aElementCount )
    {
    TInt size = 2; //array count is stored to 32 bit ==> 2 x 16bytes
    size += aElementCount * 2;  //each array element takes 2 x 16bytes
    return size;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::PackTIntArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC16* PEngMessagePacker::PackTIntArrayL( const RArray<TInt>& aTIntArray )
    {
    TInt packSize = PEngMessagePacker::TIntArrayPackageSize( aTIntArray.Count() );
    HBufC* packBuf = HBufC::NewL( packSize );
    TPtr pack = packBuf->Des();

    //Add first the count
    TInt32 count( aTIntArray.Count() );
    AppendInt32ToDesc( pack, count );


    //Then entries
    for ( TInt ix( 0 ) ; ix < count ; ix++ )
        {
        TInt32 intValue = aTIntArray[ ix ];
        AppendInt32ToDesc( pack, intValue );
        }


    return packBuf;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::ResetTIntArray()
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngMessagePacker::ResetTIntArray( TDes& aBuffer )
    {
    __ASSERT_ALWAYS( aBuffer.MaxLength() >= 2,
                     User::Panic( KPEngMsgPckPanic, EResetIntArrayBufLength ) );

    aBuffer.Zero();

    //Reset first the count
    AppendInt32ToDesc( aBuffer, 0 ); //No entries ==> count is 0

    //Then entries
    const TInt placeCount = ( aBuffer.MaxLength() - 2 ) / 2;
    for ( TInt ix = 0; ix < placeCount; ix++ )
        {
        AppendInt32ToDesc( aBuffer, KErrNotFound ); //Entries are reset to -1
        }
    }



// -----------------------------------------------------------------------------
// PEngMessagePacker::TIntArrayCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::TIntArrayCount( const TDesC& aBuffer )
    {
    //Array count is stored in the start of buffer, 2 x 16bytes
    __ASSERT_ALWAYS( aBuffer.Length() >= 2,
                     User::Panic( KPEngMsgPckPanic, EIntArrayCountBufLength ) );


    TInt32 count = *( TInt32* ) aBuffer.Ptr();
    return count;
    }

// -----------------------------------------------------------------------------
// PEngMessagePacker::TIntFromArray()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PEngMessagePacker::TIntFromArray( const TDesC& aBuffer,
                                                TInt aIndex )
    {
    //Assert index range
    __ASSERT_ALWAYS( aIndex >= 0,
                     User::Panic( KPEngMsgPckPanic, EIntFromArrayIndexNegative ) );

    __ASSERT_ALWAYS( aIndex < PEngMessagePacker::TIntArrayCount( aBuffer ),
                     User::Panic( KPEngMsgPckPanic, EIntFromArrayIndexTooLarge ) );


    TInt entryOffset = 2;   //array count is in the start, 2 x 16bytes
    entryOffset += aIndex * 2;  //each entry takes 2 x 16bytes


    TInt entryValue = *( TInt32* ) aBuffer.Mid( entryOffset ).Ptr();
    return entryValue;
    }


// -----------------------------------------------------------------------------
// PEngMessagePacker::UnpackDesTempArrayL()
// -----------------------------------------------------------------------------
//
template<class T>
TInt PEngMessagePacker::UnpackDesTempArrayL( const TDesC& aBuffer,
                                             T& aArray )
    {
    TInt count ( DesArrayCount( aBuffer ) );
    TInt desOffset( 1 ); // start with first element
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        // get length of the des
        TInt desL ( static_cast<TUint16> ( ( aBuffer[ desOffset ] ) ) );
        // get offset to the beginning of the packed descriptor
        desOffset++;
        aArray.AppendL( aBuffer.Mid( desOffset, desL ) );
        // move to length info of another packed des
        desOffset += desL;
        }

    return count;
    }

//  End of File

