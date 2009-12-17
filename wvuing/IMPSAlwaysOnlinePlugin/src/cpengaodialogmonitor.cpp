/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains the implementation of dialog monitor
 *
*/



// INCLUDE FILES
#include "cpengaodialogmonitor.h"
#include "mpengaodialogstatusnotifier.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAODialogMonitor::CPEngAODialogMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAODialogMonitor::CPEngAODialogMonitor( MPEngAODialogStatusNotifier& aNotifier ) :
        CActive( CActive::EPriorityStandard ),
        iNotifier( aNotifier )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAODialogMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAODialogMonitor::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPEngAODialogMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAODialogMonitor* CPEngAODialogMonitor::NewL( MPEngAODialogStatusNotifier& aNotifier )
    {
    CPEngAODialogMonitor* self = new( ELeave ) CPEngAODialogMonitor( aNotifier );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// CPEngAODialogMonitor::~CPEngAODialogMonitor()
// Destructor
//-----------------------------------------------------------------------------
//
CPEngAODialogMonitor::~CPEngAODialogMonitor()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    }

//-----------------------------------------------------------------------------
// CPEngAODialogMonitor::Start()
// Starts monitoring the status
//-----------------------------------------------------------------------------
//
void CPEngAODialogMonitor::Start()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    SetActive();
    }

//----------------------------------------------------------------------------
// CPEngAODialogMonitor::DoCancel()
// Cancellation of an outstanding request.
//----------------------------------------------------------------------------
//
void CPEngAODialogMonitor::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CPEngAODialogMonitor::RunL()
// Handles object’s request completion event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAODialogMonitor::RunL()
    {
    iNotifier.CommonDialogDismissedL( iStatus.Int() );
    }

//  End of File
//
//
