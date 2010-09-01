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
* Description:  Reactive authorization engine base class.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "CPEngAuthorizationEngine.h"
#include "CPEngAuthorizationResponse.h"
#include "CPEngAuthorizationRequest.h"
#include "CPEngAuthorizationItem.h"

#include "PEngStorageGlobals.h"
#include "MPEngStorageManager.h"
#include "PEngStorageManager.h"
#include "CPEngSessionSlotId.h"
#include "RObjectArray.h"
#include "PresenceDebugPrint.h"



#define ARRAY_ORDER_REQ TLinearOrder<CPEngAuthorizationRequest> (CPEngAuthorizationRequest::Compare)
#define ARRAY_ORDER_STAT TLinearOrder<CPEngAuthorizationResponse> (CPEngAuthorizationResponse::Compare)


_LIT( KPEngAuthorizationsSId, "PEngAuthorizations" );

// Init size for the store enry fo the authorization engine
// 2x4 bytes for two array counts
static const TInt KInitSizeOfStoreEntry = 8;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::CPEngAuthorizationEngine()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationEngine::CPEngAuthorizationEngine( MPEngListLibFactory& aFactory )
        : CPEngStoreEntry( EPEngMixedCachedVersionCon ),
        iFactory( aFactory ),
        iAccessCount( 1 ) // init ref count on 1
    {
    iSize = KInitSizeOfStoreEntry;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::ConstructL( const CPEngSessionSlotId& aSessionSlot )
    {
    iSessionId = aSessionSlot.CloneL();

    MPEngStorageManager* storageManager =
        PEngStorageManager::GetStorageManagerL( *iSessionId );
    CleanupClosePushL( *storageManager );

    CPEngStoreEntry::BaseConstructL( *storageManager );

    CleanupStack::PopAndDestroy(); //storageManager

    iStorageManager->RetrieveL( *this );
    }


// Destructor
CPEngAuthorizationEngine::~CPEngAuthorizationEngine()
    {
    iAuthRequests.ResetAndDestroy();
    iAuthStatuses.ResetAndDestroy();
    delete iSessionId;
    }



// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::SessionId()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngAuthorizationEngine::SessionId() const
    {
    return *iSessionId;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::Open()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::Open()
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::UpdateAuthorizationStateL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::UpdateAuthorizationStateL(
    CPEngAuthorizationResponse& aAuthResponse,
    TBool aPartialAcceptance )
    {
    TInt index( FindUserIdItem( iAuthRequests, aAuthResponse.Id() ) );
    CPEngAuthorizationRequest* authRq = NULL;

    if ( index == KErrNotFound )
        {
        CPEngAuthorizationRequest* authRq = CPEngAuthorizationRequest::NewLC(
                                                aAuthResponse.Id(),
                                                iSize );

        iAuthRequests.InsertInOrderL( authRq, ARRAY_ORDER_REQ );
        CleanupStack::Pop();
        }
    else
        {
        authRq = iAuthRequests[ index ];
        }


    // update authorization
    authRq->SetAuthResponse(
        static_cast<MPEngAuthorizationRequest::TPEngAuthorizationResponseType>
        ( aAuthResponse.AuthorizationStatus()  ) );

    authRq->SetAuthState( MPEngAuthorizationRequest::EPEngAuthAnswered );

    // copy authorized attributes
    if ( aPartialAcceptance )
        {
        authRq->UpdateAuthorizedAttributesL( aAuthResponse.AuthorizedAttributes() );
        }
    else
        {
        authRq->UpdateAuthorizedAttributesL( authRq->RequestedAttributes() );
        }

    StoreL();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::RemoveAuthorizationRequestL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::RemoveAuthorizationRequestL(
    CPEngAuthorizationResponse& aAuthResponse )
    {
    TInt index( FindUserIdItem( iAuthRequests, aAuthResponse.Id() ) );
    if ( index < 0 )
        {
        return;
        }
    delete iAuthRequests[ index ];
    iAuthRequests.Remove( index );

    StoreL();
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AddAuthRequestLX()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::AddAuthRequestLX(
    CPEngAuthorizationRequest* aAuthRequest )
    {
    TInt err( iAuthRequests.InsertInOrder( aAuthRequest, ARRAY_ORDER_REQ ) );
    if ( err == KErrNone )
        {
        CleanupStack::Pop( aAuthRequest );
        }

    if ( err == KErrAlreadyExists )
        {
        // it is there already, update set of requested attributes
        TInt index( FindUserIdItem( iAuthRequests, aAuthRequest->Id() ) );
        iAuthRequests[ index ]->UpdateRequestedAttributesL(
            aAuthRequest->RequestedAttributes() );
        CleanupStack::PopAndDestroy( aAuthRequest );
        err = KErrNone;
        }

    User::LeaveIfError( err );
    CPEngStoreEntry::StoreL();
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AddAuthStatusLX()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::AddAuthStatusLX(
    CPEngAuthorizationResponse* aAuthStatus )
    {
    TInt err( iAuthStatuses.InsertInOrder( aAuthStatus, ARRAY_ORDER_STAT ) );
    if ( err == KErrNone )
        {
        CleanupStack::Pop( aAuthStatus ); // aAuthRequest
        }
    if ( err == KErrAlreadyExists )
        {
        // it is there already, update set of requested attributes
        TInt index( FindUserIdItem( iAuthStatuses, aAuthStatus->Id() ) );

        iAuthStatuses[ index ]->SetAttributesToAuthorizeL(
            aAuthStatus->AuthorizedAttributes().Array(),
            NULL );

        CleanupStack::PopAndDestroy( aAuthStatus ); // aAuthRequest
        err = KErrNone;
        }

    User::LeaveIfError( err );
    CPEngStoreEntry::StoreL();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::SizeCounter()
// -----------------------------------------------------------------------------
//
TInt& CPEngAuthorizationEngine::SizeCounter()
    {
    return iSize;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::LockForNetworkPublishing()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::LockForNetworkPublishing()
    {
    CPEngStoreEntry::Lock( EStorageLockLevelHigh );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::UnLockForNetworkPublishing()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::UnLockForNetworkPublishing()
    {
    CPEngStoreEntry::Unlock();
    }



// -----------------------------------------------------------------------------
// CPEngContactListMainArray::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::ExternalizeL( RWriteStream& aStream,
                                             TPEngStorageType aStorageType ) const
    {
    switch ( aStorageType )
        {
        case EPEngStorageBasicCached:
            {
            TInt count( iAuthRequests.Count() );
            aStream.WriteInt32L( count );
            for ( TInt x( 0 ) ; x < count ; ++x )
                {
                iAuthRequests[ x ]->ExternalizeL( aStream );
                }

            count = iAuthStatuses.Count();
            aStream.WriteInt32L( count );
            for ( TInt y( 0 ) ; y < count ; ++y )
                {
                iAuthStatuses[ y ]->ExternalizeL( aStream );
                }
            break;
            }


        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::InternalizeL( RReadStream& aStream,
                                             TPEngStorageType aStorageType )
    {
    switch ( aStorageType )
        {
        case EPEngStorageBasicCached:
            {
            // authorization requests
            RObjectArray<CPEngAuthorizationRequest> authRequests;
            CleanupClosePushL( authRequests );

            TInt count( aStream.ReadInt32L() );
            for ( TInt x( 0 ) ; x < count ; ++x )
                {
                CPEngAuthorizationRequest* authRq =
                    CPEngAuthorizationRequest::NewLC( aStream,
                                                      iSize );
                authRequests.AppendL( authRq );
                CleanupStack::Pop( authRq ); // authRq
                // adopt flag and clean array
                AdoptAndClean( iAuthRequests, *authRq );
                }
            AppendRemoveL( iAuthRequests, authRequests );
            CleanupStack::PopAndDestroy(); // authRequests


            // authorization statuses
            RObjectArray<CPEngAuthorizationResponse> authStatuses;
            CleanupClosePushL( authStatuses );

            count = aStream.ReadInt32L();
            for ( TInt y( 0 ) ; y < count ; ++y )
                {
                CPEngAuthorizationResponse* authRes =
                    CPEngAuthorizationResponse::NewLC( aStream,
                                                       iSize );
                authStatuses.AppendL( authRes );
                CleanupStack::Pop( authRes ); // authRes
                // adopt flag and clean array
                AdoptAndClean( iAuthStatuses, *authRes );
                }
            AppendRemoveL( iAuthStatuses, authStatuses );
            CleanupStack::PopAndDestroy(); // authStatuses
            break;
            }

        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::StorageId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngAuthorizationEngine::StorageId() const
    {
    return KPEngAuthorizationsSId;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::EntrySize()
// -----------------------------------------------------------------------------
//
TUint32 CPEngAuthorizationEngine::EntrySize() const
    {
    return iSize;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::HandleSIDsChangeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::HandleSIDsChangeL( CPtrCArray& /* aChangedSIDs */ )
    {
    iStorageManager->RetrieveL( *this );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::HandleSIDNotifyError()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationEngine::HandleSIDNotifyError( TInt /* aError */ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AuthorizationRequestCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationEngine::AuthorizationRequestCount() const
    {
    return iAuthRequests.Count();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AuthorizationRequest()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationRequest& CPEngAuthorizationEngine::AuthorizationRequest(
    TInt aIndex )
    {
    return *iAuthRequests[ aIndex ];
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AuthorizationStatusCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationEngine::AuthorizationStatusCount() const
    {
    return iAuthStatuses.Count();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AuthorizationStatus()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationResponse& CPEngAuthorizationEngine::AuthorizationStatus(
    TInt aIndex )
    {
    return * iAuthStatuses[ aIndex ];
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AuthorizationRequestL()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationRequest& CPEngAuthorizationEngine::AuthorizationRequestL(
    const TDesC& aUserId )
    {
    TInt index ( FindUserIdItem( iAuthRequests, aUserId ) );
    User::LeaveIfError( index );
    return *iAuthRequests[ index ];
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::FindAuthResponse()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationEngine::FindAuthResponse(
    const RPointerArray<CPEngAuthorizationResponse>& aArray,
    const TDesC& aUserId  )
    {
    return FindUserIdItem( aArray, aUserId );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::FindUserIdItem()
// -----------------------------------------------------------------------------
//
template <class T>
TInt CPEngAuthorizationEngine::FindUserIdItem( const RPointerArray<T>& aArray,
                                               const TDesC& aUserId )
    {
    TInt l( 0 );
    TInt r( aArray.Count() );
    while ( r > l )
        {
        TInt inx = ( l + r ) >> 1;

        TInt k ( aUserId.CompareF( aArray[ inx ]->Id() ) );
        if ( k == 0 )
            {
            return inx;
            }
        else if ( k > 0 )
            l = inx + 1;
        else
            r = inx;
        }
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AdoptAndClean()
// -----------------------------------------------------------------------------
//
template <class T>
void CPEngAuthorizationEngine::AdoptAndClean( RPointerArray<T>& aArray,
                                              T& aNewItem )
    {
    TInt count( aArray.Count() );
    while ( count != 0 )
        {
        TInt err( aArray[ 0 ]->Id().CompareF( aNewItem.Id() ) );
        if ( err == KErrNone )
            {
            aNewItem.DoUpdateLocalFlags( *( aArray[ 0 ] ) );
            delete aArray[ 0 ];
            aArray.Remove( 0 );
            return;
            }

        if ( err > 0 ) // no sense to loop furder in the array
            {
            return;
            }

        // clean array
        delete aArray[ 0 ];
        aArray.Remove( 0 );
        count--;
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationEngine::AppendRemoveL()
// -----------------------------------------------------------------------------
//
template <class T>
void CPEngAuthorizationEngine::AppendRemoveL( RPointerArray<T>& aTarget,
                                              RPointerArray<T>& aSource )
    {
    aTarget.ResetAndDestroy();
    TInt count( aSource.Count() );
    while ( count != 0 )
        {
        aTarget.AppendL( aSource[ 0 ] );
        aSource.Remove( 0 );
        count--;
        }
    }

//  End of File











