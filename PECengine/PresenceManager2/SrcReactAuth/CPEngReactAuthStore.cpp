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
* Description:  Store API to retrieve reactive authorization models.
*
*/

// INCLUDE FILES
#include "CPEngReactAuthStore.h"
#include "CPEngReactAuthStoreImp.h"
#include "MPEngAuthorizationManager.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngReactAuthStore* CPEngReactAuthStore::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngReactAuthStore* self = CPEngReactAuthStore::NewLC( aNWSessionSlotID,
                                                            aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngReactAuthStore* CPEngReactAuthStore::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngReactAuthStore* self = new ( ELeave ) CPEngReactAuthStore;
    CleanupStack::PushL( self );
    self->iImp = CPEngReactAuthStoreImp::NewL( aNWSessionSlotID,
                                               aPriority );
    return self;
    }


// Destructor
CPEngReactAuthStore::~CPEngReactAuthStore()
    {
    delete iImp;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::CPEngReactAuthStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngReactAuthStore::CPEngReactAuthStore()
    {
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::AuthorizationReqCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngReactAuthStore::AuthorizationReqCount() const
    {
    return iImp->iAuthManager->AuthorizationsCount();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::AuthorizationReq()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAuthorizationRequest& CPEngReactAuthStore::AuthorizationReq( TInt aIndex )
    {
    return iImp->iAuthManager->Authorization( aIndex );

    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::PendingAuthorizationReqCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngReactAuthStore::PendingAuthorizationReqCount()  const
    {
    return iImp->iAuthManager->PendingAuthorizationsCount();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::PendingAuthorizationReq()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAuthorizationRequest& CPEngReactAuthStore::PendingAuthorizationReq( TInt aIndex )
    {
    return iImp->iAuthManager->PendingAuthorization( aIndex );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::RespondedAuthorizationCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngReactAuthStore::RespondedAuthorizationCount() const
    {
    return iImp->iAuthManager->RespondedAuthorizationsCount();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::RespondedAuthorization()
// -----------------------------------------------------------------------------
//
EXPORT_C const MPEngAuthorizationStatus& CPEngReactAuthStore::RespondedAuthorization(
    TInt aIndex ) const
    {
    return iImp->iAuthManager->RespondedAuthorization( aIndex );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::ServerDeniedAuthorizationCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngReactAuthStore::ServerDeniedAuthorizationCount() const
    {
    return iImp->iAuthManager->ServerDeniedAuthorizationsCount();
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::ServerDeniedAuthorization()
// -----------------------------------------------------------------------------
//
EXPORT_C const MPEngAuthorizationStatus& CPEngReactAuthStore::ServerDeniedAuthorization(
    TInt aIndex ) const
    {
    return iImp->iAuthManager->ServerDeniedAuthorization( aIndex );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStore::GetAndLockAuthorizationRespond()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngReactAuthStore::GetAndLockAuthorizationRespond(
    MPEngAuthorizationRespond*& aRespond )
    {
    TRAPD( err,
           iImp->GetAndLockAuthorizationRespondL( aRespond ) );

    return err;
    }


// End of File

