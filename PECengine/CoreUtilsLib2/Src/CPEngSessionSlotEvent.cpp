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
* Description:  Session Slot Event container.
*
*/

// INCLUDE FILES
#include    "CPEngSessionSlotEvent.h"
#include    "PEngCoreUtilsTools.h"
#include    "CPEngSessionSlotId.h"



// CONSTANTS

//LOCAL PANIC
namespace
    {
    //SessionSlotEvent panic
    _LIT( KPEngSessionSlotEventPanic, "SessSlotEvent" );

    //SessionSlotEvent panic reasons
    enum TPEngSessionSlotEventPanicReasons
        {
        EPEngNoSessionSlotId = 1
        };

    void PanicSessionSlotEvent( TPEngSessionSlotEventPanicReasons aPanicReason )
        {
        User::Panic( KPEngSessionSlotEventPanic, aPanicReason );
        }
    }




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::CPEngSessionSlotEvent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSessionSlotEvent::CPEngSessionSlotEvent()
        : iIdentificationOwned( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotEvent* CPEngSessionSlotEvent::NewL()
    {
    CPEngSessionSlotEvent* self = new ( ELeave ) CPEngSessionSlotEvent();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSessionSlotEvent* CPEngSessionSlotEvent::NewLC()
    {
    CPEngSessionSlotEvent* self = new ( ELeave ) CPEngSessionSlotEvent();
    CleanupStack::PushL( self );
    return self;
    }


// Destructor
CPEngSessionSlotEvent::~CPEngSessionSlotEvent()
    {
    if ( iIdentificationOwned )
        {
        delete iSessionId;
        }
    delete iEventDesc;
    delete iApplicationId;


#if _BullseyeCoverage
    cov_write();
#endif
    }



// =============================================================================
// ======================= Session Slot Event ==================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::SessionSlotIndentification()
// -----------------------------------------------------------------------------
//
EXPORT_C const CPEngSessionSlotId& CPEngSessionSlotEvent::SessionSlotIndentification( ) const
    {
    __ASSERT_ALWAYS( iSessionId, PanicSessionSlotEvent( EPEngNoSessionSlotId ) );
    return *iSessionId;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::ApplicationId()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPEngSessionSlotEvent::ApplicationId( void ) const
    {
    return DoGetDescriptor( iApplicationId );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::Event()
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotEvent CPEngSessionSlotEvent::Event( void ) const
    {
    return iEvent;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::AppSessSlotState()
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotState CPEngSessionSlotEvent::AppSessSltState( void ) const
    {
    return iAppSessState;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::GlobSessSltState()
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotState CPEngSessionSlotEvent::GlobSessSltState( void ) const
    {
    return iGlobSessState;
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::EventInt()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSessionSlotEvent::EventInt( void ) const
    {
    return iEventInt;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::EventDesc()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPEngSessionSlotEvent::EventDesc( void ) const
    {
    return DoGetDescriptor( iEventDesc );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::SetSessionStlotId()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::SetSessionSlotId(
    CPEngSessionSlotId* aSlotId,
    TBool aOwnershipTransfered /* EFalse */ )
    {
    if ( iSessionId && iIdentificationOwned )
        {
        delete iSessionId;
        }

    iSessionId = aSlotId;
    iIdentificationOwned = aOwnershipTransfered;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::SetAppIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::SetAppIdL( const TDesC& aAppId )
    {
    DoSetDescriptorL( iApplicationId, aAppId );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::SetSessSltEvent()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::SetSessSltEvent(
    TPEngNWSessionSlotEvent aEvent )
    {
    iEvent = aEvent;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::SetAppSessSltState()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::SetAppSessSltState(
    TPEngNWSessionSlotState aState )
    {
    iAppSessState = aState;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::SetGlobSessSltState()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::SetGlobSessSltState(
    TPEngNWSessionSlotState aState )
    {
    iGlobSessState = aState;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::SetEventInt()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::SetEventInt(
    TInt aEventInt )
    {
    iEventInt = aEventInt;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::SetEventDesL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::SetEventDesL(
    const TDesC& aEventDes )
    {
    DoSetDescriptorL( iEventDesc, aEventDes );
    }




// =============================================================================
// =========== Externalize Internalize functions ===============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::InternalizeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::InternalizeL( RReadStream& aStream,
                                                   TBool aAllData )
    {
    if ( aAllData )
        {
        CPEngSessionSlotId* newId = CPEngSessionSlotId::NewLC();
        newId->InternalizeL( aStream );
        SetSessionSlotId( newId, ETrue );
        CleanupStack::Pop( newId );
        }

    delete iApplicationId;
    iApplicationId = NULL;
    iApplicationId = InternalizeBufferL( aStream );


    iEvent = static_cast<TPEngNWSessionSlotEvent>( aStream.ReadInt32L() );
    iAppSessState = static_cast<TPEngNWSessionSlotState>( aStream.ReadInt32L() );
    iGlobSessState = static_cast<TPEngNWSessionSlotState>( aStream.ReadInt32L() );


    iEventInt = aStream.ReadInt32L();

    delete iEventDesc;
    iEventDesc = NULL;
    iEventDesc = InternalizeBufferL( aStream );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::UnpackEventL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::UnpackEventL( const TDesC8& aDes )
    {
    RDesReadStream rs;
    rs.Open( aDes );               // CSI: 65 #
    CleanupClosePushL( rs );
    InternalizeL( rs, ETrue );
    CleanupStack::PopAndDestroy(); // rs
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::UnpackEventPartL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::UnpackEventPartL( const TDesC8& aDes )
    {
    RDesReadStream rs;
    rs.Open( aDes );               // CSI: 65 #
    CleanupClosePushL( rs );

    InternalizeL( rs, EFalse );

    CleanupStack::PopAndDestroy(); // rs
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::ExternalizeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::ExternalizeL( RWriteStream& aStream,
                                                   TBool aAllData ) const
    {
    if ( aAllData )
        {
        __ASSERT_ALWAYS( iSessionId, PanicSessionSlotEvent( EPEngNoSessionSlotId ) );
        iSessionId->ExternalizeL( aStream );
        }

    ExternalizeBufferL( iApplicationId, aStream );

    aStream.WriteInt32L( iEvent );
    aStream.WriteInt32L( iAppSessState );
    aStream.WriteInt32L( iGlobSessState );

    aStream.WriteInt32L( iEventInt );
    ExternalizeBufferL( iEventDesc, aStream );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::Size()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSessionSlotEvent::Size( TBool aAllData ) const
    {
    TInt size = 16; // iEvent: 4 bytes
    // iAppSessState: 4 bytes
    // iGlobSessState: 4 bytes
    // iEventInt: 4 bytes

    if ( aAllData )
        {
        __ASSERT_ALWAYS( iSessionId, PanicSessionSlotEvent( EPEngNoSessionSlotId ) );
        size += iSessionId->Size();
        }

    size += BufferExternalizeSizeInBytes( iApplicationId );
    size += BufferExternalizeSizeInBytes( iEventDesc );

    return size;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::PackEventLC()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPEngSessionSlotEvent::PackEventLC()
    {
    HBufC8* packBuffer = HBufC8::NewLC( Size( ETrue ) );
    TPtr8 pack( packBuffer->Des() );

    RDesWriteStream ws;
    ws.Open( pack );               // CSI: 65 #
    CleanupClosePushL( ws );

    ExternalizeL( ws, ETrue );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws

    packBuffer = packBuffer->ReAllocL( packBuffer->Length() );
    CleanupStack::Pop();
    CleanupStack::PushL( packBuffer ); //Due realloc
    return packBuffer;
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::PackEventL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSessionSlotEvent::PackEventL( RBuf8& aBuf )
    {
    TInt size( Size( ETrue ) );
    if ( aBuf.MaxSize() < size )
        {
        aBuf.ReAllocL( size );
        }
    aBuf.Zero();

    RDesWriteStream ws;
    ws.Open( aBuf );              // CSI: 65 #
    CleanupClosePushL( ws );

    ExternalizeL( ws, ETrue );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws
    }

// -----------------------------------------------------------------------------
// CPEngSessionSlotEvent::PackEventPartL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPEngSessionSlotEvent::PackEventPartLC()
    {
    HBufC8* packBuffer = HBufC8::NewLC( Size( EFalse ) );
    TPtr8 pack( packBuffer->Des() );

    RDesWriteStream ws;
    ws.Open( pack );              // CSI: 65 #
    CleanupClosePushL( ws );

    ExternalizeL( ws, EFalse );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws

    packBuffer = packBuffer->ReAllocL( packBuffer->Length() );
    CleanupStack::Pop();
    CleanupStack::PushL( packBuffer ); //Due realloc
    return packBuffer;
    }


// End of File


