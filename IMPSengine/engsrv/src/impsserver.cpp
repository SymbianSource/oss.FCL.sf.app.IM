/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Classes for IM server and Conn.
*
*
*/



// INCLUDE FILES
#include    <e32math.h>
#include    <ecom/ecom.h>
#include    "impsserver.h"
#include    "impssession.h"
#include    "impssubsession.h"
#include    "impsfields.h"
#include    "impsutils.h"
#include    "impserrors.h"
#include    "impstimer.h"
#include    "impsservices.h"
#include    "impscommonenums.h"
#include    "impssdatautils.h"
#include    "impsmessageinterpreterapi.h"
#include    "ImpsVariantAPI.h"
#include    "impscspsession.h"
#include    "impsorphans.h"
#include    "impstoffobserver.h"
#include    "WVEngineInternalCRKeys.h"


#include    "impsserversecuritypolicy.h"


// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// CONSTANTS
// Shutdown idle wait time in seconds
const TInt KImpsShutdownTime = 0;
// Overhead estimate for XML message : total - transaction content
const TInt KImpsBufOverhead = 100;

// ================= MEMBER FUNCTIONS =======================

void CImpsScheduler::Error( TInt anError ) const
    {
    // This is just for compiler to avoid warning
    TInt err = anError;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Scheduler: error=%d" ), anError );
#endif
    // This is just for another compiler to avoid warning
    anError = err;
    }

// -----------------------------------------------------------------------------
// CLASS CImpsConn
// -----------------------------------------------------------------------------
CImpsConn::CImpsConn( MImpsCSPSession* aSess ) :
        iSess( aSess ), iRemover( NULL )
    {}

CImpsConn::~CImpsConn()
    {

    }   //lint !e1540 iSess freed in Destroy

// Two-phased constructor.
CImpsConn* CImpsConn::NewL( MImpsCSPSession* aSess, CImpsServer& aServer )
    {
    CImpsConn* self = new ( ELeave ) CImpsConn( aSess );
    CleanupStack::PushL( self );
    self->ConstructL( aServer );
    CleanupStack::Pop();
    return self;
    }

// Two-phased constructor.
// It creates an active object to destroy CSP session asynchronously later.
// It's better to create it now to avoid OOM errors in CSP closing.
void CImpsConn::ConstructL( CImpsServer& aServer )
    {
    iRemover = new ( ELeave ) CImpsCSPDestroyer(
        aServer, *iSess, CActive::EPriorityUserInput + 1 );
    }

// -----------------------------------------------------------------------------
// CImpsConn::Destroy
// -----------------------------------------------------------------------------
void CImpsConn::Destroy()
    {
    iRemover->Cancel();
    delete iRemover;
    iSess->Destroy();
    delete ( CImpsCSPSession* )iSess;
    iSess = NULL;
    iLink.Deque();
    delete this;
    }

// -----------------------------------------------------------------------------
// CImpsConn::Sess
// -----------------------------------------------------------------------------
MImpsCSPSession* CImpsConn::Sess()
    {
    return iSess;
    }

// -----------------------------------------------------------------------------
// CImpsConn::DeleteCSPAsynch
// -----------------------------------------------------------------------------
void CImpsConn::DeleteCSPAsynch()
    {
    // iRemover cannot be NULL since it's created in contructor
    if ( iRemover->IsActive() )
        {
        // asynchronous request already runnig, nothing to do
        return;
        }
    iRemover->DeleteCSPAsynch( );
    }

