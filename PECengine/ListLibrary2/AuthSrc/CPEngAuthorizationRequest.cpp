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
* Description:  Container of the one reactive authorization request
*
*/

// INCLUDE FILES
#include    "CPEngAuthorizationRequest.h"
#include 	"MPEngAuthorizationRequest.h"
#include 	"MPEngAuthorizationManagerSet.h"

#include    <e32std.h>
#include    <s32strm.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::CPEngAuthorizationRequest()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationRequest::CPEngAuthorizationRequest( TInt& aSize )
        : CPEngAuthorizationItem( aSize ),
        iState( EPEngAuthPending ),
        iResponseType( MPEngAuthorizationRequest::EPEngAuthUndefined )
    {
    // state, response type, 2 array count, 4bytes each
    iSize += 16;
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::ConstructL( const TDesC& aUserId )
    {
    CPEngAuthorizationItem::ConstructL( aUserId );
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationRequest* CPEngAuthorizationRequest::NewLC( const TDesC& aUserId,
                                                             TInt& aSize )
    {
    CPEngAuthorizationRequest* self = new( ELeave )
    CPEngAuthorizationRequest( aSize );

    CleanupStack::PushL( self );
    self->ConstructL( aUserId );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationRequest* CPEngAuthorizationRequest::NewLC( RReadStream& aStream,
                                                             TInt& aSize )
    {
    CPEngAuthorizationRequest* self = new( ELeave )
    CPEngAuthorizationRequest( aSize );

    CleanupStack::PushL( self );
    self->InternalizeL( aStream );

    return self;
    }


// Destructor
CPEngAuthorizationRequest::~CPEngAuthorizationRequest()
    {
    // state, response type, 4bytes each
    iSize -= ( 8 + SizeOfArray( iRequestedAttr ) + SizeOfArray( iAuthorizedAttr ) );
    iRequestedAttr.Reset();
    iAuthorizedAttr.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::UpdateLocalFlags()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::DoUpdateLocalFlags(
    const CPEngAuthorizationItem& aSource )
    {
    const CPEngAuthorizationRequest& source =
        static_cast< const CPEngAuthorizationRequest&> ( aSource );
    if ( ( source.AuthorizationState() == AuthorizationState() ) &&
         ( source.AuthorizationResponse() == AuthorizationResponse() ) )
        {
        iLocalFlags = source.LocalFlags();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::UserId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngAuthorizationRequest::UserId() const
    {
    return Id();
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::RequestedAttributes()
// -----------------------------------------------------------------------------
//
const RArray<TUint32>& CPEngAuthorizationRequest::RequestedAttributes() const
    {
    return iRequestedAttr;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::AuthorizationState()
// -----------------------------------------------------------------------------
//
MPEngAuthorizationRequest::TPEngAuthorizationRequestState
CPEngAuthorizationRequest::AuthorizationState() const
    {
    return iState;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::AuthorizationResponse()
// -----------------------------------------------------------------------------
//
MPEngAuthorizationRequest::TPEngAuthorizationResponseType
CPEngAuthorizationRequest::AuthorizationResponse()const
    {
    return iResponseType;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::AuthorizedAttributes()
// -----------------------------------------------------------------------------
//
const RArray<TUint32>& CPEngAuthorizationRequest::AuthorizedAttributes() const
    {
    return iAuthorizedAttr;
    }


// =============================================================================
// ========== Functions for authorization response setting =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::SetAuthorizedAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::SetAuthorizedAttributesL(
    TArray<TUint32>& aAttributes,
    TBool aKeepOldAuthorization /* ETRue */ )
    {
    iAuthManager->SetAuthorizedAttributesL( *this,
                                            aAttributes,
                                            aKeepOldAuthorization );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::DenyAuthorizationL
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::DenyAuthorizationL()
    {
    if ( iState != MPEngAuthorizationRequest::EPEngAuthPending )
        {
        User::Leave( KErrNotReady );
        }
    iAuthManager->DenyAuthorizationL( Id() );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::CancelAuthorizationL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::CancelAuthorizationL()
    {
    if ( iState == MPEngAuthorizationRequest::EPEngAuthPending )
        {
        User::Leave( KErrNotReady );
        }

    iAuthManager->CancelAuthorizationL( Id() );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::RemoveAuthorizationResponse
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::RemoveAuthorizationResponse()
    {
    iAuthManager->RemoveAuthorizationResponse( Id() );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::SetAuthorizationManager()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::SetAuthorizationManager(
    MPEngAuthorizationManagerSet* aAuthManager )
    {
    iAuthManager = aAuthManager;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::SetAuthState()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::SetAuthState(
    TPEngAuthorizationRequestState aState )
    {
    iState = aState;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::SetAuthState()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::SetAuthResponse(
    TPEngAuthorizationResponseType aResponse )
    {
    iResponseType = aResponse;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::UpdateAuthorizedAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::UpdateAuthorizedAttributesL(
    const RArray<TUint32>& aAuthorizedAttr )
    {
    // 4 bytes per each attribute
    iSize -= 4 * iAuthorizedAttr.Count();
    iAuthorizedAttr.Reset();
    iSize += CopyArrayContentL( iAuthorizedAttr, aAuthorizedAttr.Array() );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::UpdateRequestedAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::UpdateRequestedAttributesL(
    const RArray<TUint32>& aRequestedAttr )
    {
    // 4 bytes per each attribute
    iSize -= 4 * iRequestedAttr.Count();
    iRequestedAttr.Reset();
    iSize +=  CopyArrayContentL( iRequestedAttr, aRequestedAttr.Array() );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::AddRequestedAttributeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::AddRequestedAttributeL( TUint32 aAttribute )
    {
    iSize += AddAttributeToArrayL( iRequestedAttr, aAttribute );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::ExternalizeL( RWriteStream& aStream ) const
    {
    // base class
    CPEngAuthorizationItem::ExternalizeL( aStream );

    // state, response type
    aStream.WriteInt32L( iState );
    aStream.WriteInt32L( iResponseType );

    ExternalizeArrayL( aStream, iRequestedAttr );
    ExternalizeArrayL( aStream, iAuthorizedAttr );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::ExternalizeCachedL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRequest::InternalizeL( RReadStream& aStream )
    {
    // base class
    CPEngAuthorizationItem::InternalizeL( aStream );

    // state, response type
    iState =
        static_cast<TPEngAuthorizationRequestState>( aStream.ReadInt32L() );

    iResponseType =
        static_cast<TPEngAuthorizationResponseType>( aStream.ReadInt32L() );

    InternalizeArrayL( aStream, iRequestedAttr, iSize );
    InternalizeArrayL( aStream, iAuthorizedAttr, iSize );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRequest::Compare()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationRequest::Compare( const CPEngAuthorizationRequest& aFirst,
                                         const CPEngAuthorizationRequest& aSecond )
    {
    return aFirst.Id().CompareF( aSecond.Id() );
    }


//  End of File
