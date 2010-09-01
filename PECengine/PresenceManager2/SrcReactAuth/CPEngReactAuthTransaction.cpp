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
* Description:  Presence reactive authorization transactions.
*
*/

// INCLUDE FILES
#include "CPEngReactAuthTransaction.h"
#include "CPEngReactAuthTransactionImp.h"
#include "TPEngMDesCArrayAdapter.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngReactAuthTransaction::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngReactAuthTransaction* CPEngReactAuthTransaction::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngReactAuthTransaction* self = CPEngReactAuthTransaction::NewLC(
                                          aNWSessionSlotID,
                                          aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthTransaction::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngReactAuthTransaction* CPEngReactAuthTransaction::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngReactAuthTransaction* self = new ( ELeave ) CPEngReactAuthTransaction;
    CleanupStack::PushL( self );
    self->iImp = CPEngReactAuthTransactionImp::NewL( *self,
                                                     aPriority,
                                                     aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngReactAuthTransaction::~CPEngReactAuthTransaction()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngReactAuthTransaction::CPEngReactAuthTransaction
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngReactAuthTransaction::CPEngReactAuthTransaction()
    {
    }



// -----------------------------------------------------------------------------
// CPEngReactAuthTransaction::IsPublishReactAuthRespondsActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngReactAuthTransaction::IsPublishReactAuthRespondsActive() const
    {
    return iImp->IsPublishReactAuthRespondsActive();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthTransaction::PublishReactAuthResponds()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngReactAuthTransaction::PublishReactAuthResponds(
    MPEngAuthorizationRespond*& aResponds,
    MPEngReactAuthTransactionObserver& aObserver )
    {
    return iImp->PublishReactAuthResponds( aResponds, aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthTransaction::CancelPublishReactAuthResponds()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngReactAuthTransaction::CancelPublishReactAuthResponds()
    {
    iImp->CancelPublishReactAuthResponds();
    }



// End of File


