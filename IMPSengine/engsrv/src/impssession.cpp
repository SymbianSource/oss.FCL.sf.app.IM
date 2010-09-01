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
* Description: Class for Imps session.
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
#include    "impssubsession.h"
#include    "ImpsVariantAPI.h"
#include    "impspacked.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsSession::CImpsSession( RMessage2& aMsg )
        :     CSession2(),
        iCanceled( EFalse ),
        iMessageCompleted( EFalse ),
        iMsgR( aMsg )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: NEW session sess=%d" ), ( TInt )this );
#endif
    }


// Destructor
// RHandleBase::Close() causes this destructor to be called
CImpsSession::~CImpsSession()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: DELETE session sess=%d" ), ( TInt )this );
#endif
    // Check if destructor is called by CServer2 class in failed
    // entity creation or in other case. iContainer is the last
    // member created is CreateL() so it can be used for this purpose.
    TBool wasComplete = iContainer ? ETrue : EFalse;
    // This is here to close CSP session if client is deleted
    // without calling close method.
    CloseSession();
    TImpsSessIdent csp( SID(), SAP(), UserId() );
    Server()->CloseSession( csp, wasComplete );
    delete iApplicationId;
    delete iSID;
    delete iSAP;
    delete iUserId;
    delete iStream;
    delete iFields;
    }

// ---------------------------------------------------------
// CImpsSession::CreateL()
// ---------------------------------------------------------
// This overloads the base class implementation
void CImpsSession::CreateL( )
    {
    iFields = CImpsFields::NewL();
    // create new object index
    iSubSessions = CObjectIx::NewL();
    // initialize the object container using the object container index in the server.
    iContainer = Server()->NewContainerL();
    }

// ---------------------------------------------------------
// CImpsSession::DispatchMessageL()
// ---------------------------------------------------------
void CImpsSession::DispatchMessageL( const RMessage2& aMessage )
    {
    TInt nbrSess = 0;
    TUint flag = 0;

    iMsgR = aMessage;

    if ( iCanceled )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Session: already CANCELED aMsg=%d" ),
                                aMessage.Function() );
#endif
        CompleteMe( KErrCancel );
        return;
        }

    // Set ETrue at the same time as Message().Complete() takes place
    iMessageCompleted = EFalse;
    TInt msg = aMessage.Function();
    // stripp off bit mask in an upper word
    msg = msg & 0x0FFFF;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: DispatchMessageL msg=%d sess=%d" ), msg, ( TInt )this );
