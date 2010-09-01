/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  One Session manager of the presence server
* 				 Handles all management concerning one network session
*  				 Opening, closing, ownership handling, updating
*  				 of Session related data to network
*
*/



// INCLUDE FILES
#include	<e32std.h>
#include    "CPEngCSPSessManager.h"

#include	"CPEngAppAccessContainer.h"
#include	"MPEngSubSession.h"

#include	"MPEngRequestHandler.h"
#include	"CPEngHandlerLogIn.h"
#include	"CPEngHandlerLogOff.h"
#include	"CPEngHandlerSendData.h"
#include	"MPEngOutgoingTransactionHandler.h"
#include	"CPEngIncomingDataHandler.h"


#include	"TPEngServerMessages.h"
#include	"MPEngServer.h"

#include	"CPEngSessionSlotId.h"
#include	"CPEngSessionSlotEvent.h"
#include	"PEngWVServices2.h"
#include	"RObjectArray.h"

#include	"PEngTransferAdapter.h"


// transaction factories
#include	"MPEngTransactionFactory.h"
#include	"PEngAttrLibFactory.h"
#include	"PEngListLibraryFactory.h"

// pure data handler
#include	"MPEngPureDataHandler.h"
#include	"MPEngAccessHandler.h"

//	Own RMessage class wraper
#include	"RPEngMessage.h"

// Debug prints
#include	"PresenceDebugPrint.h"



// CONSTANTS
const TInt KLastRefAccessCount = 1;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::CPEngCSPSessManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngCSPSessManager::CPEngCSPSessManager(
    MPEngServer& aServer )
        : iAccessCount( 1 ), // init access count to 1
        iState( EPEngNWPresenceSessionClosed ),
        iServer( aServer ),
        iRequestHandlers( 2 ),	// not many requests at time
        iTransFactories( 3 ) // there 3 three factories

    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::CPEngCSPSessManager" ) );
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::ConstructL(
    const CPEngSessionSlotId& aSessId,
    RPEngStorageAdminClient& aAdmin )
    {
    iSessionSlotId = aSessId.CloneL();

    iAccessCounters = CObjectCon::NewL();
    // we need to set Unique ID of the containers so they own their objects
    iAccessCounters->iUniqueID = reinterpret_cast<TInt> ( this );

    HBufC8* sessId = aSessId.PackLC();
    User::LeaveIfError( iStoreAdmin.Connect( aAdmin, *sessId ) );
    CleanupStack::PopAndDestroy( sessId ); // sessId
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngCSPSessManager* CPEngCSPSessManager::NewLC(
    MPEngServer& aServer,
    const CPEngSessionSlotId& aSessId,
    RPEngStorageAdminClient& aAdmin )
    {
    CPEngCSPSessManager* self = new( ELeave ) CPEngCSPSessManager( aServer );

    CleanupClosePushL( *self );
    self->ConstructL( aSessId, aAdmin );

    return self;
    }


// Destructor
CPEngCSPSessManager::~CPEngCSPSessManager()
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::~CPEngCSPSessManager" ) );

    delete iSessionSlotId;
    ResetAccessContainers();
    delete iAccessCounters;

    iRequestHandlers.ResetAndDestroy();
    // set state to the Close, so all resources are sure deleted
    iState = EPEngNWPresenceSessionClosed;
    UnloadResources();

    iIncomingHandlers.ResetAndDestroy();

    iStoreAdmin.Close();
    iServer.RemoveCSPSessManager( this );

    iServSubSessions.Reset();


#if _BullseyeCoverage
    cov_write();
#endif
    }

/////////////////////////////////////////////////////////////////////////////////
// ========== Functions from MPEngRequestHandlerObserver ========================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::CompleteRequestHandler()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::CompleteRequestHandler(
    MPEngRequestHandler* aRequestHandler )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::CompleteRequestHandler: %d" ),
             ( TInt )aRequestHandler  );

    TInt handlerIndex ( iRequestHandlers.Find( aRequestHandler ) );
    if ( handlerIndex != KErrNotFound )
        {
        iRequestHandlers.Remove( handlerIndex );
        }
    // if we are offline and there is no more request handler, unload
    // resources
    if (
        ( iState == EPEngNWPresenceSessionClosed )
        &&
        ( iRequestHandlers.Count() == 0 )
    )
        {
        UnloadResources();
        }
    CheckReferenceAccessD();
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::SetCSPSessionOpenedL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::SetCSPSessionOpenedL()
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::SetCSPSessionOpenedL: %d" ) );

    // Load resources
    LoadResourcesL();
    // set state to online
    iState	= EPEngNWPresenceSessionOpen;
    }


