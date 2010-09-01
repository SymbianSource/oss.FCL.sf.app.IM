/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
* package entry and entity class.
*
*/


// INCLUDE FILES
#include    <s32std.h>
#include    "impsfields.h"
#include    "impspacked.h"
#include    "impsutils.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif
// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// TImpsPackedEntity::TImpsPackedEntity
// ---------------------------------------------------------
//
EXPORT_C TImpsPackedEntity::TImpsPackedEntity( HBufC8*& aBuffer )
: iBuffer( aBuffer )
    {}

// ---------------------------------------------------------
// TImpsPackedEntity::PackEntity
// ---------------------------------------------------------
//
EXPORT_C TInt TImpsPackedEntity::PackEntity( const CImpsFields& aEntity )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("TImpsPackedEntity::PackEntityL begins"));
#endif
    // find the start and end of the buffer
    const TUint8* pS = iBuffer->Ptr();
    const TUint8* pE = PtrAdd( pS, iBuffer->Des().MaxSize() );

    TInt error = DoPackEntity( pS, pE, aEntity );
    if ( error==KErrNone )
        {
        // update the length of the buffer
        iBuffer->Des().SetLength( pS-iBuffer->Ptr() );
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("TImpsPackedEntity::PackEntityL ends with %d"), error);
#endif
    return error;
    }

// ---------------------------------------------------------
// TImpsPackedEntity::UnpackEntityL
// ---------------------------------------------------------
//
EXPORT_C void TImpsPackedEntity::UnpackEntityL( CImpsFields& aEntity )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("TImpsPackedEntity::UnPackEntityL begins"));
#endif

    TUint8* pS = CONST_CAST(TUint8*, iBuffer->Ptr());
    DoUnpackEntityL( pS, aEntity );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("TImpsPackedEntity::UnPackEntityL ends"));
#endif
    }

// ---------------------------------------------------------
// TImpsPackedEntity::DoUnpackEntityL
// ---------------------------------------------------------
void TImpsPackedEntity::DoUnpackEntityL( 
    TUint8*& aPtr, 
    CImpsFields& aEntity )
    {
    TInt sizePackedCont = Align4( KImpsPackedCopyData );
    
    // Get the entry from the start of the buffer
    Mem::Copy( &aEntity, aPtr, sizePackedCont);
    aPtr = aPtr + sizePackedCont;

    CImpsData* data = aEntity.Data( );
    DoUnpackDataL( aPtr, *data );
    }



// ---------------------------------------------------------
// TImpsPackedEntity::DoUnpackDataL
// ---------------------------------------------------------
void TImpsPackedEntity::DoUnpackDataL(
    TUint8*& aPtr, 
    CImpsData& aEntity )
    {
    TInt mySize = 0;
    Mem::Copy( &mySize, aPtr, sizeof( mySize ) );
    aPtr += sizeof( mySize );
    aEntity.UnPackL( aPtr );
    }

// ---------------------------------------------------------
// TImpsPackedEntity::DoPackEntity
// Format is
//   fundamental types copied 
//   plus each descriptor: size + 8-bit data
// NOTICE: This must be modified always when CImpsFields members is changed!!!
// ---------------------------------------------------------
TInt TImpsPackedEntity::DoPackEntity(
    const TUint8*& aPtrStart, 
    const TUint8* aPtrEnd, 
    const CImpsFields& aEntity )
//
// Fails with KErrOverflow if the packed entry is too large
// aPtrStart is always returned pointing to the end of the packed entry (even if too large)
//
    {
    // make sure the entry can fit into the memory area defined by the two pointers

    TInt size =  aEntity.Size();
    if ( ( aPtrStart + size ) > aPtrEnd )
        {
        aPtrStart += size;
        return KErrOverflow;
        }

    size = Align4( KImpsPackedCopyData );
    // copy the entry and descriptors into the memory area
    Mem::Copy((void*)aPtrStart, &aEntity, size );
    aPtrStart += size;

    // Notice: Login data is not packed at the very moment

    TInt ret = DoPackData( aPtrStart, aPtrEnd, *aEntity.Data( ) );

    return ret;
    }



