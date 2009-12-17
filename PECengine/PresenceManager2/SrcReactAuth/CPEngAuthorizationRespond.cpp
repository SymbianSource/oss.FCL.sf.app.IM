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
* Description:  Reactive authorization responds implementation.
*
*/

// INCLUDE FILES
#include    "CPEngAuthorizationRespond.h"
#include    "MPEngAuthorizationManager.h"
#include    <e32std.h>



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAuthorizationRespond::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAuthorizationRespond* CPEngAuthorizationRespond::NewLC()
    {
    CPEngAuthorizationRespond* self = new( ELeave )
    CPEngAuthorizationRespond;

    CleanupStack::PushL( self );
    return self;
    }


// Destructor
CPEngAuthorizationRespond::~CPEngAuthorizationRespond()
    {
    if ( iAuthManager )
        {
        iAuthManager->RemoveAllAuthorizationResponses();
        iAuthManager->Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRespond::CPEngAuthorizationRespond
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAuthorizationRespond::CPEngAuthorizationRespond()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRespond::Extension()
// -----------------------------------------------------------------------------
//
MPEngAuthorizationRespondExt* CPEngAuthorizationRespond::Extension()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationRespond::Extension()
// -----------------------------------------------------------------------------
//
const MPEngAuthorizationRespondExt* CPEngAuthorizationRespond::Extension() const
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRespond::Advanced()
// -----------------------------------------------------------------------------
//
const MPEngAuthorizationRespondAdvanced* CPEngAuthorizationRespond::Advanced() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationRespond::Advanced()
// -----------------------------------------------------------------------------
//
MPEngAuthorizationRespondAdvanced* CPEngAuthorizationRespond::Advanced()
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRespond::Close()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRespond::Close()
    {
    delete this;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationRespond::LockRespondToAuthManagerL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationRespond::LockRespondToAuthManagerL(
    MPEngAuthorizationManager& aAuthMan )
    {
    iAuthManager = &aAuthMan;
    }



//  End of File
