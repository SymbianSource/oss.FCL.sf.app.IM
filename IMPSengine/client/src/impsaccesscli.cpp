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
* class for imps access client.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "impsservercommon.h"
#include    "impsaccesshandler.h"
#include    "impsaccesscommand.h"
#include    "impsaccesscli.h"
#include    "impserrors.h"
#include    "impsclient.h"
#include    "impsutils.h"



// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// RImpsAccessClient2::RImpsAccessClient2
// ----------------------------------------------------------------------------
//
EXPORT_C RImpsAccessClient2::RImpsAccessClient2() :
        RImpsClient2(),
        iCommand( NULL ),
        iHandlerCallBack( NULL ),
        iLoginOpId( 0 ),
        iLoginCancelled( EFalse )
    {
    SetOpIdRange( ); 
    }
  
// ----------------------------------------------------------------------------
// RImpsAccessClient2::RegisterL
// ----------------------------------------------------------------------------
//
EXPORT_C void RImpsAccessClient2::RegisterL(
    RImpsEng& aEngine,
    MImpsAccessHandler2* aObserver,
    TInt aPriority )
    {
    iHandlerCallBack = aObserver;
    iHandler = CImpsAccessHandler2::NewL( *this, aPriority );
    iCommand = CImpsAccessCommand2::NewL( *this );
    iActiveCommand = iCommand;

    // Start the handler
    iHandler->StartRun();
    iHandleNew = ETrue;
    iAnyContent = EFalse;

    TInt err = DoRegister( aEngine, iHandler );
    User::LeaveIfError( err );
    }

// ----------------------------------------------------------------------------
// RImpsAccessClient2::Unregister()
// ----------------------------------------------------------------------------
EXPORT_C void RImpsAccessClient2::Unregister()
    {
    DoUnregister();
    }

