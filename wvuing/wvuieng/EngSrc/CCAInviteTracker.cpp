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
* Description:  Describes one tracked invitation
*
*/


// INCLUDE FILES
#include "CCAInviteTracker.h"
#include "CCAInvitation.h"
#include "MCAInviteTrackerCallback.h"
#include "ChatDebugPrint.h"
#include "PrivateEngineDefinitions.h"

// ================= MEMBER FUNCTIONS =======================

//-----------------------------------------------------------------------------
// CCAInviteTracker::CCAInviteTracker
// C++ default constructor can NOT contain any code, that might leave.
//-----------------------------------------------------------------------------
CCAInviteTracker::CCAInviteTracker( MCAInviteTrackerCallback* aCallback,
                                    MCAInvitation* aInvitation )
        : CTimer( CActive::EPriorityHigh ),
        iInvitation( aInvitation ),
        iCallback( aCallback )
    {
    }

//-----------------------------------------------------------------------------
// CCAInviteTracker::ConstructL
// Symbian OS default constructor can leave.
//-----------------------------------------------------------------------------
void CCAInviteTracker::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );

    if ( iInvitation )
        {
        iTimeout = iInvitation->Timeout();

        CHAT_DP( D_CHAT_LIT( "Reveived invites timeout-value: %d" ), iTimeout );

        // If no expiry-information is available, then use our default value
        if ( iTimeout == 0 )
            {
            iTimeout = KDefaultInviteTimeout;
            }

        // Because CTimer can't handle long periods, we must divide total
        // expiry-time to sub-periods.
        if ( iTimeout > KInviteTimeoutCycleLength )
            {
            iTimeout = - KInviteTimeoutCycleLength;
            // timeout value changed from seconds to microseconds
            After( KInviteTimeoutCycleLength * 1000000 );
            }
        else
            {
            TInt timeOut( iTimeout );
            iTimeout = 0;
            // timeout value changed from seconds to microseconds
            After( timeOut * 1000000 );
            }
        }
    }

//-----------------------------------------------------------------------------
// CCAInviteTracker::NewL
// Two-phased constructor.
//-----------------------------------------------------------------------------
CCAInviteTracker* CCAInviteTracker::NewL( MCAInviteTrackerCallback* aCallback,
                                          MCAInvitation* aInvitation )
    {
    CCAInviteTracker* self = new ( ELeave ) CCAInviteTracker( aCallback,
                                                              aInvitation );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

//-----------------------------------------------------------------------------
// CCAInviteTracker::~CCAInviteTracker
// Destructor
//-----------------------------------------------------------------------------
CCAInviteTracker::~CCAInviteTracker()
    {
    delete iInvitation;
    }

//-----------------------------------------------------------------------------
// CCAInviteTracker::Invitation
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAInvitation* CCAInviteTracker::Invitation() const
    {
    return iInvitation;
    }

//-----------------------------------------------------------------------------
// CCAInviteTracker::RunL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAInviteTracker::RunL()
    {
    TInt realTimeout( 0 );

    // Sub-period handling
    if ( iTimeout > KInviteTimeoutCycleLength )
        {
        realTimeout = KInviteTimeoutCycleLength;
        iTimeout = - KInviteTimeoutCycleLength;
        CHAT_DP( D_CHAT_LIT( "Invite expiration timer starting new wait: \
		                      %d secs" ), realTimeout );
        // timeout value changed from seconds to microseconds
        After( realTimeout * 1000000 );
        }
    else if ( iTimeout > 0 )
        {
        realTimeout = iTimeout;
        iTimeout = 0;
        CHAT_DP( D_CHAT_LIT( "Invite expiration timer starting new wait: %d \
		                      secs" ), realTimeout );
        // timeout value changed from seconds to microseconds
        After( realTimeout * 1000000 );
        }
    else
        {
        iCallback->InviteExpiredL( iInvitation );
        }
    }


//  End of File
