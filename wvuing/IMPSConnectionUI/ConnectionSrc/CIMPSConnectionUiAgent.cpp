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
* Description:  IMPS Connection UI Agent.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "cimpsconnectionuingagent.h"
#include "CIMPSConnectionUiAgentImp.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
EXPORT_C CIMPSConnectionUiAgent* CIMPSConnectionUiAgent::NewL( TIMPSConnectionClient aClient )
    {
    CIMPSConnectionUiAgent* self = new ( ELeave  ) CIMPSConnectionUiAgent() ;
    CleanupStack::PushL( self );
    self->ConstructL( aClient );
    CleanupStack::Pop( self );  //self
    return self;
    }


// Destructor
EXPORT_C CIMPSConnectionUiAgent::~CIMPSConnectionUiAgent()
    {
    IMPSCUI_DP_TXT( "CIMPSConnectionUiAgent::~CIMPSConnectionUiAgent()" );
    delete iImp;
    }


// C++ constructor
CIMPSConnectionUiAgent::CIMPSConnectionUiAgent()
    {
    IMPSCUI_DP_TXT( "CIMPSConnectionUiAgent::CIMPSConnectionUiAgent()" );
    }


// Symbian OS constructor
void CIMPSConnectionUiAgent::ConstructL( TIMPSConnectionClient aClient )
    {
    iImp = CIMPSConnectionUiAgentImp::NewL( aClient );
    }


// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgent::StartServiceL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSConnectionUiAgent::StartServiceL( TIMPSConnectionUiAgentService aService )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSConnectionUiAgent::StartServiceL() [%d]" ), aService );
    return iImp->StartServiceL( aService );
    }


// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgent::StopService()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSConnectionUiAgent::StopService( TIMPSConnectionUiAgentService aService )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSConnectionUiAgent::StopService() [%d]" ), aService );
    return iImp->StopService( aService );
    }


// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgent::StopAllServices()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnectionUiAgent::StopAllServices()
    {
    IMPSCUI_DP_TXT( "CIMPSConnectionUiAgent::StopAllServices()" );
    iImp->StopAllServices();
    }


// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgent::UseResourceFileL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSConnectionUiAgent::UseResourceFileL( const TDesC& aNewResourceFile )
    {
    IMPSCUI_DP_TXT( "CIMPSConnectionUiAgent::UseResourceFileL()" );
    iImp->UseResourceFileL( aNewResourceFile );
    }

//  End of File




