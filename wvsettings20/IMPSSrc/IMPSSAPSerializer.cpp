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
* Description:  Serialization tools for SAP Settings Store.
*
*/

// INCLUDE FILES
#include    <e32std.h>
#include    "IMPSSAPSerializer.h"


// -----------------------------------------------------------------------------
// IMPSSAPSerializer::ExternalizeBufferL()
// -----------------------------------------------------------------------------
//
void IMPSSAPSerializer::ExternalizeBufferL( const HBufC* aBuffer, RWriteStream& aStream )
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
// IMPSSAPSerializer::InternalizeBufferL()
// -----------------------------------------------------------------------------
//
HBufC* IMPSSAPSerializer::InternalizeBufferL( RReadStream& aStream )
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
// IMPSSAPSerializer::BufferSizeInBytes()
// -----------------------------------------------------------------------------
//
TInt IMPSSAPSerializer::BufferSizeInBytes( const HBufC* aBuffer )
    {
    TInt size = 4; //Buffer length indicator: 4 bytes

    if ( aBuffer && aBuffer->Length() != 0 )
        {
        size = size + aBuffer->Size();
        }

    return size;
    }



//  End of File
