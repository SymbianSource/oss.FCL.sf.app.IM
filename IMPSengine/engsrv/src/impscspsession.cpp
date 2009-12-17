/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Session for IMPS.
*
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    <e32math.h>
#include    "impsserver.h"
#include    "impssession.h"
#include    "impssubsession.h"
#include    "impscspsession.h"
#include    "impsfields.h"
#include    "impsutils.h"
#include    "impserrors.h"
#include    "impstimer.h"
#include    "impssendreceive.h"
#include    "impsservices.h"
#include    "impscommonenums.h"
#include    "impssdatautils.h"
#include    "impsmessageinterpreterapi.h"
#include    "impsdataaccessor.h"
#include    "impsdigestbytes.h"
#include    "impsvariant.h"
#include    "impsalivemanager.h"
#include    "impsactivecirmonitor.h"
#include    "ImpsVariantAPI.h"
#include    "impstidqueue.h"
#include    "impsactivecirmonitor.h"

#ifdef LOCAL_IMPS
#ifndef _FAKE_RESPONSE
#define _FAKE_RESPONSE
#endif
#endif

#ifdef _FAKE_RESPONSE
#include    "impssrvtestutils.h"
#endif

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// CONSTANTS
// Adative polling policy time increment
const TInt KImpsPollIncrement = 10;

// ================= MEMBER FUNCTIONS =======================

CImpsCSPSession::CImpsCSPSession( CImpsServer& aServer,
                                  TImpsSrvSettings& aSettings,
                                  RFs& aFs,
                                  CImpsVariant& aVariant,
                                  TImpsCspVersion aVer ):
        iServer( aServer ),
        iCSPVersion( aVer ),
        iFs( aFs ),
        iVariant( aVariant ),
        iTransactionList( _FOFF( CTransaction, iLink ) ), //lint !e413
        iRequestList( _FOFF( CReq, iLink ) ), //lint !e413
        iCSPState( EImpsCSPIdle ),
        iPendingLogout( EFalse ),
        iAliveMgr( NULL ),
        iPollTime( KImpsPollTime ),
        iPollWasRequested( EFalse ),
        iUserId( NULL ),
        iIntStatus ( EInternal_NOT_LOGGED ),
        iMultiTrans( KImpsMultiTrans ),
        iNegoExpiry( ),
        iCirManager( NULL ),
        iLogoutTID( KNullDesC ),
        iConAllowed( ETrue ),
        iPendingAlive( EFalse ),
        iSAP( NULL ),
        iIAP( 0 ),
        iPollState( EImpsPollNone ),
        iPollInResume( 0 ),
        iPendingPDP( EImpsPDPPendNone ),    // PDP:
        iPendPDPLogout ( EFalse ), // PDP:
        iTidSapHistory( NULL ),
        iTidCliHistory( NULL ),
        iTcpCIRError( EFalse ),
        iCancelLogin ( EFalse )
    {
    iCSPSessionId = KNullDesC;
    iLogCID = KNullDesC;
    iTempTid = KNullDesC;
    iTempTid2 = KNullDesC;
    iMsg = EImpsMessageNone;
    iLoginPhase = 1;
    iKey1 = NULL;
    iKey2 = NULL;
    iSettings = aSettings;
    }

// Create and start a new server.

CImpsCSPSession* CImpsCSPSession::NewL( CImpsServer& aServer,
                                        TImpsSrvSettings& aSettings,
                                        RFs& aFs,
                                        CImpsVariant& aVariant,
                                        TImpsCspVersion aVer )
    {
#ifndef _NO_IMPS_LOGGING_
    if ( aVer == EImpsCspVersion11 )
        {
        CImpsClientLogger::Log( _L( "CSPSession: NewL rel200542.2+ WV 1.1" ) );
        }
    else
        {
        CImpsClientLogger::Log( _L( "CSPSession: NewL rel200542.2+ WV 1.2" ) );
        }
#endif

    CImpsCSPSession* self = new ( ELeave ) CImpsCSPSession( aServer,
                                                            aSettings, aFs, aVariant, aVer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CImpsCSPSession::~CImpsCSPSession()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DESTRUCTOR csp=%d **" ), ( TInt )this );
#endif
    // deallocate all memory
    DeleteTransactions();
    DeleteRequests();

    delete iTransportOut;

    delete iCirManager;
    delete iReceiver2;
    delete iAllMessages;
    delete iRcv;
    delete iSnd;

    delete iIdleTimer;
    delete iAliveMgr;

    delete iSendQ;

    delete iTCPAddr;
    delete iLogPwd;
    delete iLogTid;
    delete iKey1;
    delete iKey2;
    delete iUserId;

    delete iSAP;

    if ( iDecoder )
        {
        iDecoder->Destroy();
        }
    if ( iEncoder )
        {
        iEncoder->Destroy();
        }

    delete iPDPIdleTimer;
    delete iPDPOpenTimer;
    delete iTidSapHistory;
    delete iTidCliHistory;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::Destroy()
// -----------------------------------------------------------------------------
void CImpsCSPSession::Destroy()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: Destroy csp=%d" ), ( TInt )this );
#endif
    // You MUST delete ip-cir entity before conn-manager entity.
    // refer to ImpsIpCirWatcherApi.h.
    delete iCirManager;
    iCirManager = NULL;
    iReceiver2->Destroy();
    delete iReceiver2;
    iReceiver2 = NULL;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::ConstructL()
// -----------------------------------------------------------------------------
void CImpsCSPSession::ConstructL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: ConstructL begins csp=%d **" ), ( TInt )this );
#endif

    iTransportOut = CBufFlat::NewL( KImpsStreamSize / 2 );
    iTransportOut->ResizeL( KImpsStreamSize );

    iDecoder = NewDecoderL( this );
    iEncoder = NewEncoderL();

    iTidSapHistory = CImpsTidQueue::NewL();
    iTidCliHistory = CImpsTidQueue::NewL();

    iSnd = CImpsFields::NewL();
    iRcv = CImpsFields::NewL();
    iIdleTimer = new ( ELeave ) CImpsIdleTimer( *this, CActive::EPriorityStandard );
    iPDPIdleTimer = new ( ELeave ) CImpsPDPIdleTimer( *this, CActive::EPriorityStandard );
    iPDPOpenTimer = new ( ELeave ) CImpsPDPOpenTimer( *this, CActive::EPriorityStandard );
    iAliveMgr = CImpsAliveManager::NewL( *this );

    iSendQ = new ( ELeave ) CImpsSendQueued( * this, CActive::EPriorityStandard );

    // Just initialize undefined address
    iTCPAddr = HBufC::NewL( 1 );
    *iTCPAddr = KNullDesC;

    TPtrC8 myMime = iEncoder->MimeType();
    iReceiver2 = CImpsSendReceive2::NewL(
                     iFs, *this, *iTransportOut, myMime );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession:ConstructL ends" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::CloseAll()
// CImpsServer::CloseEngine calls this. TransportStatus handles the callback.
// -----------------------------------------------------------------------------
void CImpsCSPSession::CloseAll( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CloseAll %d csp=%d" ), ( TInt )this );
#endif

    NewState( EImpsCSPShuttingDown );

    if ( iIdleTimer )
        {
        iIdleTimer->Stop( );
        }
    iPollState = EImpsPollNone;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
#endif

    if ( iAliveMgr )
        {
        iAliveMgr->StopTimer( );
        }

    if ( iSendQ )
        {
        iSendQ->Cancel( );
        }

    if ( iCirManager )
        {
        iCirManager->CloseCh( 0xF );
        delete iCirManager;
        iCirManager = NULL;
        }

    // Close the transport
    if ( iReceiver2 )
        {
        iReceiver2->CloseTr( );
        }
    // TransportStatus handles rest of shut down activities
    }


// -----------------------------------------------------------------------------
// CImpsCSPSession::ClosePDP
// called by - CImpsPDPIdleTimer
// ClosePDP method needed to close PDP-context.
// -----------------------------------------------------------------------------
void CImpsCSPSession::ClosePDP()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: ClosePDP" ) );
#endif

    if ( iPollState != EImpsPollCIR )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: ClosePDP IGNORED iPollState = %d****" ),
                                iPollState );
#endif
        return;
        }
    if ( IsPDPIdle() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: ClosePDP PDP already IDLE ****" ) );
#endif
        if ( iCSPState == EImpsCSPOnLineIdleEnd )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: ClosePDP -> EImpsPDPPendClose" ) );
#endif
            // set pending close request if PDP is opening
            iPendingPDP = EImpsPDPPendClose;
            }
        return;
        }

    NewState( EImpsCSPOnLineIdleStart );

    // This does not close WAP-SMS-CIR
    if ( iCirManager )
        {
        iCirManager->CloseCh( 0xF );
        delete iCirManager;
        iCirManager = NULL;
        }

    // Close the transport
    if ( iReceiver2 )
        {
        iReceiver2->ClosePDP( );
        }
    // TransportStatus handles rest of shut down activities
    }


// -----------------------------------------------------------------------------
// CImpsCSPSession::OpenPDP
// Called by LogoutL, SendAliveL, SendDataL, TransportStatus, SendAllQueued
// Called by CIRMessageL directly and by the CImpsPDPTimer class too.
// OpenPDP method needed to re-open PDP-context
// -----------------------------------------------------------------------------
TInt CImpsCSPSession::OpenPDP()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: OpenPDP" ) );
#endif
    if ( !IsPDPIdle() )
        {
        // Nothing to do
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: OpenPDP IGNORED - not idle" ) );
#endif
        return KErrAlreadyExists;
        }
    else if ( iCSPState == EImpsCSPOnLineIdleEnd )
        {
        // already opening
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: OpenPDP IGNORED - EImpsCSPOnLineIdleEnd" ) );
#endif
        return KErrNotReady;
        }
    else if ( iCSPState == EImpsCSPOnLineIdleStart )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: OpenPDP -> EImpsPDPPendOpen" ) );
#endif
        iPendingPDP = EImpsPDPPendOpen;
        return KErrNone;
        }

    NewState( EImpsCSPOnLineIdleEnd );
    TRAPD( errx, iReceiver2->OpenPDPL( SAP(), iIAP ) );
    if ( errx )
        {
        NewState( EImpsCSPOnLineIdle );
        // Does not matter which error code is returned. Used in Logout only
        // to start internal logout routines.
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: err: openpdp E1 ****" ) );
#endif
        return KErrGeneral;
        }
    return KErrNone;
    // TransportStatus/transportError handles rest of open activities
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::LoginL
// -----------------------------------------------------------------------------
TPtrC CImpsCSPSession::LoginL(
    const TDesC& aUser,
    const TDesC& aPassword,
    const TDesC& aClientId,
    const TDesC& aSAP,
    TUint32 aAP,
    const TDesC& aKey1,
    const TDesC& aKey2,
    TTime aLoginExpiry,
    TBool aReactive )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: LoginL state=%d csp=%d" ), iCSPState, ( TInt )this );
#endif

    // AOL Login
    if ( aKey1.Length() && aKey2.Length() )
        {
        // this should be the case anyway
        iSettings.iFourWayLogin = ETrue;
        delete iKey1;
        iKey1 = NULL;
        iKey1 = aKey1.AllocL();
        delete iKey2;
        iKey2 = NULL;
        iKey2 = aKey2.AllocL();
        }
    TPtrC tid ( KNullDesC );

    delete iSAP;
    iSAP = NULL;
    iSAP = aSAP.AllocL();
    iIAP = aAP;

    iPendingAlive = EFalse;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: iPendingAlive->F" ) );
#endif

    if ( iCSPState >= EImpsCSPLogged )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: err: Login when logged ***" ) );
#endif
        User::Leave( KImpsErrorAlreadyLogged );
        }
    else if ( iCSPState == EImpsCSPDisconnecting )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: err: Login when CSP logging out ***" ) );
#endif
        User::Leave(  KErrNotReady );
        }
    // Check that no active CSP session exists
    // This should not happen before login
    else if ( iReceiver2->NbrOfPending() >= KImpsMaxPending ||
              iCSPState == EImpsCSPLogging )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: err: Login when busy ***" ) );
