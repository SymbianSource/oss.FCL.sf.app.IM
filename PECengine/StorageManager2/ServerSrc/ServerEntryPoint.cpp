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
* Description:  Dll entry points for server exe.
*
*/


#include <e32base.h>

#include "CPEngStorageServer.h"
#include "PEngStorageServerCommon.h"
#include "TPEngServerParams.h"
#include "PresenceDebugPrint.h"

TInt DoMain( TPEngServerParams& aParams )
    {
    CTrapCleanup* tc = CTrapCleanup::New();
    if ( !tc )
        {
        return KErrNoMemory;
        }
    TRAPD( err, CPEngStorageServer::ExecuteServerL( aParams ) );
    delete tc;

#if _BullseyeCoverage
    cov_write();
#endif

    return err;
    }

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    PENG_DP( D_PENG_LIT( "PresenceStorageServer::E32Main() start" ) );


    TPEngServerParams params;
    params.InitFromCmdLine();
    TInt err = DoMain( params );


    PENG_DP( D_PENG_LIT( "PresenceStorageServer - E32Main() end" ) );
    __UHEAP_MARKEND;
    return err;
    }

