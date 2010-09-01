/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Class for imps sub session.
*
*
*/


// INCLUDE FILES
#include    "impsserver.h"
#include    "impsutils.h"
#include    "impssession.h"
#include    "impsfields.h"
#include    "impserrors.h"
#include    "impstimer.h"
#include    "impssdatautils.h"
#include    "impspacked.h"
#include    "impssubsession.h"
#include    "impstdataaccessor.h"
#include    "impsmessageinterpreterapi.h"
#include    "impsliterals.h"
#include    "ImpsVariantAPI.h"

#ifdef _FAKE_RESPONSE
#include   "impssrvtestutils.h"
#endif

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif




// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsSubSession::CImpsSubSession(  )
        :   iEventList( _FOFF( CWvEvent, iLink ) ),  //lint !e413
        iRequestList( _FOFF( CRequest, iLink ) ), //lint !e413
        iStatusObserver( EFalse ),
        iDetailedError( EFalse ),
        iLastEvent( EImpsMessageNone ),
        iExpiryTime( 0 )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: CREATE subses=%d" ), ( TInt )this );
#endif
    }

// Destructor
// Session destructor calls this
CImpsSubSession::~CImpsSubSession()
    {
    DeleteAllRequests();
    DeleteAllEvents();
    delete iVariant;

    // complete event msg
    CompleteEventMsg( KErrCancel );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DELETE subses=%d" ), ( TInt )this );
#endif
    }

// ---------------------------------------------------------
// CImpsSubSession::NewL()
// ---------------------------------------------------------
CImpsSubSession* CImpsSubSession::NewL(
    CImpsSession* aSession,
    TImpsEventType aType,
    const RMessage2 aMessage )
    {
    CImpsSubSession*  self = new ( ELeave ) CImpsSubSession();
    // CleanupStack::PushL( self );
    // Close method has to be called since this is derived from CObject.
    CleanupClosePushL( *self );
    self->ConstructL( aSession, aType, aMessage );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CImpsSubSession::ConstructL()
// ---------------------------------------------------------
void CImpsSubSession::ConstructL(
    CImpsSession* aSession,
    TImpsEventType aType,
    const RMessage2 aMessage )
    {
    iServiceType = aType;
    iSession = aSession;
    TInt msg = aMessage.Function();

    // get bit mask in an upper byte
    TInt flags = ( msg >> 16 );

    iOOMErr.Reset();

#ifdef _FAKE_RESPONSE
    // **********************************
    // MODULE TEST for auxiliary classes

    TImpsSrvTestUtils::TestOOMList();

    // **********************************
#endif

    iHandleNew = ( flags & KImpsReqHandleNew ) ? ETrue : EFalse;
    iVariant = CImpsVariant::NewLC( );
    CleanupStack::Pop( ); // >> iVariant
    // default value used ubtil client request to change it.
    iExpiryTime = Server()->ExpirySeconds( iServiceType );
    }

// -----------------------------------------------------------------------------
// CImpsSubSession::Unregister()
// -----------------------------------------------------------------------------
void CImpsSubSession::Unregister()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: Unregister subses=%d h=%d type=%d" ),
                            ( TInt )this, Handle(), ( TInt )iServiceType );
#endif
    iCanceled = ETrue;
    DeleteAllRequests();

    // Complete new message observer
    CompleteEventMsg( KErrCancel );

    ( void ) DeleteAllEvents();
    }

// -----------------------------------------------------------------------------
// CImpsSubSession::DeleteSub()
// -----------------------------------------------------------------------------
void CImpsSubSession::DeleteSub()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DeleteSub subses=%d type=%d" ), ( TInt )this, ( TInt )iServiceType );
#endif
    // Calculate new expiry timer value
    Server()->ResetExpiryTimer( iExpiryTime );
    // Delete this entity
    RMessage2 myMsg( Message() );
    iSession->DeleteSubSession( myMsg.Int3() );
    }

