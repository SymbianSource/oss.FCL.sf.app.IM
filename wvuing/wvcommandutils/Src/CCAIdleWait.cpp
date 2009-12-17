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
* Description:  Helper class for idle wait
 *
*/


// INCLUDE FILES
#include "CCAIdleWait.h"
#include "ChatDefinitions.h"
#include "ChatDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCAIdleWait::IdleWaitL()
// ---------------------------------------------------------
//
void CCAIdleWait::IdleWaitL( const TInt* aCallbackFlag /* = NULL */ )
    {
    CHAT_DP_FUNC_ENTER( "CCAIdleWait::IdleWaitL" );
    CCAIdleWait* wait = new( ELeave )CCAIdleWait( aCallbackFlag );
    CleanupStack::PushL( wait );
    wait->DoIdleWaitL();
    CleanupStack::PopAndDestroy( wait );
    CHAT_DP_FUNC_ENTER( "CCAIdleWait::Done" );
    }

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
//
CCAIdleWait::CCAIdleWait( const TInt* aCallbackFlag )
        : iCallbackFlag( aCallbackFlag )
    {
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CCAIdleWait::~CCAIdleWait()
    {
    CHAT_DP_FUNC_ENTER( "CCAIdleWait::~CCAIdleWait" );
    DoIdleStop();
    delete iIdle;
    CHAT_DP_FUNC_DONE( "CCAIdleWait::~CCAIdleWait" );
    }

// ---------------------------------------------------------
// CCAIdleWait::DoIdleWaitL()
// ---------------------------------------------------------
//
void CCAIdleWait::DoIdleWaitL()
    {
    CHAT_DP_FUNC_ENTER( "CCAIdleWait::DoIdleWaitL" );
    // create the idle callback
    iIdle = CIdle::NewL( CActive::EPriorityIdle );
    iIdle->Start( TCallBack( Callback, this ) );

    // and wait for completion
    // not a active object hence codescanner warning can be ignored
    if ( ! iWait.IsStarted() )
        {
        iWait.Start();  // CSI: 10 # iWait is not an active object
        }
    CHAT_DP_FUNC_DONE( "CCAIdleWait::DoIdleWaitL" );
    }

// ---------------------------------------------------------
// CCAIdleWait::DoIdleStop()
// ---------------------------------------------------------
//
void CCAIdleWait::DoIdleStop()
    {
    CHAT_DP_FUNC_ENTER( "CCAIdleWait::DoIdleStop" );
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    CHAT_DP_FUNC_DONE( "CCAIdleWait::DoIdleStop" );
    }

// -----------------------------------------------------------------------------
// CCAIdleWait::Callback
// -----------------------------------------------------------------------------
//
TBool CCAIdleWait::Callback( TAny* aInstance )
    {
    CHAT_DP_FUNC_ENTER( "CCAIdleWait::Callback" );
    CCAIdleWait* wait = static_cast<CCAIdleWait*>( aInstance );
    // stop the waiting
    const TInt* flag = wait->iCallbackFlag;
    if ( flag )
        {
        if ( *flag )
            {
            // Continue waiting.
            return ETrue;
            }
        }
    wait->DoIdleStop();
    CHAT_DP_FUNC_DONE( "CCAIdleWait::Callback" );
    return EFalse;
    }

//  End of File
