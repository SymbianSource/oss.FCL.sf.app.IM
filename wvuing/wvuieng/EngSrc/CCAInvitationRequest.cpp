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
* Description:  Describes one invitation request
*
*/


// INCLUDE FILES
#include "CCAInvitationRequest.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCAInvitationRequest::CCAInvitationRequest
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CCAInvitationRequest::CCAInvitationRequest( const TInt aTimeout )
        : iTimeout( aTimeout )
    {
    }

// -----------------------------------------------------------------------------
// CCAInvitationRequest::ConstructL
// Symbian OS default constructor can leave.
// -----------------------------------------------------------------------------
void CCAInvitationRequest::ConstructL(
    const CDesCArray& aUserIDs,
    const TDesC&      aGroupID,
    const TDesC&      aMessage )
    {
    iMessage  = aMessage.AllocL();
    iGroupID  = aGroupID.AllocL();
    TInt count( aUserIDs.Count() );
    TInt granularity( count );

    if ( granularity == 0 )
        {
        granularity++;
        }
    iInvitees = new ( ELeave ) CDesCArrayFlat( granularity );

    for ( TInt arrayIndex( 0 ); arrayIndex < count; ++arrayIndex )
        {
        iInvitees->AppendL( aUserIDs[arrayIndex] );
        }

    }

// -----------------------------------------------------------------------------
// CCAInvitationRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CCAInvitationRequest* CCAInvitationRequest::NewL(
    const CDesCArray& aUserIDs,
    const TDesC&      aGroupID,
    const TDesC&      aMessage,
    const TInt        aTimeout /*= 1800*/ )
    {
    CCAInvitationRequest* self =
        new ( ELeave ) CCAInvitationRequest( aTimeout );

    CleanupStack::PushL( self );
    self->ConstructL( aUserIDs, aGroupID, aMessage );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCAInvitationRequest::~CCAInvitationRequest
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C CCAInvitationRequest::~CCAInvitationRequest()
    {
    delete iMessage;
    delete iGroupID;

    if ( iInvitees )
        {
        iInvitees->Reset();
        }

    delete iInvitees;
    }

// -----------------------------------------------------------------------------
// CCAInvitationRequest::Message()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCAInvitationRequest::Message() const
    {
    return *iMessage;
    }

// -----------------------------------------------------------------------------
// CCAInvitationRequest::GroupID()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCAInvitationRequest::GroupID() const
    {
    return *iGroupID;
    }

// -----------------------------------------------------------------------------
// CCAInvitationRequest::Invitees()
// -----------------------------------------------------------------------------
//
EXPORT_C const CDesCArray& CCAInvitationRequest::Invitees() const
    {
    return *iInvitees;
    }

// -----------------------------------------------------------------------------
// CCAInvitationRequest::Timeout()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCAInvitationRequest::Timeout() const
    {
    return iTimeout;
    }


//  End of File