// ---------------------------------------------------------
// CImpsSubSession::AssignIdL()
// ---------------------------------------------------------
void CImpsSubSession::AssignIdL( TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: AssignIdL subses=%d" ), ( TInt )this );
#endif
    // create an event for each orphan message
    CImpsFields* msg = NULL;
    msg = Server()->NextOrphanLC( iSession->ApplicationId(), iServiceType, aCSP );
    while ( msg )
        {
        SendEvent( msg, 0, EImpsServNone, EImpsMessageNone );
        CleanupStack::PopAndDestroy();
        msg = Server()->NextOrphanLC( iSession->ApplicationId(), iServiceType, aCSP );
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::HandleAllOrphans()
// ---------------------------------------------------------
void CImpsSubSession::HandleAllOrphans( TImpsSessIdent aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: HandleAllOrphans subses=%d" ), ( TInt )this );
#endif
    TInt err = KErrNone;
    TRAP( err, AssignIdL( aCSP ) );
    }

// -----------------------------------------------------------------------------
// CImpsSubSession::DeleteAllEvents()
// -----------------------------------------------------------------------------
TInt CImpsSubSession::DeleteAllEvents(  )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DeleteAllEvents subses=%d" ), ( TInt )this );
#endif

    TInt ret = 0;

    // Reset OOM_LIST too
    iOOMErr.Reset();

    // Delete all buffered events from this client,

    TDblQueIter<CWvEvent> rIter( iEventList );

    rIter.SetToFirst();

    while ( rIter )
        {
        CWvEvent* event = rIter;
        rIter++;
        if ( event->iSent )
            {
            ret++;
            }
        event->Destroy();
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CImpsSubSession::DeleteAllRequests()
// -----------------------------------------------------------------------------
void CImpsSubSession::DeleteAllRequests(  )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DeleteAllRequests" ) );
#endif
    // Delete all buffered requests from this client.

    TDblQueIter<CRequest> rIter( iRequestList );

    rIter.SetToFirst();

    while ( rIter )
        {
        CRequest* requ = rIter;
        rIter++;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: DeleteAllRequests TID=%S" ), &( requ->iTID ) );
#endif
        requ->Destroy();
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::LoginL()
// ---------------------------------------------------------
void CImpsSubSession::LoginL( TBool aReactive )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: LoginL begins reactive=%d subses=%d" ),
                            aReactive, ( TInt )this );
#endif

    // OPID
    RMessage2 myMsg( Message() );
    TInt opId = myMsg.Int1();
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: Login opid=%d" ), opId );
#endif

    // Read PACKED ARRAY
    CDesCArrayFlat* tempArr = new ( ELeave )CDesCArrayFlat( 7 );
    CleanupStack::PushL( tempArr );   // << tempArr

    HBufC8* stream = *StreamBufAddr();
    if ( iSession->ReadBuffer8L( 0, stream ) )
        {
        HBufC8** stream2 = StreamBufAddr();
        *stream2 = stream;
        }
    TImpsPackedEntity packedMessage( stream );
    const TUint8* pS = stream->Ptr();
    packedMessage.DoUnpackArrayL( pS, tempArr );

    __ASSERT_DEBUG(
        tempArr->MdcaCount() == 7,
        User::Panic( KImpsPanicCategory,
                     EImpsCorrupted ) );
    TPtrC tid;
    TPtrC tempUser = tempArr->MdcaPoint( 0 );
    TPtrC tempPassword = tempArr->MdcaPoint( 1 );
    TPtrC tempCliId = tempArr->MdcaPoint( 2 );
    TPtrC tempSAP = tempArr->MdcaPoint( 3 );
    TPtrC key1 = tempArr->MdcaPoint( 4 );
    TPtrC key2 = tempArr->MdcaPoint( 5 );
    TPtrC ap = tempArr->MdcaPoint( 6 );

    // convert AP back
    TLex lex;
    TUint32 myAp;
    lex.Assign( ap );
    lex.Val( myAp, EDecimal );

    // Multi: update session's user-id and SAP.
    iSession->SetUserIdL( tempUser );
    iSession->SetSAPL( tempSAP );

    TImpsSessIdent csp( KNullDesC, tempSAP, tempUser );
    TPtrC p;
    if ( Server()->IsLogged( csp, &p ) )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: CSP already logged ín subses=%d" ), ( TInt )this );
#endif
        // check password: if it does not match then return an error.
        if ( p.CompareF( tempPassword ) )
            {
            User::Leave( KImpsErrorUnauthorized );
            }

        // Update session SID and other CSP identification data.
        iSession->ModifySIDL( Server()->SID( csp ) );
        iSession->SetSAPL( tempSAP );
        iSession->SetUserIdL( tempUser );

        // multi: Share existing CSP session
        // Send callback reponse to a client
        // Complete the open asyncronous request first and
        // after that send login event (callback).
        iSession->CompleteMe( KErrNone );
        SendLoginEvent( KErrNone, opId );
        if ( Server()->IsNegotiated( csp ) )
            {
            // On-line event too if CSP is negotiated.
            // Must sent via Session to all subsessions
            iSession->SendEvent( EInternal_ON_LINE );
            // Check orphan messages because of IM subsessions
            // may be registered before login request,
            // Create an event for each orphan message
            Server()->HandleAllOrphans();
            }
        tempArr->Reset();
        CleanupStack::PopAndDestroy( 1 );   // >> tempArr
        return;
        }


    // Save login expiry time and give it to CSP session too.
    TTime loginExpiry = Server()->ExpiryTime( iExpiryTime );

    tid.Set( Server()->LoginL( tempUser, tempPassword, tempCliId,
                               tempSAP, myAp, key1, key2, this, loginExpiry, aReactive ) );

    tempArr->Reset();
    CleanupStack::PopAndDestroy( 1 );   // >> tempArr

    // Update request queue
    // Add request into queue
    CRequest* request = new ( ELeave ) CRequest(
        tid,    // tid
        opId,   // op-id
        EImpsServWVLogin,
        loginExpiry,
        EImpsLoginReq
    );

    iRequestList.AddLast( *request );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: LoginL ends TID=%S" ), &tid );
#endif

    }

// ---------------------------------------------------------
// CImpsSubSession::LogoutL()
// ---------------------------------------------------------
TPtrC CImpsSubSession::LogoutL( TImpsSessIdent aCSP, TBool aCancel )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: LogoutL begins subses=%d" ), ( TInt )this );
#endif

    RMessage2 myMsg( Message() );
    TInt opId = myMsg.Int0();
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: Logout opid=%d" ), opId );
#endif

    TPtrC tid;

    // Login-cancel.
    if ( aCancel )
        {
        // Check that request still exists, if not then leave KErrNotFound
        TBool found( EFalse );
        TDblQueIter<CRequest> requestIter( iRequestList );
        requestIter.SetToFirst();
        while ( requestIter )
            {
            CRequest* request = requestIter;
            requestIter++;
            if ( request->iOpId == opId )
                {
                found = ETrue;
                break;
                }
            }
        if ( !found )
            {
            User::Leave( KErrNotFound );
            }
        }

    // Send logout request forward to CSP session entity
    TRAPD( errL, tid.Set( iSession->Server()->LogoutL( aCancel, aCSP ) ) );

    // If canceling login operation then client needs to get only one callback method call,
    // so do not save new CRequest entity then. The CSP session entity will call later
    // SendEvent to respond to the cancelled login operation if there is something
    // to cancel.
    if ( aCancel )
        {
        if ( errL )
            {
            // If the previous request leaves then there is nothing to cancel.
            // Therefore delete the login request entity now and verify that CSP entity
            // clear the particular transaction data,
            CancelTrans( myMsg, aCSP );
            User::Leave( errL );
            }
        // CSPSession will call back later and delete the request and send a response
        // event to cancel request when everyting is clear regarding cancel of login
        // transaction..
        return TPtrC();
        }
    if ( errL == KImpsErrorTerminalOffLine )
        {
        // Logout requested in such a situation where Logout request is
        // not needed nor not possible to be sent to SAP server.
        // We have to generate a response here
        // because of Logout must not fail in client API.
        SendLogoutEvent( KErrNone, opId );
        // Send NOT_LOGGED events to subhandles
        iSession->SendEvent( EInternal_NOT_LOGGED );
        return TPtrC();
        }
    else if ( errL )
        {
        // Unexpeted error situation
        User::Leave( errL );
        }

    // Add request into request queue
    CRequest* request = new ( ELeave ) CRequest(
        tid,
        opId,
        EImpsServWVLogout,
        Server()->ExpiryTime( iExpiryTime ),
        EImpsLogoutReq
    );
    iRequestList.AddLast( *request );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: LogoutL ends with tid=%S" ), &tid );
#endif
    return tid;
    }

// ---------------------------------------------------------
// CImpsSubSession::Message()
// ---------------------------------------------------------
// return message - get it from session
const RMessagePtr2 CImpsSubSession::Message() const
    {
    return iSession->Message();
    }

// ---------------------------------------------------------
// CImpsSubSession::Server()
// ---------------------------------------------------------
// return message - get it from session
CImpsServer* CImpsSubSession::Server()
    {
    return iSession->Server();
    }

// ---------------------------------------------------------
// CImpsSubSession::DoSendStatusEvent()
// ---------------------------------------------------------
void CImpsSubSession::DoSendStatusEvent( CWvEvent& aEvent )
    {

    // Status event is lighter than a regular event in a sense that
    // it does not write data to client thread, but updates iStatus
    // of the handler active object in a client thread only.
    if ( iEventMsg.IsNull() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: DoSendStatusEvent NULL MSG subses=%d" ),
                                ( TInt )this );
#endif
        return;
        }

    TInt newStatus = ConvertStatusCode( ( EImpsInternalStatus )aEvent.iAux );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DoSendStatusEvent stat=%d subses=%d" ),
                            newStatus, ( TInt )this );
#endif
    CompleteEventMsg( newStatus );

    aEvent.iSent = ETrue;
    }

// ---------------------------------------------------------
// CImpsSubSession::SendEvent()
// ---------------------------------------------------------
void CImpsSubSession::SendEvent(
    CImpsFields *aFields,
    TInt aOpId,
    TImpsServRequest aRequestType,
    TImpsMessageType aReqMsgType )
    {

    TInt memErr = KErrNone;
    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled  )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "SubSession: SendEvent cancelled subses=%d ****" ), ( TInt )this );
#endif
        return;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendEvent begins" ) );
#endif


    TDblQueIter<CWvEvent> eventIter( iEventList );

    // Check if too many events waiting
    CWvEvent* event = NULL;

    // check if there is unhandled events for this session
    TBool thisFirst = ( NbrEvents() > 0 || iOOMErr.Exists() ) ? EFalse : ETrue;

    // Split GroupChangeNotice up to three parts for
    // HandleNewUsersL, HandleLeftUsersL and HandleGroupProperties
    if ( aFields->MessageType() == EImpsGroupChangeNotice )
        {
        TRAP( memErr, DoSplitGroupChangeL( aOpId, aRequestType, aFields ) );
        }
    else
        {
        TRAP( memErr, DoCreateEventL( aOpId, aRequestType, aReqMsgType, aFields ) );
        }

    // Now the event is ready to be sent.
    // Send the event to client thread if this is first one
    if ( thisFirst && !memErr )
        {
        // This should be a new event just created
        eventIter.SetToFirst();
        event = eventIter;
        __ASSERT_DEBUG(
            !event->iSent,
            User::Panic( KImpsPanicCategory,
                         EImpsCorrupted ) );
        // Select between base event and EventBody
        DoSendBaseEvent( *event );
        }
    // Notice: CSP negotiation requests (client capabilities and service
    // negotiaion) are internal for CSP class entity and they are not
    // originated from client. Thus corresponding negotiation phase
    // errors are not guaranted to be sent to a client in OOM situation.
    // In those cases the aOpId is NULL.
    //
    else if ( memErr && aOpId )
        {
        // send/store OOM error message ( KErrNoMemory, aOpId )
        iOOMErr.StoreOOM( aOpId );
        if ( thisFirst )
            {
            ( void )SendOOMError();
            }
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendEvent ends" ) );
#endif
    }

