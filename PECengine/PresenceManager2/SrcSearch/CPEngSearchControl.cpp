/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: this class handls search operation
*
*/

// INCLUDE FILES
#include "CPEngSearchControl.h"
#include "CPEngSearchResultEntry.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngAdvTransactionStatus2.h"
#include "CPEngTransactionStatus.h"

#include "CPEngSearchSessionHandler.h"
#include "CPEngSearchHandler.h"
#include "CPEngSearchTransaction2Imp.h"
#include "PEngWVSearchPanics.h"

#include <CPEngSearchTransaction2.h>
#include <MPEngSearchTransactionObserver2.h>
#include <CPEngSearchCriteria2.h>
#include <PEngPresenceEngineConsts2.h>
#include <CPEngNWSessionSlotNotifier2.h>
#include <CPEngNWSessionSlotEvent2.h>



//MACROS
#define RETURN_ERR_IF_NOT_READY()               \
        {                                           \
        if( IsActive() )                            \
            {                                       \
            return KErrInUse;                       \
            }                                       \
        \
        if( iSearchState != EPEngSrch_Searching )   \
            {                                       \
            return KErrNotReady;                    \
            }                                       \
        }                                           \
     


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSearchControl::NewL()
// -----------------------------------------------------------------------------
CPEngSearchControl* CPEngSearchControl::NewL(
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    const RPointerArray< CPEngSearchCriteria2 >& aCriterias,
    TInt aSearchLimit,
    MPEngSearchTransactionObserver2& aObserver )
    {
    CPEngSearchControl* self = new ( ELeave ) CPEngSearchControl( aPriority,
                                                                  aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aPriority,
                      aNWSessionSlotID,
                      aCriterias,
                      aSearchLimit );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngSearchControl::~CPEngSearchControl()
// -----------------------------------------------------------------------------
CPEngSearchControl::~CPEngSearchControl()
    {
    Cancel();

    if ( iSlotNotifier )
        {
        iSlotNotifier->RemoveObserver( *this );
        }
    delete iSlotNotifier;

    delete iSearchHandler;
    delete iSearchSessionHandler;

    delete iTransactionStatus;
    delete iSearchResult;

    if ( iOwner )
        {
        iOwner->HandleSearchDestruction( this );
        }
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::CPEngSearchControl()
// -----------------------------------------------------------------------------
CPEngSearchControl::CPEngSearchControl( TInt aPriority,
                                        MPEngSearchTransactionObserver2& aObserver )
        : CActive( aPriority ),
        iObserver( aObserver ),
        iTransactionOp( KErrNotFound ),
        iCancelling( EFalse ),
        iSearchState( EPEngSrch_Idle ),
        iDelayedErr( KErrNone )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::ConstructL()
// -----------------------------------------------------------------------------
void CPEngSearchControl::ConstructL( TInt aPriority,
                                     const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                     const RPointerArray< CPEngSearchCriteria2 >& aCriterias,
                                     TInt aSearchLimit )
    {
    ValidateSearchCriteriasL( aCriterias );
    iSearchResult = CPEngSearchResultEntry::NewL();

    iSearchSessionHandler = CPEngSearchSessionHandler::NewL( aPriority,
                                                             aNWSessionSlotID );
    iSearchHandler = CPEngSearchHandler::NewL( aPriority,
                                               iSearchSessionHandler->ImpsEngine(),
                                               *iSearchResult,
                                               aCriterias,
                                               aSearchLimit );

    iSlotNotifier = CPEngNWSessionSlotNotifier2::NewL( aPriority );
    User::LeaveIfError( iSlotNotifier->AddObserver( *this ) );
    User::LeaveIfError( iSlotNotifier->Start( aNWSessionSlotID ) );

    iTransactionStatus = CPEngTransactionStatus::NewL();
    }



// -----------------------------------------------------------------------------
// CPEngSearchControl::SetOwner()
// -----------------------------------------------------------------------------
void CPEngSearchControl::SetOwner( CPEngSearchTransaction2Imp& aOwner )
    {
    iOwner = &aOwner;
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::IssueSearch()
// -----------------------------------------------------------------------------
void CPEngSearchControl::IssueSearch()
    {
    iDelayedErr = KErrNone;
    iTransactionOp = EPEngTransOpNetworkSearchStart;

    TRAPD( err, iSearchSessionHandler->OpenCspSessionL( iStatus ) );
    HandleStepIssued( EPEngSrch_OpeningCspSession, err );
    }



// -----------------------------------------------------------------------------
// CPEngSearchControl::IssueContinueSearchFromNetwork()
// -----------------------------------------------------------------------------
TInt CPEngSearchControl::IssueContinueSearchFromNetwork( TInt aContinueIndex )
    {
    RETURN_ERR_IF_NOT_READY();

    iDelayedErr = KErrNone;
    iTransactionOp = EPEngTransOpNetworkSearchContinue;

    TRAPD( err, iSearchHandler->ContinueSearchL( aContinueIndex, iStatus ) );
    HandleStepIssued( EPEngSrch_Searching,
                      err );

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::IssueStopSearchFromNetwork()
// -----------------------------------------------------------------------------
TInt CPEngSearchControl::IssueStopSearchFromNetwork()
    {
    RETURN_ERR_IF_NOT_READY();

    iDelayedErr = KErrNone;
    iTransactionOp = EPEngTransOpNetworkSearchStop;

    TRAPD( err, iSearchHandler->StopSearchL( iStatus ) );
    HandleStepIssued( EPEngSrch_StoppingSearch, err );

    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CPEngSearchControl::RunL()
// -----------------------------------------------------------------------------
void CPEngSearchControl::RunL()
    {
    TInt err = iStatus.Int();

    switch ( iSearchState )
        {
        case EPEngSrch_OpeningCspSession:
            {
            //Session open failed - close session
            if ( err != KErrNone )
                {
                iDelayedErr = err;
                TRAPD( err, iSearchSessionHandler->CloseCspSessionL( iStatus ) );
                HandleStepIssued( EPEngSrch_ClosingCspSession, err );
                }

            else
                {
                TRAPD( err, iSearchHandler->SearchL( iStatus ) );
                HandleStepIssued( EPEngSrch_Searching, err );
                }
            break;
            }

        case EPEngSrch_Searching:
            {
            DoNotifyObserver( err );
            break;
            }

        case EPEngSrch_StoppingSearch:
            {
            iDelayedErr = err;
            TRAPD( err, iSearchSessionHandler->CloseCspSessionL( iStatus ) );
            HandleStepIssued( EPEngSrch_ClosingCspSession, err );
            break;
            }

        case EPEngSrch_ClosingCspSession:
            {
            DoNotifyObserver( err );
            delete this;
            break;
            }

        default:
            {
            SearchPanicOrLeaveL( EPEngSearchControlRunStateMismatch, KErrGeneral );
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::RunError()
// -----------------------------------------------------------------------------
TInt CPEngSearchControl::RunError( TInt /*aError*/ )
    {
    delete this;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::DoCancel()
// -----------------------------------------------------------------------------
//
void CPEngSearchControl::DoCancel()
    {
    delete iSearchHandler;
    iSearchHandler = NULL;

    delete iSearchSessionHandler;
    iSearchSessionHandler = NULL;

    DoNotifyObserver( KErrCancel );
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::Cancel()
// -----------------------------------------------------------------------------
void CPEngSearchControl::Cancel()
    {
    if ( iCancelling )
        {
        return;
        }

    iCancelling = ETrue;
    CActive::Cancel();
    iCancelling = EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::HandleNWSessionSlotChangeL()
// From MPEngNWSessionSlotObserver2
// -----------------------------------------------------------------------------
void CPEngSearchControl::HandleNWSessionSlotChangeL(
    CPEngNWSessionSlotNotifier2& /*aNotifier*/,
    CPEngNWSessionSlotEvent2& aEvent )
    {
    if ( ( aEvent.Event() == EPEngEventAppNWPresenceSessionClosed ) ||
         ( aEvent.Event() == EPEngEventNWSessionClosedByServer ) )
        {
        iSlotNotifier->RemoveObserver( *this );
        AsyncTerminateSearch();
        }
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::HandleNWSessionSlotError()
// From MPEngNWSessionSlotObserver2
// -----------------------------------------------------------------------------
void CPEngSearchControl::HandleNWSessionSlotError(
    TInt /*aError*/,
    CPEngNWSessionSlotNotifier2& /*aNotifier*/ )
    {
    iSlotNotifier->RemoveObserver( *this );
    AsyncTerminateSearch();
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::AsyncTerminateSearch()
// -----------------------------------------------------------------------------
void CPEngSearchControl::AsyncTerminateSearch()
    {
    //Called when slots network session is closed
    //==> Search operation must release owned WV Engine sessions

    //This might be called when:
    // 1. WV Engine session open is pending -> cancel open
    // 2. Session open, search request pending on WV Engine side
    // 3. Session open, no pending search request
    // 4. Session open, search stop request pending on WV Engine side
    // 5. Session open, session close pending on WV Engine side

    //Cancel any pending operation
    if ( IsActive() )
        {
        Cancel();
        }

    //And terminate the search
    HandleStepIssued( EPEngSrch_ClosingCspSession, KErrCancel );
    }



// -----------------------------------------------------------------------------
// CPEngSearchControl::ValidateSearchCriteriasL()
// -----------------------------------------------------------------------------
void CPEngSearchControl::ValidateSearchCriteriasL(
    const RPointerArray< CPEngSearchCriteria2 >& aCriterias )
    {
    //Validate search elements & search values
    const TInt criteriaCount( aCriterias.Count() );
    if ( criteriaCount == 0 )
        {
        User::Leave( KErrArgument );
        }

    TInt ii;
    TInt jj;

    //Verify all values
    for ( ii = 0; ii < criteriaCount; ii++ )
        {

        if ( aCriterias[ii]->SearchValue().Length() == 0 )
            {
            User::Leave( KErrArgument );
            }

        if ( aCriterias[ii]->Element() == KPEngNullSearchElement )
            {
            User::Leave( KErrArgument );
            }
        }


    //Chechk  duplicates
    for ( ii = 0; ii < criteriaCount - 1; ii++ )
        {
        for ( jj = ii + 1 ; jj < criteriaCount; jj++ )
            {
            if ( aCriterias[ii]->Element() == aCriterias[jj]->Element() )
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngSearchControl::DoNotifyObserver()
// -----------------------------------------------------------------------------
void CPEngSearchControl::DoNotifyObserver( TInt aErr )
    {
    //Don't notify client if no client issued transaction pending
    if ( iTransactionOp == KErrNotFound )
        {
        return;
        }

    //Determine which error to report client
    if ( iDelayedErr != KErrNone )
        {
        iTransactionStatus->SetStatus( iDelayedErr );
        }
    else
        {
        iTransactionStatus->SetStatus( aErr );
        }

    CPEngSearchTransaction2& transInterface = iOwner->Interface();


    //Notify client
    TRAPD( err, iObserver.HandleSearchTransactionCompleteL( *iTransactionStatus,
                                                            transInterface,
                                                            iSearchResult->Interface(),
                                                            iTransactionOp ) );

    if ( err != KErrNone )
        {
        iObserver.HandleSearchTransactionError( err,
                                                transInterface,
                                                iTransactionOp );
        }

    iTransactionOp = KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngSearchControl::HandleStepIssued()
// -----------------------------------------------------------------------------
void CPEngSearchControl::HandleStepIssued( TPEngSearchState aState,
                                           TInt aErr )
    {
    iSearchState = aState;
    if ( aErr != KErrNone )
        {
        TRequestStatus* tmpStatus = &iStatus;
        User::RequestComplete( tmpStatus, aErr );
        }

    SetActive();
    }




// End of file
