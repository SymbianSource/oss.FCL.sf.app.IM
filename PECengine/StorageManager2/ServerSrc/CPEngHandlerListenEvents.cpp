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
* Description:  Listen global even request handler
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngHandlerListenEvents.h"

//  Debug prints
#include    "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngHandlerListenEvents::CPEngHandlerListenEvents
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngHandlerListenEvents::CPEngHandlerListenEvents(
    TUint32 aSessionId )
        : CPEngRequestHandler( EMainSessListenGlobalEvents,
                               aSessionId,
                               KErrNone ), // No subssesion
        iMissedEvents( 1 ) // buffer for missed events
    {
    }


// -----------------------------------------------------------------------------
// CPEngHandlerListenEvents::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngHandlerListenEvents::ConstructL()
    {
    // nothing here, client is listening all events
    }


// -----------------------------------------------------------------------------
// CPEngHandlerListenEvents::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngHandlerListenEvents* CPEngHandlerListenEvents::NewL(
    TUint32 aSessionId )
    {
    CPEngHandlerListenEvents* self = NewLC( aSessionId );

    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerListenEvents::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngHandlerListenEvents* CPEngHandlerListenEvents::NewLC(
    TUint32 aSessionId )
    {
    CPEngHandlerListenEvents* self =
        new( ELeave ) CPEngHandlerListenEvents( aSessionId );


    // reference counted
    CleanupClosePushL( *self );
    self->ConstructL();

    return self;
    }


// Destructor
CPEngHandlerListenEvents::~CPEngHandlerListenEvents()
    {
    // cleaning is done in base class
    iMissedEvents.Reset();
    }


// =============================================================================
// ====== NEW FUNCTIONS FROM THE CPEngRequestHandler ===========================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngHandlerListenEvents::NotifyEventL
// -----------------------------------------------------------------------------
//
void CPEngHandlerListenEvents::NotifyEvent(
    const TDesC8& aGlobEvent )
    {
    // Bingo!! complete message with number of event if we are active
    if ( iRequestMessage.MessageValid() )
        {
        // transfer event to the client
        TInt err = iRequestMessage.WriteOneDescriptor( KMessageSlot1,
                                                       aGlobEvent );
        // if transfer went OK, notify client
        if ( err == KErrNone )
            {
            iRequestMessage.Complete( KErrNone );
            return;
            }
        // error of some kind, buffer event
        TInt e( BufferNotification( aGlobEvent ) );
        // wes buffering OK
        if ( e == KErrNone )
            // notify error of des to client writing
            {
            err = err == KErrOverflow ? aGlobEvent.Length() : err;
            }
        else
            // notify buffering error
            {
            err = e;
            }
        // some error complete with size
        iRequestMessage.Complete( err );
        }
    else
        {
        // store event into the buffer of lost event on the end
        iLastResult = BufferNotification( aGlobEvent );
        }
    }


// -----------------------------------------------------------------------------
// CPEngHandlerListenEvents::NotifyError()
// -----------------------------------------------------------------------------
//
void CPEngHandlerListenEvents::NotifyError(
    TInt aErrorCode )
    {
    // Complete with the error if message is valid, or the buffer it
    if ( iRequestMessage.MessageValid() )
        {
        iRequestMessage.Complete( aErrorCode );
        }
    else
        {
        iLastResult = aErrorCode;
        }
    }


// -----------------------------------------------------------------------------
// CPEngHandlerListenEvents::ReloadScoutWithNewMessageL()
// -----------------------------------------------------------------------------
//
void CPEngHandlerListenEvents::ReloadScoutWithNewMessageL(
    const RPEngMessage& aNewScout )
    {
    // reload client if needed
    if ( iRequestMessage.MessageValid() )
        {
        User::Leave( KErrAlreadyExists );
        }
    iRequestMessage = aNewScout;
    // is here some error code buffered for client
    if ( iLastResult != KErrNone )
        {
        iRequestMessage.Complete( iLastResult );
        // set it to zero
        iLastResult = KErrNone;
        return;
        }
    // is there some event buffered
    if ( iMissedEvents.Count() != 0 )
        {
        // try to send event to client, always first one from the array
        TInt err = iRequestMessage.WriteOneDescriptor(  KMessageSlot1,
                                                        iMissedEvents.MdcaPoint( 0 ) );
        // notify client
        if ( err == KErrNone )
            {
            // remove notified event
            iMissedEvents.Delete( 0 );
            }
        else
            {
            err = err == KErrOverflow ?
                  iMissedEvents.MdcaPoint( 0 ).Length() : err;
            }
        iRequestMessage.Complete( err );
        }
    }



// =============================================================================
// ============= New protected function of the class ===========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngHandlerListenEvents::BufferNotification()
// Buffer Global event notification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerListenEvents::BufferNotification(
    const TDesC8& aEvent )
    {
    TRAPD( e, iMissedEvents.AppendL( aEvent ) );
    return e;
    }


//  End of File
