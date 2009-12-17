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
* Description:  Server startup parameters.
*
*/

#include "TPEngServerParams.h"
#include "PresenceDebugPrint.h"
#include <E32std.h>



// -----------------------------------------------------------------------------
// TPEngServerParams::TPEngServerParams()
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngServerParams::TPEngServerParams()
        : iServerName( KNullDesC ),
        iParam1( KErrNone ),
        iParam2( KErrNone )
    {
    }


// -----------------------------------------------------------------------------
// TPEngServerParams::TPEngServerParams()
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngServerParams::TPEngServerParams( const TDesC& aServerName,
                                               TInt aParam1,
                                               TInt aParam2 )
        : iServerName( aServerName ),
        iParam1( aParam1 ),
        iParam2( aParam2 )
    {
    }


// -----------------------------------------------------------------------------
// TPEngServerParams::AsCommandLine()
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC TPEngServerParams::AsCommandLine() const
    {
    return TPtrC( ( TText16* )this, sizeof( TPEngServerParams ) / 2 );
    }


// -----------------------------------------------------------------------------
// TPEngServerParams::InitFromCmdLine()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TPEngServerParams::InitFromCmdLine()
    {
    TPtr selfBuffer( ( TText16* )this, sizeof( TPEngServerParams ) / 2 );


    TInt commandLineLength = 0;
    commandLineLength = User::CommandLineLength();

    if ( commandLineLength == selfBuffer.MaxLength() )
        {
        User::CommandLine( selfBuffer );


        PENG_DP( D_PENG_LIT( "TPEngServerParams::InitFromCmdLine() N[%S] P1[%d] P2[%d]" ),
                 &iServerName, iParam1, iParam2 );
        return KErrNone;
        }

    PENG_DP( D_PENG_LIT( "TPEngServerParams::InitFromCmdLine() - No ServerParams present in commandline" ) );
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// TPEngServerParams::RenameMainThread()
// -----------------------------------------------------------------------------
//
EXPORT_C void TPEngServerParams::RenameMainThread( const TDesC& aThreadName )
    {
    PENG_DP( D_PENG_LIT( "TPEngServerParams::RenameMainThread()" ) );

    //Do not care if renaming fails. Renaming is done only to find guilty
    //when something is crashing.
    User::RenameThread( aThreadName );

#ifdef PENG_ENABLE_DEBUG_PRINT
    TBuf< 256 > buffer;
    buffer.Append( _L( "Process[" ) );          // CSI: 78 #
    buffer.Append( RProcess().Name() );
    buffer.Append( _L( "] Thread[" ) );         // CSI: 78 #
    buffer.Append( RThread().Name() );
    buffer.Append( _L( "]" ) );                 // CSI: 78 #
    PENG_DP( buffer );
#endif //PENG_ENABLE_DEBUG_PRINT
    }



// -----------------------------------------------------------------------------
// TPEngServerParams::Signal()
// -----------------------------------------------------------------------------
//
EXPORT_C void TPEngServerParams::Signal()
    {
    RProcess::Rendezvous( KErrNone );
    }



// -----------------------------------------------------------------------------
// TPEngServerParams::Param1()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TPEngServerParams::Param1() const
    {
    return iParam1;
    }


// -----------------------------------------------------------------------------
// TPEngServerParams::Param2()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TPEngServerParams::Param2() const
    {
    return iParam2;
    }


// -----------------------------------------------------------------------------
// TPEngServerParams::ServerName()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& TPEngServerParams::ServerName() const
    {
    return iServerName;
    }



//End of file

