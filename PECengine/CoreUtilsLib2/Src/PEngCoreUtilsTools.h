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
* Description:  CoreUtils private tools.
*
*/

#ifndef __PENGCOREUTILSTOOLS_H__
#define __PENGCOREUTILSTOOLS_H__

//  INCLUDES
#include    <e32base.h>
#include    <s32strm.h>
#include    <s32mem.h>
#include    <bamdesca.h>



// ======================= DLL GLOBAL INLINE FUNCTIONS ========================

// -----------------------------------------------------------------------------
// DoGetDescriptor()
// Data get utility
// -----------------------------------------------------------------------------
inline const TDesC& DoGetDescriptor( const HBufC* const & aBuffer )
    {
    if ( aBuffer )
        {
        return *aBuffer;
        }

    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// DoSetSubPartL()
// Data set utility
// -----------------------------------------------------------------------------
inline void DoSetDescriptorL( HBufC*& aPtr, const TDesC& aData )
    {
    HBufC* tmp = aData.AllocL(); //alloc first the new data

    delete aPtr;
    aPtr = tmp;
    }


// -----------------------------------------------------------------------------
// ExternalizeBufferL()
// Buffer utility
// -----------------------------------------------------------------------------
//
inline void ExternalizeBufferL( HBufC* aBuffer, RWriteStream& aStream )
    {
    if ( aBuffer && aBuffer->Length() != 0 )
        {
        aStream.WriteInt32L( aBuffer->Length() );
        aStream.WriteL( *aBuffer );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }
    }


// -----------------------------------------------------------------------------
// InternalizeBufferL()
// Buffer utility
// -----------------------------------------------------------------------------
//
inline HBufC* InternalizeBufferL( RReadStream& aStream )
    {
    HBufC* buffer = NULL;

    TInt length = aStream.ReadInt32L();
    if ( length != 0 )
        {
        buffer = HBufC::NewLC( length );
        TPtr ptr( buffer->Des() );
        aStream.ReadL( ptr, length );
        CleanupStack::Pop(); //buffer
        }

    return buffer;
    }


// -----------------------------------------------------------------------------
// BufferExternalizeSizeInBytes()
// Buffer utility
// -----------------------------------------------------------------------------
//
inline TInt BufferExternalizeSizeInBytes( const HBufC* aBuffer )
    {
    TInt size( 4 ); //Buffer length indicator: 4 bytes

    if ( aBuffer && aBuffer->Length() != 0 )
        {
        size += aBuffer->Size();
        }

    return size;
    }


#endif // __PENGCOREUTILSTOOLS_H__

// end of File


