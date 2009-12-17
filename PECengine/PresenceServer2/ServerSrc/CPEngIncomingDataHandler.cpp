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
* Description:  Incoming data handler
*                it handles incoming data from the network server
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngIncomingDataHandler.h"
#include    "MPEngIncomingTransactionHandler.h"
#include    "MPEngPureDataHandler.h"
#include	"MPEngTransactionFactory.h"
#include	"MPEngRequestHandlerObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::CPEngIncomingDataHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngIncomingDataHandler::CPEngIncomingDataHandler(
    MPEngPureDataHandler& aPureDataHandler,
    RPointerArray<MPEngTransactionFactory>& aTransactionFactories,
    MPEngRequestHandlerObserver& aRequestHandlerObserver )
        : CActive( CActive::EPriorityStandard ),
        iPureDataHandler( aPureDataHandler ),
        iTransactionFactories( aTransactionFactories ),
        iRequestHandlerObserver( aRequestHandlerObserver )



    {
    }

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngIncomingDataHandler::ConstructL()
    {
    // add active object to the active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngIncomingDataHandler* CPEngIncomingDataHandler::NewL(
    MPEngPureDataHandler& aPureDataHandler,
    RPointerArray<MPEngTransactionFactory>& aTransactionFactories,
    MPEngRequestHandlerObserver& aRequestHandlerObserver )
    {
    CPEngIncomingDataHandler* self = NewLC( aPureDataHandler, aTransactionFactories, aRequestHandlerObserver );

    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngIncomingDataHandler* CPEngIncomingDataHandler::NewLC(
    MPEngPureDataHandler& aPureDataHandler,
    RPointerArray<MPEngTransactionFactory>&
    aTransactionFactories,
    MPEngRequestHandlerObserver& aRequestHandlerObserver )
    {
    CPEngIncomingDataHandler* self =
        new( ELeave ) CPEngIncomingDataHandler(
        aPureDataHandler,
        aTransactionFactories,
        aRequestHandlerObserver );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
CPEngIncomingDataHandler::~CPEngIncomingDataHandler()
    {
    Cancel();
    delete iTransactionHandler;
    // remove if from the list of the listener
    }

/////////////////////////////////////////////////////////////////////////////////
// ============== Function from the Main class =================================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::StartListeningL
// Start listening of the incoming data from the server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngIncomingDataHandler::StartListeningL()
    {
    // start listening incoming data
    TInt count( iPureDataHandler.ListenIncomingData( iStatus ) );
    SetActive();
    for ( TInt x( 0 ) ;  x < count ; ++x )
        {
        HandleIncomingDataL();
        }
    }

/////////////////////////////////////////////////////////////////////////////////
// ============== Function from the CActive =====================================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::RunL
// Handles an active object’s request completion event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngIncomingDataHandler::RunL()
    {
    // OK there is something from the WV server
    HandleIncomingDataL();
    StartListeningL();
    }

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::RunError
// Handles a leave occurring in the request completion event handler RunL()
// implementation should return KErrNone, if it handles the leave
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngIncomingDataHandler::RunError(
    TInt /* aError*/ )
    {
    // restartt listening incoming data
    iPureDataHandler.ListenIncomingData( iStatus );
    SetActive();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::DoCancel
// Implements cancellation of an outstanding request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngIncomingDataHandler::DoCancel()
    {
    iPureDataHandler.CancelListening();
    }

/////////////////////////////////////////////////////////////////////////////////
// ============== New private Functions of the main class =======================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngIncomingDataHandler::HandleIncomingDataL
// Handle here incoming data from the presence server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngIncomingDataHandler::HandleIncomingDataL()
    {
    // first get new data from the pure data handler
    HBufC8* newData;
    newData = iPureDataHandler.ResponseL( 0 ); // zero is incoming Data
    if ( !newData )
        {
        StartListeningL();
        return;
        }
    CleanupStack::PushL( newData );
    TPtr8 incomingData( newData->Des() );
    // TO-DO create new listen handler, since this one will be bussy
    // with processing


    // ask transaction factories if we want this data
    TInt count( iTransactionFactories.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        iTransactionHandler =
            iTransactionFactories[x]->IncomingTransactionHandlerL(
                incomingData );
        if ( iTransactionHandler )
            {
            // no process new Transaction Handler
            // TO-DO, do processing asynchronous
            iTransactionHandler->ProcessRequestL( incomingData, iStatus );
            // check if transaction needs to perform more complicated answer
            delete iTransactionHandler;
            iTransactionHandler = NULL; // this is here for the destructor
            }
        }
    CleanupStack::PopAndDestroy(); // newData
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