#endif

    // current CSP indetification
    TImpsSessIdent csp( SID(), SAP(), UserId() );

    switch ( msg )
        {
        case EImpsServAssign:
            AssignIdL();
            // Notice that in this case there is subsession specific part too.
            // So, do not return yet.
            break;

        case EImpsAccessRegister2:
            // NewSubSessionL will check SHUTTING_DOWN state of server and
            // leaves when necessary
            NewSubSessionL( EImpsEventServerLogin, aMessage );
            return;

        case EImpsImRegister:
            NewSubSessionL( EImpsEventMessage, aMessage );
            return;

        case EImpsFundRegister:
            NewSubSessionL( EImpsEventCommon, aMessage );
            return;

        case EImpsGroupRegister:
            NewSubSessionL( EImpsEventGroup, aMessage );
            return;

        case EImpsServPureRegister:
            {
            NewSubSessionL( EImpsEventPresencePure, aMessage );
            }
        return;

        case EImpsServIsLogged:
            // This has to return positive value is CSP is found
            // and it has got SID.
            if ( IsThisLogged() )
                {
                CompleteMe( KErrNone );
                }
            else
                {
                CompleteMe( KImpsErrorNotLogged );
                }
            return;

        case EImpsServNbrSessions:
            nbrSess = NbrOfSessionsL( );
            CompleteMe( nbrSess );
            return;

        case EImpsServServices:
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "Session: EImpsServServices" ) );
#endif
            CurrentServicesL();
            return;

        case EImpsServPush:
            // multi-csp supported, i.e. CIR given for every CSP session
            // until sessoin cookie matches
            if ( Server()->IsShuttingDown() )
                {
                User::Leave( KImpsErrorShuttingDown );
                }
            PushMessageL();
            iCIRSess = ETrue;
            return;

        case EImpsServBuffSizeReq:
            CompleteMe( IsThisLogged() ? Server()->BufferSize() : 0 );
            return;

        case EImpsServCloseAll:		// close all sessions
            Server()->LogoutAll();
            CompleteMe( KErrNone );
            return;
        }

    // subsession specific requests
    TInt myHandle = aMessage.Int3( );
    CImpsSubSession* sub = ConnectionFromHandle( myHandle );
    if ( !sub )
        {
        PanicClient( EImpsBadRequest );
        return;
        }

    TBool reactive( EFalse );

    switch ( msg )
        {

        case EImpsServAssign:
            sub->AssignIdL( csp );
            return;

            // completion of request made in ServiceL
        case EImpsServReactiveLogin:
            reactive = ETrue;
        case EImpsServWVLogin:
            if ( Server()->IsShuttingDown() )
                {
                User::Leave( KImpsErrorShuttingDown );
                }
            if ( !Server()->IsConAllowed() )
                {
                User::Leave( KImpsErrorTerminalOffLine );
                }
            // multi: check if this session is already logged in.
            if ( IsThisLogged() )
                {
                User::Leave( KImpsErrorAlreadyLogged );
                }
            iLogoutTID = KNullDesC;
            sub->LoginL( reactive );
            return;

            // case EImpsServWVLogoutOne:
        case EImpsServWVLogout:
            if ( Server()->IsShuttingDown() )
                {
                User::Leave( KImpsErrorShuttingDown );
                }
            if ( !IsThisActive() )
                {
                User::Leave( KImpsErrorNotLogged );
                }
            iLogoutTID = KNullDesC;
            iLogoutTID = sub->LogoutL( csp, EFalse );
            // The following is needed if no force logout
            // and a shared CSP exists.
            CompleteMe( KErrNone );
            return;

        case EImpsServWVSendOnly:
        case EImpsServBlock:
        case EImpsServGroup:
        case EImpsServPres:
        case EImpsServFundSearch:
        case EImpsServFundInvite:
            if ( Server()->IsShuttingDown() )
                {
                User::Leave( KImpsErrorShuttingDown );
                }
            // multi: check if this handle is logged
            if ( !IsThisLogged() )
                {
                User::Leave( KImpsErrorNotLogged );
                }
            else if ( !Server()->IsConAllowed() )
                {
                User::Leave( KImpsErrorTerminalOffLine );
                }
            sub->SendPrimitiveL( aMessage.Function(), csp );
            return;

        case EImpsServNextEvent:
            sub->NextEventL( aMessage );
            // This is completed later
            iMessageCompleted = ETrue;
            return;

        case EImpsServEventBody:
            sub->SendEventBodyL( aMessage );
            // This is completed later
            iMessageCompleted = ETrue;
            return;

        case EImpsServCloseSub:
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "Session: EImpsServCloseSub" ) );
#endif
            sub->Unregister();
            // remove subsession type
            flag = sub->Type();
            iTypes = ( iTypes & ~flag );
            CompleteMe( KErrNone );
            return;

        case EImpsServDeleteSub:
            // remove subsession entity
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "Session: EImpsServDeleteSub" ) );
#endif
            sub->DeleteSub();
            CompleteMe( KErrNone );
            return;

        case EImpsServGetBlocked:
            if ( Server()->IsShuttingDown() )
                {
                User::Leave( KImpsErrorShuttingDown );
                }
            // multi: check if this handle is logged in
            if ( !IsThisLogged() )
                {
                User::Leave( KImpsErrorNotLogged );
                }
            else if ( !Server()->IsConAllowed() )
                {
                User::Leave( KImpsErrorTerminalOffLine );
                }
            sub->SendGetBlockedL( csp );
            return;

        case EImpsServStatusReg:
            sub->SetStatusObserver( ETrue );
            CompleteMe( KErrNone );
            return;

        case EImpsServStatusUnreg:
            sub->SetStatusObserver( EFalse );
            CompleteMe( KErrNone );
            return;

        case EImpsServDetailedReg:
            sub->SetDetailedError( ETrue );
            CompleteMe( KErrNone );
            return;

        case EImpsServDetailedUnreg:
            sub->SetDetailedError( EFalse );
            CompleteMe( KErrNone );
            return;

        case EImpsServCspVersion:
            CompleteMe( Server()->CspVersion() );
            return;

        case EImpsServCancelLogin:	// cancel ongoing login
            if ( Server()->IsShuttingDown() )
                {
                User::Leave( KImpsErrorShuttingDown );
                }
            // Call logout with cancel parameter.
            ( void )sub->LogoutL( csp, ETrue );
            return;

        case EImpsServCancelTrans:	// cancel transaction
            // If the specified request is not found then just complete the current request.
            if ( !sub->CancelTrans( aMessage, csp ) )
                {
                CompleteMe( KErrNone );
                }
            else
                {
                // Complete with cancel if operation is cancelled
                CompleteMe( KErrCancel );
                }
            return;

        case EImpsServSetExpiry:
            sub->SetExpiryTime( aMessage );
            CompleteMe( KErrNone );
            return;

        default:
            PanicClient( EImpsBadRequest );
            return;
        }
    }


