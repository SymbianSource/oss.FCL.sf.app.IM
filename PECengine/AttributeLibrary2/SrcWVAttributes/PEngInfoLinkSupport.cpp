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
* Description:  Wireless Village InfoLink attribute support selector.
*
*/

// INCLUDE FILES
#include "PEngInfoLinkSupport.h"
#include "PresenceDebugPrint.h"
#include <centralrepository.h>
#include <PresenceEngineInternalCRKeys.h>
#include <E32Base.h>



// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DoInfoLinkSupported()
// -----------------------------------------------------------------------------
//
void DoInfoLinkSupportedL( TBool& aInfoLinkSupported )
    {
    aInfoLinkSupported = ETrue;

    TInt supportValue;
    CRepository* cenRep = CRepository::NewLC( KCRUIDPresenceEngineVariation );
    TInt err = cenRep->Get( KPEngVariationInfoLinkSupported,
                            supportValue );
    CleanupStack::PopAndDestroy(); //cenRep

    if ( err == KErrNone )
        {
        if ( supportValue == EPEngVariationInfoLinkNotSupported )
            {
            aInfoLinkSupported = EFalse;
            }
        }
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngInfoLinkSupport::InfoLinkSupportedL()
// -----------------------------------------------------------------------------
EXPORT_C TBool PEngInfoLinkSupport::InfoLinkSupportedL()
    {
    PENG_DP( D_PENG_LIT( "PEngInfoLinkSupport::InfoLinkSupportedL() - from PE CenRep" ) );


    TBool infoLinkSupported = ETrue;
    TRAPD( err, DoInfoLinkSupportedL( infoLinkSupported ) );
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }

    PENG_DP( D_PENG_LIT( "PEngInfoLinkSupport::InfoLinkSupportedL() - %d" ),
             infoLinkSupported );

    return infoLinkSupported;
    }

// -----------------------------------------------------------------------------
// PEngInfoLinkSupport::SetInfoLinkSupportedL()
// -----------------------------------------------------------------------------
EXPORT_C void PEngInfoLinkSupport::SetInfoLinkSupportedL( TBool aSupport )
    {
    PENG_DP( D_PENG_LIT( "PEngInfoLinkSupport::SetVersionL() - Setting %d to PE CenRep" ), aSupport );


    TInt protocolValue = EPEngVariationInfoLinkNotSupported;
    if ( aSupport )
        {
        protocolValue = EPEngVariationInfoLinkSupported;
        }

    CRepository* cenRep = CRepository::NewLC( KCRUIDPresenceEngineVariation );
    User::LeaveIfError( cenRep->Set( KPEngVariationInfoLinkSupported, protocolValue ) );
    CleanupStack::PopAndDestroy(); //cenRep


    PENG_DP( D_PENG_LIT( "PEngInfoLinkSupport::SetVersionL() - done" ) );
    }


//  End of File



