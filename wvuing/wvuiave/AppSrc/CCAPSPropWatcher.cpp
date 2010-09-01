/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Publish & Subscribe property watcher
*
*/



// INCLUDE FILES
#include    "CCAPSPropWatcher.h"
#include    "MCAPSPropChangeObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAPSPropWatcher::CCAPSPropWatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAPSPropWatcher::CCAPSPropWatcher( MCAPSPropChangeObserver* aObserver )
        : CActive( CActive::EPriorityIdle ),
        iObserver( aObserver )
    {

    }

// -----------------------------------------------------------------------------
// CCAPSPropWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAPSPropWatcher::ConstructL( TUid aCategory, TUint aKey )
    {
    User::LeaveIfError( iProperty.Attach( aCategory, aKey ) );
    CActiveScheduler::Add( this );
    RunL();
    }

// -----------------------------------------------------------------------------
// CCAPSPropWatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAPSPropWatcher* CCAPSPropWatcher::NewL( MCAPSPropChangeObserver* aObserver, TUid aCategory, TUint aKey )
    {
    CCAPSPropWatcher* self = new( ELeave ) CCAPSPropWatcher( aObserver );

    CleanupStack::PushL( self );
    self->ConstructL( aCategory, aKey );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CCAPSPropWatcher::~CCAPSPropWatcher()
    {
    Cancel();
    iProperty.Close();
    }


// -----------------------------------------------------------------------------
// CCAPSPropWatcher::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPSPropWatcher::RunL()
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    if ( iObserver )
        {
        iObserver->HandlePropertyChanged();
        }
    }

// -----------------------------------------------------------------------------
// CCAPSPropWatcher::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPSPropWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

//  End of File
