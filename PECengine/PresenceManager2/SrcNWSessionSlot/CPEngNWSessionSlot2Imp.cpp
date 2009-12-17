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
* Description:  NWSessionSlot handle implementation.
*
*/

// INCLUDE FILES
#include "CPEngNWSessionSlot2Imp.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngNWSessionOperation.h"
#include "CPEngNWSessionOpenOp.h"
#include "CPEngNWSessionCloseOp.h"
#include "PEngLoginDataCodec.h"


#include "CPEngSessionSlotState.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "PEngStorageManager.h"
#include "CPEngNWSessionProxy.h"


#include <PEngPresenceEngineConsts2.h>
#include <CPEngNWSessionSlot2.h>
#include <CPEngNWSessionSlotID2.h>
#include <PEngWVServices2.h>



//LOCAL constants
#ifdef _DEBUG
namespace
    {
    //Panic
    _LIT( KPEngNWSessionSlotPanic, "PEngAtLstTrans" );

    //Panic reasons
    enum TPEngNWSessionSlotPanicReasons
        {
        ENWSessionSlotUnkownOp
        };

    void PEngNWSessionSlotPanic( TPEngNWSessionSlotPanicReasons aPanicReason )
        {
        User::Panic( KPEngNWSessionSlotPanic, aPanicReason );
        }
    }
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlot2Imp* CPEngNWSessionSlot2Imp::NewL( CPEngNWSessionSlot2& aInterface,
                                                      TInt aPriority,
                                                      const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngNWSessionSlot2Imp* self =
        new ( ELeave ) CPEngNWSessionSlot2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngNWSessionSlot2Imp::~CPEngNWSessionSlot2Imp()
    {
    //Cancel also deletes the operation objects
    CancelOpenNWPresenceSession();
    CancelCloseNWPresenceSession();

    delete iUsedSlot;
    delete iNWSessionProxy;

#if _BullseyeCoverage
    cov_write();
#endif
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::CPEngNWSessionSlot2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlot2Imp::CPEngNWSessionSlot2Imp( CPEngNWSessionSlot2& aInterface,
                                                TInt aPriority )
        : iInterface( aInterface ),
        iActivePriority( aPriority )
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlot2Imp::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iNWSessionProxy = CPEngNWSessionProxy::NewL( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::GetNWSessionSlotID()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::GetNWSessionSlotID( CPEngNWSessionSlotID2& aNWSessionSlotID ) const
    {
    TRAPD( err, aNWSessionSlotID.Implementation()->SetAllL( iUsedSlot->BaseId(),
                                                            iUsedSlot->AppId() ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::GetOption()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::GetOption( TUint aOptionName,
                                        TUint aOptionLevel,
                                        TDes8& aOption )
    {
    if ( ( aOptionName == KPEngWVCspServicesTree2 ) &&
         ( aOptionLevel == KPEngNWSessionSlotTransportQuery ) )
        {
        if ( aOption.Length() != sizeof( TPEngWVCspServicesTree2 ) )
            {
            return KErrArgument;
            }

        TPEngWVCspServicesTree2 cspTree;
        TInt err = iNWSessionProxy->GetServiceTree( cspTree );
        TPckg<TPEngWVCspServicesTree2> cspTreeBuf( cspTree );
        aOption = cspTreeBuf;

        return err;
        }

    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::IsOpenNWPresenceSessionActive()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlot2Imp::IsOpenNWPresenceSessionActive() const
    {
    return( iSessionOpenOp != NULL );       // CSI: 64 #
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::OpenNWPresenceSession()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::OpenNWPresenceSession(
    const CIMPSSAPSettings& aSAP,
    MPEngNWSessionOperationObserver2& aObserver )
    {
    if ( iSessionOpenOp || iSessionCloseOp )
        {
        return KErrInUse;
        }

    CPEngNWSessionOpenOp* op = NULL;
    TRAPD( err,
           op = CPEngNWSessionOpenOp::NewL( iActivePriority,
                                            *iNWSessionProxy,
                                            iInterface,
                                            aSAP,
                                            aObserver ) );

    if ( err == KErrNone )
        {
        op->SetOwner( *this );
        op->IssueNWSessionOpen();
        iSessionOpenOp = op;
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::CancelOpenNWPresenceSession()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlot2Imp::CancelOpenNWPresenceSession()
    {
    //Deletes also the operation
    if ( iSessionOpenOp )
        {
        iSessionOpenOp->CancelOpD();
        iSessionOpenOp = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::IsCloseNWPresenceSessionActive()
// -----------------------------------------------------------------------------
//
TBool CPEngNWSessionSlot2Imp::IsCloseNWPresenceSessionActive() const
    {
    return( iSessionCloseOp  != NULL );     // CSI: 64 #
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::CloseNWPresenceSession()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::CloseNWPresenceSession(
    MPEngNWSessionOperationObserver2& aObserver,
    TBool aForceClose )
    {
    if ( iSessionOpenOp || iSessionCloseOp )
        {
        return KErrInUse;
        }

    CPEngNWSessionCloseOp* op = NULL;
    TRAPD( err,
           op = CPEngNWSessionCloseOp::NewL( iActivePriority,
                                             *iNWSessionProxy,
                                             iInterface,
                                             aObserver,
                                             aForceClose ) );

    if ( err == KErrNone )
        {
        op->SetOwner( *this );
        op->IssueNWSessionClose();
        iSessionCloseOp = op;
        }

    return err;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::CancelCloseNWPresenceSession()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlot2Imp::CancelCloseNWPresenceSession()
    {
    //Deletes also the operation
    if ( iSessionCloseOp )
        {
        iSessionCloseOp->CancelOpD();
        iSessionCloseOp = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::OpenNWPresenceSessionOwnership()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::OpenNWPresenceSessionOwnership()
    {
    return iNWSessionProxy->OpenOwnership();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::CloseNWPresenceSessionOwnership()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlot2Imp::CloseNWPresenceSessionOwnership()
    {
    iNWSessionProxy->Close();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::StoreNWPresenceSessionOwnership()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::StoreNWPresenceSessionOwnership(
    const TDesC16& aOwnerID )
    {
    return iNWSessionProxy->StoreOwnership( aOwnerID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::RestoreNWPresenceSessionOwnership()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::RestoreNWPresenceSessionOwnership(
    const TDesC16& aOwnerID )
    {
    return iNWSessionProxy->RestoreOwnership( aOwnerID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::GetNWPresenceSessionSap()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::GetNWPresenceSessionSap( CIMPSSAPSettings& aSAP ) const
    {
    TRAPD( err, DoGetNWPresenceSessionSapL( aSAP ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::GetNWSessionSlotState()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionSlot2Imp::GetNWSessionSlotState(
    TPEngNWSessionSlotState& aState ) const
    {
    TRAPD( err,
        {
        CPEngSessionSlotState* state = CPEngSessionSlotState::NewLC();
        PEngStorageManager::SessionSlotStateL( iUsedSlot->BaseId(),
                                               *state );

        aState = state->AppState( iUsedSlot->AppId() );
        CleanupStack::PopAndDestroy( state );
        } );


    return err;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::HandleNWSessionOperationDestruction()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlot2Imp::HandleNWSessionOperationDestruction(
    CPEngNWSessionOperation* aOperation )
    {
    if ( aOperation == iSessionOpenOp )
        {
        iSessionOpenOp = NULL;
        return;
        }

    if ( aOperation == iSessionCloseOp )
        {
        iSessionCloseOp = NULL;
        return;
        }

    __ASSERT_DEBUG( EFalse, PEngNWSessionSlotPanic( ENWSessionSlotUnkownOp ) );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2Imp::DoGetNWPresenceSessionSapL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlot2Imp::DoGetNWPresenceSessionSapL( CIMPSSAPSettings& aSAP ) const
    {
    HBufC8* loginData = NULL;
    User::LeaveIfError( iNWSessionProxy->GetLoginData( loginData ) );
    __ASSERT_ALWAYS( loginData, User::Leave( KErrGeneral ) );


    CleanupStack::PushL( loginData );
    PEngLoginDataCodec::UnPackLoginDataL( *loginData, aSAP );
    CleanupStack::PopAndDestroy( loginData );
    }



// End of File