// ---------------------------------------------------------
// CImpsSession::NewSubSessionL
// ---------------------------------------------------------
// Create a new connection; pass back its handle via the message
// Ensures the object is cleaned up on failure
void CImpsSession::NewSubSessionL(
    TImpsEventType aType,
    const RMessage2& aMessage )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: NewSubSessionL sess=%d" ), TInt( this ) );
#endif
    // Reject request if server is SHUTTING_DOWN
    if ( Server()->IsShuttingDown() )
        {
        User::Leave( KImpsErrorShuttingDown );
        }
    CImpsSubSession* sub = CImpsSubSession::NewL( this, aType, aMessage );
    CleanupClosePushL( *sub );
    iContainer->AddL( sub );
    // Add subsession to object index, this returns unique handle
    TInt handle = iSubSessions->AddL( sub );
    // write the handle to client
    TPckg<TInt> handlePckg( handle );
    TRAPD( res, aMessage.WriteL( 3, handlePckg ) );
    if ( res != KErrNone )
        {
        // Removing sub from object index will
        // cause Close() to be called on sub, and that calls destructor
        CleanupStack::Pop();
        iSubSessions->Remove( handle );
        PanicClient( EImpsCorrupted );
        User::Leave( res );
        }
    sub->SetHandle( handle );
    // notch up another resource
    iResourceCount++;
    // update iTypes bit Mask
    iTypes = iTypes | aType;
    CleanupStack::Pop( );
    }

// ---------------------------------------------------------
// CImpsSession::ConnectionFromHandle()
// ---------------------------------------------------------
CImpsSubSession* CImpsSession::ConnectionFromHandle( TUint aHandle )
    {
    CImpsSubSession* sub = ( CImpsSubSession* )iSubSessions->At( aHandle );
    return sub;
    }

// ---------------------------------------------------------
// CImpsSession::DeleteSubSession
// ---------------------------------------------------------
void CImpsSession::DeleteSubSession( TUint aHandle )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: DeleteSubSession sess=%d" ), ( TInt )this );
#endif
    // panic if bad handle
    CImpsSubSession* sub = ConnectionFromHandle( aHandle );
    if ( sub == NULL )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Session: PANIC" ) );
#endif
        PanicClient( EImpsCorrupted );
        }

    // remove subsession
    iSubSessions->Remove( aHandle );
    // decrement resource count
    iResourceCount--;

#ifndef _NO_IMPS_LOGGING_
    TInt count = iSubSessions->ActiveCount();
    CImpsClientLogger::Log( _L( "Session: DeleteSubSession nbrsub=%d %d subses=%d" ),
                            count, iResourceCount,  ( TInt )sub );
#endif
    }

