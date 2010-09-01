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
* Description:  Container of one reactive authorization response
*
*/

// INCLUDE FILES
#include    <e32std.h>
#include    <s32strm.h>
#include    "CPEngAuthorizationResponse.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::CPEngAuthorizationResponse()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationResponse::CPEngAuthorizationResponse( TInt& aSize )
        : CPEngAuthorizationItem( aSize )
    {
    //state, attributes count, 4 bytes per each
    iSize += 8;
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationResponse::ConstructL( const TDesC& aUserId )
    {
    CPEngAuthorizationItem::ConstructL( aUserId );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationResponse* CPEngAuthorizationResponse::NewLC( const TDesC& aUserId,
                                                               TInt& aSize )
    {
    CPEngAuthorizationResponse* self =
        new ( ELeave ) CPEngAuthorizationResponse( aSize );

    CleanupStack::PushL( self );
    self->ConstructL( aUserId );

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationResponse* CPEngAuthorizationResponse::NewLC( RReadStream& aStream,
                                                               TInt& aSize )
    {
    CPEngAuthorizationResponse* self =
        new( ELeave ) CPEngAuthorizationResponse( aSize );

    CleanupStack::PushL( self );
    self->InternalizeL( aStream );

    return self;
    }


// Destructor
CPEngAuthorizationResponse::~CPEngAuthorizationResponse()
    {
    // 4 bytes per number: State
    iSize -= ( 4 + SizeOfArray( iAttributes ) );
    iAttributes.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::UpdateLocalFlags()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationResponse::DoUpdateLocalFlags(
    const CPEngAuthorizationItem& aSource )
    {
    const CPEngAuthorizationResponse& source =
        static_cast< const CPEngAuthorizationResponse&> ( aSource );

    if ( ( source.AuthorizationStatus() == AuthorizationStatus() )
         && ( source.AuthorizedAttributes().Count() == iAttributes.Count() ) )
        {
        iLocalFlags = source.LocalFlags();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::UserId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngAuthorizationResponse::UserId() const
    {
    return Id();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::AuthorizationStatus()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationResponse::AuthorizationStatus() const
    {
    return iResponseType;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::AuthorizedAttributes()
// -----------------------------------------------------------------------------
//
const RArray<TUint32>& CPEngAuthorizationResponse::AuthorizedAttributes() const
    {
    return iAttributes;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::SetResponseType()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationResponse::SetResponseType( TInt aResponseType )
    {
    iResponseType = aResponseType;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::SetAttributesToAuthorizeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationResponse::SetAttributesToAuthorizeL(
    const TArray<TUint32>& aAttributes,
    const RArray<TUint32>* aAuthorizedAttributes )
    {
    // 4 bytes per each attribute
    iSize -= ( 4 * iAttributes.Count() );
    iAttributes.Reset();
    iSize += CopyArrayContentL( iAttributes, aAttributes );
    // shall already authorized attributes be included
    if ( aAuthorizedAttributes )
        {
        iSize += CopyArrayContentL( iAttributes, aAuthorizedAttributes->Array() );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::AddAttributeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationResponse::AddAttributeL( TUint32 aAttribute )
    {
    AddAttributeToArrayL( iAttributes, aAttribute );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationResponse::ExternalizeL( RWriteStream& aStream ) const
    {
    // base class
    CPEngAuthorizationItem::ExternalizeL( aStream );

    // response type
    aStream.WriteInt32L( iResponseType );

    // Attributes
    ExternalizeArrayL( aStream, iAttributes );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::ExternalizeCachedL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationResponse::InternalizeL( RReadStream& aStream )
    {
    // base class
    CPEngAuthorizationItem::InternalizeL( aStream );

    // response type
    iResponseType = aStream.ReadInt32L();

    // Attributes
    InternalizeArrayL( aStream, iAttributes, iSize );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationResponse::Compare()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationResponse::Compare( const CPEngAuthorizationResponse& aFirst,
                                          const CPEngAuthorizationResponse& aSecond )
    {
    return aFirst.Id().CompareF( aSecond.Id() ) ;
    }


//  End of File
