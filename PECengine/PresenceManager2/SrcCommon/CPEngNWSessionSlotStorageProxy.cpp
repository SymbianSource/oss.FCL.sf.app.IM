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
* Description:  CPEngNWSessionSlotStorageProxy
*
*/

// INCLUDE FILES
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngStorageManager.h"
#include "PEngStorageManager.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngSessionSlotId.h"


#include <E32Std.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlotManager2.h>



// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotStorageProxy::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotStorageProxy* CPEngNWSessionSlotStorageProxy::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngNWSessionSlotStorageProxy* self =
        new ( ELeave ) CPEngNWSessionSlotStorageProxy;
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngNWSessionSlotStorageProxy::CPEngNWSessionSlotStorageProxy()
    {
    }


// Destructor
CPEngNWSessionSlotStorageProxy::~CPEngNWSessionSlotStorageProxy()
    {
    if ( iStorageManager )
        {
        iStorageManager->Close();
        }

    delete iAppId;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotStorageProxy::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotStorageProxy::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    //Verify that NWSessionSlot fully exists in the storage
    //(Storage manager doesn't care about AppId registration, but it can
    //be checked with CPEngNWSessionSlotManager2.)
    CPEngNWSessionSlotManager2* slotMan = CPEngNWSessionSlotManager2::NewLC();
    User::LeaveIfError( slotMan->ExistsNWSessionSlot( aNWSessionSlotID ) );
    CleanupStack::PopAndDestroy( slotMan );

    const CPEngSessionSlotId& ssBaseId = aNWSessionSlotID.Implementation()->BasePart();
    iStorageManager = PEngStorageManager::GetStorageManagerL( ssBaseId );
    iAppId = aNWSessionSlotID.AppId().AllocL();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotStorageProxy::StorageManager()
// -----------------------------------------------------------------------------
//
MPEngStorageManager& CPEngNWSessionSlotStorageProxy::StorageManager()
    {
    return *iStorageManager;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotStorageProxy::BaseId()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngNWSessionSlotStorageProxy::BaseId() const
    {
    return iStorageManager->SessionSlotId();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotStorageProxy::AppId()
// -----------------------------------------------------------------------------
//
const TDesC16& CPEngNWSessionSlotStorageProxy::AppId() const
    {
    return *iAppId;
    }


//  End of File

