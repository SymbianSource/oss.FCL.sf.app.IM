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
* Description:  Storage Id change observer container
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngObserverContainer.h"
#include	"MPEngSIDChangeObserver.h"
#include	"MPEngSIdContainer.h"


// MACROS
#define RETURN_IF_ERROR( aErrorCode )\
    TInt eC(  aErrorCode );\
    if ( eC != KErrNone )\
        {\
        return eC;\
        }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngObserverContainer::CPEngObserverContainer
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngObserverContainer::CPEngObserverContainer(
    MPEngSIDChangeObserver& aChangeObserver,
    TInt aPriority )
        : iObserver( aChangeObserver ),
        iObserverPriority( aPriority ),
        iSuspended( EFalse ),
        iChangedSIds( 1 ), // usually probably one SID at time
        iProcessing( EFalse )

    {
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::ConstructL( )
    {
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngObserverContainer* CPEngObserverContainer::NewLC(
    MPEngSIDChangeObserver& aChangeObserver,
    TInt aPriority )
    {
    CPEngObserverContainer* self = new( ELeave ) CPEngObserverContainer(
        aChangeObserver,
        aPriority );

    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }


// Destructor
CPEngObserverContainer::~CPEngObserverContainer()
    {
    ResetEngagedIdsArray();
    iChangedSIds.Reset();
    if ( iAliveFlag )
        {
        ( *iAliveFlag ) = EFalse;
        }
    }


// =============================================================================
// Another function of the CPEngObserverContainer
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngObserverContainer::SIdHasChangedL()
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::SIdHasChangedL(
    const TDesC& aChangedSId )
    {
    TKeyArrayFix comparison( 0, ECmpNormal );
    // do not update if: Processing is in progress(notification of itself),
    // or suspended
    if ( !iProcessing && !iSuspended  )
        {
        iChangedSIds.InsertIsqL( aChangedSId, comparison );
        }
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::RemoveSIdContainer()
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::RemoveSIdContainer(
    MPEngSIdContainer* aSIdContainer )
    {
    TInt index( 0 );
    if ( KErrNone == iEngagedSIds.FindInAddressOrder( aSIdContainer, index ) )
        {
        iEngagedSIds.Remove( index );
        }
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::NotifySIdsChanges()
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::NotifySIdsChanges(
    TInt aPriority )
    {
    // return if priority does not match
    if ( aPriority != iObserverPriority )
        {
        return;
        }
    if ( !iProcessing && ( iChangedSIds.Count() != 0 ) )
        {
        iProcessing = ETrue;

        TBool alive( ETrue );
        iAliveFlag = &alive;

        TRAPD( e, iObserver.HandleSIDsChangeL( iChangedSIds ) );

        if ( !alive )
            {
            return;
            }
        iAliveFlag = NULL;

        iProcessing = EFalse;
        if ( e != KErrNone )
            {
            iObserver.HandleSIDNotifyError( e );
            }
        iChangedSIds.Reset();
        }
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::NotifySIdListenError()
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::NotifySIdListenError(
    TInt aError )
    {
    iProcessing = ETrue;
    iObserver.HandleSIDNotifyError( aError );
    iChangedSIds.Reset();
    iProcessing = EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::UpdateSIdsContainersL()
// -----------------------------------------------------------------------------
//
TInt CPEngObserverContainer::UpdateSIdsContainers(
    const RPointerArray<MPEngSIdContainer>& aSIdContainers,
    RPointerArray<MPEngSIdContainer>& aOldSIdContainers,
    TBool aKeepOldIds )
    {
    aOldSIdContainers.Reset();
    TInt countOld ( iEngagedSIds.Count() );
    for ( TInt x ( 0 ) ; x < countOld ; ++x )
        {
        RETURN_IF_ERROR( aOldSIdContainers.Append( iEngagedSIds[x] ) );
        }

    TInt err( UpdateSIdsContainers( aSIdContainers, aKeepOldIds ) );
    if ( err == KErrNone )
        {
        return KErrNone;
        }
    // update failed, return array to previous state -
    // no fail since we had that count there before
    // pointers are also in address order

    ResetEngagedIdsArray();
    for ( TInt y ( 0 ) ; y < countOld ; ++y )
        {
        // this cannot fail, since we had there that count before,
        // keep trying for all pointers
        iEngagedSIds.Append( aOldSIdContainers[y] );
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::UpdateSIdsContainers()
// -----------------------------------------------------------------------------
//
TInt CPEngObserverContainer::UpdateSIdsContainers(
    const RPointerArray<MPEngSIdContainer>& aSIdContaners,
    TBool aKeepOldIDs )
    {
    if ( !aKeepOldIDs )
        {
        ResetEngagedIdsArray();
        }
    TInt count ( aSIdContaners.Count() );
    for ( TInt x ( 0 ) ; x < count ; ++x )
        {
        if ( KErrNone == aSIdContaners[x]->AddObserverContainer( this ) )
            {
            TInt err ( iEngagedSIds.InsertInAddressOrder( aSIdContaners[x] ) );
            if ( ( KErrNone != err ) && ( err != KErrAlreadyExists ) )
                {
                aSIdContaners[x]->RemoveObserverContainer( this );
                return err;
                }
            }
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::RemoveSIdsFromSIdsObserver()
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::RemoveSIdsFromSIdsObserver(
    const MDesCArray& aSIdsToRemove )
    {
    // go through IDs are remove them if needed, engaged needs to be check from end
    TInt removeCount( aSIdsToRemove.MdcaCount() );
    for ( TInt i ( 0 ) ; i < removeCount ; ++i )
        {
        TPtrC sId( aSIdsToRemove.MdcaPoint( i ) );
        for ( TInt ii( iEngagedSIds.Count() - 1 ) ; ii >= 0 ; --ii )
            {
            if ( KErrNone
                 ==
                 sId.CompareF( iEngagedSIds[ ii ]->SId() )
               )
                {
                iEngagedSIds[ ii ]->RemoveObserverContainer( this );
                iEngagedSIds.Remove( ii );
                break;
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::RestartObserver()
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::RestartObserver()
    {
    for ( TInt x( iEngagedSIds.Count() - 1 ) ; x >= 0 ; --x )
        {
        iEngagedSIds[ x ]->RemoveObserverContainer( this );
        iEngagedSIds.Remove( x );
        }
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::EngagedSIdsCount()
// -----------------------------------------------------------------------------
//
TInt CPEngObserverContainer::EngagedSIdsCount()
    {
    return iEngagedSIds.Count();
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::Observer()
// -----------------------------------------------------------------------------
//
const MPEngSIDChangeObserver* CPEngObserverContainer::Observer( ) const
    {
    return &iObserver;
    }


// -----------------------------------------------------------------------------
// CPEngObserverContainer::SetSuspendMode()
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::SetSuspendMode(
    TBool aSuspendMode )
    {
    iSuspended = aSuspendMode;
    }

// =============================================================================
// ======== NEW PRIVATE FUNCTIONS OF THE CPEngObserverContainer ================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngObserverContainer::ResetEngagedIdsArray
// Reset Engaged SIds array,
// Also link of the Store Id containers to the observer
// container is removed
// -----------------------------------------------------------------------------
//
void CPEngObserverContainer::ResetEngagedIdsArray()
    {
    TInt count ( iEngagedSIds.Count() );
    for ( TInt x ( 0 ) ; x < count ; ++x )
        {
        iEngagedSIds[ x ]->RemoveObserverContainer( this );
        }
    iEngagedSIds.Reset();
    }


//  End of File