// ---------------------------------------------------------
// CImpsSubSession::SendEvent()
// Lighter version for engine status event
//     Notice: CSP status changes are guaranted to
//     to be sent to a client in OOM situation too.
// ---------------------------------------------------------
void CImpsSubSession::SendEvent(
    EImpsInternalStatus aStatus )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled || !iStatusObserver  )
        {
        return;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendEvent (status) subses=%d" ), ( TInt )this );
#endif

    // check if there is unhandled events for this session
    TBool thisFirst = ( NbrEvents() > 0 || iOOMErr.Exists() ) ? EFalse : ETrue;

    // Prefer reqular event queue then OOM error queue in order
    // to save the logical order of events.
    TRAPD( memErr, DoCreateEventL( aStatus ) );

    if ( thisFirst && !memErr )
        {
        TDblQueIter<CWvEvent> eventIter( iEventList );
        CWvEvent* event = NULL;
        // This should be a new event just created
        eventIter.SetToFirst();
        event = eventIter;
        __ASSERT_DEBUG(
            !event->iSent,
            User::Panic( KImpsPanicCategory,
                         EImpsCorrupted ) );
        // Send the event immediately
        DoSendStatusEvent( *event );
        }

    else if ( memErr )
        {
        // send/store OOM error message ( status, aOpId )
        TInt retCode = ConvertStatusCode( aStatus );
        iOOMErr.StoreOOM( retCode );
        if ( thisFirst )
            {
            ( void )SendOOMError();
            }
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendEvent ends" ) );
#endif
    }

// ---------------------------------------------------------
// CImpsSubSession::DoSendBaseEvent()
// ---------------------------------------------------------
void CImpsSubSession::DoSendBaseEvent( CWvEvent& aEvent )
    {
    TInt bufSize = EventMsgBufSize();
    TInt messageLength = aEvent.iPackedMessage ?
                         aEvent.iPackedMessage->Length() : 0;
    if ( bufSize > 0 && bufSize >= messageLength )
        {
        // Message body can be sent immediately.
        // This sends message headers too.
        DoSendEventBody( aEvent );
        }
    else
        {
        // No message body or client is asked to increase the buffer
        DoSendEventHeader( aEvent );
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::DoSendEventHeader()
// ---------------------------------------------------------
void CImpsSubSession::DoSendEventHeader( CWvEvent& aEvent )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DoSendEventHeader type=%d subses=%d" ),
                            aEvent.iType, ( TInt )this );
#endif
    RMessagePtr2 ptrMsg = EventMsg();
    if ( ptrMsg.IsNull() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: DoSendEventHeader NULL MSG subses=%d" ),
                                ( TInt )this );
#endif
        return;
        }
    // Create the header information

    TBuf < sizeof( SImpsEventData ) >
    eventBuf( sizeof( SImpsEventData ) );

    SImpsEventData* eventData =
        ( SImpsEventData* )eventBuf.Ptr();

    DoCreateEventIPC( aEvent, eventData );

    // Write the message header to client thread via a message.
    TInt err = KErrNone;


    err = DoWriteEventHeaders( ptrMsg, eventBuf );

    // We use NotifyHandler status parameter to pass the size of the
    // packed message!
    TInt messageLength = aEvent.iPackedMessage ?
                         aEvent.iPackedMessage->Length() : 0;
    if ( err == KErrNone )
        {
        err = messageLength;
        }

    // Signal event handler in a client thread
    CompleteEventMsg( err );

    aEvent.iSent = ETrue;
    }

// ---------------------------------------------------------
// CImpsSubSession::DoSendEventBody()
// ---------------------------------------------------------
void CImpsSubSession::DoSendEventBody( CWvEvent& aEvent )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DoSendEventBody type=%d subses=%d" ),
                            aEvent.iType, ( TInt )this );
#endif

    RMessagePtr2 ptrMsg = EventMsg();
    if ( ptrMsg.IsNull() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: DoSendEventBody NULL MSG subses=%d" ),
                                ( TInt )this );
#endif
        return;
        }

    // This writes both headers and body.
    // This increases the robustenss and thre client thread do not need
    // to store data between these transactions, because of it receives
    // all the necessary data at once.

    TBuf < sizeof( SImpsEventData ) >
    eventBuf( sizeof( SImpsEventData ) );

    SImpsEventData* eventData =
        ( SImpsEventData* )eventBuf.Ptr();

    DoCreateEventIPC( aEvent, eventData );
    eventData->iMessageBody = ( aEvent.iPackedMessage ? ETrue : EFalse );

    // Write the message header to client thread via a message.
    TInt err = KErrNone;
    err = DoWriteEventHeaders( ptrMsg, eventBuf );

    if ( !err )
        {
        // Write the message body
        err = DoWriteEventBody( ptrMsg, aEvent );
        // We use NotifyHandler status parameter to pass the size of the
        // packed message!
        TInt messageLength = aEvent.iPackedMessage ?
                             aEvent.iPackedMessage->Length() : 0;
        if ( err == KErrNone )
            {
            err = messageLength;
            }
        }

    // Signal event handler in a client thread
    CompleteEventMsg( err );

    aEvent.iSent = ETrue;
    }

