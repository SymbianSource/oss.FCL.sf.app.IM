/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Queue class for IMPS tid.
*
*
*/


// INCLUDE FILES
#include    <e32std.h>
#include    <e32base.h>
#include    <badesca.h>
#include    "impstidqueue.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ---------------------------------------------------------
// CImpsTidQueue::NewL
// ---------------------------------------------------------
//
CImpsTidQueue* CImpsTidQueue::NewL()
    {
    CImpsTidQueue* self = new ( ELeave ) CImpsTidQueue( 6 );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CImpsTidQueue::ConstructL
// ---------------------------------------------------------
//
void CImpsTidQueue::ConstructL()
    {
    iArray = new ( ELeave ) CDesCArrayFlat( 6 );
    }

// -----------------------------------------------------------------------------
// CImpsTidQueue::CImpsTidQueue
// Constructor
// -----------------------------------------------------------------------------
//
CImpsTidQueue::CImpsTidQueue( TInt aCapacity ):
        iCapacity( aCapacity ),
        iNext( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CImpsTidQueue::~CImpsTidQueue
// Destructor
// -----------------------------------------------------------------------------
//
CImpsTidQueue::~CImpsTidQueue()
    {
    if ( iArray )
        {
        iArray->Reset();
        delete iArray;
        }
    }

// -----------------------------------------------------------------------------
// CImpsTidQueue::Add
// Adds a new orphan message to the queue
// -----------------------------------------------------------------------------
//
void CImpsTidQueue::Add( const TDesC& aTid )
    {
    TInt err = KErrNone;
    iNext++;
    if ( iNext > iCapacity )
        {
        iArray->Delete( 0 );
        iArray->Compress();
        }
    TRAP( err, iArray->AppendL( aTid ) );
    }

// -----------------------------------------------------------------------------
// CImpsTidQueue::TidFound
// -----------------------------------------------------------------------------
//
TBool CImpsTidQueue::TidFound( const TDesC& aTid )
    {
    TInt index = -1;
    TInt ret = iArray->Find( aTid, index );
    return ret ? EFalse : ETrue;
    }

// -----------------------------------------------------------------------------
// CImpsTidQueue::DeleteAll
// -----------------------------------------------------------------------------
//
void CImpsTidQueue::DeleteAll()
    {
    iNext = 0;
    iArray->Reset();
    iArray->Compress();
    }



//  End of File
