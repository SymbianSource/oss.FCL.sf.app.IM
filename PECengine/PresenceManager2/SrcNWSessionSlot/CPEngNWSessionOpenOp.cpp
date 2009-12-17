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
* Description:  Operation handler for opening the network session.
*
*/

// INCLUDE FILES
#include "CPEngNWSessionOpenOp.h"
#include "PEngLoginDataCodec.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngNWSessionProxy.h"
#include "PresenceDebugPrint.h"
#include <CoreApplicationUIsSDKCRKeys.h>
#include <centralrepository.h>
#include <PEngPresenceEngineConsts2.h>
#include <CPEngNWSessionSlotID2.h>



//LOCAL constants
#ifdef _DEBUG
namespace
    {
    //Panic
    _LIT( KPEngNWSessionOpenOpPanic, "PEngNWOpenOp" );

    //Panic reasons
    enum TPEngNWSessionOpenOpPanicReasons
        {
        ENWSessionOpenOpInUse = 1,
        };

    void PEngNWSessionOpenOpPanic( TPEngNWSessionOpenOpPanicReasons aPanicReason )
        {
        User::Panic( KPEngNWSessionOpenOpPanic, aPanicReason );
        }
    }
#endif


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEngNWSessionOpenOp::NewL()
// -----------------------------------------------------------------------------
CPEngNWSessionOpenOp* CPEngNWSessionOpenOp::NewL( TInt aPriority,
                                                  CPEngNWSessionProxy& aNWSessionProxy,
                                                  CPEngNWSessionSlot2& aInterface,
                                                  const CIMPSSAPSettings& aImpsSap,
                                                  MPEngNWSessionOperationObserver2& aObserver )
    {
    CPEngNWSessionOpenOp* self = new ( ELeave ) CPEngNWSessionOpenOp( aPriority,
                                                                      aNWSessionProxy,
                                                                      aInterface,
                                                                      aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aImpsSap );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionOpenOp::~CPEngNWSessionOpenOp()
// -----------------------------------------------------------------------------
CPEngNWSessionOpenOp::~CPEngNWSessionOpenOp()
    {
    CActive::Cancel();
    delete iLoginData;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionOpenOp::CPEngNWSessionOpenOp()
// -----------------------------------------------------------------------------
CPEngNWSessionOpenOp::CPEngNWSessionOpenOp( TInt aPriority,
                                            CPEngNWSessionProxy& aNWSessionProxy,
                                            CPEngNWSessionSlot2& aInterface,
                                            MPEngNWSessionOperationObserver2& aObserver )
        : CPEngNWSessionOperation( aPriority, aNWSessionProxy, aInterface, aObserver )
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionOpenOp::ConstructL()
// -----------------------------------------------------------------------------
void CPEngNWSessionOpenOp::ConstructL( const CIMPSSAPSettings& aImpsSap )
    {
    //Is network access allowed?
    CheckNetworkAccessAllowedL();

    //Let the base to do its construction stuff
    CPEngNWSessionOpenOp::BaseConstructL();
    iLoginData = PEngLoginDataCodec::PackLoginDataL( aImpsSap );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionOpenOp::IssueNWSessionOpen()
// -----------------------------------------------------------------------------
void CPEngNWSessionOpenOp::IssueNWSessionOpen()
    {
    __ASSERT_DEBUG( !IsActive(),
                    PEngNWSessionOpenOpPanic( ENWSessionOpenOpInUse ) );

    TInt err = iNWSessionProxy.OpenSession( *iLoginData, iStatus );
    HandleOperationIssued( err, EPEngNWSSOpNWSessionOpen );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionOpenOp::DoCancel()
// -----------------------------------------------------------------------------
void CPEngNWSessionOpenOp::DoCancel()
    {
    iNWSessionProxy.CancelOpenSession();
    NotifyObserverFromCancel();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionOpenOp::DoHandleOpCompleted()
// -----------------------------------------------------------------------------
void CPEngNWSessionOpenOp::DoHandleOpCompleted( TInt aError )
    {
    PENG_DP( D_PENG_LIT( "CPEngNWSessionSlot2::OpenNWPresenceSession() - completed [%d]" ), aError );

    //Suppress unreferenced parameter warning
    ( void ) aError;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionOpenOp::CheckNetworkAccessAllowedL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionOpenOp::CheckNetworkAccessAllowedL()
    {
    TBool connectionAllowed = ETrue;

    //Dig out the real status
    CRepository* cenRep = CRepository::NewLC( KCRUidCoreApplicationUIs );
    TInt status = ECoreAppUIsNetworkConnectionAllowed;
    cenRep->Get( KCoreAppUIsNetworkConnectionAllowed, status );
    if ( status == ECoreAppUIsNetworkConnectionNotAllowed )
        {
        connectionAllowed = EFalse;
        }
    CleanupStack::PopAndDestroy(); //cenRep

    //Handle the case
    if ( !connectionAllowed )
        {
        User::Leave( KErrAccessDenied );
        }
    }



// End of file


