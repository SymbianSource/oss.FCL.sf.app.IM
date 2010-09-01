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
* Description:  Event notifier
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "cimpsconnuipreseventnotifierng.h"
#include "CIMPSConnUiPresEventNotifierImp.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
EXPORT_C CIMPSConnUiPresEventNotifier* CIMPSConnUiPresEventNotifier::NewL()
    {
    CIMPSConnUiPresEventNotifier* self = CIMPSConnUiPresEventNotifier::NewLC();
    CleanupStack::Pop( self );  //self
    return self;
    }


// Two-phased constructor.
EXPORT_C CIMPSConnUiPresEventNotifier* CIMPSConnUiPresEventNotifier::NewLC()
    {
    CIMPSConnUiPresEventNotifier* self = new ( ELeave ) CIMPSConnUiPresEventNotifier;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
EXPORT_C CIMPSConnUiPresEventNotifier::~CIMPSConnUiPresEventNotifier()
    {
    IMPSCUI_DP_TXT( "CIMPSConnUiPresEventNotifier::~CIMPSConnUiPresEventNotifier()" );
    delete iImp;
    }


// C++ constructor
CIMPSConnUiPresEventNotifier::CIMPSConnUiPresEventNotifier()
    {
    IMPSCUI_DP_TXT( "CIMPSConnUiPresEventNotifier::CIMPSConnUiPresEventNotifier()" );
    }


// Symbian OS constructor
void CIMPSConnUiPresEventNotifier::ConstructL( CIMPSConnUiPresEventNotifierImpBase* aImp )
    {
    iImp = aImp;
    }

// Symbian OS constructor
void CIMPSConnUiPresEventNotifier::ConstructL()
    {
    iImp = CIMPSConnUiPresEventNotifierImp::NewL( *this );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifier::StartL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiPresEventNotifier::StartL()
    {
    iImp->StartL();
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifier::Stop()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiPresEventNotifier::Stop()
    {
    iImp->Stop();
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifier::AddObserverL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnUiPresEventNotifier::AddObserverL(
    MIMPSConnUiPresenceEventObserver* aObserver )
    {
    iImp->AddObserverL( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifier::RemoveObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSConnUiPresEventNotifier::RemoveObserver(
    MIMPSConnUiPresenceEventObserver* aObserver )
    {
    return iImp->RemoveObserver( aObserver );
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifier::AppServiceStatusL()
// -----------------------------------------------------------------------------
//
EXPORT_C const CPEngNWSessionSlotID2* CIMPSConnUiPresEventNotifier::AppServiceStatusL(
    TIMPSConnectionClient aClient )
    {
    const CPEngNWSessionSlotID2* sessionSlotID = iImp->AppServiceStatusL( aClient );
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSConnUiPresEventNotifier::AppServiceStatusL() []" ) );
    return sessionSlotID;
    }

//  End of File