// ----------------------------------------------------------------------------
// RImpsAccessClient2::LoginL()
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsAccessClient2::LoginL( TImpsCspIdentifier aCspId,
                              const TDesC& aPsw,
                              const TDesC& aClientId,
                              TUint32 aAP,
                              const TDesC* aKey1,
                              const TDesC* aKey2  )
    {
    if ( !iRunning ) 
        {
        User::Leave( KImpsErrorNotRegistered);
        }

    // Check length of parameters
    if ( ( aCspId.Sap().Length() > KImpsMaxSAP ) ||
         ( aPsw.Length() > KImpsMaxPwd ) ||
         ( aClientId.Length() > KImpsMaxClientId ) )
        {
        User::Leave( KErrArgument);
        }

    iEngine->SetCspIdentifier( aCspId );
    
    IncreaseOpId( );

    iCommand->SetParametersL( aCspId.Sap(), aCspId.UserId(), aPsw, aClientId,
        aAP,  aKey1, aKey2 );   
    iCommand->StartRunL( iOpId, EImpsServWVLogin);
   
    SendReceive( EImpsServWVLogin, 
        TIpcArgs( &iCommand->iMessagePtr, iOpId), iCommand->iStatus );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsAccessClient2::LoginL opId=%d"), iOpId);
#endif 
	// store the operation id
	// and set variable to false
    iLoginOpId = iOpId;
    iLoginCancelled = EFalse; 
        
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsAccessClient2::ReactiveLoginL()
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsAccessClient2::ReactiveLoginL( TImpsCspIdentifier aCspId,
                              const TDesC& aPsw,
                              const TDesC& aClientId,
                              TUint32 aAP,
                              const TDesC* aKey1,
                              const TDesC* aKey2  )
    {
    if ( !iRunning ) 
        {
        User::Leave( KImpsErrorNotRegistered);
        }

    // Check length of parameters
    if ( ( aCspId.Sap().Length() > KImpsMaxSAP ) ||
         ( aPsw.Length() > KImpsMaxPwd ) ||
         ( aClientId.Length() > KImpsMaxClientId ) )
        {
        User::Leave( KErrArgument);
        }

    iEngine->SetCspIdentifier( aCspId );
    
    IncreaseOpId( );

    iCommand->SetParametersL( aCspId.Sap(), aCspId.UserId(), aPsw, aClientId,
        aAP,  aKey1, aKey2 );   
    iCommand->StartRunL( iOpId, EImpsServReactiveLogin);
   
    SendReceive( EImpsServReactiveLogin, 
        TIpcArgs( &iCommand->iMessagePtr, iOpId), iCommand->iStatus );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsAccessClient2::ReactiveLoginL opId=%d"), iOpId);
#endif 
	// store the operation id
	// and set variable to false
    iLoginOpId = iOpId;
    iLoginCancelled = EFalse; 
        
    return iOpId;
    }    

// ----------------------------------------------------------------------------
// RImpsAccessClient2::LogoutL()
// ----------------------------------------------------------------------------

EXPORT_C TInt RImpsAccessClient2::LogoutL( )
    {

    if ( !iRunning ) 
        {
        User::Leave( KImpsErrorNotRegistered );
        }

    IncreaseOpId( );

    // Command does not need to know which one, force or not.
    iCommand->StartRunL( iOpId, EImpsServWVLogout );

    TImpsServRequest req = ( EImpsServWVLogout );
    // p[ 0 ] = iOpId
    SendReceive( req, TIpcArgs(iOpId), iCommand->iStatus );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsAccessClient2::LogoutL opId=%d"), iOpId);
#endif 
    return iOpId;

    }

// ----------------------------------------------------------------------------
// RImpsAccessClient2::NumberOfSessionsL()
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsAccessClient2::NumberOfSessionsL( TImpsCspIdentifier& aCspId )
    {
    TInt myLen = aCspId.UserId().Size() + aCspId.Sap().Size();

    // some extra space for elements needed
    HBufC8* msgStream = HBufC8::NewL( myLen + 50 );
    CleanupStack::PushL( msgStream );   // << msgStream
    *msgStream = KNullDesC8;

    CDesCArrayFlat* myTempArr = new ( ELeave )CDesCArrayFlat( 2 );
    CleanupStack::PushL( myTempArr );   // << myTempArr

    TImpsPackedEntity packedMessage( msgStream );

    myTempArr->AppendL( aCspId.Sap() );
    myTempArr->AppendL( aCspId.UserId() );

    const TUint8* pS = msgStream->Ptr();

    packedMessage.DoPackArray( pS, myTempArr );
    // update the length of the buffer
    msgStream->Des().SetLength( pS-msgStream->Ptr() );

    TInt nbrOfSessions = SendReceive( EImpsServNbrSessions, TIpcArgs( msgStream) );
    
    myTempArr->Reset();  
    CleanupStack::PopAndDestroy( 2 );  // >> msgStream, myTempArr
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsAccessClient2::NumberOfSessionsL opId=%d"), iOpId);
#endif  
    return nbrOfSessions;
    }

// ----------------------------------------------------------------------------
// RImpsAccessClient2::CancelLoginL()
// ----------------------------------------------------------------------------
EXPORT_C void RImpsAccessClient2::CancelLoginL( TInt aCancelOpId )
    {
    if ( iLoginOpId != aCancelOpId )
    {
    	User::Leave( KErrArgument );
    }
    
    TInt ret = SendReceive( EImpsServCancelLogin, TIpcArgs( aCancelOpId ) );
        
	// set variable to true
    iLoginCancelled = ETrue; 
    
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsAccessClient2::CancelLoginL opId=%d ret=%d"), aCancelOpId, ret );
#endif
    }

// ----------------------------------------------------------------------------
// RImpsAccessClient2::CloseCspSessionsL()
// ----------------------------------------------------------------------------
EXPORT_C void RImpsAccessClient2::CloseCspSessionsL( )
    {
    TImpsServRequest req = ( EImpsServCloseAll );
    SendReceive( req, TIpcArgs() );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsAccessClient2::CloseCspSessionsL") );
#endif
    }
    
// ----------------------------------------------------------------------------
// RImpsAccessClient2::DoRegister()
// ----------------------------------------------------------------------------
TInt RImpsAccessClient2::DoRegister(
    RImpsEng& aEngine,
    CImpsHandler2* aHandler )
    {
    return DoRegister2( aEngine, aHandler, EImpsAccessRegister2 );
    }

// ----------------------------------------------------------------------------
// RImpsAccessClient2::Handler
// ----------------------------------------------------------------------------
MImpsAccessHandler2* RImpsAccessClient2::Handler( )
   {
   return iHandlerCallBack;
   }

// ----------------------------------------------------------------------------
// RImpsAccessClient2::LoginOpId()
// ----------------------------------------------------------------------------
TInt RImpsAccessClient2::LoginOpId()
	{
	return iLoginOpId;	
	}

// ----------------------------------------------------------------------------
// RImpsAccessClient2::LoginCancelled()
// ----------------------------------------------------------------------------
TBool RImpsAccessClient2::LoginCancelled()
	{
	return iLoginCancelled;	
	}

// ----------------------------------------------------------------------------
// RImpsAccessClient2::CancelHandled()
// ----------------------------------------------------------------------------
void RImpsAccessClient2::CancelHandled()
	{
	iLoginOpId = 0;
	iLoginCancelled = EFalse;	
	}
// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
