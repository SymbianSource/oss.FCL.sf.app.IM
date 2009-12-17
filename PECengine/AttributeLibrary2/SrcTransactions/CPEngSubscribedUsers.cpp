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
* Description:  List of subscribed users.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include "CPEngSubscribedUsers.h"
#include "CPEngSubscribedUser.h"


// LOCAL CONSTANTS AND MACROS
#define SUBSCRIBED_USERS_ORDER TLinearOrder< CPEngSubscribedUser > ( CompareUsers )

// ============================ LOCAL FUNCTIONS ===============================

TInt CompareUsers( const CPEngSubscribedUser& aLhs, const CPEngSubscribedUser& aRhs )
    {
    return aLhs.UserID().Compare( aRhs.UserID() );
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::CPEngSubscribedUsers
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSubscribedUsers::CPEngSubscribedUsers()
    {
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSubscribedUsers* CPEngSubscribedUsers::NewL()
    {
    CPEngSubscribedUsers* self = new ( ELeave ) CPEngSubscribedUsers();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::ConstructL()
// Symbian OS constructor.
// -----------------------------------------------------------------------------
//
void CPEngSubscribedUsers::ConstructL()
    {
    }


// Destructor
CPEngSubscribedUsers::~CPEngSubscribedUsers()
    {
    iUsers.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::MdcaCount()
// -----------------------------------------------------------------------------
//
TInt CPEngSubscribedUsers::MdcaCount() const
    {
    return iUsers.Count();
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::MdcaPoint()
// -----------------------------------------------------------------------------
//
TPtrC16 CPEngSubscribedUsers::MdcaPoint( TInt aIndex ) const
    {
    return iUsers[ aIndex ]->UserID();
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::AddSubscribedUserL()
// -----------------------------------------------------------------------------
//
void CPEngSubscribedUsers::AddSubscribedUserL( CPEngSubscribedUser* aUser )
    {
    __ASSERT_ALWAYS( aUser, User::Leave( KErrArgument ) );

    User::LeaveIfError( iUsers.InsertInOrder( aUser, SUBSCRIBED_USERS_ORDER ) );
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::FindSubscribedUser()
// -----------------------------------------------------------------------------
//
TInt CPEngSubscribedUsers::FindSubscribedUser( const TDesC& aUserID ) const
    {
    const TInt count = iUsers.Count();
    for ( TInt ii = 0; ii < count; ii++ )
        {
        if ( iUsers[ ii ]->UserID().Compare( aUserID ) == 0 )
            {
            return ii;
            }
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::SubscribedUserCount()
// -----------------------------------------------------------------------------
//
TInt CPEngSubscribedUsers::SubscribedUserCount() const
    {
    return iUsers.Count();
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::SubscribedUser()
// -----------------------------------------------------------------------------
//
CPEngSubscribedUser& CPEngSubscribedUsers::SubscribedUser( TInt aIndex )
    {
    return *iUsers[ aIndex ];
    }


// -----------------------------------------------------------------------------
// CPEngSubscribedUsers::DeleteSubscribeUser()
// -----------------------------------------------------------------------------
//
void CPEngSubscribedUsers::DeleteSubscribeUser( TInt aIndex )
    {
    delete iUsers[ aIndex ];
    iUsers.Remove( aIndex );
    }



//  End of File
