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
* Description:  Storage Id container
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngSIdContainer.h"
#include	"CPEngObserverContainer.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSIdContainer::CPEngSIdContainer
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngSIdContainer::CPEngSIdContainer()
        : iObservers( 2 ) // usually 2 observers for same SID
    {
    }

// -----------------------------------------------------------------------------
// CPEngSIdContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngSIdContainer::ConstructL(
    const TDesC& aSId )
    {
    iSId = aSId.AllocL();
    // allocated some slots for observers
    User::LeaveIfError( AllocateSlots( 1 ) );
    }

// -----------------------------------------------------------------------------
// CPEngSIdContainer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSIdContainer* CPEngSIdContainer::NewLC(
    const TDesC& aSId )
    {
    CPEngSIdContainer* self = new( ELeave ) CPEngSIdContainer();

    CleanupStack::PushL( self );
    self->ConstructL( aSId );

    return self;
    }


// Destructor
CPEngSIdContainer::~CPEngSIdContainer()
    {
    // remove SID from Observers
    TInt count ( iObservers.Count() );
    for ( TInt x( 0 ) ; x < count; x++ )
        {
        iObservers[ x ]->RemoveSIdContainer( this );
        }
    iObservers.Reset();
    delete iSId;
    }


// =============================================================================
// Another function of the CPEngSIdContainer
// =============================================================================


// =============================================================================
// New function of the MPEngSIDContainer class
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSIdContainer::SId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngSIdContainer::SId() const
    {
    return *iSId;
    }


// -----------------------------------------------------------------------------
// CPEngSIdContainer::AddObserverContainer()
// -----------------------------------------------------------------------------
//
TInt CPEngSIdContainer::AddObserverContainer(
    const CPEngObserverContainer* aObserver )
    {
    TInt err ( iObservers.InsertInAddressOrder( aObserver ) );
    if  ( ( err == KErrNone ) || ( err == KErrAlreadyExists ) )
        {

        return KErrNone;
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngSIdContainer::RemoveObserverContainer()
// -----------------------------------------------------------------------------
//
void CPEngSIdContainer::RemoveObserverContainer(
    const CPEngObserverContainer* aObserver )
    {
    TInt index ( iObservers.FindInAddressOrder( aObserver ) );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }


// =============================================================================
// New function of the class
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSIdContainer::NotifySIdChangeL()
// -----------------------------------------------------------------------------
//
void CPEngSIdContainer::NotifySIdChangeL()
    {
    TInt count( iObservers.Count() );
    for ( TInt x ( 0 ) ; x < count ; ++x )
        {
        iObservers[ x ]->SIdHasChangedL( *iSId );
        }
    }


// -----------------------------------------------------------------------------
// CPEngSIdContainer::SIdPublished()
// -----------------------------------------------------------------------------
//
TBool CPEngSIdContainer::SIdPublished() const
    {
    return iPublished;
    }


// -----------------------------------------------------------------------------
// CPEngSIdContainer::SetSIdPublished()
// -----------------------------------------------------------------------------
//
void CPEngSIdContainer::SetSIdPublished()
    {
    iPublished = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngSIdContainer::ObserverCount()
// -----------------------------------------------------------------------------
//
TInt CPEngSIdContainer::ObserverCount() const
    {
    return iObservers.Count();
    }


// -----------------------------------------------------------------------------
// CPEngSIdContainer::AllocateSlots()
// -----------------------------------------------------------------------------
//
TInt CPEngSIdContainer::AllocateSlots(
    TInt aCount,
    TInt aDone )
    {
    TInt err ( iObservers.Append( NULL  ) );

    if ( err != KErrNone )
        {
        return err;
        }
    aDone++;
    if ( aCount < aDone )
        {
        err = AllocateSlots( aCount, aDone );
        }
    // remove last element from array = count minus 1
    iObservers.Remove( iObservers.Count() - 1 );
    return err;
    }

//  End of File
