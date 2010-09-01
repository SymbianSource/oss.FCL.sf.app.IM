/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Admin Sub-session class of the Storage Server
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include	"CPEngAdminSubSession.h"
#include	"PEngStorageServerCommon.h"

#include	"CPEngStorageFolder.h"
#include	"CPEngSessionSlotState.h"
#include	"CPEngSessionSlotEvent.h"
#include	"PEngPresenceEngineConsts2.h"

#include	"MPEngStorageServer.h"
#include	"RPEngMessage.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAdminSubSession::CPEngAdminSubSession
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngAdminSubSession::CPEngAdminSubSession(
    MPEngStorageServer& aServer,
    TInt32 aSessionId )
        : iSessionId( aSessionId ),
        iServer( aServer )
    {

    }


// -----------------------------------------------------------------------------
// CPEngAdminSubSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAdminSubSession::ConstructL(
    const RPEngMessage& aMessage )
    {
    iStorageFolder = iServer.StorageFolderL( aMessage );
    }


// -----------------------------------------------------------------------------
// CPEngAdminSubSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAdminSubSession* CPEngAdminSubSession::NewL(
    MPEngStorageServer& aServer,
    const RPEngMessage& aMessage,
    TInt32 aSessionId )
    {
    CPEngAdminSubSession* self = NewLC( aServer, aMessage, aSessionId );

    CleanupStack::Pop(); // self

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAdminSubSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAdminSubSession* CPEngAdminSubSession::NewLC(
    MPEngStorageServer& aServer,
    const RPEngMessage& aMessage,
    TInt32 aSessionId )
    {
    CPEngAdminSubSession* self = new( ELeave ) CPEngAdminSubSession(
        aServer,
        aSessionId );

    CleanupClosePushL( *self );
    self->ConstructL( aMessage );
    return self;
    }


// Destructor
CPEngAdminSubSession::~CPEngAdminSubSession()
    {
    // decrease count of the storage Folder
    TRAP_IGNORE( NotifySessionSlotCloseL() );
    if ( iStorageFolder )
        {
        // notify globally
        iStorageFolder->CancelSubSessionRequests( iSessionId, iHandle );
        iStorageFolder->Close();
        }
    }



// =============================================================================
// =============== Functions from base class ===================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAdminSubSession::DispatchMessageL()
// -----------------------------------------------------------------------------
//
TBool CPEngAdminSubSession::DispatchMessageL(
    const RPEngMessage& aMessage,
    TInt aRequest )
    {
    TBool completeMessage( ETrue );
    switch ( aRequest )
        {
            // ================ Admin  sub client requests ========================
            /*********************************************************************/
            /* Admin sub session requests                                        */
            /*********************************************************************/

            // Notify new global event
        case EAdminSubSessNotifyNewEvent:
            {
            NotifyGlobalEventL( aMessage );
            break;
            }

        // Check if passed App Id is registered for passed Session Slot
        case EAdminSubSessCheckAppIdRegistration:
            {
            CheckAppIdRegistrationL( aMessage );
            break;
            }

        case EAdminSubSessNotifyError:
            {
            NotifyError( aMessage );
            break;
            }

        default:
            {
            PanicClient( aMessage, ESSBadRequest );
            break;
            }
        }
    // check if message can be completed
    completeMessage &= aMessage.MessageValid();
    return completeMessage;
    }


// -----------------------------------------------------------------------------
// CPEngAdminSubSession::SetSubSesionHandle()
// -----------------------------------------------------------------------------
//
void CPEngAdminSubSession::SetSubSesionHandle(
    TInt aHandle )
    {
    iHandle = aHandle;
    }


// =============================================================================
// =============== Private Functions from base class ===========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAdminSubSession::PanicClient()
// Panic client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAdminSubSession::PanicClient(
    const RPEngMessage& aMessage,
    const TInt aPanic ) const
    {
    aMessage.Panic( KStorageSessionName, aPanic );
    }


// -----------------------------------------------------------------------------
// CPEngAdminSubSession::NotifyGlobalEventL()
// Notify about new global event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAdminSubSession::NotifyGlobalEventL(
    const RPEngMessage& aMessage )
    {
    // Try to unpack the event
    CPEngSessionSlotEvent* newEvent = CPEngSessionSlotEvent::NewLC();
    HBufC8* evetBuff = aMessage.ReadOneDescriptor8LC( KMessageSlot0 );
    newEvent->UnpackEventL( *evetBuff );
    // update state
    iStorageFolder->SessionSlotState().UpdateAndCommitStateL( *newEvent );
    iStorageFolder->CommiteStateUpdate();
    iServer.NotifyGlobalEvent( *evetBuff );
    CleanupStack::PopAndDestroy( 2 );
    }

// -----------------------------------------------------------------------------
// CPEngAdminSubSession::CheckAppIdRegistrationL()
// Notify Error to listeners
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAdminSubSession::NotifyError(
    const RPEngMessage& aMessage )
    {
    iServer.NotifyError( aMessage.Int0() );
    }

// -----------------------------------------------------------------------------
// CPEngAdminSubSession::CheckAppIdRegistrationL()
// Check application registration
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAdminSubSession::CheckAppIdRegistrationL(
    const RPEngMessage& aMessage )
    {
    HBufC* appId = aMessage.ReadOneDescriptor16LC( KMessageSlot0 );
    iStorageFolder->SessionSlotState().ApplicationRegisteredL( *appId );
    CleanupStack::PopAndDestroy(); // appId
    }

// -----------------------------------------------------------------------------
// CPEngAdminSubSession::NotifySessionSlotCloseL()
// Notify Global Session slot close
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngAdminSubSession::NotifySessionSlotCloseL()
    {
    // Construct new Session slot event
    if ( !iStorageFolder )
        {
        return;
        }
    iStorageFolder->SessionSlotState().SetSessionSlotStateClosed();
    iStorageFolder->CommiteStateUpdate();
    CPEngSessionSlotEvent* newEvent = CPEngSessionSlotEvent::NewLC();
    newEvent->SetSessionSlotId(
        &( iStorageFolder->SessionSlotState().SessionSlotId() ),
        EFalse );
    HBufC8* eventBuff = newEvent->PackEventLC();
    iServer.NotifyGlobalEvent( *eventBuff );
    CleanupStack::PopAndDestroy( 2 ); // eventBuff, newEvent
    }

//  End of File
