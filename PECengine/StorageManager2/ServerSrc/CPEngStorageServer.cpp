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
* Description:  Implementation of class CPEngStorageServer
*
*/


//  Include Files
#include <e32svr.h>
#include <badesca.h>
#include <s32mem.h>


#include "CPEngStorageServer.h"
#include "CPEngStorageSession.h"
#include "CPEngStorageFolder.h"
#include "PEngStorageServerCommon.h"
#include "TPEngServerParams.h"
#include "CPEngHandlerListenEvents.h"
#include "PEngInternalGlobalConsts.h"
#include "CPEngTimer.h"
#include "CPEngActiveScheduler.h"

// Session Slot utility
#include "CPEngSessionSlotId.h"
#include "CPEngSessionSlotState.h"
#include "CPEngSessionSlotEvent.h"
#include "RObjectArray.h"

// Hash Tool
#include "PEngHashTool.h"

// Debug Prints
#include "PresenceDebugPrint.h"


// CONSTANTS

// Init size of the buffer for client server communications
static const TInt KBufferInitSize = 100;

// Static data for Capability check configuration
static const TInt KPEngStoreServRangeCount = 2;

/**
 * Ranges for the Request values
 * All requests will fall in one range
 */
static const TInt PEngStoreServRanges[ KPEngStoreServRangeCount ] =
    {
    // Range from EMainSessShutdownServer to EFolderSubSessNotifyChangedSIDs
    0,

    // range is from EHighestStoreServerRequest request to KMaxInt
    EHighestStoreServerRequest
    };

/**
 * Element indexes for the defined ranges
 * we have only one range and for it is defined only one Element
 */
static const TUint8 KPEngStoreServElementsIndex[ KPEngStoreServRangeCount ] =
    {
    // First element in the element array will be applied for this range
    0,
    // Not supported will be applied for this range
    CPolicyServer::ENotSupported
    };

// Policy elements
static const CPolicyServer::TPolicyElement KPEngStoreServElements[] =
    {
        {
        _INIT_SECURITY_POLICY_C2( ECapabilityReadUserData,
        ECapabilityWriteUserData ),
        CPolicyServer::EFailClient
        }
    };

static const CPolicyServer::TPolicy KPEngServerPolicy =
    {
    // The index into Elements,that is used to check a connection attempt

    0,

    // Number of ranges in the iRanges array
    KPEngStoreServRangeCount,

    // A pointer to an array of ordered ranges of request numbers
    PEngStoreServRanges,

    // A pointer to an array of TUint8 values specifying
    // the appropriate action to take for each range in iRanges
    KPEngStoreServElementsIndex,

    // A pointer to an array of distinct policy elements
    KPEngStoreServElements
    };

