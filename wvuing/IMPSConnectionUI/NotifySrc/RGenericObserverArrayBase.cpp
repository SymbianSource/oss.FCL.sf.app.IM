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
* Description:  Generic observer array.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include "RGenericObserverArrayBase.h"


// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
RGenericObserverArrayBase::RGenericObserverArrayBase()
        : iNotifyLoopRunning( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// RGenericObserverArrayBase::Close()
// -----------------------------------------------------------------------------
//
void RGenericObserverArrayBase::Close()
    {
    RPointerArrayBase::Close();
    }


// -----------------------------------------------------------------------------
// RGenericObserverArrayBase::AddObserver()
// -----------------------------------------------------------------------------
//
TInt RGenericObserverArrayBase::AddObserver( const TAny* aObserver )
    {
    if ( !aObserver )
        {
        return KErrArgument;
        }

    return RPointerArrayBase::Append( aObserver );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArrayBase::RemoveObserver()
// -----------------------------------------------------------------------------
//
TInt RGenericObserverArrayBase::RemoveObserver( const TAny* aObserver )
    {
    const TInt obsCount = RPointerArrayBase::Count();
    for ( TInt ii( 0 ); ii < obsCount; ii++ )
        {
        if ( At( ii ) == aObserver )
            {
            //remove / blank the first found and return
            if ( iNotifyLoopRunning )
                {
                RPointerArrayBase::At( ii ) = NULL;
                }
            else
                {
                RPointerArrayBase::Remove( ii );
                }

            return KErrNone;
            }
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// RGenericObserverArrayBase::Count()
// -----------------------------------------------------------------------------
//
TInt RGenericObserverArrayBase::Count()
    {
    return RPointerArrayBase::Count();
    }


// -----------------------------------------------------------------------------
// RGenericObserverArrayBase::NotifyObservers()
// -----------------------------------------------------------------------------
//
void RGenericObserverArrayBase::NotifyObservers( MGenObserverNotifyMediatorBase& aNotifyMediator,
                                                 TAny* aNotifyData )
    {
    //Protect the observer handling against the observer
    //remove / adding during the notify run.
    iNotifyLoopRunning = ETrue;

    //Observers might be added during notify loop
    //==> use dynamic count
    TInt ii;
    for ( ii = 0; ii < Count(); ii++ )
        {
        TAny* obs = RPointerArrayBase::At( ii );
        if ( obs )
            {
            //observer exist, notify it
            TRAPD( err, aNotifyMediator.MediateNotifyL( obs, aNotifyData ) );
            if ( ( err != KErrNone ) && RPointerArrayBase::At( ii ) )
                {
                //handle notify caused an error and the observer is still there
                //==>report a error to it
                aNotifyMediator.MediateNotifyError( obs, err );
                }
            }
        }

    FinalizeObsArrayAfterNotifyLoop();
    iNotifyLoopRunning = EFalse;
    }


// -----------------------------------------------------------------------------
// RGenericObserverArrayBase::NotifyObserversFromError()
// -----------------------------------------------------------------------------
//
void RGenericObserverArrayBase::NotifyObserversFromError(
    MGenObserverNotifyMediatorBase& aNotifyMediator,
    TInt aError )

    {
    //Protect the observer handling against the observer
    //remove / adding during the notify run.
    iNotifyLoopRunning = ETrue;

    //Observers might be added during notify loop
    //==> use dynamic count
    TInt ii;
    for ( ii = 0; ii < Count(); ii++ )
        {
        TAny* obs = RPointerArrayBase::At( ii );
        if ( obs )
            {
            //observer exist, notify it
            aNotifyMediator.MediateError( obs, aError );
            }
        }

    FinalizeObsArrayAfterNotifyLoop();
    iNotifyLoopRunning = EFalse;
    }


// -----------------------------------------------------------------------------
// RGenericObserverArrayBase::FinalizeObsArrayAfterNotifyLoop()
// -----------------------------------------------------------------------------
//
void RGenericObserverArrayBase::FinalizeObsArrayAfterNotifyLoop()
    {
    //finalize the array
    for ( TInt ii = ( RPointerArrayBase::Count() - 1 ); ii >= 0; ii-- )
        {
        if ( !RPointerArrayBase::At( ii ) )
            {
            RPointerArrayBase::Remove( ii );
            }
        }
    }


//  End of File

