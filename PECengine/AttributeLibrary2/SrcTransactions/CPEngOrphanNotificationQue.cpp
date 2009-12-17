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
* Description:  Que for orphan presence notifications.
*
*/

// INCLUDE FILES
#include "CPEngOrphanNotificationQue.h"
#include "PresenceDebugPrint.h"

#include <E32Std.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngOrphanNotificationQue::CPEngOrphanNotificationQue
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngOrphanNotificationQue::CPEngOrphanNotificationQue( TInt aQueSize )
        : iMaxQueSize( aQueSize )
    {
    }


// -----------------------------------------------------------------------------
// CPEngOrphanNotificationQue::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngOrphanNotificationQue* CPEngOrphanNotificationQue::NewL( TInt aQueSize )
    {
    __ASSERT_ALWAYS( aQueSize >= 1, User::Leave( KErrArgument ) );
    CPEngOrphanNotificationQue* self = new ( ELeave ) CPEngOrphanNotificationQue( aQueSize );
    return self;
    }


// Destructor
CPEngOrphanNotificationQue::~CPEngOrphanNotificationQue()
    {
    iQue.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CPEngOrphanNotificationQue::AddToQueL()
// -----------------------------------------------------------------------------
//
void CPEngOrphanNotificationQue::AddToQueL( const TDesC8& aPresenceBlock )
    {
    PENG_DP( D_PENG_LIT( "CPEngOrphanNotificationQue::AddToQueL() - MaxQueSize[%d], QueSize[%d]" ),
             iMaxQueSize, iQue.Count() );

    HBufC8* newEntry = aPresenceBlock.AllocLC();

    if ( iQue.Count() == iMaxQueSize )
        {
        PENG_DP( D_PENG_LIT( "CPEngOrphanNotificationQue::AddToQueL() - discard the oldest element" ) );

        //Discard the oldest element (the first element at offset 0)
        delete iQue[ 0 ];
        iQue.Remove( 0 );
        }

    // Add new at the end of array
    iQue.AppendL( newEntry );
    CleanupStack::Pop( newEntry );

    //Reset the iterator
    iIndex = 0;
    }


// -----------------------------------------------------------------------------
// CPEngOrphanNotificationQue::GetFirst()
// -----------------------------------------------------------------------------
//
const TDesC8* CPEngOrphanNotificationQue::GetFirst()
    {
    PENG_DP( D_PENG_LIT( "CPEngOrphanNotificationQue::GetFirst() - MaxQueSize[%d], QueSize[%d]" ),
             iMaxQueSize, iQue.Count() );

    //Reset the iterator
    iIndex = 0;
    return DoGetCurrent();
    }


// -----------------------------------------------------------------------------
// CPEngOrphanNotificationQue::GetNext()
// -----------------------------------------------------------------------------
//
const TDesC8* CPEngOrphanNotificationQue::GetNext()
    {
    PENG_DP( D_PENG_LIT( "CPEngOrphanNotificationQue::GetNext() - MaxQueSize[%d], QueSize[%d], Index[%d]" ),
             iMaxQueSize, iQue.Count(), iIndex );

    //Proceed to next
    iIndex++;
    return DoGetCurrent();
    }


// -----------------------------------------------------------------------------
// CPEngOrphanNotificationQue::DeleteCurrentAndGetNext()
// -----------------------------------------------------------------------------
//
const TDesC8* CPEngOrphanNotificationQue::DeleteCurrentAndGetNext()
    {
    PENG_DP( D_PENG_LIT( "CPEngOrphanNotificationQue::DeleteCurrentAndGetNext() - MaxQueSize[%d], QueSize[%d], Index[%d]" ),
             iMaxQueSize, iQue.Count(), iIndex );

    if ( DoIndexValid( iIndex ) )
        {
        //Delete current -> array sifts 1 to down -> iIndex points automaticly to next
        delete iQue[ iIndex ];
        iQue.Remove( iIndex );
        }
    else
        {
        //Proceed to next
        iIndex++;
        }

    return DoGetCurrent();
    }


// -----------------------------------------------------------------------------
// CPEngOrphanNotificationQue::DoIndexValid()
// -----------------------------------------------------------------------------
//
TBool CPEngOrphanNotificationQue::DoIndexValid( TInt aIndex ) const
    {
    if ( ( aIndex >= 0 ) && ( aIndex < iQue.Count() ) )
        {
        return ETrue;
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngOrphanNotificationQue::DoGetCurrent()
// -----------------------------------------------------------------------------
//
const TDesC8* CPEngOrphanNotificationQue::DoGetCurrent() const
    {
    if ( DoIndexValid( iIndex ) )
        {
        return iQue[ iIndex ];
        }

    return NULL;
    }


//  End of File


