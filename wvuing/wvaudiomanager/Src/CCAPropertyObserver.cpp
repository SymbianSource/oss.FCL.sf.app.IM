/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "CCAPropertyObserver.h"
#include "MCAPropertyNotificationObserver.h"

#include <bautils.h>
#include <barsc.h>
#include <e32property.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAPropertyObserver::CCAPropertyObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAPropertyObserver::CCAPropertyObserver(
    MCAPropertyNotificationObserver& aObserver )
        : CActive( CActive::EPriorityLow ),
        iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CCAPropertyObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAPropertyObserver* CCAPropertyObserver::NewL(
    MCAPropertyNotificationObserver& aObserver )
    {
    CCAPropertyObserver* self = new( ELeave ) CCAPropertyObserver( aObserver );
    return self;
    }


// Destructor
CCAPropertyObserver::~CCAPropertyObserver()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    }


// -----------------------------------------------------------------------------
// CCAPropertyObserver::ObservePropertyChangeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPropertyObserver::ObservePropertyChangeL( TUid aCategory, TUint aKey )
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
// CCAPropertyObserver::CancelObserve
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPropertyObserver::CancelObserve()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CCAPropertyObserver::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPropertyObserver::RunL()
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    iObserver.HandlePropertyNotificationEventL( iCategory, iKey );
    }

// -----------------------------------------------------------------------------
// CCAPropertyObserver::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPropertyObserver::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CCAPropertyObserver::Category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CCAPropertyObserver::Category()
    {
    return iCategory;
    }

// -----------------------------------------------------------------------------
// CCAPropertyObserver::Key
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint CCAPropertyObserver::Key()
    {
    return iKey;
    }

//  End of File
