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
* Description: Class for Data send and recevive.
*
*
*/


// INCLUDE FILES
#include    <apparc.h>
#include    <bautils.h>
#include    "impssendreceive.h"
#include    "Impsutils.h"
#include    "impserrors.h"
#include    "impstimer.h"
#include    "impsactiveconnmonitor.h"
#include    "Impsdatautils.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

#ifdef LOCAL_IMPS
#ifndef _FAKE_RESPONSE
#define _FAKE_RESPONSE
#endif
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsSendReceive2::CImpsSendReceive2(
    RFs& aFs,
    MImpsCSPSession& aServer,
    CBufFlat& aOutputBuffer )
        : iServer( aServer ),
        iOutputBuffer( aOutputBuffer ),
        iOpenId( 0 ),
        iFs( aFs ),
        iSender( NULL ),
        iRequList( _FOFF( CTrReq, iLink ) ),  //lint !e413
        iConnMan( NULL ),
        iSAP ( NULL ),
        iTrState( EImpsTrInit )
    {

    }

// default constructor can leave.
void CImpsSendReceive2::ConstructL( TDesC8& aMimeType )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: rel200541.3+" ) );
#endif
    iMimeType = aMimeType;
    iConnMan = CImpsConnManager::NewL( *this, CActive::EPriorityUserInput + 1 );
    MMsgConnManager& man = iConnMan->ManagerHandleL();
    iSender = NewImpsSenderL( *this, man, aMimeType );
    }

// Two-phased constructor.
CImpsSendReceive2* CImpsSendReceive2::NewL(
    RFs&  aFs,
    MImpsCSPSession& aServer,
    CBufFlat& aOutputBuffer,
    TDesC8& aMimeType )
    {
    CImpsSendReceive2* self = new ( ELeave ) CImpsSendReceive2(
        aFs,
        aServer,
        aOutputBuffer );

    CleanupStack::PushL( self );
    self->ConstructL( aMimeType );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CImpsSendReceive2::~CImpsSendReceive2()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: destructor begins" ) );
#endif
    delete iConnMan;
    delete iSAP;

    if ( iSender )
        {
        // In error case this may be null
        iSender->Destroy();
        }
    DeleteAllIds();
    // delete iObserver;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: destructor ends" ) );
#endif

    }

// ---------------------------------------------------------
// CImpsSendReceive2::CloseTr
// ---------------------------------------------------------
//
void CImpsSendReceive2::CloseTr( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: CloseTr" ) );
#endif

    DeleteAllIds();

    if ( iTrState == EImpsTrClosing )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: CloseTr IGNORED ***" ) );
#endif
        return;
        }
    // cancel open AP
    if ( iTrState == EImpsTrAPOpening )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: CANCEL open ***" ) );
#endif
        NewTrState( EImpsTrClosing );
        iConnMan->CloseAP( ETrue );
        return;
        }

    if ( iSender )
        {
        // In error cases iSender may be null
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: CloseTr CLOSE DATA CHANNEL" ) );
#endif
        iSender->Close( );
        iSender->Destroy();
        iSender = NULL;
        NewTrState( EImpsTrAPOpen );
        }
    else if ( iTrState == EImpsTrCloseDelayed )
        {
        NewTrState( EImpsTrAPOpen );
        }

    if ( iTrState == EImpsTrAPOpen )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: CloseTr START TO CLOSE AP" ) );
#endif
        // This causes APStatusEvent call finally.
        NewTrState( EImpsTrClosing );
        iConnMan->CloseAP( ETrue );
        }

    // This is needed in CImpsServer so that transient server can be closed
    else if ( iTrState == EImpsTrInit )
        {
        iServer.TransportStatus( EInternal_NO_IAP );
        }
    }

