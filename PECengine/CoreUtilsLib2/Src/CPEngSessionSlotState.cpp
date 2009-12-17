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
* Description:  NWSessionSlotID implementation.
*
*/

// INCLUDE FILES
#include    "CPEngSessionSlotState.h"
#include    "CPEngSessionSlotId.h"
#include    "CPEngSessionSlotEvent.h"
#include    "PEngCoreUtilsTools.h"



// CONSTANTS

/**
 * Default array granurality.
 */
const TInt KArrayGranularity = 5;


/**
 * Version of the session state storage format.
 */
const TInt KSessionStateVersion = 0x00000001;




// ====================== LOCAL FUNCTIONS ======================================

//LOCAL PANIC FUNCTIONS
namespace
    {
    //SessionSlotState panic
    _LIT( KPEngSessionSlotStatePanic, "SessSlotState" );

    //SessionSlotState panic reasons
    enum TPEngSessionSlotStatePanicReasons
        {
        EPEngNoSessionSlotId = 1,
        EPEngSessionUpdateInconsistent = 2,
        EPEngSessionOpenAppNotRegistered = 3,
        EPEngSessionCloseAppNotRegistered = 4,
        EPEngSessionOpenGlobalStateNotCorrect = 5,
        EPEngSessionHasActiveAppsAtClosedState = 6
        };

    //SessionSlotState panic function
    void PanicSessionSlotState( TPEngSessionSlotStatePanicReasons aPanicReason )
        {
        User::Panic( KPEngSessionSlotStatePanic, aPanicReason );
        }
    }


// -----------------------------------------------------------------------------
// DesArrayExternalizeSize()
// -----------------------------------------------------------------------------
TInt DesArrayExternalizeSize( const MDesC16Array& aArray )
    {
    TInt size = 4; //array count

    const TInt count( aArray.MdcaCount() );
    for ( TInt ix = 0 ; ix < count ; ix++ )
        {
        //Below: descriptor length is streamed as 32 bit integer ==> 4 bytes
        size += aArray.MdcaPoint( ix ).Size() + 4;
        }
    return size;
    }



// -----------------------------------------------------------------------------
// ExternalizeDesArrayWithSkipL()
// -----------------------------------------------------------------------------
void ExternalizeDesArrayWithSkipL( const MDesC16Array& aArray,
                                   TInt aIxToSkip,
                                   RWriteStream& aStream )
    {
    TInt count ( aArray.MdcaCount() );
    aStream.WriteInt32L( count - ( aIxToSkip == KErrNotFound ? 0 : 1 ) );

    for ( TInt ix = 0 ; ix < count ; ix++ )
        {
        if ( ix == aIxToSkip )
            {
            continue;
            }

        TPtrC16 data( aArray.MdcaPoint( ix ) );
        aStream.WriteInt32L( data.Length() );
        aStream.WriteL( data );
        }
    }


// -----------------------------------------------------------------------------
// InternalizeDesArrayL()
// -----------------------------------------------------------------------------
void InternalizeDesArrayL( CDesCArray& aArray,
                           RReadStream& aStream )
    {
    aArray.Reset();

    TInt count = aStream.ReadInt32L();
    for ( TInt ii = 0; ii < count; ii++ )
        {
        TInt length = aStream.ReadInt32L();
        HBufC* buffer = HBufC::NewLC( length );
        TPtr ptr( buffer->Des() );
        aStream.ReadL( ptr, length );
        aArray.AppendL( *buffer );
        CleanupStack::PopAndDestroy( buffer );
        }
    }




// ====================== MEMBER FUNCTIONS ======================================