// ---------------------------------------------------------
// TImpsPackedEntity::DoPackData
// ---------------------------------------------------------
TInt TImpsPackedEntity::DoPackData(
    const TUint8*& aPtrStart, 
    const TUint8* aPtrEnd, 
    const CImpsData& aEntity )
    {
    TInt size = aEntity.Size(); 
    size += (2 * sizeof (TInt));		//size,count
    if ( ( aPtrStart + size ) > aPtrEnd )
        {
        aPtrStart += size;
        return KErrOverflow;
        }
    // write the size first.
    Mem::Copy( (void*)aPtrStart, &size, sizeof( size ) );
    aPtrStart = aPtrStart + sizeof( size );

    TInt ret = aEntity.Pack( aPtrStart );
    return ret;
    }

// ---------------------------------------------------------
// TImpsPackedEntity::DoPackDescriptor
// ---------------------------------------------------------
void TImpsPackedEntity::DoPackDescriptor(
    const TUint8*& aPtrStart, 
    const TDesC& /*HBufC* */ aDescriptor )
    {

    TInt32 tempSize = 0;
    tempSize = aDescriptor.Size();
    Mem::Copy( (void*)aPtrStart, &tempSize, sizeof( tempSize) );
    aPtrStart = aPtrStart + sizeof( tempSize );
    if ( tempSize )
        {
        Mem::Copy( (void*)aPtrStart, aDescriptor.Ptr(), tempSize);
        aPtrStart = aPtrStart + tempSize ;
        }
    }

// ---------------------------------------------------------
// TImpsPackedEntity::DoUnpackDescriptorLC
// ---------------------------------------------------------
void TImpsPackedEntity::DoUnpackDescriptorLC(
    const TUint8*& aPtrStart, 
    HBufC*& aDescriptor )
    {

    const TUint8* textPtr = aPtrStart;
    TInt32 tempSize = 0;

    Mem::Copy( &tempSize, textPtr, sizeof( tempSize) );
    textPtr = textPtr + sizeof( tempSize );
    if ( tempSize > 0 )
        {
        // put and leave this to the CleanupStack because of other the
        // calling method can leave
        aDescriptor = HBufC::NewL( tempSize / 2 );  
        CleanupStack::PushL( aDescriptor );
        Mem::Copy( (void*)aDescriptor->Ptr(), textPtr, tempSize );
        aDescriptor->Des().SetLength( tempSize / 2 ); 
        textPtr = textPtr + tempSize; 
        }
    else
        {
        aDescriptor = NULL;
        }
    if ( tempSize < 0 )
        {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("TImpsPackedEntity: Descriptor Length 0"));
#endif
        }

    aPtrStart = (TUint8*) textPtr;
    
    }

// ---------------------------------------------------------
// TImpsPackedEntity::DoPackArray
// ---------------------------------------------------------
EXPORT_C void TImpsPackedEntity::DoPackArray(
            const TUint8*& aPtrStart,
            const MDesCArray* aArray )
    {

    // First count nbr of elements
    TInt32 nbrElements = 0;

    if ( aArray != NULL )
        {
        nbrElements = aArray->MdcaCount();
        }
    Mem::Copy( (void*)aPtrStart, &nbrElements, sizeof( nbrElements ) );
    aPtrStart = aPtrStart + sizeof( nbrElements );

    // Store each array element
    for ( TInt i = 0; i < nbrElements; i++ )
        {
        TPtrC element = aArray->MdcaPoint(i);
        DoPackDescriptor( aPtrStart, element );
        }

    }

// ---------------------------------------------------------
// TImpsPackedEntity::DoUnpackArrayL
// ---------------------------------------------------------
EXPORT_C void TImpsPackedEntity::DoUnpackArrayL(
            const TUint8*& aPtrStart,
            CDesCArray* aArray )
    {

    const TUint8* textPtr = aPtrStart;
    TInt32 tempSize = 0;

    // Get the number of elements
    Mem::Copy( &tempSize, textPtr, sizeof( tempSize) );
    textPtr = textPtr + sizeof( tempSize );

    if ( tempSize == 0 )
        {
        // There are no elements in array
        aPtrStart = (TUint8*) textPtr;
        return;
        }

    // Unpack each element and add to the array
    HBufC* myPtr;
    HBufC*& descriptor = myPtr;
    for (  TInt i = 0; i < tempSize; i++ )
        {
        // This allocates memory for each element
        DoUnpackDescriptorLC( textPtr, descriptor );  
        // Array may contain zero length descriptor
        if ( descriptor != NULL )
            {
            aArray->AppendL( descriptor->Des() );
            //delete myPtr;
            CleanupStack::PopAndDestroy();
            }
        else
            {
            aArray->AppendL( KNullDesC );
            }
        myPtr = NULL;
        }

    // Increase the packed data pointer
    aPtrStart = (TUint8*) textPtr;
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
