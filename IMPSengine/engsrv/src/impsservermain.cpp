/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Console class for server.
*
*
*/



#include "impsserver.h"
#include "impsutils.h"

#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

TInt SetSignal( TImpsSignal& aSignal )
    {
    TBuf<50> buf;

    User::CommandLine( buf );

    return aSignal.Set( buf );
    }

GLDEF_C TInt E32Main()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "E32Main begins" ) );
    TFileName name( RProcess().FileName( ) );
    CImpsClientLogger::Log( _L( "Server: Exe loaded from %S" ), &name );
#endif

    TImpsSignal signal;
    TInt err = SetSignal( signal );
    if ( !err )
        {
        err = CImpsServer::ThreadStart( signal );
        }
    return err;
    }

//  End of File

