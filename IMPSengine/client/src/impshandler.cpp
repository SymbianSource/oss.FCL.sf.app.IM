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
* handler class for imps common client.
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "impsclient.h"
#include "impsfields.h"
#include "impspacked.h"
#include "impskey.h"
#include "impshandler.h"
#include "impsutils.h"
#include "impsservercommon.h"
#include "impserrors.h"
#include "impsdetailed.h"
#include "impsdataaccessor.h"
#include "impscdatautils.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TWvBuf::TWvBuf
// -----------------------------------------------------------------------------

TWvBuf::TWvBuf( HBufC8* aBuf ) :
        iPtr( aBuf->Des() ) 
        {}

// ----------------------------------------------------------------------------
// CImpsHandler2::CImpsHandler2()
// ----------------------------------------------------------------------------
CImpsHandler2::CImpsHandler2( 
    TInt aPriority, 
    RImpsClient2& aClient  ) :
    CActive( aPriority ), 
    iClient( aClient ),
    iBody( NULL ),
    iPtrStore( NULL )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::~CImpsHandler2()
// ----------------------------------------------------------------------------
 CImpsHandler2::~CImpsHandler2()
     {

     delete iBody;
     delete iFields;
     delete iPtrStore;
     delete iBusyTimer;

     if ( iDestroyedPtr )
         {
         // We are called via own RunL!
         *iDestroyedPtr = ETrue;
         iDestroyedPtr = NULL;
         }

     // Do not call Cancel() here, because of it is called outside if needed.
     // This is observer for server thread, so do not cancel yourself!
     }

// ----------------------------------------------------------------------------
// CImpsHandler2::StartRun()
// ----------------------------------------------------------------------------
void CImpsHandler2::StartRun()
    {

    // Start to wait push messages from the server
    iStatus = KRequestPending;
    SetActive();
    iFields->Reset();

    }

// ----------------------------------------------------------------------------
// CImpsHandler2::RunL()
// ----------------------------------------------------------------------------
void CImpsHandler2::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandler2::RunL begin status=%d h=%d cli=%d"), 
        iStatus.Int(), (TInt)this, (TInt)&iClient );
#endif

    // Set the member to point to stack variable
    TBool   destroyed( EFalse );
    iDestroyedPtr = &destroyed;
    // Is the event handled
    TBool handled( EFalse );
    TInt err = KErrNone;
    
    
    // Server is busy
    if ( iStatus == KErrServerBusy )
        {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandler2::Starting timer") ); 
#endif
		iBusyTimer->Start();
        return;
        }
    
    // Server is cancelled
    if ( iStatus == KErrCancel || iStatus == KErrServerTerminated )
        {
        return;
        }

    // special "error" codes for status changes
    if ( iStatus == KImpsOnlineStatus ||
         iStatus == KImpsOfflineStatus ||
         iStatus == KImpsNotLoggedStatus || 
         iStatus == KImpsErrorShuttingDown )
        {
        TImpsServiceStatus newStatus = EImps_ON_LINE;
        if ( iStatus == KImpsNotLoggedStatus )
            {
            newStatus = EImps_NOT_LOGGED;
            }
        else if ( iStatus == KImpsErrorShuttingDown )
            {
            newStatus = EImps_SHUTTING_DOWN;
            }
        if ( iStatusHandler )
            {
            TInt err ( KErrNone );
            TRAP( err, iStatusHandler->HandleStatusChangeL( newStatus , *iClient.CspIdentifier() ));
            handled = ETrue;
            }
        }

    // Destructor sets this to ETrue. An application may have called unregister
    if ( destroyed )
        {       
        return;
        }
    
    TInt dataLength = iStatus.Int();

    // Check first if a special OOM error event
    if ( dataLength > 0 && ( dataLength & KImpsOOMReply ) )
        {
        TInt errId = dataLength & KImpsOOMReplyOpId;
        dataLength = 0;
        err = KErrNoMemory;
        // This cannot leave with NULL fields parameter
        HandleErrorEventL( errId, KErrNoMemory, NULL );
        }

    // Read the data if any to iFields
    // Check if need to read message body separately
    TBool comp( ETrue );
    if ( dataLength > 0 )
        {
        TRAP( err, comp = ReadDataL( dataLength ) );
        }

    if ( !err && !handled && comp )
        {
        // Not here if reading of data fails
        CImpsFields* errF = ( dataLength > 0 ? iFields : NULL );
        { // These parenthises must be here.
          // Otherwise the program will crash
          // This is releated to the destroying this object
          // and trap frame.

        // this trap handles all internal errors
        // errors caused by callbacks are trapped elsewhere
        TRAP( err, HandleEventL( errF ) ); 
        if ( err != KErrNone )
            {
            SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();
            HandleErrorEventL( event->iOpCode, err, NULL );
            }
        }
        }

    // Destructor sets this to ETrue, nice trick.
    if ( !destroyed )
        {       
        // Activate again and reset data.
        // This must not leave but has to ask for more by
        // calling either EventHandled() or EventBody(). 
        StartRun();
        TInt bufSize = CurrentSize();
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandler2: comp=%d bufSize=%d h=%d cli=%d"), 
        comp, bufSize, (TInt)this, (TInt)&iClient );
