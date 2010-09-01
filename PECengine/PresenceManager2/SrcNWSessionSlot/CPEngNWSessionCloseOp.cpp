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
* Description:  Operation handler for closing the network session.
*
*/

// INCLUDE FILES
#include "CPEngNWSessionCloseOp.h"
#include "CPEngNWSessionProxy.h"
#include "PresenceDebugPrint.h"
#include <PEngPresenceEngineConsts2.h>
#include <CPEngNWSessionSlotID2.h>



//LOCAL constants
#ifdef _DEBUG
namespace
    {
    //Panic
    _LIT( KPEngNWSessionCloseOpPanic, "PEngNWCloseOp" );

    //Panic reasons
    enum TPEngNWSessionCloseOpPanicReasons
        {
        ENWSessionCloseOpInUse
        };

    void PEngNWSessionCloseOpPanic( TPEngNWSessionCloseOpPanicReasons aPanicReason )
        {
        User::Panic( KPEngNWSessionCloseOpPanic, aPanicReason );
        }
    }
#endif



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEngNWSessionCloseOp::NewL()
// -----------------------------------------------------------------------------
CPEngNWSessionCloseOp* CPEngNWSessionCloseOp::NewL( TInt aPriority,
                                                    CPEngNWSessionProxy& aNWSessionProxy,
                                                    CPEngNWSessionSlot2& aInterface,
                                                    MPEngNWSessionOperationObserver2& aObserver,
                                                    TBool aForceClose )
    {
    CPEngNWSessionCloseOp* self = new ( ELeave ) CPEngNWSessionCloseOp( aPriority,
                                                                        aNWSessionProxy,
                                                                        aInterface,
                                                                        aObserver,
                                                                        aForceClose  );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngNWSessionCloseOp::~CPEngNWSessionCloseOp()
// -----------------------------------------------------------------------------
CPEngNWSessionCloseOp::~CPEngNWSessionCloseOp()
    {
    CActive::Cancel();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionCloseOp::CPEngNWSessionCloseOp()
// -----------------------------------------------------------------------------
CPEngNWSessionCloseOp::CPEngNWSessionCloseOp( TInt aPriority,
                                              CPEngNWSessionProxy& aNWSessionProxy,
                                              CPEngNWSessionSlot2& aInterface,
                                              MPEngNWSessionOperationObserver2& aObserver,
                                              TBool aForceClose )
        : CPEngNWSessionOperation( aPriority, aNWSessionProxy, aInterface, aObserver ),
        iForceClose( aForceClose )
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionCloseOp::ConstructL()
// -----------------------------------------------------------------------------
void CPEngNWSessionCloseOp::ConstructL()
    {
    //Let the base to do its construction stuff
    CPEngNWSessionCloseOp::BaseConstructL();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionCloseOp::IssueNWSessionClose()
// -----------------------------------------------------------------------------
void CPEngNWSessionCloseOp::IssueNWSessionClose()
    {
    __ASSERT_DEBUG( !IsActive(),
                    PEngNWSessionCloseOpPanic( ENWSessionCloseOpInUse ) );

    TInt err = KErrNone;
    if ( iForceClose )
        {
        err = iNWSessionProxy.ForceCloseSession( iStatus );
        }
    else
        {
        err = iNWSessionProxy.CloseSession( iStatus );
        }

    HandleOperationIssued( err, EPEngNWSSOpNWSessionClose );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionCloseOp::DoCancel()
// -----------------------------------------------------------------------------
void CPEngNWSessionCloseOp::DoCancel()
    {
    if ( iForceClose )
        {
        iNWSessionProxy.CancelForceCloseSession();
        }
    else
        {
        iNWSessionProxy.CancelCloseSession();
        }

    NotifyObserverFromCancel();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionCloseOp::DoHandleOpCompleted()
// -----------------------------------------------------------------------------
void CPEngNWSessionCloseOp::DoHandleOpCompleted( TInt aError )
    {
    PENG_IF_DP(
        {
        if ( iForceClose )
            {
            PENG_DP( D_PENG_LIT( "CPEngNWSessionSlot2::ForceCloseNWPresenceSession() - completed [%d]" ), aError );
            }
        else
            {
            PENG_DP( D_PENG_LIT( "CPEngNWSessionSlot2::CloseNWPresenceSession() - completed [%d]" ), aError );
            }
        } );


    //Suppress unreferenced parameter warning
    ( void ) aError;
    }


// End of file

