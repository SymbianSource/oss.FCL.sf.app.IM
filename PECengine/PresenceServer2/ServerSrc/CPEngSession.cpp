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
* Description:  Presence Server session handler implementation
*				 All clients request are handler here,
*  				 or forwarded to the sub-session
*
*/



//  Include Files
#include	<e32base.h>
#include	"CPEngSession.h"
#include	"MPEngServer.h"
#include	"CPEngSubSession.h"
#include	"RPEngMessage.h"
#include	"TPEngServerCommon.h"

//	Debug prints
#include	"PresenceDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSession::CPEngSession
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngSession::CPEngSession(
    MPEngServer& aServer )
        : iMainServer( aServer )
    {
    iMainServer.SessionCreated();
    }

// -----------------------------------------------------------------------------
// CPEngSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngSession::ConstructL()
    {
    iSubSessions = CObjectIx::NewL();
    }

// Static constructor
CPEngSession* CPEngSession::NewL( MPEngServer& aServer )
    {
    CPEngSession* self = new( ELeave ) CPEngSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor (virtual by CBase)
CPEngSession::~CPEngSession()
    {
    delete iSubSessions;
    // If main session will have some Async operations,
    // it needs to be canceled here
    iMainServer.SessionDied();
    }

// =============================================================================
// =============== Functions from CSession2 base class =========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSession::ServiceL()
// -----------------------------------------------------------------------------
//
void CPEngSession::ServiceL(
    const RMessage2 &aMessage )
    {
    PENG_DP( D_PENG_LIT( "CPEngSession::ServiceL:%d" ), aMessage.Function() );

    if ( DispatchMessageL( RPEngMessage( aMessage ) ) )
        {
        aMessage.Complete( KErrNone );
        }
    PENG_DP( D_PENG_LIT( "CPEngSession::ServiceL:%d -End" ), aMessage.Function() );
    }

// -----------------------------------------------------------------------------
// CPEngSession::ServiceError()
// -----------------------------------------------------------------------------
//
void CPEngSession::ServiceError(
    const RMessage2& aMessage,
    TInt aError )
    {
    PENG_DP( D_PENG_LIT( "CPEngSession::ServiceError:  %d" ), aError );
    aMessage.Complete( aError );
    }

// =============================================================================
// =============== Private functions of the Session class ======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSession::DispatchMessageL()
// Dispatch message
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEngSession::DispatchMessageL(
    const RPEngMessage& aMessage )
    {
    TInt request( aMessage.Function() );
    TBool completeRequest( ETrue );

    switch ( request )
        {
        case EMainSessCreateHolderSubSession:
        case EMainSessCreateTransactionSubSession:
            {
            CreateNewSubSessionL( aMessage );
            break;
            }

        case EMainSessCloseSubSession:
            {
            RemoveSubSessionL( aMessage.Int3() );
            break;
            }
        default:
            {
            // try to handle message in sub-session
            CPEngSubSession& subSession = FindSubSessionL( aMessage.Int3() );
            completeRequest = subSession.DispatchMessageL( aMessage, request );
            break;
            }
        }
    return completeRequest && aMessage.MessageValid();
    }

// -----------------------------------------------------------------------------
// CPEngSession::CreateNewSubSessionL()
// Create new sub-session
// Back to the client is written handle of the created Sub session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSession::CreateNewSubSessionL(
    const RPEngMessage& aMessage )
    {
    PENG_DP( D_PENG_LIT( "CPEngSession::CreateNewSubSessionL" ) );
    CPEngSubSession* subSession =
        CPEngSubSession::NewLC(
            iMainServer,
            aMessage,
            reinterpret_cast<TInt32>( this ) );

    iMainServer.AddSubSessionL( *subSession );
    CleanupStack::Pop( ); // newSubsSession

    TInt handle( 0 );
    TRAPD( err, handle = iSubSessions->AddL( subSession ) );
    if ( err != KErrNone )
        {
        iMainServer.RemoveSubSessionL( *subSession );
        User::Leave( err );
        }

    subSession->SetSubSesionHandle( handle );

    // write back handle of the sub-session
    TPckg<TInt> handlePckg( handle );
    err = aMessage.WriteOneDescriptor( KMessageSlot3 , handlePckg );
    if ( err != KErrNone )
        {
        iSubSessions->Remove( handle );
        PanicClient( aMessage, EPECBadDescriptor );
        return;
        }
    }


// -----------------------------------------------------------------------------
// CPEngSession::RemoveSubSession()
// Remove Sub-Session
// Function leaves if there is no such a subssession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSession::RemoveSubSessionL(
    TUint  aHandle )
    {
    // this will leave if there is such a sub-session
    FindSubSessionL( aHandle );
    iSubSessions->Remove( aHandle );
    }

// -----------------------------------------------------------------------------
// CPEngSession::FindSubSession()
// Find Sub-session
// Function leaves if such a subsession does not exists
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngSubSession& CPEngSession::FindSubSessionL(
    TUint  aHandle ) const
    {
    // if there is no sub-session, it leaves with KErrBadHandle
    return static_cast<CPEngSubSession&> ( *( iSubSessions->AtL( aHandle ) ) );
    }

// -----------------------------------------------------------------------------
// CPEngSession::PanicClient()
// Panic client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngSession::PanicClient(
    const RPEngMessage& aMessage,
    const TInt aPanic ) const
    {
    aMessage.Panic( KSessionName, aPanic );
    }

//  End of File









