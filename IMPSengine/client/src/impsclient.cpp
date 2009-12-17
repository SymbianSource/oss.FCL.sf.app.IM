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
* class for imps server.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    <e32std.h>
#include    "impsservercommon.h"
#include    "impshandler.h"
#include    "impsclient.h"
#include    "impserrors.h"
#include    "impsutils.h"
#include    "impsclientsrv.h"
#include    "impsconst.h"

// CONSTANTS
const TInt  KWaitForServerRetries = 3;

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// RImpsEng::RImpsEng
// ----------------------------------------------------------------------------
EXPORT_C RImpsEng::RImpsEng()
: RSessionBase(),
  iSessionOk (EFalse )
    {
    }

// ----------------------------------------------------------------------------
// RImpsEng::Connect
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsEng::Connect( TInt aRate /*, TBool aReqular*/ )
    {
    // This has similar functionality than old version RImpsClient
    TInt err = KErrNone;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client: Connect rate=0x%x"), aRate);
#endif

    for ( TInt tries = 0; tries < KWaitForServerRetries ; tries++ )
        { 
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client: Calling CreateSession "));
#endif
        err = CreateSession( 
                KImpsServerName, Version(), KImpsDefaultMessageSlots );

        if ( err == KErrNone )
            {
            iSessionOk = ETrue;
            break; // connected to existing server - ok
            }

        // problems other than server not here - propagate error
        if ( err != KErrNotFound && err != KErrServerTerminated ) 
            {
            break;
            }

        err = StartServer( aRate, 0 );    // start a new server
        }
#ifndef _NO_IMPS_LOGGING_
    const TSecureId currentSID( RProcess().SecureId() );
    CImpsClientLogger::Log(_L("Client Connect returns=0x%x currentSID=0x%x"), err, (TInt)currentSID);

    TBuf<256> name;
	RProcess currentProcess;
    name.Append( RProcess().FullName() );
    CImpsClientLogger::Log( _L("Client CONNECT Process name: %S"), &name );
#endif
    return err;
    }

// ----------------------------------------------------------------------------
// RImpsEng::Close
// ----------------------------------------------------------------------------
EXPORT_C void RImpsEng::Close()
    {  
#ifndef _NO_IMPS_LOGGING_
    const TSecureId currentSID( RProcess().SecureId() );
    CImpsClientLogger::Log(_L("Client Close currentSID=0x%x"), (TInt)currentSID);
    
    TBuf<256> name;
	RProcess currentProcess;
    name.Append( RProcess().FullName() );
    CImpsClientLogger::Log( _L("Client CLOSE Process name: %S"), &name );
#endif

    iSessionOk = EFalse;
    RSessionBase::Close();   // deletes session object
    }

// ----------------------------------------------------------------------------
// RImpsEng::StartServer
// ----------------------------------------------------------------------------
TInt RImpsEng::StartServer( TInt aRate, TInt /*aRound*/ )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client: StartServer rate=0x%x"), aRate);
#endif

    TInt ret( KErrNone );
    TRequestStatus status  = KRequestPending;
    TImpsSignal signal( aRate );  

    if ( !IsServerStarted() )
        {
        // launch server process
        

        RProcess server;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(_L("Client: PROCESS TO START"));
#endif

        ret = server.Create( 
            KImpsServerExe,  
            signal.Get() );
            
        if ( ret )
            {
            // we are lost
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log(_L("Client: process START FAILED"));
#endif
            return ret;
            }
            
        
              
        server.Rendezvous( status );    	
        if ( status != KRequestPending )
            {            
            server.Kill(0);		// abort startup
            }
        else
            {          
            server.Resume();	// logon OK - start the server
            }
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(_L("Client: WaitForAnyRequest"));
#endif		
        // wait for start or death 
        User::WaitForRequest( status );	
        
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(_L("WaitForRequest return=%d"), status.Int() );
#endif        
        // we can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        ret = (server.ExitType() == EExitPanic) ? KErrGeneral : status.Int();
        server.Close();	
    	
        } // if !IsStarted
    
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("StartServer return=0x%x"), ret);
#endif
    return ret;
    }

