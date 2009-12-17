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
* Description:  Implementation of CPEngPluginStatus
*
*/


//  Include Files
#include <e32base.h>
#include <e32svr.h>
#include <f32file.h>
#include <e32uid.h>

#include "CPEngPluginStatus.h"
#include "PresenceDebugPrint.h"



// CONSTRUCTION
// Static constructor
CPEngPluginStatus* CPEngPluginStatus::NewL()
    {
    CPEngPluginStatus* self = CPEngPluginStatus::NewLC();
    CleanupStack::Pop();

    return self;
    }

// Static constructor, leaves pointer to cleanup-stack
CPEngPluginStatus* CPEngPluginStatus::NewLC()
    {
    CPEngPluginStatus* self = new( ELeave ) CPEngPluginStatus();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
CPEngPluginStatus::~CPEngPluginStatus()
    {
    iResults.Reset();
    }

// Default constructor, protected
CPEngPluginStatus::CPEngPluginStatus()
        : iResults( 1 )
    {
    }

// Second phase construct
void CPEngPluginStatus::ConstructL()
    {
    }

TUint CPEngPluginStatus::Count() const
    {
    return iResults.Count();
    }

TUid CPEngPluginStatus::Uid( TUint aIndex ) const
    {
    return iResults[aIndex].iUid;
    }

TPEngPluginStatusCode CPEngPluginStatus::Status( TUint aIndex ) const
    {
    return iResults[aIndex].iStatus;
    }

TInt CPEngPluginStatus::Find( TUid aPluginUid ) const
    {
    TInt count = Count();
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iResults[i].iUid == aPluginUid )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

void CPEngPluginStatus::Close()
    {
    delete this;
    }

void CPEngPluginStatus::AddResultL( TUid aUid, TPEngPluginStatusCode aStatus )
    {
    TPEngPluginResult result( aUid, aStatus );
    User::LeaveIfError( iResults.Append( result ) );
    }

void CPEngPluginStatus::Zero()
    {
    iResults.Reset();
    }

void CPEngPluginStatus::Remove( TUint aIndex )
    {
    iResults.Remove( aIndex );
    }

void CPEngPluginStatus::AllocateSpaceForResultsL( TUint aCount )
    {
    iResults.Reset();

    TPEngPluginResult dummyResult( KNullUid, EPEngPlgLoaded );

    while ( aCount-- > 0 )
        {
        User::LeaveIfError( iResults.Append( dummyResult ) );

        }
    }

TPtr8 CPEngPluginStatus::Buffer( TUint aCount ) const
    {
    TInt count = aCount;
    if ( count == 0 )
        {
        count = Count();
        }
    return TPtr8( ( TUint8* )&iResults[0], sizeof( TPEngPluginResult ) * count, sizeof( TPEngPluginResult ) * count );
    }

