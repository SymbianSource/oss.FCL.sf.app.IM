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
* Description:  Wireless Village CSP version selector.
*
*/

// INCLUDE FILES
#include "PEngWVCspVersion.h"
#include "PresenceDebugPrint.h"
#include <centralrepository.h>
#include <wvengineInternalCRKeys.h>
#include <E32Base.h>


const TReal KPEngWVEngineCsp12 = 1.2;
const TReal KPEngWVEngineCsp11 = 1.1;


// ============================ LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// DoSelectVersionL()
// -----------------------------------------------------------------------------
void DoSelectVersionL( TPEngWVCspVersion& aVersion )
    {
    aVersion = EWVCspV11;

    TReal protocolValue;
    CRepository* cenRep = CRepository::NewLC( KCRUIDWVEngineVariation );
    TInt err = cenRep->Get( KWVEngineCspVersion, protocolValue );
    CleanupStack::PopAndDestroy(); //cenRep

    if ( ( err == KErrNone ) && ( protocolValue == KPEngWVEngineCsp12 ) )
        {
        aVersion = EWVCspV12;
        }
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngWVCspVersion::SelectVersionL()
// -----------------------------------------------------------------------------
EXPORT_C TPEngWVCspVersion PEngWVCspVersion::SelectVersionL()
    {
    PENG_DP( D_PENG_LIT( "PEngWVCspVersion::SelectVersionL() - from WV Engine CenRep" ) );

    TPEngWVCspVersion version = EWVCspV11;
    TRAPD( err, DoSelectVersionL( version ) );
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }


    PENG_DP( D_PENG_LIT( "PEngWVCspVersion::SelectVersionL() - Using WV CSP %d" ),
             version );
    return version;
    }


// -----------------------------------------------------------------------------
// PEngWVCspVersion::SetVersionL()
// -----------------------------------------------------------------------------
EXPORT_C void PEngWVCspVersion::SetVersionL( TPEngWVCspVersion aVersion )
    {
    PENG_DP( D_PENG_LIT( "PEngWVCspVersion::SetVersionL() - Setting %d to WV Engine CenRep" ), aVersion );


    TReal protocolValue = KPEngWVEngineCsp11;
    if ( aVersion == EWVCspV12 )
        {
        protocolValue = KPEngWVEngineCsp12;
        }

    CRepository* cenRep = CRepository::NewLC( KCRUIDWVEngineVariation );
    User::LeaveIfError( cenRep->Set( KWVEngineCspVersion, protocolValue ) );
    CleanupStack::PopAndDestroy(); //cenRep


    PENG_DP( D_PENG_LIT( "PEngWVCspVersion::SetVersionL() - done" ) );
    }


//  End of File



