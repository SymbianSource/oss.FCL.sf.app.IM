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
*				 engine
*
*/



// INCLUDE FILES
#include "MPEngAOCallObserver.h"
#include "CPEngAOCallObserver.h"
#include "IMPSCommonUiDebugPrint.h"
#include "CIMPSSharedDataFactory.h"
#include "IMPSSystemNotifyDefs.h"

#include <flogger.h>
#include <e32std.h>


// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAOCallObserver::CPEngAOCallObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAOCallObserver::CPEngAOCallObserver() :
        iObserver( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CPEngAOCallObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAOCallObserver::ConstructL()
    {
    iSystemNotifier =
        CIMPSSharedDataFactory::CreateSystemNotifyHandlerL( *this,
                                                            KIMPSSystemUidCurrentCall );
    }

// -----------------------------------------------------------------------------
// CPEngAOCallObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAOCallObserver* CPEngAOCallObserver::NewL()
    {
    CPEngAOCallObserver* self = new( ELeave ) CPEngAOCallObserver;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CPEngAOCallObserver::~CPEngAOCallObserver()
    {
    delete iSystemNotifier;
    }

// ---------------------------------------------------------
// CPEngAOCallObserver::HandleSystemNotifierEventL
//
// ---------------------------------------------------------
//
void CPEngAOCallObserver::HandleSystemNotifierEventL( TUid /*aKey*/, TInt aValue )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT(
                    "CPEngAOCallObserver::HandleSystemNotifierEventL  aValue [%d]" ), aValue );

    // the call ended, inform the observer
    if ( aValue != EIMPSSystemCallNone )
        {
        // something else happened, inform that the call didn't end just yet
        HandleCallEndedL( EFalse );

        }
    else
        {
        // Stop listening call state changes
        iSystemNotifier->UnSubscribe();

        HandleCallEndedL( ETrue );
        }
    }

// ---------------------------------------------------------
// CPEngAOCallObserver::CallActive
//
// ---------------------------------------------------------
//
TBool CPEngAOCallObserver::CallActive( MPEngAOCallObserver*
// Compile warning removal.
#ifndef _DEBUG
                                       aObserver
#endif
                                     )
    {
#ifdef _DEBUG
    return EFalse;
#else // #else used instead of #endif, to get rvct compile warning removed

    // get the current state of the call from system notifier
    TInt err( KErrNone );
    TInt value( EIMPSSystemCallNone );

    err = iSystemNotifier->GetIntKey( value );
    IMPSCUI_DP( D_IMPSCUI_LIT( "CPEngAOCallObserver::CallActive state [%d]" ), value );

    // no active call or an error in GetState -> return
    if ( ( value <= EIMPSSystemCallNone && ( err == KErrNone ) ) ||
         ( err != KErrNone ) )
        {
        return EFalse;
        }

    // start observing for call status change
    iObserver = aObserver;

    iSystemNotifier->Subscribe();

    // and tell the calling object that the call is on and we are
    // observing it
    return ETrue;
#endif
    }

// ---------------------------------------------------------
// CPEngAOCallObserver::HandleCallEndedL
//
// ---------------------------------------------------------
//
void CPEngAOCallObserver::HandleCallEndedL( TBool aEnded )
    {
    if ( iObserver )
        {
        iObserver->HandleCallEndedL( aEnded );
        }
    }

// ---------------------------------------------------------
// CPEngAOCallObserver::StopObserving
//
// ---------------------------------------------------------
//
void CPEngAOCallObserver::StopObserving()
    {
    iSystemNotifier->UnSubscribe();
    }
//  End of File