// ---------------------------------------------------------
// CImpsSendReceive2::Close2
// Delayed close operation
// ---------------------------------------------------------
//
TInt CImpsSendReceive2::Close2( )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: Close2" ) );
#endif

    __ASSERT_DEBUG( iConnMan,
                    User::Panic( KImpsPanicCategory, EImpsCorrupted ) );

    TInt ret = KErrNone;

    if ( iTrState == EImpsTrAPOpening )
        {
        CloseTr();
        return ret;
        }
    else if ( iTrState == EImpsTrClosing || iTrState == EImpsTrCloseDelayed )
        {
        // There is pending close request that will call callback sooner or later.
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: Close2 ignored ***" ) );
#endif
        return ret;
        }
    else if ( iTrState == EImpsTrInit )
        {
        // Nothing to do, callback will not be called.
        // This error must be handled in calling method.
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: iTrState was EImpsTrInit ***" ) );
#endif
        return KErrCancel;
        }

    DeleteAllIds();

    if ( iSender )
        {
        // In error cases iSender may be null
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: Close2 CLOSE DATA CHANNEL" ) );
#endif
        iSender->Close( );
        iSender->Destroy();
        iSender = NULL;
        NewTrState( EImpsTrAPOpen );
        }

    // yield control for scheduler before close takes place
    iConnMan->StartDelayed( );
    NewTrState( EImpsTrCloseDelayed );

    return ret;

    }

// ---------------------------------------------------------
// CImpsSendReceive2::IsConnected
// ---------------------------------------------------------
//
TBool CImpsSendReceive2::isConnected( )
    {
    TBool ret( EFalse );
    TRAPD( errx, ( ret = DoIsConnectedL() ) );
    if ( errx || !ret )
        {
        return EFalse;
        }

    if ( iSender && iTrState == EImpsTrDataOpen )
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CImpsSendReceive2::DoIsConnectedL
// ---------------------------------------------------------
//
TBool CImpsSendReceive2::DoIsConnectedL( )
    {
    MMsgConnManager& man = iConnMan->ManagerHandleL();
    if ( !man.Status() )
        {
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CImpsSendReceive2::OpenL
// ---------------------------------------------------------
//
void CImpsSendReceive2::OpenL(
    const TDesC& aTID,
    TInt aXmlLog,
    const TDesC& aSAP,
    TInt  aIAP,
    TBool aDelayed )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: OpenL Begins iTrState=%d" ), iTrState );
#endif

    delete iSAP;
    iSAP = NULL;
    iSAP = aSAP.AllocL();

    iIAP = aIAP;
    DeleteId( iOpenId );
    iOpenId = 0;
    iOpenId = AddIdL( aTID ); // saved so that it can be deleted later
    iXmlLog = aXmlLog;

    if ( iTrState == EImpsTrClosing || iTrState == EImpsTrCloseDelayed ||
         iTrState == EImpsTrClosingPDP )
        {
        // just parameters saved
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: Leaves(KErrNotReady) ***" ) );
#endif
        DeleteId( iOpenId );
        iOpenId = 0;
        User::Leave( KErrNotReady );
        }

    OpenL( aTID, aDelayed );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: OpenL ends" ) );
#endif

    }

// ---------------------------------------------------------
// CImpsSendReceive2::OpenL
// ---------------------------------------------------------
//
void CImpsSendReceive2::OpenL( const TDesC& aTID, TBool aDelayed )
    {
    // Prevent opening twice data channel or connection
    if ( iTrState == EImpsTrAPOpening )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: Leaves(KErrNotReady) ****" ) );
#endif
        User::Leave( KErrNotReady );
        }

    // Handle NULL TID so that Login primitive is not sent later,
    // only channels are opened when recovering from flight mode.
    // The old state is ignored, because of
    // this should be called after NO_IAP only.

    if ( aTID.Length() == 0 )
        {
        iOpenId = 0;
        }

    if ( iSender && iTrState == EImpsTrDataOpen )
        {
        iSender->CancelAll();
        iSender->Destroy();
        iSender = NULL;
        NewTrState( EImpsTrAPOpen );
        }
    if ( iTrState == EImpsTrAPOpen )
        {
        NewTrState( EImpsTrClosing );
        iConnMan->CloseAP( ETrue );
        }
    else if ( iTrState == EImpsTrInit )
        {
        Open2L( iIAP, aDelayed );
        }
    }

// ---------------------------------------------------------
// CImpsSendReceive2::Open2L
// ---------------------------------------------------------
//
void CImpsSendReceive2::Open2L( TInt aIAP, TBool aDelayed )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: Open2L Begins aIAP=%d" ), aIAP );
#endif

    __ASSERT_DEBUG( iConnMan,
                    User::Panic( KImpsPanicCategory, EImpsCorrupted ) );

    iConnMan->OpenAPL( aIAP, aDelayed );
    NewTrState( EImpsTrAPOpening );
    }