// ---------------------------------------------------------
// CImpsSession::MatchSession
// ---------------------------------------------------------
TBool CImpsSession::MatchSession( TImpsSessIdent aCSP )
    {
    // -----------------------------------
    // Notice: comment
    // extra stuff for debugging
    /*
    TPtrC pp1 = aCSP.SAP();
    TPtrC pp2 = aCSP.UserId();
    TPtrC pp3 = aCSP.SID();
    TPtrC ppa = SAP();
    TPtrC ppb = UserId();
    TPtrC ppc = SID();
    CImpsClientLogger::Log(_L("Session: MatchSession aCSP sap=%S user=%S sid=%S"), &pp1, &pp2, &pp3);
    CImpsClientLogger::Log(_L("Session: MatchSession this sap=%S user=%S sid=%S"), &ppa, &ppb, &ppc);
    */
    // -----------------------------------

    // The current session-id must match if it is non-zero
    if ( aCSP.SID().Length() > 0 &&
         SID().Length() > 0 &&
         aCSP.SID().CompareF( SID() ) )
        {
        return EFalse;
        }
    // Also user-id and SAP address must match
    else if ( !( SAP().CompareF( aCSP.SAP() ) ) &&
              !( UserId().CompareF( aCSP.UserId() ) ) )
        {
#ifndef _NO_IMPS_LOGGING_
        TPtrC p1 = SAP();
        TPtrC p2 = UserId();
        TPtrC p3 = aCSP.SID();
        CImpsClientLogger::Log( _L( "Session: MatchSession MATCH sess=%d sap=%S user=%S sid=%S" ),
                                ( TInt )this,  &p1, &p2, &p3 );
#endif
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CImpsSession::SendEvent()
// ---------------------------------------------------------
void CImpsSession::SendEvent(
    CImpsFields *aFields,
    TInt aOpId,
    TImpsServRequest aRequestType,
    TImpsMessageType aReqMsgType,
    TUint aHandle )
    {
    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return;
        }

    TInt errx = KErrNone;

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: SendEvent begins" ) );
#endif

    // specified subsession
    if ( aHandle )
        {
        CImpsSubSession* sub = ConnectionFromHandle( aHandle );
        if ( sub )
            {
            sub->SendEvent(
                aFields,
                aOpId,
                aRequestType,
                aReqMsgType );
            }
        }
    // scan subsessions
    else
        {
        // This is safe even if client would close session or subsession
        // Scheduler does not allocate time for it until this loop ends.
        TInt count = iSubSessions->Count();
        for ( TInt i = 0; i < count; i++ )
            {
            CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
            if ( sub )
                {
                TRAP( errx, sub->SendEvent(
                          aFields,
                          aOpId,
                          aRequestType,
                          aReqMsgType ) );
                }
            }
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: SendEvent ends" ) );
#endif
    }

// ---------------------------------------------------------
// CImpsSession::SendEvent()
// Lighter version for engine status event
// ---------------------------------------------------------
void CImpsSession::SendEvent(
    EImpsInternalStatus aStatus )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: SendEvent begins aStatus=%d" ), aStatus );
#endif

    if ( aStatus == EInternal_NOT_LOGGED || aStatus == EInternal_SHUTTING_DOWN )
        {
        // NOT_LOGGED event is sent after LogoutResponse and it cleans
        // the WV CSP TID, so that it will not receive any further
        // messages from transport.
        CleanSID();
        SetThisInactive();
        }

    // Scan subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub )
            {
            sub->SendEvent( aStatus );
            }
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: SendEvent ends" ) );
#endif
    }

// ---------------------------------------------------------
// CImpsSession::SendLogoutEvent
// ---------------------------------------------------------
void CImpsSession::SendLogoutEvent(
    TInt aRespStatus, TInt aOpId, CImpsSubSession* aSub,
    TUint aHandle )
    {
    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return;
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: SendLogoutEventL" ) );
#endif

    // specified subsession only
    if ( aHandle )
        {
        CImpsSubSession* sub = ConnectionFromHandle( aHandle );
        if ( sub )
            {
            sub->SendLogoutEvent( aRespStatus, aOpId );
            return;
            }
        }

    // Scan subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        // Igone a specific subsession
        if ( sub && sub != aSub )
            {
            sub->SendLogoutEvent( aRespStatus, aOpId );
            }
        }
    }

