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
#include "MIMPSSystemNotifierObserver.h"
#include "CIMPSSystemNotifierPubSub.h"
#include "CIMPSPropertyObserver.h"
#include "IMPSSystemNotifyDefs.h"

#include <sacls.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierPubSub::CIMPSSystemNotifierPubSub
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSystemNotifierPubSub::CIMPSSystemNotifierPubSub(
    MIMPSSystemNotifierObserver& aObserver,
    const TUid aKey
)
        : iObserver( aObserver ),
        iKey( aKey )
    {
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierPubSub::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSSystemNotifierPubSub::ConstructL()
    {
    iPropertyObserver = CIMPSPropertyObserver::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierPubSub::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSystemNotifierPubSub* CIMPSSystemNotifierPubSub::NewL(
    MIMPSSystemNotifierObserver& aObserver,
    TUid aKey )
    {
    CIMPSSystemNotifierPubSub* self =
        new( ELeave ) CIMPSSystemNotifierPubSub( aObserver, aKey );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CIMPSSystemNotifierPubSub::~CIMPSSystemNotifierPubSub()
    {
    delete iPropertyObserver;
    }


// -----------------------------------------------------------------------------
// CIMPSSystemNotifierPubSub::Subscribe
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSystemNotifierPubSub::Subscribe()
    {
    TInt err( KErrNone );
    TRAP( err,
          iPropertyObserver->ObservePropertyChangeL( KUidSystemCategory, iKey.iUid ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierPubSub::UnSubscribe
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSystemNotifierPubSub::UnSubscribe()
    {
    iPropertyObserver->CancelObserve();
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierPubSub::GetIntKey
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSystemNotifierPubSub::GetIntKey( TInt& aValue )
    {
    TInt err = RProperty::Get( KUidSystemCategory, iKey.iUid, aValue );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierPubSub::HandlePropertyNotificationEventL( TUid /*aCategory*/, TUint /*aKey*/ )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSystemNotifierPubSub::HandlePropertyNotificationEventL( TUid /*aCategory*/, TUint /*aKey*/ )
    {
    TInt value( KErrNone );
    User::LeaveIfError( RProperty::Get( KUidSystemCategory, iKey.iUid, value ) );
    iObserver.HandleSystemNotifierEventL( iKey, value );
    }

