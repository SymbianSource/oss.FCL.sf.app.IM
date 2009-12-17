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
* Description:  Connection settings notifier.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "CIMPSConnUiConnectionSettingsNotifier.h"
#include "CIMPSConnUiConnectionSettingsNotifierImp.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
EXPORT_C CIMPSConnUiConnectionSettingsNotifier* CIMPSConnUiConnectionSettingsNotifier::NewL(
    TIMPSConnectionClient aClient )
    {
    CIMPSConnUiConnectionSettingsNotifier* self =
        CIMPSConnUiConnectionSettingsNotifier::NewLC( aClient );
    CleanupStack::Pop( self );  //self
    return self;
    }


// Two-phased constructor.
EXPORT_C CIMPSConnUiConnectionSettingsNotifier* CIMPSConnUiConnectionSettingsNotifier::NewLC(
    TIMPSConnectionClient aClient )
    {
    CIMPSConnUiConnectionSettingsNotifier* self =
        new ( ELeave ) CIMPSConnUiConnectionSettingsNotifier;
    CleanupStack::PushL( self );
    self->ConstructL( aClient );
    return self;
    }


// Destructor
EXPORT_C CIMPSConnUiConnectionSettingsNotifier::~CIMPSConnUiConnectionSettingsNotifier()
    {
    IMPSCUI_DP_TXT( "CIMPSConnUiConnectionSettingsNotifier::~CIMPSConnUiConnectionSettingsNotifier()" );
    delete iImp;
    }


// C++ constructor
CIMPSConnUiConnectionSettingsNotifier::CIMPSConnUiConnectionSettingsNotifier()
    {
    IMPSCUI_DP_TXT( "CIMPSConnUiConnectionSettingsNotifier::CIMPSConnUiConnectionSettingsNotifier()" );
    }


// Symbian OS constructor
void CIMPSConnUiConnectionSettingsNotifier::ConstructL( TIMPSConnectionClient aClient )
    {
    iImp = CIMPSConnUiConnectionSettingsNotifierImp::NewL( *this, aClient );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifier::StartL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiConnectionSettingsNotifier::StartL()
    {
    iImp->StartL();
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifier::Stop()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiConnectionSettingsNotifier::Stop()
    {
    iImp->Stop();
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifier::AddObserverL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiConnectionSettingsNotifier::AddObserverL(
    MIMPSConnUiConnectionSettingsObserver* aObserver )
    {
    iImp->AddObserverL( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifier::RemoveObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSConnUiConnectionSettingsNotifier::RemoveObserver(
    MIMPSConnUiConnectionSettingsObserver* aObserver )
    {
    return iImp->RemoveObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifier::ConnectionSettingByCategoryL()
// -----------------------------------------------------------------------------
//
EXPORT_C TIMPSConnectionSettingsEvent
CIMPSConnUiConnectionSettingsNotifier::ConnectionSettingByCategoryL(
    TIMPSConnectionSettingsEvent aSettingsEventCateqory )
    {
    return iImp->ConnectionSettingByCategoryL( aSettingsEventCateqory );
    }


//  End of File




