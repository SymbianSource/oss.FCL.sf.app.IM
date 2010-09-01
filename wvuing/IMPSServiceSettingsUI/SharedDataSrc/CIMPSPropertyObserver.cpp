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
#include "CIMPSPropertyObserver.h"
#include "MIMPSPropertyNotificationObserver.h"

#include <bautils.h>
#include <barsc.h>
#include <e32property.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::CMGXPropertyObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSPropertyObserver::CIMPSPropertyObserver( MIMPSPropertyNotificationObserver& aObserver )
        : CActive( CActive::EPriorityLow ),
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSPropertyObserver::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSPropertyObserver* CIMPSPropertyObserver::NewL( MIMPSPropertyNotificationObserver& aObserver )
    {
    CIMPSPropertyObserver* self = new( ELeave ) CIMPSPropertyObserver( aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CIMPSPropertyObserver::~CIMPSPropertyObserver()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    }


// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::ObservePropertyChangeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPropertyObserver::ObservePropertyChangeL( TUid aCategory, TUint aKey )
    {
    if ( IsActive() )
        {
        return;
        }
    User::LeaveIfError( iProperty.Attach( aCategory, aKey ) );
    iKey = aKey;
    iCategory = aCategory;
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::CancelObserve
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPropertyObserver::CancelObserve()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPropertyObserver::RunL()
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    iObserver.HandlePropertyNotificationEventL( iCategory, iKey );
    }

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPropertyObserver::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::Category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CIMPSPropertyObserver::Category()
    {
    return iCategory;
    }

// -----------------------------------------------------------------------------
// CIMPSPropertyObserver::Key
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint CIMPSPropertyObserver::Key()
    {
    return iKey;
    }

//  End of File