// -----------------------------------------------------------------------------
// CImpsConn::MatchCSP
// -----------------------------------------------------------------------------
TBool CImpsConn::MatchCSP( TImpsSessIdent aCSP )
    {
    if ( ( aCSP.UserId().Compare( iSess->UserId() ) ) ||
         ( aCSP.SAP().Compare( iSess->SAP() ) ) )
        // Notice: no need for this
        // ( aCSP.SID().Length() && aCSP.SID().Compare( iSess->SID() )))
        {
        return EFalse;
        }
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CLASS CImpsCSPDestroyer
// -----------------------------------------------------------------------------
CImpsCSPDestroyer::CImpsCSPDestroyer(
    CImpsServer& aServer, MImpsCSPSession& aSess, TInt aPriority )
        : CActive( aPriority ),
        iServer( aServer ),
        iSess( aSess ),
        iCanceled ( EFalse )
    {
    // Add this to the scheduler
    CActiveScheduler::Add( this );
    }

CImpsCSPDestroyer::~CImpsCSPDestroyer()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CImpsCSPDestroyer::DeleteCSPAsynch
// -----------------------------------------------------------------------------
void CImpsCSPDestroyer::DeleteCSPAsynch(  )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCSPDestroyer: DeleteCSPAsynch" ) );
#endif
    // This yields the control to the server thread active scheduler
    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        iCanceled = EFalse;
        TRequestStatus* s = &iStatus;
        User::RequestComplete( s, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CImpsCSPDestroyer::DoCancel
// -----------------------------------------------------------------------------
void CImpsCSPDestroyer::DoCancel()
    {
    iCanceled = ETrue;
    }

// -----------------------------------------------------------------------------
// CImpsCSPDestroyer::RunL
// -----------------------------------------------------------------------------
void CImpsCSPDestroyer::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCSPDestroyer: RunL" ) );
#endif
    if ( iCanceled || iStatus.Int() != KErrNone )
        {
        return;
        }
    // Trigger a server thread to send a queued message
    iServer.DeleteCSP( &iSess );
    // It is extremely important that after the previous call this
    // method does nothing since DeleteCSP actully deletes the
    // current entity if this class!
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsCSPDestroyer: RunL ends" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CLASS CImpsServer
// -----------------------------------------------------------------------------
CImpsServer::CImpsServer()
        : CPolicyServer( EPriorityUserInput /*EPriorityHigh*/, KImpsServerPolicy ),
        iFs(),
        iSettings( ),
        iConAllowed( ETrue ),
        iCSPVersion( EImpsCspVersion11 ),
        iOperation( EImpsSrvNone ),
        iOrphans( NULL ),
        iCSPList( _FOFF( CImpsConn, iLink ) ), //lint !e413,
        iObserver( NULL ),
        iExpiryInterval( 0 )
    {
    }

// Create and start a new server.

CImpsServer* CImpsServer::New()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: rel200614.4+" ) );
#endif

    CImpsServer* server = new CImpsServer();
    if ( server )
        {
        if ( server->Start( KImpsServerName ) != KErrNone )
            {
            delete server;
            server = NULL;
            }
        }

#ifdef _DEBUG
    if ( server && server->iFs.Connect() )
        {
        delete server;
        server = NULL;
        }
#endif // _DEBUG               

    return server;
    }

CImpsServer::~CImpsServer()
    {
    // deallocate all memory

    delete iSnd;
    delete iShutTimer;
    delete iExpiryTimer;

    delete iContainerIndex;
    delete iVariant;
    delete iOrphans;
    delete iObserver;

#ifdef _DEBUG
    // close file session
    iFs.Close();
#endif // _DEBUG      
    }

// -----------------------------------------------------------------------------
// CImpsServer::ConstructL()
// -----------------------------------------------------------------------------
void CImpsServer::ConstructL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server:ConstructL begins" ) );
#endif

    iContainerIndex = CObjectConIx::NewL();

    // Read static settings, use defaults if ini file not exist
    iSettings.ReadStaticSettings();
#ifdef _DEBUG
    TInt errx = KErrNone;
    TRAP( errx, iSettings.ReadIniFileL( iFs ) );
#endif

    iVariant = CImpsVariant::NewLC( );
    CleanupStack::Pop( ); // >> iVariant

    iSnd = CImpsFields::NewL();
    iShutTimer = new ( ELeave ) CImpsShutdownTimer( *this,
                                                    CActive::EPriorityUserInput + 1 );
    iExpiryTimer = new ( ELeave ) CImpsExpiryTimer( *this,
                                                    EImpsEventAll, CActive::EPriorityStandard );

    // Get the supported WV CSP version for all sessions
    GetCSPVersion();

    // local variation
    //----------------------------
    if ( iVariant->IsFeatureSupportedL( EImLauncher ) )
        {
        iOrphans = CImpsOrphans::NewL();
        }
    //----------------------------

    // Check terminal off line state
    iObserver = CImpsTOffObserver::NewL( *this );
    if ( !iObserver->CheckConnAllowed() )
        {
        SetConnAllowed( EFalse );
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server:ConstructL ends" ) );
#endif

    }

// -----------------------------------------------------------------------------
// CImpsServer::NewSessionL()
// -----------------------------------------------------------------------------
CSession2* CImpsServer::NewSessionL(
    const TVersion &aVersion, const RMessage2& aMessage ) const
    {
    // check that we're the right version
    TVersion v( KImpsServMajorVersionNumber,
                KImpsServMinorVersionNumber,
                KImpsServBuildVersionNumber );
    if ( !User::QueryVersionSupported( v, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    if ( iOperation == EImpsSrvShuttingDown )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: NewSessionL in shut down ***" ) );
#endif
        User::Leave( KImpsErrorShuttingDown );
        }

    StopTimer();

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: NewSessionL ends" ) );
#endif

    // make a new session
    RMessage2 msg = aMessage;
    return new( ELeave ) CImpsSession( msg );
    }

// -----------------------------------------------------------------------------
// CImpsServer::CloseSession()
// -----------------------------------------------------------------------------
void CImpsServer::CloseSession( TImpsSessIdent aCSP, TBool aComplete )
    {

#ifndef _NO_IMPS_LOGGING_
    TInt temp1 = NbrSessions( ETrue );
    TInt temp2 = NbrSessions( EFalse );
    CImpsClientLogger::Log( _L( "Server: CloseSession nbrses=%d-%d aComplete=%d" ), temp1, temp2, aComplete );
#endif

    // CServer2 calls CImpsSession's desctrutor if its consctrutor fails
    // but in that case NbrSessions() method does not see the failed entity.
    // Thus in case of failed session creation the limit is 0, otherwise 1,
    // i.e. the last client session to be closed starts the shut down timer.
    TInt limit = aComplete ? 1 : 0;
    MImpsCSPSession* ses = GetCSP( aCSP, ETrue );
    if ( !ses )
        {
        // If the last client session and no CSP sessions
        // then start final count down
        if ( NbrSessions( EFalse ) <= limit )
            {
            // There is nobody listening to expiry events
            iExpiryTimer->Stop( );
            if ( !NbrCSPs() )
                {
                // start the final count down
                iOperation = EImpsSrvFinish;
                iShutTimer->Start( KImpsShutdownTime );
                }
            }
        return;
        }

    // This is very last client session for the CSP connection.
    // Disconnect CSP session on behalf of applications in this case.
    TInt myCount = NbrSessions( EFalse, aCSP );
    if ( myCount == 1 )
        {
        TRAPD( errx, ( void )ses->LogoutL( EFalse ) );
        if ( !errx )
            {
            return;
            }
        }

    }

// -----------------------------------------------------------------------------
// CImpsServer::ThreadStart()
// -----------------------------------------------------------------------------
EXPORT_C TInt CImpsServer::ThreadStart( TImpsSignal& aSignal )
    {
    // naming the server thread after the server helps to debug panics
    RThread().RenameMe( KImpsServerExe );

    TInt err = KErrNone;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: ThreadStart begins" ) );
    CImpsClientLogger::Log( _L( "Server: aSignal.iRate = %x" ), aSignal.iRate );
#endif
    __UHEAP_MARK;

    CTrapCleanup* cleanup = NULL;
    CImpsScheduler* scheduler = NULL;
    CImpsServer* server = NULL;
    cleanup = CTrapCleanup::New();

#ifdef _DEBUG
    TInt rate = aSignal.iRate;
    if ( rate > 0 )
        {
        TRAP( err, __UHEAP_SETFAIL( RHeap::EFailNext, rate ) );
        }
#endif

    scheduler = new CImpsScheduler;

    if ( cleanup && scheduler )
        {
        CImpsScheduler::Install( scheduler );
        server = CImpsServer::New();      // adds server in scheduler
        }

    if ( !cleanup || !scheduler || !server )
        {
        err = KErrNoMemory;
        }
    else
        {
        TRAP( err, server->ConstructL() );
        }

    if ( err )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server:ConstructL failed = %d" ), err );
#endif
        delete server;      //lint !e644 initialization ok
        delete cleanup;     //lint !e644 initialization ok
        delete scheduler;   //lint !e644 initialization ok
        }

    // signal that we've started
    RProcess::Rendezvous( err );

    // start fielding requests from clients
    if ( err == KErrNone )
        {
        CImpsScheduler::Start();

        // comes here when server shuts down

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: Scheduler is STOPPED" ) );
#endif
        delete server;      //lint !e644 initialization ok
        delete scheduler;   //lint !e644 initialization ok
        delete cleanup;     //lint !e644 initialization ok
        }

#ifdef _DEBUG
    if ( rate > 0 )
        {
        TInt err2 = KErrNone;
        TRAP( err2, __UHEAP_SETFAIL( RHeap::ENone, 0 ) );
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: MEM TEST begins" ) );
#endif
    __UHEAP_MARKEND;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: MEM TEST OK" ) );
#endif
#endif

    return( err );
    }

// -----------------------------------------------------------------------------
// CImpsServer::LoginL
// -----------------------------------------------------------------------------
TPtrC CImpsServer::LoginL(
    const TDesC& aUser,
    const TDesC& aPassword,
    const TDesC& aClientId,
    const TDesC& aSAP,
    TUint32 aAP,
    const TDesC& aKey1,
    const TDesC& aKey2,
    CImpsSubSession* /*aSub*/,
    const TTime aLoginExpiry,
    TBool aReactive )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: LoginL" ) );
#endif

    if ( iOperation == EImpsSrvShuttingDown )
        {
        User::Leave( KImpsErrorShuttingDown );
        }

    // multi: check if CSP already exists, otherwise create a new CSP session
    TImpsSessIdent csp( KNullDesC, aSAP, aUser );
    MImpsCSPSession* ses = GetCSP( csp, EFalse );
    // multi: Check if another client is logging with this CSP session
    if ( ses && ses->IsLogging() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: CSP is logging ****" ) );
#endif
        }
    else if ( !ses  )
        {
        ses = NewCSPL( csp, iCSPVersion );
        }

    // Before starting actual transport data transfer we start expiry timer
    SetExpiryTimer( 0, ETrue );

    // This leaves if already logged or if performing logging currently
    // Certain error types means that CSP sessions is already doing something,
    // rest of the error codes means that initialization has been failed and
    // thus the entity has to deleted
    TPtrC p( KNullDesC );
    TRAPD( errx, p.Set( ses->LoginL( aUser, aPassword, aClientId, aSAP, aAP,
                                     aKey1, aKey2, aLoginExpiry, aReactive ) ) );
    if ( errx )
        {
        // These are means that CSP entity is running and in use, don't delete it yet.
        if ( errx != KErrNotReady && errx != KImpsErrorAlreadyLogged )
            {
            DeleteCSPAsynch( ses );
            }
        User::Leave( errx );
        }
    return p;
    }

// -----------------------------------------------------------------------------
// CImpsServer::LogoutL
// -----------------------------------------------------------------------------
TPtrC CImpsServer::LogoutL(
    TBool aCancel,
    TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: LogoutL" ) );
#endif

    // Just leave if other client sessions use still this CSP session
    // or if server is shutting down.
    // KImpsErrorTerminalOffLine is handled in a special way in a client session class.
    // That's because of Logout should not fail if sessions existed.
    //
    if ( NbrSessions( ETrue, aCSP ) > 1 ||
         iOperation == EImpsSrvShuttingDown )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: LogoutL LEAVES 1 KImpsErrorTerminalOffLine  ***" ) );
#endif
        User::Leave( KImpsErrorTerminalOffLine );
        }

    // Leave also if there is no CSP session or CSP session state is improper
    MImpsCSPSession* ses = GetCSP( aCSP, EFalse );
    if ( !ses || ( !IsLogged( aCSP, NULL ) && !IsPendingLogin( aCSP ) ) )
        {
        // The following is not needed:
        //   Send error messages for existing requests
        //   DiscardRequests( EImpsEventAll, KImpsErrorNotLogged, aCSP );
        // This error code is handled in a special way in a sub-session.
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: LogoutL LEAVES 2 KImpsErrorTerminalOffLine  ***" ) );
#endif
        User::Leave( KImpsErrorTerminalOffLine );
        }

    // Be 100% sure that expiry timer is running
    if ( !iExpiryTimer->IsActive() )
        {
        TInt expiryInterval = iSettings.iAccessExp;
        iExpiryTimer->Start( expiryInterval / 2 );
        }

    return ses->LogoutL( aCancel );
    }