// -----------------------------------------------------------------------------
// CPEngSessionSlotState::CPEngSessionSlotState
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSessionSlotState::CPEngSessionSlotState()
        : iState( EPEngNWPresenceSessionClosed ),
        iActiveApps( KArrayGranularity ),
        iRegisteredApps( KArrayGranularity ),
        iActiveAdded( KErrNotFound ),
        iActiveRemoved( KErrNotFound ),
        iRegAdded( KErrNotFound ),
        iRegRemoved( KErrNotFound )
    {
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotState* CPEngSessionSlotState::NewL()
    {
    CPEngSessionSlotState* self = new ( ELeave ) CPEngSessionSlotState();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotState* CPEngSessionSlotState::NewLC()
    {
    CPEngSessionSlotState* self = new ( ELeave ) CPEngSessionSlotState();
    CleanupStack::PushL( self );
    return self;
    }


// Destructor
CPEngSessionSlotState::~CPEngSessionSlotState()
    {
    if ( iSessionIdOwned )
        {
        delete iSessionId;
        }

    iActiveApps.Reset();
    iRegisteredApps.Reset();
    }



// =============================================================================
// =============== Session ID support ==========================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSessionSlotState::SessionSlotIndentification()
// Session Slot ID identification
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotId& CPEngSessionSlotState::SessionSlotId() const
    {
    __ASSERT_ALWAYS( iSessionId, PanicSessionSlotState( EPEngNoSessionSlotId ) );
    return *iSessionId;
    }



// =============================================================================
// =============== State identificationst ======================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSessionSlotState::SessionSlotState()
// Session slot Online state
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotState CPEngSessionSlotState::SessionSlotState() const
    {
    return iState;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::RegisteredApplications()
// Gets list of Application IDs which are active with this
// -----------------------------------------------------------------------------
//
EXPORT_C const MDesC16Array& CPEngSessionSlotState::RegisteredApplications() const
    {
    return iRegisteredApps;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::ActiveApplications()
// Gets list of Application IDs which are registered to this
// -----------------------------------------------------------------------------
//
EXPORT_C const MDesC16Array& CPEngSessionSlotState::ActiveApplications() const
    {
    return iActiveApps;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::AppState()
// Gets state of the application in the session state
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotState CPEngSessionSlotState::AppState(
    const TDesC& aAppId ) const
    {
    TInt pos( KErrNotFound );
    if ( KErrNone == iActiveApps.FindIsq( aAppId, pos ) )
        {
        return EPEngNWPresenceSessionOpen;
        }

    return EPEngNWPresenceSessionClosed;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::ApplicationRegisteredL()
// Check if application is registered to this session slot
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotState::ApplicationRegisteredL(
    const TDesC& aAppId ) const
    {
    __ASSERT_ALWAYS( ApplicationRegistered( aAppId ), User::Leave( KErrNotFound ) );
    }




// =============================================================================
// =============== identification management ====================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSessionSlotState::SetSessionStlotId()
// Set Session slot ID class
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotState::SetSessionSlotId(
    CPEngSessionSlotId& aSlotId,
    TBool aOwnershipTransfered /* EFalse */ )
    {
    if ( iSessionId && iSessionIdOwned )
        {
        delete iSessionId;
        }

    iSessionId = &aSlotId;
    iSessionIdOwned = aOwnershipTransfered;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::SetSessionSlotStateClosed()
// Set state of the session slot closed
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotState::SetSessionSlotStateClosed()
    {
    iState = EPEngNWPresenceSessionClosed;
    iActiveApps.Reset();
    iActiveAdded = KErrNotFound;
    iActiveRemoved = KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::UpdateStateL()
// Update Session slot state based on the event
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngSessionSlotState::UpdateStateL(
    const CPEngSessionSlotEvent& aSessEvent )
    {
    //Precheck - SlotId must match
    __ASSERT_ALWAYS( aSessEvent.SessionSlotIndentification().Match( SessionSlotId() ) == KErrNone,
                     PanicSessionSlotState( EPEngSessionUpdateInconsistent ) );


    // first commit last update
    CommitLastUpdate();

    // set state in state from the event
    iState = aSessEvent.GlobSessSltState();


    // Check what has happened
    TBool permanentStoreNeeded = EFalse;
    switch ( aSessEvent.Event() )
        {
        case EPEngEventNWSessionSlotCreated:
            {
            // New Application Id was registered to the session
            AddRegisteredAppIdL( aSessEvent.ApplicationId() );

            // this needs to be store to permanent
            permanentStoreNeeded = ETrue;
            break;
            }


        case EPEngEventNWSessionSlotRemoved:
            {
            // Application Id was removed from the session slot
            RemoveRegisteredAppIdL( aSessEvent.ApplicationId() );

            // this needs to be store to permanent
            permanentStoreNeeded = ETrue;
            break;
            }


        case EPEngEventAppNWPresenceSessionOpened:
            {
            // New Application Id has activated session
            __ASSERT_ALWAYS( ApplicationRegistered( aSessEvent.ApplicationId() ),
                             PanicSessionSlotState( EPEngSessionOpenAppNotRegistered ) );

            __ASSERT_ALWAYS( iState  == EPEngNWPresenceSessionOpen,
                             PanicSessionSlotState( EPEngSessionOpenGlobalStateNotCorrect ) );

            AddActiveAppIdL( aSessEvent.ApplicationId() );
            break;
            }


        case EPEngEventAppNWPresenceSessionClosed:
        case EPEngEventNWSessionClosedByServer:
            {
            // Application Id has closed the session
            // Closing event does not have to have any App Id..
            RemoveActiveAppId( aSessEvent.ApplicationId() );
            break;
            }


        case EPEngEventNWSessionSlotChanged:
        case EPEngEventNWSessionTransport:
        default:
            {
            // default does nothing
            break;
            }
        }


    return permanentStoreNeeded;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::UpdateAndCommitStateL()
// Update Session slot state based on the event and commit the state
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngSessionSlotState::UpdateAndCommitStateL(
    const CPEngSessionSlotEvent& aSessEvent )
    {
    TBool storeNeed( UpdateStateL( aSessEvent ) );
    CommitLastUpdate();
    return storeNeed;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::RollBackLastUpdate()
// Rollback last change
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotState::RollBackLastUpdate()
    {
    if ( iActiveAdded != KErrNotFound )
        {
        iActiveApps.Delete( iActiveAdded );
        iActiveAdded = KErrNotFound;
        }

    iActiveRemoved = KErrNotFound;
    if ( iRegAdded != KErrNotFound )
        {
        iRegisteredApps.Delete( iRegAdded );
        iRegAdded = KErrNotFound;
        }
    iRegRemoved = KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::CommitLastUpdate()
// Commit last change
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotState::CommitLastUpdate()
    {
    iActiveAdded = KErrNotFound;
    iRegAdded = KErrNotFound;
    if ( iActiveRemoved != KErrNotFound )
        {
        iActiveApps.Delete( iActiveRemoved );
        iActiveRemoved = KErrNotFound;
        }
    if ( iRegRemoved != KErrNotFound )
        {
        iRegisteredApps.Delete( iRegRemoved );
        iRegRemoved = KErrNotFound;
        }
    }



// =============================================================================
// =============== Export, Import Functions ====================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSessionSlotState::InternalizeL()
// Internalize Session Slot State from stream
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotState::InternalizeL( RReadStream& aStream,
                                                   TPEngDataSelection aSelection )
    {
    if ( EPermanentData & aSelection )
        {
        TInt version( aStream.ReadInt32L() );
        if ( version != KSessionStateVersion )
            {
            User::Leave( KErrCorrupt );
            }
        }


    // shall we unpack identification
    if ( EIdentification & aSelection )
        {
        CPEngSessionSlotId* newId = CPEngSessionSlotId::NewLC();
        newId->InternalizeL( aStream );
        SetSessionSlotId( *newId, ETrue );
        CleanupStack::Pop( newId );
        }


    // shall we unpack Registered application IDs
    if ( ERegistredIDs & aSelection )
        {
        InternalizeDesArrayL( iRegisteredApps, aStream );
        }


    // shall we unpack State specific data
    if ( EStateSpecific & aSelection )
        {
        iState = static_cast<TPEngNWSessionSlotState>( aStream.ReadInt32L() );
        InternalizeDesArrayL( iActiveApps, aStream );
        }

    }



// -----------------------------------------------------------------------------
// CPEngSessionSlotState::ExternalizeL()
// Externalize Session Slot state to stream
// -----------------------------------------------------------------------------
//

EXPORT_C void CPEngSessionSlotState::ExternalizeL( RWriteStream& aStream,
                                                   TPEngDataSelection aSelection ) const
    {
    if ( EPermanentData & aSelection )
        {
        // pack store version
        aStream.WriteInt32L( KSessionStateVersion );
        }


    // shall we pack identification
    if ( EIdentification & aSelection )
        {
        SessionSlotId().ExternalizeL( aStream );
        }


    // shall we pack Registered application IDs
    if ( ERegistredIDs & aSelection )
        {
        ExternalizeDesArrayWithSkipL( iRegisteredApps, iRegRemoved, aStream );
        }


    // shall we pack State specific data
    if ( EStateSpecific & aSelection )
        {
        aStream.WriteInt32L( iState );
        ExternalizeDesArrayWithSkipL( iActiveApps, iActiveRemoved, aStream );
        }
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::Size()
// Size of the buffer needed for externalization
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSessionSlotState::Size( TPEngDataSelection aSelection ) const
    {
    TInt size = 0;

    if ( EPermanentData & aSelection )
        {
        size += 4; //KSessionStateVersion
        }


    if ( EIdentification & aSelection )
        {
        size += SessionSlotId().Size();
        }


    if ( EStateSpecific & aSelection )
        {
        size += 4; //iState
        size += DesArrayExternalizeSize( iActiveApps );
        }


    if ( ERegistredIDs & aSelection )
        {
        size += DesArrayExternalizeSize( iRegisteredApps );
        }


    return size;
    }



// -----------------------------------------------------------------------------
// CPEngSessionSlotState::PackDataLC()
// Pack Session Slot state to the the des buffer
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPEngSessionSlotState::PackDataLC(
    TPEngDataSelection aSelection ) const
    {
    HBufC8* packBuffer = HBufC8::NewLC( Size( aSelection ) );
    TPtr8 pack( packBuffer->Des() );

    RDesWriteStream ws;
    ws.Open( pack );                    // CSI: 65 #
    CleanupClosePushL( ws );

    ExternalizeL( ws, aSelection );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws

    packBuffer = packBuffer->ReAllocL( packBuffer->Length() );
    CleanupStack::Pop();
    CleanupStack::PushL( packBuffer ); //Due realloc

    return packBuffer;
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotState::PackDataLC()
// Pack Session Slot state to the the des buffer
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotState::PackDataL(
    TPEngDataSelection aSelection,
    RBuf8& aBuf ) const
    {
    TInt size( Size( aSelection ) );
    if ( aBuf.MaxSize() < size )
        {
        aBuf.ReAllocL( size );
        }
    aBuf.Zero();

    RDesWriteStream ws;
    ws.Open( aBuf );                    // CSI: 65 #
    CleanupClosePushL( ws );

    ExternalizeL( ws, aSelection );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotState::UnpackDataL()
// Unpack Session Slot state from the passed Descriptor
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotState::UnpackDataL(
    const TDesC8& aDes,
    TPEngDataSelection aSelection )
    {
    RDesReadStream rs;
    rs.Open( aDes );                    // CSI: 65 #
    CleanupClosePushL( rs );
    InternalizeL( rs, aSelection );
    CleanupStack::PopAndDestroy(); // rs
    }



// =============================================================================
// ============== New private helping functions of the class ===================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::AddRegisteredAppIdL()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotState::AddRegisteredAppIdL(
    const TDesC& aAppId )
    {
    __ASSERT_ALWAYS( aAppId.Length() > 0, User::Leave( KErrArgument ) );

    iRegAdded = iRegisteredApps.InsertIsqL( aAppId );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::RemoveRegisteredAppIdL()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotState::RemoveRegisteredAppIdL(
    const TDesC& aAppId )
    {
    TInt pos( 0 );
    if ( KErrNone != iActiveApps.Find( aAppId, pos ) )
        {
        if ( KErrNone == iRegisteredApps.Find( aAppId, pos ) )
            {
            // can we delete ID, isn't it last one
            if ( ( iRegisteredApps.Count() == 1 )
                 &&
                 ( iState == EPEngNWPresenceSessionOpen )
               )
                {
                // App Id cannot be deleted, last active one
                User::Leave( KErrNotSupported );
                }
            iRegRemoved = pos;
            return;
            }
        // ID was not found
        User::Leave( KErrNotFound );
        }
    // this application is still active, refuse deletion
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::AddActiveAppIdL()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotState::AddActiveAppIdL(
    const TDesC& aAppId )
    {
    // we relly on Presence Server that IDs are unique
    iActiveAdded =  iActiveApps.InsertIsqL( aAppId );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::RemoveActiveAppId()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotState::RemoveActiveAppId( const TDesC& aAppId )
    {
    TInt pos( 0 );
    if ( KErrNone == iActiveApps.Find( aAppId, pos ) )
        {
        iActiveRemoved = pos;
        }
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotState::ApplicationRegistered()
// -----------------------------------------------------------------------------
//
TBool CPEngSessionSlotState::ApplicationRegistered( const TDesC& aAppId ) const
    {
    TInt pos( KErrNotFound );
    if ( KErrNone != iRegisteredApps.FindIsq( aAppId, pos ) )
        {
        return EFalse;
        }

    return ETrue;
    }


// End of File


