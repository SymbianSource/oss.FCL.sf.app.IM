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
* Description:  Reactive authorization store API implementation owner.
*
*/

// INCLUDE FILES
#include "CPEngReactAuthStoreImp.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngSessionSlotId.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "PEngListLibraryFactory.h"
#include "MPEngAuthorizationManager.h"
#include "CPEngAuthorizationRespond.h"


#include <CPEngNWSessionSlotID2.h>



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngReactAuthStoreImp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngReactAuthStoreImp* CPEngReactAuthStoreImp::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngReactAuthStoreImp* self =
        new ( ELeave ) CPEngReactAuthStoreImp( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngReactAuthStoreImp::~CPEngReactAuthStoreImp()
    {
    delete iUsedSlot;
    if ( iAuthManager )
        {
        iAuthManager->Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStoreImp::CPEngReactAuthStoreImp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngReactAuthStoreImp::CPEngReactAuthStoreImp( TInt aPriority )
        : iCActivePriority( aPriority )
    {
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStoreImp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngReactAuthStoreImp::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iAuthManager = PEngListLibraryFactory::AuthorizationManagerL( iUsedSlot->BaseId() );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthStoreImp::GetAndLockAuthorizationRespondL()
// -----------------------------------------------------------------------------
//
void CPEngReactAuthStoreImp::GetAndLockAuthorizationRespondL(
    MPEngAuthorizationRespond*& aRespond )
    {
    CPEngAuthorizationRespond* respond = CPEngAuthorizationRespond::NewLC();
    MPEngAuthorizationManager* authMan = PEngListLibraryFactory::AuthorizationManagerL(
                                             iUsedSlot->BaseId() );
    //respond takes always the authMan ownership
    respond->LockRespondToAuthManagerL( *authMan );

    CleanupStack::Pop( respond );
    aRespond = respond;
    }


// End of File