// ---------------------------------------------------------
// CImpsSession::CheckNotification()
// ---------------------------------------------------------
TBool CImpsSession::CheckNotifications(
    CImpsFields* aFields, TImpsSessIdent aCSP )
    {
    TBool retVal( EFalse );
    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    // Multi: Validate the CSP connection id and check that
    // client has not completed logout request.
    if ( iCanceled || !MatchSession( aCSP ) || !IsThisActive() )
        {
        return retVal;
        }

    TInt myType = 0;
    TInt myOpId = 0;
    // Notice: client thread use this information
    if ( aFields->MessageType() == EImpsDisconnect )
        {
        myType = EImpsServWVLogout;
        }


    // Scan subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub && sub->CheckNotification( aFields ) )
            {
            retVal = ETrue;
            sub->SendEvent(
                aFields, myOpId, ( TImpsServRequest )myType, EImpsMessageNone );
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// CImpsSession::HandleAllOrphans()
// ---------------------------------------------------------
void CImpsSession::HandleAllOrphans()
    {
    // current CSP indetification
    TImpsSessIdent csp( SID(), SAP(), UserId() );
    // Scan subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub )
            {
            sub->HandleAllOrphans( csp );
            }
        }
    }

// -----------------------------------------------------------------------------
// CImpsSession::CloseSession()
// -----------------------------------------------------------------------------
void CImpsSession::CloseSession()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: CloseSession" ) );
#endif

    iCanceled = ETrue;
    DeleteAllRequests();
    void DeleteAllEvents();
    // Delete the object index (this stores the objects for this session)
    delete iSubSessions;
    iSubSessions = NULL;
    Server()->RemoveContainer( iContainer );
    iContainer = NULL;
    }

// ---------------------------------------------------------
// CImpsSession::CheckRequests()
// ---------------------------------------------------------
TUint CImpsSession::CheckRequests(
    const TDesC& aTid,
    TInt& aOpId,
    TImpsServRequest& aRequestType,
    TImpsMessageType& aReqMsgType,
    TImpsSessIdent aCSP )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    if ( iCanceled )
        {
        return 0;
        }

    // Check also that the CSP session matches
    if ( !MatchSession( aCSP ) )
        {
        return 0;
        }

    // Scan subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub && sub->CheckRequests(
                 aTid,
                 aOpId,
                 aRequestType,
                 aReqMsgType ) )
            {
            // Check if a successful response for login
            if ( SID().Length() == 0 &&
                 aCSP.SID().Length() > 0 &&
                 aRequestType == EImpsServWVLogin )
                {
                // If OOM here then this returns 0 and expiry routines
                // handles the request later on. This is extremely
                // rare situation anyway.
                TRAPD( errx, ModifySIDL( aCSP.SID() ) );
                if ( errx )
                    {
                    return 0;
                    }
                }
            return sub->Handle();
            }
        }

    return 0;
    }

// ---------------------------------------------------------
// CImpsSession::DiscardRequests()
// ---------------------------------------------------------
void CImpsSession::DiscardRequests(
    TTime aExpiryTime,
    TImpsEventType aServiceType,
    TImpsSessIdent aCSP,
    MImpsCSPSession* aSess )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    // Multi: Validate the CSP connection too.
    if ( iCanceled || !MatchSession( aCSP ) )
        {
        return;
        }

    // Scan subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub )
            {
            sub->DiscardRequests( aExpiryTime, aServiceType, aSess );
            }
        }
    }

// ---------------------------------------------------------
// CImpsSession::DiscardRequests()
// ---------------------------------------------------------
void CImpsSession::DiscardRequests(
    TInt aError,
    TImpsEventType aServiceType,
    TImpsSessIdent aCSP,
    MImpsCSPSession* aSess )
    {

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    // Multi: Validate the CSP connection too.
    if ( iCanceled || !MatchSession( aCSP ) )
        {
        return;
        }

    // Scan subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub )
            {
            sub->DiscardRequests( aError, aServiceType, aSess );
            }
        }
    }

