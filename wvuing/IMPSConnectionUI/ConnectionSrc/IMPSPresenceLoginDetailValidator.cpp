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
* Description:  IMPS Presence login detail validator
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <aputils.h>
#include <commdb.h>
#include "IMPSPresenceLoginDetailValidator.h"
#include "CnUiIMPSIdValidator.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// IMPSPresenceLoginDetailValidator::ValidWVLoginIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool IMPSPresenceLoginDetailValidator::ValidWVLoginIdL( const TDesC& aWvId )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "IMPSPresenceLoginDetailValidator::ValidWVLoginIdL() [%S]" ), &aWvId );
    return CnUiIMPSIdValidator::ValidLoginIdL( aWvId );
    }


// -----------------------------------------------------------------------------
// IMPSPresenceLoginDetailValidator::ValidWVLoginIAPL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool IMPSPresenceLoginDetailValidator::ValidWVLoginIAPL( TUint32 aIAP )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "IMPSPresenceLoginDetailValidator::ValidWVLoginIAPL() [%d]" ), aIAP );

    CCommsDatabase* commsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commsDb );
    CApUtils* apUtils = CApUtils::NewLC( *commsDb );

    TBool isValid = EFalse;
    TBool exits = EFalse;

    //apUtils->IAPExistsL might leave if aIAP doesn't exist.
    //we want return this existence status as TBool, so a TRAP here.
    TRAPD( err, ( exits = apUtils->IAPExistsL( aIAP ) ) )
    if ( ( err == KErrNone ) && exits )
        {
        isValid = ETrue;
        }

    CleanupStack::PopAndDestroy( 2 ); // apUtils, commsDb
    return isValid;
    }


//  End of File