// -----------------------------------------------------------------------------
// CImpsServer::CirMessageL
// -----------------------------------------------------------------------------
void CImpsServer::CirMessageL(
    const TDesC8& aCookie )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: CirMessageL" ) );
#endif

    // Call Session

    // multi: scan all sessions until session cookie matches
    TDblQueIter<CImpsConn> rIter( iCSPList );
    rIter.SetToFirst();
    while ( rIter )
        {
        CImpsConn* conn = rIter;
        rIter++;
        TRAPD( errx, conn->Sess()->CirMessageL( aCookie ) );
        if ( !errx )
            {
            // If no error then the CSP session was a right one
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::SendDataL
// aSession MUST NOT be NULL
// -----------------------------------------------------------------------------
TPtrC CImpsServer::SendDataL(
    CImpsSubSession* aSession, TImpsSessIdent aCSP )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: SendDataL begins" ) );
#endif

    // multi:
    MImpsCSPSession* ses = GetCSP( aCSP, EFalse );
    if ( !ses )
        {
        User::Leave( KImpsErrorNotLogged );
        }

    TInt expiryTime = aSession->ExpiryTime();

    // iSnd is for own creations, but this is created in a session
    CImpsFields* fields = NULL;
    fields = aSession->ImpsFields();

    __ASSERT_DEBUG( fields, User::Panic( KImpsPanicCategory, EImpsCorrupted ) );

    TBool ownerCh( EFalse );
    TPtrC ptr = ses->SendDataL( fields, expiryTime, ownerCh );
    if ( ownerCh )
        {
        // Transport adapter is busy so the request is queued and
        // data owner is changed
        aSession->NewFieldsL();
        }
    return ptr;
    }

// -----------------------------------------------------------------------------
// CImpsServer::StopTimer
// -----------------------------------------------------------------------------
void CImpsServer::StopTimer() const
    {
    iShutTimer->Stop();
    }

// -----------------------------------------------------------------------------
// CImpsServer::CheckRequestsL
// -----------------------------------------------------------------------------
void CImpsServer::CheckRequestsL(
    CImpsFields* aFields,
    TBool& aFound,
    TImpsSessIdent aCSP )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: CheckRequests begins " ) );
#endif

    aFound = EFalse;

    // Block some message types for push message, not MO CSP transactions,
    // to speed up performance
    // Notice: update list as needed
    TInt messageType = aFields->MessageType();
    if ( messageType == EImpsNewMessage ||
         messageType == EImpsDeliveryReportReq ||
         // This may be requested or SAP initiated
         // so, don't do that :messageType == EImpsLeaveGroupRes ||
         messageType == EImpsInviteUserReq ||
         messageType == EImpsInviteRes ||
         messageType == EImpsCancelInviteUserReq ||
         messageType == EImpsPresenceNotification ||
         messageType == EImpsGroupChangeNotice )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: CheckRequests cancelled" ) );
#endif
        return;
        }

    // Go through all sessions (each having an own request list)

    iSessionIter.SetToFirst();
    TInt myOpId = 0;
    TImpsServRequest reqType = EImpsServNone;
    TImpsMessageType msgType = EImpsMessageNone;

    TUint subHandle = 0;

    // Check TID match
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        // return handle of matching subsession and give it as parameter
        subHandle = ( ( CImpsSession* )session )->CheckRequests( aFields->TidL(),
                                                                 myOpId, reqType, msgType, aCSP );
        if ( subHandle )
            {
            // Do not delete request until corresponding event is sent.
            // If event sending fails then expiration will take place later.
            ( ( CImpsSession* )session )->SendEvent(
                aFields, myOpId, reqType, msgType, subHandle );

            ( ( CImpsSession* )session )->DeleteRequest( myOpId, subHandle );
            aFound = ETrue;
            // multi: do not stop searching because of there may be
            // multiple login requests with the same TID.
            // break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::CheckNotifications
// -----------------------------------------------------------------------------
TBool CImpsServer::CheckNotifications(
    CImpsFields* aFields,
    TImpsSessIdent aCSP )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: CheckNotifications begins" ) );
#endif

    // Check if IM message max size is exceeded
    if ( aFields->MessageType() == EImpsNewMessage &&
         aFields->Size() > iSettings.MaximumMessageSize() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: error: IM Message max size exceeded ***" ) );
#endif
        // This is not saved in an orphan queue either
        return ETrue;
        }

    TBool retVal( EFalse );
    // Go through all sessions
    iSessionIter.SetToFirst();
    // Check notification events
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        if ( ( ( CImpsSession* )session )->CheckNotifications( aFields, aCSP ) )
            {
            retVal = ETrue;
            }
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CImpsServer::CheckExpiryL
// -----------------------------------------------------------------------------
void CImpsServer::CheckExpiryL(
    TImpsEventType aType, TTime aExpiry  )
    {
    if ( iOrphans )
        {
        TInt errx = KErrNone;
        TRAP( errx, iOrphans->CheckExpiryL() );
        }
    // multi: check all csp sessions
    TDblQueIter<CImpsConn> rIter( iCSPList );
    rIter.SetToFirst();
    while ( rIter )
        {
        CImpsConn* conn = rIter;
        rIter++;
        conn->Sess()->CheckExpiry(  aType, aExpiry );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::CheckExpiryL
// -----------------------------------------------------------------------------
void CImpsServer::CheckExpiryL(
    TImpsEventType aType  )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: CheckExpiryL begins" ) );
#endif
    TTime myExpiry;
    myExpiry.HomeTime();
    CheckExpiryL( aType, myExpiry );
    }

// -----------------------------------------------------------------------------
// CImpsServer::DiscardRequests
// -----------------------------------------------------------------------------
void CImpsServer::DiscardRequests( TImpsEventType aType, TTime aExpiry,
                                   TImpsSessIdent aCSP, MImpsCSPSession* aSess )
    {
    iSessionIter.SetToFirst();
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        ( ( CImpsSession* )session )->DiscardRequests(
            aExpiry,
            aType,
            aCSP, aSess );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::DiscardRequests
// -----------------------------------------------------------------------------
void CImpsServer::DiscardRequests( TImpsEventType aType, TInt aError,
                                   TImpsSessIdent aCSP, MImpsCSPSession* aSess )
    {
    iSessionIter.SetToFirst();
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        ( ( CImpsSession* )session )->DiscardRequests(
            aError,
            aType,
            aCSP,
            aSess );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::DiscardLogout
// -----------------------------------------------------------------------------
void CImpsServer::DiscardLogout(
    const TDesC& aTID,
    TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: DiscardLogoutL begins" ) );
#endif
    TUint subHandle = 0;
    // This is a special case to handle logout error
    // Check TID match
    iSessionIter.SetToFirst();
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        TInt myHit = 0;     // opId
        // output params
        TImpsServRequest reqType = EImpsServNone;
        TImpsMessageType msgType = EImpsMessageNone;

        // return handle of matching subsession and give it as parameter
        subHandle = ( ( CImpsSession* )session )->CheckRequests( aTID,
                                                                 myHit, reqType, msgType, aCSP );
        // call SendLogoutevent
        if ( subHandle )
            {
            // Send event to client
            ( ( CImpsSession* )session )->SendLogoutEvent(
                KErrNone, myHit, NULL, subHandle );

            // Delete client request after successful event sending
            ( ( CImpsSession* )session )->DeleteRequest( myHit, subHandle );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::HandleOrphanL
// -----------------------------------------------------------------------------
void CImpsServer::HandleOrphanL( CImpsFields* aFields, TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: HandleOrphanL" ) );
#endif
    if ( iOrphans )
        {
        // CSP identification
        iOrphans->NewOrphanL( aFields, aCSP );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::NextOrphanLC
// -----------------------------------------------------------------------------
CImpsFields* CImpsServer::NextOrphanLC( const TDesC& aCID,
                                        TImpsEventType aServiceType,
                                        TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: NextOrphanL" ) );
#endif
    if ( !iOrphans )
        {
        return NULL;
        }
    // CSP session and application id identification
    return iOrphans->NextOrphanLC( aCID, aServiceType, aCSP );
    }

// -----------------------------------------------------------------------------
// CImpsServer::HandleAllOrphans
// -----------------------------------------------------------------------------
void CImpsServer::HandleAllOrphans( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: HandleAllOrphans" ) );
#endif
    if ( !iOrphans )
        {
        return;
        }
    // loop each session
    iSessionIter.SetToFirst();
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        ( ( CImpsSession* )session )->HandleAllOrphans();
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::IsLogged
// -----------------------------------------------------------------------------
TBool CImpsServer::IsLogged( TImpsSessIdent aCSP, TPtrC* aPsw )
    {
    MImpsCSPSession* csp = GetCSP( aCSP, EFalse );
    TBool ret =  ( csp ? csp->IsLogged() : EFalse );
    if ( ret && aPsw )
        {
        aPsw->Set( csp->Password() );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CImpsServer::SID()
// -----------------------------------------------------------------------------
TPtrC CImpsServer::SID( TImpsSessIdent aCSP )
    {
    if ( aCSP.IsZero() )
        {
        return TPtrC();
        }
    else
        {
        MImpsCSPSession* csp = GetCSP( aCSP, EFalse );
        return ( csp ? csp->SID() : TPtrC() );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::GetCSP
// -----------------------------------------------------------------------------
MImpsCSPSession* CImpsServer::GetCSP( TImpsSessIdent aCSP, TBool aAll )
    {
    TDblQueIter<CImpsConn> rIter( iCSPList );
    rIter.SetToFirst();
    while ( rIter )
        {
        CImpsConn* conn = rIter;
        rIter++;
        if ( conn->MatchCSP( aCSP ) )
            {
            if ( ( !aAll && !conn->Sess()->IsShuttingDown() ) ||
                 ( aAll ) )
                {
                return conn->Sess();
                }
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CImpsServer::NewCSPL
// -----------------------------------------------------------------------------
MImpsCSPSession* CImpsServer::NewCSPL( TImpsSessIdent /*aCSP*/, TImpsCspVersion aVer )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: NewCSPL" ) );
#endif
    CImpsCSPSession* ses = CImpsCSPSession::NewL( *this,
                                                  iSettings, iFs, *iVariant, aVer );
    CleanupStack::PushL( ses );
    CImpsConn* conn = CImpsConn::NewL( ses, *this );
    iCSPList.AddLast( *conn );
    CleanupStack::Pop( 1 );
    return ses;
    }

// -----------------------------------------------------------------------------
// CImpsServer::DeleteCSP
// -----------------------------------------------------------------------------
void CImpsServer::DeleteCSP( MImpsCSPSession* aSess )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: DeleteCSP" ) );
#endif
    TDblQueIter<CImpsConn> rIter( iCSPList );
    rIter.SetToFirst();
    while ( rIter )
        {
        CImpsConn* conn = rIter;
        rIter++;
        if ( conn->Sess() == aSess )
            {
            conn->Destroy();
            break;
            }
        }
    // Check also if all the client sessions are closed and if this is the
    // last CSP session. Then start final count down.
    if ( !NbrSessions( EFalse ) && !NbrCSPs() )
        {
        iOperation = EImpsSrvFinish;
        iShutTimer->Start( KImpsShutdownTime );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::DeleteCSPAsynch
// -----------------------------------------------------------------------------
void CImpsServer::DeleteCSPAsynch( MImpsCSPSession* aSess )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: DeleteCSPAsynch" ) );
#endif
    TDblQueIter<CImpsConn> rIter( iCSPList );
    rIter.SetToFirst();
    while ( rIter )
        {
        CImpsConn* conn = rIter;
        rIter++;
        if ( conn->Sess() == aSess )
            {
            conn->DeleteCSPAsynch();
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::CloseAllCSPs
// -----------------------------------------------------------------------------
void CImpsServer::CloseAllCSPs()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: CloseAllCSPs" ) );
#endif
    TDblQueIter<CImpsConn> rIter( iCSPList );
    rIter.SetToFirst();
    while ( rIter )
        {
        CImpsConn* conn = rIter;
        rIter++;
        conn->Sess()->DoLogout();
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::NbrCSPs
// -----------------------------------------------------------------------------
TInt CImpsServer::NbrCSPs( )
    {
    TDblQueIter<CImpsConn> rIter( iCSPList );
    rIter.SetToFirst();
    TInt count = 0;
    while ( rIter )
        {
        rIter++;
        count++;
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CImpsServer::NbrSessions
// -----------------------------------------------------------------------------
TInt CImpsServer::NbrSessions( TBool aIgnoreCIR )
    {
    iSessionIter.SetToFirst();
    TInt myRet = 0;

    // Check TID match
    while ( iSessionIter )
        {
        if ( aIgnoreCIR )
            {
            CSession2* session = iSessionIter;
            iSessionIter++;
            if ( !( ( CImpsSession* )session )->IsCIRWatcherSession() )
                {
                myRet++;
                }
            }
        else
            {
            iSessionIter++;
            myRet++;
            }
        }
    return myRet;

    }

// -----------------------------------------------------------------------------
// CImpsServer::NbrSessions
// -----------------------------------------------------------------------------
TInt CImpsServer::NbrSessions( TBool aIgnoreCIR, TImpsSessIdent aCSP )
    {
    iSessionIter.SetToFirst();
    TInt myRet = 0;

    // Check TID match
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        if ( aIgnoreCIR )
            {
            if ( ( ( CImpsSession* )session )->MatchSession( aCSP ) &&
                 !( ( CImpsSession* )session )->IsCIRWatcherSession() )
                {
                myRet++;
                }
            }
        else if ( ( ( CImpsSession* )session )->MatchSession( aCSP ) )
            {
            myRet++;
            }
        }
    return myRet;

    }

// -----------------------------------------------------------------------------
// CImpsServer::CancelTrans
// -----------------------------------------------------------------------------
void CImpsServer::CancelTrans( const TDesC& aTID, TImpsSessIdent aCSP )
    {
    MImpsCSPSession* sess = GetCSP( aCSP, EFalse );
    CancelTrans( aTID, sess );
    }

// -----------------------------------------------------------------------------
// CImpsServer::CancelTrans
// -----------------------------------------------------------------------------
void CImpsServer::CancelTrans( const TDesC& aTID, MImpsCSPSession* aSess )
    {
    if ( aSess )
        {
        aSess->CancelTrans( aTID );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::DoLogoutSrv
// -----------------------------------------------------------------------------
void CImpsServer::DoLogoutSrv( TInt aInt, TBool aCSPDis,
                               MImpsCSPSession* aSess )
    {
    // aSess is never NULL
    TImpsSessIdent csp( KNullDesC, aSess->SAP(), aSess->UserId() );
    DoLogoutNow( NULL, aInt, aCSPDis, csp );
    }

// -----------------------------------------------------------------------------
// CImpsServer::DoLogoutNow
// -----------------------------------------------------------------------------
void CImpsServer::DoLogoutNow( CImpsSubSession* aSub,
                               TInt aOpId, TBool aCSPDis,
                               TImpsSessIdent aCSP )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: DoLogoutNow subses=%d opid=%d dis=%d" ),
                            aSub, aOpId, aCSPDis );
#endif

    iExpiryTimer->Stop();

    // Clean all pending requests from all sessions
    // Logout response must have beed delivered
    iSessionIter.SetToFirst();
    // Delete requests from the corresponding client sessions only.
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        ( ( CImpsSession* )session )->DeleteAllRequests( &aCSP );
        }

    // Call logout callback methods only if CSP did exist.
    if ( aCSPDis )
        {
        // send the appropriate opid to the requesting client
        SendLogoutEvents( aSub, aOpId, aCSP );
        }

    // Send not_logged state events. This also cleans WV CSP SID in client sessions
    // belonging to the CSP session.
    SendStatusEvents( EInternal_NOT_LOGGED, aCSP );

    // If no sessions then start countdown
    if ( !NbrSessions( EFalse ) && !NbrCSPs() )
        {
        iOperation = EImpsSrvFinish;
        iShutTimer->Start( KImpsShutdownTime );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::IsPendingLogin
// -----------------------------------------------------------------------------
TBool CImpsServer::IsPendingLogin( TImpsSessIdent aCSP )
    {
    MImpsCSPSession* csp = GetCSP( aCSP, EFalse );
    return ( csp ? csp->IsPendingLogin() : EFalse );
    }

// -----------------------------------------------------------------------------
// CImpsServer::ExpiryTime
// Generate default expiry time
// -----------------------------------------------------------------------------
TTime CImpsServer::ExpiryTime( TImpsMessageType aMsgType )
    {
    TImpsEventType srv = impsService( iVariant, aMsgType );
    return ExpiryTime( srv );
    }

// -----------------------------------------------------------------------------
// CImpsServer::ExpiryTime
// Generate default expiry time
// -----------------------------------------------------------------------------
TTime CImpsServer::ExpiryTime( TImpsEventType aSrv )
    {
    TInt extra = ExpirySeconds( aSrv );
    return ExpiryTime( extra );
    }

// ---------------------------------------------------------
// CImpsServer::ExpiryTime
// ---------------------------------------------------------
TTime CImpsServer::ExpiryTime( TInt aExpiry )
    {
    TTime time;        // time in microseconds since 0AD nominal Gregorian
    time.HomeTime();   // set time to now
    // add expiry seconds to the time
    TTimeIntervalSeconds expirySeconds( aExpiry );
    time += expirySeconds;
    return time;
    }

// -----------------------------------------------------------------------------
// CImpsServer::ExpirySeconds
// Generate default expiry time
// -----------------------------------------------------------------------------
TInt CImpsServer::ExpirySeconds( TImpsEventType aSrv )
    {
    TInt extra = 0;

    switch ( aSrv )
        {
        case EImpsEventServerLogin:
        case EImpsEventNone:        // used for PollRequest e.g.
            extra = iSettings.iAccessExp;
            break;
        case EImpsEventMessage:
            extra = iSettings.iImExp;
            break;
        case EImpsEventPresence:
        case EImpsEventPresencePure:
        case EImpsEventPure:
            extra = iSettings.iPrExp;
            break;
        case EImpsEventGroup:
            extra = iSettings.iGrExp;
            break;
        case EImpsEventCommon:
            extra = iSettings.iFuExp;
            break;
        default:
            break;
        }

    return extra;
    }

// -----------------------------------------------------------------------------
// CImpsServer::SendStatusEvents
// -----------------------------------------------------------------------------
void CImpsServer::SendStatusEvents( EImpsInternalStatus aStatus,
                                    TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: SendStatusEvents begins" ) );
#endif

    iSessionIter.SetToFirst();

    // Scan thru sessions and send event if they have registered an
    // status observer
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        if ( ( ( CImpsSession* )session )->MatchSession( aCSP ) )
            {
            ( ( CImpsSession* )session )->SendEvent( aStatus );
            }
        }
    }

// ---------------------------------------------------------
// CImpsServer::TransportStatus
// ---------------------------------------------------------
//
void CImpsServer::TransportStatus(
    EImpsInternalStatus aConnectionState,
    MImpsCSPSession* aSess )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: TransportStatus=%d" ),
                            ( TInt ) aConnectionState );
#endif

    TInt errx = KErrNone;
    TRAP( errx, DoTransportStatusL( aConnectionState, aSess ) );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: TransportStatus ends" ) );
#endif

    }

// ---------------------------------------------------------
// CImpsServer::DoTransportStatusL
// ---------------------------------------------------------
//
void CImpsServer::DoTransportStatusL(
    EImpsInternalStatus aConnectionState,
    MImpsCSPSession* aSess )
    {
    TImpsSessIdent csp( KNullDesC, aSess->SAP(), aSess->UserId() );
    TInt nbrSes = NbrSessions( ETrue, csp );

    // Logout particular CSP session if there is no corresponding client-sessions.
    if ( aConnectionState != EInternal_NO_IAP && !nbrSes )
        {
        aSess->LogoutL( EFalse );
        return;
        }
    else if ( aConnectionState == EInternal_NO_IAP )
        {
        // start to delete CSP session asynchronously
        DeleteCSPAsynch( aSess );
        return;
        }

    // Send status events only if the state has changed.
    // OFF_LINE events are not send since client API does not support them.
    if ( aConnectionState != EInternal_OFF_LINE )
        {
        SendStatusEvents( aConnectionState, csp );
        }
    }

// ---------------------------------------------------------
// CImpsServer::BufferSize
// ---------------------------------------------------------
//
TInt CImpsServer::BufferSize( )
    {
    return iSettings.MaximumParserSize() - KImpsBufOverhead;
    }

// ---------------------------------------------------------
// CImpsServer::SetConnAllowed
// ---------------------------------------------------------
//
void CImpsServer::SetConnAllowed( TBool aParam )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: SetConnAllowed %d" ), aParam );
#endif
    iConAllowed = aParam;
    // Logout internally all CSP sessions if connection not allowed
    TDblQueIter<CImpsConn> rIter( iCSPList );
    rIter.SetToFirst();
    while ( rIter )
        {
        CImpsConn* conn = rIter;
        rIter++;
        conn->Sess()->SetConnAllowed( aParam );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::SendErrorEvent
// -----------------------------------------------------------------------------
//
void CImpsServer::SendErrorEvent(
    TImpsEventType aType,
    TInt aCode,
    TInt aOpId,
    TImpsSessIdent aCSP )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: SendErrorEvent begins" ) );
#endif
    // Create error event
    iSnd->Reset();
    iSnd->SetStatus( aCode );

    // Search proper session type for an error not response for
    // a client request.
    iSessionIter.SetToFirst();
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        if ( ( ( CImpsSession* )session )->Types( ) && aType &&
             ( ( CImpsSession* )session )->MatchSession( aCSP ) )
            {
            // Send event to client
            ( ( CImpsSession* )session )->SendEvent(
                iSnd,
                aOpId,
                EImpsServNone,
                EImpsMessageNone,
                0 );
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::NewContainerL
// -----------------------------------------------------------------------------
//
CObjectCon* CImpsServer::NewContainerL()
    {
    return iContainerIndex->CreateL();
    }

// -----------------------------------------------------------------------------
// CImpsServer::RemoveContainer
// -----------------------------------------------------------------------------
//
void CImpsServer::RemoveContainer( CObjectCon* aCon )
    {
    iContainerIndex->Remove( aCon );
    }


// -----------------------------------------------------------------------------
// CImpsServer::DiscardRequest
// -----------------------------------------------------------------------------
//
void CImpsServer::DiscardRequest(
    const TDesC& aTid, TImpsEventType aType, TInt aCode,
    TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: DiscardRequest code=%d" ), aCode );
#endif

    iSessionIter.SetToFirst();
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        if ( ( ( CImpsSession* )session )->DiscardRequest(
                 aTid, aType, aCode, aCSP ) )
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::SendLogoutEvents
// -----------------------------------------------------------------------------
//
void CImpsServer::SendLogoutEvents( CImpsSubSession* aSub, TInt aOpId,
                                    TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: SendLogoutEvents" ) );
#endif
    iSessionIter.SetToFirst();
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        if ( ( ( CImpsSession* )session )->MatchSession( aCSP ) )
            {
            ( ( CImpsSession* )session )->SendLogoutEvent(
                KErrNone, aOpId, aSub, 0 );
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::DoShutDown
// -----------------------------------------------------------------------------
//
void CImpsServer::DoShutDown()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: DoShutDown begins" ) );
#endif
    StopTimer();
    if ( iOrphans )
        {
        iOrphans->Stop();
        }

    // Free ECOM plugins
    REComSession::FinalClose();

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: DoShutDown ends" ) );
#endif
    CImpsScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CImpsServer::IsNegotiated
// -----------------------------------------------------------------------------
//
TBool CImpsServer::IsNegotiated( TImpsSessIdent aCSP )
    {
    MImpsCSPSession* csp = GetCSP( aCSP, EFalse );
    if ( !csp )
        {
        return EFalse;
        }
    return csp->IsNegotiated();
    }

// -----------------------------------------------------------------------------
// CImpsServer::Services
// -----------------------------------------------------------------------------
//
TImpsServices* CImpsServer::Services( TImpsSessIdent aCSP )
    {
    MImpsCSPSession* csp = GetCSP( aCSP, EFalse );
    if ( !csp )
        {
        return NULL;
        }
    return csp->Services();
    }

// -----------------------------------------------------------------------------
// CImpsServer::TidSeed
// -----------------------------------------------------------------------------
//
TInt CImpsServer::TidSeed()
    {
    ++iSeed;
    if ( iSeed == KMaxTInt )
        {
        iSeed = 0;
        }
    return iSeed;
    }

// -----------------------------------------------------------------------------
// CImpsServer::LogoutAll
// -----------------------------------------------------------------------------
//
void CImpsServer::LogoutAll()
    {
    // Set server state as SHUTTING_DOWN
    iOperation = EImpsSrvShuttingDown;

    // Send SHUTTING_DOWN events to clients
    iSessionIter.SetToFirst();

    // Check TID match
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        ( ( CImpsSession* )session )->SendEvent( EInternal_SHUTTING_DOWN );
        }

    // Search each CSP session logged in and logout them
    CloseAllCSPs();
    }

// -----------------------------------------------------------------------------
// CImpsServer::CustomSecurityCheckL
// This is used for connect request only
// -----------------------------------------------------------------------------
//

CPolicyServer::TCustomResult CImpsServer::CustomSecurityCheckL(
    const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/ )
    {
    CPolicyServer::TCustomResult test;
    if ( aMsg.HasCapability( ECapabilityNetworkServices ) )
        {
        test = EPass;
        }
    else
        {
        test = EFail;
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: CustomSecurityCheckL returns %d" ), test );
#endif
    return test;
    }


// -----------------------------------------------------------------------------
// CImpsServer::GetCSPVersion
// -----------------------------------------------------------------------------
//
void CImpsServer::GetCSPVersion()
    {
    // Get supported CSP WV version in the terminal.
    // Default is 1.1 version, thus only 1.2 needs to be handled here
    TReal ver ( 0 );
    TRAPD( errx, ver = TImpsDataUtils::GetCenRepRealValueL(
                           KCRUIDWVEngineVariation, KWVEngineCspVersion ) );
    if ( !errx && ver == 1.2 )
        {
        iCSPVersion = EImpsCspVersion12;
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::SetExpiryTimer
// -----------------------------------------------------------------------------
//
void CImpsServer::SetExpiryTimer( TInt aExpiry, TBool aInit )
    {
    // search the minimum value from Cent Rep values
    TInt expiryInterval = iSettings.iAccessExp;
    if ( iSettings.iFuExp < expiryInterval )
        {
        expiryInterval = iSettings.iFuExp;
        }
    if ( iSettings.iGrExp < expiryInterval )
        {
        expiryInterval = iSettings.iGrExp;
        }
    if ( iSettings.iImExp < expiryInterval )
        {
        expiryInterval = iSettings.iImExp;
        }
    if ( iSettings.iPrExp < expiryInterval )
        {
        expiryInterval = iSettings.iPrExp;
        }
    // Then compare the given value and select minimum
    if ( aExpiry )
        {
        if ( aExpiry < expiryInterval )
            {
            expiryInterval = aExpiry;
            }
        }
    if ( !iExpiryInterval || iExpiryInterval > expiryInterval )
        {
        iExpiryInterval = expiryInterval;
        }
    if ( aInit && !iExpiryTimer->IsActive() )
        {
        // Add 1 so that it can alwayd be divided.
        iExpiryTimer->Start( ( iExpiryInterval + 1 ) / 2 );
        }
    }

// -----------------------------------------------------------------------------
// CImpsServer::ResetExpiryTimer
// -----------------------------------------------------------------------------
//
void CImpsServer::ResetExpiryTimer( TInt aExpiry )
    {
    if ( aExpiry >= iExpiryInterval )
        {
        // no need to change the value
        return;
        }

    TInt expiryInterval = 0;
    // Do not start expiry timer if it is not runnig.
    if ( iExpiryTimer->IsActive() )
        {
        // Search minimum expiry time from sub-sessions
        TInt exp = 0;
        while ( iSessionIter )
            {
            CSession2* session = iSessionIter;
            iSessionIter++;
            exp = ( ( CImpsSession* )session )->ExpiryTime( );
            if ( ( exp && exp < expiryInterval ) || !expiryInterval )
                {
                expiryInterval = exp;
                }
            }
        SetExpiryTimer( expiryInterval, EFalse );
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

// -----------------------------------------------------------------------------
// ThreadFunction()
// Needed only in WINS build
// -----------------------------------------------------------------------------

#ifdef __WINS__

EXPORT_C TInt ThreadFunction( TAny* aThreadParams )
    {
    // increase dll's user count so it can't get unloaded when the client application
    // terminates

    RLibrary lib;
    lib.Load( KImpsLibName ); //

    return CImpsServer::ThreadStart( *( TImpsSignal* )aThreadParams );
    }

#endif






//  End of File