// ---------------------------------------------------------
// CImpsSubSession::SendLogoutEvent
// ---------------------------------------------------------
void CImpsSubSession::SendLogoutEvent( TInt aRespStatus, TInt aOpId )
    {
    // transfer CSP error code if any
    if ( aRespStatus > 0 )
        {
        aRespStatus = ErrorCode( aRespStatus );
        }

    // check session type first and filter sending
    // of the same logout twice
    if ( iServiceType != EImpsEventServerLogin ||
         ( iLastEvent == EImpsDisconnect && aOpId == 0 ) )
        {
        return;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendLogoutEvent in action subses=%d" ),
                            ( TInt )this );
#endif

    TDblQueIter<CWvEvent> eventIter( iEventList );
    CWvEvent* event = NULL;

    // check if there is unhandled events for this session
    TBool thisFirst = ( NbrEvents() > 0 || iOOMErr.Exists() ) ? EFalse : ETrue;

    // Let this behave like real SAP initiated Disconnect primitive
    TRAPD( memErr, DoCreateEventL( aOpId, aRespStatus,
                                   EImpsMessageNone, EImpsDisconnect, EImpsServWVLogout ) );
    // Since WV 1,2 logout messages may be either disconnect or Status,
    // harmomize them here in order to avoid sending logout message
    // with opid=actual_id and opid=0.
    iLastEvent = EImpsDisconnect;

    // Now the event is ready to be sent.
    // Send the event to client thread if this is first one
    if ( thisFirst && !memErr )
        {
        // This should be a new event just created
        eventIter.SetToFirst();
        event = eventIter;
        __ASSERT_DEBUG(
            !event->iSent,
            User::Panic( KImpsPanicCategory,
                         EImpsCorrupted ) );
        // Logout event does not contain body in this case
        DoSendEventHeader( *event );
        }
    else if ( memErr && aOpId )
        {
        // send/store OOM error message ( KErrNoMemory, aOpId )
        iOOMErr.StoreOOM( aOpId );
        if ( thisFirst )
            {
            ( void )SendOOMError();
            }
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::SendLoginEvent
// Multi: new method to generate events when CSP session
// already exists
// ---------------------------------------------------------
void CImpsSubSession::SendLoginEvent( TInt aRespStatus, TInt aOpId )
    {
    // transfer CSP error code if any
    if ( aRespStatus > 0 )
        {
        aRespStatus = ErrorCode( aRespStatus );
        }

    // check session type first and filter sending
    // of the same logout twice
    if ( iServiceType != EImpsEventServerLogin )
        {
        return;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendLoginEvent in action subses=%d" ),
                            ( TInt )this );
#endif

    TDblQueIter<CWvEvent> eventIter( iEventList );
    CWvEvent* event = NULL;

    // check if there is unhandled events for this session
    TBool thisFirst = ( NbrEvents() > 0 || iOOMErr.Exists() ) ? EFalse : ETrue;

    // Let this behave like real SAP initiated Disconnect primitive
    TRAPD( memErr, DoCreateEventL( aOpId, aRespStatus,
                                   EImpsMessageNone, EImpsLoginRes, EImpsServWVLogin ) );

    // Now the event is ready to be sent.
    // Send the event to client thread if this is first one
    if ( thisFirst && !memErr )
        {
        // This should be a new event just created
        eventIter.SetToFirst();
        event = eventIter;
        __ASSERT_DEBUG(
            !event->iSent,
            User::Panic( KImpsPanicCategory,
                         EImpsCorrupted ) );
        // Logout event does not contain body in this case
        DoSendEventHeader( *event );
        }
    else if ( memErr && aOpId )
        {
        // send/store OOM error message ( KErrNoMemory, aOpId )
        iOOMErr.StoreOOM( aOpId );
        if ( thisFirst )
            {
            ( void )SendOOMError();
            }
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::SendOOMError
// ---------------------------------------------------------
TBool CImpsSubSession::SendOOMError( )
    {

    if ( iEventMsg.IsNull() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "SubSession: SendOOMError NULL MSG subses=%d" ), ( TInt )this );
#endif
        return EFalse;
        }

    TInt opId = iOOMErr.GetOOM();
    if ( opId != 0 )
        {
        if ( opId > 0 )
            {
            CompleteEventMsg( KImpsOOMReply | opId );
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log(
                _L( "SubSession: SendOOMError completes opid=%d ****" ), opId );
#endif
            }
        else
            {
            // Status change special error code etc
            CompleteEventMsg( opId );
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log(
                _L( "SubSession: SendOOMError completes status=%d ****" ), opId );
#endif
            }
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::SendPrimitiveL()
// ---------------------------------------------------------
void CImpsSubSession::SendPrimitiveL( TInt aFunction, TImpsSessIdent aCSP )
    {
    RMessage2 myMsg( Message() );
    TInt opId = myMsg.Int1();
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendPrimitiveL begins %d opid=%d" ),
                            aFunction, opId );
#endif

    switch ( aFunction )
        {
        case EImpsServWVSendOnly:
        case EImpsServBlock:
            // Check availability of the service in general
            TImpsSrvUtils::CheckServiceL( EIMFeat,
                                          Server()->Services( aCSP ) );
            break;
        case EImpsServGroup:
            TImpsSrvUtils::CheckServiceL( EGroupFeat,
                                          Server()->Services( aCSP ) );
            break;
        case EImpsServPres:
            TImpsSrvUtils::CheckServiceL( EPresenceFeat,
                                          Server()->Services( aCSP ) );
            break;
        case EImpsServFundSearch:
        case EImpsServFundInvite:
            TImpsSrvUtils::CheckServiceL( EFundamentalFeat,
                                          Server()->Services( aCSP ) );
            break;
        default:
            break;
        }
    // Get data to iFields
    GetWVDataL();

    TImpsMessageType myMesType = ( TImpsMessageType ) ImpsFields()->MessageType();

    // Check primitive if this is pure subsession
    // Discard anything that does not belong to presence service
    if ( iServiceType == EImpsEventPresencePure &&
         impsService( iVariant, TImpsMessageType( myMesType ) ) != EImpsEventPresence )
        {
        User::Leave( KImpsErrorUnknownMessageType );
        }

    // Add sender if it is empty in new IM message
    if ( aFunction == EImpsServWVSendOnly )
        {
        TPtrC sender;
        TPtrC group;
        CImpsKey* myKey = CImpsKey::NewLC();      // <<< myKey
        TImpsSDataUtils::AddValuesFromArrayL(
            myKey,
            KSendMessageElements,
            sizeof( KSendMessageElements ) /
            sizeof( KSendMessageElements[0] ) );
        myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
        TImpsDataAccessor myAc( ImpsFields() );
        TImpsSDataUtils::GetSenderL( myKey, &myAc, sender, group );
        if ( sender.Length() == 0 )
            {
            TPtrC userId = iSession->UserId();
            // Set Sender( User( UserID )) if sender not specified
            // in client API
            // Notice: Here we could add assigned clientID if wanted so.
            TImpsSDataUtils::SetSenderL(
                myKey,
                &myAc,
                &userId,
                NULL );
            }

        TPtrC applicationID( iSession->ApplicationId() );
        if ( applicationID.Length() != 0 )
            {
            // insert the ExtBlock
            myKey->PopL( 3 );
            myKey->AddL( CREATEKEY( EImpsKeyExtBlock, 0 ) );
            myKey->AddL( CREATEKEY( EImpsKeyAPIClient, 0 ) );
            myAc.StoreDescL( myKey, applicationID );
            }


        CleanupStack::PopAndDestroy( 1 );   // >>> myKey

        }

    TPtrC tid;

    // Send data may in certain situation change iFields,
    // so copy data address to safe.
    tid.Set( Server()->SendDataL( this, aCSP ) );

    // Add request into queue
    CRequest* request = new ( ELeave ) CRequest(
        tid,    // tid
        opId,   // op-id
        ( TImpsServRequest )aFunction,
        Server()->ExpiryTime( iExpiryTime ),
        myMesType );
    iRequestList.AddLast( *request );

    }

// ---------------------------------------------------------
// CImpsSubSession::SendGetBlockedL()
// This is for IM GetBlocked, only ID received from client
// ---------------------------------------------------------
void CImpsSubSession::SendGetBlockedL( TImpsSessIdent aCSP )
    {

    RMessage2 myMsg( Message() );
    TInt opId = myMsg.Int1();

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendGetBlockedL  opid=%d" ), opId );
#endif

    // Check availability of the service in general
    TImpsSrvUtils::CheckServiceL( EIMFeat, Server()->Services( aCSP ) );

    // Generate the stuff. Server thread finalizes the message adding
    // session-id etc

    ImpsFields()->Reset();
    ImpsFields()->SetMessageType( EImpsGetBlockedListReq );

    CImpsDataAccessor* myAc = CImpsDataAccessor::NewL( ImpsFields() );
    CleanupStack::PushL( myAc );        // <<< myAc
    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey

    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
    myKey->AddL( CREATEKEY( EImpsKeyGetBlockedList_Request, 0 ) );
    myAc->StoreEmptyL( myKey );

    TPtrC tid;

    // Send data may in certain situation change iFields,
    // so copy data to safe.
    tid.Set( Server()->SendDataL( this, aCSP ) );

    // Update request queue
    // Add request into queue
    CRequest* request = new ( ELeave ) CRequest(
        tid,            // tid
        opId,           // op-id
        EImpsServGetBlocked,
        Server()->ExpiryTime( iExpiryTime ),
        EImpsGetBlockedListReq );
    iRequestList.AddLast( *request );

    CleanupStack::PopAndDestroy( 2 );   // >>> myAc, myKey
    }

// ---------------------------------------------------------
// CImpsSubSession::DoNextEvent()
// ---------------------------------------------------------
void CImpsSubSession::DoNextEvent()
    {

    // check OOM_LIST first
    iOOMErr.RemoveOOM();
    TInt errId = SendOOMError();
    if ( errId )
        {
        return;
        }

    // Delete the event that was sent and send next
    TDblQueIter<CWvEvent> eventIter( iEventList );
    eventIter.SetToFirst();

    while ( eventIter )
        {
        CWvEvent* event = eventIter;
        eventIter++;

        if ( event->iSent )
            {
            event->Destroy();
            }
        else
            {
            if ( event->iRequestType != EImpsServStatusReg )
                {
                DoSendBaseEvent( *event );
                }
            else
                {
                DoSendStatusEvent( *event );
                }
            // Only one event at a time is possible
            break;
            }
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::DoEventBodyL()
// ---------------------------------------------------------
void CImpsSubSession::DoEventBodyL()
    {
    // Check that the first event (headers) in the queue
    // is sent
    TDblQueIter<CWvEvent> eventIter( iEventList );
    eventIter.SetToFirst();
    CWvEvent* event = eventIter;
    if ( !event || !event->iSent )
        {
        // This ensures that if the synchronization between client-server
        // is lost the next messages are handled because of the
        // client asks next event after this error.
        User::Leave( KImpsGeneralError );
        }

    // Write the body to the client thread
    DoSendEventBody( *event );
    }

// ---------------------------------------------------------
// CImpsSubSession::DiscardRequests()
// Expiry detection
// ---------------------------------------------------------
void CImpsSubSession::DiscardRequests(
    TTime aExpiryTime,
    TImpsEventType aServiceType,
    MImpsCSPSession* aSess )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return;
        }

    if ( !( aServiceType & iServiceType ) )
        {
        return;
        }

    // check if there is unhandled events for this session
    TBool thisFirst = ( NbrEvents() > 0 || iOOMErr.Exists() ) ? EFalse : ETrue;

    TDblQueIter<CRequest> requestIter( iRequestList );
    requestIter.SetToFirst();
    while ( requestIter )
        {
        CRequest* request = requestIter;
        requestIter++;
        if ( request->iExpiry < aExpiryTime )
            {
            // expired entry found
            // Create error event for UI API
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log(
                _L( "SubSession: Entry EXPIRED type=%d subses=%d" ),
                request->iRequestType, ( TInt )this );
#endif
            // Delete the message from server queues if not send
            Server()->CancelTrans( request->iTID, aSess );
            // Create event for client
            TRAPD( memErr, DoCreateEventL(
                       request->iOpId,
                       KErrTimedOut,
                       request->iMessageType ) );

            // store OOM error message if necessary ( KErrNoMemory, aOpId )
            if ( memErr )
                {
                iOOMErr.StoreOOM( request->iOpId );
                }

            // Send an event if this is first
            if ( thisFirst )
                {
                DoNextEvent();
                thisFirst = EFalse;
                }
            // delete request from queue
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log(
                _L( "SubSession: DiscardRequests TID=%S" ), &( request->iTID ) );  //lint !e525
#endif
            request->Destroy();
            }
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::DiscardRequests()
// Discrad all requests
// ---------------------------------------------------------
void CImpsSubSession::DiscardRequests(
    TInt aError,
    TImpsEventType aServiceType,
    MImpsCSPSession* aSess )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return;
        }

    if ( !( aServiceType & iServiceType ) )
        {
        return;
        }

    // check if there is unhandled events for this session
    TBool thisFirst = ( NbrEvents() > 0 || iOOMErr.Exists() ) ? EFalse : ETrue;

    TDblQueIter<CRequest> requestIter( iRequestList );
    requestIter.SetToFirst();
    while ( requestIter )
        {
        CRequest* request = requestIter;
        requestIter++;

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "SubSession: Entry DISCARDED type=%d subses=%d" ),
            request->iRequestType, ( TInt )this );
#endif
        // Delete the message from server queues if not send
        Server()->CancelTrans( request->iTID, aSess );
        // Create event for client
        TRAPD( memErr, DoCreateEventL(
                   request->iOpId,
                   aError,
                   request->iMessageType ) );

        if ( memErr )
            {
            iOOMErr.StoreOOM( request->iOpId );
            }

        // Try to send event if this is first
        if ( thisFirst )
            {
            DoNextEvent();
            thisFirst = EFalse;
            }
        // delete request from queue
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "SubSession: DiscardRequests TID=%S" ), &( request->iTID ) ); //lint !e525
#endif
        request->Destroy();

        }
    }

