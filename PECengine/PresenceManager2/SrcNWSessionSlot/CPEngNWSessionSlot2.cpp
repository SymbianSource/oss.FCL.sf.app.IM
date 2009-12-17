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
* Description:  Handle to NWSessionSlot.
*
*/

// INCLUDE FILES
#include "CPEngNWSessionSlot2.h"
#include "CPEngNWSessionSlot2Imp.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlot2* CPEngNWSessionSlot2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::NewL()" );

    CPEngNWSessionSlot2* self = new ( ELeave ) CPEngNWSessionSlot2;
    CleanupStack::PushL( self );
    self->iImp = CPEngNWSessionSlot2Imp::NewL( *self,
                                               aPriority,
                                               aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlot2* CPEngNWSessionSlot2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::NewLC()" );

    CPEngNWSessionSlot2* self = new ( ELeave ) CPEngNWSessionSlot2;
    CleanupStack::PushL( self );
    self->iImp = CPEngNWSessionSlot2Imp::NewL( *self,
                                               aPriority,
                                               aNWSessionSlotID );
    return self;
    }



// Destructor
CPEngNWSessionSlot2::~CPEngNWSessionSlot2()
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::~CPEngNWSessionSlot2()" );

    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::CPEngNWSessionSlot2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlot2::CPEngNWSessionSlot2()
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::GetNWSessionSlotID()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::GetNWSessionSlotID(
    CPEngNWSessionSlotID2& aNWSessionSlotID ) const
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::GetNWSessionSlotID()" );

    return iImp->GetNWSessionSlotID( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::GetOption()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::GetOption( TUint aOptionName,
                                              TUint aOptionLevel,
                                              TDes8& aOption )
    {
    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlot2::GetOption() [%08x][%08x]" ),
             aOptionName, aOptionLevel );

    return iImp->GetOption( aOptionName, aOptionLevel,  aOption );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::IsOpenNWPresenceSessionActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngNWSessionSlot2::IsOpenNWPresenceSessionActive() const
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::IsOpenNWPresenceSessionActive()" );

    return iImp->IsOpenNWPresenceSessionActive();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::OpenNWPresenceSession()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::OpenNWPresenceSession(
    const CWVSAPSettings& /*aSAP*/,
    MPEngNWSessionOperationObserver2& /*aObserver*/ )
    {
    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlot2::OpenNWPresenceSession() - CWVSAPSettings deprecated" ) );
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::OpenNWPresenceSession()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::OpenNWPresenceSession(
    const CIMPSSAPSettings& aSAP,
    MPEngNWSessionOperationObserver2& aObserver )
    {
    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlot2::OpenNWPresenceSession()" ) );

    return iImp->OpenNWPresenceSession( aSAP, aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::CancelOpenNWPresenceSession()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlot2::CancelOpenNWPresenceSession()
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::CancelOpenNWPresenceSession()" );

    iImp->CancelOpenNWPresenceSession();
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::CancelOpenNWPresenceSession()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::OpenNWPresenceSessionOwnership()
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::OpenNWPresenceSessionOwnership()" );

    return iImp->OpenNWPresenceSessionOwnership();
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::IsCloseNWPresenceSessionActive()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngNWSessionSlot2::IsCloseNWPresenceSessionActive() const
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::IsCloseNWPresenceSessionActive()" );

    return iImp->IsCloseNWPresenceSessionActive();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::CloseNWPresenceSession()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::CloseNWPresenceSession(
    MPEngNWSessionOperationObserver2& aObserver )
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::CloseNWPresenceSession()" );

    return iImp->CloseNWPresenceSession( aObserver, EFalse );
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::ForceCloseNWPresenceSession()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::ForceCloseNWPresenceSession(
    MPEngNWSessionOperationObserver2& aObserver )
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::ForceCloseNWPresenceSession()" );

    return iImp->CloseNWPresenceSession( aObserver, ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::CancelCloseNWPresenceSession()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlot2::CancelCloseNWPresenceSession()
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::CancelCloseNWPresenceSession()" );

    iImp->CancelCloseNWPresenceSession();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::CloseNWPresenceSessionOwnership()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlot2::CloseNWPresenceSessionOwnership()
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::CloseNWPresenceSessionOwnership()" );

    iImp->CloseNWPresenceSessionOwnership();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::StoreNWPresenceSessionOwnership()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::StoreNWPresenceSessionOwnership(
    const TDesC16& aOwnerID )
    {
    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlot2::StoreNWPresenceSessionOwnership() [%S]" ),
             &aOwnerID );

    return iImp->StoreNWPresenceSessionOwnership( aOwnerID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::RestoreNWPresenceSessionOwnership()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::RestoreNWPresenceSessionOwnership(
    const TDesC16& aOwnerID )
    {
    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlot2::RestoreNWPresenceSessionOwnership() [%S]" ),
             &aOwnerID );

    return iImp->RestoreNWPresenceSessionOwnership( aOwnerID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::GetNWPresenceSessionSap()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::GetNWPresenceSessionSap(
    CWVSAPSettings& /*aSAP*/ ) const
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::GetNWPresenceSessionSap() - CWVSAPSettings deprecated" );
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::GetNWPresenceSessionSap()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::GetNWPresenceSessionSap(
    CIMPSSAPSettings& aSAP ) const
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::GetNWPresenceSessionSap() - CIMPSSAPSettings" );

    return iImp->GetNWPresenceSessionSap( aSAP );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::GetNWSessionSlotState()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlot2::GetNWSessionSlotState( TPEngNWSessionSlotState& aState )  const
    {
    PENG_DP_TXT( "CPEngNWSessionSlot2::GetNWSessionSlotState()" );

    return iImp->GetNWSessionSlotState( aState );
    }



// End of File