// ---------------------------------------------------------
// CImpsSendReceive2::ClosePDP
// ---------------------------------------------------------
//
void CImpsSendReceive2::ClosePDP()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: ClosePDP iTrState=%d" ), iTrState );
#endif

    if ( iTrState == EImpsTrInit )
        {
        iServer.TransportStatus( EInternal_NO_IAP );
        }

    if ( iTrState != EImpsTrDataOpen )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: ClosePDP error ***" ) );
#endif
        return;
        }

    DeleteAllIds();

    if ( iSender )
        {
        // In error cases iSender may be null
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: ClosePDP CLOSE DATA CHANNEL" ) );
#endif
        iSender->Close( );
        iSender->Destroy();
        iSender = NULL;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: ClosePDP START TO CLOSE AP" ) );
#endif
    // This causes APStatusEvent call finally.
    NewTrState( EImpsTrClosingPDP );
    iConnMan->CloseAP( EFalse );
    }

// ---------------------------------------------------------
//CImpsSendReceive2::OpenPDPL
// ---------------------------------------------------------
//
void CImpsSendReceive2::OpenPDPL(
    const TDesC& aSAP,
    TInt  aIAP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: OpenPDPL iTrState=%d" ), iTrState );
#endif

    __ASSERT_DEBUG( iConnMan,
                    User::Panic( KImpsPanicCategory, EImpsCorrupted ) );

    // Prevent opening twice data channel or connection
    if ( iTrState == EImpsTrAPOpening )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: OpenPDPL Leaves(corrupted) ***" ) );
#endif
        User::Leave( EImpsCorrupted );
        }

    delete iSAP;
    iSAP = NULL;
    iSAP = aSAP.AllocL();

    iIAP = aIAP;

    if ( iTrState == EImpsTrClosedPDP || iTrState == EImpsTrInit )
        {
        iConnMan->OpenAPL( aIAP, EFalse );
        NewTrState( EImpsTrOpeningPDP );
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: OpenPDPL Leaves(corrupted) ****" ) );
#endif
        User::Leave( EImpsCorrupted );
        }
    }

// ---------------------------------------------------------
// CImpsSendReceive2::Cancel
// ---------------------------------------------------------
//
void CImpsSendReceive2::Cancel()
    {
    if ( iSender )
        {
        iSender->CancelAll();
        }
    }

// ---------------------------------------------------------
// CImpsSendReceive2::SendAndWaitL
// ---------------------------------------------------------
//
void CImpsSendReceive2::SendAndWaitL(
    const TDesC& aTID,
    TInt aExpiryTime )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: SendAndWait begins %d" ), iTrState );
#endif

    if ( !iSender || iTrState != EImpsTrDataOpen )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: SendAndWaitL Leaves(KErrNotReady) ****" ) );
#endif
        User::Leave( KErrNotReady );
        }

    // XML log ***************************************************************
#ifdef _DEBUG
    if ( iXmlLog&0x1 )
        {
        if ( iXmlLog&0x2 || aTID.Length() > 0 ) // This works with WBXML too
            {
            TFileName fileName = _L( "c:\\logs\\impss\\" );
            if ( BaflUtils::PathExists( iFs, fileName ) )
                {
                _LIT( KRelated, "Sent.xml" );
                iParse.Set( fileName, &KRelated, NULL );
                fileName = iParse.FullName();
                TRAPD( errxy, CApaApplication::GenerateFileName( iFs, fileName ) );
                if ( errxy )
                    {
#ifndef _NO_IMPS_LOGGING_
                    CImpsClientLogger::Log( _L( "SendReceive2: SendAndWaitL Leaves(%d) ****" ), errxy );
#endif
                    User::Leave( errxy );
                    }

                RFile file;
                TInt xmlError = file.Create( iFs, fileName, EFileWrite | EFileShareExclusive );
                if ( xmlError == KErrNone )
                    {
                    // the data is supposed to be in the encode buffer
                    TPtr8 ptr = iOutputBuffer.Ptr( 0 );
                    file.Write( ptr );
                    file.Flush();
                    }

                // done - close files
                file.Close();

#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "SendReceive2: LogFile = %S" ), &fileName );
#endif
                }
            }
        }
