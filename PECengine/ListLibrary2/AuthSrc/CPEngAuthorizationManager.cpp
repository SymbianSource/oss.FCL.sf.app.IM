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
* Description:  Reactive authorization manager.
*
*/

// INCLUDE FILES
#include "CPEngAuthorizationManager.h"
#include "MPEngAuthorizationObserver.h"

#include "CPEngAuthorizationRequest.h"
#include "CPEngAuthorizationResponse.h"
#include "MPEngListLibFactory.h"

#include "PresenceDebugPrint.h"
#include <S32mem.h>


// MACRO
#define CLEAN_LOCK_CLEANUPSTACK( aPushed )\
    if( aPushed > 1 )\
        {\
        CleanupStack::PopAndDestroy();\
        }\
    if( aPushed == 1 )\
        {\
        CleanupStack::Pop();\
        }


// CONSTANTS
/**
 * Flag used in Authorization item to mark notification done
 */
const TInt32 KAuthNotifyFlag = 0x00001;

/**
 * Base part of the size of the buffer to stream responses
 */
const TInt KBaseSizeForResponse = 20;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::CPEngAuthorizationManager()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationManager::CPEngAuthorizationManager( MPEngListLibFactory& aFactory )
        : CPEngAuthorizationEngine( aFactory )
    {
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::ConstructL( const CPEngSessionSlotId& aSessionId )
    {
    CPEngAuthorizationEngine::ConstructL( aSessionId );
    RebuildSupportingArraysL();
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationManager* CPEngAuthorizationManager::NewLC(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionId )
    {
    CPEngAuthorizationManager* self =
        new( ELeave ) CPEngAuthorizationManager( aFactory );

    CleanupClosePushL( *self );
    self->ConstructL( aSessionId );

    return self;
    }


// Destructor
CPEngAuthorizationManager::~CPEngAuthorizationManager()
    {
    iResponses.ResetAndDestroy();
    iPendingAuth.Reset();
    iDeniedAuth.Reset();
    iObservers.Reset();
    }



// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::Close()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 )
        {
        iFactory.RemoveAuthorizationManager( this );
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::AuthorizationsCount
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationManager::AuthorizationsCount() const
    {
    return AuthorizationRequestCount();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::Authorization()
// -----------------------------------------------------------------------------
//
MPEngAuthorizationRequest& CPEngAuthorizationManager::Authorization( TInt aIndex )
    {
    CPEngAuthorizationRequest& authRq = AuthorizationRequest( aIndex );
    authRq.SetAuthorizationManager( this );
    return authRq;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::PendingAuthorizationsCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationManager::PendingAuthorizationsCount() const
    {
    return iPendingAuth.Count();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::PendingAuthorization()
// -----------------------------------------------------------------------------
//
MPEngAuthorizationRequest& CPEngAuthorizationManager::PendingAuthorization(
    TInt aIndex )
    {
    CPEngAuthorizationRequest& authRq =  *iPendingAuth[ aIndex ];
    authRq.SetAuthorizationManager( this );
    return authRq;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::RespondedAuthorizationsCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationManager::RespondedAuthorizationsCount() const
    {
    return AuthorizationStatusCount();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::RespondedAuthorization()
// -----------------------------------------------------------------------------
//
const MPEngAuthorizationStatus& CPEngAuthorizationManager::RespondedAuthorization(
    TInt aIndex ) const
    {
    return const_cast<CPEngAuthorizationManager*>( this )->AuthorizationStatus( aIndex );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::ServerDeniedAuthorizationsCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAuthorizationManager::ServerDeniedAuthorizationsCount() const
    {
    return iDeniedAuth.Count();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::ServerDeniedAuthorization()
// -----------------------------------------------------------------------------
//
const MPEngAuthorizationStatus&
CPEngAuthorizationManager::ServerDeniedAuthorization( TInt aIndex ) const
    {
    return *( iDeniedAuth[ aIndex ] );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::RemoveAllAuthorizationResponses()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::RemoveAllAuthorizationResponses()
    {
    iResponses.ResetAndDestroy();
    ReleaseLock();
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::BufferedTransactionsLC()
// -----------------------------------------------------------------------------
//
HBufC16* CPEngAuthorizationManager::BufferedTransactionsLC()
    {
    TInt count( iResponses.Count() );
    if ( !count )
        {
        HBufC16* buf = NULL;
        CleanupStack::PushL( buf );
        return NULL;
        }

    // stream responses to the buffer,
    // size iBufRespSize is defined for the 8 bit stream, therefore needs to be
    // divided by 2 for 16 bit buffer.
    HBufC16* buf = HBufC16::NewLC( ( iBufRespSize + KBaseSizeForResponse  ) / 2 );
    TPtr16 bufDes16( buf->Des() );
    // des is accessed as 8bit-> length is double
    TPtr8 bufDes8( ( TUint8* ) bufDes16.Ptr(), ( bufDes16.MaxLength() * 2 ) );

    RDesWriteStream	ws( bufDes8 );
    CleanupClosePushL( ws );

    // externalize
    ws.WriteInt32L( count );
    for ( TInt y( 0 ) ; y < count ; ++y )
        {
        iResponses[ y ]->ExternalizeL( ws );
        }

    // write 32 bit zero, to ensure stream won't be cut by 8/16 bit conversion
    ws.WriteInt32L( 0 );

    // clean stream
    CleanupStack::PopAndDestroy(); // ws
    TInt length( bufDes8.Length() / 2 );
    bufDes16.SetLength( length );
    return buf;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::RegisterAuthorizationObserverL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::RegisterAuthorizationObserverL(
    MPEngAuthorizationObserver& aObserver )
    {
    User::LeaveIfError( iObservers.InsertInAddressOrder( &aObserver ) );

    // if this was first observer, then reset flags to all be notified
    if ( iObservers.Count() == 1 )
        {
        UpdateAllFlags();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::UnRegisterAuthorizationObserver()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::UnRegisterAuthorizationObserver(
    MPEngAuthorizationObserver& aObserver )
    {
    TInt index( iObservers.FindInAddressOrder( &aObserver ) );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::SetAuthorizedAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::SetAuthorizedAttributesL(
    const CPEngAuthorizationRequest& aAuthRequest,
    TArray<TUint32>& aAttributes,
    TBool aKeepOldAuthorization /* ETrue */ )
    {
    TInt pushed( EnableLockLC() );
    CPEngAuthorizationResponse& authResponse =
        AuthorizationResponseL( aAuthRequest.Id() );

    authResponse.SetResponseType( MPEngAuthorizationRequest::EPEngAuthAccepted );
    if ( aKeepOldAuthorization )
        {
        authResponse.SetAttributesToAuthorizeL(
            aAttributes,
            &aAuthRequest.AuthorizedAttributes() );
        }
    else
        {
        authResponse.SetAttributesToAuthorizeL( aAttributes, NULL );
        }

    CLEAN_LOCK_CLEANUPSTACK( pushed );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::DenyAuthorizationL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::DenyAuthorizationL( const TDesC& aUserId )
    {
    TInt pushed( EnableLockLC() );
    CPEngAuthorizationResponse& authResponse = AuthorizationResponseL( aUserId );
    authResponse.SetResponseType( MPEngAuthorizationRequest::EPEngAuthDenied );

    CLEAN_LOCK_CLEANUPSTACK( pushed );
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::CancelAuthorizationL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::CancelAuthorizationL( const TDesC& aUserId )
    {
    TInt pushed( EnableLockLC() );
    CPEngAuthorizationResponse& authResponse = AuthorizationResponseL( aUserId );
    authResponse.SetResponseType( MPEngAuthorizationRequest::EPEngAuthCanceled );

    CLEAN_LOCK_CLEANUPSTACK( pushed );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::RemoveAuthorizationResponse()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::RemoveAuthorizationResponse( const TDesC& aUserId )
    {
    TInt index( FindAuthResponse( iResponses, aUserId ) );
    if ( index == KErrNotFound )
        {
        return;
        }
    delete iResponses[ index  ];
    iResponses.Remove( index );
    if ( iResponses.Count() == 0 )
        {
        // release lock
        ReleaseLock();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::HandleSIDsChangeL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::HandleSIDsChangeL( CPtrCArray& aChangedSIDs )
    {
    CPEngAuthorizationEngine::HandleSIDsChangeL( aChangedSIDs );
    RebuildSupportingArraysL();
    NotifyAllChangesToObservers();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::HandleSIDNotifyError()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::HandleSIDNotifyError( TInt aError )
    {
    CPEngAuthorizationEngine::HandleSIDNotifyError( aError );
    TInt count( iObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iObservers[ x ]->HandleAuthorizationEngineError( aError );
        }
    }



// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::RebuildSupportingArraysL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::RebuildSupportingArraysL()
    {
    iPendingAuth.Reset();
    TInt count( AuthorizationRequestCount() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( AuthorizationRequest( x ).AuthorizationState() ==
             MPEngAuthorizationRequest::EPEngAuthPending )
            {
            iPendingAuth.AppendL( &( AuthorizationRequest( x ) ) );
            }
        }

    iDeniedAuth.Reset();

    count = AuthorizationStatusCount();
    for ( TInt y( 0 ) ; y < count ; ++y )
        {
        if ( AuthorizationStatus( y ).AuthorizationStatus() ==
             MPEngAuthorizationStatus::EPEngAuthDenied )
            {
            iDeniedAuth.AppendL( &( AuthorizationStatus( y ) ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::NotifyAllChangesToObservers()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::NotifyAllChangesToObservers()
    {
    // notify about new pending authorizations
    TInt count( AuthorizationRequestCount() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        CPEngAuthorizationRequest& authRequest = AuthorizationRequest( x );
        if ( ( authRequest.AuthorizationState() ==
               MPEngAuthorizationRequest::EPEngAuthPending ) &&
             !( authRequest.LocalFlags() & KAuthNotifyFlag ) )
            {
            // inform observers
            NotifyChangeToObservers(
                &MPEngAuthorizationObserver::HandleNewPendingAuthorization,
                static_cast<MPEngAuthorizationRequest&>( authRequest ) );

            authRequest.SetLocalFlags( KAuthNotifyFlag );
            }
        }


    count = AuthorizationStatusCount();
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        CPEngAuthorizationResponse& authResponse = AuthorizationStatus( x );
        if ( !( authResponse.LocalFlags() & KAuthNotifyFlag ) )
            {
            // inform observers
            NotifyChangeToObservers(
                &MPEngAuthorizationObserver::HandlerNewAuthorizationStatus,
                static_cast<const MPEngAuthorizationStatus&>( authResponse ) );

            authResponse.SetLocalFlags( KAuthNotifyFlag );
            }
        }

    // inform observers about global change
    count = iObservers.Count();
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iObservers[ x ]->HandleAuthorizationEngineUpdate();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::NotifyChangeToObservers()
// -----------------------------------------------------------------------------
//
template <class NotifyItem>
void CPEngAuthorizationManager::NotifyChangeToObservers(
    void ( MPEngAuthorizationObserver:: *aNotifFunc )(  NotifyItem& ),
    NotifyItem& aArgument )
    {
    TInt count( iObservers.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        MPEngAuthorizationObserver& observer = *( iObservers[ x ] );
        ( observer.*aNotifFunc )( aArgument );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::UpdateAllFlags()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::UpdateAllFlags()
    {
    TInt count( AuthorizationRequestCount() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        AuthorizationRequest( x ).SetLocalFlags( KAuthNotifyFlag );
        }

    count = AuthorizationStatusCount();
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        AuthorizationStatus( x ).SetLocalFlags( KAuthNotifyFlag );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::AuthorizationResponseL()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationResponse& CPEngAuthorizationManager::AuthorizationResponseL(
    const TDesC& aUserId )
    {
    TInt index( CPEngAuthorizationEngine::FindAuthResponse( iResponses, aUserId ) );
    CPEngAuthorizationResponse* authResponse = NULL;
    if ( index == KErrNotFound )
        {
        authResponse = CPEngAuthorizationResponse::NewLC( aUserId, iBufRespSize );
        iResponses.AppendL( authResponse );
        CleanupStack::Pop( authResponse );
        }

    else
        {
        authResponse = iResponses[ index ];
        }

    return *authResponse;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::EnableLockLC()
// -----------------------------------------------------------------------------
//
TBool CPEngAuthorizationManager::EnableLockLC()
    {
    if ( iResponses.Count() == 0 )
        {
        CPEngStoreEntry::LockLC();
        return ETrue;
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::ReleaseLock()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::ReleaseLock()
    {
    CPEngStoreEntry::Unlock();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationManager::Release()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationManager::Release()
    {
    iResponses.ResetAndDestroy();
    CPEngStoreEntry::Release();
    }


//  End of File

