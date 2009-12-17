/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Connection UI interprocess signaller.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "CCnUiSignaller.h"


// ================= GLOBAL FUNCTIONS ====================
// -----------------------------------------------------------------------------
// CreateConnUiSignallerL()
// -----------------------------------------------------------------------------
//
GLREF_C MCnUiSignaller* CreateConnUiSignallerL()
    {
    return CCnUiSignaller::NewL();
    }

GLREF_C MCnUiSignaller* CreateConnUiSignallerLC()
    {
    CCnUiSignaller* self = CCnUiSignaller::NewL();
    CleanupStack::PushL( self );
    return self;
    }



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiSignaller* CCnUiSignaller::NewL()
    {
    CCnUiSignaller* self = new ( ELeave ) CCnUiSignaller();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self
    return self;
    }


// Destructor
CCnUiSignaller::~CCnUiSignaller()
    {
    //no need to explicitly to cancel the
    //signal ==> closing will do it automaticly
    delete iOperationSignalChannel;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiSignaller::CCnUiSignaller()
        : iOperationSignalRequested( EFalse )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiSignaller::ConstructL()
    {
    iOperationSignalChannel = CCnUiGroupChannel::NewL( ECnUiGlobalGroup,
                                                       ECnUiGlobalOperationSignalChannel );
    }


// -----------------------------------------------------------------------------
// CCnUiSignaller::SignalOperationL()
// -----------------------------------------------------------------------------
//
TInt CCnUiSignaller::SignalOperationL( const TDesC& aDetailDesc )
    {
    if ( OperationRunning() )
        {
        return KErrInUse;
        }

    //construct objects
    iOperationSignalChannel->SignalL( aDetailDesc );
    iOperationSignalRequested = ETrue;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiSignaller::CancelOperationSignal()
// -----------------------------------------------------------------------------
//
void CCnUiSignaller::CancelOperationSignal()
    {
    if ( iOperationSignalRequested )
        {
        iOperationSignalChannel->CancelSignal();
        iOperationSignalRequested = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiSignaller::OperationRunning()
// -----------------------------------------------------------------------------
//
TBool CCnUiSignaller::OperationRunning()
    {
    TInt signalCount = 0;
    iOperationSignalChannel->Read( signalCount );

    return ( signalCount > 0 );
    }


// -----------------------------------------------------------------------------
// CCnUiSignaller::OperationDetailsL()
// -----------------------------------------------------------------------------
//
void CCnUiSignaller::OperationDetailsL( HBufC*& aDetailDesc )
    {
    if ( !OperationRunning() )
        {
        //if the operation isn't running
        //values are defaulted to empty buffer and 0
        aDetailDesc = KNullDesC().AllocL();
        }

    else
        {
        iOperationSignalChannel->ReadL( aDetailDesc );
        }
    }


//  End of File