#endif // _DEBUG        
    // XML log ends **********************************************************

    TInt myId = AddIdL( aTID );

#ifdef _FAKE_RESPONSE
    // local transport does not support more than one request at a time
    if ( iSender->PendingRequests() >= KImpsMaxPending )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: SendAndWaitL Leaves(KErrInUse) ****" ) );
#endif
        User::Leave( KErrInUse );
        }
#endif


#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: Call transport SendL" ) );
#endif

    TRAPD( er, iSender->SendL( myId, iOutputBuffer.Ptr( 0 ), aExpiryTime ) );
    if ( er )
        {
        // If send fails immediatly the callback is not called and thus
        // queue must be cleaned now.
        DeleteId( myId );
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: SendAndWaitL Leaves(%d) ****" ), er );
#endif
        User::Leave( er );
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: SendAndWait ends" ) );  //lint !e539
#endif

    }

// ---------------------------------------------------------
// CImpsSendReceive2::CancelTrans
// ---------------------------------------------------------
//
void CImpsSendReceive2::CancelTrans( const TDesC& aTID )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: CancelTrans" ) );  //lint !e539
#endif
    TInt opid = TidToOpid( aTID );
    if ( opid != KImpsNullId )
        {
        if ( iSender )
            {
            iSender->CancelTransaction( opid );
            }
        DeleteId( opid );
        }
    }

