/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence Server sub-session handler implementation
*                All clients sub-session related request are handler here
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngSubSession.h"
#include    "TPEngServerMessages.h"
#include    "TPEngServerCommon.h"

#include    "MPEngServer.h"
#include    "RPEngMessage.h"
#include    "CPEngSessionSlotId.h"
#include    "CPEngCSPSessManager.h"


namespace
    {
    void LeaveIfNotAttachedL( TBool aAttached )
        {
        if ( !aAttached )
            {
            User::Leave( KErrNotReady );
            }
        }

    void LeaveIfAttachedL( TBool aAttached )
        {
        if ( aAttached )
            {
            User::Leave( KErrAlreadyExists );
            }
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSubSession::CPEngSubSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSubSession::CPEngSubSession(
    TInt32 aSessionId )
        : iSessionId( aSessionId )
    {
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngSubSession::ConstructL(
    MPEngServer& aServer,
    const RPEngMessage& aMessage )
    {
    // app Id
    iType = aMessage.Function();
    iAppId = aMessage.ReadOneDescriptor16L( KMessageSlot1 );

    // Read session id from the message
    HBufC8* sessIdBuff = aMessage.ReadOneDescriptor8LC( KMessageSlot0 );
    CPEngSessionSlotId* sessId = CPEngSessionSlotId::NewLC();
    sessId->UnPackL( * sessIdBuff );

    // get Session manager from the server
    CPEngCSPSessManager* sessMan = aServer.CSPSessionManagerLC( *sessId );

    // if this is transaction sub session, check app Id validity
    if ( iType == EMainSessCreateTransactionSubSession )
        {
        sessMan->AppIdActiveL( *iAppId );
        }
    // open reference to the CPS holder
    sessMan->OpenL( *this );
    iSessionManager = sessMan;
    CleanupStack::PopAndDestroy( 3 ); // sessMan, sessId, sessIdBuff
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSubSession* CPEngSubSession::NewL(
    MPEngServer& aServer,
    const RPEngMessage& aMessage,
    TInt32 aSessionId )
    {
    CPEngSubSession* self = NewLC( aServer, aMessage, aSessionId );

    CleanupStack::Pop(); // self

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSubSession* CPEngSubSession::NewLC(
    MPEngServer& aServer,
    const RPEngMessage& aMessage,
    TInt32 aSessionId )
    {
    CPEngSubSession* self = new( ELeave ) CPEngSubSession( aSessionId );

    CleanupClosePushL( *self );
    self->ConstructL( aServer, aMessage );
    return self;
    }

// Destructor
CPEngSubSession::~CPEngSubSession()
    {
    // decrease count of state machine
    if ( iSessionManager )
        {
        iSessionManager->CancelAllSubSessionRquests( iSessionId, iHandle );
        iSessionManager->Close( this ); // normal reference
        if ( iAppIdAttached )
            {
            // remove appId reference
            iSessionManager->DetachAppId( *iAppId );
            }
        }
    delete iAppId;
    delete iRequestResult;
    }

// =============================================================================
// =========== New Functions of the MPengSubSession class ======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSubSession::StoreRequestResponse()
// Store request response for the sub-session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSubSession::StoreRequestResponse(
    HBufC8* aResponse )
    {
    delete iRequestResult;
    iRequestResult = aResponse;
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::AppId()
// Application Id of the sub-session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDesC& CPEngSubSession::AppId()
    {
    return *iAppId;
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::HandleLogInL()
// Handler successful application Id login
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSubSession::HandleLogInL()
    {
    // remove normal reference by the app Id reference
    iSessionManager->AttachAppIdL( *iAppId );
    iAppIdAttached = ETrue;
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::HandleLogOut()
// Handler successful application Id logOut
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSubSession::HandleLogOut()
    {
    if ( iAppIdAttached )
        {
        iSessionManager->DetachAppId( *iAppId );
        iAppIdAttached = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::DisconnectAppId()
// Disconnect Application Id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSubSession::DisconnectAppId(
    const TDesC& aAppId )
    {
    if ( iAppId->Compare( aAppId ) != KErrNone )
        {
        // not our app Id
        return;
        }
    iAppIdAttached = EFalse;
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::DisconnectAppId()
// Handle CSP closing
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSubSession::CSPSessionClosed()
    {
    iAppIdAttached = EFalse;
    }

// =============================================================================
// =============== Functions of the main class =================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSubSession::DispatchMessageL()
// Dispatch message from the client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEngSubSession::DispatchMessageL(
    const RPEngMessage& aMessage,
    TInt aRequest )
    {
    TBool completeMessage( ETrue );
    switch ( aRequest )
        {
            // Storage Related Requests
        case ESubSessCancelRequest:
            {
            iSessionManager->CancelAsynchronousRequest( aMessage,
                                                        iSessionId,
                                                        iHandle );
            break;
            }

        // Holder client
        case ESubSessLogIn:
            {
            iSessionManager->HandleLogInRequestL( aMessage,
                                                  *this,
                                                  iSessionId,
                                                  iHandle );
            completeMessage = EFalse;
            break;
            }

        case ESubSessForceLogOut:
            {
            LeaveIfNotAttachedL( iAppIdAttached );
            iSessionManager->HandleForceLogOutRequestL( aMessage,
                                                        *this,
                                                        iSessionId,
                                                        iHandle );
            completeMessage = EFalse;
            break;
            }

        case ESubSessAttachHolder:
            {
            LeaveIfAttachedL( iAppIdAttached );
            iSessionManager->AttachAppIdL( *iAppId );
            iAppIdAttached = ETrue;
            break;
            }

        case ESubSessAttachWithProcessName:
            {
            LeaveIfAttachedL( iAppIdAttached );
            HBufC* processName = aMessage.ReadOneDescriptor16LC(
                                     KMessageSlot0 );

            iSessionManager->ActivateProcessL( *iAppId, *processName );
            iAppIdAttached = ETrue;
            CleanupStack::PopAndDestroy(); // processName
            break;
            }

        case ESubSessDetachHolder:
            {
            LeaveIfNotAttachedL( iAppIdAttached );
            iSessionManager->HandleDetachAppIdL( aMessage,
                                                 *this,
                                                 iSessionId,
                                                 iHandle );
            completeMessage = EFalse;
            break;
            }

        case ESubSessLeaveAlive:
            {
            LeaveIfNotAttachedL( iAppIdAttached );
            HBufC* processName = aMessage.ReadOneDescriptor16LC(
                                     KMessageSlot0 );

            iSessionManager->ReserveProcessL( *iAppId, *processName );
            iAppIdAttached = EFalse;
            CleanupStack::PopAndDestroy(); // processName
            break;
            }

        case ESubSessGetServiceTree:
        case ESubSessGetLogInData:
            {
            iSessionManager->HandleSynchronousRequestL( aMessage,
                                                        iSessionId,
                                                        iHandle );
            break;
            }

        // Transfer client
        case ESubSessUpdateData:
            {
            iSessionManager->HandleAsynchronousRequestL( aMessage,
                                                         *this,
                                                         iSessionId,
                                                         iHandle );
            completeMessage = EFalse;
            break;
            }

        case ESubSessFetchUpdateRqstResult:
            {
            FetchRequestresultL( aMessage );
            break;
            }

        default:
            {
            PanicClient( aMessage, EPECBadRequest );
            break;
            }
        }
    // check if message can be completed
    completeMessage &= aMessage.MessageValid();
    return completeMessage;
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::SetSubSesionHandle()
// Set Sub-session handle ID for the Session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSubSession::SetSubSesionHandle(
    TInt aHandle )
    {
    iHandle = aHandle;
    }

// =============================================================================
// =============== Private Functions from base class ===========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSubSession::PanicClient()
// Panic client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSubSession::PanicClient(
    const RPEngMessage& aMessage,
    const TInt aPanic ) const
    {
    aMessage.Panic( KSessionName, aPanic );
    }

// -----------------------------------------------------------------------------
// CPEngSubSession::FetchRequestresultL()
// Fetch result of last asynchronous request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSubSession::FetchRequestresultL(
    const RPEngMessage& aMessage )
    {
    if ( !iRequestResult )
        {
        User::Leave( KErrNotFound );
        }
    aMessage.WriteOneDescriptorL( KMessageSlot0, *iRequestResult );
    delete iRequestResult;
    iRequestResult = NULL;
    }

//  End of File