// ---------------------------------------------------------
// CImpsSubSession::DiscardRequest()
// Discard a single request
// ---------------------------------------------------------
TBool CImpsSubSession::DiscardRequest(
    const TDesC& aTid,
    TImpsEventType aServiceType,
    TInt aCode )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return EFalse;
        }

    TBool ret( EFalse );
    if ( aServiceType != iServiceType )
        {
        return ret;
        }

    // check if there is unhandled events for this session
    TBool thisFirst = ( NbrEvents() > 0 || iOOMErr.Exists() ) ? EFalse : ETrue;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DiscardRequest subses=%d" ), ( TInt )this );
#endif

    TDblQueIter<CRequest> requestIter( iRequestList );
    requestIter.SetToFirst();
    while ( requestIter )
        {
        CRequest* request = requestIter;
        requestIter++;
        if ( !request->iTID.Compare( aTid ) )
            {
            // entry found
            // Create error event for UI API
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "SubSession: entry DISCARDED" ) );
#endif
            TRAPD( memErr, DoCreateEventL( request->iOpId,
                                           aCode,
                                           request->iMessageType ) );

            if ( memErr )
                {
                // store OOM error message ( KErrNoMemory, aOpId )
                iOOMErr.StoreOOM( request->iOpId );
                }

            // The following is NOT called because of server has called this.
            // Server()->CancelTrans( request->iTID );
            // delete request from queue
            request->Destroy();
            ret = ETrue;
            break;
            }
        }

    // Send the new event only if no old events in queue
    if ( ret && thisFirst )
        {
        DoNextEvent();
        }

    return ret;
    }

// ---------------------------------------------------------
// CImpsSubSession::CheckNotification()
// ---------------------------------------------------------
TBool CImpsSubSession::CheckNotification( CImpsFields* aFields )
    {

    TBool ret ( EFalse );
    TRAPD( errx, ( ret = DoCheckNotificationL( aFields ) ) );
    if ( ret && !errx )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::DoCheckNotificationL()
// Notice: MessageNotifiction not supported.
// ---------------------------------------------------------
TBool CImpsSubSession::DoCheckNotificationL( CImpsFields* aFields )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return EFalse;
        }

    // Filter new messages out if necessary
    if ( !iHandleNew )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: new msg FILTERED OUT" ) );
#endif
        return EFalse;
        }

    TImpsDataAccessor myAc( aFields );
    TPtrC msgContentType;
    TBool retVal = EFalse;
    TInt messageType = aFields->MessageType();

    // Handle SAP initiated transactions here.
    //
    switch ( this->iServiceType )
        {
        case EImpsEventMessage:
            if ( messageType == EImpsNewMessage )
                {
                retVal = ETrue;
                // Checking the ApplicationID
                // Consider client-id routing if media type is ok
                /*
                 * 1) Get the ApplicationID from the message
                 * 2) Get the ApplicationID from the corresponding session
                 *    ownClientId = iSession->ApplicationID();
                 * 3) if ownClientId == messageClientId -> retVal = ETrue,
                 *    this includes the case when both are empty
                 * 4) otherwise retVal = EFalse
                */
                TPtrC messageAppID; // The recipient's ApplicationID

                TImpsSDataUtils::GetApplicationIDL( &myAc, messageAppID );

                if ( !messageAppID.Compare( iSession->ApplicationId() ) )
                    {
                    // The ClientIDs are matching
                    retVal = ETrue;
                    }
                else
                    {
                    retVal = EFalse;
                    }
                }
            else if ( messageType == EImpsDeliveryReportReq )
                {
                retVal = ETrue;
                }
            break;
        case EImpsEventServerLogin:
            if ( messageType == EImpsDisconnect )
                {
                retVal = ETrue;
                }
            break;
        case EImpsEventGroup:
            if ( messageType == EImpsLeaveGroupRes ||
                 messageType == EImpsGroupChangeNotice )
                {
                retVal = ETrue;
                }
            break;
        case EImpsEventCommon:
            if ( messageType == EImpsInviteUserReq ||
                 messageType == EImpsInviteRes ||
                 messageType == EImpsCancelInviteUserReq )
                {
                retVal = ETrue;
                }
            break;
        case EImpsEventPresence:
        case EImpsEventPresencePure:
            if ( messageType == EImpsPresenceNotification ||
                 messageType == EImpsPresenceAuthReq ||
                 messageType == EImpsPureData )
                {
                retVal = ETrue;
                }
            break;
        default:
            retVal = EFalse;
            break;
        }

    return retVal;
    }

// ---------------------------------------------------------
// CImpsSubSession::CheckRequests()
// ---------------------------------------------------------
TBool CImpsSubSession::CheckRequests(
    const TDesC& aTid,
    TInt& aOpId,
    TImpsServRequest& aRequestType,
    TImpsMessageType& aReqMsgType )
    {
    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return EFalse;
        }

    aRequestType = EImpsServNone;
    aReqMsgType = EImpsMessageNone;

    TDblQueIter<CRequest> requestIter( iRequestList );

    requestIter.SetToFirst();

    while ( requestIter )
        {
        CRequest* request = requestIter;
        requestIter++;

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: CheckRequests subses=%d aTID=%S reg.TID=%S" ),
                                ( TInt )this, &aTid, &request->iTID );
