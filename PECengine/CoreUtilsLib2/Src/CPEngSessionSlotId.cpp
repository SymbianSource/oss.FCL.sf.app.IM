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
* Description:  NWSessionSlotID implementation.
*
*/

// INCLUDE FILES
#include    "CPEngSessionSlotId.h"
#include    "PEngCoreUtilsTools.h"

// CONSTANTS


// ============================ LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CopyBufferL()
// -----------------------------------------------------------------------------
void CopyBufferL( HBufC*& aDest, const HBufC* aSource )
    {
    if ( aSource )
        {
        aDest = aSource->AllocL();
        }
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSessionSlotId::CPEngSessionSlotId
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSessionSlotId::CPEngSessionSlotId()
    {
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotId::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotId* CPEngSessionSlotId::NewL()
    {
    CPEngSessionSlotId* self = new ( ELeave ) CPEngSessionSlotId();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotId::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotId* CPEngSessionSlotId::NewLC()
    {
    CPEngSessionSlotId* self = new ( ELeave ) CPEngSessionSlotId();
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPEngSessionSlotId::~CPEngSessionSlotId()
    {
    delete iServiceAddress;
    delete iUserId;
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotId::CloneL()
// Two-phased clone constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotId* CPEngSessionSlotId::CloneL() const
    {
    CPEngSessionSlotId* clone = CloneLC();
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotId::CloneLC()
// Two-phased clone constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotId* CPEngSessionSlotId::CloneLC() const
    {
    CPEngSessionSlotId* clone = new ( ELeave ) CPEngSessionSlotId();
    CleanupStack::PushL( clone );

    clone->CopyL( *this );

    return clone;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotId::CopyL()
// Copy operation.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotId::CopyL( const CPEngSessionSlotId& aSource )
    {
    CopyBufferL( iServiceAddress, aSource.iServiceAddress );
    CopyBufferL( iUserId, aSource.iUserId );
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotId::Reset()
// Reset operation.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotId::Reset()
    {
    delete iServiceAddress;
    delete iUserId;

    iServiceAddress = NULL;
    iUserId = NULL;
    }



// -----------------------------------------------------------------------------
// CPEngSessionSlotId::SetServiceAddressL()
// Mutator
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotId::SetServiceAddressL( const TDesC& aAddress )
    {
    DoSetDescriptorL( iServiceAddress, aAddress );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotId::SetUserIdL()
// Mutator
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotId::SetUserIdL( const TDesC& aUserId )
    {
    DoSetDescriptorL( iUserId, aUserId );
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotId::ServiceAddress()
// Accessor
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CPEngSessionSlotId::ServiceAddress() const
    {
    return DoGetDescriptor( iServiceAddress );
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotId::UserId()
// Accessor
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CPEngSessionSlotId::UserId() const
    {
    return DoGetDescriptor( iUserId );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotId::Match()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSessionSlotId::Match( const CPEngSessionSlotId& aIdToMatch ) const
    {
    //Use ServiceAddress() and UserId() accessors to avoid NULL pointer problems
    if ( aIdToMatch.ServiceAddress().Compare( ServiceAddress() ) != KErrNone )
        {
        return KErrNotFound;
        }

    if ( aIdToMatch.UserId().Compare( UserId() ) != KErrNone )
        {
        return KErrNotFound;
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotId::PackLC()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPEngSessionSlotId::PackLC() const
    {
    HBufC8* packBuffer = HBufC8::NewLC( Size() );
    TPtr8 pack( packBuffer->Des() );

    RDesWriteStream ws;
    ws.Open( pack );                        // CSI: 65 #
    CleanupClosePushL( ws );

    ExternalizeL( ws );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws


    packBuffer = packBuffer->ReAllocL( packBuffer->Length() );
    CleanupStack::Pop();
    CleanupStack::PushL( packBuffer ); //Due realloc

    return packBuffer;
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotId::UnPackFullL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotId::UnPackL( const TDesC8& aPack )
    {
    RDesReadStream rs;
    rs.Open( aPack );                       // CSI: 65 #
    CleanupClosePushL( rs );

    InternalizeL( rs );

    CleanupStack::PopAndDestroy(); //rs
    }



// -----------------------------------------------------------------------------
// CPEngSessionSlotId::ExternalizeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotId::ExternalizeL( RWriteStream& aStream ) const
    {
    ExternalizeBufferL( iServiceAddress, aStream );
    ExternalizeBufferL( iUserId, aStream );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotId::InternalizeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotId::InternalizeL( RReadStream& aStream )
    {
    delete iServiceAddress;
    iServiceAddress = NULL;
    iServiceAddress = InternalizeBufferL( aStream );


    delete iUserId;
    iUserId = NULL;
    iUserId = InternalizeBufferL( aStream );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotId::Size()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSessionSlotId::Size() const
    {
    TInt size = BufferExternalizeSizeInBytes( iServiceAddress );
    size += BufferExternalizeSizeInBytes( iUserId );
    return size;
    }


// End of File