// MACROS
#define RETURN_IF_ERROR( aError )\
    if( aError != KErrNone )\
        {\
        return aError;\
        }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStorageServer::CPEngStorageServer
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngStorageServer::CPEngStorageServer( TInt aPriority )
        : CPolicyServer( aPriority, KPEngServerPolicy ),
        iRunning( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CPEngStorageServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::ConstructL()
    {
    StartL( KStorageServerName );
    iBuff16.CreateL( KBufferInitSize );
    iBuff8.CreateL( KBufferInitSize );
    // create container collection
    iContainerIx = CObjectConIx::NewL();
    iSubSessionCnt = iContainerIx->CreateL();
    iFoldersCnt = iContainerIx->CreateL();
    iObserversCnt = iContainerIx->CreateL();

    // set up file system enviroment
    User::LeaveIfError( iFs.Connect() );// connect to file server
    // set session to the private folder
    TInt err( iFs.CreatePrivatePath( EDriveC ) );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    User::LeaveIfError( iFs.SetSessionToPrivate( EDriveC ) );
    TFileName name;
    iFs.SessionPath( name );
    name.Append( KPEngStorageTempFolder );
    iTempFolder = name.AllocL();

    // make sure we have Storage folder, flush it first and then created
    CFileMan* fileMan = CFileMan::NewL( iFs );
    CleanupStack::PushL( fileMan );
    fileMan->RmDir( *iTempFolder );
    // ignore errors
    iFs.Delete( *iTempFolder );
    CleanupStack::PopAndDestroy(); // fileMan
    // try to create temp folder
    TInt er ( iFs.MkDirAll( *iTempFolder ) );
    if ( er != KErrAlreadyExists )
        {
        User::LeaveIfError( er );
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageServer* CPEngStorageServer::NewL( TInt aPriority )
    {
    CPEngStorageServer* self = CPEngStorageServer::NewLC( aPriority );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngStorageServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageServer* CPEngStorageServer::NewLC(  TInt aPriority )
    {
    CPEngStorageServer* self = new( ELeave ) CPEngStorageServer( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
CPEngStorageServer::~CPEngStorageServer()
    {
    // clean all containers from the objects in it
    // close all elements of the array
    if ( iFoldersCnt )
        {
        for ( TInt x( iFoldersCnt->Count() - 1 ) ; x >= 0  ; --x )
            {
            ( *iFoldersCnt )[ x ]->Close();
            }
        }


    // close all elements of the array
    if ( iObserversCnt )
        {
        for ( TInt y( iObserversCnt->Count() - 1 ) ; y >= 0  ; --y )
            {
            ( *iObserversCnt )[ y ]->Close();
            }
        }

    // all containers are deleted with the collection
    delete iContainerIx;
    // clean file enviroment
    iFs.Close();
    delete iTempFolder;
    // shut down plugin server
    delete iTimeOut;
    iBuff16.Close();
    iBuff8.Close();
    }


// =============================================================================
// =============== Functions from base class ===================================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngStorageServer::ExecuteServerL()
// -----------------------------------------------------------------------------
//
TInt CPEngStorageServer::ExecuteServerL( TPEngServerParams& aParams )
    {
    //Renaming must be done as early as possible
    aParams.RenameMainThread( KStorageServerName );

    TInt res( KErrNone );

    // start scheduler and server
    CPEngActiveScheduler* pA = new( ELeave )CPEngActiveScheduler;
    CleanupStack::PushL( pA );
    CPEngActiveScheduler::Install( pA );

    //If exe server, call RunServerL directly.
    TRAP( res, RunServerL( aParams ) );

    CPEngActiveScheduler::Install( NULL );
    CleanupStack::PopAndDestroy();//pA
    return res;
    }



// -----------------------------------------------------------------------------
// CPEngStorageServer::StopServer()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::StopServer()
    {
    if ( iRunning )
        {
        CPEngActiveScheduler::Stop();
        }
    iRunning = EFalse;
    }



// -----------------------------------------------------------------------------
// CPEngStorageServer::RunServerL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::RunServerL( TPEngServerParams& aParams )
    {
    PENG_DP( D_PENG_LIT( "CPEngStorageServer::RunServerL()" ) );

    //One instance of server must be allocated here.
    CPEngStorageServer* server = CPEngStorageServer::NewLC( KServerPriority );

    //must die if can't signal client
    aParams.Signal();
    // start fielding requests from clients
    //Thread is ended when CPEngActiveScheduler::Stop is called.
    server->StartServer();


    CleanupStack::PopAndDestroy();//server
    // finished
    }


// =============================================================================
// =============== Functions from the MPEngStorageServer class =================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageServer::AddSubSessionL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::AddSubSessionL(
    CObject& aSubSession )
    {
    iSubSessionCnt->AddL( &aSubSession );
    }

// -----------------------------------------------------------------------------
// CPEngStorageServer::RemoveSubSessionL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::RemoveSubSessionL(
    CObject& aSubSession )
    {
    iSubSessionCnt->Remove( & aSubSession );
    }

// -----------------------------------------------------------------------------
// CPEngStorageServer::RegisterGlobEventListenerL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::RegisterGlobEventListenerL(
    const RPEngMessage& /* aMessage */,
    TUint32 aSessionId )
    {
    CPEngHandlerListenEvents* reqHandler = FindRequestHandler( aSessionId ,
                                                               EMainSessListenGlobalEvents );
    if ( reqHandler )
        {
        // this handler already exists, not allowed by multiple
        User::Leave( KErrAlreadyExists );
        }
    // create new handler
    reqHandler = CPEngHandlerListenEvents::NewLC( aSessionId );
    iObserversCnt->AddL( reqHandler );
    CleanupStack::Pop(); // newHandler
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::CancelRequestL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::CancelRequestL(
    const RPEngMessage& aMessage,
    TUint32 aSessionId )
    {
    CPEngRequestHandler* reqHandler = FindRequestHandler( aSessionId,
                                                          static_cast<TPEngStorageServerMessages> ( aMessage.Int0() ) );
    // ignore if such a request does not exists
    if ( reqHandler )
        {
        reqHandler->CancelRequestD();
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::CancelRequestL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::CancelAllSessionRequests(
    TUint32 aSessionId )
    {
    for ( TInt x( iObserversCnt->Count() - 1 ) ;  x >= 0  ; --x )
        {
        CPEngRequestHandler* handler = static_cast<CPEngRequestHandler*>( ( *iObserversCnt )[ x ] );
        if ( handler->SessionId() == aSessionId )
            {
            handler->CancelRequestD();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEngStorageServer::ReloadAsynchronousScoutL()
// Reload Asynchronous scout of the async observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::ReloadAsynchronousScoutL(
    const RPEngMessage& aMessage,
    TUint32 aSessionId )
    {
    CPEngRequestHandler* reqHandler = FindRequestHandler( aSessionId,
                                                          static_cast<TPEngStorageServerMessages> ( aMessage.Int0() ) );
    if ( !reqHandler )
        {
        // there is not such a request to be reloaded
        User::Leave( KErrArgument );
        }
    reqHandler->ReloadScoutWithNewMessageL( aMessage );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::CreateSessionFolderL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::CreateSessionFolderL(
    const RPEngMessage& aMessage )
    {
    CPEngSessionSlotId* sessId = TranslateSessionIdLC( aMessage );
    CPEngSessionSlotState* state = NULL;
    CPEngStorageFolder* folder = NULL;
    HBufC* stateFile = NULL;
    // load state
    TInt cleanUp ( LoadSessionStateLCX( *sessId, ETrue,
                                        state, folder, stateFile ) );

    // update state with the new application ID
    aMessage.ReadOneDescriptorL( KMessageSlot1, iBuff16 );

    // also create session slot global event for notification
    CPEngSessionSlotEvent* globEvent =  PackGlobEventLC( *state,
                                                         EPEngEventNWSessionSlotCreated,
                                                         iBuff16 );

    TPtr stateFileName( stateFile->Des() );
    UpdateSessionSltStateL( folder, *state, *globEvent, stateFileName );

    // notify about event
    globEvent->PackEventL( iBuff8 );
    NotifyGlobalEvent( iBuff8 );

    // clean globEvent, (state), stateFile, sessId
    CleanupStack::PopAndDestroy( 2 + cleanUp );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::RemoveSessionFolderL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::RemoveSessionFolderL(
    const RPEngMessage& aMessage )
    {
    CPEngSessionSlotId* sessId = TranslateSessionIdLC( aMessage );
    CPEngSessionSlotState* state = NULL;
    CPEngStorageFolder* folder = NULL;
    HBufC* stateFile = NULL;
    // load state
    TInt cleanUp ( LoadSessionStateLCX( *sessId,
                                        EFalse,
                                        state,
                                        folder,
                                        stateFile ) );

    if ( folder )
        {
        // folder is active, refuse any deletion
        User::Leave( KErrInUse );
        }
    // remove registered ID from the session
    aMessage.ReadOneDescriptorL( KMessageSlot1, iBuff16 );

    // also create session slot global event for notification
    CPEngSessionSlotEvent* globEvent =  PackGlobEventLC(
                                            *state,
                                            EPEngEventNWSessionSlotRemoved,
                                            iBuff16 );

    TPtr stateFileName( stateFile->Des() );
    UpdateSessionSltStateL( folder, *state, *globEvent, stateFileName );

    // notify about event
    globEvent->PackEventL( iBuff8 );
    NotifyGlobalEvent( iBuff8 );

    // clean globEvent, (state), stateFile, sessId
    CleanupStack::PopAndDestroy( 2 + cleanUp );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::SessionStateL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::SessionStateL(
    const RPEngMessage& aMessage )
    {
    CPEngSessionSlotId* sessId = TranslateSessionIdLC( aMessage );
    CPEngStorageFolder* folder = FindStorageFolder( *sessId );
    CPEngSessionSlotState* state = NULL;
    if ( folder )
        {
        state = &( folder->SessionSlotState() );
        }
    else
        {
        HBufC* stateFile = NULL;
        state = ReadSessionStatePermanentLC( *sessId, stateFile );
        }

    state->PackDataL( EPureState, iBuff8 );

    TInt err( aMessage.WriteOneDescriptor( KMessageSlot1, iBuff8 ) );
    if ( err == KErrOverflow )
        {
        User::Leave( iBuff8.Length() );
        }
    User::LeaveIfError( err );

    if ( !folder )
        {
        CleanupStack::PopAndDestroy( 2 ); // state, stateFile
        }
    CleanupStack::PopAndDestroy(); // sessId
    }



// -----------------------------------------------------------------------------
// CPEngStorageServer::AllSessionStatesL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::AllSessionStatesL(
    const RPEngMessage& aMessage )
    {
    // pack all states to one big buffer
    // this takes all active folders and adds sessions not active
    CDir* dirList = NULL;
    TFileName dir;
    User::LeaveIfError( iFs.PrivatePath( dir ) );
    // insert drive leter on the beginning
    dir.Insert( 0, KPEngStorageDrive );

    User::LeaveIfError( iFs.GetDir( dir,
                                    KEntryAttNormal,
                                    ESortByName,
                                    dirList ) );
    CleanupStack::PushL( dirList );
    TInt count( dirList->Count() );
    RObjectArray<CPEngSessionSlotState> states;
    CleanupClosePushL( states );
    // 4 for the count of the states
    TInt size( 4 );
    for ( TInt x( 0 ) ; x < count  ; x++ )
        {
        const TEntry& entry = ( *dirList )[ x ];
        if ( ! entry.IsDir() )
            {
            CPEngSessionSlotState* state = CPEngSessionSlotState::NewLC();
            ReadFileL( entry.iName, iBuff8 );
            state->UnpackDataL( iBuff8, EPermanentData );
            states.AppendL( state );
            size += state->Size( EPermanentData );
            CleanupStack::Pop(); // state
            }
        }
    // now go through all session and if they are loaded as folders
    // and pack them to the buffer
    // get buffer for transfer
    CBufFlat* buf = CBufFlat::NewL( size );
    CleanupStack::PushL( buf );
    RBufWriteStream ws;
    CleanupClosePushL( ws );
    ws.Open( *buf );                            // CSI: 65 #

    // write count of all sessions
    count = states.Count();
    ws.WriteInt32L( count );

    for ( TInt i( 0 ) ; i < count ; i++ )
        {
        CPEngStorageFolder* folder = FindStorageFolder(
                                         states[ i ]->SessionSlotId() );
        if ( folder )
            {
            // there is loaded folder, take state from there
            folder->SessionSlotState().ExternalizeL( ws, EWholeState );
            }
        else
            {
            states[ i ]->ExternalizeL( ws, EWholeState );
            }
        }
    // write data to the client side
    TInt err( aMessage.WriteOneDescriptor( KMessageSlot0, buf->Ptr( 0 ) ) );
    if ( err == KErrOverflow )
        {
        User::Leave( buf->Ptr( 0 ).Length() );
        }
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( 4 ); // ws, buf, states, dirList,
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::StorageFolderL()
// -----------------------------------------------------------------------------
//
CPEngStorageFolder* CPEngStorageServer::StorageFolderL(
    const RPEngMessage& aMessage )
    {
    CPEngSessionSlotId* sessId = TranslateSessionIdLC( aMessage );
    CPEngStorageFolder* folder = LoadStorageFolderL( *sessId );
    CleanupStack::PopAndDestroy(); // sessId
    return folder;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::NotifyGlobalEvent()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::NotifyGlobalEvent(
    const TDesC8& aGlobalEvent )
    {
    // better to go through array from end
    for ( TInt x( iObserversCnt->Count() - 1 ) ; x >= 0 ; x-- )
        {
        CPEngHandlerListenEvents* handler =
            static_cast<CPEngHandlerListenEvents*>( ( *iObserversCnt )[ x ] );
        handler->NotifyEvent( aGlobalEvent );
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::NotifyError()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::NotifyError(
    TInt aError )
    {
    // better to go through array from end
    for ( TInt x( iObserversCnt->Count() - 1 ) ; x >= 0 ; x-- )
        {
        CPEngHandlerListenEvents* handler =
            static_cast<CPEngHandlerListenEvents*>( ( *iObserversCnt )[ x ] );
        handler->NotifyError( aError );
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::IsAppIdRegisteredL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::IsAppIdRegisteredL(
    const RPEngMessage& aMessage )
    {
    CPEngSessionSlotId* sessId = TranslateSessionIdLC( aMessage );
    aMessage.ReadOneDescriptorL( KMessageSlot1, iBuff16 );

    CPEngSessionSlotState* state = NULL;
    CPEngStorageFolder* folder = NULL;
    HBufC* stateFile = NULL;
    // load state
    TInt cleanUp ( LoadSessionStateLCX( *sessId, ETrue,
                                        state, folder, stateFile ) );
    // check if appId is registered
    state->ApplicationRegisteredL( iBuff16 );
    // clean (state),stateFile,appId,sessId
    CleanupStack::PopAndDestroy( cleanUp + 2 );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::BackUpActive()
// -----------------------------------------------------------------------------
//
TBool CPEngStorageServer::BackUpActive()
    {
    return iBackUpActive;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::WipeSessionSlotFolderL()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::WipeSessionSlotFolderL(
    const RPEngMessage& aMessage )
    {
    CPEngSessionSlotId* sessId = TranslateSessionIdLC( aMessage );
    CPEngSessionSlotState* state = NULL;
    CPEngStorageFolder* folder = NULL;
    HBufC* stateFile = NULL;
    // load state
    TInt cleanUp ( LoadSessionStateLCX( *sessId,
                                        EFalse,
                                        state,
                                        folder,
                                        stateFile ) );

    if ( folder )
        {
        // folder is active, refuse any deletion
        User::Leave( KErrInUse );
        }

    TPtr stateFileName( stateFile->Des() );
    User::LeaveIfError( CleanStorageFromSessionL( stateFileName ) );

    // clean  globEvent, (state), stateFile, sessId
    CleanupStack::PopAndDestroy( 1 + cleanUp );
    }


// =============================================================================
// ======== New function of the CPEngStorageServer =====================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngStorageServer::SessionCreated()
// Session has been created
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::SessionCreated()
    {
    iSessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::SessionDied()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::SessionDied()
    {
    iSessCount--;
    if ( iSessCount == 0 )
        {
        //  Start timer
        delete iTimeOut;
        iTimeOut = NULL;

        TRAP_IGNORE(
            {
            iTimeOut = CPEngTimer::NewL( *this );
            iTimeOut->After( KServerCloseTimeOut );
            } );
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::TimeExpired()
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::TimeExpired()
    {
    if ( iSessCount == 0 )
        {
        // stop server
        StopServer();
        }
    }


// =============================================================================
// =============== Functions from the MBackupOperationObserver class ===========
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageServer::HandleBackupOperationEventL()
// Called when a backup or restore operation either starts or ends.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// Restore is not implemented before new API won't be delivered
/*
void CPEngStorageServer::HandleBackupOperationEventL(
                        const TBackupOperationAttributes& aBackupOperationAttributes)
    {
    }
*/
// =============================================================================
// =============== Functions from the CServer2 class ===========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageServer::NewSessionL()
// -----------------------------------------------------------------------------
//
CSession2* CPEngStorageServer::NewSessionL(
    const TVersion &aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
    if ( !User::QueryVersionSupported( aVersion, TVersion(
                                           KRequiredVersionMajor,
                                           KRequiredVersionMinor,
                                           KRequiredVersionBuild ) ) )
        {
        User::Leave( KErrNotSupported );
        }

    return CPEngStorageSession::NewL( const_cast<CPEngStorageServer&>( *this ) );
    }


// =============================================================================
// =============== New Private methods from class ==============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageServer::StartServer()
// Start Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::StartServer()
    {
    iRunning = ETrue;
    PENG_DP( D_PENG_LIT( "PECStorageServer::Started" ) );
    CPEngActiveScheduler::Start();
    PENG_DP( D_PENG_LIT( "PECStorageServer::Stopped" ) );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::TranslateSessionIdLC()
// Read Session ID from the client's message
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngSessionSlotId* CPEngStorageServer::TranslateSessionIdLC(
    const RPEngMessage& aMessage )
    {
    // 0 slot of the message has always session identification if required
    CPEngSessionSlotId* sessId = CPEngSessionSlotId::NewLC();

    aMessage.ReadOneDescriptorL( KMessageSlot0, iBuff8 );
    sessId->UnPackL( iBuff8 );
    return sessId;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::FindStorageFolder()
// Try to find session Folder if it is active
// if folder is not active, it returns NULL pointer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngStorageFolder* CPEngStorageServer::FindStorageFolder(
    const CPEngSessionSlotId& aSessionId )
    {
    // look for the session
    TInt count( iFoldersCnt->Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        CPEngStorageFolder* folder =
            static_cast<CPEngStorageFolder*> ( ( *iFoldersCnt )[ x ] );
        if ( KErrNone == folder->SessionSlotState().SessionSlotId().Match( aSessionId ) )
            {
            return  folder;
            }
        }
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::LoadStorageFolderL()
// Load storage folder,
// there is open handle to the returner pointer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngStorageFolder* CPEngStorageServer::LoadStorageFolderL(
    const CPEngSessionSlotId& aSessionId )
    {
    CPEngStorageFolder* folder = FindStorageFolder( aSessionId );
    if ( folder )
        {
        TInt val = folder->Open();             // CSI: 65 #
        return folder;
        }

    // read session state from permanent store
    HBufC* stateFile = NULL;
    CPEngSessionSlotState* state = ReadSessionStatePermanentLC( aSessionId,
                                                                stateFile );

    // it makes directory if does not exists
    TPtr fileName = stateFile->Des();
    UpdateFileToFolderName( fileName );

    // create folder class, pass state
    CPEngStorageFolder* newFolder = CPEngStorageFolder::NewL( iFs,
                                                              *iTempFolder,
                                                              iBuff16 );
    CleanupClosePushL( *newFolder );

    // update Folder path to include, session part
    TFileName sessPath;
    User::LeaveIfError( iFs.SessionPath( sessPath ) );
    stateFile = stateFile->ReAllocL( stateFile->Length() +
                                     sessPath.Length() );
    stateFile->Des().Insert( 0, sessPath );

    CleanupStack::Pop(); //newFolder
    CleanupStack::Pop( 2 ); // state, stateFile

    // following functions are transfering ownership
    newFolder->SetSessionSlotState( *state );
    newFolder->SetSessionFolder( * stateFile );

    // add folder to the folders container
    CleanupClosePushL( * newFolder );
    iFoldersCnt->AddL( newFolder );
    CleanupStack::Pop(); // newFolder
    return newFolder;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::LoadSessionStateLCX()
// Load session slot state
// If there is Storage folder for this session
// then state is taken from it, while nothing is put on clean up stack
// if not, then state is loaded from state file,
// if file does not exist, depends on the aCreate flag, can be
// created or it leave with KErrNotFound
// order items on clan up stack is: aStateFileName,
// or aStateFileName, aSessState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngStorageServer::LoadSessionStateLCX(
    CPEngSessionSlotId& aSessId,
    TBool aCreate,
    CPEngSessionSlotState*& aSessState,
    CPEngStorageFolder*& aStorageFolder,
    HBufC*& aStateFileName )
    {
    aStorageFolder = FindStorageFolder( aSessId );
    // was state folder found or not
    if ( aStorageFolder )
        {
        aSessState = &( aStorageFolder->SessionSlotState() );
        aStateFileName = PEngHashTool::HashStateNameL(              // CSI: 42 #
                             aSessState->SessionSlotId().ServiceAddress(),
                             aSessState->SessionSlotId().UserId() );
        CleanupStack::PushL( aStateFileName );                      // CSI: 42 #
        return 1;
        }
    if ( !aCreate )
        {
        aSessState = ReadSessionStatePermanentLC( aSessId, aStateFileName );    // CSI: 42 #
        return 2;
        }

    // if it won't exist create it, so we need to trap leave
    TRAPD( e,
        {
        aSessState = ReadSessionStatePermanentLC( aSessId,
        aStateFileName );
        // we need to fool TRAP CleanUp level check
        CleanupStack::Pop( 2 );
        } );

    // put those elements back to CleanUpStack
    if ( e == KErrNone )
        {
        CleanupStack::PushL( aStateFileName );                      // CSI: 42 #
        CleanupStack::PushL( aSessState );                          // CSI: 42 #
        }

    // does even the file exist?
    else if ( e == KErrNotFound )
        {
        // file does not exists, create state file and folder
        aStateFileName = PEngHashTool::HashStateNameL( aSessId.ServiceAddress(),    // CSI: 42 #
                                                       aSessId.UserId() );
        CleanupStack::PushL( aStateFileName );                      // CSI: 42 #
        aSessState = CPEngSessionSlotState::NewLC();                // CSI: 42 #
        aSessState->SetSessionSlotId( aSessId, EFalse );
        TPtr fileName = aStateFileName->Des();
        CreateSessionDirectoryL( fileName );                        // CSI: 42 #
        }
    else
        {
        // was there some other error?
        User::LeaveIfError( e );                                    // CSI: 42 #
        }
    return 2;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::ReadSessionStatePermanentLC()
// Read State file from permanent store if it does exists
// Function returns Session Slot state and des buffer with
// session slot state file name ownership of both is transfered
// to the calling client
// both items are left on clean up stack, while state is topest one
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngSessionSlotState* CPEngStorageServer::ReadSessionStatePermanentLC(
    const CPEngSessionSlotId& aSessionId,
    HBufC*& aFileName )
    {
    // Folder is not activated yet, check if folder does exist in the store
    // get folder name of the session
    aFileName = PEngHashTool::HashStateNameL(
                    aSessionId.ServiceAddress(),
                    aSessionId.UserId() );
    CleanupStack::PushL( aFileName );

    // try to get state of the session if it exists
    CPEngSessionSlotState* state = CPEngSessionSlotState::NewLC();
    ReadFileL( *aFileName, iBuff8 );
    state->UnpackDataL( iBuff8, EPermanentData );
    return state;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::UpdateFileToFolderName()
// Modify State file name to folder name
// Functions remove state suffix from file name and replaces is with
// folder delimiter characters
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::UpdateFileToFolderName(
    TDes& aFileName )
    {
    aFileName.SetLength( aFileName.Length() -
                         KPEngStorageStateSuffix().Length() );
    aFileName.Append( KDirDelim );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::CreateSessionDirectoryL()
// Create session directory in the file system
// Passed buffer is used, modified to sued session folder
// and then back to the state as it was before
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::CreateSessionDirectoryL(
    TDes& aFileName )
    {
    // make directory if does not exists
    UpdateFileToFolderName( aFileName );
    TFileName folderName;
    iFs.SessionPath( folderName );
    folderName.Append( aFileName );
    TInt err( iFs.MkDirAll( folderName ) );
    // ignore KErrAlreadyExist
    err = ( err == KErrAlreadyExists ? KErrNone : err );
    User::LeaveIfError( err );
    // modify folder name back to the state file name
    aFileName.SetLength( aFileName.Length() - KDirDelimLength );
    aFileName.Append( KPEngStorageStateSuffix );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::StoreFile()
// Store data to the file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngStorageServer::StoreFile(
    const TDesC& aFileName,
    const TDesC8& aData )
    {
    // open temp file, write date into it and rename it to the correct file
    RFile temp;
    TFileName fName;
    TInt err( temp.Temp( iFs, *iTempFolder, fName, EFileShareExclusive ) );
    RETURN_IF_ERROR( err );
    err = temp.Write( aData );
    if ( ( err != KErrNone ) )
        {
        temp.Close();
        return err;
        }
    err = temp.Flush();
    temp.Close();
    RETURN_IF_ERROR( err );

    err = iFs.Delete( aFileName );
    if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
        {
        return err;
        }
    return iFs.Rename( fName, aFileName );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::ReadFileL()
// Read data from the file
// if file does not exists function will leave
// returned buffer is left on the clean up stack
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::ReadFileL(
    const TDesC& aFileName,
    RBuf8& aBuff )
    {
    RFile f;
    User::LeaveIfError( f.Open( iFs, aFileName, EFileRead ) );
    CleanupClosePushL( f );
    TInt size( 0 );
    User::LeaveIfError( f.Size( size ) );
    if ( size > aBuff.MaxSize() )
        {
        aBuff.ReAllocL( size );
        }
    aBuff.Zero();
    User::LeaveIfError( f.Read( aBuff ) );
    CleanupStack::PopAndDestroy(); // f
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::CleanStorageFromSessionL()
// Clean storage from session
// Function deletes session folder and session state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEngStorageServer::CleanStorageFromSessionL( TDes& aFileName )
    {
    // delete directory
    UpdateFileToFolderName( aFileName );
    TFileName sessionFolder;
    iFs.SessionPath( sessionFolder );
    sessionFolder.Append( aFileName );

    CFileMan* fileMan = CFileMan::NewL( iFs );
    CleanupStack::PushL( fileMan );
    TInt err( fileMan->RmDir( sessionFolder ) );
    CleanupStack::PopAndDestroy(); // fileMan
    RETURN_IF_ERROR( err );

    // modify folder name back to the state file name
    aFileName.SetLength( aFileName.Length() - KDirDelimLength );
    aFileName.Append( KPEngStorageStateSuffix );

    // try to delete file
    return iFs.Delete( aFileName );
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::PackGlobEventLC()
// Create Buffer with global event from the passed
// state and event enumeration, and application Id
// Function deletes session folder and session state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngSessionSlotEvent* CPEngStorageServer::PackGlobEventLC(
    CPEngSessionSlotState& aState,
    TPEngNWSessionSlotEvent aEvent,
    const TDesC& aAppId )
    {
    CPEngSessionSlotEvent* newEvent = CPEngSessionSlotEvent::NewLC();
    newEvent->SetSessionSlotId( &( aState.SessionSlotId() ), EFalse );
    newEvent->SetAppIdL( aAppId );
    newEvent->SetSessSltEvent( aEvent );
    newEvent->SetAppSessSltState( aState.AppState( aAppId ) );
    newEvent->SetGlobSessSltState( aState.SessionSlotState() );
    return newEvent;
    }


// -----------------------------------------------------------------------------
// CPEngStorageServer::UpdateSessionSltStateL()
// Update Session Slot state
// Passed session slot folder pointer can be NULL, then
// session slot state reference is used instead
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageServer::UpdateSessionSltStateL(
    CPEngStorageFolder* aFolder,
    CPEngSessionSlotState& aState,
    CPEngSessionSlotEvent& aEvent,
    TDes& aFileName )
    {
    // do we need to do permanent store update?
    if ( aState.UpdateStateL( aEvent ) )
        {
        // store changes
        aState.PackDataL( EPermanentData, iBuff8 );

        // open file and store there data
        TInt e ( StoreFile( aFileName, iBuff8 ) );
        if ( e != KErrNone )
            {
            // remove the ID from the sate, this is for case folder is active
            aState.RollBackLastUpdate();
            User::Leave( e );
            }
        }
    aState.CommitLastUpdate();
    if ( aFolder )
        {
        aFolder->CommiteStateUpdate();
        }

    // shall we clean storage?
    if ( aState.RegisteredApplications().MdcaCount() == KErrNone )
        {
        CleanStorageFromSessionL( aFileName );
        }
    }



// =============================================================================
// ============= New notification engine functions =============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageServer::FindRequestHandler()
// Find Request Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngHandlerListenEvents* CPEngStorageServer::FindRequestHandler(
    TUint32 aSessionId,
    TPEngStorageServerMessages aFunction )
    {
    TInt count( iObserversCnt->Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        CPEngHandlerListenEvents* reqHandler = static_cast<CPEngHandlerListenEvents*>( ( *iObserversCnt )[ x ] );
        if ( ( reqHandler->SessionId() == aSessionId )
             && ( reqHandler->RequestFunction() == aFunction ) )
            {
            return reqHandler;
            }
        }
    return NULL;
    }

//  End of File
