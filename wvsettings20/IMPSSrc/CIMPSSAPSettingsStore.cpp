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
* Description:  Interface to SAP settings store.
*
*/

// INCLUDE FILES
#include    <e32std.h>
#include    "cimpssapsettingsstore.h"
#include    "CIMPSSAPCenRep.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsStore* CIMPSSAPSettingsStore::NewL( TInt aPriority )
    {
    CIMPSSAPSettingsStore* self = CIMPSSAPSettingsStore::NewLC( aPriority );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsStore* CIMPSSAPSettingsStore::NewLC( TInt aPriority )
    {
    CIMPSSAPSettingsStore* self = new ( ELeave ) CIMPSSAPSettingsStore();
    CleanupStack::PushL( self );
    self->iImp = CIMPSSAPCenRep::NewL( aPriority );
    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::()
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsStore::~CIMPSSAPSettingsStore()
    {
    delete iImp;
    }

//Observer support

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::AddObserverL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CIMPSSAPSettingsStore::AddObserverL( MIMPSSAPObserver* aObserver,
                                                   TIMPSAccessGroup aGroup )
    {
    iImp->AddObserverL( aObserver, aGroup );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::RemoveObserver()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CIMPSSAPSettingsStore::RemoveObserver( MIMPSSAPObserver* aObserver )
    {
    iImp->RemoveObserver( aObserver );
    }

// SAP Settings related

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::StoreNewSAPL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TUint32 CIMPSSAPSettingsStore::StoreNewSAPL( CIMPSSAPSettings* aSAPSettings,
                                                      TIMPSAccessGroup aGroup )
    {
    return iImp->StoreNewSAPL( aSAPSettings, aGroup );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::GetSAPL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CIMPSSAPSettingsStore::GetSAPL( TUint32 aUid,
                                              CIMPSSAPSettings* aSAPSettings )
    {
    iImp->GetSAPL( aUid, aSAPSettings );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::SAPCountL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CIMPSSAPSettingsStore::SAPCountL( TIMPSAccessGroup aGroup )
    {
    return iImp->SAPCountL( aGroup );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::DeleteSAPL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CIMPSSAPSettingsStore::DeleteSAPL( TUint32 aUid )
    {
    iImp->DeleteSAPL( aUid );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::UpdateOldSAPL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CIMPSSAPSettingsStore::UpdateOldSAPL( CIMPSSAPSettings* aSAPSettings,
                                                    TUint32 aUid )
    {
    iImp->UpdateOldSAPL( aSAPSettings, aUid );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::PopulateSAPSettingsListL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CIMPSSAPSettingsStore::PopulateSAPSettingsListL( CIMPSSAPSettingsList& aList,
                                                               TIMPSAccessGroup aGroup,
                                                               TBool aAscending )
    {
    iImp->PopulateSAPSettingsListL( aList, aGroup, aAscending );
    }

// Default SAP

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::GetDefaultL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CIMPSSAPSettingsStore::GetDefaultL( TUint32& aUid,
                                                  TIMPSAccessGroup aGroup )
    {
    iImp->GetDefaultL( aUid, aGroup );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::GetDefaultL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CIMPSSAPSettingsStore::GetDefaultL( CIMPSSAPSettings* aSAPSettings,
                                                  TIMPSAccessGroup aGroup )
    {
    iImp->GetDefaultL( aSAPSettings, aGroup );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsStore::SetToDefaultL()
// (Other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettingsStore::SetToDefaultL( TUint32 aUid,
                                                    TIMPSAccessGroup aGroup )
    {
    iImp->SetToDefaultL( aUid, aGroup );
    }

//  End of File

