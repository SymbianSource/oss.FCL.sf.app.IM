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
* Description:  Handles the always online functionality for the presence
*				 engine and instant messaging application
*
*/



// INCLUDE FILES
#include <flogger.h>

#include "MPEngAONwStatusObserver.h"
#include "CPEngAONwStatusObserver.h"
#include "CIMPSSharedDataFactory.h"
#include "IMPSSystemNotifyDefs.h"

#include "ImpsCommonUiDebugPrint.h"

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAONwStatusObserver::CPEngAONwStatusObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAONwStatusObserver::CPEngAONwStatusObserver( MPEngAONwStatusObserver& aObserver ) :
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CPEngAONwStatusObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAONwStatusObserver::ConstructL()
    {
    iSystemNotifier =
        CIMPSSharedDataFactory::CreateSystemNotifyHandlerL( *this,
                                                            KIMPSSystemUidNetworkStatus );
    }

// -----------------------------------------------------------------------------
// CPEngAONwStatusObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAONwStatusObserver* CPEngAONwStatusObserver::NewL( MPEngAONwStatusObserver& aObserver )
    {
    CPEngAONwStatusObserver* self = new( ELeave ) CPEngAONwStatusObserver( aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CPEngAONwStatusObserver::~CPEngAONwStatusObserver()
    {
    delete iSystemNotifier;
    }

// ---------------------------------------------------------
// CPEngAONwStatusObserver::HandleSystemNotifierEventL
//
// ---------------------------------------------------------
//
void CPEngAONwStatusObserver::HandleSystemNotifierEventL( TUid /*aKey*/, TInt
// Compile warning removal
#ifndef _DEBUG
                                                          aValue
#endif
                                                        )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CPEngAONwStatusObserver::RunL" ) );

#ifdef _DEBUG
    iObserver.HandleNwStatusChange( ETrue );
    return;
#else // #else used instead of #endif, to get rvct compile warning removed
    if ( aValue != EIMPSSystemNetworkAvailable )
        {
        iObserver.HandleNwStatusChange( EFalse );
        }
    else
        {
        iSystemNotifier->UnSubscribe();
        iObserver.HandleNwStatusChange( ETrue );
        }
#endif
    }

// ---------------------------------------------------------
// CPEngAONwStatusObserver::IsNetworkAvailable
//
// ---------------------------------------------------------
//
TBool CPEngAONwStatusObserver::IsNetworkAvailable()
    {
#ifdef _DEBUG
    return ETrue;
#else // #else used instead of #endif, to get rvct compile warning removed
    TInt err( KErrNone );
    TInt value( KErrNone );

    err = iSystemNotifier->GetIntKey( value );

    if ( ( ( value == EIMPSSystemNetworkAvailable ) && ( err == KErrNone ) )
         || ( err != KErrNone ) )
        {
        return ETrue;
        }
    else
        {
        // start observing network status
        iSystemNotifier->Subscribe();
        return EFalse;
        }
#endif
    }

// ---------------------------------------------------------
// CPEngAONwStatusObserver::StopObserving
//
// ---------------------------------------------------------
//
void CPEngAONwStatusObserver::StopObserving()
    {
    iSystemNotifier->UnSubscribe();
    }

//  End of File
