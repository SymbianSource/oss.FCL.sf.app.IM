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
* Description:  Subscribed user.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include "CPEngSubscribedUser.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSubscribedUser::CPEngSubscribedUser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSubscribedUser::CPEngSubscribedUser()
    {
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUser::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSubscribedUser* CPEngSubscribedUser::NewLC( const TDesC& aUserID )
    {
    CPEngSubscribedUser* self = new ( ELeave ) CPEngSubscribedUser();
    CleanupStack::PushL( self );
    self->ConstructL( aUserID );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUser::ConstructL()
// Symbian OS constructor.
// -----------------------------------------------------------------------------
//
void CPEngSubscribedUser::ConstructL( const TDesC& aUserID  )
    {
    iUserID = aUserID.AllocL();
    }


// Destructor
CPEngSubscribedUser::~CPEngSubscribedUser()
    {
    delete iUserID;
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUser::UserID()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngSubscribedUser::UserID() const
    {
    return *iUserID;
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUser::AddSubscription()
// -----------------------------------------------------------------------------
//
void CPEngSubscribedUser::AddSubscription()
    {
    iSubsCount++;
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUser::RemoveSubscription()
// -----------------------------------------------------------------------------
//
void CPEngSubscribedUser::RemoveSubscription()
    {
    if ( iSubsCount > 0 )
        {
        iSubsCount--;
        }
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUser::SubscriptionsCount()
// -----------------------------------------------------------------------------
//
TBool CPEngSubscribedUser::SubscriptionsCount() const
    {
    return iSubsCount;
    }


//  End of File
