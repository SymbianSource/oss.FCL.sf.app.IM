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
* Description:  SAP Settings Store proxy.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include <CIMPSSAPSettingsStore.h>
#include "CCnUiSapStoreProxy.h"


// ================= LOCAL FUNCTIONS =======================
// -----------------------------------------------------------------------------
// ReleaseSapStoreIfOwned()
// -----------------------------------------------------------------------------
//
void ReleaseSapStoreIfOwned( TAny* aSapStoreProxy )
    {
    CCnUiSapStoreProxy* p = static_cast< CCnUiSapStoreProxy* >( aSapStoreProxy );
    p->ReleaseOwnedSapStore();
    }



// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiSapStoreProxy::CCnUiSapStoreProxy( CIMPSSAPSettingsStore* aSapStore )
        : iSapStore( aSapStore )
    {
    }


// Destructor
CCnUiSapStoreProxy::~CCnUiSapStoreProxy()
    {
    ReleaseOwnedSapStore();
    }


// -----------------------------------------------------------------------------
// CCnUiSapStoreProxy::SapStoreL()
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsStore& CCnUiSapStoreProxy::SapStoreL()
    {
    //lazy initialize the SAP store if yet not having one.
    if ( !iSapStore )
        {
        iSapStore = CIMPSSAPSettingsStore::NewL();
        iOwnsSapStore = ETrue;
        }
    return *iSapStore;
    }


// -----------------------------------------------------------------------------
// CCnUiSapStoreProxy::PushOwnedSapStoreReleaseLC()
// -----------------------------------------------------------------------------
//
void CCnUiSapStoreProxy::PushOwnedSapStoreReleaseLC()
    {
    CleanupStack::PushL( TCleanupItem( ReleaseSapStoreIfOwned, this ) );
    }


// -----------------------------------------------------------------------------
// CCnUiSapStoreProxy::ReleaseOwnedSapStore()
// -----------------------------------------------------------------------------
//
void CCnUiSapStoreProxy::ReleaseOwnedSapStore()
    {
    //if owning SAP store, destroy it and clear holding flags.
    if ( iOwnsSapStore )
        {
        delete iSapStore;
        iSapStore = NULL;
        iOwnsSapStore = EFalse;
        }
    }

//  End of File