// ---------------------------------------------------------
// CImpsSendReceive2::CancelLoginTrans
// ---------------------------------------------------------
//
TBool CImpsSendReceive2::CancelLoginTrans( const TDesC& aTID )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: CancelLoginTransL TID=%S" ), &aTID );  //lint !e539
#endif
    TBool ret( KErrNone );
    if ( iTrState == EImpsTrAPOpening || iTrState == EImpsTrAPOpen )
        {
        CloseTr();
        }
    else if ( iTrState == EImpsTrDataOpen )
        {
        TInt opid = TidToOpid( aTID );
        if ( opid != KImpsNullId )
            {
            if ( iSender )
                {
                iSender->CancelTransaction( opid );
                }
            DeleteId( opid );
            ret = ETrue;
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CImpsSendReceive2::AddIdL
// ---------------------------------------------------------
//
TInt CImpsSendReceive2::AddIdL( const TDesC& aTid )
    {
    GenerateId();

    CTrReq* requ = new ( ELeave ) CTrReq(
        aTid,
        iID );

    iRequList.AddLast( *requ );

    return iID;
    }

// ---------------------------------------------------------
// CImpsSendReceive2::OpidToTid
// ---------------------------------------------------------
//
TPtrC CImpsSendReceive2::OpidToTid( TInt aId )
    {
    TDblQueIter<CTrReq> rIter( iRequList );

    rIter.SetToFirst();

    while ( rIter )
        {
        CTrReq* requ = rIter;
        if ( requ->iOpId == aId )
            {
            return requ->iTID;
            }
        rIter++;
        }
    return TPtrC();
    }

// ---------------------------------------------------------
// CImpsSendReceive2::TidToOpid
// ---------------------------------------------------------
//
TInt CImpsSendReceive2::TidToOpid( const TDesC& aId )
    {
    TDblQueIter<CTrReq> rIter( iRequList );

    rIter.SetToFirst();

    while ( rIter )
        {
        CTrReq* requ = rIter;
        if ( !requ->iTID.Compare( aId ) )
            {
            return requ->iOpId;
            }
        rIter++;
        }
    return KImpsNullId;
    }

// ---------------------------------------------------------
// CImpsSendReceive2::DeleteId
// ---------------------------------------------------------
//
TBool CImpsSendReceive2::DeleteId( TInt aId )
    {

    TDblQueIter<CTrReq> rIter( iRequList );

    rIter.SetToFirst();

    while ( rIter )
        {
        CTrReq* requ = rIter;
        rIter++;
        if ( requ->iOpId == aId )
            {
            requ->Destroy();
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsSendReceive2::DeleteAllIds
// ---------------------------------------------------------
//
void CImpsSendReceive2::DeleteAllIds()
    {
    // Delete all buffered requests from this client,
    TDblQueIter<CTrReq> rIter( iRequList );

    rIter.SetToFirst();

    while ( rIter )
        {
        CTrReq* requ = rIter;
        rIter++;
        requ->Destroy();
        }
    }

// ---------------------------------------------------------
// CImpsSendReceive2::TransportResponse
// ---------------------------------------------------------
//
void CImpsSendReceive2::TransportResponse(
    TInt aID,
    TInt aError,
    TInt aHttpStatus,
    HBufC8* aCspMessage )
    {
    TInt myErr = KErrNone;
    HBufC8** dataPtr = &aCspMessage;
    TRAP( myErr, DoTransportResponseL( aID, aError, aHttpStatus, dataPtr ) );
    if ( myErr )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: DoTransportResponseL leaves = %d" ), myErr ); //lint !e725
#endif
        DeleteId( aID );
        }
    // This ensures the data buffer deletion in error cases
    delete *dataPtr;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: TransportResponse ends" ), myErr );
#endif

    }

// ---------------------------------------------------------
// CImpsSendReceive2::DoTransportResponseL
// ---------------------------------------------------------
//
void CImpsSendReceive2::DoTransportResponseL(
    TInt aID,
    TInt aError,
    TInt aHttpStatus,
    HBufC8** aCspPtr )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: DoTransportResponse start err=%d http=%d" ), aError, aHttpStatus );
#endif

    HBufC8* cspMsg = *aCspPtr;

    TPtrC tid = OpidToTid( aID );
    TInt origState = iTrState;
    // error handling
    if ( aError || ( aHttpStatus != 200 && aHttpStatus != 0 ) )
        {
        if ( aHttpStatus != 200 && aHttpStatus != 0 )
            {
            aError = KImpsErrorResponseStatus;
            }
        iServer.TransportErrorL( tid, aError );
        DeleteId( aID );
        return;
        }
    if ( cspMsg && cspMsg->Size() > iServer.MaxParserSize() )
        {
        iServer.TransportErrorL( tid, KImpsErrorMessageTooBig );
        DeleteId( aID );
        return;
        }

    if ( origState == EImpsTrDataOpen )
        {

        // XML log ***************************************************************
#ifdef _DEBUG
        // XML log
        if ( iXmlLog&1 && cspMsg )
            {
            TPtr8 ptr = cspMsg->Des();
            if ( !( !( iXmlLog&2 ) && !ptr.Length( ) ) )
                {
                TFileName fileName = _L( "c:\\logs\\impss\\" );
                if ( BaflUtils::PathExists( iFs, fileName ) )
                    {
                    _LIT( KRelated, "Receive.xml" );
                    iParse.Set( fileName, &KRelated, NULL );
                    fileName = iParse.FullName();
                    User::LeaveIfError( CApaApplication::GenerateFileName( iFs, fileName ) );

                    RFile file;
                    TInt xmlError = file.Create( iFs, fileName, EFileWrite | EFileShareExclusive );
                    if ( xmlError == KErrNone )
                        {
                        TPtr8 ptr2 = cspMsg->Des();
                        file.Write( ptr2 );
                        file.Flush();
                        }

                    // done - close files
                    file.Close();

#ifndef _NO_IMPS_LOGGING_
                    CImpsClientLogger::Log( _L( "SendReceive2: LogFile = %S" ), &fileName );
#endif
                    }
                }
            }// if ( iXmlLog&1 && cspMsg )
#endif // _DEBUG
        // XML log ends **********************************************************

        // we catch error and continue ...
        TRAPD( erxx, iServer.TransportResponseL( aCspPtr ) );
        if ( erxx )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "SendReceive2:TransportResponseL ERROR= %d" ), erxx );
#endif
            }

        // This makes csp session ready for next response xxx
        iServer.GetNextMessageL();
        }

    DeleteId( aID );

    return;
    }

// ---------------------------------------------------------
// CImpsSendReceive2::APStatusEvent
// This methods handles both completion of Connection Manager
// operations and connection status events.
// ---------------------------------------------------------
//
void CImpsSendReceive2::APStatusEvent(
    TImpsConnectionState aConnectionState,
    TInt aStatus,
    TBool aOperation,
    TBool aAuthClose )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: APStatusEvent %d %d op=%d trstate=%d authClose=%d" ),
                            ( TInt )aConnectionState, aStatus, aOperation, iTrState, aAuthClose );