#endif

        if ( !request->iTID.Compare( aTid ) )
            {
            aOpId = request->iOpId;
            aRequestType = request->iRequestType;
            aReqMsgType = request->iMessageType;
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CImpsSubSession::DeleteRequest
// -----------------------------------------------------------------------------
TBool CImpsSubSession::DeleteRequest( TInt aOpId )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DeleteRequest opid=%d, subses=%d" ), aOpId, ( TInt )this );
#endif
    // Delete one buffered requests from this client,
    TDblQueIter<CRequest> rIter( iRequestList );

    rIter.SetToFirst();

    while ( rIter )
        {
        CRequest* requ = rIter;
        rIter++;
        if ( requ->iOpId == aOpId )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "SubSession: DeleteRequest TID=%S" ), &( requ->iTID ) ); //lint !e525
#endif
            requ->Destroy();
            return ETrue;
            }
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: DeleteRequest failed" ) );
#endif
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsSubSession::NextEventL()
// ---------------------------------------------------------
void CImpsSubSession::NextEventL( const RMessage2& aMsg )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: NextEventL begins subses=%d" ), ( TInt )this );
#endif
    if ( iCanceled )
        {
        User::Leave( KErrCancel );
        }
    StoreEventMsg( aMsg );
    DoNextEvent();
    }

// ---------------------------------------------------------
// CImpsSubSession::SendEventBodyL()
// ---------------------------------------------------------
void CImpsSubSession::SendEventBodyL( const RMessage2& aMsg )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: SendEventBodyL subses=%d" ), ( TInt )this );
#endif
    if ( iCanceled )
        {
        User::Leave( KErrCancel );
        }

    StoreEventMsg( aMsg );
    DoEventBodyL();
    }

// ---------------------------------------------------------
// CImpsSubSession::DoCreateEventL
// ---------------------------------------------------------
void CImpsSubSession::DoCreateEventL(
    TInt aOpId,
    TImpsServRequest aRequestType,
    TImpsMessageType aReqMesType,
    CImpsFields* aFields )
    {

#ifdef _DEBUG
    TInt rate = 0;
    if ( !rate )
        {
        __UHEAP_RESET;
        }
    else
        {
        __UHEAP_FAILNEXT( rate );
        }
#endif

    // Create an event
    CWvEvent* myEvent = new ( ELeave ) CWvEvent( iServiceType );
    CleanupStack::PushL( myEvent );     // <<< myEvent
    myEvent->iOpCode = aOpId;
    myEvent->iStatus = aFields->Status();
    myEvent->iRequestType = aRequestType;
    myEvent->iMessageType = ( TImpsMessageType )aFields->MessageType();
    myEvent->iReqMesType = aReqMesType;

    // convert CSP error code if any
    TInt32 resstatus = aFields->ResponseStatusL();
    if ( resstatus != KImpsStatusOk &&
         resstatus != KErrNone )
        {
        myEvent->iStatus = ErrorCode( resstatus );
        }

    TImpsMessageType msgType = ( TImpsMessageType )myEvent->iMessageType;

    // *********************************************************
    // Handle pure client responses in different way
    // In that case XML-formatted response needed in a client
    if ( iServiceType == EImpsEventPresencePure )
        {
        if ( msgType == EImpsStatus ||
             impsService( iVariant, TImpsMessageType( msgType ) ) == EImpsEventPresence )
            {
            myEvent->iMessageType = EImpsPureData;
            msgType = EImpsPureData;
            }
        }
    //*********************************************************/

    // List of responses having serialized CImpsFields
    // Notice: Check this list if new methods in API presented
    if ( // im client
        msgType == EImpsNewMessage ||
        msgType == EImpsGetBlockedListRes ||
        msgType == EImpsSendMessageRes ||
        msgType == EImpsDeliveryReportReq ||
        // group client
        msgType == EImpsJoinGroupRes ||
        msgType == EImpsLeaveGroupRes ||
        msgType == EImpsGroupMembersRes ||
        msgType == EImpsGroupPropertiesRes ||
        msgType == EImpsGroupRejectListRes ||
        msgType == EImpsNewUsers ||
        msgType == EImpsLeftUsers ||
        msgType == EImpsGroupChangeNotice ||
        // Pure data
        msgType == EImpsPureData ||
        // fundamental
        impsService( iVariant, TImpsMessageType( msgType ) ) == EImpsEventCommon ||
        // presence
        impsService( iVariant, TImpsMessageType( msgType ) ) == EImpsEventPresence )
        // access service has nothing to pack in responses
        {
        HBufC8* dataBuffer = NULL;
        TRAPD( errx, ( dataBuffer = HBufC8::NewL( aFields->Size() ) ) );
        if ( errx )
            {
            myEvent->iStatus = errx;
            myEvent->iPackedMessage = NULL;
            }
        else
            {
            TImpsPackedEntity packed( dataBuffer );
            // Check data length if enough
            User::LeaveIfError ( packed.PackEntity( *aFields ) );
            myEvent->iPackedMessage = dataBuffer;
            }
        }

    // detailed error data code in special format
    // notice: avoid to pack in vain
    if ( msgType == EImpsStatus && iDetailedError )
        {
        HBufC8* dataBuffer = HBufC8::NewLC( aFields->Size() );
        TImpsPackedEntity packed( dataBuffer );
        // Check data length if enough
        User::LeaveIfError ( packed.PackEntity( *aFields ) );
        myEvent->iPackedMessage = dataBuffer;
        CleanupStack::Pop();    // dataBuffer
        }

    // list of responses having one byte response
    if ( aFields->MessageType() == EImpsSubsGroupNoticeRes )
        {
        CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey
        CImpsDataAccessor* myAc = CImpsDataAccessor::NewL( aFields );
        CleanupStack::PushL( myAc );            // <<< myAc
        TBool groupNotice = TImpsSDataUtils::SubGroupNoticeResponseL(
                                myKey,
                                myAc );
        myEvent->iAux = groupNotice;
        CleanupStack::PopAndDestroy( 2 );   // >>> myKey, myAc
        }

    // New event created succesfully
    CleanupStack::Pop( 1 ); // >>> myEvent
    // Since WV 1,2 logout messages may be either disconnect or Status,
    // harmomize them here in order to avoid sending logout message
    // with opid=actual_id and opid=0.
    if ( aRequestType == EImpsServWVLogout )
        {
        iLastEvent = EImpsDisconnect;
        }
    else
        {
        iLastEvent = myEvent->iMessageType;
        }
    iEventList.AddLast( *myEvent );
    }

// ---------------------------------------------------------
// CImpsSubSession::DoCreateEventL
// This creates wv engine online status event
// ---------------------------------------------------------
void CImpsSubSession::DoCreateEventL(
    EImpsInternalStatus aStatus )
    {
#ifdef _DEBUG
    TInt rate = 0;
    if ( !rate )
        {
        __UHEAP_RESET;
        }
    else
        {
        __UHEAP_FAILNEXT( rate );
        }
#endif
    // Create an event
    CWvEvent* myEvent = new ( ELeave ) CWvEvent( iServiceType );
    // No need for CleanupStack, no leave before inserted to queue
    myEvent->iOpCode = 0;
    myEvent->iStatus = KErrNone;
    myEvent->iRequestType = EImpsServStatusReg;
    myEvent->iMessageType = EImpsMessageNone;
    myEvent->iReqMesType = 0;
    myEvent->iAux = aStatus;

    // New event created succesfully
    iEventList.AddLast( *myEvent );

    // Notice: this does not update iLastEvent
    }

// ---------------------------------------------------------
// CImpSsubSession::DoCreateEventL
// This creates wv engine expiry events and other internal messages
// ---------------------------------------------------------
void CImpsSubSession::DoCreateEventL(
    TInt aOpCode,
    TInt aStatus,
    TImpsMessageType aReqMesType,
    TImpsMessageType aRcvMesType,
    TImpsServRequest aReqType )
    {
#ifdef _DEBUG
    TInt rate = 0;
    if ( !rate )
        {
        __UHEAP_RESET;
        }
    else
        {
        __UHEAP_FAILNEXT( rate );
        }
#endif
    // Create an event
    CWvEvent* myEvent = new ( ELeave ) CWvEvent( iServiceType );
    // No need for CleanupStack, no leave before inserted to queue
    myEvent->iOpCode = aOpCode;
    myEvent->iStatus = aStatus;
    myEvent->iMessageType = aRcvMesType;
    myEvent->iReqMesType = aReqMesType;
    myEvent->iRequestType = aReqType;

    // New event created succesfully
    iLastEvent = myEvent->iMessageType;
    iEventList.AddLast( *myEvent );
    }

