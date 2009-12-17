/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  wvuiengine utility methods
*
*/


#include <centralrepository.h>

#include "wvengutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
TUint32 WVEngUtils::CRKeyL( TUid aUid, TUint32 aKey )
    {
    TInt retVal = 0;

    TRAPD( err,
           CRepository* rep = CRepository::NewL( aUid );
           rep->Get( aKey, retVal );
           delete rep;
         );  // TRAPD

    if ( err != KErrNotFound && err != KErrPathNotFound )
        {
        // "Not found" errors are ignored, because it's ok to
        // return zero if key is not found
        User::LeaveIfError( err );
        }

    return retVal;
    }
