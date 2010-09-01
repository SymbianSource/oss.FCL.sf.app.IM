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
* Description: 
* watcher class to im client.
*
*/


#include    <e32base.h>
#include    "impswatchcli.h"
#include    "impsservercommon.h"
#include    "impsclientsrv.h"

// For two pending request + cancel
const TUint KImpsDefMsgSlots = 3;


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

EXPORT_C RImpsWatchClient::RImpsWatchClient() : RImpsEng( )
    {
    }


// ---------------------------------------------------------
// RImpsWatchClient::RegisterL
// ---------------------------------------------------------
//
EXPORT_C TInt RImpsWatchClient::Register( )
   {
   return CreateSession( KImpsServerName, Version(), KImpsDefMsgSlots );
   }

// -----------------------------------------------------------------------------
// RImpsWatchClient::SendCIR( )
// -----------------------------------------------------------------------------
EXPORT_C void RImpsWatchClient::SendCIR( const TDesC8& aCookie )
   {

   // The server reads from the index 1. This is because the version 1.2
   // is also used this way
   SendReceive( EImpsServPush, TIpcArgs(TIpcArgs::ENothing, &aCookie) );
   }

// -----------------------------------------------------------------------------
// RImpsWatchClient::UnRegister( )
// -----------------------------------------------------------------------------
EXPORT_C void RImpsWatchClient::UnRegister( )
   {
   RSessionBase::Close( );
   }


// -----------------------------------------------------------------------------
// RImpsWatchClient::DoRegister()
//
// -----------------------------------------------------------------------------
TInt RImpsWatchClient::DoRegister( CImpsHandler2* /*aHandler*/ )
    {
    return KErrNone;
    }

//  End of File 