// ---------------------------------------------------------
// CImpsSubSession::DoSplitGroupChangeL
// ---------------------------------------------------------
void CImpsSubSession::DoSplitGroupChangeL(
    TInt aOpId,
    TImpsServRequest aRequestType,
    CImpsFields* aFields )
    {

    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewL( aFields );
    CleanupStack::PushL( myAc );        // myAc

    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
    myKey->AddL( CREATEKEY( EImpsKeyGroupChangeNotice, 0 ) );

    // First get GroupId
    TDesC* pId;  // for GroupID
    myKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
    if ( myAc->RestoreDescL( myKey, pId ) )
        {
        myKey->PopL();
        }
    else
        {
        // This means illegal data from transport.
        User::Leave( EImpsCorrupted );
        }

    // Search if new users
    myKey->AddL( CREATEKEY( EImpsKeyJoined, 0 ) );
    if ( myAc->CheckBranchExistenceL( myKey ) )
        {
        CImpsFields* newFields = CImpsFields::NewL();
        CleanupStack::PushL( newFields );   // <<< newFields

        // Copy message type etc important
        newFields->SetStatus( aFields->Status( ) );

        // set special internal message type
        newFields->SetMessageType( EImpsNewUsers );

        // Copy data, only Joined users list
        CImpsDataAccessor* myAc2 = CImpsDataAccessor::NewLC( newFields );
        TImpsDataUtils::CopyNewUsersL( *myAc, *myAc2 );

        // Add Group Id
        myKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, 0 ) );
        myAc2->StoreDescL( myKey, *pId );
        // Make a request entity into queue
        DoCreateEventL( aOpId, aRequestType, EImpsMessageNone, newFields );

        CleanupStack::PopAndDestroy( 2 );   // >>> myAc2, newFields
        }

    // Search if LeftUsers
    myKey->ReplaceLastL( CREATEKEY( EImpsKeyLeft, 0 ) );
    if ( myAc->CheckBranchExistenceL( myKey ) )
        {
        CImpsFields* newFields = CImpsFields::NewL();
        CleanupStack::PushL( newFields );   // <<< newFields

        // Copy message type etc important
        newFields->SetStatus( aFields->Status( ) );

        // set special internal message type
        newFields->SetMessageType( EImpsLeftUsers );

        // Copy data, only Left users list
        CImpsDataAccessor* myAc2 = CImpsDataAccessor::NewLC( newFields );
        TImpsDataUtils::CopyLeftUsersL( *myAc, *myAc2 );

        // Add Group Id
        myKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, 0 ) );
        myAc2->StoreDescL( myKey, *pId );

        // Make a request entity into queue
        DoCreateEventL( aOpId, aRequestType, EImpsMessageNone, newFields );

        CleanupStack::PopAndDestroy( 2 );   // >>> myAc2, newFields
        }

    CImpsFields* newFields = CImpsFields::NewL();
    CleanupStack::PushL( newFields );   // <<< newFields

    // Copy message type etc important
    newFields->SetStatus( aFields->Status( ) );

    // set special internal message type
    newFields->SetMessageType( EImpsGroupChangeNotice );

    // Search if GroupProperties
    CImpsDataAccessor* myAc2 = CImpsDataAccessor::NewLC( newFields );
    myKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupProperties, 0 ) );
    if ( myAc->CheckBranchExistenceL( myKey ) )
        {
        TImpsDataUtils::CopyGroupPropertiesL( *myAc, *myAc2 );
        }
    // Search if ownproperties
    myKey->ReplaceLastL( CREATEKEY( EImpsKeyOwnProperties, 0 ) );
    if ( myAc->CheckBranchExistenceL( myKey ) )
        {
        TImpsDataUtils::CopyOwnPropertiesL( *myAc, *myAc2 );
        }

    // Add Group Id
    myKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, 0 ) );
    myAc2->StoreDescL( myKey, *pId );

    // Make a request entity into queue
    DoCreateEventL( aOpId, aRequestType, EImpsMessageNone, newFields );

    CleanupStack::PopAndDestroy( 4 );     // >>> myAc2, newFields
    // myKey, myAc
    }

// ---------------------------------------------------------
// CImpsSubSession::NbrEvents
// ---------------------------------------------------------
TInt CImpsSubSession::NbrEvents()
    {
    TDblQueIter<CWvEvent> eventIter( iEventList );
    CWvEvent* event = NULL;
    TInt counter = 0;

    eventIter.SetToFirst();
    event = eventIter;
    while ( event )
        {
        counter++;
        eventIter++;
        event = eventIter;
        }

    return counter;
    }

// ---------------------------------------------------------
// CImpsSubSession::GetWVDataL()
// Generic data converter from packed data to internal data structure.
// ---------------------------------------------------------
void CImpsSubSession::GetWVDataL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: GetWVDataL begins" ) );
#endif

    // serialized message is always in ptr0
    HBufC8* stream = *StreamBufAddr();
    if ( iSession->ReadBuffer8L( 0, stream ) )
        {
        HBufC8** stream2 = StreamBufAddr();
        *stream2 = stream;
        }
    ImpsFields()->Reset();
    TImpsPackedEntity packedMessage( *StreamBufAddr() );
    packedMessage.UnpackEntityL( *ImpsFields() );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: GetWVDataL ends" ) );
#endif
    }

// ---------------------------------------------------------
// CImpsSubSession::ErrorCode
// ---------------------------------------------------------
TInt CImpsSubSession::ErrorCode( TInt aCSPError )
    {

    if ( aCSPError >= 100 && aCSPError <= 999 )
        {
        // Convert the error code if valid input value
        return KImpsGeneralError - aCSPError;
        }
    // This means illagel data from transport.
    return KImpsErrorValidate;

    }

// ---------------------------------------------------------
// CImpsSubSession::DoCreateEventIPC
// ---------------------------------------------------------
void CImpsSubSession::DoCreateEventIPC(
    CWvEvent& aEvent, SImpsEventData* aData )
    {
    // Set pointers to the data
    aData->iOpCode = aEvent.iOpCode;
    aData->iMessageType = aEvent.iMessageType;
    aData->iRequestType = ( TInt ) aEvent.iRequestType;
    aData->iStatus = aEvent.iStatus;
    aData->iMessageBody = EFalse;
    aData->iReqMesType = aEvent.iReqMesType;
    // this is not used in all cases but let's write it every time
    aData->iAux = aEvent.iAux;

    // Make a special change for Logout SAP errors to hide the error code
    if ( iServiceType == EImpsEventServerLogin &&
         aEvent.iRequestType == EImpsServWVLogout &&
         aEvent.iStatus < ( Imps_ERROR_BASE - 200 ) )
        {
        aData->iStatus = KErrNone;
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L
                                ( "SubSession: DoCreateEventIPC converts LOGOUT code" ) );
#endif
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::DoWriteEventHeaders
// ---------------------------------------------------------
TInt CImpsSubSession::DoWriteEventHeaders(
    RMessagePtr2 aMsg, const TDes& aData )
    {

    TInt err = KErrNone;

    TPtrC ptr( aData );

    err = aMsg.Write( 0, ptr );

    __ASSERT_DEBUG( err != KErrBadDescriptor,
                    aMsg.Panic( KImpsPanicCategory, EImpsCorrupted ) );

#ifndef _NO_IMPS_LOGGING_
    SImpsEventData* eventData = ( SImpsEventData* )aData.Ptr();
    CImpsClientLogger::Log( _L(
                                "SubSession: DoWriteEventHeaders opid=%d msg=0x%x status=%d err=%d subses=%d" ),
                            eventData->iOpCode, eventData->iMessageType, eventData->iStatus, err, ( TInt )this );
#endif

    return err;
    }

// ---------------------------------------------------------
// CImpsSubSession::DoWriteEventBody
// ---------------------------------------------------------
TInt CImpsSubSession::DoWriteEventBody(
    RMessagePtr2 aMsg, CWvEvent& aEvent )
    {
    // Write message body
    TPtrC8 ptr8( KNullDesC8 );
    if ( aEvent.iPackedMessage )
        {
        ptr8.Set( aEvent.iPackedMessage->Des() );
        }
    TInt mLen = aMsg.GetDesMaxLength( 1 );
    __ASSERT_DEBUG( mLen != KErrBadDescriptor,
                    aMsg.Panic( KImpsPanicCategory, EImpsCorrupted ) );
    __ASSERT_DEBUG( mLen >= ptr8.Length(),
                    aMsg.Panic( KImpsPanicCategory, EImpsCorrupted ) );
    if ( mLen < ptr8.Length() )
        {
        // This is an error situation in a client side
        return KErrBadHandle;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: EventMsg.Write2 subses=%d" ), ( TInt )this );
    TTime testTime1;
    testTime1.HomeTime();
#endif

    TInt err = aMsg.Write( 1, ptr8 );
    __ASSERT_DEBUG( err != KErrBadDescriptor,
                    aMsg.Panic( KImpsPanicCategory, EImpsCorrupted ) );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L(
                                "SubSession: DoWriteEventBody opid=%d msg=0x%x status=%d err=%d" ),
                            aEvent.iOpCode, aEvent.iMessageType, aEvent.iStatus, err );
#endif

    return KErrNone;
    }

// ---------------------------------------------------------
// CImpsSubSession::StoreEventMsg
// ---------------------------------------------------------
void CImpsSubSession::StoreEventMsg( RMessagePtr2 aMsg )
    {
#ifndef _NO_IMPS_LOGGING_
    RMessage2 myMsg( aMsg );
    CImpsClientLogger::Log( _L( "SubSession: StoreEventMsg subses=%d bufSize=%d" ), ( TInt )this, myMsg.Int2() );
#endif
    iEventMsg = aMsg;
    }

// ---------------------------------------------------------
// CImpsSubSession::CompleteEventMsg
// ---------------------------------------------------------
void CImpsSubSession::CompleteEventMsg( TInt aStatus )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: CompleteEventMsg size=%d subses=%d" ), aStatus, ( TInt )this );
#endif
    if ( iEventMsg.IsNull() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: CompleteEventMsg NULL MSG subses=%d" ), ( TInt )this );
#endif
        return;
        }
    iEventMsg.Complete( aStatus );
    }