#endif
        User::Leave( KErrNotReady );
        }
    // Close old connections if any in re-login case (wrong psw first time e.g.)
    // The pending logout response is handled above, not here.
    else if ( iReceiver2->isConnected() )
        {
        // These are for re-login without logout after an error response
        NewState( EImpsCSPIdle );
        iPollState = EImpsPollNone;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
        CImpsClientLogger::Log( _L( "CSPSession: err: transport was connected ***" ) );
#endif
        iAliveMgr->StopTimer();
        iSendQ->Cancel();
        iIntStatus = EInternal_NOT_LOGGED;
        iReceiver2->CloseTr();
        User::Leave( KErrNotReady );
        }

    // aPassword cannot be NULL
    delete iLogPwd;
    iLogPwd = NULL;
    delete iUserId;
    iUserId = NULL;
    iLogPwd = aPassword.AllocL();
    iUserId = aUser.AllocL();
    iLogCID = aClientId;
    iLogoutTID = KNullDesC;
    iNegoExpiry = aLoginExpiry;
    iReactive = aReactive;
    TImpsSrvUtils::InitializeServices( iServices, iReactive );

    // Make a login request
    iSnd->Reset();
    tid.Set( GenerateTid() );
    iSnd->SetTidL( tid );
    iSnd->SetMessageType( EImpsLoginReq );
    // Session type is OUTBAND
    iSnd->SetSessionTypeL( EImpsOutband );

    TImpsDataAccessor myAccess( iSnd );
    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey

    iAliveTime = iSettings.iAliveTime;

    if ( iSettings.iFourWayLogin == EFalse )
        {
        TImpsSDataUtils::CreateLoginReqL(
            myKey,
            &myAccess,
            aUser,
            iLogCID,
            LogPwd(),
            GenerateCookie(),
            iAliveTime );
        // set this, that response goes to the right place
        iLoginPhase = 2;
        }
    else
        {
        TImpsDigestSchema aSchema = EImpsMD5;
        TImpsSDataUtils::CreateLoginReqPhaseOneL(
            myKey,
            &myAccess,
            aUser,
            iLogCID,
            aSchema,
            GenerateCookie(),
            iAliveTime );
        // just to be sure
        iLoginPhase = 1;
        }

    // Add TImpsTransactionMode now
    myKey->Reset();
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransModeElements,
        sizeof( KTransModeElements ) /
        sizeof( KTransModeElements[0] ) );
    myAccess.StoreIntegerL( myKey, EImpsRequest );
    CleanupStack::PopAndDestroy( 1 );   // >>> myKey

    // Save login id for error handling purposes
    iTempTid2 = tid;

    // Set WV version
    iSnd->SetCspVersion( iCSPVersion );

    // Encode login request
    iEncoder->EncodeMessageL( myAccess, *iTransportOut );
    iLastSent = EImpsLoginReq;

    delete iLogTid;
    iLogTid = NULL;
    iLogTid = tid.AllocL();

    // This does not leave immediately if network is not available
    NewState( EImpsCSPLogging );
    TRAPD( errx, iReceiver2->OpenL( tid, iSettings.iXmlLog, aSAP, aAP ) );
    if ( errx )
        {
        // In rare situations the previous call may leave
        NewState( EImpsCSPIdle );
        User::Leave( errx );
        }

    // iReceiver callback handles the rest of the login method.
    // This handles interrupted logout request as well as shut down interrupt

    iMsg = EImpsMessageNone;

    return tid;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::LogoutL
// Notice that subsession class prevents calling this in certain
// situations, if not iLogged or not pending login operation.
// Therefore shut down situation is not handled here.
// -----------------------------------------------------------------------------
TPtrC CImpsCSPSession::LogoutL( TBool aCancel )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: LogoutL state=%d cancel=%d csp=%d" ), iCSPState,
                            aCancel, ( TInt )this );
#endif

    if ( iCSPState == EImpsCSPIdle )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: LogoutL LEAVES KImpsErrorNotLogged" ) );
#endif
        iPendingPDP = EImpsPDPPendNone;
        iPendPDPLogout = EFalse;
        User::Leave( KImpsErrorNotLogged );
        }

    if ( iCancelLogin && aCancel )
        {
        // cancel login already requested!
        return TPtrC();
        }

    // Make a logout request (encode)
    TPtrC tid;
    tid.Set( GenerateTid() );
    iLogoutTID = tid;
    // Use iLogoutTID when loggin out later after PDP-open
    // Check if PDP is closed and start to re-open it here (Logout)
    if ( IsPDPIdle() )
        {
        // check this flag when PDP has been re-opened
        iPendPDPLogout = ETrue;
        if ( iCSPState == EImpsCSPOnLineIdle )
            {
            // E1: error handling; return boolean to speed up error handling
            // E2: fix:
            if ( OpenPDP() )
                {
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: err: Ex2 OpenPDP FAILS ****" ) );
#endif
                DoLogout();
                }
            return tid;
            }
        }

    if ( iCSPState != EImpsCSPLogging )
        {
        // Clear buffers from old stuff
        DeleteRequests();
        DeleteTransactions();
        // Send Logout primitive and set state to EImpsCSPDisconnecting.
        TRAPD( errx, DoSendLogoutL( tid ) );
        if ( errx )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: DoSendLogoutL error %d ***" ), errx );
#endif
            // Do logout routines immediately if transport gives an error
            DoLogout();
            // The following error code is handled in a special way in
            // client session classes.
            User::Leave( KImpsErrorTerminalOffLine );
            }
        }
    else // LoggingIn
        {
        if ( !aCancel )
            {
            iPendingLogout = ETrue;
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iPendingLogout->TRUE ***" ) );
#endif
            }
        else
            {
            iCancelLogin = ETrue;
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iCancelLogin->TRUE ***" ) );
#endif
            }
        // Logout must not ever fail so this must not leave.
        StartLoginCancel( );
        }

    return tid;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::CirMessageL
// -----------------------------------------------------------------------------
void CImpsCSPSession::CirMessageL(
    const TDesC8& aCookie )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CirMessageL csp=%d" ), ( TInt )this );
#endif

    // if CIR is not negotiated then ignore
    if ( ( iSettings.iUDPWAPCIR != 2 && iSettings.iSMSWAPCIR != 2 &&
           iSettings.iTCPSCIR != 2 && iSettings.iUDPSCIR != 2 ) ||
         !IsNegotiated() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: CirMessage IGNORED" ) );
#endif
        return;
        }

    // check session cookie but not CSP-version.
    // CIR syntax: WVCI <CSP-version> <Session-cookie>

    TPtrC8 cookie = aCookie.Left( 4 );
    if ( cookie.CompareF( _L8( "WVCI" ) ) )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: CirMessage SYNTAX ERROR" ) );
#endif
        User::Leave( KErrArgument );
        }

    cookie.Set( aCookie );
    // Find if the cookie exists in this push body
    if ( cookie.Find( iCookie8 ) == KErrNotFound )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: CirMessage COOKIE ERROR" ) );
#endif
        User::Leave( KImpsErrorUnauthorized );
        }

    // reset PDP idle timer when CIR received
    iPDPIdleTimer->Reset();

    // If PDP-context is idle or shutting down then set pending open request.
    if ( IsPDPIdle() )
        {
        if ( iCSPState == EImpsCSPOnLineIdle ||
             iCSPState == EImpsCSPOnLineIdleStart )
            {
            iPendingPDP = EImpsPDPPendOpen;
            }
        if ( !iPollInResume )
            {
            iPollInResume = 1;
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iPollInResume %d" ),
                                    iPollInResume );
#endif
            }
        // This is a fix for cases when GPRS bearer event has not been received
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: CIR -> openPDP ****" ) );
#endif
        OpenPDP();
        iPDPOpenTimer->Start( KImpsPDPRetryOpenTime );
        }

    // Send the actual Poll message
    else if ( iIntStatus == EInternal_ON_LINE )  // && !PDPIdle
        {
        TRAPD( errPoll, SendPollL() );
        if ( errPoll && !iPollInResume )
            {
            iPollInResume = 1;
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iPollInResume %d" ), iPollInResume );
#endif
            }
        }
    else  // NOT ON_LINE && !PDPIdle
        {
        // WAP SMS CIR causes OFF-LINE and therefore has to wait ON-LINE
        iPollState = EImpsPollPending;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
#endif
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::SendDataL
// -----------------------------------------------------------------------------
TPtrC CImpsCSPSession::SendDataL(
    CImpsFields* aFields, TInt aExpiry, TBool& aOwnerCh )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: SendDataL begins csp=%d" ), ( TInt )this );
#endif

    // reset PDP idle timer
    iPDPIdleTimer->Reset();

    aOwnerCh = EFalse;

    // generate TID
    CImpsFields* fields = aFields;
    TPtrC tid;
    tid.Set( GenerateTid() );
    fields->SetTidL( tid );
    // Insert CSP Session-id
    fields->SetSessionIdL( iCSPSessionId );
    // Session-type
    fields->SetSessionTypeL( EImpsInband );

    TImpsDataAccessor myAccess( fields );
    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey

    // Add TImpsTransactionMode now
    myKey->Reset();
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransModeElements,
        sizeof( KTransModeElements ) /
        sizeof( KTransModeElements[0] ) );
    myAccess.StoreIntegerL( myKey, EImpsRequest );

    // API supports deliver method and thus do not overwrite it here anymore

    CleanupStack::PopAndDestroy( 1 );   // >>> myKey

    // We have to check if queued messages exist
    TBool queuedMsg = EFalse;
    queuedMsg = RequestExist();

    // if PDP-idle then queue request + start to open PDP
    TInt nbrPend = iReceiver2->NbrOfPending();
    if ( nbrPend >= KImpsMaxPending ||
         iIntStatus != EInternal_ON_LINE ||
         iTr >= iMultiTrans ||
         queuedMsg ||
         IsPDPIdle() )
        {
        // Transport adapter is busy so queue the request
        QueueClientRequestL( fields, aExpiry );
        aOwnerCh = ETrue;
        if ( iCSPState == EImpsCSPOnLineIdle )
            {
            // Try to open PDP only once, don't start open PDP timer.
            // The request will expiry if this fails anyway and it's
            // a task of application to retry sending of this message.
            OpenPDP( );
            }
        else if ( iCSPState == EImpsCSPOnLineIdleStart ||
                  iCSPState == EImpsCSPOnLineIdle )
            {
            iPendingPDP = EImpsPDPPendOpen;

            /* Notice:
             Thus CImpsConnManager DLL
             does not give bearer event for GPRS resume in all situations
             it is better to try to open the connection always.
            */
            OpenPDP();
            }

        else if ( queuedMsg )
            {
            // This must not leave because of the error is not message specific
            SendAllQueued();
            }
        }
    else
        {
        // Message sending should be possible without PDP open
        DoSendSingleL(
            &myAccess, tid, ( TImpsMessageType ) fields->MessageType() );
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: SendData ends" ) );
#endif

    return tid;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::TransportResponseL
// -----------------------------------------------------------------------------
void CImpsCSPSession::TransportResponseL( HBufC8** aDataPtr )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: TransportResponseL csp=%d" ), ( TInt )this );
#endif

    HBufC8* rawData = *aDataPtr;

    // reset PDP idle timer
    iPDPIdleTimer->Reset();

    SendAllQueued();

    TInt myError = KErrNone;
    // Handle pending logout here
    if ( iPendingLogout || iCancelLogin )
        {
        DoPendingLogout();
        return;
        }

    if ( rawData == NULL || rawData->Des().Length() == 0 )
        {
        // SAP did not send anything in the response
        iLastReceived = 0;
        // Increase adaptive polling time if necessary
        if ( iLastSent == EImpsPolling )
            {
            IncreasePollTime();
            }
        return;
        }

    if ( iLastSent != EImpsKeepAliveReq )
        {
        // Reset poll timer
        ResetPollTime();
        }

    if ( iRcv )
        {
        iRcv->Reset();
        }
    else
        {
        // QueueTidL may have leaved and this is NULL
        iRcv = CImpsFields::NewL();
        }

    CImpsDataAccessor* myAccess = CImpsDataAccessor::NewLC( iRcv ); // <<<

    TInt retCode = KErrNone;

// ----------- NORMAL CASE -------------------------
#ifndef _FAKE_RESPONSE

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: Gonna to call Decode" ) );
#endif
    // Decode to iRcv
    iDecoder->DecodeMessageL( *myAccess, aDataPtr );
    retCode = iRcv->ResponseStatusL();
    // Decode errors cancel this method and error are
    // reported to client by expiry timers.

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: Decoded status = %d" ), iRcv->Status() );
    CImpsClientLogger::Log( _L( "CSPSession: Decoded ResponseStatus = %d" ),
                            retCode );
#endif

// ----------- NORMAL CASE ENDS -------------------------

// ----------- TEST CASE -------------------------
#endif  // ifndef _FAKE_RESPONSE

#ifdef _FAKE_RESPONSE
    TImpsSrvTestUtils tests( *this, iFs );
    tests.MakeFakeResponseL( myAccess, aDataPtr );
#endif
// ----------- TEST CASE ENDS -------------------------

    // Message successfully received from SAP
    // Session-id check
    if ( iRcv->SessionTypeL() == EImpsInband )
        {
        TPtrC sesId = iRcv->SessionIdL();
        // This is case sensitive
        if ( sesId.Length( ) > 0 && sesId.Compare( iCSPSessionId ) )
            {
            // Invalid session id
            User::Leave( KImpsErrorSID );
            }
        }

    // KeepAlive handling
    iAliveMgr->CheckResp( retCode );

    // ---------------------------------
    // check if multiTrans messages
    // ---------------------------------
    TInt trs = myAccess->NbrOfTransactionsL( );
    if ( trs < 1 )
        {
        // If no transaction content then it is handled as encode error
        myError = KImpsErrorDecode;
        }
    else if ( trs == 1 )
        {
        // regular case, one transaction content in a transport messages
        // Update internal data structure (CImpsFields)
#ifndef _FAKE_RESPONSE
        if ( !( myAccess->GetTransactionL( 0, NULL ) ) )
            {
            // internal error
            myError = KErrCorrupt;
            }
#endif
        }

    // ----------------------------------
    // checking multiTrans messages ends
    // ----------------------------------

    // If error then stop further progress of transport response
    if ( myError )
        {
        User::Leave( myError );
        }

    // Check if PollRequest was requested in incoming transport message
    if ( iRcv->PollL() )
        {
        iPollWasRequested = ETrue;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPollWasRequested->T in msg" ) );
#endif
        }
    else
        {
        iPollWasRequested = EFalse;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPollWasRequested->F in msg" ) );
#endif
        }

    if ( trs > 1 )
        {
        delete iAllMessages;
        iAllMessages = iRcv;
        iRcv = CImpsFields::NewL();
        }
    else
        {
        // paranoid about memory leaks
        delete iAllMessages;
        iAllMessages = NULL;
        }

    // ---------------------------------
    // Send each new message to sessions
    // ---------------------------------
    for ( TInt i = 0; i < trs; i ++ )
        {

        if ( trs == 1 )
            {
            // No need for split
            }
        else
            {
            // Data accessor points to the original iRcv (allMessages)
            iRcv->Reset();
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: MULTI-TRANS nbr=%d" ), i );
#endif
            if ( !( myAccess->GetTransactionL( i, iRcv ) ) )
                {
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: Error with MultiTrans" ) );
#endif
                // error, continue
                continue;
                }
            }

        iLastReceived = iRcv->MessageType();

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: TransportResponse msg = 0x%x" ),
                                iLastReceived );
#endif

        // Check if csp Session startup routines needed
        // TID is case sensitive
        if ( ( iCSPState == EImpsCSPLogging || iCSPState == EImpsCSPNegotiation ) &&
             ( iRcv->MessageType() == EImpsLoginRes ||
               iRcv->MessageType() == EImpsClientCapabilityRes ||
               iRcv->MessageType() == EImpsServiceRes ||
               ( iRcv->MessageType() == EImpsStatus &&
                 !iRcv->TidL().Compare( iTempTid2 ) ) ) )
            {
            SessionStartupL( iRcv );
            }
        // Sent events to all sub-sessions interested in this type of event
        // This also handles login-resposes and removes the login-req from
        // queues.
        if ( iRcv->MessageType() != 0 &&
             iRcv->MessageType() != EImpsClientCapabilityRes &&
             iRcv->MessageType() != EImpsServiceRes &&
             iLoginPhase != 2 )
            {
            RouteMessageL();
            }
        }  // for

    // CIR-error-flag check in WV 1.2.
    // WV 1.1 messages returns always EFalse.
    // Check this flag only if TCP/IP CIR channel is negotiated.
    if ( !iTcpCIRError && iSettings.TCPSCIR() == 2 && IsLogged() )
        {
        // Close and re-open TCP/IP CIR if CIR flag is on.
        TInt erx = KErrNone;
        TRAP( erx, iTcpCIRError = TImpsSDataUtils::GetCIRL( myAccess ) );
        // iTcpCIRError is reset in CirChOpened()
        if ( iTcpCIRError )
            {
            if ( iCirManager )
                {
                iCirManager->CloseCh( KImpsCirTcp );
                }
            TRAP( erx, DoStartIpCIRL() );
            }
        }

    CleanupStack::PopAndDestroy( 1 );   // >>> myAccess

    delete iAllMessages;
    iAllMessages = NULL;

    SendAllQueued();

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: TransportResponse ends" ) );
#endif

    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::TransportErrorL