// ----------------------------------------------------------------------------
// RImpsEng::IsServerStarted
// ----------------------------------------------------------------------------
TBool RImpsEng::IsServerStarted()
    {
    TFindServer findServer( KImpsServerName );
    TFullName name;
    return ( findServer.Next( name ) == KErrNone );
    }

// ----------------------------------------------------------------------------
// RImpsEng::Version()
// Returns the client side version number.
// ----------------------------------------------------------------------------
TVersion RImpsEng::Version() const
    {
    return TVersion( KImpsServMajorVersionNumber, KImpsServMinorVersionNumber,
                     KImpsServBuildVersionNumber );
    }

// ----------------------------------------------------------------------------
// RImpsEng::CspIdentifier()
// ----------------------------------------------------------------------------
TImpsCspIdentifier* RImpsEng::CspIdentifier()
    {
    return &iCspId;
    }
    
// ----------------------------------------------------------------------------
// RImpsEng::SetCspIdentifier()
// ----------------------------------------------------------------------------
void RImpsEng::SetCspIdentifier( TImpsCspIdentifier aCspId )
    {
    TInt err ( KErrNone );
    TRAP ( err, iCspId.SetSapL( aCspId.Sap() ) );
    TRAP ( err, iCspId.SetUserIdL( aCspId.UserId() ) );
    }
    
// ---------------------------------------------------------
// RImpsClient2::RImpsClient2
// ---------------------------------------------------------
//
EXPORT_C RImpsClient2::RImpsClient2() :
        iHandler( NULL ), 
        iActiveCommand( NULL ),
        iRunning( EFalse ),
        iOpId( 0 ),
        iLimitUpperValue( 0 ),
        iLimitLowerValue( 0 ),
        iEngine( NULL )
    {}
  
// ---------------------------------------------------------
// RImpsClient2::DoAssign
// ---------------------------------------------------------
//
TInt RImpsClient2::DoAssign(
   const TDesC& aClientId )
   {
   TInt err = KErrNone;
   TPtrC myId;
   myId.Set( aClientId );
   err = SendReceive( EImpsServAssign, TIpcArgs( &myId ) );
   return err;
   }   

// ---------------------------------------------------------
// RImpsClient2::DoUnregister
// SubSession Close
// ---------------------------------------------------------
//
void RImpsClient2::DoUnregister()
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client: DoUnregister cli=%d"), (TInt)this);
#endif
    if ( iRunning )
       {
       // Send cancel message to the server thread
       TInt srvRet = SendReceive( EImpsServCloseSub, TIpcArgs() );

       // Server completes pending next event request message for this subsession
       __ASSERT_DEBUG( iHandler, 
                       User::Panic( KImpsPanicCategory,
                                    EImpsNotifyHandlerMissing ) );
       if ( !srvRet && iHandler->IsActive() )
           {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(_L("Client2: WaitForRequest START %d"), srvRet);
#endif
           User::WaitForRequest( iHandler->iStatus );  

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(_L("Client2: WaitForRequest END"));
#endif
           }

       iRunning = EFalse;
       RSubSessionBase::CloseSubSession( EImpsServDeleteSub );
       }

   // Cancel active objects before deleting them. 
   // Server is informed about cancel above so that it will not send data.
   if ( iHandler )
       {
       iHandler->Cancel();
       }
   if ( iActiveCommand )
       {
       iActiveCommand->Cancel();
       }

   // Now this is safe, i.e. no requests outstanding for the handler
   delete iHandler; 
   iHandler = NULL;

   delete iActiveCommand;
   iActiveCommand = NULL;
   }


