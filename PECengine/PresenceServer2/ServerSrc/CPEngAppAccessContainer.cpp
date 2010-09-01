/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSP session access container
*  				 This class maintain reference counted access to the CSP session
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngAppAccessContainer.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::CPEngAppAccessContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAppAccessContainer::CPEngAppAccessContainer()
        : iReserved( 2 ) // Usually IM and PEC
    {
    }

// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAppAccessContainer::ConstructL(
    const TDesC& aAppId )
    {
    iAppId = aAppId.AllocL();
    }

// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAppAccessContainer* CPEngAppAccessContainer::NewLC(
    const TDesC& aAppId )
    {
    CPEngAppAccessContainer* self = new( ELeave ) CPEngAppAccessContainer( );

    CleanupClosePushL( *self );
    self->ConstructL( aAppId );

    return self;
    }


// Destructor
CPEngAppAccessContainer::~CPEngAppAccessContainer()
    {
    iReserved.Reset();
    delete iAppId;
    }

/////////////////////////////////////////////////////////////////////////////////
// =============== New Functions of the main class =============================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::AppId
// -----------------------------------------------------------------------------
//
const TDesC& CPEngAppAccessContainer::AppId() const
    {
    return *iAppId;
    }


// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::ActiveAccessCount
// -----------------------------------------------------------------------------
//
TInt CPEngAppAccessContainer::ActiveAccessCount()
    {
    return AccessCount() + iReserved.Count();
    }


// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::CloseAccess
// -----------------------------------------------------------------------------
//
TBool CPEngAppAccessContainer::CloseAccess()
    {
    Dec();
    // delete if access is zero
    if ( ( 0 == AccessCount() ) && ( iReserved.Count() == 0 ) )
        {
        delete this;
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::ReserveProcessL
// -----------------------------------------------------------------------------
//
void CPEngAppAccessContainer::ReserveProcessL(
    const TDesC& aProcessId )
    {
    iReserved.InsertIsqL( aProcessId );
    Dec();
    }


// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::ActivateProcessIdL
// -----------------------------------------------------------------------------
//
void CPEngAppAccessContainer::ActivateProcessIdL(
    const TDesC& aProcessId )
    {
    TInt pos( KErrNotFound );
    if ( iReserved.FindIsq( aProcessId, pos ) != KErrNone )
        {
        User::LeaveIfError( KErrNotFound );
        }

    iReserved.Delete( pos );
    Inc();
    }


// -----------------------------------------------------------------------------
// CPEngAppAccessContainer::CloseContainer
// -----------------------------------------------------------------------------
//
void CPEngAppAccessContainer::CloseContainer()
    {
    iReserved.Reset();
    TInt count( AccessCount() );
    // can we delete straight, or we need to run down access count?
    if ( count == 0 )
        {
        delete this;
        return;
        }
    for ( TInt x( 0 ) ; x < count  ; ++x )
        {
        Close();
        }
    }

//  End of File
