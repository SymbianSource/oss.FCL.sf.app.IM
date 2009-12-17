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
* Description:  Detailed entry implementation.
*
*/

// INCLUDE FILES
#include <E32Base.h>
#include <S32Strm.h>
#include "CPEngDetailedResultEntry.h"

//DATA TYPES

/**
 * Stream content indication.
 */
enum TPEngStreamContent
    {
    EPEngNullField = -1,    //Field isn't defined in stream (it is NULL)
    EPEngHasField = 1       //Field is defined in stream
    };


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngDetailedResultEntry* CPEngDetailedResultEntry::NewLC( TInt aOperationID,
                                                           TInt aDetErr,
                                                           const TUint32* aAttribID,
                                                           const TDesC* aPresID,
                                                           const TDesC* aContactListID,
                                                           const TDesC* aDescription  )
    {
    CPEngDetailedResultEntry* self = new ( ELeave ) CPEngDetailedResultEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aOperationID,
                      aDetErr,
                      aAttribID,
                      aPresID,
                      aContactListID,
                      aDescription );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngDetailedResultEntry* CPEngDetailedResultEntry::NewLC( RReadStream& aStream )
    {
    CPEngDetailedResultEntry* self = new ( ELeave ) CPEngDetailedResultEntry();
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;
    }



// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngDetailedResultEntry* CPEngDetailedResultEntry::NewLC( const CPEngDetailedResultEntry& aEntry )
    {
    CPEngDetailedResultEntry* self = new ( ELeave ) CPEngDetailedResultEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    return self;
    }


// Destructor
CPEngDetailedResultEntry::~CPEngDetailedResultEntry()
    {
    delete iAttribID;
    delete iPresID;
    delete iContactListID;
    delete iDescription;
    }



// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::CPEngDetailedResultEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngDetailedResultEntry::CPEngDetailedResultEntry()
    {
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngDetailedResultEntry::ConstructL( TInt aOperationID,
                                           TInt aDetErr,
                                           const TUint32* aAttribID,
                                           const TDesC* aPresID,
                                           const TDesC* aContactListID,
                                           const TDesC* aDescription  )
    {
    iOperationID = aOperationID;
    iDetErr = aDetErr;

    if ( aAttribID )
        {
        iAttribID = new ( ELeave ) TUint32( *aAttribID );
        }

    if ( aPresID )
        {
        iPresID = aPresID->AllocL();
        }

    if ( aContactListID )
        {
        iContactListID  = aContactListID->AllocL();
        }

    if ( aDescription )
        {
        iDescription = aDescription->AllocL();
        }
    }



// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngDetailedResultEntry::ConstructL( const CPEngDetailedResultEntry& aEntry )
    {
    iOperationID = aEntry.iOperationID;
    iDetErr = aEntry.iDetErr;

    if ( aEntry.iAttribID )
        {
        iAttribID = new ( ELeave ) TUint32( *aEntry.iAttribID );
        }

    if ( aEntry.iPresID )
        {
        iPresID = aEntry.iPresID->AllocL();
        }

    if ( aEntry.iContactListID )
        {
        iContactListID = aEntry.iContactListID->AllocL();
        }

    if ( aEntry.iDescription )
        {
        iDescription = aEntry.iDescription->AllocL();
        }
    }




// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::Operation()
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::Operation() const
    {
    return iOperationID;
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::Error()
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::Error() const
    {
    return iDetErr;
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::GetDetailedDesc()
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::GetDetailedDesc( TPtrC& aDescriptionText,
                                                TPEngDetailedResultDescriptionText2 aDescriptionID ) const
    {
    switch ( aDescriptionID )
        {
        case EPEngDTPresenceID:
            {
            return DoReturnBuffer( iPresID, aDescriptionText );
            }

        case EPEngDTContactListID:
            {
            return DoReturnBuffer( iContactListID, aDescriptionText );
            }

        case EPEngDTErrorDescription:
            {
            return DoReturnBuffer( iDescription, aDescriptionText );
            }
        }

    return DoReturnBuffer( NULL, aDescriptionText );
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::GetDetailedInt()
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::GetDetailedInt( TUint32& aDescriptionInt,
                                               TPEngDetailedResultDescriptionInteger2 aDescriptionID ) const
    {
    switch ( aDescriptionID )
        {
        case EPEngDIAttributeID:
            {
            return DoReturnInteger( iAttribID, aDescriptionInt );
            }
        }

    return DoReturnInteger( NULL, aDescriptionInt );
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::EntryExtension()
// -----------------------------------------------------------------------------
//
MPEngDetailedResultEntryExtension2* CPEngDetailedResultEntry::EntryExtension()
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngDetailedResultEntry::InternalizeL( RReadStream& aStream )
    {
    iOperationID = aStream.ReadInt32L();
    iDetErr = aStream.ReadInt32L();

    DoInternalizeIntegerOrNullL( iAttribID, aStream );
    DoInternalizeBufferOrNullL( iPresID, aStream );
    DoInternalizeBufferOrNullL( iContactListID, aStream );
    DoInternalizeBufferOrNullL( iDescription, aStream );
    }



// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngDetailedResultEntry::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iOperationID );
    aStream.WriteInt32L( iDetErr );

    DoExternalizeIntegerOrNullL( iAttribID, aStream );
    DoExternalizeBufferOrNullL( iPresID, aStream );
    DoExternalizeBufferOrNullL( iContactListID, aStream );
    DoExternalizeBufferOrNullL( iDescription, aStream );
    }



// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::ExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::ExternalizeSize() const
    {
    TInt size = 8; //operation 4 bytes
    //error code 4 bytes

    size += DoIntegerExternalizeSizeInBytes( iAttribID );
    size += DoBufferExternalizeSizeInBytes( iPresID );
    size += DoBufferExternalizeSizeInBytes( iContactListID );
    size += DoBufferExternalizeSizeInBytes( iDescription );

    return size;
    }



// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::DoReturnBuffer()
// Static helper.
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::DoReturnBuffer( const HBufC* const & aPtr,
                                               TPtrC16& aBuffer )
    {
    if ( aPtr )
        {
        aBuffer.Set( *aPtr );
        return KErrNone;
        }

    aBuffer.Set( NULL, 0 );
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::DoReturnInteger()
// Static helper.
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::DoReturnInteger( const TUint32* aPtr,
                                                TUint32& aInteger )
    {
    if ( aPtr )
        {
        aInteger = *aPtr;
        return KErrNone;
        }

    aInteger = 0;
    return KErrNotFound;
    }



// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::DoExternalizeBufferOrNullL()
// Static helper.
// -----------------------------------------------------------------------------
//
void CPEngDetailedResultEntry::DoExternalizeBufferOrNullL( const HBufC* aBuffer,
                                                           RWriteStream& aStream )
    {
    if ( aBuffer )
        {
        aStream.WriteInt8L( EPEngHasField );
        aStream << TCardinality( aBuffer->Length() );  //pack the length bits
        aStream.WriteL( *aBuffer );
        }
    else
        {
        aStream.WriteInt8L( EPEngNullField );
        }
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::DoExternalizeIntegerOrNullL()
// Static helper.
// -----------------------------------------------------------------------------
//
void CPEngDetailedResultEntry::DoExternalizeIntegerOrNullL( TUint32* aInteger,
                                                            RWriteStream& aStream )
    {
    if ( aInteger )
        {
        aStream.WriteInt8L( EPEngHasField );
        aStream.WriteUint32L( *aInteger );
        }
    else
        {
        aStream.WriteInt8L( EPEngNullField );
        }
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::DoInternalizeBufferOrNullL()
// Static helper.
// -----------------------------------------------------------------------------
//
void CPEngDetailedResultEntry::DoInternalizeBufferOrNullL( HBufC*& aBuffer,
                                                           RReadStream& aStream )
    {
    HBufC* buffer = NULL;

    TPEngStreamContent content = static_cast< TPEngStreamContent >( aStream.ReadInt8L() );

    if ( content == EPEngHasField )
        {
        TCardinality c;
        aStream >> c;   //read the length bits
        TInt length = c;

        buffer = HBufC::NewLC( length );
        TPtr ptr( buffer->Des() );
        aStream.ReadL( ptr, length );
        CleanupStack::Pop(); //buffer
        }

    delete aBuffer;
    aBuffer = buffer;
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::DoInternalizeIntegerOrNullL()
// Static helper.
// -----------------------------------------------------------------------------
//
void CPEngDetailedResultEntry::DoInternalizeIntegerOrNullL( TUint32*& aInteger,
                                                            RReadStream& aStream )
    {
    TUint32* integer = NULL;

    TPEngStreamContent content = static_cast< TPEngStreamContent >( aStream.ReadInt8L() );

    if ( content == EPEngHasField )
        {
        TUint32 value = aStream.ReadUint32L();
        integer = new ( ELeave ) TUint32( value );
        }

    delete aInteger;
    aInteger = integer;
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::DoBufferExternalizeSizeInBytes()
// Static helper.
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::DoBufferExternalizeSizeInBytes( const HBufC* aBuffer )
    {
    TInt size = 1; // 1 byte for content mark

    if ( aBuffer )
        {
        size = size + aBuffer->Size() + 4; // max four bytes for buffer length
        }

    return size;
    }


// -----------------------------------------------------------------------------
// CPEngDetailedResultEntry::DoIntegerExternalizeSizeInBytes()
// Static helper.
// -----------------------------------------------------------------------------
//
TInt CPEngDetailedResultEntry::DoIntegerExternalizeSizeInBytes( TUint32* aInteger )
    {
    TInt size = 1; // 1 byte for content mark

    if ( aInteger )
        {
        size = size + 4; // four bytes for integer
        }

    return size;
    }



//  End of File


