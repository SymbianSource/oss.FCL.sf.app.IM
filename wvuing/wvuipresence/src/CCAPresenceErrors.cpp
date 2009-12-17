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
* Description:  Container for detailed errors
*
*/



// INCLUDE FILES
#include    "CCAPresenceErrors.h"
#include    "CCAPresenceError.h"
#include    "ChatDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAPresenceErrors::CCAPresenceErrors
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAPresenceErrors::CCAPresenceErrors()
    {
    }

// -----------------------------------------------------------------------------
// CCAPresenceErrors::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAPresenceErrors* CCAPresenceErrors::NewL()
    {
    CCAPresenceErrors* self = new( ELeave ) CCAPresenceErrors;
    return self;
    }


// Destructor
CCAPresenceErrors::~CCAPresenceErrors()
    {
    CHAT_DP_TXT( "CCAPresenceErrors::~CCAPresenceErrors" );
    iErrors.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCAPresenceErrors::SetError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPresenceErrors::SetError( TInt aError )
    {
    iMainError = aError;
    }

// -----------------------------------------------------------------------------
// CCAPresenceErrors::Error
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPresenceErrors::Error() const
    {
    return iMainError;
    }

// -----------------------------------------------------------------------------
// CCAPresenceErrors::AddDetailedErrorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPresenceErrors::AddDetailedErrorL( CCAPresenceError* aError )
    {
    CHAT_DP_TXT( "CCAPresenceErrors::AddDetailedErrorL" );
    User::LeaveIfError( iErrors.Append( aError ) );
    }

// -----------------------------------------------------------------------------
// CCAPresenceErrors::RemoveDetailedError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPresenceErrors::RemoveDetailedError( CCAPresenceError* aError )
    {
    CHAT_DP_TXT( "CCAPresenceErrors::RemoveDetailedError" );

    TInt index( iErrors.Find( aError ) );

    if ( index != KErrNotFound )
        {
        delete iErrors[index];
        iErrors.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CCAPresenceErrors::Reset
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPresenceErrors::Reset()
    {
    CHAT_DP_TXT( "CCAPresenceErrors::Reset" );

    iErrors.ResetAndDestroy();
    iMainError = KErrNone;
    }

// -----------------------------------------------------------------------------
// CCAPresenceErrors::DetailedErrors
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const RPointerArray<MCAPresenceError>& CCAPresenceErrors::DetailedErrors() const
    {
    return iErrors;
    }

//  End of File
