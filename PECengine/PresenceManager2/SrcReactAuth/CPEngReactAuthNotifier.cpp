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
* Description:  Notifier API to listen presence reactive authorization changes.
*
*/

// INCLUDE FILES
#include "CPEngReactAuthNotifier.h"
#include "CPEngReactAuthNotifierImp.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifier::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngReactAuthNotifier* CPEngReactAuthNotifier::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngReactAuthNotifier* self = CPEngReactAuthNotifier::NewLC(
                                       aNWSessionSlotID,
                                       aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifier::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngReactAuthNotifier* CPEngReactAuthNotifier::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngReactAuthNotifier* self = new ( ELeave ) CPEngReactAuthNotifier;
    CleanupStack::PushL( self );
    self->iImp = CPEngReactAuthNotifierImp::NewL( *self,
                                                  aPriority,
                                                  aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngReactAuthNotifier::~CPEngReactAuthNotifier()
    {
    delete iImp;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifier::CPEngReactAuthNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngReactAuthNotifier::CPEngReactAuthNotifier()
    {
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifier::Start()
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CPEngReactAuthNotifier::Start( TInt aOptions )
    {
    return iImp->Start( aOptions );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifier::Stop()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngReactAuthNotifier::Stop()
    {
    iImp->Stop();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifier::IsActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngReactAuthNotifier::IsActive() const
    {
    return iImp->IsActive();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifier::AddObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngReactAuthNotifier::AddObserver(
    MPEngReactAuthObserver& aObserver )

    {
    return iImp->AddObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifier::RemoveObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngReactAuthNotifier::RemoveObserver(
    MPEngReactAuthObserver& aObserver )

    {
    return iImp->RemoveObserver( aObserver );
    }


// End of File


