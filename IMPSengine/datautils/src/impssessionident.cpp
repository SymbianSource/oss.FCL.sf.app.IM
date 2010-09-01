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
* Description:  Test utilities for WV engine
*
*
*/


// INCLUDE FILES

#include    "impssessionident.h"

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// TImpsSessIdent::TImpsSessionIdent
// ---------------------------------------------------------
//
EXPORT_C TImpsSessionIdent::TImpsSessionIdent(
    const TDesC& aSID, const TDesC& aSAP, const TDesC& aUserId )
        : iSID( aSID ), iSAP( aSAP ), iUserId( aUserId )
    {}




//  End of File