// ---------------------------------------------------------
// CImpsSession::DiscardRequest()
// ---------------------------------------------------------
TBool CImpsSession::DiscardRequest(
    const TDesC& aTid,
    TImpsEventType aServiceType,
    TInt aCode,
    TImpsSessIdent aCSP )
    {

    TBool ret( EFalse );

    // It is possible that the cancel request from client is not
    // complete and this session entity is still waiting deletion.
    // Multi: Validate the CSP connection too.
    if ( iCanceled || !MatchSession( aCSP ) )
        {
        return EFalse;
        }

    // Scan subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub )
            {
            ret = sub->DiscardRequest( aTid, aServiceType, aCode );
            if ( ret )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CImpsSession::DeleteAllRequests()
// -----------------------------------------------------------------------------
void CImpsSession::DeleteAllRequests( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: DeleteAllRequests" ) );
#endif
    if ( iCanceled )
        {
        return;
        }

    // Delete all buffered requests from this client.
    // Delete from subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub )
            {
            sub->DeleteAllRequests();
            }
        }

    }

// -----------------------------------------------------------------------------
// CImpsSession::DeleteAllRequests()
// -----------------------------------------------------------------------------
void CImpsSession::DeleteAllRequests( TImpsSessIdent* aCSP )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: DeleteAllRequests aCSP" ) );
#endif
    if ( iCanceled )
        {
        return;
        }

    if ( !MatchSession( *aCSP ) )
        {
        return;
        }

    // Delete all buffered requests from this client.
    // Delete from subsessions
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub )
            {
            sub->DeleteAllRequests();
            }
        }

    }

// -----------------------------------------------------------------------------
// CImpsSession::DeleteRequest
// -----------------------------------------------------------------------------
void CImpsSession::DeleteRequest( TInt aOpId, TUint aSubHandle )
    {

    if ( aSubHandle )
        {
        CImpsSubSession* sub = ConnectionFromHandle( aSubHandle );
        if ( sub )
            {
            sub->DeleteRequest( aOpId );
            }
        return;
        }
    }

// -----------------------------------------------------------------------------
// CImpsSession::ExpiryTime
// -----------------------------------------------------------------------------
TInt CImpsSession::ExpiryTime( )
    {
    if ( iCanceled )
        {
        return 0;
        }

    // Search the minimum subsession expiry time.
    TInt sesexp = 0;
    TInt exp = 0;
    TInt count = iSubSessions->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CImpsSubSession* sub = ( CImpsSubSession* )( ( *iSubSessions )[i] );
        if ( sub )
            {
            exp = sub->ExpiryTime();
            if ( ( exp && exp < sesexp ) || !sesexp )
                {
                sesexp = exp;
                }
            }
        }
    return sesexp;
    }

// ---------------------------------------------------------
// CImpsSession::CompleteMe()
// ---------------------------------------------------------
void CImpsSession::CompleteMe( TInt aStatus )
    {
    iMsgR.Complete( aStatus );
    iMessageCompleted = ETrue;
    }

// ---------------------------------------------------------
// CImpsSession::PushMessageL()
// ---------------------------------------------------------
void CImpsSession::PushMessageL()
    {
    // read cookie
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: PushMessageL" ) );
#endif
    if ( !iStream )
        {
        iStream = HBufC8::NewL( KImpsMaxCookie );
        }

    // server instance saves these after successful login
    ( void ) ReadBuffer8L(  1, iStream );
    Server()->CirMessageL( iStream->Des() );
    }


// ---------------------------------------------------------
// CImpsSession::ServiceL()
// ---------------------------------------------------------

void CImpsSession::ServiceL( const RMessage2& aMessage )
    {
    TRAPD( err, DispatchMessageL( aMessage ) );
    if ( !iMessageCompleted )
        {
        CompleteMe( err );
        }
    }

// ---------------------------------------------------------
// CImpsSession::PanicClient()
// ---------------------------------------------------------
void CImpsSession::PanicClient( TImpsPanic aPanic ) const
    {
    iMsgR.Panic( KImpsPanicCategory, aPanic );
    }

