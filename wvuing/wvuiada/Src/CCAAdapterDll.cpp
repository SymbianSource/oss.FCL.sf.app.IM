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
* Description:  Dll access point.
*
*/



//  INCLUDES
#include "CCAAdapterDll.h"
#include "CCAImpsFactory.h"
#include <e32std.h>

// ================= OTHER EXPORTED FUNCTIONS ==============


/**
* Creates concrete interface factory if not created earlier.
* @return MCAImpsFactory abstract base class for factory.
*/
EXPORT_C MCAImpsFactory* CreateImpsFactoryL()
    {
    MCAImpsFactory* mFactory = NULL;
    mFactory = static_cast< MCAImpsFactory* > ( Dll::Tls() );
    if ( ! mFactory )
        {
        CCAImpsFactory *cFactory = CCAImpsFactory::NewL();
        mFactory = cFactory;
        CleanupStack::PushL( cFactory );
        User::LeaveIfError( Dll::SetTls( static_cast< TAny* > ( mFactory ) ) );
        CleanupStack::Pop( cFactory );
        }
    return mFactory;
    }

/**
* Returns current factory instance or NULL if not created yet
* @return MCAImpsFactory current instance, can be NULL
*/
EXPORT_C MCAImpsFactory* CurrentImpsFactoryInstance()
    {
    MCAImpsFactory *mFactory = NULL;
    mFactory = static_cast< MCAImpsFactory* > ( Dll::Tls() );
    return mFactory;
    }

/**
* Destroys the adapter.
*/
EXPORT_C void ReleaseAdapterL()
    {
    MCAImpsFactory *mFactory = NULL;
    mFactory = static_cast< MCAImpsFactory* > ( Dll::Tls() );
    if ( mFactory )
        {
        CCAImpsFactory *cFactory = NULL;
        cFactory = static_cast< CCAImpsFactory* > ( mFactory );
        if ( cFactory )
            {
            delete cFactory;
            }
        // Ignore error
        Dll::SetTls( NULL );
        }
    }

//  End of File