// -----------------------------------------------------------------------------
void CImpsCSPSession::TransportErrorL(
    const TDesC& aTID,
    TInt aError )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: TransportError err=%d tid=%S csp=%d" ), aError, &aTID, ( TInt )this );
#endif

    TImpsCSPState origCSP = iCSPState;
    // reset PDP idle timer
    iPDPIdleTimer->Reset();

    TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );


    // Handle pending logout here
    // CSP destroyer is started finally
    if ( iPendingLogout || iCancelLogin )
        {
        NewState( EImpsCSPDisconnecting );
        DoPendingLogout();
        return;
        }

    // Do not let the PDP start up again if it has just shut down.
    if ( origCSP == EImpsCSPOnLineIdleEnd && aTID.Length() == 0 &&
         aError == KImpsErrorNoIAP )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: DON'T START UP PDP AGAIN ****" ) );
#endif
        if ( iPollState > EImpsPollAdaptive )
            {
            iPollState = EImpsPollCIR;
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
#endif
            }
        ResetPollTime();
        iPendingAlive = EFalse;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPendingAlive->F" ) );
#endif
        NewState( EImpsCSPOnLineIdle );

        // KeepAlive handling
        iAliveMgr->CheckError( aTID );

        // Discard all the current requests
        // We change the error code for UI.
        // First we have to check if there is Logout request so that
        // it is completed with OK status!
        if ( LogoutTID().Length() )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: Err: Discard logout ****" ) );
#endif
            // This is for normal logout response error, i.e. no pending logout nor login-cancel.
            iServer.DiscardLogout( LogoutTID(), csp );
            DoLogout();
            }
        iServer.DiscardRequests( EImpsEventAll, KImpsErrorBearerSuspended, csp, this );
        return;
        }

    // Let's call this to be sure that new messages are sent
    SendAllQueued();

    // Reset poll timer to resume from error case
    ResetPollTime();

    TInt myErr = KErrNone;

    if ( iRcv )
        {
        iRcv->Reset();
        }
    else
        {
        // QueueTidL may have leaved and this is NULL
        iRcv = CImpsFields::NewL();
        }

    // This ensures that login gets a response in 4-way login later phase
    if ( origCSP == EImpsCSPLogging && !LogTid().CompareF( aTID ) )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: Special LogTid ****" ) );
#endif
        // Route response to a requestion client and later below do logout.
        iRcv->SetTidL( LogTid() );
        }
    else
        {
        iRcv->SetTidL( aTID );
        }
    iRcv->SetMessageType( EImpsStatus );
    iRcv->SetStatus( aError );
    TRAP( myErr, RouteMessageL( ) );

    // KeepAlive handling
    iAliveMgr->CheckError( aTID );

    if ( origCSP == EImpsCSPLogging )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: err: Ex ****" ) );
#endif
        DoLogout();
        }
    // If RouteMessage has not done internal logout then let's do it here
    else if ( origCSP == EImpsCSPDisconnecting )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: err: E6 ****" ) );
#endif
        // Message type is used in subsession to prevent sending logout event twice
        iServer.DiscardLogout( iLogoutTID, csp );
        DoLogout();
        }

    return;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::RouteMessageL
// -----------------------------------------------------------------------------
void CImpsCSPSession::RouteMessageL(  )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: RouteMessageL begins csp=%d" ), ( TInt )this );
#endif

    TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );

    // *****************************************************
    // First check keep alive reponses
    // *****************************************************

    TInt32 resstatus = iRcv->ResponseStatusL();

    // KeepAlive time may be reset on fly
    if ( iRcv->MessageType() == EImpsKeepAliveRes )
        {
        if ( resstatus != KImpsStatusOk )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: RouteMessageL ends 1" ) );
#endif
            return;
            }
        // get new KeepAlive time if any
        TInt newTime = 0;
        TImpsDataAccessor myAc( iRcv );
        if ( TImpsSDataUtils::GetKeepAliveL( &myAc, newTime ) )
            {
            iAliveTime = newTime;
            iAliveMgr->StartTimer( iAliveTime );
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "Server: new AliveTime %d" ), iAliveTime );
#endif
            }
        // Keep alive response does not cause events to clients
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: RouteMessageL ends 2" ) );
#endif
        return;
        }

    // *************************************************************
    // Ignore informational messages from server status code 100-199
    // *************************************************************
    if ( resstatus >= 100 && resstatus < 200 )
        {
        iTr--;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: 100-200 iTr = %d" ), iTr );
        CImpsClientLogger::Log( _L( "CSPSession: RouteMessageL ends 3" ) );
#endif
        return;
        }

    // *************************************************************
    // Harmonize WV 1.1 and WV 1.2 logout responses
    // *************************************************************
    TImpsLogoutTrans logoutRsp = IsLogoutResp( iRcv );

    // *****************************************************
    // Handle regular request
    // *****************************************************

    // Check requests
    TBool someFound = EFalse;
    TBool notOrphan = EFalse;
    TInt error = 0;
    // The following sends logout reponse to a requesting client
    TRAP( error, iServer.CheckRequestsL ( iRcv, someFound, csp ) );

    if ( someFound )
        {
        // Message was response to our request, login phase has not
        // increased this counter, that's why if statement here
        TrMinus();
        }
    else if ( iRcv->MessageType() != EImpsDisconnect )
        {
        // Check that SAP initiated TID is not sent to clients already.
        if ( ValidateTid( ETrue ) )
            {
            // Notifications check.
            notOrphan = iServer.CheckNotifications( iRcv, csp );
            }
        }

    // Check error that means cancelled CSP session
    // notice: fullfill the list of serious erros
    if ( logoutRsp != EImpsLogoutNone  ||
         resstatus == 600 || resstatus == 601 || resstatus == 604 )
        {
        TInt myOpId = 0;
        if ( logoutRsp == EImpsLogoutSAP )
            {
            // Detect SAP initiated logout. disconnect primitive and
            // TID not matching to our own logout TID.
            myOpId = KImpsNullId;
            }
        // Both logout callback events and Status change events
        // are sent here in logout.
        DoLogoutNow( myOpId );
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: RouteMessageL ends 4" ) );
#endif
        return;
        }

    // No reason to do this:
    // if ( someFound ) notOrphan = ETrue;

    // If the message was not a response to our TID then it was SAP originated
    // Update pending transaction queue
    if ( ( iRcv->MessageType() != EImpsDisconnect ) &&
         ( iRcv->MessageType() != EImpsMessageNone ) &&
         ( iRcv->MessageType() != EImpsStatus ) &&
         ( !someFound )
       )
        {
        // Make SAP response to wait actions
        QueueTidL( !notOrphan );
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: RouteMessageL ends 5" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::GetNextMessageL
// Called by SendReceive2
// -----------------------------------------------------------------------------
void CImpsCSPSession::GetNextMessageL( )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: GetNextMessageL begins" ) );
#endif

    // Use this to generate response for SAP intiated transactions
    iSnd->Reset();

    // Rejected, nothing to send
    if ( !IsLogged() )
        {
        return;
        }

    // Send next buffered user request if any
    // search. Not immediately, but via active object scheduler
    SendAllQueued();

    iLastSent = 0;
    return;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::CheckExpiry
// -----------------------------------------------------------------------------
void CImpsCSPSession::CheckExpiry(
    TImpsEventType aType, const TTime aExpiry  )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CheckExpiry begins" ) );
#endif

    TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );

    // Login negotiation phase is a special case.
    // Access timer handles that.
    // Because of there may be messages not initiated by
    // API methods, server must check special time-out variable iNegoExpiry.
    // Internal iRequestList cannot be used for that purpose
    // because of requests are deleted after transport has
    // got them succesfully, but we are not satisfied until get response.

    if ( IsLogged() && !IsNegotiated() && iNegoExpiry < aExpiry )
        {
        // Expired negotiation in session startup negotiation
        TInt errorCode = KErrTimedOut;
        switch ( iMsg )
            {
            case EImpsClientCapabilityReq:
                errorCode = KImpsErrorCapabilities;
                break;
            case EImpsServiceReq:
                errorCode = KImpsErrorServices;
                break;
            default:
                break;
            }
        iServer.SendErrorEvent( EImpsEventServerLogin, errorCode, 0, csp );
        // goto to NOT_LOGGED
        DoLogout();
        }
    else if ( iCSPState == EImpsCSPDisconnecting && iNegoExpiry < aExpiry )
        {
        if ( iCancelLogin || iPendingLogout )
            {
            DoPendingLogout();
            }
        else
            {
            iServer.DiscardLogout( LogoutTID(), csp );
            // goto to NOT_LOGGED
            DoLogout();
            }
        }
    else if ( iCSPState == EImpsCSPLogging && iNegoExpiry < aExpiry )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: CheckExpiry in EImpsCSPLogging ***" ) );
