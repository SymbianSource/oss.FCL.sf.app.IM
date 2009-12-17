/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Publish and subscribe implementation
*
*/



// INCLUDE FILES
#include    "CCANotifierImpl.h"
#include    "MCANotifierKeyObserver.h"
#include    "CCAPropertyObserver.h"
#include    <e32property.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCANotifierImpl::CCANotifierImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCANotifierImpl::CCANotifierImpl( MCANotifierKeyObserver* aObserver, const TUid aUid )
        : iObserver( aObserver ),
        iCatUID( aUid )
    {
    }

// -----------------------------------------------------------------------------
// CCANotifierImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCANotifierImpl* CCANotifierImpl::NewL( MCANotifierKeyObserver* aObserver, const TUid aUid )
    {
    CCANotifierImpl* self = new( ELeave ) CCANotifierImpl( aObserver, aUid );
    return self;
    }


// Destructor
CCANotifierImpl::~CCANotifierImpl()
    {
    delete iPropWatcher;
    }

// -----------------------------------------------------------------------------
// CCANotifierImpl::SetInt
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCANotifierImpl::SetInt( TNotifyKey aKey, TInt aValue )
    {
    TInt err = KErrNone;
    switch ( aKey )
        {
        case EUIPIndKey:
            {
            err = RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsUipInd, aValue ? ECoreAppUIsShow : ECoreAppUIsDoNotShow );
            break;
            }
        case EMSGToneQuitKey:
            err = RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, aValue );
            break;
        default:
            {
            return KErrNotFound;
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CCANotifierImpl::GetInt
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCANotifierImpl::GetInt( TNotifyKey aKey, TInt& aValue )
    {
    TInt err = KErrNone;
    switch ( aKey )
        {
        case EUIPIndKey:
            {
            err = RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsUipInd, aValue );
            break;
            }
        case EMSGToneQuitKey:
            {
            err = RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, aValue );
            break;
            }
        default:
            {
            err = KErrNotFound;
            break;
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CCANotifierImpl::ListenKeyChanges
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCANotifierImpl::ListenKeyChanges( TNotifyKey aKey )
    {
    TInt err = KErrNone;
    delete iPropWatcher;
    iPropWatcher = NULL;
    switch ( aKey )
        {
        case EUIPIndKey:
            {
            TRAP( err, iPropWatcher = CCAPropertyObserver::NewL( *this ) );
            if ( !err )
                {
                TRAP( err, iPropWatcher->ObservePropertyChangeL(
                          KPSUidCoreApplicationUIs, KCoreAppUIsUipInd ) );
                }
            break;
            }
        case EMSGToneQuitKey:
            {
            TRAP( err, iPropWatcher = CCAPropertyObserver::NewL( *this ) );
            if ( !err )
                {
                TRAP( err, iPropWatcher->ObservePropertyChangeL(
                          KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit ) );
                }
            break;
            }
        default:
            {
            err = KErrNotFound;
            break;
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CCANotifierImpl::HandlePropertyNotificationEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCANotifierImpl::HandlePropertyNotificationEventL( TUid /* aCategory */, TUint aKey )
    {
    TInt value( 0 );
    User::LeaveIfError(
        RProperty::Get( KPSUidCoreApplicationUIs, aKey, value ) );
    TUpdatedKey key;
    key.iUid = KPSUidCoreApplicationUIs;
    key.iKey = aKey;
    key.iValue = value;
    iObserver->HandleKeyChange( key );
    }

//  End of File
