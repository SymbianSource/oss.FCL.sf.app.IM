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
* Description:  Request Handler to update data to the network server
*
*/



// INCLUDE FILES
#include    "CPEngHandlerSendData.h"
#include    <e32std.h>

#include	"MPEngRequestHandlerObserver.h"
#include	"CPEngOutGoingTransContainer.h"
#include	"MPEngOutgoingTransactionHandler.h"
#include	"MPEngSubSession.h"
#include	"PEngWVPresenceErrors2.h"
#include	"CPEngTransactionStatus.h"

//	Debug prints
#include	"PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::CPEngHandlerSendData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngHandlerSendData::CPEngHandlerSendData(
    MPEngRequestHandlerObserver& aRequestHandlerObserver,
    MPEngPureDataHandler& aPureDataHandler,
    MPEngSubSession& aServSubSession,
    const RPEngMessage& aMessage,
    TInt32 aSessionId,
    TInt32 aSubSessionId )
        : iRequestHandlerObserver( aRequestHandlerObserver ),
        iPureDataHandler( aPureDataHandler ),
        iServSubSession( aServSubSession ),
        iSessionId( aSessionId ),
        iSubSessionId( aSubSessionId ),
        iFunction( aMessage.Function() )
    {
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::ConstructHandlerFromTransactionHandlersL
// Symbian 2nd phase constructor can leave.
// Construct Send Data handler from the passed array of the outgoing Transaction handlers
// -----------------------------------------------------------------------------
//
void CPEngHandlerSendData::ConstructL(
    RPointerArray<MPEngOutgoingTransactionHandler>& aTransactions )
    {
    // copy handlers from the given array, but just their pointers, it takes ownership
    for ( TInt x( aTransactions.Count() - 1 ) ; x >= 0 ; x-- )
        {
        User::LeaveIfError( iOutgoingTransactionHandlers.Append( aTransactions[ x ] ) );
        aTransactions.Remove( x );
        }
    iTransactionStatus = CPEngTransactionStatus::NewL();
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::NewHandlerFromTransactionHandlersLC
// Two-phased constructor. which creates Send Data handler from the passed array of the
// Transaction handlers, no reading of the storage IDs from the client side is needed then
// are readed from the client RPEngMessage
// -----------------------------------------------------------------------------
//
CPEngHandlerSendData* CPEngHandlerSendData::NewLC(
    MPEngRequestHandlerObserver&  aRequestHandlerObserver,
    MPEngPureDataHandler& aPureDataHandler,
    MPEngSubSession& aServSubSession,
    const RPEngMessage& aMessage,
    RPointerArray<MPEngOutgoingTransactionHandler>& aTransactions,
    TInt32 aSessionId,
    TInt32 aSubSessionId )
    {
    CPEngHandlerSendData* self = new( ELeave ) CPEngHandlerSendData(
        aRequestHandlerObserver,
        aPureDataHandler,
        aServSubSession,
        aMessage,
        aSessionId,
        aSubSessionId );

    CleanupStack::PushL( self );
    self->ConstructL( aTransactions );

    return self;
    }

// Destructor
CPEngHandlerSendData::~CPEngHandlerSendData()
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerSendData::~CPEngHandlerSendData" ) );
    // cancel and delete all transaction containers
    iTrasactionContainersArray.ResetAndDestroy();
    iOutgoingTransactionHandlers.ResetAndDestroy();
    delete iTransactionStatus;

    // complete message with KErrCancel, just in case it was not completed yet
    iMessage.Complete( KErrCancel );
    // remove handler from the CSP Session manager
    iRequestHandlerObserver.CompleteRequestHandler( this );
    PENG_DP( D_PENG_LIT( "CPEngHandlerSendData::~CPEngHandlerSendData - End" ) );
    }


/////////////////////////////////////////////////////////////////////////////////
// =============== New Functions of the MPEngRequestHandler class ==============
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::StartHandlerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerSendData::StartHandlerL( )
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerSendData::StartHandlerL" ) );
    // create containers and run them
    CreateNewContainersRunThemL();
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::SessionId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt32 CPEngHandlerSendData::SessionId( ) const
    {
    return iSessionId;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::SubSessionId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerSendData::SubSessionId( ) const
    {
    return iSubSessionId;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::RequestFunction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngHandlerSendData::RequestFunction( ) const
    {
    return iFunction;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::CancelRequest
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerSendData::CancelRequestD( )
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerSendData::CancelRequest" ) );
    // delete containers and complete message
    iTrasactionContainersArray.ResetAndDestroy();
    iMessage.Complete( KErrCancel );
    // now we are done and handler can be deleted
    delete this;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::SetMessage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerSendData::SetMessage(
    const RPEngMessage& aMessage )
    {
    iMessage = aMessage;
    }


/////////////////////////////////////////////////////////////////////////////////
// =============== New Functions of the MPEngHandlerSendData class =============
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::CompleteRequestD
// Complete request handlers, store request result and delete itself
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerSendData::CompleteRequestD()
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerSendData::CompleteRequestL" ) );
    // get transaction result and store it in SubSession
    HBufC8* volatile reqResult = NULL;
    TRAPD( e, reqResult = iTransactionStatus->PackResultsL() );
    // if error complete with error
    if ( e != KErrNone )
        {
        iMessage.Complete( e );
        delete this;
        return;
        }
    iServSubSession.StoreRequestResponse( reqResult );
    // complete RPEngMessage and delete itself
    iMessage.Complete( reqResult->Length() );
    delete this;
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::CreateContainersAndRunThemL
// go through all transaction handlers in iOutgoingTransactionHandlers
// array, create for each of then container with given Transaction handler
// and start it
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerSendData::CreateNewContainersRunThemL()
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerSendData::CreateNewContainersRunThemL" ) );

    // we have to go through array from the end, created
    // transaction containers are taking owner ship of the transaction
    // handler from the array, so they have to be removed from there
    for ( TInt i( iOutgoingTransactionHandlers.Count() - 1 ) ; i >= 0 ; i-- )
        {
        CPEngOutGoingTransContainer* newContainer = CPEngOutGoingTransContainer::NewLC(
                                                        *this,
                                                        iPureDataHandler,
                                                        iOutgoingTransactionHandlers );
        newContainer->StartTransactionL( iOutgoingTransactionHandlers[ i ] );
        iOutgoingTransactionHandlers.Remove( i );
        // remove trans. handler from the array since its ownership was
        // taken by transaction container
        iTrasactionContainersArray.AppendL( newContainer );
        CleanupStack::Pop( newContainer ); // newContainer
        }
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::CompleteTransContainer
// Remove transaction container from the array of containers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerSendData::CompleteTransContainer(
    CPEngOutGoingTransContainer* aTransContainer,
    CPEngTransactionStatus* aTransactionStatus,
    TInt aErrCode )
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerSendData::CompleteTransContainer" ) );

    // inport transaction status
    iTransactionStatus->ImportStatusFrom( *aTransactionStatus );

    // find transaction and remove it from the list
    TInt index ( iTrasactionContainersArray.Find( aTransContainer ) );
    if ( index != KErrNotFound )
        {
        if ( KErrCancel != aErrCode &&  aErrCode != KPEngNwErrForcedLogout )
            {
            //Skip the deletion if aErrCode is KErrCancel
            delete iTrasactionContainersArray[ index ];
            }
        iTrasactionContainersArray.Remove( index );
        }
    else
        {
        // if container was not found in array, complete it anyway,
        // however, it should not happen
        delete aTransContainer;
        }

    // end if there was some Symbian error
    if ( aErrCode != KErrNone )
        {
        // cancel all active transaction
        iTrasactionContainersArray.ResetAndDestroy();
        iMessage.Complete( aErrCode );
        delete this;
        }
    else if ( iTrasactionContainersArray.Count() == 0 )
        {
        // we can safely call complete, even this function was
        // most likely called from the destructor of the
        // Transaction Container, it was already removed from the array
        CompleteRequestD();
        }
    PENG_DP( D_PENG_LIT( "CPEngHandlerSendData::CompleteTransContainer - End" ) );
    }

// -----------------------------------------------------------------------------
// CPEngHandlerSendData::LastRunningTransaction
// Check how many running transactions is still there,
// and if calling one is last, return ETrue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEngHandlerSendData::LastRunningTransaction( )
    {
    return ( 1 == iTrasactionContainersArray.Count() );
    }

//  End of File