#endif
        iServer.DiscardRequests( aType, aExpiry, csp, this );
        DoLogout();
        }
    else
        {
        // Regular mode.
        // Go through all sessions (each having an own request list)
        // The following ends up calling CancelTrans and there
        // we can check if login or logout is expired and change the
        // iCSPState.
        iServer.DiscardRequests( aType, aExpiry, csp, this );
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::SessionStartupL
// -----------------------------------------------------------------------------
void CImpsCSPSession::SessionStartupL( CImpsFields* aFields )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: SessionStartupL begins csp=%d" ), ( TInt )this );
#endif

    TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
    TInt errx = KErrNone;
    TrMinus();

    // check if error for 2-way login
    if ( ( !IsLogged() ) &&
         (
             aFields->MessageType() == EImpsLoginRes ||
             aFields->MessageType() == EImpsStatus ) &&
         aFields->ResponseStatusL() != KImpsStatusOk &&
         !iSettings.iFourWayLogin
       )
        {
        // RouteMessageL handles errors if iLoginPhase != 2
        // iLoginPhase starts from 2 in 2-way login
        // See LoginL method
        NewState( EImpsCSPIdle );
        iReceiver2->CloseTr();
        return;
        }

    // Login response, 4-way login phase one
    // *************************************
    if ( aFields->MessageType() == EImpsLoginRes &&
         aFields->Status() == KErrNone &&
         ( aFields->ResponseStatusL() == KImpsStatusOk ||
           aFields->ResponseStatusL() == 401 ) &&
         iLoginPhase == 1 )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: SessionStartupL 4-login phase one" ) );
#endif
        iLoginPhase++;
        TRAP( errx, HandleLoginResponseL( aFields ) );
        if ( errx )
            {
            iServer.DiscardRequest( LogTid(), EImpsEventServerLogin, errx, csp );
            NewState( EImpsCSPIdle );
            }
        return;
        }   // LOGIN RESPONSE1

    // Login response, 4-way login phase two or 2-way login
    // ****************************************************
    // save session id if received
    if ( aFields->MessageType() == EImpsLoginRes &&
         aFields->Status() == KErrNone &&
         aFields->ResponseStatusL() == KImpsStatusOk &&
         iLoginPhase == 2 )
        {
        iLoginPhase++;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: SessionStartupL 4-login phase two" ) );
#endif
        TImpsDataAccessor myAc( aFields );

        TPtrC newId;
        TImpsSDataUtils::GetLoginSessionIDL( &myAc, newId );
        // protect us against illegal data from SAP
        if ( newId.Length() > KImpsMaxSID )
            {
            User::Leave( KErrCorrupt );
            }
        iCSPSessionId = newId;

        // get TimeToLive
        TInt newTime( 0 );
        if ( TImpsSDataUtils::GetKeepAliveL( &myAc, newTime ) )
            {
            iAliveTime = newTime;
            }

        NewState( EImpsCSPLogged );
        // This is next message to be sent
        iMsg = EImpsClientCapabilityReq;

        // KeepAlive timer started when logged in
        iAliveMgr->StartTimer( iAliveTime );
        // Start polling timer with a static value to be sure that
        // something is received if SAP happened to send an empty response.
        // After capability negotiation a proper value is set for polling timer.
        iPollState = EImpsPollAdaptive;
        iIdleTimer->Start( iSettings.iMaxPollTime );
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
        CImpsClientLogger::Log( _L( "CSPSession: PollTime %d" ), iSettings.iMaxPollTime );
        CImpsClientLogger::Log( _L( "CSPSession: AliveTime %d" ), iAliveTime );
#endif

        // Let's send Client Capability request
        TRAP( errx, DoSendClientCapabilityReqL() );
        if ( errx )
            {
            TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
            iServer.SendErrorEvent( EImpsEventServerLogin,
                                    KImpsErrorCapabilities, 0, csp );
            }
        return;
        }   // LOGIN RESPONSE


    // Client capability response
    // ***************************
    if ( aFields->MessageType() == EImpsClientCapabilityRes &&
         aFields->Status() == KErrNone &&
         IsLogged() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: SessionStartupL ClientCapabality" ) );
#endif
        TRAP( errx, HandleClientCapabilityResL ( aFields ) );
        if ( errx )
            {
            TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
            iServer.SendErrorEvent( EImpsEventServerLogin, KImpsErrorCapabilities, 0, csp );
            NewState( EImpsCSPLogged );
            }
        // Let's send Service Negotiation request
        TRAP( errx, DoSendServiceNegotiationReqL () );
        if ( errx )
            {
            TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
            iServer.SendErrorEvent( EImpsEventServerLogin, KImpsErrorServices, 0, csp );
            NewState( EImpsCSPLogged );
            }
        return;
        }

    // Service Negotiation response
    // ****************************
    if ( aFields->MessageType() == EImpsServiceRes &&
         aFields->Status() == KErrNone &&
         IsLogged() )
        {

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: SessionStartupL ServiceRes" ) );
#endif
        iTempTid2 = KNullDesC;

        if ( aFields->MessageType() == EImpsServiceRes )
            {
            TImpsDataAccessor myAccess( aFields );
            // Calculate agreed features and functions
            TImpsSrvUtils::DiscardServicesL(
                &myAccess,
                &iServices );
            }

        // WV engine state in ON_LINE now
        iIntStatus = EInternal_ON_LINE;
        NewState( EImpsCSPOnLine );
        iMsg = EImpsMessageNone;

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: status ON_LINE" ) );
#endif
        // Send events to clients
        iServer.TransportStatus( EInternal_ON_LINE, this );

        // Start PDP idle time if WAP-SMS-CIR is negotiated as only CIR channel.
        if ( iSettings.iSMSWAPCIR == 2 ||
             iSettings.iUDPWAPCIR == 2 ||
             iSettings.iTCPSCIR == 2 ||
             iSettings.iUDPSCIR == 2 )
            {
            // goto CIR-polling-mode
            iPollState = EImpsPollCIR;
            iPollTime = iSettings.CIRModePollTime();
            iIdleTimer->Start( iPollTime );
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
            CImpsClientLogger::Log( _L( "CSPSession: iPollTime %d" ), iPollTime );
#endif
            }
        if ( iSettings.iSMSWAPCIR == 2 )
            {
            // PDP Idle timer closes PDP-context
            iPDPIdleTimer->Start( iSettings.PDPExpiryTime() );
            }

        TInt errxy = KErrNone;
        TRAP( errxy, DoStartIpCIRL() );
        }

    // Error handling in negotiation phase AND FOR 4-WAY-LOGIN
    // ********************************************************
    if ( ( aFields->Status() != KErrNone ) ||
         ( aFields->ResponseStatusL() != KImpsStatusOk &&
           aFields->ResponseStatusL() != 0 ) )
        {
        TInt errorCode = KErrNone;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: SessionStartupL Error handling %d" ), iMsg );
#endif
        switch ( iMsg )
            {
            case EImpsClientCapabilityReq:
                errorCode = KImpsErrorCapabilities;
                break;
            case EImpsServiceReq:
                errorCode = KImpsErrorServices;
                break;
            default:
                break;
            }
        if ( errorCode )
            {
            TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
            iServer.SendErrorEvent( EImpsEventServerLogin, errorCode, 0, csp );
            // We assume that an application will call logout in this case
            // and thus we do not close PDP here.
            }
        else
            {
            // RouteMessageL handles errors if iLoginPhase != 2
            NewState( EImpsCSPIdle );
            // routeMessaegL does not close PDP context so let't do it now
            iReceiver2->CloseTr();
            }
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: SessionStartupL ends" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoSendClientCapabilityReqL
// -----------------------------------------------------------------------------
void CImpsCSPSession::DoSendClientCapabilityReqL ()
    {
    CImpsFields* tempFields = CImpsFields::NewL();
    CleanupStack::PushL( tempFields );  // <<< tempFields
    tempFields->SetMessageType( EImpsClientCapabilityReq );
    tempFields->SetSessionTypeL( EImpsInband );
    iTempTid2 = GenerateTid();
    TImpsDataAccessor myAccess( tempFields );

    iMultiTrans = iSettings.iMultiTrans;
    iPollTime = iSettings.iPollTime;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: MaxParserSize=%d" ), iSettings.MaximumParserSize() );
    CImpsClientLogger::Log( _L( "CSPSession: MaxMessageSize=%d" ), iSettings.MaximumMessageSize() );
#endif

    TImpsSDataUtils::CreateClientCapabilityReqL(
        &myAccess,
        iCSPSessionId,
        iTempTid2,
        iLogCID,
        iMultiTrans,
        iPollTime,
        iSettings );
    // Put Client Capability request into queue to wait sending
    // Do not copy but add this instance.
    // Negotiation phase expiry time is saved in iNegoExpiry so does not
    // matter what we set here.
    iMsg = EImpsClientCapabilityReq;
    QueueClientRequestL( tempFields, 0, EFalse );
    SendAllQueued();
    CleanupStack::Pop( 1 ); // tempFields  >>>
    iNegoExpiry = iServer.ExpiryTime( EImpsEventServerLogin );
    NewState( EImpsCSPNegotiation );
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoSendServiceNegotiationReqL
// -----------------------------------------------------------------------------
void CImpsCSPSession::DoSendServiceNegotiationReqL()
    {
    iMsg = EImpsServiceReq;
    CImpsFields* tempFields = CImpsFields::NewL();
    CleanupStack::PushL( tempFields );      // <<< tempfields
    tempFields->SetMessageType( EImpsServiceReq );
    tempFields->SetSessionTypeL( EImpsInband );
    iTempTid2 = GenerateTid();
    TImpsDataAccessor myAcc( tempFields );

    // Initialize the iServices again (in case of re-login)
    TImpsSrvUtils::InitializeServices( iServices, iReactive );
    TImpsSDataUtils::CreateServiceRequestL(
        &myAcc,
        iServices,
        iCSPSessionId,
        iTempTid2,
        iLogCID,
        iReactive );

    // Put Service Negotiation request into queue to wait sending.
    // Negotiation phase expiry time is saved in iNegoExpiry so does not
    // matter what we set here.
    QueueClientRequestL( tempFields, 0, EFalse );
    SendAllQueued();
    CleanupStack::Pop( 1 );         // >>> tempFields
    iNegoExpiry = iServer.ExpiryTime( EImpsEventServerLogin );
    NewState( EImpsCSPNegotiation );
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::HandleLoginResponseL
// -----------------------------------------------------------------------------
void CImpsCSPSession::HandleLoginResponseL ( CImpsFields* aFields )
    {
    // protect us against illegal data from SAP
    TImpsDataAccessor myAc( aFields );

    TPtrC newId;
    TImpsSDataUtils::GetLoginSessionIDL( &myAc, newId );
    if ( newId.Length() > KImpsMaxSID )
        {
        User::Leave( KErrCorrupt );
        }
    iCSPSessionId = newId;

    TDesC8* nonce = NULL;
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( EImpsKeySession );
    myKey->AddL( EImpsKeyTransaction );
    myKey->AddL( EImpsKeyTransactionContent );
    myKey->AddL( EImpsKeyLogin_Response );
    myKey->AddL( EImpsKeyDigestSchema );
    TInt schema;
    TBool sendPwd = myAc.RestoreIntegerL( myKey, schema );
    if ( sendPwd )
        {
        if ( schema == EImpsPWD )
            {
            // Now we have to send the PWD
            sendPwd = ETrue;
            }
        else
            sendPwd = EFalse;
        }
    myKey->PopL();
    myKey->AddL( EImpsKeyNonce );
    ( void )myAc.RestoreDesc8L( myKey, nonce );
    TInt newTime( 0 );
    if ( TImpsSDataUtils::GetKeepAliveL( &myAc, newTime ) )
        {
        iAliveTime = newTime;
        }

    TImpsSDataUtils::SetResultStatusL( myKey, &myAc, KImpsStatusOk );

    CleanupStack::PopAndDestroy( 1 );
    CImpsFields* tempFields = CImpsFields::NewL();
    CleanupStack::PushL( tempFields );
    tempFields->SetMessageType( EImpsLoginReq );
    iTempTid2 = LogTid();

    TImpsDataAccessor myAcc( tempFields );

    TImpsSDataUtils::CreateLoginReqPhaseTwoL(
        sendPwd,
        &myAcc,
        iCSPSessionId,
        LogTid(),
        UserId(),
        iLogCID,
        LogPwd(),
        nonce,
        iCookie,
        iAliveTime,
        iKey1,
        iKey2 );

    iMsg = EImpsMessageNone;

    // Send 2nd login phase request
    // Login expiry time is saved in iNegoExpiry so no need to calculate it here.
    QueueClientRequestL( tempFields, 0, EFalse );
    SendAllQueued();

    CleanupStack::Pop( 1 );
    delete iKey1; // not needed anymore
    iKey1 = NULL;
    delete iKey2; // not needed anymore
    iKey2 = NULL;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::HandleClientCapabilityResL
// -----------------------------------------------------------------------------
void CImpsCSPSession::HandleClientCapabilityResL ( CImpsFields* aFields )
    {
    iTempTid2 = KNullDesC;

    TImpsDataAccessor myAccess( aFields );
    CImpsKey* myKey = CImpsKey::NewLC();   // <<< myKey

    const TImpsContent* myContent = KClientCapabilityResElements;
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        myContent,
        sizeof( KClientCapabilityResElements ) /
        sizeof( KClientCapabilityResElements[0] ) );


    // WV CSP versions are not fully compatible
    if ( iCSPVersion == EImpsCspVersion11 )
        {
        // WV 1.1 code
        // CapabilityList element
        myKey->AddL( CREATEKEY( EImpsKeyCapabilityList, 0 ) );

        // We are interested in supported CIR methods, TCP/IP CIR addresses
        // and polltime.
        // SupportedBearer is ignored because of we support HTTP only.´
        // WV 1.1 specific ClientID is ignored too.
        // AcceptedContentType not in a response based on
        // Corrigenda Catalog for WV ver 1.1
        }
    else
        {
        // WV 1.2 code
        // AgreedCapabilityList element
        myKey->AddL( CREATEKEY( EImpsKeyAgreedCapabilityList, 0 ) );

        // We are interested in supported CIR methods, TCP/IP CIR addresses
        // and polltime.
        // SupportedBearer is ignored because of we support HTTP only.
        // CIRURL is ignored too because of it is for HTTP CIR binding only
        // that is not supported by this sw.
        // <!ELEMENT AgreedCapabilityList
        //   (SupportedBearer*, SupportedCIRMethod*, TCPAddress?, TCPPort?,
        //   ServerPollMin?, CIRURL?)>
        }

    // ServerPollMin
    myKey->AddL( CREATEKEY( EImpsKeyServerPollMin, 0 ) );
    TInt myInt;
    if ( myAccess.RestoreIntegerL( myKey, myInt ) )
        {
        if ( myInt > iPollTime )
            {
            iPollTime = myInt;
            }
        }
    // Save new minimum polling time value in settings
    iSettings.iPollTime = iPollTime;

    // Reset polling timer, ServerPollMin is optional, but if we do not get
    // new value then we start to us our own suggestion
    iPollState = EImpsPollAdaptive;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: PollTime %d" ), iPollTime );
    CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
#endif
    iIdleTimer->Start( iPollTime );

    // MultiTrans response
    myKey->ReplaceLastL( CREATEKEY( EImpsKeyMultiTrans, 0 ) );
    if ( myAccess.RestoreIntegerL( myKey, myInt ) )
        {
        iMultiTrans = myInt;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: new iMultiTrans %d" ), iMultiTrans );
#endif
        }

    myKey->PopL();

    // SupportedCIRMethod update
    iSettings.SetCirBindingsL( &myAccess, myKey );

    // TCPAddress
    TDesC* myPtr;
    myKey->AddL( CREATEKEY( EImpsKeyTCPAddress, 0 ) );
    if ( myAccess.RestoreDescL( myKey, myPtr ) )
        {
        HBufC* newAttrib = myPtr->AllocL();
        delete iTCPAddr;
        iTCPAddr = newAttrib;
        }

    // TCPPort
    myKey->ReplaceLastL( CREATEKEY( EImpsKeyTCPPort, 0 ) );
    if ( myAccess.RestoreIntegerL( myKey, myInt ) )
        {
        iTCPPort = ( TUint )myInt;
        }

    CleanupStack::PopAndDestroy( 1 );   // >>> mKey
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::SendPollL
// -----------------------------------------------------------------------------
void CImpsCSPSession::SendPollL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: SendPollL csp=%d" ), ( TInt )this );
#endif

    TInt orig = iPollInResume;
    iPollInResume = 0;

    if ( iIntStatus != EInternal_ON_LINE || IsPDPIdle() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: SendPollL IGNORED" ) );
#endif
        iPollInResume = 2;
        User::Leave( KErrNotReady );
        }

    // Create Poll request
    iSnd->Reset();
    iSnd->SetSessionTypeL( EImpsInband );
    iSnd->SetMessageType( EImpsPolling );
    iSnd->SetSessionIdL( iCSPSessionId );
    CImpsDataAccessor* myAccess = CImpsDataAccessor::NewLC( iSnd ); // <<<
    CImpsKey* myKey = CImpsKey::NewLC(); // <<<
    // Add TImpsTransactionMode now
    myKey->Reset();
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransModeElements,
        sizeof( KTransModeElements ) /
        sizeof( KTransModeElements[0] ) );
    myAccess->StoreIntegerL( myKey, EImpsRequest );
    myKey->Reset();
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
    myKey->AddL( CREATEKEY( EImpsKeyPolling_Request, 0 ) );
    myAccess->StoreEmptyL( myKey );

    // Send
    iTempTid = KNullDesC;
    iSnd->SetTidL( iTempTid );

    iPollWasRequested = EFalse;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: iPollWasRequested->F in SendPollL" ) );
#endif

    if ( orig >= 1 )
        {
        iPollInResume = 2;
        }
    DoSendSingleL( myAccess, iTempTid, EImpsPolling, KImpsPollTO );
    iPollInResume = 0;

    CleanupStack::PopAndDestroy( 2 );   // >> myKey, myAccess
    }


// -----------------------------------------------------------------------------
// CImpsCSPSession::SendAliveL
// -----------------------------------------------------------------------------
TPtrC CImpsCSPSession::SendAliveL( )
    {
    return DoSendAliveL();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::SendAliveInResume
// -----------------------------------------------------------------------------
void CImpsCSPSession::SendAliveInResume( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: SendAliveInResume iPendingAlive->T" ) );
#endif
    iPendingAlive = ETrue;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoSendAliveL
// -----------------------------------------------------------------------------
TPtrC CImpsCSPSession::DoSendAliveL( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DoSendAliveL csp=%d" ), ( TInt )this );
#endif

    // send poll request. Do not update Transaction queue
    iSnd->Reset();
    iSnd->SetSessionTypeL( EImpsInband );
    iSnd->SetMessageType( EImpsKeepAliveReq );
    iSnd->SetSessionIdL( iCSPSessionId );

    // Set TID
    TPtrC tid;
    tid.Set( GenerateTid() );
    iSnd->SetTidL( tid );

    // Add KeepAlive
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( iSnd ); // <<< myAc
    CImpsKey* myk = CImpsKey::NewLC();    // <<< myk
    TImpsSDataUtils::AddValuesFromArrayL(
        myk,
        KKeepAliveReq,
        sizeof( KKeepAliveReq ) /
        sizeof( KKeepAliveReq[0] ) );
    myAc->StoreEmptyL( myk );
    // Add Transaction mode
    myk->Reset();
    TImpsSDataUtils::AddValuesFromArrayL(
        myk,
        KTransModeElements,
        sizeof( KTransModeElements ) /
        sizeof( KTransModeElements[0] ) );
    myAc->StoreIntegerL( myk, EImpsRequest );

    if ( IsPDPIdle() )  // PDP idle-> start to open
        {
        iPendingAlive = ETrue;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPendingAlive->T" ) );
#endif
        if ( iCSPState == EImpsCSPOnLineIdle )
            {
            OpenPDP();
            }
        else if ( iCSPState == EImpsCSPOnLineIdleStart )
            {
            iPendingPDP = EImpsPDPPendOpen;
            }
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: DoSendAliveL POSTPONED ***" ) );
#endif
        }
    else if ( iIntStatus != EInternal_ON_LINE )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: DoSendAliveL IGNORED" ) );
#endif
        iSnd->Reset();
        User::Leave( KImpsGeneralError );
        }
    else
        {
        DoSendSingleL( myAc, iTempTid, EImpsKeepAliveReq, KImpsKeepATO );
        iPendingAlive = EFalse;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPendingAlive->F" ) );
#endif
        }

    CleanupStack::PopAndDestroy( 2 );   // >>> myk myAc

    return iTempTid;
    }


// -----------------------------------------------------------------------------
// CImpsCSPSession::SendAllQueued
// -----------------------------------------------------------------------------
void CImpsCSPSession::SendAllQueued()
    {
    // Actually this does not send all messages at once but one by one.
    // iSendQ calls DoSendAllQueued that calls (in most cases)
    // this again and so on until no more valid messages found!
    // DoSendAllQueued handles SAP initiated requests in same way.
    if ( RequestExist() || SapReqExist() || iPendingAlive ||
         iPollState == EImpsPollExtWait || iPollInResume == 1 ||
         iPollWasRequested )
        {
        if ( !IsPDPIdle() )
            {
            iSendQ->Send();
            }
        else if ( iCSPState == EImpsCSPOnLineIdle )
            {
            iPollState = EImpsPollCIR;
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
#endif
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::CreateSapResponse
// -----------------------------------------------------------------------------
TImpsSAPResStatus CImpsCSPSession::CreateSapResponse()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CreateSapResponse begins csp=%d" ), ( TInt )this );
#endif

    TImpsSAPResStatus ret( EImpsSAPNone );
    TInt err = KErrNone;
    // Check if pending SAP initiated CSP transactions exists
    TDblQueIter<CTransaction> iter2 ( iTransactionList );
    iter2.SetToFirst();
    if ( iter2 )
        {
        ret = EImpsSAPMore;
        CTransaction* trans = iter2;
        // Get first
        iter2++;
        CImpsFields* fields = trans->iFields;
        TInt messageType = fields->MessageType();

        // Try to get actual message type
        if ( messageType == EImpsPureData )
            {
            TRAP( err, TImpsDataAccessor temp ( fields );
                  messageType = TImpsSDataUtils::GetPureMessageTypeL( &temp ) );
            }

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: CreateSapResponse messageType=%x" ), messageType );
#endif
        // Here we have a list of supported push messages, i.e.
        // SAP initiated transactions.
        // Other pushed messages are not supported!
        // Notice: add new methods as needed
        // Disconnect is a special case handled before this method call
        if ( messageType == EImpsNewMessage ||
             messageType == EImpsLeaveGroupRes ||
             messageType == EImpsInviteUserReq ||
             messageType == EImpsInviteRes ||
             messageType == EImpsCancelInviteUserReq ||
             messageType == EImpsPresenceNotification ||
             messageType == EImpsGroupChangeNotice ||
             messageType == EImpsPresenceAuthReq ||
             messageType == EImpsDeliveryReportReq
           )
            {
            // This also tries to send a SAP initiated transaction response once immediately
            TRAP( err, DoMakeSapResponseL( trans ) );
            if ( !err )
                {
                iPollInResume = 0;
                }
            // Last request handled.
            if ( !iter2 )
                {
                ret = EImpsSAPLast;
                iPollInResume = 0;

                // All the WV servers do not work according to WV specs and they seems to need
                // to get a Poll request instead for SAP initiated transactions, therefore the
                // extra polling is needed.
                err = KErrNone;
                TBool polli ( EFalse );
                TRAP( err, polli = fields->PollL() )
                if ( polli )
                    {
                    iPollWasRequested = ETrue;
#ifndef _NO_IMPS_LOGGING_
                    CImpsClientLogger::Log( _L( "CSPSession: iPollWasRequested->T in CreateSapResponse" ) );
#endif
                    }
                }

#ifndef _NO_IMPS_LOGGING_
            if ( err )
                {
                CImpsClientLogger::Log( _L( "CSPSession: DoMakeSapResponseL ERROR %d" ), err );
                }

#endif
            }

        // Transaction is deleted. If the response has failed the WV server should send
        // the transaction again. Timer limit is not implemented in terminal side here.
        trans->Destroy();

        } // if (iter2)
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CreateSapResponse returns %d" ), ret );
#endif
    return ret;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoSendAllQueuedL
// -----------------------------------------------------------------------------
void CImpsCSPSession::DoSendAllQueuedL()
    {

    TInt errx = KErrNone;

    // Handle login phase specials first
    // ---------------------------------
    if ( iLoginPhase != 2 )
        {
        if ( !IsLogged() || !iReceiver2->isConnected() ||
             ( iReceiver2->NbrOfPending() >= KImpsMaxPending ) ||
             IsPDPIdle() ) // no reason to try if PDP is not open
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: DoSendAllQueuedL IGNORED" ) );
#endif
            return;
            }
        }

    // Pending Alive messages and pending Poll messages
    // -------------------------------------------------
    if ( iPendingAlive )
        {
        // Pending Alive overrides pending Poll
        iPendingAlive = EFalse;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPendingAlive->F" ) );
#endif
        iPollInResume = 0;
        iAliveMgr->SendKeepAlive( EFalse );
        SendAllQueued();
        return;
        }
    else if ( iPollState == EImpsPollExtWait )
        {
        iPollState = EImpsPollCIR;
        iPollTime = iSettings.CIRModePollTime();
        iIdleTimer->Start( iPollTime );
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
        CImpsClientLogger::Log( _L( "CSPSession: iPollTime %d" ), iPollTime );
#endif
        errx = KErrNone;
        TRAP( errx, SendPollL() );
        // If Polling fails then try again
        if ( errx && !iPollInResume )
            {
            iPollInResume = 1;
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iPollInResume %d" ), iPollInResume );
#endif
            }

        SendAllQueued();
        return;
        }

    // Reqular cases
    // --------------------------
    CReq* request = NULL;

    if ( iTr < iMultiTrans )
        {
        // Try to send terminal initiated request
        request = GiveNextRequestL();

        // Block messages in negotiation phase
        if ( request && request->iFields )
            {
            TInt msgType = request->iFields->MessageType();
            if ( iIntStatus != EInternal_ON_LINE &&
                 ( msgType != EImpsKeepAliveReq &&
                   msgType != EImpsClientCapabilityReq &&
                   msgType != EImpsServiceReq &&
                   msgType != EImpsLoginReq &&
                   msgType != EImpsLogoutReq ) )

                {
                request = NULL;
                }
            }

        if ( request )
            {
            iPollInResume = 0;
            // Send next buffered user request if any
            // search. Not immediately, but via active object scheduler
            SendAllQueued();

            CImpsFields* fields = request->iFields;
            errx = KErrNone;
            TRAP( errx, DoSendNextBufferedL( fields ) );
            // Delete the client request
            request->Destroy();
            } // if request
        }

    if ( !request )
        {
        // Check if SAP initiated message instead of terminal initiated one.
        // This sends data and calls SendAllQueued if needed.
        // comment: SAP responses should not exists in PDP-idle state

        // CreateSapResponse not only create the response but also tries to send it..
        TImpsSAPResStatus sapr = CreateSapResponse();
        if ( ( sapr != EImpsSAPMore ) &&
             ( iPollWasRequested || iPollInResume == 1 ) &&
             ( IsLogged() ) )
            {
            errx = KErrNone;
            // If Poll fails then re-try polling when GPRS resumes
            TRAP( errx, SendPollL() );
            if ( errx && !iPollInResume )
                {
                iPollInResume = 1;
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: err: iPollInResume %d" ), iPollInResume );
#endif
                }
            }
        else if ( sapr == EImpsSAPMore )
            {
            SendAllQueued();
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::CancelTrans
// client subsession calls this when request expires
// -----------------------------------------------------------------------------
void CImpsCSPSession::CancelTrans( const TDesC& aTID )
    {
    DeleteRequest( aTID );
    // CancelTrans does not cause callback.
    iReceiver2->CancelTrans( aTID );
    TrMinus();
    // Check if login-req expires and change CSP state
    if ( iCSPState == EImpsCSPLogging && !aTID.Compare( LogTid() ) )
        {
        NewState( EImpsCSPIdle );
        CancelData();
        }

    SendAllQueued();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::GenerateTid
// -----------------------------------------------------------------------------
TPtrC CImpsCSPSession::GenerateTid( )
    {
    // iCounter++;
    // TInt tidNbr = iCounter;
    TInt tidNbr = iServer.TidSeed();
    iTempTid = KNullDesC;
    iTempTid = _L( "nok" );
    iTempTid.AppendFormat( _L( "%d" ), tidNbr  );
    return iTempTid;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::GenerateCookie
// -----------------------------------------------------------------------------
TPtrC CImpsCSPSession::GenerateCookie( )
    {
    TTime myTime;
    myTime.HomeTime();
    const TInt64& seed = myTime.Int64();
    TUint low = I64LOW( seed );
    TUint high = I64HIGH( seed );
    TInt64 seed2 MAKE_TINT64( high, low );
    iCookie = KNullDesC;
    iCookie = _L( "wv:nokia." );
#ifndef _FAKE_RESPONSE
    TInt tidNbr = Math::Rand( seed2 );
    iCookie.AppendFormat( _L( "%d" ), tidNbr  );
#else
    iCookie.AppendFormat( _L( "%d" ), 12345  );
#endif
    // copy to 8-bit buffer for CIR
    iCookie8.Copy( iCookie );
    return iCookie;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoLogout
// -----------------------------------------------------------------------------
void CImpsCSPSession::DoLogout( )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DoLogout" ) );
#endif
    DoLogoutNow( 0 );
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoLogoutNow
// -----------------------------------------------------------------------------
void CImpsCSPSession::DoLogoutNow( TInt aOpId )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DoLogoutNow opid=%d, log=%d csp=%d" ),
                            aOpId, IsLogged(), ( TInt )this );
#endif

    // save old status for this method
    TBool oldLogged = IsLogged();
    TImpsCSPState oldState = iCSPState;
    NewState( EImpsCSPIdle );

    iAliveMgr->StopTimer();
    iIdleTimer->Stop();
    // stop PDP idle timer in logout routines
    iPDPIdleTimer->Stop( );
    iPDPOpenTimer->Stop( );
    iPollState = EImpsPollNone;
    iPendingPDP = EImpsPDPPendNone;
    iPendPDPLogout = EFalse;
    iPollWasRequested = EFalse;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: iPollState %d  iPollWasRequested->F in logout" ), iPollState );
#endif

    // Reset settings for further logins
    ResetSession();

    iSendQ->Cancel();

    // Reset local variables
    iIntStatus = EInternal_NOT_LOGGED;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: status NOT_LOGGED" ) );
#endif

    // Clean own queues
    DeleteRequests();
    DeleteTransactions();
    iTidSapHistory->DeleteAll();
    iTidCliHistory->DeleteAll();

    // Close ip cir
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CLOSE IP-CIR CHANNEL" ) );
#endif
    if ( iCirManager )
        {
        // Close all cir bindings
        iCirManager->CloseCh( 0xF );
        }

    // Cancel the HTTP transport in logout
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CANCEL TRANSPORT IN DOLOGOUT" ) );
#endif
    CancelData();

    // Clean server core and send logout and status events there.
    // We first detect if this is really CSP disconnect situation.
    TBool cspDisconnect = ( oldLogged || oldState == EImpsCSPDisconnecting ) ?
                          ETrue : EFalse ;
    iServer.DoLogoutSrv( aOpId, cspDisconnect, this );

    delete iSAP;
    iSAP = NULL;
    iIAP = 0;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CLOSE TRANSPORT SOON..." ) );
#endif
    if ( iReceiver2->Close2() == KErrCancel )
        {
        // Start to delete this session asynchronously
        // if there is nothing to close.
        iServer.TransportStatus(  EInternal_NO_IAP, this );
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::CancelData
// server CloseSession calls this
// -----------------------------------------------------------------------------
void CImpsCSPSession::CancelData( )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CancelData csp=%d" ), ( TInt )this );
#endif
    // Cancel the HTTP transport now, no callback
    iReceiver2->Cancel();
    // Must reset variables because if new Login request mey be
    // received before WV Engine is shut down and CloseEngine() called.
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::QueueClientRequestL
// -----------------------------------------------------------------------------
void CImpsCSPSession::QueueClientRequestL(
    CImpsFields* aFields,
    TInt aExpiry,
    TBool aLast )
    {
    // Check if subsession specific expiry time needed
    CReq* req = new ( ELeave ) CReq( aFields );
    if ( !aExpiry )
        {
        req->iExpiry = iServer.ExpiryTime( ( TImpsMessageType ) aFields->MessageType() );
        }
    else
        {
        req->iExpiry = iServer.ExpiryTime( aExpiry );
        }

    // Put request into queue in a right place,
    if ( aLast )
        {
        iRequestList.AddLast( *req );
        }
    else
        {
        iRequestList.AddFirst( *req );
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::QueueTidL
// This resets iRcv!
// -----------------------------------------------------------------------------
void CImpsCSPSession::QueueTidL( TBool aOrphan )
    {

    if ( !ValidateTid( EFalse ) )
        {
        // TID is already responded to SAP
        return;
        }

    TPtrC tid;
    TImpsDataAccessor myAccess( iRcv );
    TImpsDataUtils::GetTransactionIDL( &myAccess, tid );

    if ( tid.Length() > 0 )
        {

        // Add new Sap initiated transaction to queue
        CTransaction* trans = new ( ELeave ) CTransaction(
            aOrphan,
            tid,                    // tid
            iRcv->SessionIdL(),     // session-id
            ( TImpsMessageType ) iRcv->MessageType(),
            iRcv );

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: QUEUE SAP TID=%S csp=%d" ),
                                &( trans->iTID ), ( TInt )this ); //lint !e525
#endif

        iTransactionList.AddLast( *trans );
        iRcv = NULL;
        iRcv = CImpsFields::NewL();
        iRcv->Reset();
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::ValidateTid
// Validate SAP initiated TID
// -----------------------------------------------------------------------------
TBool CImpsCSPSession::ValidateTid( TBool aClientHistory )
    {

    // The requesting entity may resend the request message using the same
    // transaction identifier. The responding entity should
    // guarantee that the requested operation or data is carried out only once,
    // even if multiple request messages with the same transaction identifier
    // are received.

    // Check that TID has not already sent to clients or has not responded to SAP.

    TPtrC tid;
    TImpsDataAccessor myAccess( iRcv );
    TInt errx = KErrNone;
    TRAP( errx, TImpsDataUtils::GetTransactionIDL( &myAccess, tid ) );

    if ( tid.Length() > 0 )
        {
        if ( aClientHistory && iTidCliHistory->TidFound( tid ) )
            {
            // TID has sent to clients
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: already received TID=%S ***" ), &tid );
#endif
            return EFalse;
            }
        else if ( aClientHistory /* && !iTidCliHistory->TidFound( tid )*/ )
            {
            // Save TId into client history
            iTidCliHistory->Add( tid );
            }
        else if ( !aClientHistory && iTidSapHistory->TidFound( tid ) )
            {
            // TID has already responded to SAP
            // TID-SAP-history has been updated when the transport response is sent
            // successfully to SAP (i.e. no transport error)
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: already responded TID=%S ***" ), &tid );
#endif
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DiscardTid
// -----------------------------------------------------------------------------
void CImpsCSPSession::DiscardTid( const TDesC& aTid )
    {
    // Delete buffered transaction
    TDblQueIter<CTransaction> rIter( iTransactionList );
    rIter.SetToFirst();
    while ( rIter )
        {
        CTransaction* trans = rIter;
        rIter++;
        if ( !trans->iTID.Compare( aTid ) )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: DiscardTid deletes a request " ) );
#endif
            trans->Destroy();
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DeleteTransactions
// -----------------------------------------------------------------------------
void CImpsCSPSession::DeleteTransactions()
    {
    // Delete all buffered transactions
    TDblQueIter<CTransaction> rIter( iTransactionList );

    rIter.SetToFirst();

    while ( rIter )
        {
        CTransaction* trans = rIter;
        rIter++;
        trans->Destroy();
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DeleteRequests
// -----------------------------------------------------------------------------
void CImpsCSPSession::DeleteRequests()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DeleteRequests csp=%d" ), ( TInt )this );
#endif
    // Delete all buffered transaction requests
    TDblQueIter<CReq> rIter( iRequestList );
    rIter.SetToFirst();

    while ( rIter )
        {
        CReq* req = rIter;
        rIter++;
        req->Destroy();
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DeleteRequest
// -----------------------------------------------------------------------------
void CImpsCSPSession::DeleteRequest( const TDesC& aTID )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DeleteRequest tid=%S csp=%d" ), &aTID, ( TInt )this );
#endif
    // Delete buffered transaction request
    TInt errx = KErrNone;
    TPtrC p( KNullDesC );

    TDblQueIter<CReq> rIter( iRequestList );
    rIter.SetToFirst();

    while ( rIter )
        {
        CReq* req = rIter;
        rIter++;
        TRAP( errx, p.Set( req->iFields->TidL() ) );
        if ( !p.Compare( aTID ) )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: DeleteRequest MATCH tid=%S csp=%d" ), &aTID, ( TInt )this );
#endif
            req->Destroy();

            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::RequestExist
// -----------------------------------------------------------------------------
TBool CImpsCSPSession::RequestExist()
    {
    // Check buffered transactions
    TDblQueIter<CReq> rIter( iRequestList );

    rIter.SetToFirst();

    if ( rIter )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::SapReqExist
// -----------------------------------------------------------------------------
TBool CImpsCSPSession::SapReqExist()
    {
    TDblQueIter<CTransaction> iter2 ( iTransactionList );
    iter2.SetToFirst();
    if ( iter2 )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoSendNextBufferedL
// -----------------------------------------------------------------------------
void CImpsCSPSession::DoSendNextBufferedL( CImpsFields* aFields )
    {
    TImpsDataAccessor myAccess( aFields );
    TRAPD( err, DoSendSingleL( &myAccess, aFields->TidL(),
                               ( TImpsMessageType )aFields->MessageType() ) );
    if ( err )
        {
        //  send an error event
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: DoSendSingleL ERROR=%d csp=%d" ), err, ( TInt )this );
#endif
        iSnd->Reset();
        iSnd->SetStatus( err );
        iSnd->SetTidL( aFields->TidL() );
        TBool someFound;
        // This sends error event to the right client session
        TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
        iServer.CheckRequestsL( iSnd, someFound, csp );
        }
    }

// ---------------------------------------------------------
// CImpsCSPSession::TransportStatus
// ---------------------------------------------------------
//
void CImpsCSPSession::TransportStatus(
    EImpsInternalStatus aConnectionState )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: TransportStatus=%d old=%d state=%d csp=%d" ),
                            ( TInt ) aConnectionState, iIntStatus, iCSPState, ( TInt )this );
#endif

    TInt errx = KErrNone;
    TRAP( errx, DoTransportStatusL ( aConnectionState ) );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: TransportStatus ends" ) );
#endif

    }

// ---------------------------------------------------------
// CImpsCSPSession::DoTransportStatusL
// ---------------------------------------------------------
//
void CImpsCSPSession::DoTransportStatusL(
    EImpsInternalStatus aConnectionState )
    {
    TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
    EImpsInternalStatus oldStatus = iIntStatus;

    // If there is pending logout request then the login is cancelled and
    // logout performed immediately.
    if ( iPendingLogout || iCancelLogin )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: err: ex3 iPendingLogout->F *****" ) );
#endif
        iPendingLogout = EFalse;
        if ( iCSPState == EImpsCSPLogging )
            {
            // This sends first a login response
            iServer.DiscardRequest( LogTid(), EImpsEventServerLogin, KErrCancel, csp );
            // Special case where login is cancelled
            if ( aConnectionState == EInternal_NO_IAP )
                {
                iServer.DiscardLogout( LogoutTID(), csp );
                // below rest of activities are done
                }
            }
        }

    // *********************************************
    // PDP idle situations handled first
    if ( IsPDPIdle() )
        {
        if ( aConnectionState == EInternal_NO_IAP )
            {
            NewState( EImpsCSPOnLineIdle );
            // In a rare situation there is pending PDP open request
            // received when PDP is closing down.
            if ( iPendingPDP == EImpsPDPPendOpen )
                {
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: iPendingLogout->F in NO_IAP ***" ) );
#endif
                iPendingPDP = EImpsPDPPendNone;
                OpenPDP();
                }
            return;
            }
        else if ( aConnectionState == EInternal_IAP_OPEN )
            {
            // PDP re-open completes here.
            NewState( EImpsCSPOnLine );
            if ( iPendingPDP == EImpsPDPPendOpen )
                {
                iPendingPDP = EImpsPDPPendNone;
                }
            // pending logout if PDP-context has been opened for Logout msg
            if ( iPendPDPLogout )
                {
                iPendPDPLogout = EFalse;
                iPendingPDP = EImpsPDPPendNone;
                TRAPD( errx, DoSendLogoutL( LogoutTID() ) );
                if ( errx )
                    {
                    // E5: Typically Bearer Suspended error with bad luck
#ifndef _NO_IMPS_LOGGING_
                    CImpsClientLogger::Log( _L( "CSPSession: err: E5 ****" ) );
#endif
                    DoLogout();
                    }
                }
            else
                {
                // Send all queued reqular requests to transport
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: Normal PDP open ***" ) );
#endif
                iPollInResume = 1;
                // *********************************************************
                // Notice: we assume that GPRS is resumed in this case even
                // if ConnManager DLL cannot provide us such an information
                // in all cases.
                iIntStatus = EInternal_ON_LINE;
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: status ON_LINE" ) );
#endif
                // start ip-cir-channels if not running
                TInt errxy = KErrNone;
                TRAP( errxy, DoStartIpCIRL() );
                }
            // Send all queued requests to transport
            SendAllQueued();
            }
        }

    // *********************************************
    // NO PDP IDLE STARTS HERE
    else if ( aConnectionState == EInternal_ON_LINE && IsNegotiated() )
        {
        iIntStatus = EInternal_ON_LINE;
        // we assume that old messages are in a bit heaven
        iTr = 0;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: status ON_LINE" ) );
#endif
        if ( iPollState == EImpsPollPending )
            {
            iPollState = EImpsPollExtWait;
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
#endif
            }

        // This makes poll to be sent if no other requests
        iPollInResume = 1;

        // pending logout if PDP-context has been opened for Logout msg
        if ( iPendPDPLogout )
            {
            iPendPDPLogout = EFalse;
            DoSendLogoutL( LogoutTID() );
            }
        else
            {
            SendAllQueued();
            // start ip-cir-channels if not running
            TInt errxy = KErrNone;
            TRAP( errxy, DoStartIpCIRL() );
            }
        }
    else if ( aConnectionState == EInternal_ON_LINE && !IsNegotiated() )
        {
        iIntStatus = EInternal_NOT_LOGGED;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: status NOT_LOGGED" ) );
#endif
        if ( IsLogged() )
            {
            // Rare situation where GPRS resume is taken place in negotiation phase.
            // iAliveMgr->SendKeepAlive( EFalse );
            SendAllQueued();
            }
        }
    else if ( aConnectionState == EInternal_OFF_LINE &&
              oldStatus == EInternal_ON_LINE )
        {
        iIntStatus = EInternal_OFF_LINE;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: status OFF_LINE" ) );
#endif
        }
    else if ( iCSPState == EImpsCSPLogging &&
              ( aConnectionState == EInternal_NO_IAP ||
                aConnectionState == EInternal_NO_IAP_AUTH ) )
        {
        // Discard request immediately in this case
        // This sends first a login response
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: err: Login cancelled" ) );
#endif
        TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
        iServer.DiscardRequest( LogTid(), EImpsEventServerLogin, KImpsErrorBearerSuspended, csp );
        DoLogout();
        }
    else if ( aConnectionState == EInternal_NO_IAP )
        {
        if ( IsNegotiated() )
            {
            // Check if no WAP SMS CIR negotiated then do logout.
            // Otherwise close PDP connection only
            if ( iSettings.iSMSWAPCIR == 2 )   // WAP CIR
                {
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: err: PDP LOST SMS-CIR ***" ) );
#endif
                NewState( EImpsCSPOnLineIdle );
                if ( iPendingPDP == EImpsPDPPendClose )
                    {
                    iPendingPDP = EImpsPDPPendNone;
                    ClosePDP();
                    }
                return;
                }
            else  // no WAP CIR
                {
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: err: PDP LOST NO-SMS-CIR ***" ) );
#endif
                DoLogout();
                }
            }
        else
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: err: PDP LOST NOT-NEGOTIATED ***" ) );
#endif
            if ( !IsLogged() )
                {
                iIntStatus = EInternal_NOT_LOGGED;
                // This causes DoLogout call from CImpsServer.
                iServer.TransportStatus( EInternal_NO_IAP, this );
                NewState( EImpsCSPIdle );
                return;
                }
            else
                // PDP can be lost in negotiation phase. Then we do internal
                // logout.
                {
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CSPSession: LOGGED BUT NOT-NEGOTIATED -> Logout ***" ) );
#endif
                DoLogout();
                }
            }
        }
    else if ( aConnectionState == EInternal_NO_IAP_AUTH )
        {
        // Handle authorative bearer-lost situation here and
        // make CSP logout
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: NO_IAP_AUTH ***" ) );
#endif
        iIntStatus = EInternal_NOT_LOGGED;
        DoLogout();
        }

    // Call server's TransportStatus only if this is a state change from
    // LOGGED->NOT_LOGGED.
    // NOT_LOGGED event clears the client session CSP identification data and
    // thus it must not be called in vain..
    if ( iIntStatus == EInternal_NOT_LOGGED && oldStatus != EInternal_NOT_LOGGED )
        {
        iServer.TransportStatus( iIntStatus, this );
        }
    }

// ---------------------------------------------------------
// CImpsCSPSession::CirChOpened
// ---------------------------------------------------------
//
void CImpsCSPSession::CirChOpened( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CirChOpened" ) );
#endif
    iTcpCIRError = EFalse;
    }

// ---------------------------------------------------------
// CImpsCSPSession::CirChError
// ---------------------------------------------------------
//
void CImpsCSPSession::CirChError( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: CirChError csp=%d" ), ( TInt )this );
#endif
    delete iCirManager;
    iCirManager = NULL;
    // start poll mode again
    if ( IsLogged() )
        {
        iPollTime = iSettings.iPollTime;
        iPollState = EImpsPollAdaptive;
        iIdleTimer->Start( iPollTime );
        iPDPIdleTimer->Stop();
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: iPollState %d" ), iPollState );
        CImpsClientLogger::Log( _L( "CSPSession: iPollTime %d" ), iPollTime );
#endif
        }
    }

// ---------------------------------------------------------
// CImpsCSPSession::SetConnAllowed
// ---------------------------------------------------------
//
void CImpsCSPSession::SetConnAllowed( TBool aParam )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: SetConnAllowed %d" ), aParam );
#endif
    iConAllowed = aParam;
    // Logout CSP session on behalf of user if terminal connection is not allowed.
    if ( !aParam )
        {
        // It's important that data channel is started to close first and after
        // that internal CSP disconnect is done.
        iReceiver2->CloseTr( );
        DoLogout( );
        }
    }

// ---------------------------------------------------------
// CImpsCSPSession::DoSendSingleL
// aAc connot be null
// ---------------------------------------------------------
//
void CImpsCSPSession::DoSendSingleL(
    MImpsDataAccessor* aAc,
    const TDesC& aTID,
    TImpsMessageType aMsgType,
    TInt aExpiryTime )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DoSendSingle messagetype = 0x%x csp=%d" ),
                            aMsgType, ( TInt )this );
#endif

    // Set the WV 1.2 version if needed, WV 1.1 is the default
    if ( iCSPVersion == EImpsCspVersion12 )
        {
        CImpsFields* f = aAc->GetImpsFields();
        f->SetCspVersion( EImpsCspVersion12 );
        }

    // Encode the message
    iEncoder->EncodeMessageL( *aAc, *iTransportOut );

    // Send to the transport module
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DoSendSingle message encoded = 0x%x" ), aMsgType );
#endif

    // reset PDP idle timer
    iPDPIdleTimer->Reset();

    iReceiver2->SendAndWaitL( aTID, aExpiryTime );
    iLastSent = aMsgType;

    // Notice: Maintain this list of non terminal originated
    // transactions, send by terminal to SAP.
    if ( !( aMsgType == EImpsPolling || aMsgType == EImpsKeepAliveReq ||
            aMsgType == EImpsStatus  || aMsgType == EImpsMessageDelivered ) )
        {
        iTr++;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: DoSendSingle iTr = %d" ), iTr );
#endif
        }
    // Anything except Poll and KeepAlive request resets Poll Timer.
    if ( !( aMsgType == EImpsPolling || aMsgType == EImpsKeepAliveReq ) )
        {
        ResetPollTime( );
        }

#ifdef _FAKE_RESPONSE
    iTestMsgTid = aTID;
#endif
    }
// -----------------------------------------------------------------------------
// CImpsCSPSession::DoSendLogoutL
// -----------------------------------------------------------------------------
void CImpsCSPSession::DoSendLogoutL( const TDesC& aTID )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DoSendLogoutL csp=%d" ), ( TInt )this );
#endif

    // Check if the transport is halted
    if ( iReceiver2->NbrOfPending() >= KImpsMaxPending )
        {
        User::Leave( KErrInUse );
        }

    NewState( EImpsCSPDisconnecting );
    // Set expiry time for logout.
    // In case of Login-csncel expiry time is not overwritten, but it
    // is the original login expiry time.
    // Otherwise clint session would expire first and the user client would get
    // normal time-out error, but we want that the user gets OK response.
    if ( !iCancelLogin )
        {
        iNegoExpiry = iServer.ExpiryTime( EImpsEventServerLogin );
        }

    // Make a logout request (encode)
    iSnd->Reset();

    // Insert CSP Session-id
    iSnd->SetSessionIdL( iCSPSessionId );
    iSnd->SetTidL( aTID );
    iSnd->SetMessageType( EImpsLogoutReq );

    // Session type is inband
    iSnd->SetSessionTypeL( EImpsInband );

    TImpsDataAccessor myAc( iSnd );
    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey

    // Add TImpsTransactionMode
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransModeElements,
        sizeof( KTransModeElements ) /
        sizeof( KTransModeElements[0] ) );
    myAc.StoreIntegerL( myKey, EImpsRequest );
    myKey->Reset();
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
    myKey->AddL( CREATEKEY( EImpsKeyLogout_Request, 0 ) );
    myAc.StoreEmptyL( myKey );

    CleanupStack::PopAndDestroy( 1 );   // >>> myKey

    DoSendSingleL( &myAc, aTID, EImpsLogoutReq );
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoMakeSapResponseL
// -----------------------------------------------------------------------------
//
void CImpsCSPSession::DoMakeSapResponseL( CTransaction* aTrans )
    {
    TPtrC tid;
    CImpsFields* fields = aTrans->iFields;
    tid.Set( aTrans->iTID );
    TInt messageType = fields->MessageType();

    TInt responseType = EImpsStatus;
    // Copy values from the new message to a response
    iSnd->Reset();
    iSnd->SetSessionIdL( fields->SessionIdL() );
    iSnd->SetTidL( fields->TidL() );
    iSnd->SetSessionTypeL( EImpsInband );

#ifndef _NO_IMPS_LOGGING_
    TPtrC tid2;
    tid2.Set( fields->TidL() );
    CImpsClientLogger::Log( _L( "CSPSession: MakeSAPresp SAP TID=%S" ), &( tid2 ) );//lint !e525
#endif

    CImpsDataAccessor* myAccess = CImpsDataAccessor::NewL( iSnd );
    CleanupStack::PushL( myAccess );    // << myAccess
    CImpsKey* myKey = CImpsKey::NewLC();  // <<< myKey

    CImpsDataAccessor* myAcRcv = CImpsDataAccessor::NewL( fields );
    CleanupStack::PushL( myAcRcv );    // << myAcRcv

    TPtrC myId;

    switch ( messageType )
        {
        case EImpsNewMessage:
            responseType = EImpsMessageDelivered;
            iSnd->SetMessageType( responseType );
            // Get messageId from received message
            TImpsSDataUtils::GetNewMessageIdL( myKey, myAcRcv, myId );
            // ... and copy data to outgoing response
            TImpsSDataUtils::SetMessageDeliveredIdL(
                myKey, myAccess, myId );
            break;
        default:
            iSnd->SetMessageType( responseType );
            TImpsSDataUtils::SetResultStatusL( myKey, myAccess, KImpsStatusOk );
            break;
        }

    CleanupStack::PopAndDestroy( 1 );   // >> myAcRcv

    // Add TImpsTransactionMode now
    myKey->Reset();
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransModeElements,
        sizeof( KTransModeElements ) /
        sizeof( KTransModeElements[0] ) );
    myAccess->StoreIntegerL( myKey, EImpsResponse );
    CleanupStack::PopAndDestroy( 1 );   // >>> myKey

    // -------------------------------------------
    // Handle orphan message
    if ( aTrans->iOrphan )
        {
        TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
        TRAPD( errx, iServer.HandleOrphanL( aTrans->iFields, csp ) );
        if ( !errx )
            {
            // ownership of the msg is in CImpsServer now
            aTrans->iFields = NULL;
            }
        }
    // -------------------------------------------

    // DoSendSingleL leaves if GPRS suspended,
    // so don't delete entry too early
    DoSendSingleL( myAccess, tid, ( TImpsMessageType )responseType );


    // Save TID into tid-sap-history so that we so not send the response twicw
    // even if SAP would send the transaction multiple times.
    iTidSapHistory->Add( tid );

    CleanupStack::PopAndDestroy( 1 );   // >> myAccess
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoStartIpCIRL
// -----------------------------------------------------------------------------
//
void CImpsCSPSession::DoStartIpCIRL()
    {
    // Start ip-cir-watcher
    if ( iVariant.IsFeatureSupportedL( EIpCirStandalone ) &&
         iSettings.iTCPSCIR == 2 || iSettings.iUDPSCIR == 2 )
        {
        // Check which CIR bindings are negotiated for CSP session
        TUint ipChannels = 0;
        if ( iSettings.iTCPSCIR == 2 )
            {
            ipChannels = KImpsCirTcp;
            }
        if ( iSettings.iUDPSCIR == 2 )
            {
            ipChannels = ipChannels | KImpsCirUdp;
            }
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: Starting ip-cir-watcher %d" ),
                                ipChannels );
#endif
        iCSPSessionId8.Copy( iCSPSessionId );
        if ( !iCirManager )
            {
            iCirManager = CImpsCirManager::NewL( *this,
                                                 CActive::EPriorityStandard );
            }
        iCirManager->OpenChL( ipChannels, iReceiver2->ConnManagerHandleL(),
                              iTCPPort, *iTCPAddr, iCSPSessionId8 );
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::IncreasePollTime
// -----------------------------------------------------------------------------
//
void CImpsCSPSession::IncreasePollTime()
    {
    // Increase poll timer. Two modes; CIR Mode and other modes.
    if ( iPollState == EImpsPollCIR )
        {
        ResetPollTime();
        }
    else if ( iPollTime < iSettings.iMaxPollTime && iIdleTimer->IsActive() )
        {
        iPollTime += KImpsPollIncrement;
        iIdleTimer->Start( iPollTime );
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: PollTime increased to %d" ), iPollTime );
#endif
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::ResetPollTime
// -----------------------------------------------------------------------------
//
void CImpsCSPSession::ResetPollTime()
    {
    // Reset poll timer. Two modes; CIR Mode and other modes.
    if ( iPollState == EImpsPollCIR )
        {
        if ( iPollTime != iSettings.CIRModePollTime() )
            {
            iPollTime = iSettings.CIRModePollTime();
            iIdleTimer->Start( iPollTime );
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: PollTime reset %d" ), iPollTime );
#endif
            }
        }
    else if ( iPollTime > iSettings.iPollTime && iIdleTimer->IsActive() )
        {
        iPollTime = iSettings.iPollTime;
        iIdleTimer->Start( iPollTime );
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CSPSession: PollTime reset %d" ), iPollTime );
#endif
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::IsLogged
// -----------------------------------------------------------------------------
//
TBool CImpsCSPSession::IsLogged(  )
    {
    return DoIsLogged();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::IsPendingLogin
// -----------------------------------------------------------------------------
//
TBool CImpsCSPSession::IsPendingLogin()
    {
    return iCSPState == EImpsCSPLogging ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::IsNegotiated
// -----------------------------------------------------------------------------
//
TBool CImpsCSPSession::IsNegotiated(  )
    {
    return DoIsNegotiated();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::Servicses
// -----------------------------------------------------------------------------
//
TImpsServices* CImpsCSPSession::Services(  )
    {
    return &iServices;  //lint !e1536 this pointer must be used with care
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::IsShuttingDown
// -----------------------------------------------------------------------------
//
TBool CImpsCSPSession::IsShuttingDown(  )
    {
    return DoIsShuttingDown();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::IsLogging
// -----------------------------------------------------------------------------
//
TBool CImpsCSPSession::IsLogging(  )
    {
    return DoIsLogging();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::UserId
// -----------------------------------------------------------------------------
//
TPtrC CImpsCSPSession::UserId(  )
    {
    return DoUserId();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::SAP
// -----------------------------------------------------------------------------
//
TPtrC CImpsCSPSession::SAP(  )
    {
    return DoSAP();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::SID
// -----------------------------------------------------------------------------
//
TPtrC CImpsCSPSession::SID(  )
    {
    return iCSPSessionId;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::Password
// -----------------------------------------------------------------------------
//
TPtrC CImpsCSPSession::Password(  )
    {
    return LogPwd();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::GiveNextRequestL
// -----------------------------------------------------------------------------
CReq* CImpsCSPSession::GiveNextRequestL()
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: GiveNextRequestL begins" ) );
#endif
    // If logginOff then this should be logout request
    TDblQueIter<CReq> iter ( iRequestList );
    iter.SetToFirst();

    while ( iter )
        {
        CReq* request = iter;
        // Get first and check if it is expired
        iter++;

        TTime myTime;
        myTime.HomeTime();
        if ( request->iExpiry < myTime )
            {
            // session is asked to discard request from its structures
            // Check TID match
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "Server: EXPIRED entry to be removed" ) );
#endif
            TImpsMessageType msgType = ( TImpsMessageType ) request->iFields->MessageType();
            TImpsEventType myType = impsService( &iVariant, msgType );
            // expired request is destroyed in client sessions too.
            TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
            iServer.DiscardRequest( request->iFields->TidL(), myType, KErrTimedOut, csp );

            // expired entry is deleted
            request->Destroy();
            TrMinus();

            // We want to push new messages from queued
            SendAllQueued();

            }  // expired
        else
            {
            return request;
            }
        }

    return ( CReq* )NULL;
    }


// -----------------------------------------------------------------------------
// CImpsCSPSession::NewState
// -----------------------------------------------------------------------------
void CImpsCSPSession::NewState( TImpsCSPState aNew )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: NewState old=%d new=%d csp=%d" ),
                            iCSPState, aNew, ( TInt )this );
#endif
    // In  EImpsCSPDisconnecting iLogTid is still needed fore cancel-login
    if ( iCSPState >= EImpsCSPLogging && aNew < EImpsCSPDisconnecting )
        {
        delete iLogTid;
        iLogTid = NULL;
        }
    if ( aNew == EImpsCSPIdle )
        {
        iTr = 0;
        iCookie = KNullDesC;
        iCookie8 = KNullDesC8;
        iCSPSessionId = KNullDesC;
        iCSPSessionId8 = KNullDesC8;
        iLoginPhase = 1;
        }
    else if ( aNew <= EImpsCSPOnLine )
        {
        // PDP is opened sucessfully or CSP login not done or logging out.
        iPDPOpenTimer->Stop();
        }
    iCSPState = aNew;
    }


// -----------------------------------------------------------------------------
// CImpsCSPSession::TrMinus
// -----------------------------------------------------------------------------
void CImpsCSPSession::TrMinus()
    {
    if ( iTr > 0 )
        {
        iTr--;
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: iTr=%d csp=%d" ), iTr, ( TInt )this );
#endif
    }


// -----------------------------------------------------------------------------
// CImpsCSPSession::ResetSession
// -----------------------------------------------------------------------------
void CImpsCSPSession::ResetSession()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: ResetSession csp=%d" ), ( TInt )this );
#endif
    // This is not needed since CSP session entity is not re-cycled?
    iSettings.iUDPWAPCIR = 1;
    iSettings.iSMSWAPCIR = 1;
    iSettings.iTCPSCIR = 1;
    iSettings.iUDPSCIR = 1;
    iSettings.iPollTime = KImpsPollTime;
    }


// -----------------------------------------------------------------------------
// CImpsCSPSession::MaxParserSize
// -----------------------------------------------------------------------------
TInt CImpsCSPSession::MaxParserSize()
    {
    return iServer.Settings()->MaximumParserSize();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::MaxParserSize
// -----------------------------------------------------------------------------
TInt CImpsCSPSession::MaxMessageSize()
    {
    return iServer.Settings()->MaximumMessageSize();
    }

// -----------------------------------------------------------------------------
// CTransaction::CTransaction
// -----------------------------------------------------------------------------

CTransaction::CTransaction(
    TBool aOrphan,
    const TDesC& aTID,
    const TDesC& aSessionId,
    TImpsMessageType aMessageType,
    CImpsFields* aFields ) :
        iMessageType( aMessageType ),
        iFields ( aFields ),
        iOrphan ( aOrphan )
    {
    iTID = aTID;
    iSessionId = aSessionId;
    }

CTransaction::~CTransaction()
    {

    } //lint !e1540 iFields freed in Destroy

void CTransaction::Destroy()
    {
    iLink.Deque();
    delete iFields;
    iFields = NULL;
    delete this;
    }


// -----------------------------------------------------------------------------
// CReq::CReq
// -----------------------------------------------------------------------------

CReq::CReq( CImpsFields* aFields ) :
        iFields( aFields )
    {}

CReq::~CReq()
    {

    }   //lint !e1540 iFields freed in Destroy

void CReq::Destroy()
    {
    delete iFields;
    iFields = NULL;
    iLink.Deque();
    delete this;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::IsLogoutResp()
// aFields must not be NULL
// -----------------------------------------------------------------------------
TImpsLogoutTrans CImpsCSPSession::IsLogoutResp( CImpsFields* aFields )
    {
    TImpsLogoutTrans ret = EImpsLogoutNone;

    // set pointers to received TID and to current pending Logout-request TID
    TPtrC recId;
    TPtrC logoutTid;
    logoutTid.Set( LogoutTID() );
    TRAPD( errxy, recId.Set( aFields->TidL() ) );
    if ( errxy )
        {
        return ret;
        }

    // Check if current logout transaction TID matches with received message.
    if ( iCSPState == EImpsCSPDisconnecting &&
         !logoutTid.Length() || ( logoutTid.Length() && !recId.Compare( logoutTid  ) ) )  // NOTE
        {
        if ( aFields->MessageType() == EImpsDisconnect )
            {
            // WV 1.1 logout response
            ret = EImpsLogoutTerminal;
            }
        else if ( aFields->MessageType() == EImpsStatus )
            {
            // WV 1.2 logout response
            ret = EImpsLogoutTerminal;
            }
        }
    else if ( aFields->MessageType() == EImpsDisconnect )
        {
        // SAP initiated CSP disconnect
        ret = EImpsLogoutSAP;
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: IsLogoutResp returns %d csp=%d" ),
                            ret, ( TInt )this );
#endif
    return ret;
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::StartLoginCancel()
// -----------------------------------------------------------------------------
void CImpsCSPSession::StartLoginCancel( )
    {
    // Cancel login transaction, either login+logout or login+cancelLogin.
    // DoPendingLogout() is called later to complete the client requests.
    if ( iReceiver2->CancelLoginTrans( LogTid() ) )
        {
        // Generate CSP Logout transaction
        // Clear buffers from old stuff
        DeleteRequests();
        DeleteTransactions();
        // Send Logout primitive and set state to EImpsCSPDisconnecting.
        TRAPD( errx, DoSendLogoutL( LogoutTID() ) );
        if ( errx )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CSPSession: DoSendLogoutL 2 error %d ***" ), errx );
#endif
            }
        else
            {
            // If CSP Logout transaction is sent then we wait TransportResponseL or
            // TransportErrorL callback methods to be called later.
            return;
            }
        }
    // Start to send events to clients if CSP Disconnect transaction is not sent.
    DoPendingLogout();
    }

// -----------------------------------------------------------------------------
// CImpsCSPSession::DoPendingLogout()
// -----------------------------------------------------------------------------
void CImpsCSPSession::DoPendingLogout( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CSPSession: DoPendingLogout ***" ) );
#endif
    // Send critical response events to client sessions and start to logout
    // internally this CSP session entity.
    TImpsSessIdent csp( iCSPSessionId, DoSAP(), DoUserId() );
    if ( iPendingLogout )
        {
        // Send login response for pending request first
        iServer.DiscardRequest( LogTid(), EImpsEventServerLogin, KErrCancel, csp );
        // Send logout response, that is always successful
        iServer.DiscardLogout( LogoutTID(), csp );
        }

    if ( iCancelLogin  )
        {
        // Send cancel-login response to client
        iServer.DiscardRequest( LogTid(), EImpsEventServerLogin, KErrNone, csp );
        }

    iPendingLogout = EFalse;
    iCancelLogin = EFalse;
    DoLogout();
    }

//  End of File