// ---------------------------------------------------------
// CImpsSubSession::EventMsgBufSize
// ---------------------------------------------------------
TInt CImpsSubSession::EventMsgBufSize( ) const
    {
    if ( iEventMsg.IsNull() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: EventMsgBufSize NULL MSG subses=%d" ), ( TInt )this );
#endif
        return 0;
        }
    RMessage2 myMsg( iEventMsg );
    return myMsg.Int2();
    }

// ---------------------------------------------------------
// CImpsSubSession::ConvertStatusCode
// ---------------------------------------------------------
TInt CImpsSubSession::ConvertStatusCode( EImpsInternalStatus aStatus ) const
    {
    TInt newStatus = KImpsOnlineStatus;
    if ( aStatus == EInternal_OFF_LINE )
        {
        // This is not supported in client API anymore, so this is an error code.
        newStatus = KImpsOfflineStatus;
        }
    else if ( aStatus == EInternal_NOT_LOGGED )
        {
        newStatus = KImpsNotLoggedStatus;
        }
    else if ( aStatus == EInternal_NO_IAP )
        {
        newStatus = KImpsNoIapStatus;
        }
    else if ( aStatus == EInternal_SHUTTING_DOWN )
        {
        newStatus = KImpsErrorShuttingDown;
        }
    return newStatus;
    }

// ---------------------------------------------------------
// CImpsSubSession::CancelTrans
// ---------------------------------------------------------
TBool CImpsSubSession::CancelTrans(
    RMessagePtr2 aMsg, TImpsSessIdent aCSP )
    {
    // Get operation id from client request messages
    RMessage2 myMsg( aMsg );
    TInt opid = myMsg.Int0();
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: CancelTransL opid=%d subses=%d" ), opid, ( TInt )this );
#endif

    TBuf<KImpsMaxTID> myTid;

    // delete the request
    // If there is no such opid then return an error code
    TBool found( EFalse );
    TDblQueIter<CRequest> requestIter( iRequestList );
    requestIter.SetToFirst();
    while ( requestIter )
        {
        CRequest* request = requestIter;
        requestIter++;
        if ( request->iOpId == opid )
            {
            myTid = request->iTID;
            // entry found and deleted
            request->Destroy();
            found = ETrue;
            break;
            }
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "SubSession: CancelTransL returns %d" ), found );
#endif
    if ( found )
        {
        // ok, the session cancels the transport and completes the request
        Server()->CancelTrans( myTid, aCSP );
        }
    return found;
    }

// ---------------------------------------------------------
// CImpsSubSession::SetExpiryTime
// ---------------------------------------------------------
void CImpsSubSession::SetExpiryTime( RMessagePtr2 aMsg )
    {
    RMessage2 myMsg( aMsg );
    TInt time = myMsg.Int0();
    SetExpiryTime( time );
    }

// ---------------------------------------------------------
// CImpsSubSession::SetExpiryTime
// ---------------------------------------------------------
void CImpsSubSession::SetExpiryTime( TInt aVal )
    {
    if ( aVal > 0 )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "SubSession: SetExpiryTime %d" ), aVal );
#endif
        iExpiryTime = aVal;
        Server()->SetExpiryTimer( aVal, EFalse );
        }
    }

// ---------------------------------------------------------
// CImpsSubSession::ExpiryTime
// ---------------------------------------------------------
TInt CImpsSubSession::ExpiryTime( )
    {
    return iExpiryTime;
    }

// ---------------------------------------------------------
// TImpsOOMErrors::StoreOOM
// ---------------------------------------------------------
void TImpsOOMErrors::StoreOOM( TInt aOpCode )
    {
    // FIFO, a ring buffer
    iOOMList[iW].SetOpId( aOpCode );
    TInt newW = ( iW + 1 ) % KImpsMaxBuffered;
    if ( newW != iR )
        {
        // overflow, don't increase the index.
        // the last free write cell is re-used as needed
        iW = newW;
        }
    }

// ---------------------------------------------------------
// ImpsOOMErrors::GetOOM
// ---------------------------------------------------------
TInt TImpsOOMErrors::GetOOM()
    {
    if ( iR != iW )
        {
        iOOMList[iR].SetAsSent();
        return iOOMList[iR].OpId();
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------
// ImpsOOMErrors::RemoveOOM
// ---------------------------------------------------------
TInt TImpsOOMErrors::RemoveOOM()
    {
    if ( iR != iW && iOOMList[iR].IsSent() )
        {
        TInt ret = iOOMList[iR].OpId();
        iR = ( ++iR ) % KImpsMaxBuffered;
        return ret;
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------
// ImpsOOMErrors::Exists
// ---------------------------------------------------------
TBool TImpsOOMErrors::Exists()
    {
    if ( iR != iW /*&& !iOOMList[iR].IsSent()*/ )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// TImpsOOMErrors::Reset
// ---------------------------------------------------------
void TImpsOOMErrors::Reset()
    {
    iR = 0;
    iW = 0;
    }

// ---------------------------------------------------------
// TImpsOOMError::TImpsOOMError
// ---------------------------------------------------------
TImpsOOMError::TImpsOOMError()
    {
    iOpId = 0;
    iSent = EFalse;
    }

// ---------------------------------------------------------
// TImpsOOMError::SetOpId
// ---------------------------------------------------------
void TImpsOOMError::SetOpId( TInt aOpId )
    {
    iOpId = aOpId;
    iSent = EFalse;
    }

// ---------------------------------------------------------
// TImpsOOMError::OpId
// ---------------------------------------------------------
TInt TImpsOOMError::OpId()
    {
    return iOpId;
    }

// ---------------------------------------------------------
// TImpsOOMError::SetAsSent
// ---------------------------------------------------------
void TImpsOOMError::SetAsSent( )
    {
    iSent = ETrue;
    }

// ---------------------------------------------------------
// TImpsOOMError::IsSent
// ---------------------------------------------------------
TBool TImpsOOMError::IsSent( )
    {
    return iSent;
    }


//  End of File
