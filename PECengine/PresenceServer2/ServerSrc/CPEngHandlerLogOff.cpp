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
* Description:  Log out Handler
*			     Handles Log in for closing of the network session
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngHandlerLogOff.h"
#include	"MPEngRequestHandlerObserver.h"
#include	"MPEngAccessHandler.h"
#include	"MPEngSubSession.h"

#include	"PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::CPEngHandlerLogOff
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngHandlerLogOff::CPEngHandlerLogOff(
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
// CPEngHandlerLogOff::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogOff::ConstructL()
    {
    // add active object to the active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngHandlerLogOff* CPEngHandlerLogOff::NewL(
    MPEngRequestHandlerObserver& aObserver,
    MPEngAccessHandler& aAccessHandler,
    MPEngSubSession& aSubSession,
    const RPEngMessage& aMessage,
    TInt32 aSessionId,
    TInt32 aSubSessionId )
    {
    CPEngHandlerLogOff* self = NewLC( aObserver,
                                      aAccessHandler,
                                      aSubSession,
                                      aMessage,
                                      aSessionId,
                                      aSubSessionId );

    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngHandlerLogOff* CPEngHandlerLogOff::NewLC(
    MPEngRequestHandlerObserver& aObserver,
    MPEngAccessHandler& aAccessHandler,
    MPEngSubSession& aSubSession,
    const RPEngMessage& aMessage,
    TInt32 aSessionId,
    TInt32 aSubSessionId )
    {
    CPEngHandlerLogOff* self = new( ELeave ) CPEngHandlerLogOff(
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
CPEngHandlerLogOff::~CPEngHandlerLogOff()
    {
    Cancel();
    // remove handler from the CSP Session manager
    iObserver.CompleteRequestHandler( this );
    }


/////////////////////////////////////////////////////////////////////////////////
// =============== New Functions of the MPEngRequestHandler class ==============
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::StartHandlerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogOff::StartHandlerL()
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerLogOff::StartHandlerL()" ) );
    // issue log out request to the access handler
    iAccessHandler.LogOutL( iStatus );
    if ( iStatus == KRequestPending )
        {
        SetActive();
        }
    else
        {
        iMessage.Complete( KErrNone );
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::SessionId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt32 CPEngHandlerLogOff::SessionId() const
    {
    return iSessionId;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::SubSessionId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerLogOff::SubSessionId() const
    {
    return iSubSessionId;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::RequestFunction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerLogOff::RequestFunction() const
    {
    return iFunction;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::CancelRequestD
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogOff::CancelRequestD()
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerLogOff::CancelRequest" ) );
    // delete handler, it will cancel itself
    delete this;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::SetMessage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogOff::SetMessage(
    const RPEngMessage& aMessage )
    {
    iMessage = aMessage;
    }


/////////////////////////////////////////////////////////////////////////////////
// =============== New Functions of the CActive class ==========================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::RunL
// Handles an active object’s request completion event.
// Check if log in was successful and update state according to it,
// then complete request with appropriate Error code
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogOff::RunL()
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerLogOff::RunL() - result:  %d" ),
             iStatus.Int() );
    TInt status( iStatus.Int() );
    if ( status == KErrNone )
        {
        iSubSession.HandleLogOut();
        }
    // transfer adapter will call service status change of session holder
    iMessage.Complete( status );
    // complete request
    delete this;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::RunError
// Handles a leave occurring in the request completion event handler RunL()
// implementation should return KErrNone, if it handles the leave
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerLogOff::RunError(
    TInt aError )
    {
    // nothing to do, just to complete and destroy this object
    iMessage.Complete( aError );
    delete this;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerLogOff::DoCancel
// Implements cancellation of an outstanding request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerLogOff::DoCancel()
    {
    // request was canceled, cancel log out, complete RPEngMessage
    // we should not leave, but if it fails we are baaaad
    iAccessHandler.Cancel();

    // Complete message
    iMessage.Complete( KErrCancel );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
