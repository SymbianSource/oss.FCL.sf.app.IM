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
* Description:  Log in Handler
* 				 Handles Log in for opening of the network session
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngHandlerLogIn.h"
#include	"MPEngRequestHandlerObserver.h"
#include	"MPEngAccessHandler.h"
#include	"MPEngSubSession.h"
#include	"TPEngServerMessages.h"

//	Debug prints
#include	"PresenceDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::CPEngHandlerLogIn
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngHandlerLogIn::CPEngHandlerLogIn(
    MPEngRequestHandlerObserver& aObserver,
    MPEngAccessHandler& aAccessHandler,
    MPEngSubSession& aSubSession,
    const RPEngMessage& aMessage,
    TInt32 aSessionId,
    TInt32 aSubSessionId )
        : CActive( CActive::EPriorityStandard ),
        iSessionId( aSessionId ),
        iSubSessionId( aSubSessionId ),
        iFunction( aMessage.Function() ),
        iAccessHandler( aAccessHandler ),
        iObserver( aObserver ),
        iSubSession( aSubSession )
    {
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogIn::ConstructL()
    {
    // add active object to the active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngHandlerLogIn* CPEngHandlerLogIn::NewL(
    MPEngRequestHandlerObserver& aObserver,
    MPEngAccessHandler& aAccessHandler,
    MPEngSubSession& aSubSession,
    const RPEngMessage& aMessage,
    TInt32 aSessionId,
    TInt32 aSubSessionId )
    {
    CPEngHandlerLogIn* self = NewLC( aObserver,
                                     aAccessHandler,
                                     aSubSession,
                                     aMessage,
                                     aSessionId,
                                     aSubSessionId );

    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngHandlerLogIn* CPEngHandlerLogIn::NewLC(
    MPEngRequestHandlerObserver& aObserver,
    MPEngAccessHandler& aAccessHandler,
    MPEngSubSession& aSubSession,
    const RPEngMessage& aMessage,
    TInt32 aSessionId,
    TInt32 aSubSessionId )
    {
    CPEngHandlerLogIn* self = new( ELeave ) CPEngHandlerLogIn(
        aObserver,
        aAccessHandler,
        aSubSession,
        aMessage,
        aSessionId,
        aSubSessionId );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
CPEngHandlerLogIn::~CPEngHandlerLogIn()
    {
    Cancel();
    // remove handler from the CSP Session manager
    iObserver.CompleteRequestHandler( this );
    }

/////////////////////////////////////////////////////////////////////////////////
// =============== New Functions of the MPEngRequestHandler class ==============
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::StartHandlerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogIn::StartHandlerL(
    const RPEngMessage& aMessage )
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerLogIn::StartHandlerL" ) );
    // read log in buffer
    HBufC8*	loginData = aMessage.ReadOneDescriptor8L( KMessageSlot0 );
    // Issue request to the access handler and activate Active object
    // takes ownership of login buffer, safe to call
    iAccessHandler.LogInL( iStatus, loginData );
    if ( iStatus == KRequestPending )
        {
        SetActive();
        }
    else
        {
        aMessage.Complete( KErrNone );
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::SessionId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt32 CPEngHandlerLogIn::SessionId() const
    {
    return iSessionId;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::SubSessionId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerLogIn::SubSessionId() const
    {
    return iSubSessionId;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::RequestFunction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerLogIn::RequestFunction() const
    {
    return iFunction;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::CancelRequestD
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogIn::CancelRequestD()
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerLogIn::CancelRequest" ) );
    // delete handler, it will cancel itself
    delete this;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::SetMessage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogIn::SetMessage(
    const RPEngMessage& aMessage )
    {
    iMessage = aMessage;
    }


/////////////////////////////////////////////////////////////////////////////////
// =============== New Functions of the CActive class ==========================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::RunL
// Handles an active object’s request completion event.
// Check if log in was successful and update state according to it, then complete
// request with appropriate Error code
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogIn::RunL()
    {
    // what is necessary depends how request went
    TInt compCode ( iStatus.Int() );
    PENG_DP( D_PENG_LIT( "CPEngHandlerLogIn::RunL() - result:  %d" ), compCode );
    switch ( compCode )
        {
        case KErrNone:
            {
            // log in successful, update state of the CSP holder
            iObserver.SetCSPSessionOpenedL();
            iSubSession.HandleLogInL();
            break;
            }
        default:
            {
            // complete Client's message with the error, this might lead to
            // deletion of whole session holder, along with this handler,
            // so later completion would not be safe,we might not exist anymore
            break;
            }
        }
    iMessage.Complete( compCode );
    delete this;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::RunError
// Handles a leave occurring in the request completion event handler RunL()
// implementation should return KErrNone, if it handles the leave
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerLogIn::RunError(
    TInt aError )
    {
    // seems like nothing to do here
    PENG_DP( D_PENG_LIT( "CPEngHandlerLogIn::RunError() - result:  %d" ), aError );
    // nothing to do, just to complete and destroy this object
    iMessage.Complete( aError );
    delete this;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogIn::DoCancel
// Implements cancellation of an outstanding request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogIn::DoCancel()
    {
    // request was canceled, cancel log in, complete RPEngMessage
    // we should not leave, but if it fails we are baaaad
    iAccessHandler.Cancel();

    // complete message
    iMessage.Complete( KErrCancel );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
