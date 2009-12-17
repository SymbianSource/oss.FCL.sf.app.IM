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
* Description:  NWSessionSlotID implementation.
*
*/

// INCLUDE FILES
#include    "CPEngNWSessionSlotID2Imp.h"
#include    "CPEngSessionSlotId.h"
#include    <s32strm.h>
#include    <s32mem.h>


// ============================ LOCAL FUNCTIONS ===============================
namespace
    {
    // -----------------------------------------------------------------------------
    // ValueForMatchMode()
    // -----------------------------------------------------------------------------
    //
    const TDesC& ValueForMatchMode( const TDesC& aReqValue,
                                    TPEngNWSSIDMatchMode aMatchMode )
        {
        if ( aMatchMode == EPEngMMWildAny )
            {
            return KPEngMatchSymbolWildAny;
            }

        return aReqValue;
        }

    // -----------------------------------------------------------------------------
    // ExternalizeBufferL()
    // -----------------------------------------------------------------------------
    //
    void ExternalizeBufferL( const HBufC* aBuffer, RWriteStream& aStream )
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
    // -----------------------------------------------------------------------------
    //
    HBufC* InternalizeBufferL( RReadStream& aStream )
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
    // -----------------------------------------------------------------------------
    //
    TInt BufferExternalizeSizeInBytes( const HBufC* aBuffer )
        {
        TInt size( 4 ); //Buffer length indicator: 4 bytes

        if ( aBuffer && aBuffer->Length() != 0 )
            {
            size += aBuffer->Size();
            }

        return size;
        }
    }


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2Imp* CPEngNWSessionSlotID2Imp::NewL()
    {
    CPEngNWSessionSlotID2Imp* self = new ( ELeave ) CPEngNWSessionSlotID2Imp();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::CloneL()
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2Imp* CPEngNWSessionSlotID2Imp::CloneL() const
    {
    CPEngNWSessionSlotID2Imp* clone = CPEngNWSessionSlotID2Imp::NewL();
    CleanupStack::PushL( clone );

    clone->iBasePart->CopyL( *iBasePart );

    if ( iAppId )
        {
        clone->iAppId = iAppId->AllocL();
        }

    clone->iServiceAddressMatchMode = iServiceAddressMatchMode;
    clone->iUserIdMatchMode = iUserIdMatchMode;
    clone->iAppIdMatchMode = iAppIdMatchMode;

    CleanupStack::Pop( clone );
    return clone;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::Reset()
// Reset operation.
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::Reset()
    {
    iBasePart->Reset();

    delete iAppId;
    iAppId = NULL;

    iServiceAddressMatchMode = EPEngMMExact;
    iUserIdMatchMode = EPEngMMExact;
    iAppIdMatchMode = EPEngMMExact;
    }


// Destructor
CPEngNWSessionSlotID2Imp::~CPEngNWSessionSlotID2Imp()
    {
    delete iBasePart;
    delete iAppId;
    }




// -----------------------------------------------------------------------------
// Symbian OS constructor.
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::ConstructL()
    {
    iBasePart = CPEngSessionSlotId::NewL();
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::CPEngNWSessionSlotID2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2Imp::CPEngNWSessionSlotID2Imp()
        : iServiceAddressMatchMode( EPEngMMExact ),
        iUserIdMatchMode( EPEngMMExact ),
        iAppIdMatchMode( EPEngMMExact )
    {
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::SetServiceAddressL()
// Mutator
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::SetServiceAddressL( const TDesC& aAddress,
                                                   TPEngNWSSIDMatchMode aMatchMode )
    {
    iBasePart->SetServiceAddressL( ValueForMatchMode( aAddress, aMatchMode ) );
    iServiceAddressMatchMode = aMatchMode;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::SetUserIdL()
// Mutator
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::SetUserIdL( const TDesC& aUserId,
                                           TPEngNWSSIDMatchMode aMatchMode )
    {
    iBasePart->SetUserIdL( ValueForMatchMode( aUserId, aMatchMode ) );
    iUserIdMatchMode = aMatchMode;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::SetAppIdL()
// Mutator
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::SetAppIdL( const TDesC& aAppId,
                                          TPEngNWSSIDMatchMode aMatchMode )
    {
    HBufC* tmpBuf = ValueForMatchMode( aAppId, aMatchMode ).AllocL();
    delete iAppId;
    iAppId = tmpBuf;
    iAppIdMatchMode = aMatchMode;
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::SetAllL()
// Mutator
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::SetAllL( const CPEngSessionSlotId& aBasePart,
                                        const TDesC& aAppId )
    {
    SetServiceAddressL( aBasePart.ServiceAddress(), EPEngMMExact );
    SetUserIdL( aBasePart.UserId(), EPEngMMExact );
    SetAppIdL( aAppId, EPEngMMExact );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::ServiceAddress()
// Accessor
// -----------------------------------------------------------------------------
//
TPtrC CPEngNWSessionSlotID2Imp::ServiceAddress() const
    {
    return iBasePart->ServiceAddress();
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::UserId()
// Accessor
// -----------------------------------------------------------------------------
//
TPtrC CPEngNWSessionSlotID2Imp::UserId() const
    {
    return iBasePart->UserId();
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::AppId()
// Accessor
// -----------------------------------------------------------------------------
//
TPtrC CPEngNWSessionSlotID2Imp::AppId() const
    {
    if ( iAppId )
        {
        return *iAppId;
        }

    return KNullDesC();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::BasePart()
// Accessor
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngNWSessionSlotID2Imp::BasePart() const
    {
    return *iBasePart;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::IsServiceAddressWild()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlotID2Imp::IsServiceAddressWild() const
    {
    return ( iServiceAddressMatchMode != EPEngMMExact );
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::IsUserIdWild()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlotID2Imp::IsUserIdWild() const
    {
    return ( iUserIdMatchMode != EPEngMMExact );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::IsAppIdWild()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlotID2Imp::IsAppIdWild() const
    {
    return ( iAppIdMatchMode != EPEngMMExact );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::IsWild()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlotID2Imp::IsWild() const
    {
    return ( IsServiceAddressWild() || IsUserIdWild() || IsAppIdWild() );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::IsFullyDefined()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlotID2Imp::IsFullyDefined() const
    {
    if ( IsWild() )
        {
        return EFalse;
        }


    if ( ServiceAddress().Length() == 0 )
        {
        return EFalse;
        }


    if ( UserId().Length() == 0 )
        {
        return EFalse;
        }


    if ( AppId().Length() == 0 )
        {
        return EFalse;
        }


    return ETrue;
    }






// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::MatchFullId()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotID2Imp::MatchFullId( const CPEngNWSessionSlotID2Imp& aIdToMatch ) const
    {
    if ( MatchBasePart( aIdToMatch ) != KErrNone )
        {
        return KErrNotFound;
        }

    if ( MatchAppIdPart( aIdToMatch ) != KErrNone )
        {
        return KErrNotFound;
        }

    return KErrNone;
    }




// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::MatchBasePart()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotID2Imp::MatchBasePart(
    const CPEngNWSessionSlotID2Imp& aIdToMatch ) const
    {
    //In case of full ID container, the wild definitions can be
    //in either self object or in parameter one
    //==> must be matched both ways

    if ( MatchBasePart( aIdToMatch.BasePart() ) == KErrNone )
        {
        return KErrNone;
        }

    if ( aIdToMatch.MatchBasePart( BasePart() ) == KErrNone )
        {
        return KErrNone;
        }

    return KErrNotFound;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::MatchBasePart()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotID2Imp::MatchBasePart(
    const CPEngSessionSlotId& aBasePartToMatch ) const
    {
    //Both base part fragments must match

    //Check first serviceAddress
    TBool serviceAddressMatch = EFalse;
    if ( IsServiceAddressWild() )
        {
        serviceAddressMatch = ETrue;
        }

    else if ( ServiceAddress().Compare( aBasePartToMatch.ServiceAddress() )
              == KErrNone )
        {
        serviceAddressMatch = ETrue;
        }


    //And then user ID
    TBool userIdMatch = EFalse;
    if ( IsUserIdWild() )
        {
        userIdMatch = ETrue;
        }

    else if ( UserId().Compare( aBasePartToMatch.UserId() ) == KErrNone )
        {
        userIdMatch = ETrue;
        }


    if ( serviceAddressMatch && userIdMatch )
        {
        return KErrNone;
        }
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::MatchAppIdPart()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotID2Imp::MatchAppIdPart(
    const CPEngNWSessionSlotID2Imp& aIdToMatch ) const
    {
    //In case of full ID container, the wild definitions can be
    //in either self object or in parameter one
    //==> must be matched both ways
    if ( MatchAppIdPart( aIdToMatch.AppId() ) == KErrNone )
        {
        return KErrNone;
        }

    if ( aIdToMatch.MatchAppIdPart( AppId() ) == KErrNone )
        {
        return KErrNone;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::MatchAppIdPart()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlotID2Imp::MatchAppIdPart( const TDesC& aAppId ) const
    {
    if ( IsAppIdWild() )
        {
        return KErrNone;
        }

    if ( AppId().Compare( aAppId ) == KErrNone )
        {
        return KErrNone;
        }

    return KErrNotFound;
    }




// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::PackFullLC()
// -----------------------------------------------------------------------------
//
HBufC8* CPEngNWSessionSlotID2Imp::PackFullLC() const
    {
    TInt size = 6 * 4; //6 static TInts
    //   - Version
    //   - 1 x buffer length
    //   - 3 x matchMode
    //   - Extension length

    size += iBasePart->Size();
    size += BufferExternalizeSizeInBytes( iAppId );

    HBufC8* packBuffer = HBufC8::NewLC( size );
    TPtr8 pack( packBuffer->Des() );

    RDesWriteStream ws;
    ws.Open( pack );                // CSI: 65 #
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
// CPEngNWSessionSlotID2Imp::UnPackFullL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::UnPackFullL( const TDesC8& aPack )
    {
    RDesReadStream rs;
    rs.Open( aPack );               // CSI: 65 #
    CleanupClosePushL( rs );

    InternalizeL( rs );

    CleanupStack::PopAndDestroy(); //rs
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( 0 );   //Version, currently first

    iBasePart->ExternalizeL( aStream );
    aStream.WriteInt32L( iServiceAddressMatchMode );
    aStream.WriteInt32L( iUserIdMatchMode );

    ExternalizeBufferL( iAppId, aStream );
    aStream.WriteInt32L( iAppIdMatchMode );

    aStream.WriteInt32L( 0 ); //Extension length for future, currently 0
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2Imp::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotID2Imp::InternalizeL( RReadStream& aStream )
    {
    aStream.ReadInt32L(); //Stream version

    iBasePart->InternalizeL( aStream );
    iServiceAddressMatchMode = aStream.ReadInt32L();
    iUserIdMatchMode = aStream.ReadInt32L();


    HBufC* tmpBuf = InternalizeBufferL( aStream );
    delete iAppId;
    iAppId = tmpBuf;
    iAppIdMatchMode = aStream.ReadInt32L();

    TInt extensionLength = aStream.ReadInt32L();
    if ( extensionLength > 0 )
        {
        //skip the extension data
        aStream.ReadL( extensionLength );
        }
    }


// End of File


