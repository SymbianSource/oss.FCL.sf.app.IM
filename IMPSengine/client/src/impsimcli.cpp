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
* imps client interface.
*
*/

// INCLUDE FILES

#include    "impsImHandler.h"
#include    "ImpsImCli.h"
#include    "ImpsImCommand.h"
#include    "impsServerCommon.h"

#include    "impsutils.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RImpsImClient2::RImpsImClient2
// ---------------------------------------------------------
EXPORT_C RImpsImClient2::RImpsImClient2() :
        RImpsClient2(),
        iImHandlerCallback( NULL ),
        iBlockingHandlerCallback( NULL ),
        iCommand( NULL )
    {
    SetOpIdRange( ); 
    }

// ---------------------------------------------------------
// RImpsImClient2::RegisterL
// ---------------------------------------------------------
EXPORT_C void RImpsImClient2::RegisterL(
    RImpsEng& aEngine, 
    MImpsImHandler2* aImpsObserver,
    MImpsBlockingHandler2* aBlockingHandler,
    const TDesC& aClientId,
    TBool aReceiveNew,
    TInt aPriority )
    {
    iImHandlerCallback = aImpsObserver;
    iBlockingHandlerCallback = aBlockingHandler;
    iHandler = CImpsImHandler2::NewL( *this, aPriority );
    iCommand = CImpsImCommand2::NewL( *this );
    iActiveCommand = iCommand;

    // Start the handler
    iHandler->StartRun();
    iHandleNew = aReceiveNew;
    iAnyContent = EFalse;

    // Send registration to the server thread
    TInt err = DoRegister( aEngine, iHandler );
    User::LeaveIfError( err ); 

    (void) DoAssign( aClientId );
    }

// ----------------------------------------------------------------------------
// RImpsImClient2::Unregister()
// ----------------------------------------------------------------------------
EXPORT_C void RImpsImClient2::Unregister()
    {
    DoUnregister();
    }

// ----------------------------------------------------------------------------
// RImpsImClient2::DoRegister()
// ----------------------------------------------------------------------------
TInt RImpsImClient2::DoRegister(
   RImpsEng& aEngine,
   CImpsHandler2* aHandler )
   {
   return DoRegister2( aEngine, aHandler, EImpsImRegister );
   }

// ----------------------------------------------------------------------------
// RImpsImClient2::SendTextMessageL()
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsImClient2::SendTextMessageL( 
            const TDesC* aSenderSn,     
            const MDesCArray* aUserIds,
            const TDesC* aGroupId,         
            const MDesCArray* aScreenNames, 
            const TDesC& aContent,
            TBool aDeliveryReportWanted )
    {
    IncreaseOpId( );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsImClient2: SendTextMessageL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif

    iCommand->SetParametersL( 
        aSenderSn, aUserIds, aGroupId, aScreenNames, aContent, aDeliveryReportWanted );

    iMessagePtr.Set( iCommand->iMessageStream->Des() );
    iCommand->StartRunL( iOpId, EImpsServWVSendOnly );
    SendReceive( EImpsServWVSendOnly, 
                 TIpcArgs( &iMessagePtr, iOpId ), iCommand->iStatus );

    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsImClient2::BlockEntityRequestL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsImClient2::BlockEntityRequestL( 
    const MDesCArray* aBlockEntity,
    const MDesCArray* aUnBlockEntity,
    TBool aBlockedListInUse,
    const MDesCArray* aGrantedEntityList,
    const MDesCArray* aUnGrantEntity,
    TBool aGrantedListInUse )
    {
    IncreaseOpId( );

    iCommand->SetParametersL( 
         aBlockEntity, aUnBlockEntity, aBlockedListInUse, 
         aGrantedEntityList, aUnGrantEntity, aGrantedListInUse );

    iMessagePtr.Set( iCommand->iMessageStream->Des() );
   
    iCommand->StartRunL( iOpId,  EImpsServBlock );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsImClient2: BlockEntityRequestL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif
    SendReceive( EImpsServBlock, 
                 TIpcArgs( &iMessagePtr, iOpId ), iCommand->iStatus );
    
    return iOpId;

    }

// ----------------------------------------------------------------------------
// RImpsImClient2::GetBlockedListRequestL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsImClient2::GetBlockedListRequestL()
    {
    IncreaseOpId( );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsImClient2: GetBlockedListRequestL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif

    // This has no serialized data, but the message 
    // primitive is created in a server thread to minimize
    // the amount of data transferred between client and server thread.
    iCommand->StartRunL( iOpId,  EImpsServBlock );
    TImpsServRequest req = EImpsServGetBlocked;
    SendReceive( req, TIpcArgs( TIpcArgs::ENothing, iOpId ), iCommand->iStatus );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsImClient2: GetBlockedListRequestL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsImClient2::SendContentMessageL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsImClient2::SendContentMessageL( const TDesC* aSenderSn,     
                                           const MDesCArray* aUserIds,
                                           const TDesC* aGroupId,         
                                           const MDesCArray* aScreenNames, 
                                           const TDesC& aContentType, 
                                           const TDesC8& aContent,
                                           TBool aDeliveryReportWanted )
    {
    IncreaseOpId( );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsImClient2: SendContentMessageL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif

    iCommand->SetParametersL( 
        aSenderSn, aUserIds, aGroupId, aScreenNames, aContentType, aContent, aDeliveryReportWanted );

    iMessagePtr.Set( iCommand->iMessageStream->Des() );
    iCommand->StartRunL( iOpId, EImpsServWVSendOnly );
    SendReceive( EImpsServWVSendOnly, 
                 TIpcArgs( &iMessagePtr, iOpId ), iCommand->iStatus );

    return iOpId;
    }
// ----------------------------------------------------------------------------
// RImpsImClient2::Handler
// ----------------------------------------------------------------------------
MImpsImHandler2* RImpsImClient2::Handler( )
   {
    return iImHandlerCallback;
    }

// ----------------------------------------------------------------------------
// RImpsImClient2::BlockHandler
// ----------------------------------------------------------------------------
MImpsBlockingHandler2* RImpsImClient2::BlockHandler( )
   {
    return iBlockingHandlerCallback;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  
