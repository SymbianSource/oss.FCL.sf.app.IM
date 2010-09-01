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
* Description:  Implementation of class CPEngStorageSession
*
*/


//  Include Files
#include <e32base.h>
#include "CPEngStorageSession.h"
#include "MPEngStorageServer.h"
#include "CPEngStorageSubSession.h"
#include "CPEngAdminSubSession.h"
#include "RPEngMessage.h"
#include "PEngStorageServerCommon.h"
#include "PEngInternalGlobalConsts.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStorageSession::CPEngStorageSession
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngStorageSession::CPEngStorageSession( MPEngStorageServer& aServer )
        : iStorageServer( aServer )
    {
    iStorageServer.SessionCreated();
    }


// -----------------------------------------------------------------------------
// CPEngStorageSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngStorageSession::ConstructL()
    {
    iSubSessions = CObjectIx::NewL();
    }


// Static constructor
CPEngStorageSession* CPEngStorageSession::NewL( MPEngStorageServer& aServer )
    {
    CPEngStorageSession* self = CPEngStorageSession::NewLC( aServer );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngStorageSession::NewLC
//  Symbian 2nd phase constructor can leave.leaves pointer to cleanup-stack
// -----------------------------------------------------------------------------
//
CPEngStorageSession* CPEngStorageSession::NewLC( MPEngStorageServer& aServer )
    {
    CPEngStorageSession* self = new( ELeave ) CPEngStorageSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor (virtual by CBase)
CPEngStorageSession::~CPEngStorageSession()
    {
    delete iSubSessions;
    iStorageServer.CancelAllSessionRequests( reinterpret_cast<TInt32>( this ) );
    iStorageServer.SessionDied();


#if _BullseyeCoverage
    cov_write();
#endif
    }


// =============================================================================
// =============== Functions from CSession2 base class =========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageSession::ServiceL()
// -----------------------------------------------------------------------------
//
void CPEngStorageSession::ServiceL( const RMessage2 &aMessage )
    {
    iCompleteMessage = ETrue;
    DispatchMessageL( RPEngMessage( aMessage ) );
    if ( iCompleteMessage )
        {
        aMessage.Complete( KErrNone );
        }
    }


// -----------------------------------------------------------------------------
// CPEngStorageSession::ServiceError()
// -----------------------------------------------------------------------------
//
void CPEngStorageSession::ServiceError( const RMessage2& aMessage, TInt aError )
    {
    aMessage.Complete( aError );
    }



// =============================================================================
// =============== Private functions of the Session class ======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageSession::DispatchMessageL()
// -----------------------------------------------------------------------------
//
void CPEngStorageSession::DispatchMessageL( const RPEngMessage& aMessage )
    {
    if ( iStorageServer.BackUpActive() )
        {
        User::Leave( KErrServerBusy );
        }
    TInt request( aMessage.Function() );
    switch ( request )
        {
            /*********************************************************************/
            /* Server Management                                                 */
            /*********************************************************************/

            // shut down the server
        case EMainSessShutdownServer:
            {

            break;
            }

        /*********************************************************************/
        /* Sub Session management                                            */
        /*********************************************************************/

        // Create new sub-session
        case EMainSessCreateFolderSubSession:
            {
            CreateNewFolderSubSessionL( aMessage );
            break;
            }

        // 	Create new Administrator sub-session
        case EMainSessCreateAdminSubSession:
            {
            CreateNewAdminSubSessionL( aMessage );
            break;
            }

        // close sub-session
        case EMainSessCloseSubSession:
            {
            RemoveSubSessionL( aMessage.Int3() );
            break;
            }

        /*********************************************************************/
        /* Global Event Notification                                         */
        /*********************************************************************/

        // start listening of global event
        case EMainSessListenGlobalEvents:
            {
            iStorageServer.RegisterGlobEventListenerL( aMessage,
                                                       reinterpret_cast<TInt32>( this ) );
            break;
            }

        /*********************************************************************/
        /* Asynchronous Requests Management                                  */
        /*********************************************************************/

        // Cancel Asynchronous request Request
        case EMainSessCancelRequest:
            {
            iStorageServer.CancelRequestL( aMessage,
                                           reinterpret_cast<TInt32>( this ) );
            break;
            }

        // Reload Asynchronous Scout on the Server side
        case EMainSessReloadAsynchronousScout:
            {
            iStorageServer.ReloadAsynchronousScoutL( aMessage,
                                                     reinterpret_cast<TInt32>( this ) );
            iCompleteMessage = EFalse;
            break;
            }

        /*********************************************************************/
        /* Session Slot Management                                           */
        /*********************************************************************/

        // Create Session Slot Folder
        case EMainSessCreateSessionFolder:
            {
            iStorageServer.CreateSessionFolderL( aMessage );
            break;
            }

        // Remove Session Slot Folder
        case EMainSessRemoveSessionFolder:
            {
            iStorageServer.RemoveSessionFolderL( aMessage );
            break;
            }

        // Get Session Slot State
        case EMainSessGetSessionState:
            {
            iStorageServer.SessionStateL( aMessage );
            break;
            }

        // Get States of all sessions
        case EMainSessGetAllSessionStates:
            {
            iStorageServer.AllSessionStatesL( aMessage );
            break;
            }

        /*********************************************************************/
        /* Admin sub session requests                                        */
        /*********************************************************************/

        // Wipe session slot folder from storage
        case EMainSessWipeSessionFolder:
            {
            iStorageServer.WipeSessionSlotFolderL( aMessage );
            break;
            }

        /*********************************************************************/
        /* Admin sub session requests                                        */
        /*********************************************************************/

        // Is this request meant to be for admin client
        case EAdminSubSessNotifyNewEvent:
        case EAdminSubSessCheckAppIdRegistration:
            {
            iCompleteMessage =
                FindAdminSubSessionL( aMessage.Int3() )->DispatchMessageL(
                    aMessage,
                    request );
            break;
            }

        default:
            {
            // try to handle message in sub-session
            iCompleteMessage =
                FindFolderSubSessionL( aMessage.Int3() )->DispatchMessageL(
                    aMessage,
                    request );
            break;
            }
        }
    iCompleteMessage &= aMessage.MessageValid();
    }



// -----------------------------------------------------------------------------
// CPEngStorageSession::CreateNewSubSessionL()
// -----------------------------------------------------------------------------
//
void CPEngStorageSession::CreateNewFolderSubSessionL(
    const RPEngMessage& aMessage )
    {
    CPEngStorageSubSession* subSession =
        CPEngStorageSubSession::NewLC(  iStorageServer,
                                        aMessage,
                                        reinterpret_cast<TInt32>( this ) );
    iStorageServer.AddSubSessionL( *subSession );
    CleanupStack::Pop( ); // subSession

    TInt handle( 0 );
    TRAPD( err, handle = iSubSessions->AddL( subSession ) );
    if ( err != KErrNone )
        {
        iStorageServer.RemoveSubSessionL( *subSession );
        User::Leave( err );
        }

    subSession->SetSubSesionHandle( handle );

    // write back handle of the sub-session
    TPckg<TInt> handlePckg( handle );
    err = aMessage.WriteOneDescriptor( KMessageSlot3 ,
                                       handlePckg );
    if ( err != KErrNone )
        {
        iSubSessions->Remove( handle );
        PanicClient( aMessage, ESSBadDescriptor );
        }
    }



// -----------------------------------------------------------------------------
// CPEngStorageSession::CreateAdminSubSessionL()
// -----------------------------------------------------------------------------
//
void CPEngStorageSession::CreateNewAdminSubSessionL(
    const RPEngMessage& aMessage )
    {
    CPEngAdminSubSession* subSession =
        CPEngAdminSubSession::NewLC(  iStorageServer,
                                      aMessage,
                                      reinterpret_cast<TInt32>( this ) );

    iStorageServer.AddSubSessionL( *subSession );
    CleanupStack::Pop( ); // newSubsSession

    TInt handle( 0 );
    TRAPD( err, handle = iSubSessions->AddL( subSession ) );
    if ( err != KErrNone )
        {
        iStorageServer.RemoveSubSessionL( *subSession );
        User::Leave( err );
        }

    subSession->SetSubSesionHandle( handle );

    // write back handle of the sub-session
    TPckg<TInt> handlePckg( handle );
    err = aMessage.WriteOneDescriptor( KMessageSlot3,
                                       handlePckg );
    if ( err != KErrNone )
        {
        iSubSessions->Remove( handle );
        PanicClient( aMessage, ESSBadDescriptor );
        }
    }



// -----------------------------------------------------------------------------
// CPEngStorageSession::RemoveSubSession()
// -----------------------------------------------------------------------------
//
void CPEngStorageSession::RemoveSubSessionL(
    TUint  aHandle )
    {
    // this will leave if there is such a sub-session
    iSubSessions->AtL( aHandle );
    iSubSessions->Remove( aHandle );
    }


// -----------------------------------------------------------------------------
// CPEngStorageSession::FindSubSession()
// -----------------------------------------------------------------------------
//
CPEngStorageSubSession* CPEngStorageSession::FindFolderSubSessionL(
    TUint  aHandle )
    {
    // if there is no sub-session, it leaves with KErrBadHandle
    return static_cast<CPEngStorageSubSession*> (
               iSubSessions->AtL( aHandle ) );
    }


// -----------------------------------------------------------------------------
// CPEngStorageSession::FindAdminSubSessionL()
// -----------------------------------------------------------------------------
//
CPEngAdminSubSession* CPEngStorageSession::FindAdminSubSessionL(
    TUint aHandle )
    {
    // if there is no sub-session, it leaves with KErrBadHandle
    return static_cast<CPEngAdminSubSession*> (
               iSubSessions->AtL( aHandle ) );
    }


// -----------------------------------------------------------------------------
// CPEngStorageSession::PanicClient()
// -----------------------------------------------------------------------------
//
void CPEngStorageSession::PanicClient( const RPEngMessage& aMessage,
                                       const TInt aPanic ) const
    {
    aMessage.Panic( KStorageSessionName, aPanic );
    }


//  End of File









