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
* Description:  Represents one detailed error result
*
*/



// INCLUDE FILES
#include    "CCAPresenceError.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAPresenceError::CCAPresenceError
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAPresenceError::CCAPresenceError( TInt aErrorCode )
        : iErrorCode( aErrorCode )
    {
    }

// -----------------------------------------------------------------------------
// CCAPresenceError::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAPresenceError::ConstructL( const TDesC& aUserId )
    {
    iUserId = aUserId.AllocL();
    }

// -----------------------------------------------------------------------------
// CCAPresenceError::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAPresenceError* CCAPresenceError::NewL( TInt aErrorCode, const TDesC& aUserId )
    {
    CCAPresenceError* self = NewLC( aErrorCode, aUserId );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCAPresenceError::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAPresenceError* CCAPresenceError::NewLC( TInt aErrorCode, const TDesC& aUserId )
    {
    CCAPresenceError* self = new( ELeave ) CCAPresenceError( aErrorCode );

    CleanupStack::PushL( self );
    self->ConstructL( aUserId );

    return self;
    }

// Destructor
CCAPresenceError::~CCAPresenceError()
    {
    delete iUserId;
    }


// -----------------------------------------------------------------------------
// CCAPresenceError::ErrorCode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPresenceError::ErrorCode() const
    {
    return iErrorCode;
    }

// -----------------------------------------------------------------------------
// CCAPresenceError::UserId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCAPresenceError::UserId() const
    {
    return *iUserId;
    }

//  End of File
