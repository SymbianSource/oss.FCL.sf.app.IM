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
* Description:  Connection mode notifier.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "cimpsconnuiconnectionmodenotifierng.h"
#include "CIMPSConnUiConnectionModeNotifierImp.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
EXPORT_C CIMPSConnUiConnectionModeNotifier* CIMPSConnUiConnectionModeNotifier::NewL(
    TIMPSConnectionClient aClient )
    {
    CIMPSConnUiConnectionModeNotifier* self = CIMPSConnUiConnectionModeNotifier::NewLC( aClient );
    CleanupStack::Pop( self );  //self
    return self;
    }


// Two-phased constructor.
EXPORT_C CIMPSConnUiConnectionModeNotifier* CIMPSConnUiConnectionModeNotifier::NewLC(
    TIMPSConnectionClient aClient )
    {
    CIMPSConnUiConnectionModeNotifier* self = new ( ELeave ) CIMPSConnUiConnectionModeNotifier;
    CleanupStack::PushL( self );
    self->ConstructL( aClient );
    return self;
    }


// Destructor
EXPORT_C CIMPSConnUiConnectionModeNotifier::~CIMPSConnUiConnectionModeNotifier()
    {
    IMPSCUI_DP_TXT( "CIMPSConnUiConnectionModeNotifier::~CIMPSConnUiConnectionModeNotifier()" );
    delete iImp;
    }


// C++ constructor
CIMPSConnUiConnectionModeNotifier::CIMPSConnUiConnectionModeNotifier()
    {
    IMPSCUI_DP_TXT( "CIMPSConnUiConnectionModeNotifier::CIMPSConnUiConnectionModeNotifier()" );
    }


// Symbian OS constructor
void CIMPSConnUiConnectionModeNotifier::ConstructL( TIMPSConnectionClient aClient )
    {
    iImp = CIMPSConnUiConnectionModeNotifierImp::NewL( *this, aClient );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifier::StartL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiConnectionModeNotifier::StartL()
    {
    iImp->StartL();
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifier::Stop()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiConnectionModeNotifier::Stop()
    {
    iImp->Stop();
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifier::AddObserverL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiConnectionModeNotifier::AddObserverL(
    MIMPSConnUiConnectionModeObserver* aObserver )
    {
    iImp->AddObserverL( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifier::RemoveObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSConnUiConnectionModeNotifier::RemoveObserver(
    MIMPSConnUiConnectionModeObserver* aObserver )
    {
    return iImp->RemoveObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifier::ConnectionModeL()
// -----------------------------------------------------------------------------
//
EXPORT_C TIMPSConnectionModeEvent CIMPSConnUiConnectionModeNotifier::ConnectionModeL()
    {
    return iImp->ConnectionModeByCategoryL( EIMPSCMEUserLevelSelectionEventGroup );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifier::ConnectionModeByCategoryL()
// -----------------------------------------------------------------------------
//
EXPORT_C TIMPSConnectionModeEvent CIMPSConnUiConnectionModeNotifier::ConnectionModeByCategoryL(
    TIMPSConnectionModeEvent aModeEventCateqory )
    {
    return iImp->ConnectionModeByCategoryL( aModeEventCateqory );
    }



//  End of File