// ---------------------------------------------------------
// CImpsSession::CurrentServicesL()
// ---------------------------------------------------------
void CImpsSession::CurrentServicesL()
    {

    TImpsSessIdent csp( SID(), SAP(), UserId() );
    if ( !Server()->IsNegotiated( csp ) )
        {
        User::Leave( KImpsErrorServices );
        }

    // Buffer for response
    TBuf < sizeof( TImpsServices ) + 4 >
    myBuf( sizeof( TImpsServices ) + 4 );
    TImpsServices myServices;
    myServices.Reset();

    TImpsServices* activeServices = Server()->Services( csp );
    if ( !activeServices )
        {
        User::Leave( KImpsErrorServices );
        }
    myServices.Copy( *activeServices );

    // Copy service tree to the local descriptor buffer
    const TUint8* aPtrStart = ( const TUint8* )myBuf.Ptr();
    TInt32 tempSize = 0;
    tempSize = sizeof( TImpsServices );
    Mem::Copy( ( void* )aPtrStart, &tempSize, sizeof( tempSize ) );
    aPtrStart = aPtrStart + sizeof( tempSize );
    if ( tempSize )
        {
        Mem::Copy( ( void* )aPtrStart, ( void* )&myServices, tempSize );
        }
    TInt ret = iMsgR.Write( 0, myBuf );
    // complete the request
    CompleteMe( ret );

    }

// ---------------------------------------------------------
// CImpsSession::ReadBuffer8L()
// aBuffer must be a member if there is a risk that
// more memory must be allocated, because of cleanupstack is
// inconsistent otherwise.
// ---------------------------------------------------------
TBool CImpsSession::ReadBuffer8L(
    TInt aIndex,
    HBufC8*& aBuffer )
//
// Copies a buffer from the client
// Only fails if there is not enough memory to increase the buffer size
//
    {
    TBool alloced = EFalse;
    TInt desLen = iMsgR.GetDesLength( aIndex );

    if ( desLen < 0 )
        {
        User::Leave( desLen );
        }

    HBufC8* newBuffer = NULL;

    if ( aBuffer == NULL )
        {
        newBuffer = HBufC8::NewLC( desLen );
        alloced = ETrue;
        }
    else if ( desLen > aBuffer->Des().MaxLength() )
        {
        alloced = ETrue;
        // we have to increase the size of buffer
        aBuffer->Des().SetLength( 0 ); // to avoid copying the contents
        newBuffer = aBuffer->ReAllocL( desLen  );
        CleanupStack::PushL( newBuffer );
        }

    TPtr8 desPtr = alloced ? newBuffer->Des() : aBuffer->Des();  //lint !e613 !e661
    // newBuffer is allocated, aBuffer must not be zero
    iMsgR.ReadL( aIndex, desPtr );

    if ( alloced )
        {
        aBuffer = newBuffer;
        CleanupStack::Pop();
        }

    return alloced;

    }

// ---------------------------------------------------------
// CImpsSession::ApplicationId()
// ---------------------------------------------------------
TPtrC CImpsSession::ApplicationId()
    {
    return iApplicationId ? TPtrC( *iApplicationId ) : TPtrC();
    }

// ---------------------------------------------------------
// CImpsSession::SetUserIdL()
// ---------------------------------------------------------
void CImpsSession::SetUserIdL( const TDesC& aId )
    {
    delete iUserId;
    iUserId = NULL;
    iUserId = aId.AllocL();
    }

// ---------------------------------------------------------
// CImpsSession::SetSAPL()
// ---------------------------------------------------------
void CImpsSession::SetSAPL( const TDesC& aId )
    {
    delete iSAP;
    iSAP = NULL;
    iSAP = aId.AllocL();
    }

// ---------------------------------------------------------
// CImpsSession::SetThisInactive()
// Use iUserId to check whether client session is logged in
// ---------------------------------------------------------
void CImpsSession::SetThisInactive()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: SetThisInactive sess=%d" ), ( TInt )this );
#endif
    delete iUserId;
    iUserId = NULL;
    }

// ---------------------------------------------------------
// CImpsSession::NewFieldsL()
// Old iFields must not be deleted because of CImpsServer
// still uses that.
// ---------------------------------------------------------
void CImpsSession::NewFieldsL()
    {
    CImpsFields* newF = CImpsFields::NewL();
    iFields = newF;
    }

// ---------------------------------------------------------
// CImpsSession::AssignIdL()
// ---------------------------------------------------------
void CImpsSession::AssignIdL()
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Session: AssignIdL begins" ) );
#endif
    // Generate session's application id and client id
    RMessage2 myMsg( iMsgR );
    // verify that length is valid
    TInt desLen = myMsg.GetDesLength( 0 );
    if ( desLen > KImpsMaxClientId )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Session: Error: too long client-id" ) );