#endif
        if ( comp )
            {
            // Message handling is complete. Free the buffer if the 
            // message was a big one.
            if ( bufSize > KImpsIPCThreshold )
                {
                ResetBuffers();
                }
            EventHandled();
            }
        else
            {
            if ( CurrentSize() < dataLength )
                {
                // allocate memory if needed
                ResetBuffers();
                TRAP( err, 
                    {
                    iBody = HBufC8::NewL( dataLength );  
                    iPtrStore = new (ELeave) TWvBuf( iBody );
                    } );
                }
            if ( err )
                {
                ResetBuffers();
                EventHandled();
                }
            else
                {
                // Ask rest of the message
                EventBody();
                }
            }
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(_L("CImpsHandler2: DESTROYED **** cli=%d"), (TInt)&iClient );
#endif
        }

    iDestroyedPtr = NULL; 

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandler2: RunL ends cli=%d"), (TInt)&iClient);
#endif    
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::ReadDataL()
// ----------------------------------------------------------------------------
TBool CImpsHandler2::ReadDataL( TInt aLength )
    {
    // Event data has been given in next event request
    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();

    if ( event->iMessageBody )
        {
        // verify iBody.Length and aLength
       __ASSERT_DEBUG( iBody && aLength > 0 && iBody->Des().Length() == aLength, 
                       User::Panic( KImpsPanicCategory, EImpsCorrupted ) );

        // Unpack the streaming
        iFields->Reset();
        TImpsPackedEntity packed( iBody );
        packed.UnpackEntityL( *iFields );
        }
    else if ( aLength > CurrentSize() )
        {
        // This means event without body
        return EFalse;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::DoCancel()
// ----------------------------------------------------------------------------
void CImpsHandler2::DoCancel()
    {

    // Complete the request with error
    TRequestStatus* s = &iStatus;
    User::RequestComplete( s, KErrCancel );
    }

// ----------------------------------------------------------------------------
// CImpsHandler2:: SetStatusHandlerL
// ----------------------------------------------------------------------------
void CImpsHandler2::SetStatusHandlerL( MImpsStatusHandler2* aObs)
    {

    iStatusHandler = aObs;
    // register new handler
    if ( iStatusHandler )
        {
        DoRegisterStatusHandlerL(  );
        }
    else
        {
        DoUnregisterStatusHandlerL( );
        }
    }

// ----------------------------------------------------------------------------
// CImpsHandler2:: SetErrorHandlerL
// ----------------------------------------------------------------------------
void CImpsHandler2::SetErrorHandlerL( MImpsErrorHandler2* aObs)
    {

    iErrorHandler = aObs;
    // register new handler
    if ( iErrorHandler )
        {
        DoRegisterErrorHandlerL(  );
        }
    else
        {
        DoUnregisterErrorHandlerL( );
        }
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::DoRegisterStatusHandlerL
// ----------------------------------------------------------------------------
void CImpsHandler2::DoRegisterStatusHandlerL(  )
    {
    TInt ret =  iClient.SendReceive( EImpsServStatusReg, TIpcArgs() );
    User::LeaveIfError( ret );
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::DoUnregisterStatusHandlerL
// ----------------------------------------------------------------------------
void CImpsHandler2::DoUnregisterStatusHandlerL(  )
    {
    TInt ret =  iClient.SendReceive( EImpsServStatusUnreg, TIpcArgs() );
    User::LeaveIfError( ret );
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::DoRegisterErrorHandlerL
// ----------------------------------------------------------------------------
void CImpsHandler2::DoRegisterErrorHandlerL(  )
    {
    TInt ret =  iClient.SendReceive( EImpsServDetailedReg, TIpcArgs() );
    User::LeaveIfError( ret );
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::DoUnregisterErrorHandlerL
// ----------------------------------------------------------------------------
void CImpsHandler2::DoUnregisterErrorHandlerL(  )
    {
    TInt ret =  iClient.SendReceive( EImpsServDetailedUnreg, TIpcArgs() );
    User::LeaveIfError( ret );
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::HandleErrorEventL
// ----------------------------------------------------------------------------
void CImpsHandler2::HandleErrorEventL(
            TInt aOpCode, TInt aStatus, CImpsFields* aFields)
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandler2: HandleErrorEventL opid=%d stat=%d cli=%d"),
                aOpCode, aStatus, (TInt)&iClient );
#endif


    if ( !iErrorHandler )
        {
        // handler should exist
        return;
        }

    if ( aFields == NULL )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(_L("CImpsHandler2: HandleError call"));
#endif
        // This has been internal error
        iErrorHandler->HandleErrorL( 
            aStatus,
            aOpCode,
            NULL,
            NULL,
            *iClient.CspIdentifier() );
        return;
        }


    CImpsDetailed* myDet = new (ELeave) CImpsDetailed(4);
    CleanupStack::PushL( myDet );       // <<< myDet
    TPtrC descr;
    descr.Set( KNullDesC );

    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewL( aFields );
    CleanupStack::PushL( myAc );            // <<< myAc

    TImpsCDataUtils::GetDetailedResultL( myKey, myAc, descr, myDet );
    CleanupStack::PopAndDestroy(2);     // >>> myKey, myAc

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandler2: HandleError call"),
                aOpCode, aStatus );
#endif

    iErrorHandler->HandleErrorL( 
        aStatus,
        aOpCode,
        &descr,
        myDet,
        *iClient.CspIdentifier() );

    CleanupStack::PopAndDestroy(1);  // >>> myDet
    }

// -----------------------------------------------------------------------------
// CImpsHandler2::ConstructL
// -----------------------------------------------------------------------------
void CImpsHandler2::ConstructL()
    {
    iFields = CImpsFields::NewL();
    iBusyTimer = new (ELeave) CImpsHandlerTimer( CActive::EPriorityStandard, *this);
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::EventHandled()
// ----------------------------------------------------------------------------
void CImpsHandler2::EventHandled()
    {
    TInt bufSize = CurrentSize();
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandler2: EImpsServNextEvent bufSize=%d h=%d cli=%d"), 
        bufSize, (TInt)this, (TInt)&iClient );
#endif
    iArgs.Set( 0, &iEventData );
    iArgs.Set( 1, &iPtrStore->iPtr ); 
    iArgs.Set( 2, bufSize );      
    iClient.SendReceive( EImpsServNextEvent, iArgs, iStatus );
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::EventBody()
// ----------------------------------------------------------------------------
void CImpsHandler2::EventBody()
    {
    __ASSERT_DEBUG( iBody && iPtrStore, 
                    User::Panic( KImpsPanicCategory, EImpsCorrupted ) );
    TInt bufSize = CurrentSize(); 
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsHandler2: EImpsServEventBody bufSize=%d h=%d cli=%d"), 
        bufSize, (TInt)this, (TInt)&iClient );
#endif
    iArgs.Set( 0, &iEventData );
    iArgs.Set( 1, &iPtrStore->iPtr ); 
    iArgs.Set( 2, bufSize );
    iClient.SendReceive( EImpsServEventBody, iArgs, iStatus );
    }

// ----------------------------------------------------------------------------
// CImpsHandler2::ResetBuffers()
// ----------------------------------------------------------------------------
void CImpsHandler2::ResetBuffers()
    {
    delete iPtrStore;
    iPtrStore = NULL;
    delete iBody;
    iBody = NULL;
    }

//  End of File  
