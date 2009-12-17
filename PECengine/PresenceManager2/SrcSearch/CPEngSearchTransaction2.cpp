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
#include "CPEngSearchTransaction2.h"
#include "CPEngSearchTransaction2Imp.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSearchTransaction2* CPEngSearchTransaction2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngSearchTransaction2* self = CPEngSearchTransaction2::NewLC(
                                        aNWSessionSlotID,
                                        aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSearchTransaction2* CPEngSearchTransaction2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngSearchTransaction2* self = new ( ELeave ) CPEngSearchTransaction2;
    CleanupStack::PushL( self );
    self->iImp = CPEngSearchTransaction2Imp::NewL( *self,
                                                   aPriority,
                                                   aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngSearchTransaction2::~CPEngSearchTransaction2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngSearchTransaction2::CPEngSearchTransaction2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSearchTransaction2::CPEngSearchTransaction2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2::IsSearchFromNetworkActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngSearchTransaction2::IsSearchFromNetworkActive() const
    {
    return iImp->IsSearchFromNetworkActive();
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2::SearchFromNetwork()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSearchTransaction2::SearchFromNetwork(
    CPEngSearchCriteria2*& aCriteria,
    TInt aSearchLimit,
    MPEngSearchTransactionObserver2& aObserver )
    {
    return iImp->SearchFromNetwork( aCriteria,
                                    aSearchLimit,
                                    aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2::SearchFromNetwork()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSearchTransaction2::SearchFromNetwork(
    RPointerArray< CPEngSearchCriteria2 >& aCriterias,
    TInt aSearchLimit,
    MPEngSearchTransactionObserver2& aObserver )
    {
    return iImp->SearchFromNetwork( aCriterias,
                                    aSearchLimit,
                                    aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2::ContinueSearchFromNetwork()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSearchTransaction2::ContinueSearchFromNetwork( TInt aContinueIndex )
    {
    return iImp->ContinueSearchFromNetwork( aContinueIndex );
    }


// -----------------------------------------------------------------------------
// CPEngSearchTransaction2::StopSearchFromNetwork()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSearchTransaction2::StopSearchFromNetwork()
    {
    return iImp->StopSearchFromNetwork();
    }


// End of File


