/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     WV Services tree data structures for Service Negotiation.
*     TImpsServices and TImpsFeature.
*
*/


// INCLUDE FILES
#include    <featmgr.h>
#include    "impsservices.h"
// NOTE remove this for final build
#include    "impsvariant.h"


// CONSTANTS
const TUint maskAll = 0xFFFFFFFF;


// ================= MEMBER FUNCTIONS =======================


void TImpsFeature::SetFeature( TImpsFeatures aFeature )
    {
    iFeature = aFeature;
    }

EXPORT_C void TImpsFeature::Reset( TBool aAll  )
    {
    if ( aAll )
        {
        iOn = ETrue;
        iFunctions = maskAll;
        iCapabilities = maskAll;
        }
    else
        {
        iOn = EFalse;
        iFunctions = 0;
        iCapabilities = 0;
        }
    }

EXPORT_C void TImpsFeature::Copy( TImpsFeature& aCopy )
    {
    iOn = aCopy.iOn;
    iFunctions = aCopy.iFunctions;
    iFeature = aCopy.iFeature;
    iCapabilities = aCopy.iCapabilities;
    }


EXPORT_C TImpsServices::TImpsServices()
    {
    iFundamental.SetFeature( EFundamentalFeat );
    iPresence.SetFeature( EPresenceFeat );
    iIM.SetFeature( EIMFeat );
    iGroup.SetFeature( EGroupFeat );
    }

EXPORT_C TImpsFeature* TImpsServices::Feature( TImpsFeatures aFeature )
    {
    switch ( aFeature )
        {
        case EFundamentalFeat:
            return &iFundamental;
//            break;
        case EPresenceFeat:
            return &iPresence;
//            break;
        case EIMFeat:
            return &iIM;
//            break;
        case EGroupFeat:
        default:
            return &iGroup;
//            break;
        }
    }


EXPORT_C void TImpsServices::Reset( TBool aAll )
    {
    iFundamental.Reset( aAll );
    iPresence.Reset( aAll );
    iIM.Reset( aAll );
    iGroup.Reset( aAll );
    }

EXPORT_C void TImpsServices::Copy( TImpsServices& aCopy )
    {
    iFundamental.Copy( aCopy.iFundamental );
    iPresence.Copy( aCopy.iPresence );
    iIM.Copy( aCopy.iIM );
    iGroup.Copy( aCopy.iGroup );
    }



EXPORT_C TImpsCspIdentifier::TImpsCspIdentifier()
    {
    }   

EXPORT_C TImpsCspIdentifier::TImpsCspIdentifier( const TDesC& aSap, 
                                            const TDesC& aUserId )
:iSAP(aSap),
iUserId(aUserId)
    {
    }

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  

