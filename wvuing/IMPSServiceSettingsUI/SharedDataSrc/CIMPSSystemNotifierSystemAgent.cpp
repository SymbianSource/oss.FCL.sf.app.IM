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
#include "CIMPSSystemNotifierSystemAgent.h"
#include "IMPSSystemNotifyDefs.h"
#include "IMPSCommonUiDebugPrint.h"

#include <sacls.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierSystemAgent::CIMPSSystemNotifierSystemAgent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSystemNotifierSystemAgent::CIMPSSystemNotifierSystemAgent(
    MIMPSSystemNotifierObserver& aObserver,
    const TUid aKey
)
        : CActive( CActive::EPriorityStandard ),
        iObserver( aObserver ),
        iKey( aKey )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierSystemAgent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSSystemNotifierSystemAgent::ConstructL()
    {
    User::LeaveIfError( iSysAgent.Connect() );
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierSystemAgent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSystemNotifierSystemAgent* CIMPSSystemNotifierSystemAgent::NewL(
    MIMPSSystemNotifierObserver& aObserver,
    TUid aKey )
    {
    CIMPSSystemNotifierSystemAgent* self =
        new( ELeave ) CIMPSSystemNotifierSystemAgent( aObserver, aKey );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CIMPSSystemNotifierSystemAgent::~CIMPSSystemNotifierSystemAgent()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    iSysAgent.Close();
    }


// -----------------------------------------------------------------------------
// CIMPSSystemNotifierSystemAgent::Subscribe
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSystemNotifierSystemAgent::Subscribe()
    {
    if ( IsActive() )
        {
        return KErrNone;
        }

    iAgentEvent.SetRequestStatus( iStatus );
    iAgentEvent.SetUid( iKey );

    iSysAgent.NotifyOnEvent( iAgentEvent );
    SetActive();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierSystemAgent::UnSubscribe
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSystemNotifierSystemAgent::UnSubscribe()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierSystemAgent::GetIntKey
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSystemNotifierSystemAgent::GetIntKey( TInt& aValue )
    {
    aValue = iSysAgent.GetState( iKey );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierSystemAgent::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSystemNotifierSystemAgent::RunL()
    {
    TInt value( iAgentEvent.State() );
    iSysAgent.NotifyOnEvent( iAgentEvent );
    SetActive();
    iObserver.HandleSystemNotifierEventL( iKey, value );
    }

// -----------------------------------------------------------------------------
// CIMPSSystemNotifierSystemAgent::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSystemNotifierSystemAgent::DoCancel()
    {
    iSysAgent.NotifyEventCancel();
    }