// ----------------------------------------------------------------------------
// RImpsClient2::RegisterStatusObserverL
// ----------------------------------------------------------------------------
EXPORT_C void RImpsClient2::RegisterStatusObserverL( 
    MImpsStatusHandler2* aObserver )
    {
    if ( !iHandler )
        {
        // Register must have been done in derived client API!
        User::Leave( KImpsErrorNotRegistered );
        }

    if ( iHandler->StatusHandler() == NULL )
        {
        iHandler->SetStatusHandlerL( aObserver );
        }
    }

// ----------------------------------------------------------------------------
// RImpsClient2::UnregisterStatusObserverL
// ----------------------------------------------------------------------------
EXPORT_C void RImpsClient2::UnregisterStatusObserverL( )
    {
    // Check if null if somebody calls this after Unregister anyway ...
    if ( iHandler )
        {
        if ( iHandler->StatusHandler() != NULL )
            {
            iHandler->SetStatusHandlerL( NULL );
            }
        }
    }

// ----------------------------------------------------------------------------
// RImpsClient2::RegisterErrorObserverL
// ----------------------------------------------------------------------------
EXPORT_C void RImpsClient2::RegisterErrorObserverL( 
    MImpsErrorHandler2& aObserver )
    {

    if ( !iHandler )
        {
        // Register must have been done in derived client API!
        User::Leave( KImpsErrorNotRegistered );
        }

    if ( iHandler->ErrorHandler() == NULL )
        {
        iHandler->SetErrorHandlerL( &aObserver );
        }
    }

// ----------------------------------------------------------------------------
// RImpsClient2::UnregisterErrorObserverL
// ----------------------------------------------------------------------------
EXPORT_C void RImpsClient2::UnregisterErrorObserverL( )
    {
    // Check if null if somebody calls this after Unregister anyway ...
    if ( iHandler )
        {
        if ( iHandler->ErrorHandler() != NULL )
            {
            iHandler->SetErrorHandlerL( NULL );
            }
        }
    }

// ----------------------------------------------------------------------------
// RImpsClient2::MaxTransactionContentLengthL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsClient2::MaxTransactionContentLengthL( )
    {
    TInt ret =  SendReceive( EImpsServBuffSizeReq, TIpcArgs() );
    if ( ret < 0 )
        {
        User::Leave( ret );
        } 
    return ret;
    }

// ----------------------------------------------------------------------------
// RImpsClient2::ErrorHandlerL
// ----------------------------------------------------------------------------
MImpsErrorHandler2* RImpsClient2::ErrorHandler()
    {
    if ( iHandler )
        {
        return ( iHandler->ErrorHandler() );
        }
    return (MImpsErrorHandler2*)NULL;
    }

// ----------------------------------------------------------------------------
// RImpsClient2::GetServicesL
// ----------------------------------------------------------------------------
EXPORT_C void RImpsClient2::GetServicesL( TImpsServices& aServices )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client2: GetServicesL starts"));
#endif

    // result buffer
    TBuf<sizeof( TImpsServices ) + 4> myBuf( sizeof( TImpsServices ) + 4 );

    // Server will write the data to myservices
    TInt ret = SendReceive( EImpsServServices, TIpcArgs( &myBuf ) );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client2: GetServicesL SendReceive ret=%d"), ret );
#endif
    User::LeaveIfError( ret );

    // convert descriptor to TImpsServices
    TInt32 tempSize = 0;
    const TUint8* textPtr = (const TUint8*)myBuf.Ptr();
    Mem::Copy( &tempSize, textPtr, sizeof( tempSize) );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client2: GetServicesL tempSize=%d"), tempSize);
#endif
    textPtr = textPtr + sizeof( tempSize );
    // copy result to the call parameter
    Mem::Copy( (void*)&aServices, textPtr, tempSize );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client2: GetServicesL ends Ok"));
#endif

    }

// ----------------------------------------------------------------------------
// RImpsClient2::WVVersionL
// ----------------------------------------------------------------------------