#endif

    TInt errx = KErrNone;
    if ( aOperation )
        {
        TRAP( errx, DoHandleRespL( aConnectionState, aStatus ) );
        }
    else    // !oper
        {
        TRAP( errx, DoHandleTrEventL( aConnectionState, aAuthClose ) );
        }
    }

// ---------------------------------------------------------
// CImpsSendReceive2::DoHandleRespL
// ---------------------------------------------------------
//
void CImpsSendReceive2::DoHandleRespL(
    TImpsConnectionState aConnectionState,
    TInt aStatus )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: DoHandleRespL %d %d trstate=%d" ),
                            ( TInt )aConnectionState, aStatus, iTrState );
#endif
    TInt err = aStatus;
    TInt errx = KErrNone;
    TImpsTrState origTr = iTrState;

    // Regular cases handled here
    if ( !err )
        {
        switch ( origTr )
            {
            case EImpsTrAPOpening:
                NewTrState( EImpsTrAPOpen );
                // create iSender if needed
                errx = DoOpenDataCh();
                if ( errx )
                    {
                    TPtrC tid = OpidToTid( iOpenId );
                    iServer.TransportErrorL( tid, errx );
                    }
                else
                    {
                    NewTrState( EImpsTrDataOpen );
                    // This was response for open request!
                    // If iOpenId is zero then this was a special case just
                    // to open channels
                    if ( iOpenId )
                        {
                        // Now we can send the message
                        // iID is set in Open
                        TPtrC tid = OpidToTid( iOpenId );
                        TRAPD( errxy, SendAndWaitL( tid ) );
                        if ( errxy )
                            {
                            // notice: if this leaves then ON_LINE event is
                            // not sent to a CSP class.
                            iServer.TransportErrorL( tid, errxy );
                            }
                        }
                    // Send ON_LINE status after data channel is opened
                    iServer.TransportStatus( EInternal_ON_LINE );
                    } // else
                break;
            case EImpsTrClosing:
                iIAP = 0;
                NewTrState( EImpsTrInit );
                iServer.TransportStatus( EInternal_NO_IAP );
                break;
            case EImpsTrClosingPDP:
                iIAP = 0;
                NewTrState( EImpsTrClosedPDP );
                iServer.TransportStatus( EInternal_NO_IAP );
                break;
            case EImpsTrOpeningPDP:
                NewTrState( EImpsTrAPOpen );
                // create iSender if needed
                errx = DoOpenDataCh();
                if ( errx )
                    {
                    NewTrState( EImpsTrAPOpen );
                    iServer.TransportStatus( EInternal_OFF_LINE );
                    }
                else
                    {
                    NewTrState( EImpsTrDataOpen );
                    // Send ON_LINE status not until data channel is opened
                    iServer.TransportStatus( EInternal_IAP_OPEN );
                    }
                break;
            default:
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "SendReceive2: DoHandleRespL no action ****" ) );
#endif
                break;
            };
        }

    // Error handling
    else // (err)
        {
        // Connection command failed
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SendReceive2: DoHandleRespL error case" ) ); //lint !e725
#endif

        if ( origTr == EImpsTrAPOpening || origTr == EImpsTrClosing )
            {
            NewTrState( EImpsTrInit );
            HBufC8* nullPtr = NULL;
            TRAP( errx, DoTransportResponseL( iOpenId, err, 200, &nullPtr ) );
            DeleteId( iOpenId );
            iOpenId = 0;
            if ( origTr == EImpsTrClosing )
                {
                // If closing fails then send NO_IAP event to CSP session so that
                // it can start to delete itself.
                iServer.TransportStatus( EInternal_NO_IAP );
                }
            }
        else if ( origTr == EImpsTrOpeningPDP )
            {
            // This is error from OpenIAP, not data channel opening
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "SendReceive2: err: E2 ****" ) ); //lint !e725
#endif
            NewTrState( EImpsTrClosedPDP );
            DeleteId( iOpenId );
            iOpenId = 0;
            // KImpsErrorNoIAP error code has a special error handling
            // in CSPSession class.
            iServer.TransportErrorL( KNullDesC, KImpsErrorNoIAP );
            }
        else if ( origTr == EImpsTrClosingPDP )
            {
            // This is error from Close IAP, not data channel closing
            // This is handled like Ok case, so don't call transportError callback.
            iIAP = 0;
            NewTrState( EImpsTrClosedPDP );
            iServer.TransportStatus( EInternal_NO_IAP );
            }
        if ( aConnectionState == EImpsNoAP )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L
                                    ( "SendReceive2: APStatusEvent NO_IAP error ****" ) );