/////////////////////////////////////////////////////////////////////////////////
// ========== Functions from MPEngRequestHandlerObserver ========================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::StatusChangedL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::StatusChangedL(
    TPEngNWSessionSlotState aNewState,
    TPEngNWSessionSlotEvent aNewEvent )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::StatusChangedL: state:%d Event:%d" ),
             aNewState,
             aNewEvent );


    // is tate different than holder is
    if ( aNewState == iState )
        {
        // state is same, do nothing
        return;
        }
    // update state and inform about it
    iState = aNewState;
    if ( iState == EPEngNWPresenceSessionClosed )
        {
        // we are closed, unload resources
        UnloadResources();
        TInt count( iAccessCounters->Count() );
        for ( TInt x( 0 ) ; x < count ; ++x )
            {
            NotifyNewEventL( aNewEvent,
                             static_cast<CPEngAppAccessContainer*>
                             ( ( *iAccessCounters )[ x ] )->AppId(),
                             EPEngNWPresenceSessionClosed );
            }
        // remove all access containers
        ResetAccessContainers();
        }
    else if ( iState == EPEngNWPresenceSessionOpen )
        {
        LoadResourcesL();
        // Do not notify, it will be done by LogIn Handler
        }
    }

/////////////////////////////////////////////////////////////////////////////////
//======================== New Functions of main class ==========================
/////////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::Open()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::Open()
    {
    iAccessCount++;
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::Close()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::Close()
    {
    iAccessCount--;
    CheckReferenceAccessD();
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::OpenL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::OpenL(
    MPEngSubSession& aServSubSession )
    {
    iServSubSessions.AppendL( &aServSubSession );
    iAccessCount++;
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::Close()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::Close(
    MPEngSubSession* aServSubSession )
    {
    TInt x( iServSubSessions.Find( aServSubSession ) );
    if ( x != KErrNotFound )
        {
        iServSubSessions.Remove( x );
        iAccessCount--;
        }
    CheckReferenceAccessD();
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::AttachAppIdL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::AttachAppIdL(
    const TDesC& aAppId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::AttachAppIdL:%S" ), &aAppId );


    // allow attaching of the App Id only if CSP is opened
    LeaveIfNotConnectedL();

    CPEngAppAccessContainer* accessCounter = AccessCounter( aAppId );
    if ( accessCounter )
        {
        TInt val = accessCounter->Open(); // CSI: 65 #
        return;
        }
    // does not exist, create and append
    CPEngAppAccessContainer* newContainer =
        CPEngAppAccessContainer::NewLC( aAppId );
    iAccessCounters->AddL( newContainer );
    // notify about new App Id logging
    NotifyNewEventL( EPEngEventAppNWPresenceSessionOpened,
                     aAppId,
                     EPEngNWPresenceSessionOpen );
    CleanupStack::Pop( newContainer ); // newContainer
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::DetachAppId()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::DetachAppId(
    const TDesC& aAppId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::DetachAppId:%S " ), &aAppId );

    CPEngAppAccessContainer* accessCounter = AccessCounter( aAppId );
    TBool notify( EFalse );
    if ( accessCounter )
        {
        notify = accessCounter->CloseAccess();
        }
    if ( iState == EPEngNWPresenceSessionClosed )
        {
        // no need to notify, it was already done
        notify = EFalse;
        }
    // do we need to do Log Out from CSP?
    else if ( !( iAccessCounters->Count() ) )
        {
        // quite brutal, but unload everything, we cannot do lot more
        iRequestHandlers.ResetAndDestroy();
        UnloadResources();
        iState = EPEngNWPresenceSessionClosed;
        notify = ETrue;
        }
    // notify event
    if ( notify )
        {
        TRAP_IGNORE( NotifyNewEventL( EPEngEventAppNWPresenceSessionClosed,
                                      aAppId,
                                      EPEngNWPresenceSessionClosed ) );
        }
    CheckReferenceAccessD();
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::AppIdActiveL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::AppIdActiveL(
    const TDesC& aAppId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::AppIdActiveL:%S" ), &aAppId );

    CPEngAppAccessContainer* accessCounter = AccessCounter( aAppId );
    if ( !accessCounter )
        {
        User::Leave( KErrNotReady );
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::ReserveProcessL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::ReserveProcessL(
    const TDesC& aAppId,
    const TDesC& aProcessId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::ReserveProcessL: %S : %S" ),
             &aAppId,
             &aProcessId );

    CPEngAppAccessContainer* accessCounter = AccessCounter( aAppId );
    if ( accessCounter )
        {
        accessCounter->ReserveProcessL( aProcessId );
        return;
        }
    // does not exist, leave
    User::Leave( KErrNotFound );
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::ActivateProcessL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::ActivateProcessL(
    const TDesC& aAppId,
    const TDesC& aProcessId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::ReserveProcessL: %S : %S" ),
             &aAppId,
             &aProcessId );

    CPEngAppAccessContainer* accessCounter = AccessCounter( aAppId );
    if ( accessCounter )
        {
        // it will leave if not find
        accessCounter->ActivateProcessIdL( aProcessId );
        return;
        }

    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::SessionId()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngCSPSessManager::SessionId() const
    {
    return *iSessionSlotId;
    }

////////////////////////////////////////////////////////////////////////////////
// =========== New Function for Request handling ===============================
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::HandleAsynchronousRequestL()
// -----------------------------------------------------------------------------
//
void  CPEngCSPSessManager::HandleAsynchronousRequestL(
    const RPEngMessage& aMessage,
    MPEngSubSession& aServSubSession,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::HandleAsynchronousRequestL:%d" ),
             aMessage.Function() );

    TInt function( aMessage.Function() );
    // first check that this kind of request is allowed
    CheckRequestValidityL( function, aServSessionId, aServSubSessionId );
    // request can be proceeded
    switch ( function )
        {
        case ESubSessUpdateData:
            {
            HandleUpdateDataRequestL( aMessage,
                                      aServSubSession,
                                      aServSessionId,
                                      aServSubSessionId );
            break;
            }

        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::HandleSynchronousRequestL()
// -----------------------------------------------------------------------------
//
void  CPEngCSPSessManager::HandleSynchronousRequestL(
    const RPEngMessage& aMessage,
    TInt32 /* aServSessionId */,
    TInt /* aServSubSessionId */ )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::HandleSynchronousRequestL:%d" ),
             aMessage.Function() );

    // first check that this kind of request is allowed
    TInt function( aMessage.Function() );
    switch ( function )
        {
            // Get Session service tree
        case ESubSessGetServiceTree:
            {
            LeaveIfNotConnectedL();
            TPckgBuf<TPEngWVCspServicesTree2> serviceTree(
                iAccessHandler->ServiceCapabilities() );
            aMessage.WriteOneDescriptorL( KMessageSlot0, serviceTree );
            break;
            }

        // get Session Log in data
        case ESubSessGetLogInData:
            {
            LeaveIfNotConnectedL();
            TInt err( aMessage.WriteOneDescriptor( KMessageSlot0,
                                                   iAccessHandler->LogInData() ) );
            if ( err == KErrOverflow )
                {
                User::Leave( iAccessHandler->LogInData().Length() );
                }
            User::LeaveIfError( err );
            break;
            }

        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::HandleLogInRequestL
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::HandleLogInRequestL(
    const RPEngMessage& aMessage,
    MPEngSubSession& aSubSession,
    TInt32 aServSessionId,
    TInt   aServSubSessionId  )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::HandleLogInRequestL:" ) );

    // is CSP session active?
    if ( iState == EPEngNWPresenceSessionOpen )
        {
        // verify password and leave if password is not correct
        HBufC8*	loginData = aMessage.ReadOneDescriptor8LC( KMessageSlot0 );
        iAccessHandler->AssertLoginDataL( *loginData );
        CleanupStack::PopAndDestroy( loginData ); // loginData

        // session is active, inform sub-session to Open Ref to the App Id
        aSubSession.HandleLogInL();
        // done
        aMessage.Complete( KErrNone );
        return;
        }
    // Open network connection, not logged
    DoLogInL( aMessage, aSubSession, aServSessionId, aServSubSessionId );
    // done here
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::HandleForceLogOutRequestL
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::HandleForceLogOutRequestL(
    const RPEngMessage& aMessage,
    MPEngSubSession& aSubSession,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::HandleForceLogOutRequestL:" ) );

    // is CSP session active or does App id exists
    CPEngAppAccessContainer* accessCounter = AccessCounter(
                                                 aSubSession.AppId() );

    if ( ( iState == EPEngNWPresenceSessionClosed )
         ||
         ( !accessCounter )
       )
        {
        aSubSession.HandleLogOut();
        // we are already closed
        aMessage.Complete( KErrNotReady );
        return;
        }

    // check how many active app ids we have
    if ( iAccessCounters->Count() == KLastRefAccessCount )
        {
        // we need to do proper log out of whole CSP
        DoLogOutL( aMessage,
                   aSubSession,
                   aServSessionId,
                   aServSubSessionId );
        return;
        }

    // just remove appId from active and inform sub-sessions
    NotifyNewEventL( EPEngEventAppNWPresenceSessionClosed,
                     aSubSession.AppId(),
                     EPEngNWPresenceSessionClosed );
    NotifyDisconnectedAppId( aSubSession.AppId() );
    accessCounter->CloseContainer();
    aSubSession.HandleLogOut();
    aMessage.Complete( KErrNone );
    CheckReferenceAccessD();
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::HandleDetachAppIdL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::HandleDetachAppIdL(
    const RPEngMessage& aMessage,
    MPEngSubSession& aSubSession,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::HandleDetachAppIdL:" ) );

    CPEngAppAccessContainer* accessCounter = AccessCounter(
                                                 aSubSession.AppId() );

    if ( ( iState == EPEngNWPresenceSessionClosed )
         ||
         ( !accessCounter )
       )
        {
        aSubSession.HandleLogOut();
        // we are already closed
        aMessage.Complete( KErrNone );
        return;
        }

    // is this last connection holder
    if (
        ( KLastRefAccessCount == accessCounter->ActiveAccessCount() )
        &&
        ( KLastRefAccessCount == iAccessCounters->Count() )
    )
        {
        // Issue log out of the CSP
        DoLogOutL( aMessage,
                   aSubSession,
                   aServSessionId,
                   aServSubSessionId );
        return;
        }
    // remove session Id
    TBool notify( accessCounter->CloseAccess() );
    // Notify about close of the App Id
    if ( notify )
        {
        NotifyNewEventL( EPEngEventAppNWPresenceSessionClosed,
                         aSubSession.AppId(),
                         EPEngNWPresenceSessionClosed );
        NotifyDisconnectedAppId( aSubSession.AppId() );
        aSubSession.HandleLogOut();
        aMessage.Complete( KErrNone );
        CheckReferenceAccessD();
        }
    }


// -----------------------------------------------------------------------------
// CPEngCSPSessManager::CancelAsynchronousRequest()
// -----------------------------------------------------------------------------
//
void  CPEngCSPSessManager::CancelAsynchronousRequest(
    const RPEngMessage& aMessage,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::CancelAsynchronousRequest:%d" ),
             aMessage.Int0() );

    TInt function( aMessage.Int0() );
    // there can be only one async request of each kind per each Sub session,
    TInt fescue( FindRequestInSubSession(	function,
                                          aServSessionId,
                                          aServSubSessionId ) );
    if ( fescue != KErrNotFound )
        {
        iRequestHandlers[ fescue ]->CancelRequestD();
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::CancelAllSessionRequests()
// -----------------------------------------------------------------------------
//
void  CPEngCSPSessManager::CancelAllSessionRequests(
    TInt32 aServSessionId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::CancelAllSessionRequests:" ) );

    for ( TInt fescue( iRequestHandlers.Count() - 1 );
          fescue >= 0;
          --fescue )
        {
        // check handlers only in case, handlerCount is smaller them
        // real number of requests this is because,canceling
        // of particular requests can complete other requests
        if ( fescue < iRequestHandlers.Count() )
            {
            if ( iRequestHandlers[ fescue ]->SessionId() == aServSessionId )
                {
                iRequestHandlers[ fescue ]->CancelRequestD();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::CancelAllSubSessionRquests()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::CancelAllSubSessionRquests(
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::CancelAllSubSessionRquests:" ) );

    for ( TInt fescue( iRequestHandlers.Count() - 1 );
          fescue >= 0;
          --fescue )
        {
        // check handlers only in case, handlerCount is smaller them
        // real number of requests this is because,canceling
        // of particular requests can complete other requests
        if (
            ( fescue < iRequestHandlers.Count() )
            &&
            ( iRequestHandlers[ fescue ]->SessionId() == aServSessionId )
            &&
            ( iRequestHandlers[ fescue]->SubSessionId() == aServSubSessionId )
        )
            {
            iRequestHandlers[ fescue ]->CancelRequestD();
            }
        }
    }

/////////////////////////////////////////////////////////////////////////////////
//===================== Functions for resource handling =========================
/////////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CPEngCSPSessManager::LoadResourcesL()
// Load Pure data handler
// Load transaction factories
// start listening of the incoming request from network
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::LoadResourcesL()
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::LoadResourcesL:" ) );

    // load pure data handler
    if ( !iPureDataHandler )
        {
        iPureDataHandler = PEngTransferAdapter::PureDataHandlerL(
                               *iSessionSlotId );
        }

    if ( iTransFactories.Count() == 0 )
        {
        // load and initialize factories
        // create List library transaction factory

        MPEngTransactionFactory* transFactory =
            PEngListLibraryFactory::AttributeListsTransactionManagerLC(
                *iSessionSlotId );
        iTransFactories.AppendL( transFactory );
        CleanupStack::Pop(); // transFactory

        // create attribute List Transaction factory
        transFactory =
            PEngListLibraryFactory::ContactListTransactionManagerLC(
                *iSessionSlotId );
        iTransFactories.AppendL( transFactory );
        CleanupStack::Pop(); // transFactory

        // create attribute library transaction factory
        transFactory = PEngAttrLibFactory::AttributeTransFactoryInstanceLC(
                           *iSessionSlotId );
        iTransFactories.AppendL( transFactory );
        CleanupStack::Pop(); // transFactory

        // create autorization transaction factory
        transFactory = PEngListLibraryFactory::AuthorizationTransactionManagerLC(
                           *iSessionSlotId );
        iTransFactories.AppendL( transFactory );
        CleanupStack::Pop(); // transFactory

        }

    if ( iIncomingHandlers.Count() == 0 )
        {
        CPEngIncomingDataHandler* handler =
            CPEngIncomingDataHandler::NewLC( *iPureDataHandler,
                                             iTransFactories,
                                             *this );
        handler->StartListeningL();
        iIncomingHandlers.AppendL( handler );
        CleanupStack::Pop( handler ); // handler
        }
    if ( !iPluginsLoaded )
        {
        iServer.SessionOpened();
        iPluginsLoaded = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::UnloadResources()
// Stop Incoming data listening and processing
// close transaction factories
// unload pure data handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::UnloadResources()
    {
    PENG_DP( D_PENG_LIT( "CPEngCSPSessManager::UnloadResources:" ) );

    // Stop listening
    iIncomingHandlers.ResetAndDestroy();
    if ( !iRequestHandlers.Count() || iState == EPEngNWPresenceSessionClosed )
        {
        UnloadTransactionFactories();
        if ( iPureDataHandler )
            {
            iPureDataHandler->Close();
            iPureDataHandler = NULL;
            }
        if ( iAccessHandler )
            {
            iAccessHandler->UnregisterSessionObserver( *this );
            iAccessHandler->Close();
            iAccessHandler = NULL;
            }
        }
    if ( iPluginsLoaded )
        {
        iServer.SessionClosed();
        iPluginsLoaded = EFalse;
        }
    }

/////////////////////////////////////////////////////////////////////////////////
//================= Private Functions of request management =====================
/////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::CheckRequestValidityL()
// Check if request is allowed to be handled
// If request is not allowed, function leaves, so request message is
// completed by the framework
// If not needed, server session and sub-session Ids are not obeyed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::CheckRequestValidityL(
    TInt aRequestFunction,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    // according to the request function, check if there is already
    // such a request pending and if yes, then if another is allowed
    switch ( aRequestFunction )
        {
        case ESubSessLogIn:
        case ESubSessForceLogOut:
            {
            // only one request of this kind per session manager
            if ( KErrNotFound != FindRequest( aRequestFunction ) )
                {
                User::Leave( KErrAlreadyExists );
                }
            break;
            }

        case ESubSessUpdateData:
            {
            // only one request of this kind per session
            if ( KErrNotFound != FindRequestInSubSession(
                     aRequestFunction,
                     aServSessionId,
                     aServSubSessionId ) )
                {
                User::Leave( KErrAlreadyExists );
                }
            break;
            }

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::FindRequest()
// Look for the particular request within all sessions
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngCSPSessManager::FindRequest(
    TInt aRequestFunction )
    {
    TInt handlerCount( iRequestHandlers.Count() );
    for ( TInt x( 0 ) ; x < handlerCount ; x++ )
        {
        if ( iRequestHandlers[x]->RequestFunction() == aRequestFunction )
            {
            return x;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::FindRequestInSession()
// Look for the particular request within the session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngCSPSessManager::FindRequestInSession(
    TInt aRequestFunction,
    TInt32 aServSessionId )
    {
    TInt handlerCount( iRequestHandlers.Count() );
    for ( TInt x( 0 ) ; x < handlerCount ; x++ )
        {
        if (
            ( iRequestHandlers[ x ]->SessionId() == aServSessionId )
            &&
            ( iRequestHandlers[ x ]->RequestFunction() == aRequestFunction )
        )
            {
            return x;
            }
        }
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngCSPSessManager::FindRequestInSubSession()
// Look for the particular request within the session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngCSPSessManager::FindRequestInSubSession(
    TInt aRequestFunction,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    TInt handlerCount( iRequestHandlers.Count() );
    for ( TInt x( 0 ) ; x < handlerCount ; x++ )
        {
        if (
            ( iRequestHandlers[ x ]->SessionId() == aServSessionId )
            &&
            ( iRequestHandlers[ x ]->SubSessionId() == aServSubSessionId )
            &&
            ( iRequestHandlers[ x ]->RequestFunction() == aRequestFunction )
        )
            {
            return x;
            }
        }
    return KErrNotFound;
    }



// -----------------------------------------------------------------------------
// CPEngCSPSessManager::HandleUpdateDataRequestL
// Handle Send Attributes Request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::HandleUpdateDataRequestL(
    const RPEngMessage& aMessage,
    MPEngSubSession& aServSubSession,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    // get factory and operation enumerations
    TInt opId( aMessage.Int0() );

    LeaveIfNotConnectedL();

    // read data to be updated
    HBufC* data = aMessage.ReadOneDescriptor16LC( KMessageSlot1 );
    RObjectArray<MPEngOutgoingTransactionHandler> transactions;
    CleanupClosePushL( transactions	);
    TInt count( iTransFactories.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        iTransFactories[ x ]->OutgoingTransactionHandlerL( opId,
                                                           *data,
                                                           transactions );
        }
    if ( transactions.Count() == 0 )
        {
        // nothing to do, complete Message
        aMessage.Complete( KErrNone );
        CleanupStack::PopAndDestroy( 2 ); // transactions, data,
        return;
        }

    CPEngHandlerSendData* newHandler = CPEngHandlerSendData::NewLC(
                                           *this,
                                           *iPureDataHandler,
                                           aServSubSession,
                                           aMessage,
                                           transactions,
                                           aServSessionId,
                                           aServSubSessionId );

    iRequestHandlers.AppendL( newHandler );
    // keep handler in the clean up for Starting,
    // so it is deleted when start fails
    newHandler->StartHandlerL();
    newHandler->SetMessage( aMessage );
    CleanupStack::Pop( newHandler ); // newHandler
    CleanupStack::PopAndDestroy( 2 ); // transactions, data,
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::DoLogInL
// Do Log In to the CSP session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::DoLogInL(
    const RPEngMessage& aMessage,
    MPEngSubSession& aServSubSession,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    // Load access handler and start listening
    if ( !iAccessHandler )
        {
        iAccessHandler = PEngTransferAdapter::AccessHandlerL(
                             *iSessionSlotId );
        iAccessHandler->RegisterSessionObserverL( *this );
        }
    // create new handler
    CPEngHandlerLogIn* newHandler = CPEngHandlerLogIn::NewLC(
                                        *this,
                                        *iAccessHandler,
                                        aServSubSession,
                                        aMessage,
                                        aServSessionId,
                                        aServSubSessionId );

    // store new handler
    iRequestHandlers.AppendL( newHandler );
    // keep handler in the clean up for Starting,
    // so it is deleted when start fails
    newHandler->StartHandlerL( aMessage );
    newHandler->SetMessage( aMessage );
    CleanupStack::Pop( newHandler ); // newHandler
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::DoLogOutL
// Do log out from the CSP session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::DoLogOutL(
    const RPEngMessage& aMessage,
    MPEngSubSession& aServSubSession,
    TInt32 aServSessionId,
    TInt   aServSubSessionId )
    {
    // create new handler
    CPEngHandlerLogOff* newHandler = CPEngHandlerLogOff::NewLC(
                                         *this,
                                         *iAccessHandler,
                                         aServSubSession,
                                         aMessage,
                                         aServSessionId,
                                         aServSubSessionId );

    // store new handler
    iRequestHandlers.AppendL( newHandler );
    // keep handler in the clean up for Starting,
    // so it is deleted when start fails
    newHandler->StartHandlerL();
    newHandler->SetMessage( aMessage );
    CleanupStack::Pop( newHandler ); // newHandler
    // we are done
    }


////////////////////////////////////////////////////////////////////////////////
// ================ New private Helping functions ==============================
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::AccessCounter
// Find Access counter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngAppAccessContainer* CPEngCSPSessManager::AccessCounter(
    const TDesC& aAppId )
    {
    // look for the access counter manually
    TInt count( iAccessCounters->Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        CPEngAppAccessContainer* accessContainer =
            static_cast <CPEngAppAccessContainer*>
            ( ( *iAccessCounters )[ x ] );

        if ( KErrNone == accessContainer->AppId().Compare( aAppId ) )
            {
            return accessContainer;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::CheckReferenceAccessD
// Check TInt ref access and reference access by the application Ids
// if all accesses are zero, it deletes itself
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::CheckReferenceAccessD()
    {
    if ( !iAccessCount && !iAccessCounters->Count() )
        {
        delete this;
        return;
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::UnloadTransactionFactories
// Unload Transaction factories
// Transaction factories are ref-counted singletons, so they should
// be closed rather than destroyed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::UnloadTransactionFactories()
    {
    for ( TInt x( iTransFactories.Count() - 1 ) ; x >= 0  ; --x )
        {
        iTransFactories[ x ]->Close();
        iTransFactories.Remove( x );
        }
    iTransFactories.Reset();
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::ResetAccessContainers
// Reset access Containers
// Go through all access containers and close them
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::ResetAccessContainers()
    {
    for ( TInt x( iAccessCounters->Count() - 1 ) ; x >= 0  ; --x )
        {
        static_cast<CPEngAppAccessContainer*>(
            ( *iAccessCounters )[ x ] )->CloseContainer();
        }

    // notify all connected sub-session to disattach their app ids
    NotifyCPSClose();
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::NotifyDisconnectedAppId
// Inform sub-session about app Id disconnection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::NotifyDisconnectedAppId(
    const TDesC& aAppId )
    {
    for ( TInt x( iServSubSessions.Count() - 1 ) ; x >= 0 ; --x )
        {
        iServSubSessions[ x ]->DisconnectAppId( aAppId );
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::NotifyCPSClose
// Inform sub-sessions about CPS close state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::NotifyCPSClose()
    {
    for ( TInt x( iServSubSessions.Count() - 1 ) ; x >= 0 ; --x )
        {
        iServSubSessions[ x ]->CSPSessionClosed();
        }
    }

// -----------------------------------------------------------------------------
// CPEngCSPSessManager::NotifyNewEventL
// Inform sub-sessions about CPS close state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::NotifyNewEventL(
    TPEngNWSessionSlotEvent aEvent,
    const TDesC& aAppId,
    TPEngNWSessionSlotState aAppIdState )
    {
    CPEngSessionSlotEvent* newEvent = CPEngSessionSlotEvent::NewLC();
    newEvent->SetSessionSlotId( iSessionSlotId, EFalse );
    newEvent->SetGlobSessSltState( iState );
    newEvent->SetSessSltEvent( aEvent );
    if ( aAppId != KNullDesC )
        {
        newEvent->SetAppIdL( aAppId );
        newEvent->SetAppSessSltState( aAppIdState );
        }
    HBufC8* eventBuff = newEvent->PackEventLC();
    // ignore error from notify
    iStoreAdmin.NotifyEvent( *eventBuff );
    CleanupStack::PopAndDestroy( 2 ); // eventBuff, newEvent
    }



// -----------------------------------------------------------------------------
// CPEngCSPSessManager::LeaveIfNotConnectedL()
// -----------------------------------------------------------------------------
//
void CPEngCSPSessManager::LeaveIfNotConnectedL() const
    {
    if ( iState != EPEngNWPresenceSessionOpen )
        {
        User::Leave( KErrNotReady );
        }
    }


//  End of File

