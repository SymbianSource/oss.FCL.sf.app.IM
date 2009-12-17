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
* Description:  Used when inviting to chats
*
*/


// INCLUDE FILES
#include "CCAInvitation.h"

// ========================== MEMBER FUNCTIONS =================================

//------------------------------------------------------------------------------
// CCAInvitation::CCAInvitation
// C++ default constructor can NOT contain any code, that might leave.
//------------------------------------------------------------------------------
CCAInvitation::CCAInvitation( TInt aValidityPeriod )
        : iTimeout( aValidityPeriod )
    {
    // store the time this invitation was received
    iCreated.HomeTime();
    }

//------------------------------------------------------------------------------
// CCAInvitation::ConstructL
// Symbian OS default constructor can leave.
//------------------------------------------------------------------------------
void CCAInvitation::ConstructL( const TDesC& aInviteID,
                                const TDesC& aUserID,
                                const TDesC& aGroupId,
                                const TDesC& aScreenName,
                                const TDesC& aGroupName,
                                const TDesC& aMessage )
    {
    iInviteID   = aInviteID.AllocL();
    iUserID     = aUserID.AllocL();
    iGroupID	= aGroupId.AllocL();
    iScreenName = aScreenName.AllocL();
    iGroupName  = aGroupName.AllocL();
    iMessage    = aMessage.AllocL();
    }

//------------------------------------------------------------------------------
// CCAInvitation::NewL
// Two-phased constructor.
//------------------------------------------------------------------------------
CCAInvitation* CCAInvitation::NewL(
    const TDesC& aInviteID,
    const TDesC& aUserID,
    const TDesC& aGroupID,
    const TDesC& aScreenName,
    const TDesC& aGroupName,
    const TDesC& aMessage,
    TInt   aValidityPeriod /*= 0*/ )
    {
    CCAInvitation* self = new ( ELeave ) CCAInvitation( aValidityPeriod );

    CleanupStack::PushL( self );
    self->ConstructL( aInviteID, aUserID, aGroupID, aScreenName, aGroupName,
                      aMessage );
    CleanupStack::Pop( self );

    return self;
    }

//------------------------------------------------------------------------------
// CCAInvitation::~CCAInvitation
// Destructor
//------------------------------------------------------------------------------
CCAInvitation::~CCAInvitation()
    {
    delete iInviteID;
    delete iUserID;
    delete iGroupID;
    delete iScreenName;
    delete iGroupName;
    delete iMessage;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::AddReadObserver()
// -----------------------------------------------------------------------------
//
void CCAInvitation::AddReadObserver( MCAInviteReadObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::InviteID()
// -----------------------------------------------------------------------------
//
const TDesC& CCAInvitation::InviteID() const
    {
    return *iInviteID;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::UserID()
// -----------------------------------------------------------------------------
//
const TDesC& CCAInvitation::UserID() const
    {
    return *iUserID;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::ScreenName()
// -----------------------------------------------------------------------------
//
const TDesC& CCAInvitation::ScreenName() const
    {
    return *iScreenName;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::GroupName()
// -----------------------------------------------------------------------------
//
const TDesC& CCAInvitation::GroupName() const
    {
    return *iGroupName;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::GroupId()
// -----------------------------------------------------------------------------
//
const TDesC& CCAInvitation::GroupId() const
    {
    return *iGroupID;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::Message()
// -----------------------------------------------------------------------------
//
const TDesC& CCAInvitation::Message() const
    {
    return *iMessage;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::Timeout()
// -----------------------------------------------------------------------------
//
TInt CCAInvitation::Timeout() const
    {
    return iTimeout;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::ReceivedAt()
// -----------------------------------------------------------------------------
//
TTime CCAInvitation::ReceivedAt() const
    {
    return iCreated;
    }

// -----------------------------------------------------------------------------
// CCAInvitation::SetInvitationAsReadL()
// -----------------------------------------------------------------------------
//
void CCAInvitation::SetInvitationAsReadL()
    {
    iRead = ETrue;

    if ( iObserver )
        {
        iObserver->HandleInviteReadL();
        }
    }

// -----------------------------------------------------------------------------
// CCAInvitation::IsRead()
// -----------------------------------------------------------------------------
//
TBool CCAInvitation::IsRead() const
    {
    return iRead;
    }


//  End of File
