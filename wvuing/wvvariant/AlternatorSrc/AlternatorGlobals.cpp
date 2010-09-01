/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Singleton implementation for alternator.dll.
*
*/


//  Include Files
#include "CAlternator.h"
#include "CAlternatorImp.h"
#include "AlternatorGlobals.h"
#include <e32svr.h>



// Creates alternator
EXPORT_C CAlternator* CreateAlternatorL(  const TDesC& aBrandRoot )
    {
    if ( ! Dll::Tls() )
        {
        CAlternator* alternator = CAlternatorImp::NewL( aBrandRoot );
        if ( ! alternator )
            {
            return NULL;
            }
        Dll::SetTls( alternator );
        }
    else
        {
        ( ( CAlternatorImp* )Dll::Tls() )->SetBrandRoot( aBrandRoot );
        }

    return ( CAlternator* )Dll::Tls();
    }

// Deletes alternator
void ReleaseAlternatorL()
    {
    if ( Dll::Tls() )
        {
        delete ( CAlternator* )Dll::Tls();
        Dll::SetTls( NULL ); // set null for mark of deletion
        }
    }


