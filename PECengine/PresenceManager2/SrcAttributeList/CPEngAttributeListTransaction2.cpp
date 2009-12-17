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
* Description:  Presence contact list transactions.
*
*/

// INCLUDE FILES
#include "CPEngAttributeListTransaction2.h"
#include "CPEngAttributeListTransaction2Imp.h"
#include "TPEngMDesCArrayAdapter.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngAttributeListTransaction2* CPEngAttributeListTransaction2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeListTransaction2* self = CPEngAttributeListTransaction2::NewLC(
                                               aNWSessionSlotID,
                                               aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngAttributeListTransaction2* CPEngAttributeListTransaction2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeListTransaction2* self = new ( ELeave ) CPEngAttributeListTransaction2;
    CleanupStack::PushL( self );
    self->iImp = CPEngAttributeListTransaction2Imp::NewL( *self,
                                                          aPriority,
                                                          aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngAttributeListTransaction2::~CPEngAttributeListTransaction2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2::CPEngAttributeListTransaction2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransaction2::CPEngAttributeListTransaction2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2::IsPublishAttributeListsActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngAttributeListTransaction2::IsPublishAttributeListsActive() const
    {
    return iImp->IsPublishAttributeListsActive();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2::PublishAttributeLists()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeListTransaction2::PublishAttributeLists(
    MPEngAttributeListTransactionObserver2& aObserver )
    {
    return iImp->PublishAttributeLists( aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2::CancelPublishAttributeLists()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngAttributeListTransaction2::CancelPublishAttributeLists()
    {
    iImp->CancelPublishAttributeLists();
    }


// End of File


