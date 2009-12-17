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
* Description:  Presence search transactions.
*
*/

// INCLUDE FILES
#include "CPEngSearchTransaction2Imp.h"
#include "CPEngAsyncOperation.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "CPEngSearchControl.h"

#include <CPEngNWSessionSlotID2.h>
#include <CPEngSearchCriteria2.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSearchTransaction2Imp* CPEngSearchTransaction2Imp::NewL(
    CPEngSearchTransaction2& aInterface,
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngSearchTransaction2Imp* self = new ( ELeave )
    CPEngSearchTransaction2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngSearchTransaction2Imp::~CPEngSearchTransaction2Imp()
    {
    delete iSearchCntrl; //Deleting search control cancels also it
    delete iUsedSlot;
    delete iUsedSlotId;
    }



// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::CPEngSearchTransaction2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSearchTransaction2Imp::CPEngSearchTransaction2Imp(
    CPEngSearchTransaction2& aInterface,
    TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }



// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngSearchTransaction2Imp::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iUsedSlotId = aNWSessionSlotID.CloneL();
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::IsSearchFromNetworkActive()
// -----------------------------------------------------------------------------
//
TBool CPEngSearchTransaction2Imp::IsSearchFromNetworkActive() const
    {
    return ( iSearchCntrl != NULL );    // CSI: 64 #
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::SearchFromNetwork()
// -----------------------------------------------------------------------------
//
TInt CPEngSearchTransaction2Imp::SearchFromNetwork(
    CPEngSearchCriteria2*& aCriteria,
    TInt aSearchLimit,
    MPEngSearchTransactionObserver2& aObserver )
    {
    RPointerArray<CPEngSearchCriteria2> adapter;
    TInt err = adapter.Append( aCriteria );
    if ( err == KErrNone )
        {
        err = SearchFromNetwork( adapter, aSearchLimit, aObserver );
        }

    if ( err == KErrNone )
        {
        aCriteria = NULL;
        }

    adapter.Close();
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::SearchFromNetwork()
// -----------------------------------------------------------------------------
//
TInt CPEngSearchTransaction2Imp::SearchFromNetwork(
    RPointerArray< CPEngSearchCriteria2 >& aCriterias,
    TInt aSearchLimit,
    MPEngSearchTransactionObserver2& aObserver )
    {
    if ( iSearchCntrl )
        {
        return KErrInUse;
        }


    CPEngSearchControl* cntrl = NULL;
    TRAPD( err,
           cntrl = CPEngSearchControl::NewL( iCActivePriority,
                                             *iUsedSlotId,
                                             aCriterias,
                                             aSearchLimit,
                                             aObserver ); );

    if ( err == KErrNone )
        {
        //Success.
        //Ownership of passed search criterias is taken here
        iSearchCntrl = cntrl;
        iSearchCntrl->IssueSearch();
        iSearchCntrl->SetOwner( *this );
        aCriterias.ResetAndDestroy();
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::ContinueSearchFromNetwork()
// -----------------------------------------------------------------------------
//
TInt CPEngSearchTransaction2Imp::ContinueSearchFromNetwork( TInt aContinueIndex )
    {
    if ( !iSearchCntrl )
        {
        return KErrNotFound;
        }

    return iSearchCntrl->IssueContinueSearchFromNetwork( aContinueIndex );
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::StopSearchFromNetwork()
// -----------------------------------------------------------------------------
//
TInt CPEngSearchTransaction2Imp::StopSearchFromNetwork()
    {
    if ( !iSearchCntrl )
        {
        return KErrNotFound;
        }

    return iSearchCntrl->IssueStopSearchFromNetwork();
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::HandleSearchDestruction()
// -----------------------------------------------------------------------------
//
void CPEngSearchTransaction2Imp::HandleSearchDestruction(
    CPEngSearchControl* /*aSearch*/ )
    {
    iSearchCntrl = NULL;
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2Imp::Interface()
// -----------------------------------------------------------------------------
//
CPEngSearchTransaction2& CPEngSearchTransaction2Imp::Interface()
    {
    return iInterface;
    }

// End of File