#endif
            }
        }
    }

// ---------------------------------------------------------
// CImpsSendReceive2::DoHandleTrEventL
// ---------------------------------------------------------
//
void CImpsSendReceive2::DoHandleTrEventL(
    TImpsConnectionState aConnectionState,
    TBool aAuthClose )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: DoHandleTrEventL %d trstate=%d" ),
                            ( TInt )aConnectionState, iTrState );
#endif

    EImpsInternalStatus myStatus = EInternal_NOT_LOGGED;
    switch ( aConnectionState )
        {
        case EImpsOffline:
            myStatus = EInternal_OFF_LINE;
            break;
        case EImpsNoAP:
            if ( iTrState == EImpsTrDataOpen ||
                 iTrState == EImpsTrOpeningPDP )
                {
                NewTrState( EImpsTrClosedPDP );
                iSender->Close();
                }
            else
                {
                NewTrState( EImpsTrInit );
                }
            // shut down routines
            iIAP = 0;
            myStatus = aAuthClose ? EInternal_NO_IAP_AUTH : EInternal_NO_IAP;
            break;
        default:
            myStatus = EInternal_ON_LINE;
            break;
        };
    iServer.TransportStatus( myStatus );
    }
// ---------------------------------------------------------
// CImpsSendReceive2::Destroy
// ---------------------------------------------------------
//
void CImpsSendReceive2::Destroy()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: Destroy" ) );
#endif
    NewTrState( EImpsTrInit );
    if ( iSender )
        {
        iSender->Destroy();
        iSender = NULL;
        }
    if ( iConnMan )
        {
        iConnMan->Destroy();
        delete iConnMan;
        iConnMan = NULL;
        }

    }

// ---------------------------------------------------------
// CImpsSendReceive2::ConnManagerHandleL
// ---------------------------------------------------------
//
MMsgConnManager& CImpsSendReceive2::ConnManagerHandleL()
    {
    return iConnMan->ManagerHandleL();
    }

// -----------------------------------------------------------------------------
// CImpsSendReceive2::NewTrState
// -----------------------------------------------------------------------------
void CImpsSendReceive2::NewTrState( TImpsTrState aNew )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SendReceive2: NewTrState old=%d new=%d" ),
                            iTrState, aNew );
#endif
    iTrState = aNew;
    }

// -----------------------------------------------------------------------------
// CImpsSendReceive2::DoOpenDataCh()
// This method does not change iTrState itself
// -----------------------------------------------------------------------------
TInt CImpsSendReceive2::DoOpenDataCh()
    {
    TInt errx = KErrNone;
    if ( ! iSender )
        {
        TRAP( errx,
              { MMsgConnManager& man = iConnMan->ManagerHandleL();
                iSender = NewImpsSenderL( *this, man, iMimeType );
              } );
        }
    if ( errx )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "SendReceive2: DataCh open ERROR *** %d" ), errx );
#endif
        return errx;
        }
    TRAP( errx, iSender->OpenL( SAP() ) );
    if ( errx )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "SendReceive2: DataCh open ERROR **** %d" ), errx );
#endif
        if ( errx != KErrAlreadyExists )
            {
            iSender->Close( );
            iSender->Destroy();
            iSender = NULL;
            return errx;
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTrReq::CTrReq
// -----------------------------------------------------------------------------

CTrReq::CTrReq(
    const TDesC& aTID,
    TInt aOpId ):
        iOpId( aOpId )
    {
    iTID = aTID;
    }

CTrReq::~CTrReq()
    {

    }

void CTrReq::Destroy()
    {
    iLink.Deque();
    delete this;
    }


//  End of File