EXPORT_C TPtrC RImpsClient2::WVVersionL()
    {
    TInt ret =  SendReceive( EImpsServCspVersion, TIpcArgs() );
    if ( ret == EImpsCspVersion12 )
        {
        return TPtrC( KImpsVersionStr );
        } 
    return TPtrC( KImpsVersionStr12 );
    } 

// ----------------------------------------------------------------------------
// RImpsClient2::CspIdentifierL( )
// ----------------------------------------------------------------------------
EXPORT_C TImpsCspIdentifier* RImpsClient2::CspIdentifierL( )
    {
    TInt ret =  SendReceive( EImpsServIsLogged, TIpcArgs() );
    if ( ret < 0 )
        {
        User::Leave( KErrNotFound );
        } 
    return iEngine->CspIdentifier();
    }

// ----------------------------------------------------------------------------
// RImpsClient2::CancelTransaction
// ----------------------------------------------------------------------------
EXPORT_C void RImpsClient2::CancelTransaction( TInt aOperationId )
    {
    TInt ret = SendReceive( EImpsServCancelTrans, TIpcArgs( aOperationId ) );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsClient2::CancelTransaction opId=%d ret=%d"), aOperationId, ret );
#endif
    }    

// ----------------------------------------------------------------------------
// RImpsClient2::SetExpiryTime
// ----------------------------------------------------------------------------
EXPORT_C void RImpsClient2::SetExpiryTime( TInt aSeconds )
    {
    TInt ret = SendReceive( EImpsServSetExpiry, TIpcArgs( aSeconds ) );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsClient2::SetExpiryTime time=%d ret=%d"), aSeconds, ret );
#endif
    }
    
// ----------------------------------------------------------------------------
// RImpsClient2::IncreaseOpId
// ----------------------------------------------------------------------------
void RImpsClient2::IncreaseOpId( )
    {
    iOpId++;
    if( iOpId >= iLimitUpperValue )
        {
        iOpId = iLimitLowerValue;
        }
    }

// ----------------------------------------------------------------------------
// RImpsClient2::SetOpIdRange
// ----------------------------------------------------------------------------
void RImpsClient2::SetOpIdRange( )
    {
    iLimitLowerValue = REINTERPRET_CAST(TInt, Dll::Tls() );
    iLimitUpperValue = iLimitLowerValue + 100;
    // Check that counter does not go around
        if ( iLimitUpperValue & KImpsOOMReply )
        {
        // start from 50, if this ever happens
        iLimitLowerValue = 50;
        iLimitUpperValue = iLimitLowerValue + 100;
        }
    Dll::SetTls( REINTERPRET_CAST(TAny*, iLimitUpperValue ) );
    iOpId = iLimitLowerValue;
    }

// ----------------------------------------------------------------------------
// RImpsClient2::DoRegister2
// ----------------------------------------------------------------------------
TInt RImpsClient2::DoRegister2( 
    RImpsEng& aEngine, CImpsHandler2* aHandler, TInt aReq )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("Client2: DoRegister2 cli=%d"), (TInt)this);
#endif

    iEngine = &aEngine;
    // add bit mask for extra info (handle-new and any-content)
    TInt srvReg = aReq;
    TInt bits = 0;
    bits = ( bits & ~KImpsReqHandleNew ) 
            | ( iHandleNew ? KImpsReqHandleNew : 0x0000 );
    bits = ( bits & ~KImpsReqAnyContent ) 
            | ( iAnyContent ? KImpsReqAnyContent : 0x0000 );
    bits = bits << 16;
    srvReg = srvReg | bits;

    TImpsServRequest req = (TImpsServRequest)srvReg;
    TInt err = CreateSubSession( aEngine, req );
    if ( !err )
        {
        iRunning = ETrue;
        aHandler->EventHandled();       
        }
    return err;
    }

// ----------------------------------------------------------------------------
// RImpsClient2::CspIdentifier( )
// ----------------------------------------------------------------------------
TImpsCspIdentifier* RImpsClient2::CspIdentifier( )
    {
    return iEngine->CspIdentifier();
    }    


//  End of File  

