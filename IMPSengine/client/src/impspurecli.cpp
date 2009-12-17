/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*       WV engine pure data API 
*
*
*/


// INCLUDE FILES
#include    "impspurecli.h"
#include    "impspurehandler.h"
#include    "Impspurecommand.h"
#include    "impsServerCommon.h"

#include    "impsutils.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// CONSTANTS

const TImpsContent KSendPureElements[]  = { EImpsKeySession,
                                            EImpsKeyTransaction,
                                            EImpsKeyTransactionContent };


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RImpsPureClient2::RImpsPureClient2
// -----------------------------------------------------------------------------
EXPORT_C RImpsPureClient2::RImpsPureClient2() :
        RImpsClient2(),
        iCommand( NULL ),
        iPureCallback( NULL )
    {
    SetOpIdRange( ); 
    }

// ---------------------------------------------------------
// RImpsPureClient2::RegisterL
// ---------------------------------------------------------
EXPORT_C void RImpsPureClient2::RegisterL(
   RImpsEng& aEngine,
   MImpsPureHandler2* aImpsObserver,
   TImpsServiceType aType,
   TBool aReceiveNew,
   TInt aPriority )
   {

   if ( aType != EImpsServicePresence )
       {
       User::Leave( KErrNotSupported );
       }

   iPureCallback = aImpsObserver;
   iHandler = CImpsPureHandler2::NewL( *this, aPriority );
   iCommand = CImpsPureCommand2::NewL( *this );
   iActiveCommand = iCommand;

   // Start the handler
   iHandler->StartRun();
   iHandleNew = aReceiveNew;
   iAnyContent = EFalse;

   // Send registration to the server thread
   TInt err = DoRegister( aEngine, iHandler );
   User::LeaveIfError( err );

   }

// -----------------------------------------------------------------------------
// RImpsPureClient2::Unregister()
// -----------------------------------------------------------------------------
EXPORT_C void RImpsPureClient2::Unregister()
    {
    DoUnregister();
    }

// -----------------------------------------------------------------------------
// RImpsPureClient2::DoRegister()
// -----------------------------------------------------------------------------
TInt RImpsPureClient2::DoRegister(
    RImpsEng& aEngine,
    CImpsHandler2* aHandler )
    {
    return DoRegister2( aEngine, aHandler, EImpsServPureRegister );
    }

// -----------------------------------------------------------------------------
// RImpsPureClient2::SendPureDataL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RImpsPureClient2::SendPureDataL( 
            const TDesC8& aContent )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KSendPureElements,
                           sizeof( KSendPureElements ) /
                           sizeof( KSendPureElements[0] ) );

    iCommand->InsertElementL( aContent, EImpsKeyPureData );
    // syncronous sending
    TInt ret = iCommand->PackAndSendL( EImpsPureData, iOpId  );
    if( ret != KErrNone )
    	{
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("PureClient2: SendPureDataL opid=%d cli=%d LEAVES with %d"), 
        iOpId, (TInt)this, ret );
#endif	
		User::Leave( ret );
    	}
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("PureClient2: SendPureDataL opid=%d cli=%d"), 
        iOpId, (TInt)this);
#endif
    return iOpId;
    }

// -----------------------------------------------------------------------------
// RImpsPureClient2::Handler
// -----------------------------------------------------------------------------
MImpsPureHandler2* RImpsPureClient2::Handler( )
    {
    return iPureCallback;
    }

//  End of File  