#endif
        User::Leave( KImpsErrorValidate );
        }
    // allocate memory dynamically
    delete iApplicationId;
    iApplicationId = NULL;
    iApplicationId = HBufC::NewL( desLen );
    TPtr desPtr = iApplicationId->Des();
    myMsg.ReadL( 0, desPtr );
    }

// ---------------------------------------------------------
// CImpsSession::CleanSID()
// ---------------------------------------------------------
void CImpsSession::CleanSID()
    {
    if ( !iSID )
        {
        return;
        }
#ifndef _NO_IMPS_LOGGING_
    TPtr p = iSID->Des();
    CImpsClientLogger::Log( _L( "Session: CleanSID sess=%d sid=%S" ), ( TInt )this, &p );
#endif
    delete iSID;
    iSID = NULL;
    }

// ---------------------------------------------------------
// CImpsSession::ModifySIDL()
// ---------------------------------------------------------
void CImpsSession::ModifySIDL( const TDesC& aSID )
    {
#ifndef _NO_IMPS_LOGGING_
    TPtrC p = aSID;
    CImpsClientLogger::Log( _L( "Session: ModifySIDL sess=%d sid=%S" ), ( TInt )this, &p );
#endif
    delete iSID;
    iSID = NULL;
    iSID = aSID.AllocL();
    }

// ---------------------------------------------------------
// CImpsSession::NbrSessionsL( )
// ---------------------------------------------------------
TInt CImpsSession::NbrOfSessionsL()
    {
    RMessage2 myMsg( Message() );

    // Read PACKED ARRAY
    CDesCArrayFlat* tempArr = new ( ELeave )CDesCArrayFlat( 2 );
    CleanupStack::PushL( tempArr );   // << tempArr

    HBufC8* stream = *StreamBufAddr();
    if ( ReadBuffer8L( 0, stream ) )
        {
        HBufC8** stream2 = StreamBufAddr();
        *stream2 = stream;
        }
    TImpsPackedEntity packedMessage( stream );
    const TUint8* pS = stream->Ptr();
    packedMessage.DoUnpackArrayL( pS, tempArr );

    __ASSERT_DEBUG(
        tempArr->MdcaCount() == 2,
        User::Panic( KImpsPanicCategory,
                     EImpsCorrupted ) );

    TPtrC tempUser = tempArr->MdcaPoint( 1 );
    TPtrC tempSAP = tempArr->MdcaPoint( 0 );
    TImpsSessIdent csp( KNullDesC, tempSAP, tempUser );

    // ask number of clint sessions
    TInt ret =  Server()->NbrSessions( EFalse, csp );

    tempArr->Reset();
    CleanupStack::PopAndDestroy( 1 );   // >> tempArr

    return ret;
    }


// -----------------------------------------------------------------------------
// CWvEvent::CWvEvent
// -----------------------------------------------------------------------------

CWvEvent::CWvEvent( TImpsEventType aType ) :
        iType( aType ),
        iMessageType( EImpsStatus ), // just default
        iSent( EFalse ),
        iPackedMessage ( NULL )
    {}

CWvEvent::~CWvEvent()
    {
    if ( iPackedMessage )
        {
        delete iPackedMessage;
        }
    iPackedMessage = NULL;
    }

void CWvEvent::Destroy()
    {
    iLink.Deque();
    delete this;
    }

// -----------------------------------------------------------------------------
// CRequest::CRequest
// -----------------------------------------------------------------------------
CRequest::CRequest(
    const TDesC& aTID,
    TInt aOpId,
    TImpsServRequest aRequestType,
    TTime aExpiry,
    TImpsMessageType aMessageType )
        : iOpId( aOpId ),
        iRequestType ( aRequestType ),
        iMessageType ( aMessageType ),
        iExpiry( aExpiry )
    {
    iTID = aTID;
    }

CRequest::~CRequest()
    {

    }

void CRequest::Destroy()
    {
    iLink.Deque();
    delete this;
    }

//  End of File
