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
* Description:  Data entry implementation.
*
*/

// INCLUDE FILES
#include <E32Base.h>
#include <S32Strm.h>
#include "CPEngDataResultEntry.h"



// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPEngDataResultEntry::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngDataResultEntry* CPEngDataResultEntry::NewLC( TInt aOperationID,
                                                   HBufC* aData )
    {
    CPEngDataResultEntry* self = new ( ELeave ) CPEngDataResultEntry();
    CleanupStack::PushL( self );

    //now owns the parameter data
    self->iData = aData;
    self->iOperationID = aOperationID;

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngDataResultEntry::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngDataResultEntry* CPEngDataResultEntry::NewLC( RReadStream& aStream )
    {
    CPEngDataResultEntry* self = new ( ELeave ) CPEngDataResultEntry();
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;
    }



// -----------------------------------------------------------------------------
// CPEngDataResultEntry::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngDataResultEntry* CPEngDataResultEntry::NewLC( const CPEngDataResultEntry& aEntry )
    {
    CPEngDataResultEntry* self = new ( ELeave ) CPEngDataResultEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    return self;
    }


// Destructor
CPEngDataResultEntry::~CPEngDataResultEntry()
    {
    delete iData;
    }


// -----------------------------------------------------------------------------
// CPEngDataResultEntry::CPEngDataResultEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngDataResultEntry::CPEngDataResultEntry()
    {
    }


// -----------------------------------------------------------------------------
// CPEngDataResultEntry::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngDataResultEntry::ConstructL( const CPEngDataResultEntry& aEntry )
    {
    iOperationID = aEntry.iOperationID;
    iData = aEntry.iData->AllocL();
    }


// -----------------------------------------------------------------------------
// CPEngDataResultEntry::Operation()
// -----------------------------------------------------------------------------
//
TInt CPEngDataResultEntry::Operation() const
    {
    return iOperationID;
    }


// -----------------------------------------------------------------------------
// CPEngDataResultEntry::Data()
// -----------------------------------------------------------------------------
//
const TDesC16& CPEngDataResultEntry::Data() const
    {
    return *iData;
    }


// -----------------------------------------------------------------------------
// CPEngDataResultEntry::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngDataResultEntry::InternalizeL( RReadStream& aStream )
    {
    iOperationID = aStream.ReadInt32L();

    TInt length = aStream.ReadInt32L();
    HBufC16* buffer = HBufC::NewLC( length );
    TPtr ptr( buffer->Des() );
    aStream.ReadL( ptr, length );
    CleanupStack::Pop(); //buffer

    delete iData;
    iData = buffer;
    }


// -----------------------------------------------------------------------------
// CPEngDataResultEntry::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngDataResultEntry::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iOperationID );
    aStream.WriteInt32L( iData->Length() );
    aStream.WriteL( *iData );
    }


// -----------------------------------------------------------------------------
// CPEngDataResultEntry::ExternalizeSize()
// -----------------------------------------------------------------------------
//
TInt CPEngDataResultEntry::ExternalizeSize() const
    {
    TInt size = 8; //operation: 4 bytes
    //buffer length: 4 bytes
    size += iData->Size();
    return size;
    }


//  End of File


