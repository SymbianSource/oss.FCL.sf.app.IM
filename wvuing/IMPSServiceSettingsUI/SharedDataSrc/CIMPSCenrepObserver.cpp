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
* Description:  Publish & Subscribe property change observer implementation
*
*/



// INCLUDE FILES
#include "CIMPSCenrepObserver.h"
#include "MIMPSPropertyNotificationObserver.h"

#include <bautils.h>
#include <barsc.h>
#include <centralrepository.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSCenrepObserver::CIMPSCenrepObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSCenrepObserver::CIMPSCenrepObserver( MIMPSPropertyNotificationObserver& aObserver )
        : CActive( CActive::EPriorityLow ),
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSCenrepObserver::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSCenrepObserver* CIMPSCenrepObserver::NewL( MIMPSPropertyNotificationObserver& aObserver )
    {
    CIMPSCenrepObserver* self = new( ELeave ) CIMPSCenrepObserver( aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CIMPSCenrepObserver::~CIMPSCenrepObserver()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    delete iRepository;
    }


// -----------------------------------------------------------------------------
// CIMPSCenrepObserver::ObservePropertyChangeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepObserver::ObservePropertyChangeL( TUid aCategory, TUint aKey )
    {
    if ( IsActive() )
        {
        return;
        }

    CRepository* tempRepository = CRepository::NewL( aCategory );
    delete iRepository;
    iRepository = tempRepository;

    User::LeaveIfError( iRepository->NotifyRequest( aKey, iStatus ) );

    iKey = aKey;
    iCategory = aCategory;

    SetActive();
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepObserver::CancelObserve
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepObserver::CancelObserve()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepObserver::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepObserver::RunL()
    {
    // subscribe to a new notification
    User::LeaveIfError( iRepository->NotifyRequest( iKey, iStatus ) );
    SetActive();
    // notify observer
    iObserver.HandlePropertyNotificationEventL( iCategory, iKey );
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepObserver::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepObserver::DoCancel()
    {
    iRepository->NotifyCancel( iKey );
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepObserver::Category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CIMPSCenrepObserver::Category()
    {
    return iCategory;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepObserver::Key
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint CIMPSCenrepObserver::Key()
    {
    return iKey;
    }

//  End of File
